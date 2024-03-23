package artery.lte;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.ethernet.Eth10G;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import simu5g.common.binder.Binder;
import simu5g.nodes.NR.gNodeB;
import simu5g.nodes.PgwStandard;
import simu5g.world.radio.LteChannelControl;
import simu5g.nodes.Upf;

network World extends artery.inet.World
{
    parameters:
        int numLteBaseStations = default(2);
        int numRoadSideUnitsEnB = default(1);
        traci.mapper.vehicleType = default("artery.lte.Car");
        
        @display("bgb=1047.4475,679.8825");
        
    submodules:
        binder: Binder {
            parameters:
                @display("p=140,60");
        }
        channelControl: LteChannelControl {
            parameters:
                @display("p=140,20");
        }     
    
        
        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=60,60");
        }
        
        eNodeB[numLteBaseStations]: gNodeB {
            parameters:
                mobility.initFromDisplayString = false;
                macNodeId = default(index + 1);
                macCellId = default(index + 1);
                **.eNodeBCount = default(numLteBaseStations);
                @display("p=264.9625,299.915");
        }
        upf: Upf {
                @display("p=462,136");
        }
        
        
        server: StandardHost {
                @display("p=220,20;is=n;i=device/server");
        }
        
        router: Router {
                @display("p=219.8625,164.615;i=device/smallrouter");
        }
        
        
    connections allowunconnected:
        for i=0..numLteBaseStations-2 {
                eNodeB[i].x2++ <--> Eth10G <--> eNodeB[i+1].x2++;
        }
        
        for i=0..numLteBaseStations-1 {
                upf.pppg++ <--> Eth10G <--> eNodeB[i].ppp;
        }
        
        server.pppg++ <--> Eth10G <--> router.pppg++;
        router.pppg++ <--> Eth10G <--> upf.filterGate;
        
        
}
