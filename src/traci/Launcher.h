#ifndef LAUNCHER_H_NAC0X8JG
#define LAUNCHER_H_NAC0X8JG

#include <string>
#include"traci/TraCIApiProvider.h"

namespace traci
{

struct ServerEndpoint
{
    std::string hostname;
    int port;
    bool retry = false;
};

/**
 * launch TraCI Server an create appropriate TraCI API for the server.
 */
class Launcher: public TraCIApiProvider
{
public:

    virtual ~Launcher() = default;
    virtual ServerEndpoint launch() = 0;
};

} // namespace traci

#endif /* LAUNCHER_H_NAC0X8JG */

