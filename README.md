# Artery-C

Artery-C enables V2X simulations based on ETSI ITS-G5 protocols like GeoNetworking and BTP.
Single vehicles can be equipped with multiple ITS-G5 services through Artery's middleware, which also provides common Facilities for these services.

Artery started as an extension of the [Veins framework](http://veins.car2x.org) but can be used independently nowadays.
Please refer to its [documentation](http://veins.car2x.org/documentation) for details about Veins.

## Requirements
You need a C++ Compiler with C++11 support, Boost and Vanetza libraries for building Artery along with Veins.
Artery and Veins build upon the discrete event simulator [OMNeT++](https://omnetpp.org), which you need to obtain as well.
We have tested Artery with OMNeT++ 5.4, GNU GCC 7.3 and Boost 1.65.1 successfully.
SUMO 1.0 or later is required since its TraCI protocol is not compatible with earlier versions anymore.
Only [CMake](http://www.cmake.org) is the officially supported way for building Artery.

Compatible versions of INET, Veins, Vanetza, and other components are managed as [git submodules](https://git-scm.com/docs/git-submodule) in the *extern* subdirectory.
Please make sure to fetch these submodules when cloning Artery-C repository!

    git clone --recurse-submodule https://github.com/riebl/artery.git

Alternatively, you can also load these submodules after cloning:

    git submodule update --init --recursive

You might obtain more recent versions from their upstream repositories:

- [INET](https://github.com/inet-framework/inet)
- [Veins](https://github.com/sommer/veins)
- [Vanetza](https://github.com/riebl/vanetza)


## Build instructions
The first step involves building the dependencies from the Artery-C's root directory. The ``Makefile`` contains rules for building all the dependencies by executing 
	``make all`` \
	``mkdir build``\
	``cd build``\
	``mkdir build``\
	``make -j4``\

To run the target:\
``make run_tunnel``\
	

To build specifically in debug mode:\
	``make all MODE=debug`` \
	``mkdir build``\
	``cd build``
	``ccmake .``
	Under ``CMAKE_BUILD_TYPE``, press enter and type ``Debug``\
	Press ``c`` to configure and ``g`` to generate and quit\
	
	To run the target:\
``make debug_tunnel``\

## How Artery-C starts SUMO



Please make sure that *sumo* can be executed within in your environment because this is the default SUMO executable used by Artery.
You can, however, specify which SUMO executable shall be used explicilty.
If you want Artery to start SUMO with a graphical user interface, you can put the following line in your *omnetpp.ini*:

    *.traci.launcher.sumo = "sumo-gui"
