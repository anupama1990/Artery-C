/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/application/Middleware.h"
#include "artery/envmod/EnvironmentModelObject.h"
#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/envmod/GlobalEnvironmentModel.h"
#include "artery/envmod/sensor/Sensor.h"
#include "artery/utility/FilterRules.h"
#include <inet/common/ModuleAccess.h>
#include <omnetpp/cxmlelement.h>
#include <utility>
#include <artery/envmod/sensor/CamSensor.h>

using namespace omnetpp;

namespace artery
{

Define_Module(LocalEnvironmentModel);

static const simsignal_t EnvironmentModelRefreshSignal = cComponent::registerSignal("EnvironmentModel.refresh");
static const simsignal_t scTimeBetweenUpdates = cComponent::registerSignal("timeBetweenUpdates");


LocalEnvironmentModel::LocalEnvironmentModel() :
    mGlobalEnvironmentModel(nullptr)
{
}

int LocalEnvironmentModel::numInitStages() const
{
    return 2;
}

void LocalEnvironmentModel::initialize(int stage)
{
    if (stage == 0) {
        mGlobalEnvironmentModel = inet::getModuleFromPar<GlobalEnvironmentModel>(par("globalEnvironmentModule"), this);
        mGlobalEnvironmentModel->subscribe(EnvironmentModelRefreshSignal, this);

        auto vehicle = inet::findContainingNode(this);
        mMiddleware = inet::getModuleFromPar<Middleware>(par("middlewareModule"), vehicle);
        Facilities& fac = mMiddleware->getFacilities();
        fac.register_mutable(mGlobalEnvironmentModel);
        fac.register_mutable(this);
    } else if (stage == 1) {
        initializeSensors();
    }
}

void LocalEnvironmentModel::finish()
{
    mGlobalEnvironmentModel->unsubscribe(EnvironmentModelRefreshSignal, this);
    mObjects.clear();
}

void LocalEnvironmentModel::receiveSignal(cComponent*, simsignal_t signal, cObject* obj, cObject*)
{
    if (signal == EnvironmentModelRefreshSignal) {
        for (auto* sensor : mSensors) {
            sensor->measurement();
        }
        update();
    }
}

void LocalEnvironmentModel::complementObjects(const SensorDetection& detection, const Sensor& sensor)
{
    auto it = detection.numberOfCornersDetected.begin();
    for (auto& detectedObject : detection.objects) {
        auto foundObject = mObjects.find(detectedObject);
        if (foundObject != mObjects.end()) {
            Tracking& tracking = foundObject->second;
            if(sensor.getSensorCategory() == "CA" || sensor.getSensorCategory() == "CP"){
                SimTime lastUpdate = tracking.getTimeSinceLastUpdate();
                //TODO remove the second condition
                if(!lastUpdate.isZero()) // && lastUpdate < omnetpp::SimTime(100, SIMTIME_MS)
                    emit(scTimeBetweenUpdates, lastUpdate);
            }
            tracking.updateQuality(&sensor, *it);
            tracking.tap(&sensor);
        } else {
            mObjects.emplace(detectedObject, Tracking { &sensor, *it});
        }
        it++;
    }
}

void LocalEnvironmentModel::update()
{
    for (auto it = mObjects.begin(); it != mObjects.end();) {
        const Object& object = it->first;
        Tracking& tracking = it->second;
        tracking.update();

        if (object.expired() || tracking.expired()) {
            it = mObjects.erase(it);
        } else {
            ++it;
        }
    }
}

void LocalEnvironmentModel::initializeSensors()
{
    cXMLElement* config = par("sensors").xmlValue();
    for (cXMLElement* sensor_cfg : config->getChildrenByTagName("sensor"))
    {
        cXMLElement* sensor_filters = sensor_cfg->getFirstChildWithTag("filters");
        bool sensor_applicable = true;
        if (sensor_filters) {
            auto identity = mMiddleware->getIdentity();
            FilterRules rules(getRNG(0), identity);
            sensor_applicable = rules.applyFilterConfig(*sensor_filters);
        }

        if (sensor_applicable) {
            cModuleType* module_type = cModuleType::get(sensor_cfg->getAttribute("type"));
            const char* sensor_name = sensor_cfg->getAttribute("name");
            if (!sensor_name || !*sensor_name) {
                sensor_name = module_type->getName();
            }

            cModule* module = module_type->createScheduleInit(sensor_name, this);
            auto sensor = dynamic_cast<artery::Sensor*>(module);

            if (sensor != nullptr) {
                cXMLElement* vis_cfg = sensor_cfg->getFirstChildWithTag("visualization");
                sensor->setVisualization(SensorVisualizationConfig(vis_cfg));
            } else {
                throw cRuntimeError("%s is not of type Sensor", module_type->getFullName());
            }

            mSensors.push_back(sensor);
        }
    }
}


LocalEnvironmentModel::Tracking::Tracking(const Sensor* sensor)
{
    mSensors.emplace(sensor, TrackingTime {});
}

LocalEnvironmentModel::Tracking::Tracking(const Sensor* sensor, const int nbVisiblePoints){
    mSensors.emplace(sensor, TrackingTime {});
    mQuality.emplace(sensor, nbVisiblePoints);
}

bool LocalEnvironmentModel::Tracking::expired() const
{
    return mSensors.empty();
}

void LocalEnvironmentModel::Tracking::update()
{
    for (auto it = mSensors.begin(); it != mSensors.end();) {
      const Sensor* sensor = it->first;
      const TrackingTime& tracking = it->second;

      const bool expired = tracking.last() + sensor->getValidityPeriod() < simTime();
      if (expired) {
          it = mSensors.erase(it);
      } else {
          ++it;
      }
    }
}

void LocalEnvironmentModel::Tracking::tap(const Sensor* sensor)
{
    auto found = mSensors.find(sensor);
    if (found != mSensors.end()) {
         TrackingTime& tracking = found->second;
         tracking.tap();
    } else {
         mSensors.emplace(sensor, TrackingTime {});
    }
}

void LocalEnvironmentModel::Tracking::updateQuality(const Sensor* sensor, int nbCornersDetected){
    auto found = mQuality.find(sensor);
    if (found != mQuality.end()) {
        found->second = nbCornersDetected;
    } else {
        mQuality.emplace(sensor, nbCornersDetected);
    }
}

SimTime LocalEnvironmentModel::Tracking::getTimeSinceLastUpdate(){
    SimTime timeElapsedMin = 0;

    for(auto it = this->sensors().begin(); it != this->sensors().end(); it++) {
        const Sensor* sensor = it->first;
        const TrackingTime& tracking = it->second;
        if(sensor->getSensorCategory() == "CA" || sensor->getSensorCategory() == "CP"){
            SimTime timeElapsed = simTime() - tracking.last();
            if(timeElapsedMin.isZero() || timeElapsed < timeElapsedMin){
                timeElapsedMin = timeElapsed;
            }
        }
    }
    return timeElapsedMin;
}


LocalEnvironmentModel::TrackingTime::TrackingTime() :
   mFirst(simTime()), mLast(simTime())
{
}


LocalEnvironmentModel::TrackingTime::TrackingTime(omnetpp::SimTime time) :
        mFirst(time), mLast(time)
{}


void LocalEnvironmentModel::TrackingTime::setLast(omnetpp::SimTime time)
{
    mLast = time;
}


void LocalEnvironmentModel::TrackingTime::tap()
{
    mLast = simTime();
}


TrackedObjectsFilterRange filterBySensorCategory(const LocalEnvironmentModel::TrackedObjects& all, const std::string& category)
{
    // capture `category` by value because lambda expression will be evaluated after this function's return
    TrackedObjectsFilterPredicate seenByCategory = [category](const LocalEnvironmentModel::TrackedObject& obj) {
        const auto& detections = obj.second.sensors();
        return std::any_of(detections.begin(), detections.end(),
                [&category](const LocalEnvironmentModel::Tracking::TrackingMap::value_type& tracking) {
                    const Sensor* sensor = tracking.first;
                    return sensor->getSensorCategory() == category;
                });
    };

    auto begin = boost::make_filter_iterator(seenByCategory, all.begin(), all.end());
    auto end = boost::make_filter_iterator(seenByCategory, all.end(), all.end());
    return boost::make_iterator_range(begin, end);
}

} // namespace artery
