package artery.veins;

import artery.application.IMiddleware;
import artery.networking.IVanetza;
import artery.nic.IRadioDriver;
import org.car2x.veins.base.modules.IBaseApplLayer;

// Middleware as Veins compatiblity application layer
module ApplicationLayer like IBaseApplLayer
{
	parameters:
		int headerLength @unit("bit") = 0B; // ignored

	gates:
		input lowerLayerIn;
		output lowerLayerOut;
		input lowerControlIn;
		output lowerControlOut;

	submodules:
		radioDriver : RadioDriver {
		}

		vanetza: <default("Vanetza")> like IVanetza {
			*.middlewareModule = absPath("^.middleware");
			*.radioDriverModule = absPath("^.radioDriver");
			runtime.datetime = middleware.datetime;
		}

		middleware : <default("VehicleMiddleware")> like IMiddleware {
			routerModule = absPath("^.vanetza.router");
		}

	connections allowunconnected:
		lowerLayerIn --> radioDriver.lowerLayerIn;
		lowerLayerOut <-- radioDriver.lowerLayerOut;

		radioDriver.upperLayer <--> vanetza.radioDriverData;
		radioDriver.properties --> vanetza.radioDriverProperties;
}
