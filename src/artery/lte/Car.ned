package artery.lte;

import inet.applications.contract.ITCPApp;
import inet.applications.contract.IUDPApp;
import inet.networklayer.configurator.ipv4.HostAutoConfigurator;
import inet.networklayer.contract.IRoutingTable;
import inet.networklayer.contract.INetworkLayer;
import inet.transportlayer.tcp.TCP;
import inet.transportlayer.udp.UDP;
import lte.stack.LteNicUeD2D;
import inet.networklayer.common.InterfaceTable;
import artery.nic.RadioDriver;
import artery.application.VehicleMiddleware;
import artery.networking.Vanetza;
import inet.networklayer.common.InterfaceTable;
import inet.mobility.contract.IMobility;
import inet.node.inet.INetworkNode;
import lte.apps.nonip.AlertNonIp;



module Car
{
    parameters:
        @display("i=block/wrxtx;is=vs;bgb=788,631");
        @networkNode;
        @labels(node,lte-node,wireless-node);
        // INET network layer
        *.routingTableModule = default(absPath(".routingTable"));
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        mobility.visualRepresentation = "^";
        bool ipTraffic;
        // LTE NIC
        string nicType = default("LteNicUeD2D");
        int macNodeId = default(0);
        int masterId;
        
        // application
        int numUdpApps = default(0);
        int numTcpApps = default(0);
        
    gates:
        input lteRadioIn @directIn;
        input control_IN @directIn;
        
    submodules:
        radioDriverLte: RadioDriver  {
            parameters:
                @display("p=411,261");
                *.traciCoreModule = "traci.core";
        }
        vanetza: Vanetza {
            parameters:
                @display("p=411,175");
                *.middlewareModule = absPath("^.middleware");
                *.mobilityModule = absPath("^.mobility");
                *.radioDriverModule = absPath("^.radioDriverLte");
                runtime.datetime = middleware.datetime;
        }
        //
        middleware: VehicleMiddleware {
            parameters:
                @display("p=250,261");
                mobilityModule = ".mobility";
        }
        
        
        //        nonipapp: AlertNonIp {
        //            @display("p=518.16003,431.80002");
        //        }
        mobility: <default("artery.inet.Mobility")> like IMobility {
            parameters:
                @display("p=50,200");
        }
        
        
        lteNic: LteNicUeD2D {
                @display("p=519,307");
                nodeType = "UE";
        }
        
        
        configurator: HostAutoConfigurator {
                @display("p=50,25");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=50,300");
        }
        routingTable: <default("IPv4RoutingTable")> like IRoutingTable {
                @display("p=50,125");
        }
        
        
        networkLayer: <default("IPv4NetworkLayer")> like INetworkLayer {
                @display("p=519,184");
        }
        
        udpApp[numUdpApps]: <> like IUDPApp {
                @display("p=462,33,col,-100");
        }
        
        udp: UDP if numUdpApps > 0 {
                @display("p=462,103");
        }
        
        tcpApp[numTcpApps]: <> like ITCPApp {
                @display("p=595,33,col,-100");
        }
        
        tcp: TCP if numTcpApps > 0 {
                @display("p=595,103");
        }
        
    connections allowunconnected:
        lteNic.radioIn <-- lteRadioIn; //user plane
        lteNic.control_IN <-- control_IN; //control plane
        
        //#IP traffic
        //if ipTraffic==true {
        networkLayer.ifOut++ --> lteNic.upperLayerIn;
        networkLayer.ifIn++ <-- lteNic.upperLayerOut;
        // }
        for i=0..numUdpApps-1 {
                udpApp[i].udpOut --> udp.appIn++;
                udpApp[i].udpIn <-- udp.appOut++;
        }
        
        udp.ipOut --> networkLayer.transportIn++ if numUdpApps > 0;
        udp.ipIn <-- networkLayer.transportOut++ if numUdpApps > 0;
        
        for i=0..numTcpApps-1 {
                tcpApp[i].tcpOut --> tcp.appIn++;
                tcpApp[i].tcpIn <-- tcp.appOut++;
        }
        
        // if ipTraffic==true {
        tcp.ipOut --> networkLayer.transportIn++ if numTcpApps > 0;
        tcp.ipIn <-- networkLayer.transportout++ if numTcpApps > 0;
        
        //}
        //#Non-IP traffic
        // if ipTraffic == false {
        radioDriverLte.upperLayer$o --> vanetza.radioDriverData$i;
        vanetza.radioDriverData$o --> radioDriverLte.upperLayer$i;
        //
        //            //  radioDriver.upperLayer <--> vanetza.radioDriverData;
        radioDriverLte.properties --> vanetza.radioDriverProperties;
        
        radioDriverLte.lowerLayerOut --> lteNic.upperLayerNonIpIn;
        radioDriverLte.lowerLayerIn <-- lteNic.upperLayerNonIpOut;
        
        //}
}
