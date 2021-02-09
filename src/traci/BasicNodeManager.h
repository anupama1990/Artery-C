#ifndef BASICNODEMANAGER_H_XL6ISC2V
#define BASICNODEMANAGER_H_XL6ISC2V

#include "traci/Boundary.h"
#include "traci/NodeManager.h"
#include "traci/Listener.h"
#include "traci/SubscriptionManager.h"
#include <omnetpp/ccomponent.h>
#include <omnetpp/csimplemodule.h>
#include <functional>
#include <map>
#include <memory>
#include <string>

namespace traci
{

class LiteAPI;
class ModuleMapper;
class VehicleCache;
class MovingObjectSink;

class BasicNodeManager : public NodeManager, public Listener, public omnetpp::cSimpleModule
{
public:
    static const omnetpp::simsignal_t addNodeSignal;
    static const omnetpp::simsignal_t updateNodeSignal;
    static const omnetpp::simsignal_t removeNodeSignal;
    static const omnetpp::simsignal_t addVehicleSignal;
    static const omnetpp::simsignal_t updateVehicleSignal;
    static const omnetpp::simsignal_t removeVehicleSignal;

    LiteAPI* getLiteAPI() override { return m_api; }
    std::size_t getNumberOfNodes() const override;

    /**
     * VehicleObject wraps variable cache of a subscribed TraCI vehicle
     *
     * Each emitted vehicle update signal is accompanied by a VehicleObject (cObject details)
     */
    class VehicleObject : public NodeManager::MovingObject
    {
    public:
        virtual std::shared_ptr<VehicleCache> getCache() const = 0;
    };

protected:
    using NodeInitializer = std::function<void(omnetpp::cModule*)>;

    void initialize() override;
    void finish() override;

    virtual void addVehicle(const std::string&);
    virtual void removeVehicle(const std::string&);
    virtual void updateVehicle(const std::string&, MovingObjectSink*);
    virtual omnetpp::cModule* createModule(const std::string&, omnetpp::cModuleType*);
    virtual omnetpp::cModule* addNodeModule(const std::string&, omnetpp::cModuleType*, NodeInitializer&);
    virtual void removeNodeModule(const std::string&);
    virtual omnetpp::cModule* getNodeModule(const std::string&);
    virtual MovingObjectSink* getVehicleSink(omnetpp::cModule*);
    virtual MovingObjectSink* getVehicleSink(const std::string&);

private:
    void traciInit() override;
    void traciStep() override;
    void traciClose() override;

    LiteAPI* m_api;
    ModuleMapper* m_mapper;
    Boundary m_boundary;
    SubscriptionManager* m_subscriptions;
    unsigned m_nodeIndex;
    std::map<std::string, MovingObjectSink*> m_vehicles;
    std::string m_objectSinkModule;
};

} // namespace traci

#endif /* BASICNODEMANAGER_H_XL6ISC2V */

