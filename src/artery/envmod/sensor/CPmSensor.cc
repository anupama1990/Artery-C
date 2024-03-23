/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/envmod/sensor/CPmSensor.h"
#include "artery/envmod/GlobalEnvironmentModel.h"
#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/application/CpObject.h"
#include "artery/application/Middleware.h"
#include "artery/utility/IdentityRegistry.h"
#include <inet/common/ModuleAccess.h>
#include <vanetza/asn1/its/PerceivedObjectContainer.h>

using namespace omnetpp;

namespace artery
{

static const simsignal_t CPmReceivedSignal = cComponent::registerSignal("CpmReceived");

Define_Module(CPmSensor);

void CPmSensor::initialize()
{
    BaseSensor::initialize();
    mIdentityRegistry = inet::getModuleFromPar<IdentityRegistry>(par("identityRegistryModule"), this);
    getMiddleware().subscribe(CPmReceivedSignal, this);
}

void CPmSensor::finish()
{
    getMiddleware().unsubscribe(CPmReceivedSignal, this);
    BaseSensor::finish();
}

void CPmSensor::measurement()
{
    Enter_Method("measurement");
}

void CPmSensor::receiveSignal(cComponent*, simsignal_t signal, cObject *obj, cObject*)
{
    if (signal == CPmReceivedSignal) {
        auto* cpm = dynamic_cast<CpObject*>(obj);
        if (cpm) {

            SensorDetection detection;
            std::shared_ptr<EnvironmentModelObject> object;

            //Add id of CPM sender
            uint32_t stationID = cpm->asn1()->header.stationID;
            //std::cout << "Station ID Object received sensor: " << stationID << std::endl;

            auto identity = mIdentityRegistry->lookup<IdentityRegistry::application>(stationID);
            if (identity) {
                object = mGlobalEnvironmentModel->getObject(identity->traci);
                detection.objects.push_back(object);
            }
                //Add object perceived
            PerceivedObjectContainer_t* objectsContainer = cpm->asn1()->cpm.cpmParameters.perceivedObjectContainer;
            //if(objectsContainer != nullptr)
             //   std::cout << "Received " << objectsContainer->list.count << " object in CPM" << std::endl;

            for(int i = 0 ; objectsContainer != nullptr && i < objectsContainer->list.count ; i++){
                PerceivedObject_t* objCont = objectsContainer->list.array[i];
                identity = mIdentityRegistry->lookup<IdentityRegistry::application>(objCont->objectID);
                if (identity) {
                   // std::cout << "Object received sensor: " << objCont->objectID << std::endl;
                    object = mGlobalEnvironmentModel->getObject(identity->traci);
                    detection.objects.push_back(object);
                } else{
                    object = mGlobalEnvironmentModel->getObjectFromVehDB(objCont->objectID);
                    if(object)
                        detection.objects.push_back(object);
                    else
                        EV_DETAIL << "Unknown identity for station ID " << stationID;
                }
            }


            mLocalEnvironmentModel->complementObjects(detection, *this);

        } else {
            EV_ERROR << "received signal has no CPObject";
        }
    }
}

omnetpp::SimTime CPmSensor::getValidityPeriod() const
{
    return omnetpp::SimTime { 1100, SIMTIME_MS };
}

const std::string& CPmSensor::getSensorCategory() const
{
    static const std::string category = "CP";
    return category;
}

} // namespace artery
