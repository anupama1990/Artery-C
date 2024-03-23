/*
* Collective perception service application
* Copyright 2022-2023 Anupama Hegde et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_CPSERVICE_H_
#define ARTERY_CPSERVICE_H_

#include "artery/application/ItsG5BaseService.h"
#include "artery/application/ObjectInfo.h"
#include "artery/application/FilterObjects.h"
#include "artery/utility/Channel.h"
#include "artery/utility/Geometry.h"
#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/envmod/sensor/Sensor.h"
#include <vanetza/asn1/cam.hpp>
#include <vanetza/asn1/cpm.hpp>
#include <vanetza/btp/data_interface.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/velocity.hpp>
#include <omnetpp/simtime.h>



namespace artery
{

class NetworkInterfaceTable;
class Timer;
class VehicleDataProvider;

class CpService : public ItsG5BaseService
{

	public:
		CpService();
		void initialize() override;
		void indicate(const vanetza::btp::DataIndication&, std::unique_ptr<vanetza::UpPacket>) override;
		void trigger() override;

	private:

		omnetpp::SimTime mGenCpmMin;
		omnetpp::SimTime mGenCpmMax;
		omnetpp::SimTime mGenCpm;
		omnetpp::SimTime mLastCpmTimestamp;
		omnetpp::SimTime mPerSecCpmStatTimestamp;
		omnetpp::SimTime mLastSenrInfoCntnrTimestamp;
		unsigned mGenCpmLowDynamicsCounter;
		unsigned mGenCpmLowDynamicsLimit;
		long mNumCpmPerSecCounter;

		Position mLastCpmPosition;
		vanetza::units::Velocity mLastCpmSpeed;
		vanetza::units::Angle mLastCpmHeading;
		vanetza::units::Angle mHeadingDelta;
		vanetza::units::Length mPositionDelta;
		vanetza::units::Velocity mSpeedDelta;
		bool mFixedRate;
		omnetpp::SimTime mFixedRateInterval;


		ChannelNumber mPrimaryChannel = channel::CCH;
		const NetworkInterfaceTable* mNetworkInterfaceTable = nullptr;
		const VehicleDataProvider* mVehicleDataProvider = nullptr;
		const Timer* mTimer = nullptr;
		LocalDynamicMap* mLocalDynamicMap = nullptr;
		LocalEnvironmentModel* mLocalEnvironmentModel=nullptr;
		std::map<const Sensor*, Identifier_t> mSensorsId;

		ObjectInfo::ObjectsPercievedMap mObjectsToSend;
		ObjectInfo::ObjectsPercievedMap mObjectsTracked;
		ObjectInfo::ObjectsReceivedMap mObjectsReceived;
		ObjectInfo::ObjectsReceivedMap mObjsRelevanceArea;
		Sensor* mCPSensor = nullptr;
        Sensor* mCASensor = nullptr;
		FilterObjects mFilterObj;

		void checkTriggeringConditions(const omnetpp::SimTime&);
		bool checkHeadingDelta() const;
		bool checkPositionDelta() const;
		bool checkSpeedDelta() const;
		omnetpp::SimTime genCpmDcc();
		void generateCPM(const omnetpp::SimTime&);
		void sendCpm(const omnetpp::SimTime&);
		bool generateSensorInfoCntnr(vanetza::asn1::Cpm&);
		bool generateStnAndMgmtCntnr(vanetza::asn1::Cpm&);
		void generateMgmtCntnr(vanetza::asn1::Cpm&);
		void generateCarStnCntnr(vanetza::asn1::Cpm&);
		void generateRSUStnCntnr(vanetza::asn1::Cpm&);
		void removeExpobjs();
		void recordObjectsAge();
		void retrieveCPMmessage(const vanetza::asn1::Cpm&);
		void generate_sensorid();
		void addsensorinfo(SensorInformationContainer_t *&seqSensInfCont, Sensor *&sensor, SensorType_t sensorType);
		PerceivedObject_t* createPerceivedObjectContainer(const std::weak_ptr<artery::EnvironmentModelObject>& object, ObjectInfo& infoObj);
        //void addPerceivedObjectContainer(LocalEnvironmentModel* localEnvironmentModel, vanetza::asn1::Cpm& message, const omnetpp::SimTime& T_now);
		void generateASN1Objects(vanetza::asn1::Cpm& message, const omnetpp::SimTime& T_now, ObjectInfo::ObjectsPercievedMap objToSend);
		void updateObjTrackedList(const omnetpp::SimTime& T_now, ObjectInfo::ObjectsPercievedMap objToSend);
		void checkCPMSize(const omnetpp::SimTime& T_now, ObjectInfo::ObjectsPercievedMap& objToSendNoFiltering, vanetza::asn1::Cpm& cpm);

		void updateObjlist(ObjectInfo::ObjectsReceivedMap& obj_list, uint32_t stationID, LocalEnvironmentModel::TrackingTime& newTrackingtime, 
							vanetza::units::Angle &newHeading, Position &newPosition, vanetza::units::Velocity &newSpeed, bool cal_tbu = false);

		bool generatePerceivedObjectsCntnr(vanetza::asn1::Cpm&, const omnetpp::SimTime& T_now);
		bool objinTrackedlist(const ObjectInfo::ObjectPercieved& obj);
		void printCPM(const vanetza::asn1::Cpm &message);

};

} // namespace artery

#endif /* ARTERY_CPSERVICE_H_ */
