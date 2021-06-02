/*
 * IdentityProvider.h
 *
 *  Created on: Dec 10, 2020
 *      Author: sts
 */

#pragma once

#include "artery/utility/Identity.h"

namespace artery
{

class IdentityProvider {
public:
    virtual ~IdentityProvider() = default;
    virtual const Identity& getIdentity() const = 0;

};
}



