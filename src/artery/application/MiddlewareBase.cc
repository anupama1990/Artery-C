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

#include <artery/application/MiddlewareBase.h>
#include "artery/utility/InitStages.h"
#include "inet/common/ModuleAccess.h"
#include "artery/utility/IdentityRegistry.h"

namespace artery {

Define_Module(MiddlewareBase);


int MiddlewareBase::numInitStages() const {
    return InitStages::Total;
}

void MiddlewareBase::initialize(int stage) {
    if (stage == InitStages::Prepare) {
        mIdentity.host = findHost();
        mIdentity.host->subscribe(Identity::changeSignal, this);}
    else if (stage == InitStages::Self) {
        mFacilities.register_const(&mIdentity);
    } else if (stage == InitStages::Propagate) {
        emit(artery::IdentityRegistry::updateSignal, &mIdentity);
    }
}

void MiddlewareBase::finish() {
    emit(artery::IdentityRegistry::removeSignal, &mIdentity);
}

// cListener
void MiddlewareBase::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signal, long changes, omnetpp::cObject* obj){
    if (signal == Identity::changeSignal) {
        auto identity = omnetpp::check_and_cast<Identity*>(obj);
        if (mIdentity.update(*identity, changes)) {
            emit(artery::IdentityRegistry::updateSignal, &mIdentity);
        }
    }
}

omnetpp::cModule* MiddlewareBase::findHost(){
    return inet::getContainingNode(this);
}

} /* namespace artery */
