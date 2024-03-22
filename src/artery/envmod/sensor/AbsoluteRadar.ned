//
// Artery V2X Simulation Framework
// Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.envmod.sensor;

simple AbsoluteRadar like Radar
{
    parameters:
        double fovRange @unit(m) = default(150.0 m);
        double fovAngle = default(360.0); // degree
        int numSegments = default(36);
        bool doLineOfSightCheck = default(true);
}
