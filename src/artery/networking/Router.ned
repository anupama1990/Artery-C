package artery.networking;

simple Router
{
    parameters:
        @class(Router);
        @signal[IdentityChanged](type=long);

        string dccModule;
        string middlewareModule;
        string radioDriverModule;
        string runtimeModule;
        string securityModule;

		// Management Information Base (MIB)
		int itsGnDefaultTrafficClass = default(3);
		bool vanetzaDisableBeaconing = default(false); 
		bool deferInitialBeacon = default(true);
        bool isMobile = default(true);
        double itsGnBeaconServiceRetransmitTimer @unit(s) = default(3.0s);
    	double itsGnBeaconServiceMaxJitter @unit(s) = default(0.75s);
    gates:
        input radioDriverData;
        input radioDriverProperties;
}
