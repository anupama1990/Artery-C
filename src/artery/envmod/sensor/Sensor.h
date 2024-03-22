/*
 * Artery V2X Simulation Framework
 * Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ENVMOD_SENSOR_H_
#define ENVMOD_SENSOR_H_

#include "artery/envmod/sensor/FieldOfView.h"
#include "artery/envmod/sensor/SensorVisualizationConfig.h"
#include "artery/envmod/sensor/SensorPosition.h"
#include <omnetpp/csimplemodule.h>
#include <string>

namespace artery
{

class Sensor : public omnetpp::cSimpleModule
{
public:
    virtual ~Sensor() = default;
    virtual void measurement() = 0;
    virtual void setVisualization(const SensorVisualizationConfig&) = 0;
    virtual const FieldOfView* getFieldOfView() const = 0;
    virtual SensorPosition position() const = 0;
    virtual omnetpp::SimTime getValidityPeriod() const = 0;
    virtual const std::string& getSensorCategory() const = 0;
};

} // namespace artery

#endif /* ENVMOD_SENSOR_H_ */
