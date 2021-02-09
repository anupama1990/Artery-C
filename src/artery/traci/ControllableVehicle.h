#ifndef CONTROLLABLEVEHICLE_H_DJ96H4LS
#define CONTROLLABLEVEHICLE_H_DJ96H4LS


#include "artery/traci/VehicleController.h"
#include "artery/traci/ControllableObject.h"

class ControllableVehicle: public ControllableObject
{
public:
    virtual ~ControllableVehicle() = default;
    virtual traci::VehicleController* getVehicleController();
};

#endif /* CONTROLLABLEVEHICLE_H_DJ96H4LS */

