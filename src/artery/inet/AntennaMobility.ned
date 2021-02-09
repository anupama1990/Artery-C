package artery.inet;

import inet.mobility.contract.IMobility;
import inet.mobility.single.AttachedMobility;

simple AntennaMobility extends AttachedMobility
{
    parameters:
        @class(AntennaMobility);
        @signal[mobilityStateChanged];


        double offsetAlpha @unit(rad) = default(0 rad);
        double offsetBeta @unit(rad) = default(0 rad);
        double offsetGamma @unit(rad) = default(0 rad);
}
