//
// Created by rosk on 13.02.19.
//

#ifndef ARTERY_OBJECTINFO_H
#define ARTERY_OBJECTINFO_H

#include "artery/application/ItsG5BaseService.h"
#include "artery/utility/Geometry.h"
#include <vanetza/asn1/cpm.hpp>
#include <artery/envmod/LocalEnvironmentModel.h>
#include <artery/envmod/EnvironmentModelObject.h>
#include <vanetza/btp/data_interface.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/velocity.hpp>
#include <omnetpp/simtime.h>
#include "artery/application/Configurations.h"


namespace artery
{

class ObjectInfo
{
public:
    using ObjectsPercievedMap = std::map<const LocalEnvironmentModel::Object, ObjectInfo, std::owner_less<LocalEnvironmentModel::Object>>;
    using ObjectPercieved = typename ObjectsPercievedMap::value_type;
    using ObjectsReceivedMap = std::map<uint32_t, ObjectInfo>;

    ObjectInfo();
    ObjectInfo( LocalEnvironmentModel::TrackingTime, Identifier_t&,
                    vanetza::units::Angle, Position, vanetza::units::Velocity);

    //get functions
    LocalEnvironmentModel::TrackingTime& getLastTrackingTime() { return mLastTrackingTime; }
    size_t getNumberOfSensors() const { return mNumberOfSensors; }
    Identifier_t getSensorId() const { return mSensorsId; }
    vanetza::units::Angle getLastHeading(){return mLastCpmHeading;}
    Position getLastPosition(){return mLastCpmPosition;}
    vanetza::units::Velocity getLastVelocity(){return mLastCpmSpeed;}
    omnetpp::SimTime getLastTimeSent() { return mLastTimeSent; }
    uint8_t getobjectid(){return mObjid;}


    //set functions
    void setLastTrackingTime(LocalEnvironmentModel::TrackingTime lastTrackingTime) {mLastTrackingTime = lastTrackingTime;}
    void setNumberOfSensors(size_t numberOfSensors) {mNumberOfSensors = numberOfSensors;}
    void setSensorId(Identifier_t& id) {mSensorsId = id;}
    void setLastTimeSent(omnetpp::SimTime time) { mLastTimeSent = time;}
    void setLastHeading(vanetza::units::Angle newHeading){ mLastCpmHeading = newHeading;}
    void setLastPosition(Position newPosition){mLastCpmPosition = newPosition;}
    void setLastVelocity(vanetza::units::Velocity newSpeed){mLastCpmSpeed = newSpeed;}

    //print functions
    static void printObjectsReceivedMap(ObjectsReceivedMap objReceived);
    static void printObjectsToSendMap(ObjectsPercievedMap objMap);
    


private:
    static std::atomic<uint8_t> unique_id;
    uint8_t mObjid;
    LocalEnvironmentModel::TrackingTime mLastTrackingTime;
    size_t mNumberOfSensors;
    Identifier_t mSensorsId;
    vanetza::units::Angle mLastCpmHeading;
    Position mLastCpmPosition;
    vanetza::units::Velocity mLastCpmSpeed;
    omnetpp::SimTime mLastTimeSent;
};

std::ostream& operator<<(std::ostream& os, ObjectInfo& infoObj);

} //end namespace artery

#endif //ARTERY_OBJECTINFO_H
