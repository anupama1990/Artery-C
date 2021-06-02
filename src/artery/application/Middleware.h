/*
* Artery V2X Simulation Framework
* Copyright 2014-2018 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_MIDDLEWARE_H_
#define ARTERY_MIDDLEWARE_H_

#include "artery/application/MiddlewareBase.h"
#include "artery/application/LocalDynamicMap.h"
#include "artery/application/StationType.h"
#include "artery/application/Timer.h"
#include <omnetpp/simtime.h>
#include <vanetza/btp/data_request.hpp>
#include <vanetza/btp/port_dispatcher.hpp>
#include <map>
#include <memory>

namespace artery
{

// forward declarations
class ItsG5BaseService;
class Router;

/**
 * Middleware providing a runtime context for services.
 */
class Middleware : public MiddlewareBase
{
    public:
        typedef uint16_t port_type;

        Middleware();
        ~Middleware();

        port_type getPortNumber(const ItsG5BaseService*) const;
        const StationType& getStationType() const { return mStationType; }

        vanetza::geonet::TransportInterface& getTransportInterface();
        void requestTransmission(const vanetza::btp::DataRequestB&, std::unique_ptr<vanetza::DownPacket>);

    protected:
        // cSimpleModule
        void initialize(int stage) override;
        void handleMessage(omnetpp::cMessage* msg) override;

        void setStationType(const StationType&);

    private:
        void updateServices();
        void initializeServices(int stage);

        omnetpp::SimTime mUpdateInterval;
        omnetpp::cMessage* mUpdateMessage = nullptr;
        Timer mTimer;
        LocalDynamicMap mLocalDynamicMap;
        StationType mStationType;
        Router* mRouter = nullptr;
        vanetza::btp::PortDispatcher mBtpPortDispatcher;
        std::map<ItsG5BaseService*, port_type> mServices;
};

} // namespace artery

#endif
