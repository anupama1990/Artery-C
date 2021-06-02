#ifndef CONNECTLAUNCHER_H_7XR3C68H
#define CONNECTLAUNCHER_H_7XR3C68H

#include "traci/Launcher.h"
#include <omnetpp/csimplemodule.h>

namespace traci
{

class ConnectLauncher : public Launcher, public omnetpp::cSimpleModule
{
public:
    void initialize() override;
    ServerEndpoint launch() override;
    virtual std::pair<API*, LiteAPI*> createAPI() override;

protected:
    ServerEndpoint m_endpoint;
};

} // namespace traci

#endif /* CONNECTLAUNCHER_H_7XR3C68H */
