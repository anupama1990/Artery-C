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

#ifndef APPS_MEC_MEAPPS_MEBG_APP_H_
#define APPS_MEC_MEAPPS_MEBG_APP_H_

#include "apps/mec/MecApps/MecAppBase.h"
#include "inet/common/lifecycle/NodeStatus.h"

using namespace omnetpp;

class MecRequestBackgroundApp : public MecAppBase
{
protected:

    inet::NodeStatus *nodeStatus = nullptr;
    int numberOfApplications_;    // requests to send in this session
    cMessage *burstTimer;
    cMessage *burstPeriod;
    bool      burstFlag;
    cMessage *sendBurst;

    double lambda; // it is the mean, not the rate
    inet::TcpSocket* serviceSocket_;
    inet::TcpSocket* mp1Socket_;

    HttpBaseMessage* mp1HttpMessage;
    HttpBaseMessage* serviceHttpMessage;

    virtual void handleSelfMessage(cMessage *msg) override;

    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;


    virtual void handleHttpMessage(int connId) override;
    virtual void handleServiceMessage(int connId) override;
    virtual void handleMp1Message(int connId) override;

    virtual void handleUeMessage(omnetpp::cMessage *msg) override {};


    virtual void established(int connId) override;

    virtual void sendRequest();

    virtual void finish() override;

   public:
     MecRequestBackgroundApp();
     virtual ~MecRequestBackgroundApp();
};

#endif /* APPS_MEC_MEAPPS_MEBGAPP_H_ */
