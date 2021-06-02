/*
 * MovingNodeDataProvider.h
 *
 *  Created on: Aug 5, 2020
 *      Author: vm-sts
 */

#ifndef ARTERY_APPLICATION_MOVINGNODEDATAPROVIDER_H_
#define ARTERY_APPLICATION_MOVINGNODEDATAPROVIDER_H_

#include <artery/traci/MovingNodeController.h>
#include "artery/utility/Geometry.h"
#include <omnetpp/simtime.h>
#include <boost/circular_buffer.hpp>
#include <boost/units/systems/si/angular_acceleration.hpp>
#include <vanetza/geonet/station_type.hpp>
#include <vanetza/units/acceleration.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/velocity.hpp>
#include <vanetza/units/angular_velocity.hpp>
#include <vanetza/units/curvature.hpp>
#include <cstdint>
#include <map>

namespace artery
{

class MovingNodeDataProvider {
public:
    using StationType = vanetza::geonet::StationType;

    MovingNodeDataProvider();
    MovingNodeDataProvider(uint32_t id);
    virtual ~MovingNodeDataProvider();

    // prevent inadvertent VDP copies
    MovingNodeDataProvider(const MovingNodeDataProvider&) = delete;
    MovingNodeDataProvider& operator=(const MovingNodeDataProvider&) = delete;

    void update(const traci::MovingNodeController* controller);
    omnetpp::SimTime updated() const { return mLastUpdate; }

    uint32_t station_id() const { return mStationId; }
    const Position& position() const { return mPosition; }
    vanetza::units::GeoAngle longitude() const { return mGeoPosition.longitude; } // positive for east
    vanetza::units::GeoAngle latitude() const { return mGeoPosition.latitude; } // positive for north
    vanetza::units::Velocity speed() const { return mSpeed; }
    vanetza::units::Acceleration acceleration() const { return mAccel; }
    vanetza::units::Angle heading() const { return mHeading; } // degree from north, clockwise
    vanetza::units::AngularVelocity yaw_rate() const { return mYawRate; } // left turn positive
    vanetza::units::Curvature curvature() const { return mCurvature; } // 1/m radius, left turn positive
    double confidence() const { return mConfidence; } // percentage value

    void setStationType(StationType);
    StationType getStationType() const;

private:
    typedef boost::units::quantity<boost::units::si::angular_acceleration> AngularAcceleration;
    void calculateCurvature();
    void calculateCurvatureConfidence();
    double mapOntoConfidence(AngularAcceleration) const;

    uint32_t mStationId;
    StationType mStationType;
    Position mPosition;
    GeoPosition mGeoPosition;
    vanetza::units::Velocity mSpeed;
    vanetza::units::Acceleration mAccel;
    vanetza::units::Angle mHeading;
    vanetza::units::AngularVelocity mYawRate;
    vanetza::units::Curvature mCurvature;
    double mConfidence;
    omnetpp::SimTime mLastUpdate;
    boost::circular_buffer<vanetza::units::Curvature> mCurvatureOutput;
    boost::circular_buffer<AngularAcceleration> mCurvatureConfidenceOutput;
    vanetza::units::AngularVelocity mCurvatureConfidenceInput;
    static const std::map<AngularAcceleration, double> mConfidenceTable;
};

} /* namespace artery */

#endif /* ARTERY_APPLICATION_MOVINGNODEDATAPROVIDER_H_ */
