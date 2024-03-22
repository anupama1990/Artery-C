#ifndef ARTERY_ANTENNAMOBILITY_H_BMWHZWNJ
#define ARTERY_ANTENNAMOBILITY_H_BMWHZWNJ

#include <inet/mobility/contract/IMobility.h>
#include <inet/mobility/single/AttachedMobility.h>
#include <inet/common/geometry/common/RotationMatrix.h>
#include <inet/common/geometry/Geometry_m.h>
#include <inet/common/INETDefs.h>
#include <inet/common/Simsignals.h>
#include <inet/common/Units.h>

namespace artery
{

class AntennaMobility : public inet::AttachedMobility
{
public:
    // inet::IMobility interface

//    double getMaxSpeed() const override;
//    inet::Coord getCurrentPosition() override;
//    inet::Coord getCurrentVelocity() override;
//    inet::Coord getCurrentAcceleration() override;
//    inet::EulerAngles getCurrentAngularPosition() override;
//    inet::EulerAngles getCurrentAngularVelocity() override;
//    inet::EulerAngles getCurrentAngularAcceleration() override;
//    inet::Coord getConstraintAreaMax() const override;
//    inet::Coord getConstraintAreaMin() const override;

    // omnetpp::cSimpleModule
    void initialize(int stage) override;
    int numInitStages() const override;

//private:
    inet::IMobility* mParentMobility = nullptr;
//    inet::Coord mOffsetCoord;
//    inet::EulerAngles mOffsetAngles;
//    inet::Rotation mOffsetRotation;
};

} // namespace artery
#endif /* ARTERY_ANTENNAMOBILITY_H_BMWHZWNJ */
