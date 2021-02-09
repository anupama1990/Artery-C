#ifndef ARTERY_INETMOBILITY_H_SKZPGILS
#define ARTERY_INETMOBILITY_H_SKZPGILS

#include "artery/traci/MobilityBase.h"
#include <inet/mobility/contract/IMobility.h>
#include <omnetpp/csimplemodule.h>

namespace inet { class CanvasProjection; }

namespace artery
{

class InetMobility : public inet::IMobility, public MobilityBase, public ControllableVehicle , public omnetpp::cSimpleModule
{
public:
    // artery::MobilityBase
    void initializeSink(traci::LiteAPI*, const std::string& id, const traci::Boundary&, std::shared_ptr<traci::VariableCache> cache) override;

    // inet::IMobility interface
    virtual int getId() const override { return cSimpleModule::getId(); }
    virtual double getMaxSpeed() const override;
    virtual const inet::Coord& getCurrentPosition() override;
    virtual const inet::Coord& getCurrentVelocity() override;
    virtual const inet::Coord& getCurrentAcceleration() override;
    virtual const inet::Quaternion& getCurrentAngularPosition() override;
    virtual const inet::Quaternion& getCurrentAngularVelocity() override;
    virtual const inet::Quaternion& getCurrentAngularAcceleration() override;
    virtual const inet::Coord& getConstraintAreaMax() const override;
    virtual const inet::Coord& getConstraintAreaMin() const override;

    //
    traci::MovingNodeController* getControllerBase() override {
        return MobilityBase::getControllerBase();
    }

    // omnetpp::cSimpleModule
    void initialize(int stage) override;
    int numInitStages() const override;

protected:
    void refreshDisplay() const override;

private:
    void initialize(const Position& pos, Angle heading, double speed) override;
    void update(const Position& pos, Angle heading, double speed) override;

    inet::Coord mPosition;
    inet::Coord mSpeed;
    inet::Quaternion mOrientation;
    inet::Coord mConstrainedAreaMin;
    inet::Coord mConstrainedAreaMax;

    double mAntennaHeight = 0.0;
    omnetpp::cModule* mVisualRepresentation = nullptr;
    const inet::CanvasProjection* mCanvasProjection = nullptr;
};

} // namespace artery

#endif /* ARTERY_INETMOBILITY_H_SKZPGILS */
