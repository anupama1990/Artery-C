# Artery-C
Network simulations provide the opportunity to evaluate the performance of different communication protocols and system architectures (for example, centralized, decentral-
ized, hybrid etc.) before deploying them in the real world settings. For a vehicular adhoc network (VANET), a network simulator provides a common platform for designing and
understanding a complete system that consist of two key components: (i) Communication network and (ii) Road traffic model. Cellular Vehicle-to-X (Cellular V2X) is a communication technology that aims to facilitate the communication among vehicles and with the roadside infrastructure. Introduced with LTE Release 14, Cellular V2X enables device-to-device communication to support road safety and traffic efficiency applications. We present ``Artery-C``, a simulation framework for the performance evaluation of Cellular V2X protocols for different types of V2X applications. 

## Requirements

Artery-C currently uses the following software packages (please note the specific version):
* C++ Compiler with C++11 support
* Boost 1.65.1 and Vanetza libraries
* Ubuntu 18.04 onwards
* SUMO 1.7.10 
* OMNeT++ 6.0 
* INET 4.5
* Pybind 11 (optional)
* Debugging - GNU GDB 8.1.1

The simulation framework is organized as follows:
- [Facilities layer] - comprises of the modules corresponding to different types of V2X messaging services and protocols in the facilities layer - cooperative awareness (CA) service, collective perception service (CPS), Decentralized environmental notification (DEN) service. Further extensions to other messaging services are possible to be implemented here. Currently, it re-uses the components from the ``Artery`` framework. Please refer to the path ``src/artery``.
- [Network & Transport layer] - implementation of BTP and Geonetworking protocols. Currently, it re-uses the components from the ``Vanetza`` framework. Please refer to the path ``extern/vanetza``.
- [Access layers] - comprises of the modules corresponding to the access layers of the Cellular V2X protocol stack - PDCP, RLC, MAC and PHY. Additionally, an implementation of the radio resource control (RRC) module has been provided. It was initially develeloped by implementing a dedicated sidelink interface, sidelink resource allocation modes along with control and user planes as an extension to the ``SimuLTE`` framework. It is now upgraded to support integration with ``Simu5G`` and all the 5G-NR standard compliant features of the Cellular V2X protocol stack. Different types of wireless channel models to support system level simulations are also included here. Please refer to the path ``extern/Simu5G``.
- [INET] (https://github.com/inet-framework/inet) (located in the path ``extern/inet4.5``).
- [Road traffic environments] - different types of road traffic scenarios are simulated at a microscopic level within the ``SUMO`` framework. The specific road traffic scenarios are loacted in the ``scenarios`` folder and the simulation settings can be modified in the corresponding ``*.xml`` and ``omnetpp.ini`` files within each folder. Other suitable scenarios can be added here.
- [TraCI API] - support bi-directional communication between the network simulator ``Artery-C`` and the road traffic simulator ``SUMO``. Please refer to the path ``src/traci``. To retrive additional information from the road traffic model into the network simulator ``Artery-C``, specific functions in the files (located in the path ``src/traci``) can be accordingly modified.

Note: It is recommended to align with the versions of the INET*, *Veins* and *Vanetza* as provided in the ``extern`` folder which get downloaded upon cloning the ``Artery-C`` framework. However recent versions of *INET*, *Veins* and *Vanetza* can be downloaded here:

- [INET](https://github.com/inet-framework/inet)
- [Veins](https://github.com/sommer/veins)
- [Vanetza](https://github.com/riebl/vanetza)

## Build instructions
The first step involves building the dependencies from the Artery-C's root directory. The ``Makefile`` contains rules for building all the dependencies by executing ``make all`` (for release mode) and ``make all MODE=debug`` (for debug mode).

To build Artery-C in release mode:

	mkdir build
	cd build
	cmake ..
	make -j4

To run the target "tunnel" in release mode:
``make run_tunnel``
	

To build Artery-C in debug mode:
	 
	mkdir build
	cd build
	cmake ..
	ccmake .
	Under "CMAKE_BUILD_TYPE", press enter and type "Debug"
	Press "c" to configure and "g" to generate and quit
	
To run the target "tunnel" in debug mode:
	``make debug_tunnel``

This takes you to the GDB prompt - type ``run``. In the ``GDB`` prompt mode, you can further insert breakpoints in order to step into specific lines of code. This build type is particularly suited for development work where it becomes easy to identify errors by traversing step-by-step into the code.


## Installing and running SUMO with Artery-C

First install the following dependencies:

	``sudo apt-get install cmake python g++ libxerces-c-dev libfox-1.6-dev libgdal-dev libproj-dev libgl2ps-dev swig``

Install ``SUMO`` from source code:

	git clone --recursive https://github.com/eclipse/sumo
 	export SUMO_HOME="$PWD/sumo"
 	mkdir sumo/build/cmake-build && cd sumo/build/cmake-build
 	cmake ../..
 	make -j$(nproc)

Installing ``SUMO-GUI`` related files with most recent version of SUMO (currently 1.7.10):

	sudo add-apt-repository ppa:sumo/stable
	sudo apt-get update
	sudo apt-get install sumo sumo-tools sumo-doc

Artery-C automatically starts ``SUMO`` when the target ``make run_tunnel`` or ``debug_tunnel`` is executed. If you want Artery-C to start ``SUMO`` with a graphical user interface, you can put the following line in your *omnetpp.ini*:

    *.traci.launcher.sumo = "sumo-gui"
    
## To cite Artery-C:
- [1] A. Hegde and A. Festag, “Artery-C – An OMNeT++ Based Discrete Event Simulation Framework for Cellular V2X,” in ACM Conference on Modeling, Analysis and Simulation of Wireless and Mobile Systems (MSWIM), Alicante, Spain, Nov. 2020, pp. 450–456, doi: 10.1145/3416010.3423240, © 2023 ACM, Inc. Extended version on arXiv: (URL:) http://arxiv.org/abs/2009.05724.
