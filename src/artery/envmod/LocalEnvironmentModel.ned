//
// Artery V2X Simulation Framework
// Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.envmod;

simple LocalEnvironmentModel
{
    parameters:
        @display("i=abstract/db");
        @signal[timeBetweenUpdates];
        @statistic[updateTime](source=timeBetweenUpdates);
        xml sensors = default(xml("<sensors />"));
        string globalEnvironmentModule;
        string middlewareModule;
}

