#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
//


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Open5GSUdpClientExample");

int main(int argc, char *argv[]) {
    // Enable logging for this example
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // Set the IP address and port of the Open5GS AMF
    std::string amfIp = "127.0.0.5";
    uint16_t amfPort = 38412; 

    // Create a node for the eNB
    NodeContainer enbNodes;
    enbNodes.Create(1);

    // Install networking stack on the eNB node
    InternetStackHelper internet;
    internet.Install(enbNodes);
/////


    // Set up IP address and network
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = ipv4.Assign(NetDeviceContainer());

    // Create a UDP client application to send messages to the AMF
    uint32_t packetSize = 1024; // Size of the packet sent to AMF
    Time interPacketInterval = Seconds(1.0); // Time between packets
    UdpEchoClientHelper echoClient(Ipv4Address(amfIp.c_str()), amfPort);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(interPacketInterval));
    echoClient.SetAttribute("PacketSize", UintegerValue(packetSize));

    ApplicationContainer clientApps = echoClient.Install(enbNodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    // Run the simulation
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}



