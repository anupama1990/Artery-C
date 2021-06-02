//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#pragma once

#include "artery/application/IdentityProvider.h"
#include "artery/application/Facilities.h"
#include "artery/utility/Identity.h"
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/simtime.h>

namespace artery {

class MiddlewareBase : public omnetpp::cSimpleModule, public omnetpp::cListener, public IdentityProvider {
public:
    virtual ~MiddlewareBase() = default;
    Facilities& getFacilities() { return mFacilities; }
    const Facilities& getFacilities() const { return mFacilities; }

    virtual const Identity& getIdentity() const override { return mIdentity; }

protected:
    int numInitStages() const override;
    void initialize(int stage) override;
    using omnetpp::cIListener::finish;  // [-Woverloaded-virtual]
    void finish() override;
    // cListener
    virtual void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, long, omnetpp::cObject*) override;

    virtual omnetpp::cModule* findHost();

protected:

    Identity mIdentity;
    Facilities mFacilities;
};

} /* namespace artery */

