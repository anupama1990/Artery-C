//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __ARTERY_RADIODRIVER_H_
#define __ARTERY_RADIODRIVER_H_

#include <omnetpp.h>

#include <artery/networking/GeoNetIndication.h>
#include "artery/networking/GeoNetRequest.h"
#include "artery/nic/RadioDriverProperties.h"
#include "artery/nic/ChannelLoadMeasurements.h"
#include "artery/nic/RadioDriverBase.h"
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include "common/binder/Binder.h"
#include "traci/Listener.h"
#include "traci/Core.h"
#include "traci/LiteAPI.h"
#include "traci/Position.h"
#include <inet/mobility/contract/IMobility.h>
#include <inet/common/ModuleAccess.h>
#include <chrono>
#include <iostream>
#include "traci/Boundary.h"
#include "traci/sumo/utils/traci/TraCIAPI.h"
#include "traci/sumo/libsumo/TraCIDefs.h"

//#include "PoiRetrievalModule.h"
using namespace omnetpp;
using libsumo::TraCIPosition;

namespace artery
{

class LiteAPI;

class RadioDriver : public RadioDriverBase,  public traci::Listener
{
    /*
    public:
        void initialize() override;
        void handleMessage(omnetpp::cMessage*) override;
        void finish() override;
        std::vector<TraCIPosition> getStationaryModulePosition();
    protected:
        void handleDataIndication(omnetpp::cMessage*);
        void handleDataRequest(omnetpp::cMessage*) override;
        void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, bool, omnetpp::cObject*) override;
        int CAMId;
        inet::Coord enbCoord;
        std::vector<TraCIPosition> enbPosTraci;
        std::vector<TraCIPosition> enbPosOmnet;
        PoiRetrievalModule* s;
        cModule *hostModule;
        simsignal_t numberCAMs;
        simsignal_t CAMIdSent;
        unsigned int CAMSGenerated;
    private:
        omnetpp::cModule* mHost = nullptr;
        omnetpp::cGate* mLowerLayerOut = nullptr;
        omnetpp::cGate* mLowerLayerIn = nullptr;
        ChannelLoadMeasurements mChannelLoadMeasurements;
        LiteAPI* m_api;
        LteBinder* binder_;
        MacNodeId nodeId_;
        MacNodeId masterId_;
        bool startUpComplete_;*/

};
}


#endif
