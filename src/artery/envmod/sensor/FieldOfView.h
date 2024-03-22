#ifndef ENVMOD_FIELDOFVIEW_H_RW4CQUCV
#define ENVMOD_FIELDOFVIEW_H_RW4CQUCV

#include <boost/units/quantity.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/angle/degrees.hpp>

struct FieldOfView
{
    boost::units::quantity<boost::units::si::length> range;
    boost::units::quantity<boost::units::degree::plane_angle> angle;
};

#endif /* ENVMOD_FIELDOFVIEW_H_RW4CQUCV */
