#ifndef MOVINGOBJECTSINK_H_B1HZNP6Q
#define MOVINGOBJECTSINK_H_B1HZNP6Q

#include "traci/API.h"
#include <memory>

namespace traci
{

class LiteAPI;
class VariableCache;

class MovingObjectSink
{
public:
    virtual void initializeSink(LiteAPI*, const std::string& id, const Boundary&, std::shared_ptr<VariableCache> cache) = 0;
    virtual void initializeObject(const TraCIPosition& angel, TraCIAngle angle, double speed) = 0;
    virtual void updateObject(const TraCIPosition& pos, TraCIAngle angle, double speed) = 0;
    virtual ~MovingObjectSink() = default;
};

} // namespace traci

#endif /* MOVINGOBJECTSINK_H_B1HZNP6Q */

