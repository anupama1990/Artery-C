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


#include "RadioDriver.h"
#include <omnetpp/ccomponent.h>
#include <omnetpp/cexception.h>
#include <omnetpp/checkandcast.h>
#include <omnetpp/clog.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/cmodule.h>
#include <omnetpp/cnamedobject.h>
#include <omnetpp/cobject.h>
#include <omnetpp/cobjectfactory.h>
#include <omnetpp/csimulation.h>
#include <omnetpp/regmacros.h>
#include <omnetpp/simkerneldefs.h>
#include <omnetpp/simtime.h>
#include <array>
#include <cstring>

#include "../../../extern/simulte/src/common/LteCommon.h"
#include "../../../extern/simulte/src/common/LteControlInfo_m.h"
#include "../../../extern/simulte/src/stack/phy/packet/cbr_m.h"
#include "../../../extern/vanetza/vanetza/net/access_category.hpp"
#include "../../../extern/vanetza/vanetza/net/mac_address.hpp"
#include "../../../extern/veins/src/veins/base/utils/FindModule.h"
#include "../../../extern/simulte/src/stack/phy/layer/LtePhyBase.h"
#include "../utility/Channel.h"
#include <vanetza/geonet/data_request.hpp>
#include "apps/nonip/CAMPacket_m.h"

using namespace omnetpp;
using namespace vanetza;

namespace artery
{

Register_Class(RadioDriver);

namespace {

long convert(const vanetza::MacAddress& mac)
{
    long addr = 0;
    for (unsigned i = 0; i < mac.octets.size(); ++i) {
        addr <<= 8;
        addr |= mac.octets[i];
    }
    return addr;
}

vanetza::MacAddress convert(long addr)
{
    vanetza::MacAddress mac;
    for (unsigned i = mac.octets.size(); i > 0; --i) {
        mac.octets[i - 1] = addr & 0xff;
        addr >>= 8;
    }
    return mac;
}

int user_priority(vanetza::AccessCategory ac)
{
    using AC = vanetza::AccessCategory;
    int up = 0;
    switch (ac) {
    case AC::BK:
        up = 1;
        break;
    case AC::BE:
        up = 3;
        break;
    case AC::VI:
        up = 5;
        break;
    case AC::VO:
        up = 7;
        break;
    }
    return up;
}

const simsignal_t channelBusySignal = cComponent::registerSignal("sigChannelBusy");
}


void RadioDriver::initialize()
{
    cMessage* startUpMessage = new cMessage("StartUpMsg");
    double delay = 0.001 * intuniform(0, 1000, 0);
    scheduleAt((simTime() + delay).trunc(SIMTIME_MS), startUpMessage);
    startUpComplete_ = false;

    RadioDriverBase::initialize();
    mHost = veins::FindModule<>::findHost(this);
    mHost->subscribe(channelBusySignal, this);

    mLowerLayerOut = gate("lowerLayerOut");
    mLowerLayerIn = gate("lowerLayerIn");
    CAMId = 5000;
    auto properties = new RadioDriverProperties();
    properties->LinkLayerAddress = vanetza::create_mac_address(mHost->getIndex());
    // CCH used to ensure DCC configures correctly.
    properties->ServingChannel = channel::CCH;
    indicateProperties(properties);

    binder_ = getBinder();

    cModule *ue = getParentModule();
    nodeId_ = binder_->registerNode(ue, UE, 0);
    binder_->setUeId(nodeId_);
    binder_->setMacNodeId(convert(properties->LinkLayerAddress), nodeId_);
    EV<<"RadioDriver::initialize()"<<endl;
    Listener::subscribeTraCI(getSystemModule());
    RadioDriver::getStationaryModulePosition();
    numberCAMs = registerSignal("numberCAMSGenerated");
    CAMIdSent = registerSignal("transmittedCAMId");
}



void RadioDriver::finish()
{
    unsubscribeTraCI();

}

void RadioDriver::handleMessage(cMessage* msg){

    EV<<"RadioDriver LTE::received CAM "<<msg->getName()<<endl;
    if (msg->isName("CBR")) {
        Cbr* cbrPkt = check_and_cast<Cbr*>(msg);
        double channel_load = cbrPkt->getCbr();
        //emit(RadioDriverBase::ChannelLoadSignal, channel_load);
    } else if (RadioDriverBase::isDataRequest(msg)) {

        handleDataRequest(msg);
    } else if (msg->getArrivalGate() == mLowerLayerIn) {
        handleDataIndication(msg);
    } else if (strcmp(msg->getName(), "StartUpMsg") == 0) {
        startUpComplete_ = true;
    } else {
        throw cRuntimeError("unexpected message");
    }
}

void RadioDriver::handleDataIndication(cMessage* packet)
{
    auto* lteControlInfo = check_and_cast<FlowControlInfo*>(packet->removeControlInfo());
    auto* indication = new GeoNetIndication();
    indication->source = convert(lteControlInfo->getSrcAddr());
    indication->destination = convert(lteControlInfo->getDstAddr());
    packet->setControlInfo(indication);
    delete lteControlInfo;

    indicateData(packet);

}

void RadioDriver::handleDataRequest(cMessage* packet)
{
    EV<<"RadioDriver::handleDataRequest start up complete: "<<endl;
    using vanetza::units::si::seconds;
    auto request = check_and_cast<GeoNetRequest*>(packet->removeControlInfo());

    auto lteControlInfo = new FlowControlInfo();
    lteControlInfo->setSrcAddr(convert(request->source_addr));
    lteControlInfo->setDstAddr(convert(request->destination_addr));
    lteControlInfo->setPriority(user_priority(request->access_category));
    lteControlInfo->setDuration(1);  // Duration/max lifetime of all CAM packets = 1s according to standards
    lteControlInfo->setCreationTime(packet->getCreationTime());

    if (request->destination_addr == vanetza::cBroadcastMacAddress) {
        lteControlInfo->setDirection(D2D_MULTI);
    }

    packet->setControlInfo(lteControlInfo);

    //Create LTE compatible CAM packet out of GeoNet received from Vanetza

    EV<<"Sending to CV2X protocol stack"<<endl;
    cPacket* p = check_and_cast<cPacket *>(packet);
    auto cam = makeShared<ByteCountChunk>(B(p->getByteLength()));
    Packet* campacket = new Packet("LtePdcpPdu",cam);
    auto camInfo = campacket->addTagIfAbsent<FlowControlInfo>();
    camInfo->setSrcAddr(convert(request->source_addr));
    camInfo->setDstAddr(convert(request->destination_addr));
    camInfo->setPriority(user_priority(request->access_category));
    camInfo->setDuration(1);
    camInfo->setCreationTime(packet->getCreationTime());
    if (request->destination_addr == vanetza::cBroadcastMacAddress) {
        camInfo->setDirection(D2D_MULTI);
    }

    CAMSGenerated = CAMSGenerated+1;
    CAMId = CAMId+1;
    emit(numberCAMs,CAMSGenerated);
    send(campacket, mLowerLayerOut);
    emit(CAMIdSent,CAMId);
    delete request;
}

void RadioDriver::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signal, bool busy, omnetpp::cObject*)
{
    ASSERT(signal == channelBusySignal);
    if (busy) {
        mChannelLoadMeasurements.busy();
    } else {
        mChannelLoadMeasurements.idle();
    }
}


std::vector<TraCIPosition> RadioDriver::getStationaryModulePosition()
{
    auto traci = inet::getModuleFromPar<traci::Core>(par("traciCoreModule"), this);
    traci::LiteAPI& api = traci->getLiteAPI();
    const traci::Boundary boundary { api.simulation().getNetBoundary() };

    std::vector<std::string> baseStations = api.poi().getIDList();

    for (int k=0; k< baseStations.size(); k++)
    {
        std::string poiID = baseStations.at(k);
        enbPosTraci.push_back(api.poi().getPosition(poiID));
    }
    for (int k=0; k< enbPosTraci.size(); k++)
    {
        enbPosOmnet.push_back(traci::position_cast(boundary, Position {enbPosTraci[k].x,enbPosTraci[k].y})) ;
    }
    PoiRetrievalModule* s = new PoiRetrievalModule ("enbCoord");

    s->setEnbPositionOmnet(enbPosOmnet);

    return enbPosOmnet;
}
}

