#include "artery/inet/InetMobility.h"
#include "artery/traci/VehicleController.h"
#include <inet/common/ModuleAccess.h>
#include <inet/common/geometry/common/CanvasProjection.h>
#include <inet/features.h>
#include <cmath>

#ifdef WITH_VISUALIZERS
#   include <inet/visualizer/mobility/MobilityCanvasVisualizer.h>
#else
#   include <cstdio>
#endif

namespace artery
{

Define_Module(InetMobility)


int InetMobility::numInitStages() const
{
    return inet::INITSTAGE_PHYSICAL_ENVIRONMENT + 1;
}

void InetMobility::initialize(int stage)
{
    if (stage == inet::INITSTAGE_LOCAL) {
        mVisualRepresentation = inet::findModuleFromPar<cModule>(par("visualRepresentation"), this);
        mAntennaHeight = par("antennaHeight");
        WATCH(mObjectId);
        WATCH(mPosition);
        WATCH(mSpeed);
        WATCH(mOrientation);
    } else if (stage == inet::INITSTAGE_PHYSICAL_ENVIRONMENT) {
        if (mVisualRepresentation) {
            auto visualizationTarget = mVisualRepresentation->getParentModule();
            mCanvasProjection = inet::CanvasProjection::getCanvasProjection(visualizationTarget->getCanvas());
        }
        emit(MobilityBase::stateChangedSignal, this);
    }
}

void InetMobility::initializeSink(traci::LiteAPI* api, const std::string& id, const traci::Boundary& boundary, std::shared_ptr<traci::VariableCache> cache)
{
    ASSERT(api);
    ASSERT(cache);
    ASSERT(cache->getId() == id);
    ASSERT(&cache->getLiteAPI() == api);
    mTraci = api;
    mObjectId= id;
    mNetBoundary = boundary;

    const auto& max = mNetBoundary.upperRightPosition();
    mConstrainedAreaMax = inet::Coord { max.x, max.y, max.z };

    const auto& min = mNetBoundary.lowerLeftPosition();
    mConstrainedAreaMin = inet::Coord { min.x, min.y, min.z };

    std::shared_ptr<traci::VehicleCache> vehicleCache =  std::dynamic_pointer_cast<traci::VehicleCache> (cache);
    if (!vehicleCache){
        //todo
    }
    mController.reset(new traci::VehicleController(vehicleCache));
}

double InetMobility::getMaxSpeed() const
{
    return NaN;
}

const inet::Coord& InetMobility::getCurrentPosition()
{
    return mPosition;
}

const inet::Coord& InetMobility::getCurrentVelocity()
{
    return mSpeed;
}

const inet::Coord& InetMobility::getCurrentAcceleration()
{
    return inet::Coord::NIL;
}

const inet::Quaternion& InetMobility::getCurrentAngularPosition()
{
    return mOrientation;
}

const inet::Quaternion& InetMobility::getCurrentAngularVelocity()
{
    return inet::Quaternion::NIL;
}

const inet::Quaternion& InetMobility::getCurrentAngularAcceleration()
{
    return inet::Quaternion::NIL;
}

const inet::Coord& InetMobility::getConstraintAreaMax() const
{
    return mConstrainedAreaMax;
}

const inet::Coord& InetMobility::getConstraintAreaMin() const
{
    return mConstrainedAreaMin;
}

void InetMobility::initialize(const Position& pos, Angle heading, double speed)
{
    using boost::units::si::meter;
    const double heading_rad = heading.radian();
    const inet::Coord direction { cos(heading_rad), -sin(heading_rad) };
    mPosition = inet::Coord { pos.x / meter, pos.y / meter, mAntennaHeight };
    mSpeed = direction * speed;
    inet::EulerAngles angles;
    angles.alpha = inet::units::values::rad(-heading_rad);
    mOrientation = inet::Quaternion(angles);
}

void InetMobility::update(const Position& pos, Angle heading, double speed)
{
    initialize(pos, heading, speed);
    ASSERT(inet::IMobility::mobilityStateChangedSignal == MobilityBase::stateChangedSignal);
    emit(MobilityBase::stateChangedSignal, this);
}

void InetMobility::refreshDisplay() const
{
    // following code is taken from INET's MobilityBase::refreshDisplay
    if (mVisualRepresentation) {
        auto position = mCanvasProjection->computeCanvasPoint(mPosition);
        char buf[32];
        snprintf(buf, sizeof(buf), "%lf", position.x);
        buf[sizeof(buf) - 1] = 0;
        mVisualRepresentation->getDisplayString().setTagArg("p", 0, buf);
        snprintf(buf, sizeof(buf), "%lf", position.y);
        buf[sizeof(buf) - 1] = 0;
        mVisualRepresentation->getDisplayString().setTagArg("p", 1, buf);
    }
}

} // namespace artery
