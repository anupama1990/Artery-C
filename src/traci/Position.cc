#include "traci/Position.h"
#include <omnetpp.h>
using namespace omnetpp;
namespace traci
{

artery::Position position_cast(const Boundary& boundary, const TraCIPosition& pos)
{
    const double x = pos.x - boundary.lowerLeftPosition().x;
    const double y = boundary.upperRightPosition().y - pos.y;

    EV<<"traci::position actual x: "<<pos.x <<"boundary left x: "<<boundary.lowerLeftPosition().x<<endl;
    EV<<"traci::position actual y: "<<pos.y <<"boundary right y: "<<boundary.upperRightPosition().y<<endl;

    EV<<"traci::position x= "<<x<<endl;
    EV<<"traci::position y= "<<y<<endl;

    return artery::Position(x, y);
}

TraCIPosition position_cast(const Boundary& boundary, const artery::Position& pos)
{
    const double x = pos.x.value() + boundary.lowerLeftPosition().x;
    const double y = boundary.upperRightPosition().y - pos.y.value();
    TraCIPosition tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = 0.0;
    return tmp;
}

} // namespace traci
