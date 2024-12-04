#include "ns3/applications-module.h"
#include "ns3/fd-net-device-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"

#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("UdpServerRealDevice");

void UdpServerReceive(Ptr<Socket> socket)
{
    Ptr<Packet> packet;
    Address from;
    Address localAddress;
    while ((packet = socket->RecvFrom(from)))
    {
        socket->GetSockName(localAddress);
        if (packet->GetSize() > 0)
        {
            uint32_t receivedSize = packet->GetSize();
            uint8_t data[receivedSize+1];
            packet->CopyData(data, receivedSize);
            data[receivedSize] = '\0';
            NS_LOG_INFO("Received UDP packet, size: " << receivedSize << ", data: " << data);
        }
    }
}

int main(int argc, char* argv[])
{
    NS_LOG_INFO("UDP server listen on real device example for UDP traffic on port 2152");
    std::string deviceName("enp0s9"); // Replace with the appropriate interface name
    std::string localAddress("192.168.33.3"); // Replace with the appropriate IP address
    std::string localGateway("192.168.33.3");
    uint16_t port = 2152; // GTP-U port

    CommandLine cmd(__FILE__);
    cmd.AddValue("deviceName", "Device name", deviceName);
    cmd.AddValue("localIp", "Local IP address (dotted decimal only please)", localAddress);
    cmd.AddValue("gateway", "Gateway address (dotted decimal only please)", localGateway);
    cmd.Parse(argc, argv);

    Ipv4Address localIp(localAddress.c_str());
    Ipv4Mask localMask("255.255.255.0");

    GlobalValue::Bind("SimulatorImplementationType", StringValue("ns3::RealtimeSimulatorImpl"));
    GlobalValue::Bind("ChecksumEnabled", BooleanValue(true));

    Ptr<Node> node = CreateObject<Node>();

    bool logging = true;
    if (logging)
    {
        LogComponentEnable("UdpServerRealDevice", LOG_LEVEL_INFO);
    }

    FdNetDeviceHelper* helper = nullptr;
    auto raw = new EmuFdNetDeviceHelper;
    raw->SetDeviceName(deviceName);
    helper = raw;

    if (helper == nullptr)
    {
        NS_ABORT_MSG("Emulation mode not supported.");
    }

    NetDeviceContainer devices = helper->Install(node);
    Ptr<NetDevice> device = devices.Get(0);
    device->SetAttribute("Address", Mac48AddressValue(Mac48Address("08:00:27:1b:34:42"))); // Replace with the appropriate MAC address

    InternetStackHelper internetStackHelper;
    internetStackHelper.Install(node);

    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
    uint32_t interface = ipv4->AddInterface(device);
    Ipv4InterfaceAddress address = Ipv4InterfaceAddress(localIp, localMask);
    ipv4->AddAddress(interface, address);
    ipv4->SetMetric(interface, 1);
    ipv4->SetUp(interface);

    Ptr<SocketFactory> socketFactory = node->GetObject<UdpSocketFactory>();
    Ptr<Socket> socket = socketFactory->CreateSocket();
    socket->Bind(InetSocketAddress(Ipv4Address::GetAny(), port));
    socket->SetRecvCallback(MakeCallback(&UdpServerReceive));

    NS_LOG_INFO("Run Emulation in raw mode.");
    Simulator::Run();
    //Simulator::Destroy();
    NS_LOG_INFO("Done.");

    return 0;
}
