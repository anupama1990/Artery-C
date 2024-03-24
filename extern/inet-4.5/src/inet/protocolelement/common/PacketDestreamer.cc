//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#include "inet/protocolelement/common/PacketDestreamer.h"

#include "inet/common/ModuleAccess.h"

namespace inet {

Define_Module(PacketDestreamer);

PacketDestreamer::~PacketDestreamer()
{
    delete streamedPacket;
    streamedPacket = nullptr;
}

void PacketDestreamer::initialize(int stage)
{
    PacketProcessorBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        datarate = bps(par("datarate"));
        inputGate = gate("in");
        outputGate = gate("out");
        producer = findConnectedModule<IActivePacketSource>(inputGate);
        provider = findConnectedModule<IPassivePacketSource>(inputGate);
        consumer = findConnectedModule<IPassivePacketSink>(outputGate);
        collector = findConnectedModule<IActivePacketSink>(outputGate);
    }
    else if (stage == INITSTAGE_QUEUEING) {
        checkPacketOperationSupport(inputGate);
        checkPacketOperationSupport(outputGate);
    }
}

void PacketDestreamer::handleMessage(cMessage *message)
{
    auto packet = check_and_cast<Packet *>(message);
    pushPacket(packet, packet->getArrivalGate());
}

bool PacketDestreamer::canPushSomePacket(cGate *gate) const
{
    return !isStreaming() && consumer->canPushSomePacket(outputGate->getPathEndGate());
}

bool PacketDestreamer::canPushPacket(Packet *packet, cGate *gate) const
{
    return !isStreaming() && consumer->canPushPacket(packet, outputGate->getPathEndGate());
}

void PacketDestreamer::pushPacketStart(Packet *packet, cGate *gate, bps datarate)
{
    Enter_Method("pushPacketStart");
    take(packet);
    delete streamedPacket;
    streamedPacket = packet;
    streamDatarate = datarate;
    EV_INFO << "Starting destreaming packet" << EV_FIELD(packet, *streamedPacket) << EV_ENDL;
}

void PacketDestreamer::pushPacketEnd(Packet *packet, cGate *gate)
{
    Enter_Method("pushPacketEnd");
    take(packet);
    delete streamedPacket;
    streamedPacket = packet;
    streamDatarate = datarate;
    auto packetLength = streamedPacket->getTotalLength();
    EV_INFO << "Ending destreaming packet" << EV_FIELD(packet, *streamedPacket) << EV_ENDL;
    pushOrSendPacket(streamedPacket, outputGate, consumer);
    streamedPacket = nullptr;
    numProcessedPackets++;
    processedTotalLength += packetLength;
    updateDisplayString();
}

void PacketDestreamer::pushPacketProgress(Packet *packet, cGate *gate, bps datarate, b position, b extraProcessableLength)
{
    Enter_Method("pushPacketProgress");
    take(packet);
    delete streamedPacket;
    streamedPacket = packet;
    streamDatarate = datarate;
    EV_INFO << "Progressing destreaming" << EV_FIELD(packet, *streamedPacket) << EV_ENDL;
}

void PacketDestreamer::handleCanPushPacketChanged(cGate *gate)
{
    Enter_Method("handleCanPushPacketChanged");
    if (producer != nullptr)
        producer->handleCanPushPacketChanged(inputGate->getPathStartGate());
}

void PacketDestreamer::handlePushPacketProcessed(Packet *packet, cGate *gate, bool successful)
{
    Enter_Method("handlePushPacketProcessed");
    if (producer != nullptr)
        producer->handlePushPacketProcessed(packet, inputGate->getPathStartGate(), successful);
}

bool PacketDestreamer::canPullSomePacket(cGate *gate) const
{
    return !isStreaming() && provider->canPullSomePacket(inputGate->getPathStartGate());
}

Packet *PacketDestreamer::canPullPacket(cGate *gate) const
{
    return isStreaming() ? nullptr : provider->canPullPacket(inputGate->getPathStartGate());
}

Packet *PacketDestreamer::pullPacket(cGate *gate)
{
    Enter_Method("pullPacket");
    ASSERT(!isStreaming());
    streamDatarate = datarate;
    auto packet = provider->pullPacketStart(inputGate->getPathStartGate(), streamDatarate);
    EV_INFO << "Starting destreaming packet" << EV_FIELD(packet) << EV_ENDL;
    take(packet);
    streamedPacket = packet;
    packet = provider->pullPacketEnd(inputGate->getPathStartGate());
    EV_INFO << "Ending destreaming packet" << EV_FIELD(packet) << EV_ENDL;
    take(packet);
    delete streamedPacket;
    streamedPacket = packet;
    handlePacketProcessed(packet);
    animatePullPacket(streamedPacket, outputGate);
    updateDisplayString();
    streamedPacket = nullptr;
    return packet;
}

void PacketDestreamer::handlePullPacketProcessed(Packet *packet, cGate *gate, bool successful)
{
    Enter_Method("handlePullPacketConfirmation");
    if (collector != nullptr)
        collector->handlePullPacketProcessed(packet, gate, successful);
}

void PacketDestreamer::handleCanPullPacketChanged(cGate *gate)
{
    Enter_Method("handleCanPullPacketChanged");
    if (collector != nullptr)
        collector->handleCanPullPacketChanged(outputGate->getPathEndGate());
}

} // namespace inet

