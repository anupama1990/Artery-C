/*
 * TraCIApiProvider.h
 *
 *  Created on: Aug 6, 2020
 *      Author: sts
 */

#pragma once

#include "traci/API.h"
#include "traci/LiteAPI.h"

namespace traci {

/**
 * create appropriate TraCI API
 */
class TraCIApiProvider {

public:
    virtual std::pair<API*, LiteAPI*> createAPI() = 0;

};

} /* namespace traci */

