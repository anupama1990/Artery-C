//
// Created by rosk on 01.03.19.
//

#ifndef ARTERY_ABSOLUTERADAR_H
#define ARTERY_ABSOLUTERADAR_H

#include "artery/envmod/sensor/RadarSensor.h"

#include "artery/envmod/sensor/SensorConfiguration.h"
#include "artery/envmod/sensor/SensorDetection.h"
#include "artery/envmod/sensor/BaseSensor.h"
#include <omnetpp/ccanvas.h>
#include <memory>

namespace artery
{

    class AbsoluteRadar : public RadarSensor {
        public:
            void initialize() override;
            const std::string& getSensorCategory() const override;
};


}

#endif //ARTERY_ABSOLUTERADAR_H
