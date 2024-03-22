package artery.inet.gemv2;

import inet.physicallayer.wireless.common.pathloss.FreeSpacePathLoss;

module NLOSf extends FreeSpacePathLoss
{
    parameters:
        @display("i=block/control");
        @class(gemv2::NLOSf);
        string foliageIndexModule;
        string visualizerModule;
}
