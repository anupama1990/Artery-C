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

#ifndef NRCHANNELMODEL_3GPP38_901_H_
#define NRCHANNELMODEL_3GPP38_901_H_

#include "stack/phy/ChannelModel/NRChannelModel.h"


// NRChannelModel_3GPP38_901
//
// This channel model implements path loss, LOS probability and shadowing according to
// the following specifications:
//     3GPP TR 38.901, "Study on channel model for frequencies from 0.5 to 100 GHz", v16.1.0, December 2019
//
class NRChannelModel_3GPP38_901 : public NRChannelModel
{

public:
    virtual void initialize(int stage);

    /*
     * Compute LOS probability (taken from TR TR 38.901)
     *
     * @param d between UE and gNodeB
     * @param nodeid mac node id of UE
     */
    void computeLosProbability(double d, MacNodeId nodeId);

    /*
     * Compute the building penetration loss for indoor UE
     * See section 7.4.3 of TR 38.901
     *
     * @param 3d_distance between UE and gNodeB (3D)
     */
    virtual double computePenetrationLoss(double threeDimDistance);

    /*
     * Compute the path-loss attenuation according to the selected scenario
     *
     * @param 3d_distance between UE and gNodeB (3D)
     * @param 2d_distance between UE and gNodeB (2D)
     * @param los line-of-sight flag
     */
    virtual double computePathLoss(double threeDimDistance, double twoDimDistance, bool los);

    /*
     * UMa path loss model (taken from TR 38.901)
     *
     * @param distance between UE and gNodeB
     * @param los line-of-sight flag
     */
    double computeUrbanMacro(double threeDimDistance, double twoDimDistance, bool los);

    /*
     * UMi path loss model (taken from TR 38.901)
     *
     * @param distance between UE and gNodeB
     * @param los line-of-sight flag
     */
    double computeUrbanMicro(double threeDimDistance, double twoDimDistance, bool los);

    /*
     * UMa path loss model (taken from TR 38.901)
     *
     * @param distance between UE and gNodeB
     * @param los line-of-sight flag
     */
    double computeRuralMacro(double threeDimDistance, double twoDimDistance, bool los);

    /*
     * InH path loss model (taken from TR 38.901)
     *
     * @param distance between UE and gNodeB
     * @param los line-of-sight flag
     */
    double computeIndoor(double threeDimDistance, double twoDimDistance, bool los);

    /*
     * compute std deviation of shadowing according to scenario and visibility
     *
     * @param distance between UE and gNodeB
     * @param nodeid mac node id of UE
     */
    double getStdDev(bool dist, MacNodeId nodeId);

    /*
     * Compute shadowing
     *
     * @param distance between UE and gNodeB
     * @param nodeid mac node id of UE
     * @param speed speed of UE
     */
    virtual double computeShadowing(double sqrDistance, MacNodeId nodeId, double speed, bool cqiDl);

};

#endif /* NRChannelModel_3GPP38_901_H_ */
