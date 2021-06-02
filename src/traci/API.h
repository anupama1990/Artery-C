#ifndef API_H_HBQVASFR
#define API_H_HBQVASFR

#include "traci/sumo/utils/traci/TraCIAPI.h"
#include "traci/Angle.h"
#include "traci/Boundary.h"
#include "traci/GeoPosition.h"
#include "traci/Position.h"
#include "traci/Time.h"
#include <omnetpp/simtime.h>

namespace traci
{

struct ServerEndpoint;

class API : public TraCIAPI
{
public:
    virtual ~API() = default;
    using Version = std::pair<int, std::string>;
    Version getVersion() const;
    virtual TraCIGeoPosition convertGeo(const TraCIPosition&) const;
    virtual TraCIPosition convert2D(const TraCIGeoPosition&) const;

    void connect(const ServerEndpoint&);
};

} // namespace traci

#endif /* API_H_HBQVASFR */

