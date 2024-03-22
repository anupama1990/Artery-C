project(Artery)
cmake_minimum_required(VERSION 3.1)
if(POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif()
if(POLICY CMP0078)
    cmake_policy(SET CMP0079 OLD)
endif()
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall --std=c++14 -O3 -fPIC" )
set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -O3 -fPIC" )

enable_testing()


find_package(OmnetPP 6.0 REQUIRED)
include(cmake/ImportOppTarget.cmake)
include(cmake/GenerateOppMessage.cmake)
include(cmake/GetNedFolders.cmake)
include(cmake/AddOppBuildTarget.cmake)
include(cmake/AddOppRun.cmake)

find_package(Vanetza CONFIG PATHS extern/vanetza/build REQUIRED NO_CMAKE_PACKAGE_REGISTRY)
set(VANETZA_INSTALL ON)
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/vanetza)
mark_as_advanced(BUILD_BENCHMARK BUILD_CERTIFY BUILD_SOCKTAP BUILD_TESTS BUILD_USING_CONAN)
mark_as_advanced(VANETZA_ASN1_WITH_ASN1C VANETZA_EXPORT_PACKAGE VANETZA_NET_WITH_POSIX)

find_path(Veins_DIR NAMES src/veins/package.ned PATHS extern/veins DOC "Veins root directory")
import_opp_target(veins ${Veins_DIR}/src/Makefile)

find_path(INET_DIR NAMES src/inet/package.ned PATHS extern/inet4.5 DOC "INET root directory")
import_opp_target(INET ${INET_DIR}/src/Makefile)


find_path(Simu5G_DIR NAMES src/package.ned PATHS extern/Simu5G DOC "Cellular-V2X root directory")
import_opp_target(simu5g ${Simu5G_DIR}/src/Makefile)


find_program(MAKE_COMMAND NAMES make gmake HINTS ${CMAKE_MAKE_COMMAND} DOC "Makefile processor")
set(MAKE_ARGUMENTS "" CACHE STRING "Additional arguments passed to Makefile processor")
string(REPLACE " " ";" MAKE_ARGUMENTS "${MAKE_ARGUMENTS}")

if("${Veins_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}/extern/veins")
    add_opp_build_target(Veins)
    add_dependencies(veins build_veins)
endif()

if("${INET_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}/extern/inet4.5")
    add_opp_build_target(INET)
    add_dependencies(INET build_inet)
endif()

if("${Simu5G_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}/extern/Simu5G")
message("test simu5g")
	add_opp_build_target(Simu5G)
	add_dependencies(simu5g build_simu5g)
	
endif()

option(WITH_ENVMOD "Build Artery with environment model feature" ON)
option(WITH_STORYBOARD "Build Artery with storyboard feature" ON)
option(WITH_TRANSFUSION "Build Artery with transfusion feature" OFF)
option(WITH_TESTBED "Build Artery with testbed feature" OFF)

add_subdirectory(src/artery)
add_subdirectory(src/traci)
add_subdirectory(scenarios)

# sumo-launchd should be run from root directory for correct file look-up
set(SUMO_LAUNCHD_ARGUMENTS "--daemon --kill" CACHE STRING "Additional arguments passed to sumo-launchd")
string(REPLACE " " ";" _sumo_launchd_args "${SUMO_LAUNCHD_ARGUMENTS}")
add_custom_target(launch_sumo
    COMMAND ${Veins_DIR}/sumo-launchd.py ${_sumo_launchd_args}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Launching SUMO"
    VERBATIM)
    
   
  
    
    
