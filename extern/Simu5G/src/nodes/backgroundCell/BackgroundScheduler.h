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

#ifndef __BACKGROUNDSCHEDULER_H_
#define __BACKGROUNDSCHEDULER_H_

#include <omnetpp.h>
#include "common/LteCommon.h"
#include "common/binder/Binder.h"
#include "nodes/backgroundCell/BackgroundCellChannelModel.h"
#include "stack/backgroundTrafficGenerator/BackgroundTrafficManager.h"
#include "stack/mac/scheduler/LteScheduler.h"  // for SortedDesc

typedef SortedDesc<MacCid, unsigned int> ScoreDesc;
typedef std::priority_queue<ScoreDesc> ScoreList;

class BackgroundScheduler : public omnetpp::cSimpleModule, public cListener
{
    // base station coordinates
    inet::Coord pos_;

    // id among all the background cells
    int id_;

    // tx power
    double txPower_;

    // tx direction
    TxDirectionType txDirection_;

    // tx angle
    double txAngle_;

    // if true, this is a NR base station
    // if false, this is a LTE base station
    bool isNr_;

    // numerology
    unsigned int numerologyIndex_;

    // carrier frequency
    double carrierFrequency_;

    // number of logical bands
    unsigned int numBands_;

    // reference to the binder
    Binder* binder_;

    // reference to the background traffic manager - one per carrier
    BackgroundTrafficManager* bgTrafficManager_;

    // reference to the channel model for this background base station
    BackgroundCellChannelModel* bgChannelModel_;

    // TTI self message
    omnetpp::cMessage* ttiTick_;
    double ttiPeriod_;


    /*** ALLOCATION MANAGEMENT ***/

    // Current and previous band occupation status (both for UL and DL). Used for interference computation
    BandStatus bandStatus_[2];
    BandStatus prevBandStatus_[2];

    // for the UL, we need to store which UE uses which block
    std::vector<int> ulBandAllocation_;
    std::vector<int> ulPrevBandAllocation_;


    // update the band status. Called at each TTI (not used for FULL_ALLOC)
    virtual void updateAllocation(Direction dir);
    // move the current status in the prevBandStatus structure and reset the former
    void resetAllocation(Direction dir);
    /*****************************/

    // statistics
    simsignal_t bgAvgServedBlocksDl_;
    simsignal_t bgAvgServedBlocksUl_;

  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return inet::INITSTAGE_LOCAL+2; }
    virtual void handleMessage(omnetpp::cMessage *msg) override;

  public:

    // This module is subscribed to position changes.
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *) override;

    BackgroundCellChannelModel* getChannelModel() { return bgChannelModel_; }

    const inet::Coord getPosition() const { return pos_; }

    int getId() const { return id_; }

    double getTtiPeriod() const {return ttiPeriod_; }

    double getTxPower() const { return txPower_; }

    TxDirectionType getTxDirection() const { return txDirection_; }

    double getTxAngle() const { return txAngle_; }

    bool isNr() const { return isNr_; }

    unsigned int getNumBands() const { return numBands_; }

    int getBandStatus(int band, Direction dir) { return bandStatus_[dir].at(band); }
    int getPrevBandStatus(int band, Direction dir) { return prevBandStatus_[dir].at(band); }

    TrafficGeneratorBase* getBandInterferingUe(int band);
    TrafficGeneratorBase* getPrevBandInterferingUe(int band);
};

#endif
