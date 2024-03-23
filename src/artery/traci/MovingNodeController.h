/*
 * NodeController.h
 *
 *  Created on: Aug 5, 2020
 *      Author: vm-sts
 */

#ifndef ARTERY_TRACI_MOVINGNODECONTROLLER_H_
#define ARTERY_TRACI_MOVINGNODECONTROLLER_H_

#include "artery/utility/Geometry.h"

#include <vanetza/units/acceleration.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/length.hpp>
#include <vanetza/units/velocity.hpp>
#include <string>

namespace traci {

class MovingNodeController {
public:
    using Acceleration = vanetza::units::Acceleration;
    using Length = vanetza::units::Length;
    using Velocity = vanetza::units::Velocity;

    MovingNodeController();
    virtual ~MovingNodeController();

    virtual const std::string& getNodeId() const = 0;
    virtual std::string getTypeId() const = 0;
    virtual const std::string getNodeClass() const = 0;

    virtual artery::Position getPosition() const= 0;
    virtual artery::GeoPosition getGeoPosition() const= 0;
    virtual artery::Angle getHeading() const= 0;
    virtual Velocity getSpeed() const= 0;
    virtual Velocity getMaxSpeed() const= 0;
    virtual void setMaxSpeed(Velocity)= 0;
    virtual void setSpeed(Velocity)= 0;

    virtual Length getLength() const = 0;
    virtual Length getWidth() const = 0;
};



} /* namespace traci */

#endif /* ARTERY_TRACI_MOVINGNODECONTROLLER_H_ */
