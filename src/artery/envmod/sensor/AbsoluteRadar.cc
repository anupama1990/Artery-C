//
// Created by rosk on 01.03.19.
//

#include "AbsoluteRadar.h"

namespace artery
{

Define_Module(AbsoluteRadar);

void AbsoluteRadar::initialize()
{
    RadarSensor::initialize();
    mRadarConfig.sensorPosition = SensorPosition::FRONT;
    mRadarConfig.egoID = getEgoId();
    mRadarConfig.sensorID = getId();
}


const std::string& AbsoluteRadar::getSensorCategory() const
{
    static const std::string category = "AbsoluteRadar";
    return category;
}


} // namespace artery
