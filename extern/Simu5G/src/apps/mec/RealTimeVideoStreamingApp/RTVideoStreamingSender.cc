//
//                  Simu5G
//
// Authors: Giovanni Nardini, Giovanni Stea, Antonio Virdis (University of Pisa)
//
// This file is part of a software released under the license included in file
// "license.pdf". Please read LICENSE and README files before using it.
// The above files and the present reference are part of the software itself,
// and cannot be removed from it.
//

#include "apps/mec/RealTimeVideoStreamingApp/RTVideoStreamingSender.h"
#include "apps/mec/DeviceApp/DeviceAppMessages/DeviceAppPacket_m.h"
#include "apps/mec/DeviceApp/DeviceAppMessages/DeviceAppPacket_Types.h"
#include "nodes/mec/MECPlatform/MEAppPacket_Types.h"
#include "apps/mec/RealTimeVideoStreamingApp/packets/RTVideoStreamingPacket_Types.h"
#include "apps/mec/RealTimeVideoStreamingApp/packets/RTVideoStreamingPackets_m.h"

#include "inet/common/TimeTag_m.h"
#include "inet/common/packet/chunk/BytesChunk.h"

#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"

#include <fstream>

using namespace inet;
using namespace std;

Define_Module(RTVideoStreamingSender);

RTVideoStreamingSender::RTVideoStreamingSender(){
    selfRTVideoStreamingAppStart_ = nullptr;
    selfRTVideoStreamingAppStop_ = nullptr;

    selfMecAppStart_ = nullptr;
    selfMecAppStop_ = nullptr;
    _nextFrame = nullptr;

    selfSessionStart_ = nullptr;
    selfSessionStop_ = nullptr;

    _frameNumber = 0;
    _sequenceNumber = 0;
    sessionId_ = 0;
}

RTVideoStreamingSender::~RTVideoStreamingSender(){
    cancelAndDelete(selfRTVideoStreamingAppStart_);
    cancelAndDelete(selfRTVideoStreamingAppStop_);
    cancelAndDelete(selfMecAppStart_);
    cancelAndDelete(selfMecAppStop_);
    cancelAndDelete(selfSessionStart_);
    cancelAndDelete(selfSessionStop_);

    cancelAndDelete(mobilityStats_);

    cancelAndDelete(_nextFrame);

    if(_inputFileStream.is_open())
        _inputFileStream.close();
}

void RTVideoStreamingSender::initialize(int stage)
{
    EV << "RTVideoStreamingSender::initialize - stage " << stage << endl;
    cSimpleModule::initialize(stage);
    // avoid multiple initializations
    if (stage!=inet::INITSTAGE_APPLICATION_LAYER)
        return;

    // UE app <--> Device App info
    localPort_ = par("localPort");
    deviceAppPort_ = par("deviceAppPort");
    sourceSimbolicAddress = (char*)getParentModule()->getFullName();
    deviceSimbolicAppAddress_ = (char*)par("deviceAppAddress").stringValue();
    deviceAppAddress_ = inet::L3AddressResolver().resolve(deviceSimbolicAppAddress_);

    mtu_ = 1500;
    //binding socket
    socket.setOutputGate(gate("socketOut"));
    socket.bind(localPort_);

    int tos = par("tos");
    if (tos != -1)
        socket.setTos(tos);

    //retrieving car cModule
    ue = this->getParentModule();

    //retrieving mobility module
    cModule *temp = getParentModule()->getSubmodule("mobility");
    if(temp != nullptr){
        mobility = check_and_cast<inet::IMobility*>(temp);
    }
    else {
        EV << "RTVideoStreamingSender::initialize - \tWARNING: Mobility module NOT FOUND!" << endl;
        throw cRuntimeError("RTVideoStreamingSender::initialize - \tWARNING: Mobility module NOT FOUND!");
    }

    mecAppName = par("mecAppName").stringValue();

    //retrieve parameters for the app
    size_ = par("packetSize");
    lifeCyclePeriod_ = par("period");

    // fileName of file to be transmitted
    // nullptr or "" means this system acts only as a receiver
    fileName = par("fileName").stringValue();
    openFileStream();
    initializeVideoStream();

    sendAllOnOneTime_ = par("sendAllOnOneTime").boolValue();


    //initializing the auto-scheduling messages
    selfRTVideoStreamingAppStart_ = new cMessage("selfRTVideoStreamingAppStart");
    selfRTVideoStreamingAppStop_ = new cMessage("selfRTVideoStreamingAppAppStop");

    selfMecAppStart_ = new cMessage("selfMecAppStart");
    selfMecAppStop_ = new cMessage("selfMecAppStop");

    selfSessionStart_ = new cMessage("selfSessionStart");
    selfSessionStop_ = new cMessage("selfSessionStop");

    _nextFrame = new cMessage("nextFrame");

    //starting RTVideoStreamingSender
    simtime_t startTime = par("startTime");


    EV << "RTVideoStreamingSender::initialize - starting sendStartMECApp() in " << startTime << " seconds " << endl;
    scheduleAt(simTime() + startTime, selfMecAppStart_);

    //testing
    EV << "RTVideoStreamingSender::initialize - sourceAddress: " << sourceSimbolicAddress << " [" << inet::L3AddressResolver().resolve(sourceSimbolicAddress).str()  <<"]"<< endl;
    EV << "RTVideoStreamingSender::initialize - destAddress: " << deviceSimbolicAppAddress_ << " [" << deviceAppAddress_.str()  <<"]"<< endl;
    EV << "RTVideoStreamingSender::initialize - binding to port: local:" << localPort_ << " , dest:" << deviceAppPort_ << endl;


    mobilityUpdateInterval_ = 1.0;

    velocitySignal = registerSignal("velocity");
    positionSignalX = registerSignal("positionX");
    positionSignalY = registerSignal("positionY");
    positionSignalZ = registerSignal("positionZ");
    mobilityStats_ = new cMessage("mobilityStats");
    scheduleAfter(mobilityUpdateInterval_, mobilityStats_);
}

void RTVideoStreamingSender::handleMessage(cMessage *msg)
{
    EV << "RTVideoStreamingSender::handleMessage" << endl;
    // Sender Side
    if (msg->isSelfMessage())
    {
        if(strcmp(msg->getName(), "mobilityStats") == 0)
        {
            Coord position = mobility->getCurrentPosition();
            Coord velocity = mobility->getCurrentVelocity();
            emit(positionSignalX, position.x);
            emit(positionSignalY, position.y);
            emit(positionSignalZ, position.z);

            emit(velocitySignal, velocity.length());
            scheduleAfter(mobilityUpdateInterval_, mobilityStats_);
            return;
        }
        if(!strcmp(msg->getName(), "selfMecAppStart"))
        {
            sendStartMECApp();
        }
        else if(!strcmp(msg->getName(), "selfMecAppStop"))
        {
            sendStopMECApp();
        }
        else if(!strcmp(msg->getName(), "selfRTVideoStreamingAppStop"))
        {
            sendStopMessage();
            scheduleAt(simTime() + lifeCyclePeriod_, selfRTVideoStreamingAppStop_);
        }
        else if(!strcmp(msg->getName(), "selfSessionStart"))
        {
            sendSessionStartMessage();
            // reschedule for pks losses
            scheduleAt(simTime() + lifeCyclePeriod_, selfSessionStart_);
        }
        else if(!strcmp(msg->getName(), "selfSessionStop"))
        {
            sendSessionStopMessage();
            scheduleAt(simTime() + lifeCyclePeriod_, selfSessionStop_);

        }
        else if(!strcmp(msg->getName(), "nextFrame"))
        {
            sendMessage();
        }
        else    throw cRuntimeError("RTVideoStreamingSender::handleMessage - \tWARNING: Unrecognized self message");
    }
    // Receiver Side
    else{
        inet::Packet* packet = check_and_cast<inet::Packet*>(msg);
        inet::L3Address ipAdd = packet->getTag<L3AddressInd>()->getSrcAddress();

        /*
         * From Device app
         * device app usually runs in the UE (loopback), but it could also run in other places
         */
        if(ipAdd == deviceAppAddress_ || ipAdd == inet::L3Address("127.0.0.1")) // dev app
        {
            auto mePkt = packet->peekAtFront<DeviceAppPacket>();

            if (mePkt == 0)
                throw cRuntimeError("RTVideoStreamingSender::handleMessage - \tFATAL! Error when casting to DeviceAppPacket");

            if( !strcmp(mePkt->getType(), ACK_START_MECAPP))
            {
                handleAckStartMECApp(msg);
            }

            else if(!strcmp(mePkt->getType(), ACK_STOP_MECAPP))
            {
                handleAckStopMECApp(msg);
            }
            else
            {
                throw cRuntimeError("RTVideoStreamingSender::handleMessage - \tFATAL! Error, DeviceAppPacket type %s not recognized", mePkt->getType());
            }
        }
        // From MEC application
        else
        {
            auto mePkt = packet->peekAtFront<RealTimeVideoStreamingAppPacket>();
            EV << "RTVideoStreamingSender::handleMessage - message from MEC app of type: " << mePkt->getType() << endl;
            if (mePkt == 0)
                throw cRuntimeError("RTVideoStreamingSender::handleMessage - \tFATAL! Error when casting to WarningAppPacket");

            if(mePkt->getType() == RTVIDEOSTREAMING_COMMAND)
            {
                handleInfoMECApp(msg);
            }
            else if(mePkt->getType() == START_RTVIDEOSTREAMING_NACK)
            {
                handleStartNack(msg);
            }
            else if(mePkt->getType() == START_RTVIDEOSTREAMING_ACK)
            {
                handleStartAck(msg);
            }
            else if(mePkt->getType() == START_RTVIDEOSTREAMING_SESSION_ACK)
            {
                handleStartSessionAck(msg);
            }
            else if(mePkt->getType() == START_RTVIDEOSTREAMING_SESSION_NACK)
            {
                handleStartSessionNack(msg);
            }
            else if(mePkt->getType() == STOP_RTVIDEOSTREAMING_ACK)
            {
                handleStopAck(msg);
            }
            else if(mePkt->getType() == STOP_RTVIDEOSTREAMING_NACK)
            {
                handleStopNack(msg);
            }
            else if(mePkt->getType() == STOP_RTVIDEOSTREAMING_SESSION_ACK)
            {
                handleStopSessionAck(msg);
            }
            else if(mePkt->getType() == STOP_RTVIDEOSTREAMING_SESSION_NACK)
            {
                handleStopSessionNack(msg);
            }
            else
            {
                throw cRuntimeError("RTVideoStreamingSender::handleMessage - \tFATAL! Error, WarningAppPacket type %d not recognized", mePkt->getType());
            }
        }
        delete msg;
    }
}

void RTVideoStreamingSender::finish()
{

}
/*
 * -----------------------------------------------Sender Side------------------------------------------
 */
void RTVideoStreamingSender::sendStartMECApp()
{
    inet::Packet* packet = new inet::Packet("RTVideoStreamingSenderPacketStart");
    auto start = inet::makeShared<DeviceAppStartPacket>();

    //instantiation requirements and info
    start->setType(START_MECAPP);
    start->setMecAppName(mecAppName.c_str());

    start->setChunkLength(inet::B(2+mecAppName.size()+1));
    start->addTagIfAbsent<inet::CreationTimeTag>()->setCreationTime(simTime());


    packet->insertAtBack(start);

    socket.sendTo(packet, deviceAppAddress_, deviceAppPort_);

    //rescheduling
    scheduleAt(simTime() + lifeCyclePeriod_, selfMecAppStart_);
}
void RTVideoStreamingSender::sendStopMECApp()
{
    EV << "RTVideoStreamingSender::sendStopMECApp - Sending " << STOP_MEAPP <<" type RTVideoStreamingSender\n";

    inet::Packet* packet = new inet::Packet("DeviceAppStopPacket");
    auto stop = inet::makeShared<DeviceAppStopPacket>();

    //termination requirements and info
    stop->setType(STOP_MECAPP);

    stop->setChunkLength(inet::B(size_));
    stop->addTagIfAbsent<inet::CreationTimeTag>()->setCreationTime(simTime());

    packet->insertAtBack(stop);
    socket.sendTo(packet, deviceAppAddress_, deviceAppPort_);

    //rescheduling
    if(selfMecAppStop_->isScheduled())
        cancelEvent(selfMecAppStop_);
    scheduleAt(simTime() + lifeCyclePeriod_, selfMecAppStop_);
}

void RTVideoStreamingSender::sendSessionStartMessage()
{
    inet::Packet* pkt = new inet::Packet("RealTimeVideoStreamingAppPacket");
    auto startMsg = inet::makeShared<StartSessionRealTimeVideoStreamingAppPacket>();
    startMsg->setType(START_RTVIDEOSTREAMING_SESSION);
    startMsg->setFps(_framesPerSecond);
    startMsg->setSessionId(sessionId_);

    startMsg->setChunkLength(inet::B(10));
    startMsg->addTagIfAbsent<inet::CreationTimeTag>()->setCreationTime(simTime());
    pkt->insertAtBack(startMsg);

    socket.sendTo(pkt, mecAppAddress_ , mecAppPort_);
    EV << "RTVideoStreamingSender::sendSessionStartMessage() - START_RTVIDEOSTREAMING_SESSION msg with session id [" << sessionId_ << "] sent to the MEC app" << endl;
}

void RTVideoStreamingSender::sendStartMessage()
{
    inet::Packet* pkt = new inet::Packet("RealTimeVideoStreamingAppPacket");
    auto startMsg = inet::makeShared<StartRealTimeVideoStreamingAppPacket>();
    startMsg->setType(START_RTVIDEOSTREAMING);
    startMsg->setFps(_framesPerSecond);
    startMsg->setChunkLength(inet::B(10));
    startMsg->addTagIfAbsent<inet::CreationTimeTag>()->setCreationTime(simTime());
    pkt->insertAtBack(startMsg);

    socket.sendTo(pkt, mecAppAddress_ , mecAppPort_);
    EV << "RTVideoStreamingSender::sendStartMessageToMecApp() - start Message sent to the MEC app" << endl;
}

void RTVideoStreamingSender::sendStopMessage()
{
    inet::Packet* pkt = new inet::Packet("RealTimeVideoStreamingAppPacket");
    auto startMsg = inet::makeShared<RealTimeVideoStreamingAppPacket>();
    startMsg->setType(STOP_RTVIDEOSTREAMING);
    startMsg->setChunkLength(inet::B(10));
    startMsg->addTagIfAbsent<inet::CreationTimeTag>()->setCreationTime(simTime());
    pkt->insertAtBack(startMsg);

    socket.sendTo(pkt, mecAppAddress_ , mecAppPort_);
    EV << "RTVideoStreamingSender::sendStopMessageToMecApp() - stop Message sent to the MEC app" << endl;

    //cancell all timers, i I want to stop the mec app
    cancelEvent(selfSessionStart_);
    cancelEvent(selfSessionStop_);
    cancelEvent(_nextFrame);
}

void RTVideoStreamingSender::sendSessionStopMessage()
{
    inet::Packet* pkt = new inet::Packet("RealTimeVideoStreamingAppPacket");
    auto startMsg = inet::makeShared<StopSessionRealTimeVideoStreamingAppPacket>();
    startMsg->setType(STOP_RTVIDEOSTREAMING_SESSION);
    startMsg->setSessionId(sessionId_);
    startMsg->setChunkLength(inet::B(10));
    startMsg->addTagIfAbsent<inet::CreationTimeTag>()->setCreationTime(simTime());
    pkt->insertAtBack(startMsg);

    cancelEvent(_nextFrame);

    socket.sendTo(pkt, mecAppAddress_ , mecAppPort_);
    EV << "RTVideoStreamingSender::sendSessionStopMessage() - STOP_RTVIDEOSTREAMING_SESSION msg with session id [" << sessionId_ << "] sent to the MEC app" << endl;
}

void RTVideoStreamingSender::sendMessage(){

    // send star monitoring message to the MEC application

    EV_TRACE << "RTVideoStreamingSender::sendMessageToMECApp() " << endl;
    // read next frame line
    int bits;
    char unit[100];
    char line[100];
    char description[100];

    // read a new frame
    if(sendAllOnOneTime_ || (fragFrameStatus_.remainingFragments == 0 && !sendAllOnOneTime_))
    {
        _inputFileStream >> bits; // the function sets the eofbit flag.
        _inputFileStream >> unit; // the function sets the eofbit flag.
        _inputFileStream.get(description, 100, '\n'); // the function sets the eofbit flag.
        EV << "RTVideoStreamingSender::sendPacket - new frame of size: " << bits << " bits, description "<< description << " bytes eof " << _inputFileStream.eof() << endl;
        if(_inputFileStream.eof())
        {
            EV << "RTVideoStreamingSender::sendMessage - file ended, start reading from begin" << endl;
            _inputFileStream.clear();
            _inputFileStream.seekg(0, _inputFileStream.beg);

            // first line: fps unit description
            _inputFileStream.get(line, 100, '\n');
            // get new line character
            char c;
            _inputFileStream.get(c);
            // second line: initial delay unit description
            _inputFileStream.get(line, 100, '\n');

            _inputFileStream >> bits; // the function sets the eofbit flag.
            _inputFileStream >> unit; // the function sets the eofbit flag.
            _inputFileStream.get(description, 100, '\n'); // the function sets the eofbit flag.
        }

        int bytesRemaining = bits / 8;
        EV << "RTVideoStreamingSender::sendPacket - new frame of size: " << bits << " bits, i.e. "<< bytesRemaining << " bytes" << endl;

       int frameNum = _frameNumber++;
       int pictureType;
       char *ptr;

       for (ptr = description; *ptr == ' ' || *ptr == '\t'; ptr++)
           ;
       switch (*ptr) {
           case 'I':
               pictureType = 1;
               break;
           case 'P':
               pictureType = 2;
               break;

           case 'B':
               pictureType = 3;
               break;

           case 'D':
               pictureType = 4;
               break;

           default:
               pictureType = 0;
               break;
       }

       // send the frame all on once
       if(sendAllOnOneTime_)
       {
           bool first = true;;
           int maxDataSize = 0;
           int numberOfFragments = 0;
           int slicedDataSize = 0;
           while (bytesRemaining > 0) {
               Packet *packet = new Packet("RealTimeVideoStreamingAppSegment");
               packet->addTagIfAbsent<CreationTimeTag>()->setCreationTime(simTime());

               const auto& mpegHeader = makeShared<RealTimeVideoStreamingAppSegmentHeader>(); mpegHeader->setType(RTVIDEOSTREAMING_SEGMENT);
               const auto& mpegPayload = makeShared<ByteCountChunk>();

               // the only mpeg information we know is the picture type
               mpegHeader->setPictureType(pictureType);

               if(first)
               {
                   // the maximum number of real data bytes
                   maxDataSize = mtu_ - 4 - 20 - 8;//B(mtu_) - B(mpegHeader->getChunkLength()).get();
                   EV << "r " <<  bytesRemaining/maxDataSize << " ceil "<<  ceil((double) bytesRemaining/maxDataSize) << endl;
                   numberOfFragments = ceil((double) bytesRemaining/maxDataSize);
                   slicedDataSize = bytesRemaining/(numberOfFragments);
                   EV <<"max data size " << maxDataSize << endl;
                   EV <<"num of frags " << numberOfFragments << endl;
                   EV << "sliced data "<< slicedDataSize << endl;
                   first = false;

               }

               if (bytesRemaining > maxDataSize) {
                   // we do not know where slices in the
                   // mpeg picture begin
                   // so we simulate by assuming a slice
                   // has a length of 64 bytes

                   mpegHeader->setPayloadLength(maxDataSize);
                   mpegPayload->setLength(B(maxDataSize));

                   bytesRemaining = bytesRemaining - maxDataSize;
//                       EV << "BR: " << bytesRemaining << endl;
               }
               else {
//                       EV << "else BR: " << bytesRemaining << endl;
                   mpegHeader->setPayloadLength(bytesRemaining);
                   mpegPayload->setLength(B(bytesRemaining));
                   // set marker because this is
                   // the last packet of the frame
                   mpegHeader->setIsLastFragment(true);
                   bytesRemaining = 0;
               }

               mpegHeader->setFrameLength(bits/8);
               mpegHeader->setFrameNumber(frameNum);
               mpegHeader->setSessionId(sessionId_);
               mpegHeader->setTotalFrags(numberOfFragments);
               mpegHeader->setChunkLength(B(4));
               mpegHeader->addTagIfAbsent<CreationTimeTag>()->setCreationTime(simTime());
               mpegHeader->setSequenceNumber(_sequenceNumber);

               _sequenceNumber++;

               packet->insertAtBack(mpegHeader);
               packet->insertAtBack(mpegPayload);

               socket.sendTo(packet, mecAppAddress_ , mecAppPort_);
           }
           scheduleAfter(1.0 / _framesPerSecond, _nextFrame);
       }
       else
       {
           // send the frame periodically
            Packet *packet = new Packet("RealTimeVideoStreamingAppSegment");
            packet->addTagIfAbsent<CreationTimeTag>()->setCreationTime(simTime());
            const auto& mpegHeader = makeShared<RealTimeVideoStreamingAppSegmentHeader>(); mpegHeader->setType(RTVIDEOSTREAMING_SEGMENT);
            const auto& mpegPayload = makeShared<ByteCountChunk>();

            // initialize fragFrameStatus for the new frame
            fragFrameStatus_.frameNumber = frameNum;
            fragFrameStatus_.frameSize = bytesRemaining;

            int maxDataSize = mtu_ - 4 - 20 - 8; // FIXME rtp header = 12B (fixed), rtpCarHeader is 4B
            int numberOfFragments  = ceil((double)bytesRemaining/maxDataSize);
            fragFrameStatus_.numberOfFragments =  numberOfFragments;
            fragFrameStatus_.remainingFragments = numberOfFragments - 1; //since I send the first one now
            fragFrameStatus_.bytesPerPacket = maxDataSize;
            fragFrameStatus_.remainingFrameBytes = fragFrameStatus_.frameSize - fragFrameStatus_.bytesPerPacket;

            EV << "RTVideoStreamingSender::sendPacket - number of fragments for the frame with frame number [" << frameNum << "] is " << numberOfFragments << endl;

            mpegHeader->setFrameLength(fragFrameStatus_.frameSize);
            mpegHeader->setPayloadLength(fragFrameStatus_.bytesPerPacket);
            mpegPayload->setLength(B(fragFrameStatus_.bytesPerPacket));

            mpegHeader->setFrameNumber(fragFrameStatus_.frameNumber);
            mpegHeader->setSessionId(sessionId_);
            mpegHeader->setTotalFrags(fragFrameStatus_.numberOfFragments);
            mpegHeader->addTagIfAbsent<CreationTimeTag>()->setCreationTime(simTime());

            _sequenceNumber++;

//                rtpHeader->setTimeStamp(_timeStampBase + (_initialDelay + (1 / _framesPerSecond) * (double)_frameNumber) * _clockRate);
//                rtpHeader->setSsrc(_ssrc);
//                packet->insertAtFront(rtpHeader);

            packet->insertAtBack(mpegHeader);
            packet->insertAtBack(mpegPayload);

            socket.sendTo(packet, mecAppAddress_ , mecAppPort_);
            scheduleAfter(1.0 / (_framesPerSecond * (fragFrameStatus_.numberOfFragments)), _nextFrame);
       }

    }
    else
    {
        // send a fragment of a a frame
        Packet *packet = new Packet("RealTimeVideoStreamingAppSegment");
        packet->addTagIfAbsent<CreationTimeTag>()->setCreationTime(simTime());

        const auto& mpegHeader = makeShared<RealTimeVideoStreamingAppSegmentHeader>(); mpegHeader->setType(RTVIDEOSTREAMING_SEGMENT);
        const auto& mpegPayload = makeShared<ByteCountChunk>();

        if(fragFrameStatus_.remainingFragments == 1)
        {
            mpegHeader->setPayloadLength(fragFrameStatus_.remainingFrameBytes);
            mpegPayload->setLength(B(fragFrameStatus_.remainingFrameBytes));
        }
        else
        {
            mpegHeader->setPayloadLength(fragFrameStatus_.bytesPerPacket);
            mpegPayload->setLength(B(fragFrameStatus_.bytesPerPacket));
            fragFrameStatus_.remainingFrameBytes -= fragFrameStatus_.bytesPerPacket;
            mpegHeader->setIsLastFragment(true);
        }

        fragFrameStatus_.remainingFragments--;


        mpegHeader->setFrameNumber(fragFrameStatus_.frameNumber);
        mpegHeader->setSessionId(sessionId_);
        mpegHeader->setFrameLength(fragFrameStatus_.frameSize);
        mpegHeader->setTotalFrags(fragFrameStatus_.numberOfFragments);
        mpegHeader->addTagIfAbsent<CreationTimeTag>()->setCreationTime(simTime());

//        rtpHeader->setPayloadType(_payloadType);
//        rtpHeader->setSequenceNumber(_sequenceNumber);
        _sequenceNumber++;

//        rtpHeader->setTimeStamp(_timeStampBase + (_initialDelay + (1 / _framesPerSecond) * (double)_frameNumber) * _clockRate);
//        rtpHeader->setSsrc(_ssrc);
//        packet->insertAtFront(rtpHeader);
        packet->insertAtBack(mpegHeader);
        packet->insertAtBack(mpegPayload);

//        RtpInnerPacket *rinpOut = new RtpInnerPacket("dataOut()");
//        rinpOut->setDataOutPkt(packet);
//        EV << "fragment number " <<  fragFrameStatus_.numberOfFragments - fragFrameStatus_.remainingFragments << " of frame ["<<  fragFrameStatus_.frameNumber <<"] in rtp with seq num ["<< rtpHeader->getSequenceNumber() << "]" << endl;

        socket.sendTo(packet, mecAppAddress_ , mecAppPort_);
        scheduleAfter(1.0 / (_framesPerSecond * (fragFrameStatus_.numberOfFragments)), _nextFrame);
    }
}



/*
 * ---------------------------------------------Receiver Side------------------------------------------
 */
void RTVideoStreamingSender::handleAckStartMECApp(cMessage* msg)
{
    inet::Packet* packet = check_and_cast<inet::Packet*>(msg);
    auto pkt = packet->peekAtFront<DeviceAppStartAckPacket>();

    if(pkt->getResult() == true)
    {
        mecAppAddress_ = L3AddressResolver().resolve(pkt->getIpAddress());
        mecAppPort_ = pkt->getPort();
        EV << "RTVideoStreamingSender::handleAckStartMECApp - Received " << pkt->getType() << " type RTVideoStreamingSender. mecApp instance is at: "<< mecAppAddress_<< ":" << mecAppPort_ << endl;
        cancelEvent(selfMecAppStart_);
        // send start video streaming req to the MEC App
        sendSessionStartMessage();
        // reschdule in case of pkt lost
        scheduleAt(simTime() + lifeCyclePeriod_, selfSessionStart_);

        // schedule stop mec app
        if(!selfRTVideoStreamingAppStop_->isScheduled()){
            simtime_t  stopTime = par("stopTime");
            scheduleAt(simTime() + stopTime, selfRTVideoStreamingAppStop_);
            EV << "RTVideoStreamingSender::handleAckStartMECApp - Starting sendStopMECApp() in " << stopTime << " seconds " << endl;
        }
    }
    else
    {
        EV << "RTVideoStreamingSender::handleAckStartMECApp - MEC application cannot be instantiated! Reason: " << pkt->getReason() << endl;
    }
}

void RTVideoStreamingSender::handleInfoMECApp(cMessage* msg)
{

}

void RTVideoStreamingSender::handleAckStopMECApp(cMessage* msg)
{
    inet::Packet* packet = check_and_cast<inet::Packet*>(msg);
    auto pkt = packet->peekAtFront<DeviceAppStopAckPacket>();

    EV << "RTVideoStreamingSender::handleAckStopMECApp - Received " << pkt->getType() << " type RTVideoStreamingSender with result: "<< pkt->getResult() << endl;
    if(pkt->getResult() == false)
        EV << "Reason: "<< pkt->getReason() << endl;

    cancelEvent(selfMecAppStop_);
}

void RTVideoStreamingSender::handleStartNack(cMessage* msg)
{
    EV << "RTVideoStreamingSender::handleStartNack - MEC app did not started correctly, trying to start again" << endl;
}

void RTVideoStreamingSender::handleStartAck(cMessage* msg)
{
}

void RTVideoStreamingSender::handleStopNack(cMessage* msg)
{
    EV << "RTVideoStreamingSender::handleStopNack - MEC app did not started correctly, trying to start again" << endl;
}

void RTVideoStreamingSender::handleStopAck(cMessage* msg)
{
    EV << "RTVideoStreamingSender::handleStopAck" << endl;
    //send stop metc app to device app
    sendStopMECApp();
    if(selfRTVideoStreamingAppStop_->isScheduled())
    {
        cancelEvent(selfRTVideoStreamingAppStop_);
    }
    cancelEvent(_nextFrame);
}

void RTVideoStreamingSender::handleStartSessionAck(cMessage* msg)
{
    EV << "RTVideoStreamingSender::handleStartSessionAck" << endl;
    cancelEvent(selfSessionStart_);
    sendMessage();

    //schedule stop session
    if(!selfSessionStop_->isScheduled())
    {
        double sessionDuration = par("sessionDuration").doubleValue();
        scheduleAt(simTime() + sessionDuration, selfSessionStop_);
        EV << "RTVideoStreamingSender::handleStartSessionAck - Starting sendStopSession() in " << sessionDuration << " seconds " << endl;
    }
}

void RTVideoStreamingSender::handleStartSessionNack(cMessage* msg)
{
    EV << "RTVideoStreamingSender::handleStartSessionNack" << endl;
}

void RTVideoStreamingSender::handleStopSessionAck(cMessage* msg)
{
    EV << "RTVideoStreamingSender::handleStopSessionAck" << endl;
    cancelEvent(selfSessionStop_);

    sessionId_++;

    _frameNumber = 0; //reset frame
    //schedule stop session
    if(!selfSessionStart_->isScheduled())
    {
        double periodBetweenSession = par("periodBetweenSession").doubleValue();
        scheduleAt(simTime() + periodBetweenSession, selfSessionStart_);
        EV << "RTVideoStreamingSender::handleStartSessionAck - Starting sendStartSession() in " << periodBetweenSession << " seconds " << endl;
    }
}

void RTVideoStreamingSender::handleStopSessionNack(cMessage* msg)
{}

void RTVideoStreamingSender::openFileStream()
{
    EV_TRACE << "RTVideoStreamingSender::openFileStream()" << endl;

    _inputFileStream.open(fileName, std::ifstream::in);
    if (!_inputFileStream)
        throw cRuntimeError("Error opening data file '%s'", fileName);
}


void RTVideoStreamingSender::initializeVideoStream()
{
    char line[100];
    char unit[100];
    char description[100];

    // first line: fps unit description
    _inputFileStream.get(line, 100, '\n');

    float fps;
    sscanf(line, "%f %s %s", &fps, unit, description);
    _framesPerSecond = fps;

    _frameNumber = 0;

    // get new line character
    char c;
    _inputFileStream.get(c);

    // second line: initial delay unit description
    _inputFileStream.get(line, 100, '\n');
    _inputFileStream.get(c);
    float delay;
    sscanf(line, "%f %s %s", &delay, unit, description);

    _initialDelay = delay;
}
