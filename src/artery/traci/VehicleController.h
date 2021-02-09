#ifndef VEHICLECONTROLLER_H_AXBS5NQM
#define VEHICLECONTROLLER_H_AXBS5NQM

#include <artery/traci/MovingNodeController.h>
#include "artery/traci/VehicleType.h"
#include "artery/traci/MovingNodeController.h"
#include "traci/LiteAPI.h"
#include "traci/VariableCache.h"

namespace traci
{

class VehicleCache;

class VehicleController: public MovingNodeController
{
public:
    using Acceleration = vanetza::units::Acceleration;
    using Length = vanetza::units::Length;
    using Velocity = vanetza::units::Velocity;

    VehicleController(const std::string& id, traci::LiteAPI&);
    VehicleController(std::shared_ptr<VehicleCache> cache);

    const std::string& getVehicleId() const;
    const std::string& getNodeId() const override;
    std::string getTypeId() const override;
    const VehicleType& getVehicleType() const;
    const std::string getVehicleClass() const;
    const std::string getNodeClass() const override;

    artery::Position getPosition() const override;
    artery::GeoPosition getGeoPosition() const override;
    artery::Angle getHeading() const override;
    Velocity getSpeed() const override;
    Velocity getMaxSpeed() const override;
    void setMaxSpeed(Velocity) override;
    void setSpeed(Velocity) override;
    void setSpeedFactor(double);

    Length getLength() const override;
    Length getWidth() const override;

    void changeTarget(const std::string& edge);

    traci::LiteAPI& getLiteAPI() { return m_api; }
    const traci::LiteAPI& getLiteAPI() const { return m_api; }

private:
    VehicleController(const std::string& id, traci::LiteAPI& api, std::shared_ptr<VehicleCache> cache);

    std::string m_id;
    traci::LiteAPI& m_api;
    traci::Boundary m_boundary;
    VehicleType m_type;
    std::shared_ptr<VehicleCache> m_cache;
};

} // namespace traci

#endif /* VEHICLECONTROLLER_H_AXBS5NQM */

