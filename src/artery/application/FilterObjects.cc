//
// Created by rosk on 03.02.19.
//
#include "FilterObjects.h"

#include "artery/application/CpObject.h"
#include "artery/application/Asn1PacketVisitor.h"
#include "artery/application/VehicleDataProvider.h"
#include "artery/utility/simtime_cast.h"
#include "veins/base/utils/Coord.h"

#include "artery/application/Configurations.h"
#include "artery/envmod/sensor/Sensor.h"

#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <omnetpp/cexception.h>
#include <vanetza/btp/ports.hpp>
#include <vanetza/dcc/transmission.hpp>
#include <vanetza/facilities/cam_functions.hpp>
#include <vanetza/dcc/transmit_rate_control.hpp>
#include <chrono>
#include <iostream>

/** Template to do a filter:
 * Create a function that return False if the object should not be sent
 * True otherwise
 * Add filter in the filterObjects function
 *
 * In CPService initialize(), add an entry for the filter added
 */

namespace artery
{


FilterObjects::FilterObjects(){}

FilterObjects::FilterObjects(const VehicleDataProvider* vd, LocalEnvironmentModel* le,
                             vanetza::units::Angle hd, vanetza::units::Length pd, vanetza::units::Velocity sd,
                             std::map<const Sensor*, Identifier_t>* sensorsId, const omnetpp::SimTime& T_GenCpmMin,
                            const SimTime& T_GenCpmMax):
        mVehicleDataProvider(vd), mLocalEnvironmentModel(le), 
        mHeadingDelta(hd), mPositionDelta(pd), mSpeedDelta(sd), mSensorsId(sensorsId), mGenCpmMin(T_GenCpmMin),
        mGenCpmMax(T_GenCpmMax)
{
}


template<typename T, typename U>
long round(const boost::units::quantity<T>& q, const U& u)
{
    boost::units::quantity<U> v { q };
    return std::round(v.value());
}


void FilterObjects::initialize(const VehicleDataProvider* vd, LocalEnvironmentModel* le,
                                 vanetza::units::Angle hd, vanetza::units::Length pd, vanetza::units::Velocity sd,
                                 std::map<const Sensor*, Identifier_t>* sensorsId, const omnetpp::SimTime& T_GenCpmMin,
                                 const omnetpp::SimTime& T_GenCpmMax)
{
    mVehicleDataProvider = vd;
    mLocalEnvironmentModel= le;
    mHeadingDelta = hd;
    mPositionDelta = pd;
    mSpeedDelta = sd;
    mSensorsId = sensorsId;
    mGenCpmMin = T_GenCpmMin;
    mGenCpmMax = T_GenCpmMax;
    mTimeDelta = omnetpp::SimTime(1, SIMTIME_S);
}


bool FilterObjects::checkHeadingDelta(vanetza::units::Angle prevHeading, vanetza::units::Angle headingNow) const
{
 //   std::cout << "Heading: " << std::endl;
 //   std::cout << round(prevHeading, vanetza::units::degree) << "  " << round(headingNow, vanetza::units::degree) << std::endl;

    return !vanetza::facilities::similar_heading(prevHeading, headingNow, mHeadingDelta);
}


bool FilterObjects::checkPositionDelta(Position prevPos, Position posNow) const
{
    /*std::cout << "Position: " << std::endl;
    std::cout <<  prevPos.x /  boost::units::si::meter
              << ", " << prevPos.y /  boost::units::si::meter << std::endl;
    std::cout <<  posNow.x /  boost::units::si::meter
              << ", " << posNow.y /  boost::units::si::meter << std::endl;
    std::cout << distance(prevPos, posNow) / boost::units::si::meter << std::endl;
    */
    return (distance(prevPos, posNow) > mPositionDelta);
}


bool FilterObjects::checkSpeedDelta(vanetza::units::Velocity prevVel,  vanetza::units::Velocity velNow) const
{
    /*
    std::cout << "Speed: " << std::endl;

    std::cout << prevVel / vanetza::units::si::meter_per_second << std::endl;
    std::cout << velNow / vanetza::units::si::meter_per_second << std::endl;
    std::cout << abs(prevVel - velNow) / vanetza::units::si::meter_per_second << std::endl;
    */
    return abs(prevVel - velNow) > mSpeedDelta;
}


bool FilterObjects::checkTimeDelta(omnetpp::SimTime T_prev, omnetpp::SimTime T_now) const
{
    /*std::cout << "New check time " << std::endl;
    std::cout << T_prev << std::endl;
    std::cout << mTimeDelta << std::endl;
    std::cout << T_now << std::endl;
    std::cout << T_prev + mTimeDelta << std::endl;
    */
    return T_prev + mTimeDelta < T_now;
}


void FilterObjects::changeDeltas(vanetza::units::Angle hd, vanetza::units::Length pd, vanetza::units::Velocity sd){
    mHeadingDelta = hd;
    mPositionDelta = pd;
    mSpeedDelta = sd;
}


ObjectInfo::ObjectsPercievedMap FilterObjects::getallPercievedObjs(){

    ObjectInfo::ObjectsPercievedMap prcvd_objs;
    //get all the tracked objects from the local environmental model
    const LocalEnvironmentModel::TrackedObjects& allobjs = mLocalEnvironmentModel->allObjects();

    //get objects identified with sensors (Radar, Lidar and cameras) and add them to ObjectInfo::ObjectsPercievedMap
    const TrackedObjectsFilterRange& radarobjs = artery::filterBySensorCategory(allobjs, "Radar");
 
    for(const auto& env_robj : radarobjs){

        const LocalEnvironmentModel::Tracking::TrackingMap& prcvd_snsrs =  allobjs.at(env_robj.first).sensors(); //trckd_robj->second.sensors();

        //If the object has been detected by its local perception capabilities (i.e. radar), add obj. to the list to send
        for(const auto& p_snsr : prcvd_snsrs){
            if(p_snsr.first->getSensorCategory() == "Radar"){

                //If object not already in the lists or if the current sensor has "more" updated information
                if(prcvd_objs.find(env_robj.first) == prcvd_objs.end() || prcvd_objs.at(env_robj.first).getLastTrackingTime().last() < p_snsr.second.last()){
                    const auto& vd = env_robj.first.lock()->getVehicleData();
                    prcvd_objs[env_robj.first] = ObjectInfo(p_snsr.second, mSensorsId->at(p_snsr.first), vd.heading(), vd.position(),  vd.speed());
                }//Both sensors checked the object at the same time (@todo: to remove?)
                else if (prcvd_objs.at(env_robj.first).getLastTrackingTime().last() == p_snsr.second.last()){
                    prcvd_objs.at(env_robj.first).setNumberOfSensors(prcvd_objs.at(env_robj.first).getNumberOfSensors() + 1);
                }
            }
        }
    }

    return prcvd_objs;
}


bool FilterObjects::checkobjectDynamics(const ObjectInfo::ObjectPercieved& obj, ObjectInfo::ObjectsPercievedMap& trckedobjs, omnetpp::SimTime T_now){


    //If the position of the object since last time it has been sent, refuse it
    if(trckedobjs.find(obj.first) != trckedobjs.end()) {

        const VehicleDataProvider &vd = obj.first.lock()->getVehicleData();
        ObjectInfo &infoObject = trckedobjs.at(obj.first);

        //Object need to be send at least every one second
        if(T_now - infoObject.getLastTimeSent() >= omnetpp::SimTime(1, SIMTIME_S))
            return true;


        if (!(checkHeadingDelta(infoObject.getLastHeading(), vd.heading()) ||
              checkPositionDelta(infoObject.getLastPosition(), vd.position()) ||
              checkSpeedDelta(infoObject.getLastVelocity(), vd.speed()))){
            //std::cout << "\nObject dynamic local: Filter object "<< vd.station_id() << std::endl;
            return false;
        }
    }
    return true;
}

bool FilterObjects::checkobjectDynamics(const LocalEnvironmentModel::TrackedObject& obj,
                                   ObjectInfo::ObjectsPercievedMap& prevObjSent, omnetpp::SimTime T_now){

    //If the position of the object since last time it has been sent, refuse it
    if(prevObjSent.find(obj.first) != prevObjSent.end()) {

        const VehicleDataProvider &vd = obj.first.lock()->getVehicleData();
        ObjectInfo &infoObject = prevObjSent.at(obj.first);

        //Object need to be send at least every one second
        if(T_now - infoObject.getLastTimeSent() >= omnetpp::SimTime(1, SIMTIME_S))
            return true;


        if (!(checkHeadingDelta(infoObject.getLastHeading(), vd.heading()) ||
              checkPositionDelta(infoObject.getLastPosition(), vd.position()) ||
              checkSpeedDelta(infoObject.getLastVelocity(), vd.speed()))){
            //std::cout << "\nObject dynamic local: Filter object "<< vd.station_id() << std::endl;
            return false;
        }
    }
    return true;
}

} // namespace artery
