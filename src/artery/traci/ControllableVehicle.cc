/*
 * ControllableObject.cc
 *
 *  Created on: Aug 7, 2020
 *      Author: sts
 */

#include <artery/traci/ControllableVehicle.h>

traci::VehicleController* ControllableVehicle::getVehicleController(){
    return getController<traci::VehicleController>();
}
