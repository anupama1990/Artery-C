# Artery-C

Cellular Vehicle-to-X (Cellular V2X) is a communication technology that aims to facilitate the communication among vehicles and with the roadside infrastructure. Introduced with LTE Release 14,Cellular V2X enables device-to-device communication to support road safety and traffic efficiency applications. We present ``Artery-C``, a simulation framework for the performance evaluation of Cellular V2X protocols and V2X applications. Our simulator relies on the simulation framework ``SimuLTE`` and substantially extends it by implementing a dedicated sidelink interface, sidelink resource allocation modes along with control and user planes. Artery-C integrates seamlessly with Artery framework which enables the simulation of standardized V2X messages at the facilities layer as well as the coupling to the mobility simulator SUMO. A specific feature of Artery-C is the support of dynamic switching between all modes of Cellular V2X.

# Relevant Literature
For further details about the salient features of Artery-C simulator and sidelink mode switching, please refer to the publications/documents stored here:

-[Documents](https://github.com/anupama1990/Documents.git)

Further details on the implementation of Artery can be found here:

-[Artery](https://github.com/riebl/artery.git)


## Requirements

Artery-C currently uses the following software packages (please note the specific version):
* C++ Compiler with C++11 support
* Boost 1.65.1 and Vanetza libraries
* Ubuntu 18.04
* SUMO 1.7.10 
* OMNeT++ 5.6 
* INET 3.6.5
* Pybind 11


Compatible versions of INET, Veins, Vanetza, and other components are managed as [git submodules](https://git-scm.com/docs/git-submodule) in the *extern* subdirectory.
It is recommended to clone the dependencies before building Artery-C.

More recent versions of *INET*, *Veins* and *Vanetza* can be downloaded here:

- [INET](https://github.com/inet-framework/inet)
- [Veins](https://github.com/sommer/veins)
- [Vanetza](https://github.com/riebl/vanetza)


## Build instructions
The first step involves building the dependencies from the Artery-C's root directory. The ``Makefile`` contains rules for building all the dependencies by executing ``make all`` (for release mode) and ``make all MODE=debug`` (for debug mode).

To build Artery-C in release mode:

	mkdir build
	cd build
	mkdir build
	make -j4

To run the target:
``make run_tunnel``
	

To build Artery-C in debug mode:
	 
	mkdir build
	cd build
	cmake ..
	ccmake .
	Under "CMAKE_BUILD_TYPE", press enter and type "Debug"
	Press "c" to configure and "g" to generate and quit
	
To run the target:
	``make debug_tunnel``

## Installing and running SUMO with Artery-C

First install the following dependencies:
	sudo apt-get install cmake python g++ libxerces-c-dev libfox-1.6-dev libgdal-dev libproj-dev libgl2ps-dev swig

Install SUMO from source code:

	git clone --recursive https://github.com/eclipse/sumo
 	export SUMO_HOME="$PWD/sumo"
 	mkdir sumo/build/cmake-build && cd sumo/build/cmake-build
 	cmake ../..
 	make -j$(nproc)

Installing ``SUMO-GUI`` related files with most recent version of SUMO (currently 1.7.10):

	sudo add-apt-repository ppa:sumo/stable
	sudo apt-get update
	sudo apt-get install sumo sumo-tools sumo-doc

Artery-C automatically starts SUMO when the target ``make run_tunnel`` or ``debug_tunnel`` is executed. If you want Artery-C to start SUMO with a graphical user interface, you can put the following line in your *omnetpp.ini*:

    *.traci.launcher.sumo = "sumo-gui"
