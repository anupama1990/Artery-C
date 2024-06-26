find_package(Boost 1.59 COMPONENTS date_time system REQUIRED)

add_library(artery INTERFACE)
add_library(core SHARED
    StaticNodeManager.cc
    application/CaObject.cc
    application/CaService.cc
    application/CpObject.cc
    application/CpService.cc
    application/DenmObject.cc
    application/DenService.cc
    application/ExampleService.cc
    application/FilterObjects.cc
    application/ObjectInfo.cpp
    application/ItsG5BaseService.cc
    application/ItsG5PromiscuousService.cc
    application/ItsG5Service.cc
    application/LocalDynamicMap.cc
    application/Middleware.cc
    application/MultiChannelPolicy.cc
    application/NetworkInterface.cc
    application/NetworkInterfaceTable.cc
    application/StationaryMiddleware.cc
    application/StoryboardSignal.cc
    application/Timer.cc
    application/TransportDispatcher.cc
    application/VehicleDataProvider.cc
    application/VehicleMiddleware.cc
    application/XmlMultiChannelPolicy.cc
    application/den/ImpactReductionUseCase.cc
    application/den/Memory.cc
    application/den/SuspendableUseCase.cc
    application/den/TrafficJamUseCase.cc
    application/den/UseCase.cc
    networking/AccessInterface.cc
    networking/DccEntityBase.cc
    networking/FsmDccEntity.cc
    networking/GeoNetPacket.cc
    networking/LimericDccEntity.cc
    networking/NoRateControlDccEntity.cc
    networking/Router.cc
    networking/Runtime.cc
    networking/SecurityEntity.cc
    networking/StationaryPositionProvider.cc
    networking/VehiclePositionProvider.cc
    nic/ChannelLoadSampler.cc
	nic/RadioDriver.cc
    nic/RadioDriverBase.cc  
    nic/ChannelLoadMeasurements.cc
    traci/MobilityBase.cc
    traci/ControllableVehicle.cc
    traci/MovingNodeController.cc
    traci/VehicleController.cc
    traci/VehicleType.cc
    utility/AsioScheduler.cc
    utility/AsioTask.cc
    utility/Channel.cc
    utility/Identity.cc
    utility/IdentityRegistry.cc
    utility/FilterRules.cc
    utility/Geometry.cc
)
target_link_libraries(artery INTERFACE core)
add_library(Artery::Core ALIAS core)

generate_opp_message(utility/AsioData.msg TARGET core)
generate_opp_message(nic/PoiRetrievalModule.msg TARGET core)

set_target_properties(core PROPERTIES
    OUTPUT_NAME artery_core
    OMNETPP_LIBRARY ON
    NED_FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(core PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_include_directories(core PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(core PUBLIC ${Boost_LIBRARIES})
target_link_libraries(core PUBLIC opp_interface)
target_link_libraries(core PUBLIC traci)
target_link_libraries(core PUBLIC vanetza)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/extern/Simu5G/src")
#target_link_libraries(artery INTERFACE simu5g)
if(TARGET veins)
    message(STATUS "Enable Veins integration")
    set_property(TARGET core APPEND PROPERTY SOURCES
    #veins/VeinsConnectionManager.cc
    #veins/VeinsMobility.cc
    #veins/VeinsObstacleControl.cc
    #veins/VeinsRadioDriver.cc
    )
   target_link_libraries(core PUBLIC veins)
endif()

#target_link_libraries(core PUBLIC veins)

if(TARGET INET)
    message(STATUS "Enable INET integration")
    set_property(TARGET core APPEND PROPERTY SOURCES

        inet/AntennaMobility.cc
        inet/DistanceSwitchPathLoss.cc
       # inet/InetMobility.cc
        
        inet/VanetRx.cc
       # inet/InetRadioDriver.cc
    )
    target_link_libraries(core PUBLIC INET)
endif()

macro(add_artery_feature name)
    add_library(${name} SHARED ${ARGN})
    add_library(Artery::${name} ALIAS ${name})
    set_target_properties(${name} PROPERTIES
        OUTPUT_NAME artery_${name}
        OMNETPP_LIBRARY ON)
    target_link_libraries(${name} PRIVATE core)
    target_link_libraries(artery INTERFACE Artery::${name})
endmacro()



if(WITH_ENVMOD)
    add_subdirectory(envmod)
    set_property(SOURCE application/VehicleMiddleware.cc APPEND PROPERTY COMPILE_DEFINITIONS "WITH_ENVMOD")
endif()



