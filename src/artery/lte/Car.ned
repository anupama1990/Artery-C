package artery.lte;


import inet.networklayer.configurator.ipv4.HostAutoConfigurator;
import inet.networklayer.contract.IRoutingTable;
import inet.networklayer.contract.INetworkLayer;
import lte.stack.LteNicUeD2D;
import inet.networklayer.common.InterfaceTable;
import artery.nic.RadioDriver;
import artery.application.VehicleMiddleware;
import artery.networking.Vanetza;
import inet.networklayer.common.InterfaceTable;
import inet.mobility.contract.IMobility;
import inet.node.inet.INetworkNode;
import lte.apps.nonip.AlertNonIp;
import artery.inet.Mobility;
import inet.networklayer.ipv4.Ipv4RoutingTable;
import inet.transportlayer.contract.ITcp;
import inet.transportlayer.contract.IUdp;
import inet.transportlayer.contract.ISctp;
import inet.common.MessageDispatcher;
import inet.common.packet.recorder.PcapRecorder;
import inet.linklayer.contract.IExternalInterface;
import inet.applications.contract.IApp;

module Car 
{
    parameters:
        @display("i=block/wrxtx;is=vs;bgb=788,631");
        @networkNode;
        @labels(node,lte-node,wireless-node);
        // INET network layer
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        *.routingTableModule = default(absPath(".routingTable"));
        mobility.visualRepresentation = "^";
        bool ipTraffic;
        // LTE NIC
        string nicType = default("LteNicUeD2D");
        int macNodeId = default(0);
        int masterId;
        
        // application
        int numUdpApps = default(0);
        int numTcpApps = default(0);
        int numApps = default(0);
        bool hasIpv4 = default(true);
        bool hasUdp = default(firstAvailableOrEmpty("Udp") != "");
        bool hasTcp = default(firstAvailableOrEmpty("Tcp", "TcpLwip", "TcpNsc") != "");
        bool hasSctp = default(false);
        
        //# Ext Interface
        bool enableExtInterface = default(false);
        string extHostAddress = default("");
        
        int numPcapRecorders = default(0); // no of PcapRecorders
        
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
                routerModule = ".vanetza.router";
                mobilityModule = ".mobility";
        }
        
        
        //        nonipapp: AlertNonIp {
        //            @display("p=518.16003,431.80002");
        //        }
        mobility: Mobility  {
            parameters:
                @display("p=50,200");
        }
        
        
        lteNic: LteNicUeD2D {
                @display("p=519,307");
                nodeType = "UE";
        }
            
       
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=50,300");
        }
        routingTable:Ipv4RoutingTable{
        }
            
        ipv4: <default("Ipv4NetworkLayer")> like INetworkLayer  {
            parameters:
                @display("p=375,376;q=queue");
        }
        
        
        // transport layer
        udp: <default(firstAvailableOrEmpty("Udp"))> like IUdp if hasUdp {
            parameters:
                @display("p=375,226");
        }
        tcp: <default(firstAvailableOrEmpty("Tcp", "TcpLwip", "TcpNsc"))> like ITcp if hasTcp {
            parameters:
                @display("p=525,226");
        }
        sctp: <default(firstAvailableOrEmpty("Sctp"))> like ISctp if hasSctp {
            parameters:
                @display("p=675,226");
        }
        // application layer
        app[numApps]: <> like IApp {
            @display("p=375,76,row,140");
        }

        // message dispatcher for SAP between application and transport layer
        at: MessageDispatcher {
            parameters:
                @display("p=550,146;b=600,5,,,,1");
        }
        // message dispatcher for SAP between transport and network layer
        tn: MessageDispatcher {
            parameters:
                @display("p=550,296;b=600,5,,,,1");
        }
        // message dispatcher for SAP to link layer
        nl: MessageDispatcher {
            parameters:
                @display("p=550,446;b=600,5,,,,1");
        }
                
        ext: <default("ExtInterface")> like IExternalInterface if enableExtInterface {
            parameters:
                @display("p=110.664,406.116,row,10;q=txQueue;i=block/ifcard");
        }

        pcapRecorder[numPcapRecorders]: PcapRecorder {
            @display("p=53,350,r,10");
        }
    connections allowunconnected:
        lteNic.radioIn <-- lteRadioIn; //user plane
        lteNic.control_IN <-- control_IN; //control plane
        
        //#IP traffic
        //if ipTraffic==true {
       nl.out++ --> lteNic.upperLayerIn;
       nl.in++ <-- lteNic.upperLayerOut;
        // }
//# 
        //# Internal Tcp/Udp applications connections with transport layer
        //#

        for i=0..numApps-1 {
            app[i].socketOut --> at.in++;
            app[i].socketIn <-- at.out++;
        }

        at.out++ --> udp.appIn if hasUdp;
        at.in++ <-- udp.appOut if hasUdp;

        at.out++ --> tcp.appIn if hasTcp;
        at.in++ <-- tcp.appOut if hasTcp;

        at.out++ --> sctp.appIn if hasSctp;
        at.in++ <-- sctp.appOut if hasSctp;

        //# 
		//# Internal transport layer connections to network layer
		//# 

        udp.ipOut --> tn.in++ if hasUdp;
        udp.ipIn <-- tn.out++ if hasUdp;

        tcp.ipOut --> tn.in++ if hasTcp;
        tcp.ipIn <-- tn.out++ if hasTcp;

        sctp.ipOut --> tn.in++ if hasSctp;
        tn.out++ --> sctp.ipIn if hasSctp;

        ipv4.ifIn <-- nl.out++ if hasIpv4;
        ipv4.ifOut --> nl.in++ if hasIpv4;

        ipv4.transportIn <-- tn.out++ if hasIpv4;
        ipv4.transportOut --> tn.in++ if hasIpv4;

        tn.out++ --> nl.in++;
        tn.in++ <-- nl.out++;

        //#
        //# Connections between Ext interface and IP layer 
        //#
        nl.out++ --> ext.upperLayerIn if enableExtInterface;
        nl.in++ <-- ext.upperLayerOut if enableExtInterface;

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
