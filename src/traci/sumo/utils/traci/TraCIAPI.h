/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIAPI.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
#ifndef TraCIAPI_h
#define TraCIAPI_h


// ===========================================================================
// included modules
// ===========================================================================

#include <vector>
#include <limits>
#include <string>
#include <sstream>
#include <iomanip>
#include <foreign/tcpip/socket.h>
#include <libsumo/TraCIConstants.h>
#include <libsumo/TraCIDefs.h>

// ===========================================================================
// global definitions
// ===========================================================================
#define DEFAULT_VIEW "View #0"
#define PRECISION 2

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCIAPI
 * @brief C++ TraCI client API implementation
 */


class TraCIAPI {
public:
    /** @brief Constructor
     */
    TraCIAPI();

    /// @brief Destructor
    ~TraCIAPI();

    /// @name Connection handling
    /// @{

    /** @brief Connects to the specified SUMO server
     * @param[in] host The name of the host to connect to
     * @param[in] port The port to connect to
     * @exception tcpip::SocketException if the connection fails
     */
    void connect(const std::string& host, int port);

    /// @brief set priority (execution order) for the client
    void setOrder(int order);

    /// @brief ends the simulation and closes the connection
    void close();
    /// @}

    /// @brief Advances by one step (or up to the given time)
    void simulationStep(double time = 0);

    /// @brief Let sumo load a simulation using the given command line like options.
    void load(const std::vector<std::string>& args);

    /// @name Atomar getter
    /// @{
    int getUnsignedByte(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    int getByte(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    int getInt(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    double getDouble(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    libsumo::TraCIPositionVector getPolygon(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    libsumo::TraCIPosition getPosition(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    libsumo::TraCIPosition getPosition3D(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    std::string getString(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    std::vector<std::string> getStringVector(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    libsumo::TraCIColor getColor(int cmd, int var, const std::string& id, tcpip::Storage* add = 0);
    /// @}

    const tcpip::Storage& getCommandStorage() const {
        return myOutput;
    }

    /** @class TraCIScopeWrapper
     * @brief An abstract interface for accessing type-dependent values
     *
     * Must be derived by interfaces which implement access methods to certain object types
     */
    class TraCIScopeWrapper {
    public:
        /** @brief Constructor
         * @param[in] parent The parent TraCI client which offers the connection
         */
        TraCIScopeWrapper(TraCIAPI& parent, int cmdGetID, int cmdSetID, int subscribeID, int contextSubscribeID) :
            myParent(parent),
            myCmdGetID(cmdGetID),
            myCmdSetID(cmdSetID),
            mySubscribeID(subscribeID),
            myContextSubscribeID(contextSubscribeID) {
        }

        /// @brief Destructor
        virtual ~TraCIScopeWrapper() {}

        /// @brief retrieve generic paramter
        std::string getParameter(const std::string& objectID, const std::string& key) const;

        /// @brief set generic paramter
        void setParameter(const std::string& objectID, const std::string& key, const std::string& value) const;

        void subscribe(const std::string& objID, const std::vector<int>& vars, double beginTime, double endTime) const;
        void subscribeContext(const std::string& objID, int domain, double range, const std::vector<int>& vars, double beginTime, double endTime) const;

        const libsumo::SubscriptionResults getAllSubscriptionResults() const;
        const libsumo::TraCIResults getSubscriptionResults(const std::string& objID) const;

        const libsumo::ContextSubscriptionResults getAllContextSubscriptionResults() const;
        const libsumo::SubscriptionResults getContextSubscriptionResults(const std::string& objID) const;

        // the following are only for internal use
        void clearSubscriptionResults();
        libsumo::SubscriptionResults& getModifiableSubscriptionResults();
        libsumo::SubscriptionResults& getModifiableContextSubscriptionResults(const std::string& objID);


    protected:
        /// @brief The parent TraCI client which offers the connection
        TraCIAPI& myParent;
        int myCmdGetID;
        int myCmdSetID;
        int mySubscribeID;
        int myContextSubscribeID;
        libsumo::SubscriptionResults mySubscriptionResults;
        libsumo::ContextSubscriptionResults myContextSubscriptionResults;


    private:
        /// @brief invalidated copy constructor
        TraCIScopeWrapper(const TraCIScopeWrapper& src);

        /// @brief invalidated assignment operator
        TraCIScopeWrapper& operator=(const TraCIScopeWrapper& src);

    };





    /** @class EdgeScope
     * @brief Scope for interaction with edges
     */
    class EdgeScope : public TraCIScopeWrapper {
    public:
        EdgeScope(TraCIAPI& parent) : TraCIScopeWrapper(parent, libsumo::CMD_GET_EDGE_VARIABLE, libsumo::CMD_SET_EDGE_VARIABLE, libsumo::CMD_SUBSCRIBE_EDGE_VARIABLE, libsumo::CMD_SUBSCRIBE_EDGE_CONTEXT) {}
        virtual ~EdgeScope() {}

        std::vector<std::string> getIDList() const;
        int getIDCount() const;
        double getAdaptedTraveltime(const std::string& edgeID, double time) const;
        double getEffort(const std::string& edgeID, double time) const;
        double getCO2Emission(const std::string& edgeID) const;
        double getCOEmission(const std::string& edgeID) const;
        double getHCEmission(const std::string& edgeID) const;
        double getPMxEmission(const std::string& edgeID) const;
        double getNOxEmission(const std::string& edgeID) const;
        double getFuelConsumption(const std::string& edgeID) const;
        double getNoiseEmission(const std::string& edgeID) const;
        double getElectricityConsumption(const std::string& edgeID) const;
        double getLastStepMeanSpeed(const std::string& edgeID) const;
        double getLastStepOccupancy(const std::string& edgeID) const;
        double getLastStepLength(const std::string& edgeID) const;
        double getTraveltime(const std::string& edgeID) const;
        int getLastStepVehicleNumber(const std::string& edgeID) const;
        double getLastStepHaltingNumber(const std::string& edgeID) const;
        std::vector<std::string> getLastStepVehicleIDs(const std::string& edgeID) const;
        int getLaneNumber(const std::string& edgeID) const;
        std::string getStreetName(const std::string& id) const;

        void adaptTraveltime(const std::string& edgeID, double time, double beginSeconds = 0., double endSeconds = std::numeric_limits<double>::max()) const;
        void setEffort(const std::string& edgeID, double effort, double beginSeconds = 0., double endSeconds = std::numeric_limits<double>::max()) const;
        void setMaxSpeed(const std::string& edgeID, double speed) const;

    private:
        /// @brief invalidated copy constructor
        EdgeScope(const EdgeScope& src);

        /// @brief invalidated assignment operator
        EdgeScope& operator=(const EdgeScope& src);

    };





    /** @class GUIScope
     * @brief Scope for interaction with the gui
     */
    class GUIScope : public TraCIScopeWrapper {
    public:
        GUIScope(TraCIAPI& parent) : TraCIScopeWrapper(parent, libsumo::CMD_GET_GUI_VARIABLE, libsumo::CMD_SET_GUI_VARIABLE, libsumo::CMD_SUBSCRIBE_GUI_VARIABLE, libsumo::CMD_SUBSCRIBE_GUI_CONTEXT) {}
        virtual ~GUIScope() {}

        std::vector<std::string> getIDList() const;
        double getZoom(const std::string& viewID = DEFAULT_VIEW) const;
        libsumo::TraCIPosition getOffset(const std::string& viewID = DEFAULT_VIEW) const;
        std::string getSchema(const std::string& viewID = DEFAULT_VIEW) const;
        libsumo::TraCIPositionVector getBoundary(const std::string& viewID = DEFAULT_VIEW) const;
        void setZoom(const std::string& viewID, double zoom) const;
        void setOffset(const std::string& viewID, double x, double y) const;
        void setSchema(const std::string& viewID, const std::string& schemeName) const;
        void setBoundary(const std::string& viewID, double xmin, double ymin, double xmax, double ymax) const;
        void screenshot(const std::string& viewID, const std::string& filename, const int width = -1, const int height = -1) const;
        void trackVehicle(const std::string& viewID, const std::string& vehID) const;

    private:
        /// @brief invalidated copy constructor
        GUIScope(const GUIScope& src);

        /// @brief invalidated assignment operator
        GUIScope& operator=(const GUIScope& src);

    };





    /** @class InductionLoopScope
     * @brief Scope for interaction with inductive loops
     */
    class InductionLoopScope : public TraCIScopeWrapper {
    public:
        InductionLoopScope(TraCIAPI& parent) : TraCIScopeWrapper(parent, libsumo::CMD_GET_INDUCTIONLOOP_VARIABLE, -1, libsumo::CMD_SUBSCRIBE_INDUCTIONLOOP_VARIABLE, libsumo::CMD_SUBSCRIBE_INDUCTIONLOOP_CONTEXT) {}
        virtual ~InductionLoopScope() {}

        std::vector<std::string> getIDList() const;
        double  getPosition(const std::string& loopID) const;
        std::string getLaneID(const std::string& loopID) const;
        int getLastStepVehicleNumber(const std::string& loopID) const;
        double getLastStepMeanSpeed(const std::string& loopID) const;
        std::vector<std::string> getLastStepVehicleIDs(const std::string& loopID) const;
        double getLastStepOccupancy(const std::string& loopID) const;
        double getLastStepMeanLength(const std::string& loopID) const;
        double getTimeSinceDetection(const std::string& loopID) const;
        std::vector<libsumo::TraCIVehicleData> getVehicleData(const std::string& loopID) const;


    private:
        /// @brief invalidated copy constructor
        InductionLoopScope(const InductionLoopScope& src);

        /// @brief invalidated assignment operator
        InductionLoopScope& operator=(const InductionLoopScope& src);

    };





    /** @class JunctionScope
     * @brief Scope for interaction with junctions
     */
    class JunctionScope : public TraCIScopeWrapper {
    public:
        JunctionScope(TraCIAPI& parent) : TraCIScopeWrapper(parent, libsumo::CMD_GET_JUNCTION_VARIABLE, libsumo::CMD_SET_JUNCTION_VARIABLE, libsumo::CMD_SUBSCRIBE_JUNCTION_VARIABLE, libsumo::CMD_SUBSCRIBE_JUNCTION_CONTEXT) {}
        virtual ~JunctionScope() {}

        std::vector<std::string> getIDList() const;
        libsumo::TraCIPosition getPosition(const std::string& junctionID) const;

    private:
        /// @brief invalidated copy constructor
        JunctionScope(const JunctionScope& src);

        /// @brief invalidated assignment operator
        JunctionScope& operator=(const JunctionScope& src);

    };





    /** @class LaneScope
     * @brief Scope for interaction with lanes
     */
    class LaneScope : public TraCIScopeWrapper {
    public:
        LaneScope(TraCIAPI& parent) : TraCIScopeWrapper(parent, libsumo::CMD_GET_LANE_VARIABLE, libsumo::CMD_SET_LANE_VARIABLE, libsumo::CMD_SUBSCRIBE_LANE_VARIABLE, libsumo::CMD_SUBSCRIBE_LANE_CONTEXT) {}
        virtual ~LaneScope() {}

        std::vector<std::string> getIDList() const;
        int getIDCount() const;
        double getLength(const std::string& laneID) const;
        double getMaxSpeed(const std::string& laneID) const;
        double getWidth(const std::string& laneID) const;
        std::vector<std::string> getAllowed(const std::string& laneID) const;
        std::vector<std::string> getDisallowed(const std::string& laneID) const;
        int getLinkNumber(const std::string& laneID) const;
        std::vector<libsumo::TraCIConnection> getLinks(const std::string& laneID) const;
        libsumo::TraCIPositionVector getShape(const std::string& laneID) const;
        std::string getEdgeID(const std::string& laneID) const;
        double getCO2Emission(const std::string& laneID) const;
        double getCOEmission(const std::string& laneID) const;
        double getHCEmission(const std::string& laneID) const;
        double getPMxEmission(const std::string& laneID) const;
        double getNOxEmission(const std::string& laneID) const;
        double getFuelConsumption(const std::string& laneID) const;
        double getNoiseEmission(const std::string& laneID) const;
        double getElectricityConsumption(const std::string& laneID) const;
        double getLastStepMeanSpeed(const std::string& laneID) const;
        double getLastStepOccupancy(const std::string& laneID) const;
        double getLastStepLength(const std::string& laneID) const;
        double getTraveltime(const std::string& laneID) const;
        int getLastStepVehicleNumber(const std::string& laneID) const;
        int getLastStepHaltingNumber(const std::string& laneID) const;
        std::vector<std::string> getLastStepVehicleIDs(const std::string& laneID) const;
        std::vector<std::string> getFoes(const std::string& laneID, const std::string& toLaneID) const;
        std::vector<std::string> getInternalFoes(const std::string& laneID) const;

        void setAllowed(const std::string& laneID, const std::vector<std::string>& allowedClasses) const;
        void setDisallowed(const std::string& laneID, const std::vector<std::string>& disallowedClasses) const;
        void setMaxSpeed(const std::string& laneID, double speed) const;
        void setLength(const std::string& laneID, double length) const;

    private:
        /// @brief invalidated copy constructor
        LaneScope(const LaneScope& src);

        /// @brief invalidated assignment operator
        LaneScope& operator=(const LaneScope& src);

    };


    /** @class LaneAreaScope
    * @brief Scope for interaction with lane area detectors
    */
    class LaneAreaScope : public TraCIScopeWrapper {
    public:
        LaneAreaScope(TraCIAPI& parent) : TraCIScopeWrapper(parent, libsumo::CMD_GET_LANEAREA_VARIABLE, -1, libsumo::CMD_SUBSCRIBE_LANEAREA_VARIABLE, libsumo::CMD_SUBSCRIBE_LANEAREA_CONTEXT) {}
        virtual ~LaneAreaScope() {}

        std::vector<std::string> getIDList() const;

    private:
        /// @brief invalidated copy constructor
        LaneAreaScope(const LaneAreaScope& src);

        /// @brief invalidated assignment operator
        LaneAreaScope& operator=(const LaneAreaScope& src);

    };


    /** @class MeMeScope
     * @brief Scope for interaction with multi entry/-exit detectors
     */
    class MeMeScope : public TraCIScopeWrapper {
    public:
        MeMeScope(TraCIAPI& parent) : TraCIScopeWrapper(parent, libsumo::CMD_GET_MULTIENTRYEXIT_VARIABLE, -1, libsumo::CMD_SUBSCRIBE_MULTIENTRYEXIT_VARIABLE, libsumo::CMD_SUBSCRIBE_MULTIENTRYEXIT_CONTEXT) {}
        virtual ~MeMeScope() {}

        std::vector<std::string> getIDList() const;
        int getLastStepVehicleNumber(const std::string& detID) const;
        double getLastStepMeanSpeed(const std::string& detID) const;
        std::vector<std::string> getLastStepVehicleIDs(const std::string& detID) const;
        int getLastStepHaltingNumber(const std::string& detID) const;

    private:
        /// @brief invalidated copy constructor
        MeMeScope(const MeMeScope& src);

        /// @brief invalidated assignment operator
        MeMeScope& operator=(const MeMeScope& src);

    };





    /** @class POIScope
     * @brief Scope for interaction with POIs
     */
    class POIScope : public TraCIScopeWrapper {
    public:
        POIScope(TraCIAPI& parent) : TraCIScopeWrapper(parent, libsumo::CMD_GET_POI_VARIABLE, libsumo::CMD_SET_POI_VARIABLE, libsumo::CMD_SUBSCRIBE_POI_VARIABLE, libsumo::CMD_SUBSCRIBE_POI_CONTEXT) {}
        virtual ~POIScope() {}

        std::vector<std::string> getIDList() const;
        int getIDCount() const;
        std::string getType(const std::string& poiID) const;
        libsumo::TraCIPosition getPosition(const std::string& poiID) const;
        libsumo::TraCIColor getColor(const std::string& poiID) const;
        double getWidth(const std::string& poiID) const;
        double getHeight(const std::string& poiID) const;
        double getAngle(const std::string& poiID) const;
        std::string getImageFile(const std::string& poiID) const;

        void setType(const std::string& poiID, const std::string& setType) const;
        void setPosition(const std::string& poiID, double x, double y) const;
        void setColor(const std::string& poiID, const libsumo::TraCIColor& c) const;
        void setWidth(const std::string& poiID, double width) const;
        void setHeight(const std::string& poiID, double height) const;
        void setAngle(const std::string& poiID, double angle) const;
        void setImageFile(const std::string& poiID, const std::string& imageFile) const;
        void add(const std::string& poiID, double x, double y, const libsumo::TraCIColor& c, const std::string& type, int layer, const std::string& imgFile, double width, double height, double angle) const;
        void remove(const std::string& poiID, int layer = 0) const;

    private:
        /// @brief invalidated copy constructor
        POIScope(const POIScope& src);

        /// @brief invalidated assignment operator
        POIScope& operator=(const POIScope& src);

    };





    /** @class PolygonScope
     * @brief Scope for interaction with polygons
     */
    class PolygonScope : public TraCIScopeWrapper {
    public:
        PolygonScope(TraCIAPI& parent) : TraCIScopeWrapper(parent, libsumo::CMD_GET_POLYGON_VARIABLE, libsumo::CMD_SET_POLYGON_VARIABLE, libsumo::CMD_SUBSCRIBE_POLYGON_VARIABLE, libsumo::CMD_SUBSCRIBE_POLYGON_CONTEXT) {}
        virtual ~PolygonScope() {}

        std::vector<std::string> getIDList() const;
        int getIDCount() const;
        double getLineWidth(const std::string& polygonID) const;
        std::string getType(const std::string& polygonID) const;
        libsumo::TraCIPositionVector getShape(const std::string& polygonID) const;
        libsumo::TraCIColor getColor(const std::string& polygonID) const;
        void setType(const std::string& polygonID, const std::string& setType) const;
        void setShape(const std::string& polygonID, const libsumo::TraCIPositionVector& shape) const;
        void setColor(const std::string& polygonID, const libsumo::TraCIColor& c) const;
        void setLineWidth(const std::string& polygonID, const double lineWidth) const;
        void add(const std::string& polygonID, const libsumo::TraCIPositionVector& shape, const libsumo::TraCIColor& c, bool fill, const std::string& type, int layer) const;
        void remove(const std::string& polygonID, int layer = 0) const;

    private:
        /// @brief invalidated copy constructor
        PolygonScope(const PolygonScope& src);

        /// @brief invalidated assignment operator
        PolygonScope& operator=(const PolygonScope& src);

    };





    /** @class RouteScope
     * @brief Scope for interaction with routes
     */
    class RouteScope : public TraCIScopeWrapper {
    public:
        RouteScope(TraCIAPI& parent) : TraCIScopeWrapper(parent, libsumo::CMD_GET_ROUTE_VARIABLE, libsumo::CMD_SET_ROUTE_VARIABLE, libsumo::CMD_SUBSCRIBE_ROUTE_VARIABLE, libsumo::CMD_SUBSCRIBE_ROUTE_CONTEXT) {}
        virtual ~RouteScope() {}

        std::vector<std::string> getIDList() const;
        std::vector<std::string> getEdges(const std::string& routeID) const;

        void add(const std::string& routeID, const std::vector<std::string>& edges) const;

    private:
        /// @brief invalidated copy constructor
        RouteScope(const RouteScope& src);

        /// @brief invalidated assignment operator
        RouteScope& operator=(const RouteScope& src);

    };



    /** @class SimulationScope
     * @brief Scope for interaction with the simulation
     */
    class SimulationScope : public TraCIScopeWrapper {
    public:
        SimulationScope(TraCIAPI& parent) : TraCIScopeWrapper(parent, libsumo::CMD_GET_SIM_VARIABLE, libsumo::CMD_SET_SIM_VARIABLE, libsumo::CMD_SUBSCRIBE_SIM_VARIABLE, libsumo::CMD_SUBSCRIBE_SIM_CONTEXT) {}
        virtual ~SimulationScope() {}

        int getCurrentTime() const;
        double getTime() const;
        int getLoadedNumber() const;
        std::vector<std::string> getLoadedIDList() const;
        int getDepartedNumber() const;
        std::vector<std::string> getDepartedIDList() const;
        int getArrivedNumber() const;
        std::vector<std::string> getArrivedIDList() const;
        int getStartingTeleportNumber() const;
        std::vector<std::string> getStartingTeleportIDList() const;
        int getEndingTeleportNumber() const;
        std::vector<std::string> getEndingTeleportIDList() const;
        double getDeltaT() const;
        libsumo::TraCIPositionVector getNetBoundary() const;
        int getMinExpectedNumber() const;

        libsumo::TraCIPosition convert2D(const std::string& edgeID, double pos, int laneIndex = 0, bool toGeo = false) const;
        libsumo::TraCIPosition convert3D(const std::string& edgeID, double pos, int laneIndex = 0, bool toGeo = false) const;
        libsumo::TraCIRoadPosition convertRoad(double x, double y, bool isGeo = false, const std::string& vClass = "ignoring") const;
        libsumo::TraCIPosition convertGeo(double x, double y, bool fromGeo = false) const;

        double getDistance2D(double x1, double y1, double x2, double y2, bool isGeo = false, bool isDriving = false);
        double getDistanceRoad(const std::string& edgeID1, double pos1, const std::string& edgeID2, double pos2, bool isDriving = false);


    private:
        /// @brief invalidated copy constructor
        SimulationScope(const SimulationScope& src);

        /// @brief invalidated assignment operator
        SimulationScope& operator=(const SimulationScope& src);

    };





    /** @class TrafficLightScope
     * @brief Scope for interaction with traffic lights
     */
    class TrafficLightScope : public TraCIScopeWrapper {
    public:
        TrafficLightScope(TraCIAPI& parent) : TraCIScopeWrapper(parent, libsumo::CMD_GET_TL_VARIABLE, libsumo::CMD_SET_TL_VARIABLE, libsumo::CMD_SUBSCRIBE_TL_VARIABLE, libsumo::CMD_SUBSCRIBE_TL_CONTEXT) {}
        virtual ~TrafficLightScope() {}

        std::vector<std::string> getIDList() const;
        int getIDCount() const;
        std::string getRedYellowGreenState(const std::string& tlsID) const;
        std::vector<libsumo::TraCILogic> getCompleteRedYellowGreenDefinition(const std::string& tlsID) const;
        std::vector<std::string> getControlledLanes(const std::string& tlsID) const;
        std::vector<std::vector<libsumo::TraCILink> > getControlledLinks(const std::string& tlsID) const;
        std::string getProgram(const std::string& tlsID) const;
        int getPhase(const std::string& tlsID) const;
        double getPhaseDuration(const std::string& tlsID) const;
        double getNextSwitch(const std::string& tlsID) const;
        std::string getPhaseName(const std::string& tlsID) const;

        void setRedYellowGreenState(const std::string& tlsID, const std::string& state) const;
        void setPhase(const std::string& tlsID, int index) const;
        void setPhaseName(const std::string& tlsID, const std::string& name) const;
        void setProgram(const std::string& tlsID, const std::string& programID) const;
        void setPhaseDuration(const std::string& tlsID, double phaseDuration) const;
        void setCompleteRedYellowGreenDefinition(const std::string& tlsID, const libsumo::TraCILogic& logic) const;

    private:
        /// @brief invalidated copy constructor
        TrafficLightScope(const TrafficLightScope& src);

        /// @brief invalidated assignment operator
        TrafficLightScope& operator=(const TrafficLightScope& src);

    };





    /** @class VehicleTypeScope
     * @brief Scope for interaction with vehicle types
     */
    class VehicleTypeScope : public TraCIScopeWrapper {
    public:
        VehicleTypeScope(TraCIAPI& parent) : TraCIScopeWrapper(parent, libsumo::CMD_GET_VEHICLETYPE_VARIABLE, libsumo::CMD_SET_VEHICLETYPE_VARIABLE, libsumo::CMD_SUBSCRIBE_VEHICLETYPE_VARIABLE, libsumo::CMD_SUBSCRIBE_VEHICLETYPE_CONTEXT) {}
        virtual ~VehicleTypeScope() {}

        std::vector<std::string> getIDList() const;
        double getLength(const std::string& typeID) const;
        double getMaxSpeed(const std::string& typeID) const;
        double getSpeedFactor(const std::string& typeID) const;
        double getSpeedDeviation(const std::string& typeID) const;
        double getAccel(const std::string& typeID) const;
        double getDecel(const std::string& typeID) const;
        double getEmergencyDecel(const std::string& typeID) const;
        double getApparentDecel(const std::string& typeID) const;
        double getImperfection(const std::string& typeID) const;
        double getTau(const std::string& typeID) const;
        std::string getVehicleClass(const std::string& typeID) const;
        std::string getEmissionClass(const std::string& typeID) const;
        std::string getShapeClass(const std::string& typeID) const;
        double getMinGap(const std::string& typeID) const;
        double getWidth(const std::string& typeID) const;
        double getHeight(const std::string& typeID) const;
        libsumo::TraCIColor getColor(const std::string& typeID) const;
        double getMinGapLat(const std::string& typeID) const;
        double getMaxSpeedLat(const std::string& typeID) const;
        std::string getLateralAlignment(const std::string& typeID) const;

        void setLength(const std::string& typeID, double length) const;
        void setMaxSpeed(const std::string& typeID, double speed) const;
        void setVehicleClass(const std::string& typeID, const std::string& clazz) const;
        void setSpeedFactor(const std::string& typeID, double factor) const;
        void setSpeedDeviation(const std::string& typeID, double deviation) const;
        void setEmissionClass(const std::string& typeID, const std::string& clazz) const;
        void setShapeClass(const std::string& typeID, const std::string& shapeClass) const;
        void setWidth(const std::string& typeID, double width) const;
        void setHeight(const std::string& typeID, double height) const;
        void setMinGap(const std::string& typeID, double minGap) const;
        void setAccel(const std::string& typeID, double accel) const;
        void setDecel(const std::string& typeID, double decel) const;
        void setEmergencyDecel(const std::string& typeID, double decel) const;
        void setApparentDecel(const std::string& typeID, double decel) const;
        void setImperfection(const std::string& typeID, double imperfection) const;
        void setTau(const std::string& typeID, double tau) const;
        void setColor(const std::string& typeID, const libsumo::TraCIColor& c) const;
        void setMinGapLat(const std::string& typeID, double minGapLat) const;
        void setMaxSpeedLat(const std::string& typeID, double speed) const;
        void setLateralAlignment(const std::string& typeID, const std::string& latAlignment) const;
        void copy(const std::string& origTypeID, const std::string& newTypeID) const;

    private:
        /// @brief invalidated copy constructor
        VehicleTypeScope(const VehicleTypeScope& src);

        /// @brief invalidated assignment operator
        VehicleTypeScope& operator=(const VehicleTypeScope& src);

    };





    /** @class VehicleScope
     * @brief Scope for interaction with vehicles
     */
    class VehicleScope : public TraCIScopeWrapper {
    public:
        VehicleScope(TraCIAPI& parent) : TraCIScopeWrapper(parent, libsumo::CMD_GET_VEHICLE_VARIABLE, libsumo::CMD_SET_VEHICLE_VARIABLE, libsumo::CMD_SUBSCRIBE_VEHICLE_VARIABLE, libsumo::CMD_SUBSCRIBE_VEHICLE_CONTEXT) {}
        virtual ~VehicleScope() {}

        enum VehicleSignal {
            SIGNAL_BLINKER_RIGHT = 1,
            SIGNAL_BLINKER_LEFT = 2,
            SIGNAL_BLINKER_EMERGENCY = 4,
            SIGNAL_BRAKELIGHT = 8,
            SIGNAL_FRONTLIGHT = 16,
            SIGNAL_FOGLIGHT = 32,
            SIGNAL_HIGHBEAM = 64,
            SIGNAL_BACKDRIVE = 128,
            SIGNAL_WIPER = 256,
            SIGNAL_DOOR_OPEN_LEFT = 512,
            SIGNAL_DOOR_OPEN_RIGHT = 1024,
            SIGNAL_EMERGENCY_BLUE = 2048,
            SIGNAL_EMERGENCY_RED = 4096,
            SIGNAL_EMERGENCY_YELLOW = 8192,
            SIGNAL_RESET = -1, /*< sending a negative signal resets all signals to their computed values immediately */
        };

        /// @name vehicle value retrieval
        /// @{
        std::vector<std::string> getIDList() const;
        int getIDCount() const;
        double getSpeed(const std::string& vehicleID) const;
        double getAcceleration(const std::string& vehicleID) const;
        libsumo::TraCIPosition getPosition(const std::string& vehicleID) const;
        libsumo::TraCIPosition getPosition3D(const std::string& vehicleID) const;
        double getAngle(const std::string& vehicleID) const;
        std::string getRoadID(const std::string& vehicleID) const;
        std::string getLaneID(const std::string& vehicleID) const;
        int getLaneIndex(const std::string& vehicleID) const;
        std::string getTypeID(const std::string& vehicleID) const;
        std::string getRouteID(const std::string& vehicleID) const;
        int getRouteIndex(const std::string& vehicleID) const;
        std::vector<std::string> getRoute(const std::string& vehicleID) const;
        libsumo::TraCIColor getColor(const std::string& vehicleID) const;
        double getLanePosition(const std::string& vehicleID) const;
        double getDistance(const std::string& vehicleID) const;
        int getSignals(const std::string& vehicleID) const;
        double getCO2Emission(const std::string& vehicleID) const;
        double getCOEmission(const std::string& vehicleID) const;
        double getHCEmission(const std::string& vehicleID) const;
        double getPMxEmission(const std::string& vehicleID) const;
        double getNOxEmission(const std::string& vehicleID) const;
        double getFuelConsumption(const std::string& vehicleID) const;
        double getNoiseEmission(const std::string& vehicleID) const;
        double getElectricityConsumption(const std::string& vehicleID) const;
        int getSpeedMode(const std::string& vehicleID) const;
        int getStopState(const std::string& vehicleID) const;
        double getWaitingTime(const std::string& vehicleID) const;
        double getAccumulatedWaitingTime(const std::string& vehicleID) const;
        double getSlope(const std::string& vehicleID) const;
        double getAllowedSpeed(const std::string& vehicleID) const;
        int getPersonNumber(const std::string& vehicleID) const;
        std::vector<std::string> getPersonIDList(const std::string& vehicleID) const;
        double getSpeedWithoutTraCI(const std::string& vehicleID) const;
        bool isRouteValid(const std::string& vehicleID) const;
        double getLateralLanePosition(const std::string& vehicleID) const;
        double getSpeedFactor(const std::string& vehicleID) const;
        std::string getLine(const std::string& vehicleID) const;
        std::vector<std::string> getVia(const std::string& vehicleID) const;
        std::vector<libsumo::TraCINextTLSData> getNextTLS(const std::string& vehID) const;
        std::vector<libsumo::TraCIBestLanesData> getBestLanes(const std::string& vehicleID) const;
        std::pair<std::string, double> getLeader(const std::string& vehicleID, double dist) const;
        int getRoutingMode(const std::string& vehicleID) const;
        std::pair<int, int> getLaneChangeState(const std::string& vehicleID, int direction) const;
        /// @}

        /// @name vehicle type value retrieval shortcuts
        /// @{
        double getLength(const std::string& vehicleID) const;
        double getMaxSpeed(const std::string& vehicleID) const;
        double getAccel(const std::string& vehicleID) const;
        double getDecel(const std::string& vehicleID) const;
        double getEmergencyDecel(const std::string& vehicleID) const;
        double getApparentDecel(const std::string& vehicleID) const;
        double getTau(const std::string& vehicleID) const;
        double getImperfection(const std::string& vehicleID) const;
        double getSpeedDeviation(const std::string& vehicleID) const;
        double getMinGap(const std::string& vehicleID) const;
        double getWidth(const std::string& vehicleID) const;
        double getHeight(const std::string& veihcleID) const;
        double getMaxSpeedLat(const std::string& vehicleID) const;
        double getMinGapLat(const std::string& vehicleID) const;
        std::string getVehicleClass(const std::string& vehicleID) const;
        std::string getEmissionClass(const std::string& vehicleID) const;
        std::string getShapeClass(const std::string& vehicleID) const;
        std::string getLateralAlignment(const std::string& vehicleID) const;
        /// @}

        /// @name vehicle state changing
        /// @{
        void add(const std::string& vehicleID,
                 const std::string& routeID,
                 const std::string& typeID = "DEFAULT_VEHTYPE",
                 std::string depart = "-1",
                 const std::string& departLane = "first",
                 const std::string& departPos = "base",
                 const std::string& departSpeed = "0",
                 const std::string& arrivalLane = "current",
                 const std::string& arrivalPos = "max",
                 const std::string& arrivalSpeed = "current",
                 const std::string& fromTaz = "",
                 const std::string& toTaz = "",
                 const std::string& line = "",
                 int personCapacity = 0,
                 int personNumber = 0) const;

        void changeTarget(const std::string& vehicleID, const std::string& edgeID) const;
        void changeLane(const std::string& vehicleID, int laneIndex, double duration) const;
        void changeLaneRelative(const std::string& vehicleID, int laneChange, double duration) const;
        void changeSublane(const std::string& vehicleID, double latDist) const;
        void setRouteID(const std::string& vehicleID, const std::string& routeID) const;
        void setRoute(const std::string& vehicleID, const std::vector<std::string>& edge) const;
        void rerouteTraveltime(const std::string& vehicleID, bool currentTravelTimes = true) const;
        void moveTo(const std::string& vehicleID, const std::string& laneID, double position) const;
        void moveToXY(const std::string& vehicleID, const std::string& edgeID, const int lane, const double x, const double y, const double angle, const int keepRoute) const;
        void slowDown(const std::string& vehicleID, double speed, double duration) const;
        void openGap(const std::string& vehicleID, double newTau, double duration, double changeRate, double maxDecel) const;
        void setSpeed(const std::string& vehicleID, double speed) const;
        void setSpeedMode(const std::string& vehicleID, int mode) const;
        void setStop(const std::string vehicleID, const std::string edgeID, const double endPos = 1.,
                     const int laneIndex = 0, const double duration = std::numeric_limits<double>::max(),
                     const int flags = 0, const double startPos = std::numeric_limits<int>::min(),
                     const double until = -1) const;
        void setType(const std::string& vehicleID, const std::string& typeID) const;
        void remove(const std::string& vehicleID, char reason = libsumo::REMOVE_VAPORIZED) const;
        void setColor(const std::string& vehicleID, const libsumo::TraCIColor& c) const;
        void setLine(const std::string& vehicleID, const std::string& line) const;
        void setVia(const std::string& vehicleID, const std::vector<std::string>& via) const;
        void setSignals(const std::string& vehicleID, int signals) const;
        void setRoutingMode(const std::string& vehicleID, int routingMode) const;
        /// @}

        /// @name vehicle type attribute changing shortcuts
        /// @{
        void setShapeClass(const std::string& vehicleID, const std::string& clazz) const;
        void setEmissionClass(const std::string& vehicleID, const std::string& clazz) const;
        void setSpeedFactor(const std::string& vehicleID, double factor) const;
        void setMaxSpeed(const std::string& vehicleID, double speed) const;
        /// @}

    private:
        /// @brief invalidated copy constructor
        VehicleScope(const VehicleScope& src);

        /// @brief invalidated assignment operator
        VehicleScope& operator=(const VehicleScope& src);

    };

    /** @class PersonScope
     * * @brief Scope for interaction with vehicles
     * */
    class PersonScope : public TraCIScopeWrapper {
    public:
        PersonScope(TraCIAPI& parent) : TraCIScopeWrapper(parent, libsumo::CMD_GET_PERSON_VARIABLE, libsumo::CMD_SET_PERSON_VARIABLE, libsumo::CMD_SUBSCRIBE_PERSON_VARIABLE, libsumo::CMD_SUBSCRIBE_PERSON_CONTEXT) {}
        virtual ~PersonScope() {}

        std::vector<std::string> getIDList() const;
        int getIDCount() const;
        double getSpeed(const std::string& personID) const;
        libsumo::TraCIPosition getPosition(const std::string& personID) const;
        libsumo::TraCIPosition getPosition3D(const std::string& personID) const;
        std::string getRoadID(const std::string& personID) const;
        std::string getTypeID(const std::string& personID) const;
        double getWaitingTime(const std::string& personID) const;
        std::string getNextEdge(const std::string& personID) const;
        std::string getVehicle(const std::string& personID) const;
        int getRemainingStages(const std::string& personID) const;
        int getStage(const std::string& personID, int nextStageIndex = 0) const;
        std::vector<std::string> getEdges(const std::string& personID, int nextStageIndex = 0) const;
        double getAngle(const std::string& personID) const;
        double getSlope(const std::string& personID) const;
        double getLanePosition(const std::string& personID) const;
        libsumo::TraCIColor getColor(const std::string& personID) const;

        /// @name vehicle type value retrieval shortcuts
        /// @{
        double getLength(const std::string& personID) const;
        /// @}


        void removeStages(const std::string& personID) const;
        void add(const std::string& personID, const std::string& edgeID, double pos, double depart = libsumo::DEPARTFLAG_NOW, const std::string typeID = "DEFAULT_PEDTYPE");
        void appendWaitingStage(const std::string& personID, double duration, const std::string& description = "waiting", const std::string& stopID = "");
        void appendWalkingStage(const std::string& personID, const std::vector<std::string>& edges, double arrivalPos, double duration = -1, double speed = -1, const std::string& stopID = "");
        void appendDrivingStage(const std::string& personID, const std::string& toEdge, const std::string& lines, const std::string& stopID = "");
        void removeStage(const std::string& personID, int nextStageIndex) const;
        void rerouteTraveltime(const std::string& personID) const;
        void setSpeed(const std::string& personID, double speed) const;
        void setType(const std::string& personID, const std::string& typeID) const;
        void setLength(const std::string& personID, double length) const;
        void setWidth(const std::string& personID, double width) const;
        void setHeight(const std::string& personID, double height) const;
        void setMinGap(const std::string& personID, double minGap) const;
        void setColor(const std::string& personID, const libsumo::TraCIColor& c) const;

    private:
        /// @brief invalidated copy constructor
        PersonScope(const PersonScope& src);

        /// @brief invalidated assignment operator
        PersonScope& operator=(const PersonScope& src);
    };



public:
    /// @brief Scope for interaction with edges
    EdgeScope edge;
    /// @brief Scope for interaction with the gui
    GUIScope gui;
    /// @brief Scope for interaction with inductive loops
    InductionLoopScope inductionloop;
    /// @brief Scope for interaction with junctions
    JunctionScope junction;
    /// @brief Scope for interaction with lanes
    LaneScope lane;
    /// @brief Scope for interaction with lanes
    LaneAreaScope lanearea;
    /// @brief Scope for interaction with multi-entry/-exit detectors
    MeMeScope multientryexit;
    /// @brief Scope for interaction with persons
    PersonScope person;
    /// @brief Scope for interaction with POIs
    POIScope poi;
    /// @brief Scope for interaction with polygons
    PolygonScope polygon;
    /// @brief Scope for interaction with routes
    RouteScope route;
    /// @brief Scope for interaction with the simulation
    SimulationScope simulation;
    /// @brief Scope for interaction with traffic lights
    TrafficLightScope trafficlights;
    /// @brief Scope for interaction with vehicles
    VehicleScope vehicle;
    /// @brief Scope for interaction with vehicle types
    VehicleTypeScope vehicletype;


protected:
    /// @name Command sending methods
    /// @{

    /** @brief Sends a SimulationStep command
     */
    void send_commandSimulationStep(double time) const;


    /** @brief Sends a Close command
     */
    void send_commandClose() const;


    /** @brief Sends a SetOrder command
     */
    void send_commandSetOrder(int order) const;

    /** @brief Sends a GetVariable / SetVariable request if mySocket is connected.
     * Otherwise writes to myOutput only.
     * @param[in] cmdID The command and domain of the variable
     * @param[in] varID The variable to retrieve
     * @param[in] objID The object to retrieve the variable from
     * @param[in] add Optional additional parameter
     */
    void createCommand(int cmdID, int varID, const std::string& objID, tcpip::Storage* add = nullptr) const;


    /** @brief Sends a SubscribeVariable request
     * @param[in] domID The domain of the variable
     * @param[in] objID The object to subscribe the variables from
     * @param[in] beginTime The begin time step of subscriptions
     * @param[in] endTime The end time step of subscriptions
     * @param[in] vars The variables to subscribe
     */
    void send_commandSubscribeObjectVariable(int domID, const std::string& objID, double beginTime, double endTime, const std::vector<int>& vars) const;


    /** @brief Sends a SubscribeContext request
     * @param[in] domID The domain of the variable
     * @param[in] objID The object to subscribe the variables from
     * @param[in] beginTime The begin time step of subscriptions
     * @param[in] endTime The end time step of subscriptions
     * @param[in] domain The domain of the objects which values shall be returned
     * @param[in] range The range around the obj to investigate
     * @param[in] vars The variables to subscribe
     */
    void send_commandSubscribeObjectContext(int domID, const std::string& objID, double beginTime, double endTime,
                                            int domain, double range, const std::vector<int>& vars) const;
    /// @}


    /// @name Command sending methods
    /// @{

    /** @brief Validates the result state of a command
     * @param[in] inMsg The buffer to read the message from
     * @param[in] command The original command id
     * @param[in] ignoreCommandId Whether the returning command id shall be validated
     * @param[in] acknowledgement Pointer to an existing string into which the acknowledgement message shall be inserted
     */
    void check_resultState(tcpip::Storage& inMsg, int command, bool ignoreCommandId = false, std::string* acknowledgement = 0) const;

    /** @brief Validates the result state of a command
     * @return The command Id
     */
    int check_commandGetResult(tcpip::Storage& inMsg, int command, int expectedType = -1, bool ignoreCommandId = false) const;

    bool processGet(int command, int expectedType, bool ignoreCommandId = false);
    bool processSet(int command);
    /// @}

    void readVariableSubscription(int cmdId, tcpip::Storage& inMsg);
    void readContextSubscription(int cmdId, tcpip::Storage& inMsg);
    void readVariables(tcpip::Storage& inMsg, const std::string& objectID, int variableCount, libsumo::SubscriptionResults& into);

    template <class T>
    static inline std::string toString(const T& t, std::streamsize accuracy = PRECISION) {
        std::ostringstream oss;
        oss.setf(std::ios::fixed, std::ios::floatfield);
        oss << std::setprecision(accuracy);
        oss << t;
        return oss.str();
    }

    /// @brief Closes the connection
    void closeSocket();

protected:
    std::map<int, TraCIScopeWrapper*> myDomains;
    /// @brief The socket
    tcpip::Socket* mySocket;
    /// @brief The reusable output storage
    mutable tcpip::Storage myOutput;
    /// @brief The reusable input storage
    mutable tcpip::Storage myInput;
};


#endif

/****************************************************************************/
