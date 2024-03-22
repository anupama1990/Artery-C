//
// Created by rosk on 13.02.19.
//

#include "ObjectInfo.h"

namespace artery
{

std::atomic<uint8_t> ObjectInfo::unique_id;

template<typename T, typename U>
long round(const boost::units::quantity<T>& q, const U& u)
{
    boost::units::quantity<U> v { q };
    return std::round(v.value());
}

ObjectInfo::ObjectInfo(){
    if(unique_id == sizeof(uint8_t)-1){
        unique_id=0;
    }
    mObjid = ++unique_id;
}

ObjectInfo::ObjectInfo(LocalEnvironmentModel::TrackingTime lastTrackingTime, Identifier_t& id,
        vanetza::units::Angle lastCpmHeading, Position lastCpmPosition,
        vanetza::units::Velocity lastCpmSpeed):
        mLastTrackingTime(lastTrackingTime), mNumberOfSensors(1), mSensorsId(id),
        mLastCpmHeading(lastCpmHeading), mLastCpmPosition(lastCpmPosition),
        mLastCpmSpeed(lastCpmSpeed)
{
    if(unique_id == sizeof(uint8_t)-1){
        unique_id=0;
    }
    mObjid = ++unique_id;
}

std::ostream& operator<<(std::ostream& os, ObjectInfo& infoObj){
    os << "Info of the object: " << std::endl;
    os << "\tLast Tracked time: " << infoObj.getLastTrackingTime().last() << std::endl;
    os << "\tNumber of sensors in the detection: " << infoObj.getNumberOfSensors() << std::endl;
    os << "\tSensor used in the detection: " << infoObj.getSensorId() << std::endl;
    boost::units::quantity<boost::units::degree::plane_angle> heading { infoObj.getLastHeading() };
    os << "\tLast heading perceived: " << heading.value() << std::endl;
    os << "\tLast position perceived: (" << infoObj.getLastPosition().x / boost::units::si::meter
       << ", " << infoObj.getLastPosition().y /  boost::units::si::meter << ")" << std::endl;
    os << "\tLast velocity perceived: " << infoObj.getLastVelocity() / vanetza::units::si::meter_per_second << std::endl;

    return os;
}

void ObjectInfo::printObjectsReceivedMap(ObjectsReceivedMap objReceived){
    std::cout << "Number of objects: " << objReceived.size() << std::endl;
    for(auto& mapObj : objReceived){
        std::cout << "id Object: " << mapObj.first << std::endl;
        std::cout << mapObj.second << std::endl;
    }
}

void ObjectInfo::printObjectsToSendMap(ObjectsPercievedMap objMap){
    std::cout << "Number of objects: " << objMap.size() << std::endl;
    for(auto& mapObj : objMap){
        std::cout << "id Object: " << mapObj.first.lock()->getVehicleData().station_id() << std::endl;
        std::cout << mapObj.second << std::endl;
    }
}



} // end namespace artery