/*
 * Artery V2X Simulation Framework
 * Copyright 2019 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "artery/utility/Identity.h"
#include <omnetpp/regmacros.h>

namespace artery
{

// Identity has no source file yet
Register_Class(Identity)

using namespace omnetpp;

const simsignal_t Identity::changeSignal = cComponent::registerSignal("IdentityChanged");

bool Identity::update(const Identity& update, long changes)
{
    bool changed = false;

    if (changes & ChangeTraCI) {
        traci = update.traci;
        changed = true;
    }

    if (changes & ChangeStationId) {
        application = update.application;
        changed = true;
    }

    if (changes & ChangeGeoNetAddress) {
        geonet = update.geonet;
        changed = true;
    }

    return changed;
}

} // namespace artery
