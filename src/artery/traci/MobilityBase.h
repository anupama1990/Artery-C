#ifndef ARTERY_MOBILITYBASE_H_1SQMAVHF
#define ARTERY_MOBILITYBASE_H_1SQMAVHF

#include "artery/traci/ControllableVehicle.h"
#include "traci/LiteAPI.h"
#include "traci/MovingObjectSink.h"
#include "traci/VariableCache.h"
#include <omnetpp/clistener.h>
#include <memory>
#include <string>

namespace artery
{

class MobilityBase :
    public traci::MovingObjectSink, // for receiving updates from TraCI
    public ControllableObject // for controlling the any moving object via TraCI
{
public:
    // traci::MovingObjectSink interface
    void initializeObject(const traci::TraCIPosition&, traci::TraCIAngle, double speed) override;
    void updateObject(const traci::TraCIPosition&, traci::TraCIAngle, double speed) override;

    // traci::ControllableVehicle
    traci::MovingNodeController* getControllerBase() override;

    // generic signal for mobility state changes
    static omnetpp::simsignal_t stateChangedSignal;

protected:
    std::string mObjectId;
    traci::LiteAPI* mTraci = nullptr;
    traci::Boundary mNetBoundary;
    std::unique_ptr<traci::MovingNodeController> mController;

private:
    virtual void initialize(const Position&, Angle, double speed) = 0;
    virtual void update(const Position&, Angle, double speed) = 0;


};

} // namespace artery

#endif /* ARTERY_MOBILITYBASE_H_1SQMAVHF */
