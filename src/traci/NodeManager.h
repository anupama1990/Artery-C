#ifndef NODEMANAGER_H_XH1HSC4Z
#define NODEMANAGER_H_XH1HSC4Z

#include <cstdint>
#include <omnetpp/ccomponent.h>
#include "traci/Position.h"
#include "traci/Angle.h"


namespace traci
{

class LiteAPI;

class ITraciNodeVisitor {
public:
    virtual ~ITraciNodeVisitor() = default;
    virtual void visitNode(const std::string& traciNodeId, omnetpp::cModule* mod) = 0;
};


class NodeManager
{
public:

    /**
     * MovingObject wraps variable cache of a subscribed TraCI vehicle
     *
     * Each emitted object update signal is accompanied by a MocingObject (cObject details)
     *
     * Subclass with  correct cache object.
     */
    class MovingObject : public omnetpp::cObject
    {
    public:
        virtual const TraCIPosition& getPosition() const = 0;
        virtual TraCIAngle getHeading() const = 0;
        virtual double getSpeed() const = 0;
    };

    /**
     * Number of nodes currently managed by this manager
     * \return number of nodes
     */
    virtual std::size_t getNumberOfNodes() const = 0;

    /**
     * Access to lite API interface
     * \return API object
     */
    virtual LiteAPI* getLiteAPI() = 0;

    virtual ~NodeManager() = default;

    virtual void visit(ITraciNodeVisitor *visitor) const;

protected:
    std::map<std::string, omnetpp::cModule*> m_nodes;

};

} // namespace traci

#endif /* NODEMANAGER_H_XH1HSC4Z */

