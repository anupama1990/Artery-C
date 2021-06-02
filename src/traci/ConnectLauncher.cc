#include "traci/ConnectLauncher.h"

namespace traci
{

Define_Module(ConnectLauncher)

void ConnectLauncher::initialize()
{
    m_endpoint.hostname = par("hostname").stringValue();
    m_endpoint.port = par("port");
}

ServerEndpoint ConnectLauncher::launch()
{
    return m_endpoint;
}

std::pair<API*, LiteAPI*> ConnectLauncher::createAPI(){
    API* api = new API();
    LiteAPI* liteApi = new LiteAPI(*api);
    return std::make_pair(api, liteApi);
}

} // namespace traci
