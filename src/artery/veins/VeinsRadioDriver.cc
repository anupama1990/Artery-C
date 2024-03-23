#include <inet/common/packet/Packet.h>
#include <inet/common/packet/chunk/cPacketChunk.h>
#include "artery/networking/GeoNetIndication.h"
#include "artery/networking/GeoNetRequest.h"
#include "artery/nic/RadioDriverProperties.h"
#include "artery/veins/VeinsRadioDriver.h"
#include "veins/base/utils/FindModule.h"
#include "veins/base/utils/SimpleAddress.h"
#include "veins/modules/messages/BaseFrame1609_4_m.h"
#include "veins/modules/utility/Consts80211p.h"

using namespace omnetpp;
using namespace veins;

namespace artery
{

Register_Class(VeinsRadioDriver)

namespace {

LAddress::L2Type convert(const vanetza::MacAddress& mac)
{
    if (mac == vanetza::cBroadcastMacAddress) {
        return LAddress::L2BROADCAST();
    } else {
        LAddress::L2Type addr = 0;
        for (unsigned i = 0; i < mac.octets.size(); ++i) {
            addr <<= 8;
            addr |= mac.octets[i];
        }
        return addr;
    }
}

vanetza::MacAddress convert(LAddress::L2Type addr)
{
    if (LAddress::isL2Broadcast(addr)) {
        return vanetza::cBroadcastMacAddress;
    } else {
        vanetza::MacAddress mac;
        for (unsigned i = mac.octets.size(); i > 0; --i) {
            mac.octets[i - 1] = addr & 0xff;
            addr >>= 8;
        }
        return mac;
    }
}

int user_priority(vanetza::access::AccessCategory ac)
{
    using AC = vanetza::access::AccessCategory;
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

} // namespace

void VeinsRadioDriver::initialize()
{
    RadioDriverBase::initialize();
    mHost = veins::FindModule<>::findHost(this);
    mHost->subscribe(channelBusySignal, this);

    mLowerLayerOut = gate("lowerLayerOut");
    mLowerLayerIn = gate("lowerLayerIn");

    mChannelLoadMeasurements.reset();
    mChannelLoadReport = new cMessage("report channel load");
    mChannelLoadReportInterval = par("channelLoadReportInterval");
    scheduleAt(simTime() + mChannelLoadReportInterval, mChannelLoadReport);

    auto properties = new RadioDriverProperties();
    // Mac1609_4 uses index of host as MAC address
    properties->LinkLayerAddress = vanetza::create_mac_address(mHost->getIndex());
    indicateProperties(properties);
}

void VeinsRadioDriver::handleMessage(cMessage* msg)
{
    if (msg == mChannelLoadReport) {
        double channel_load = mChannelLoadMeasurements.channel_load().value();
        emit(RadioDriverBase::ChannelLoadSignal, channel_load);
        scheduleAt(simTime() + mChannelLoadReportInterval, mChannelLoadReport);
    } else if (RadioDriverBase::isDataRequest(msg)) {
        handleDataRequest(msg);
    } else if (msg->getArrivalGate() == mLowerLayerIn) {
        handleDataIndication(msg);
    } else {
        throw cRuntimeError("unexpected message");
    }
}

void VeinsRadioDriver::handleDataIndication(cMessage* msg)
{
    auto wsm = check_and_cast<BaseFrame1609_4*>(msg);
    auto gn = wsm->decapsulate();
    auto* indication = new GeoNetIndication();
//    indication->source = convert(wsm->getSenderAddress());
    indication->source = convert(LAddress::L2NULL()); //todo: new BaseFrame1609_4 does not provide source.
    indication->destination = convert(wsm->getRecipientAddress());
    gn->setControlInfo(indication);
    delete wsm;

    indicateData(gn);
}

void VeinsRadioDriver::handleDataRequest(cMessage* packet)
{
    auto request = check_and_cast<GeoNetRequest*>(packet->removeControlInfo());
    auto wsm = new BaseFrame1609_4();
    wsm->encapsulate(check_and_cast<cPacket*>(packet));
    // todo BaseFrame1609_4 does not need source
//    wsm->setSenderAddress(convert(request->source_addr));
    wsm->setRecipientAddress(convert(request->destination_addr));
    wsm->setUserPriority(user_priority(request->access_category));
    wsm->setChannelNumber((int)Channel::cch);

    delete request;
    send(wsm, mLowerLayerOut);
}

void VeinsRadioDriver::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signal, bool busy, omnetpp::cObject*)
{
    ASSERT(signal == channelBusySignal);
    if (busy) {
        mChannelLoadMeasurements.busy();
    } else {
        mChannelLoadMeasurements.idle();
    }
}

} // namespace artery
