/*
 * Copyright (c) 2024 Nokia Bell Labs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: THT <huu_trung.thieu@nokia-bell-labs.com>
 */

#include "ns3/applications-module.h"
#include "ns3/fd-net-device-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"

#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("UdpServerReadlDevice");

void
UdpServerReceive(Ptr<Socket> socket)
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
            NS_LOG_INFO("receivedSize " << receivedSize << " data " << data);
        }
    }
}

int
main(int argc, char* argv[])
{
    NS_LOG_INFO("UDP server listen on real device example");
    std::string deviceName("enp0s3");
    std::string localAddress("192.168.56.106");
    std::string localGateway("192.168.56.106");
    uint16_t port = 8002; //2152

    std::string emuMode("raw");


    //
    // Allow the user to override any of the defaults at run-time, via
    // command-line arguments
    //
    CommandLine cmd(__FILE__);
    cmd.AddValue("deviceName",
                 "Device name (in raw, netmap mode) or Device address (in dpdk mode, eg: "
                 "0000:00:1f.6). Use `lspci` to find device address.",
                 deviceName);
    cmd.AddValue("localIp", "Local IP address (dotted decimal only please)", localAddress);
    cmd.AddValue("gateway", "Gateway address (dotted decimal only please)", localGateway);
    cmd.AddValue("emuMode", "Emulation mode in {raw, netmap, dpdk}", emuMode);
    cmd.Parse(argc, argv);

    Ipv4Address localIp(localAddress.c_str());
    NS_ABORT_MSG_IF(localIp == "1.2.3.4",
                    "You must change the local IP address before running this example");

    Ipv4Mask localMask("255.255.255.0");

    //
    // Since we are using a real piece of hardware we need to use the realtime
    // simulator.
    //
    GlobalValue::Bind("SimulatorImplementationType", StringValue("ns3::RealtimeSimulatorImpl"));

    //
    // Since we are going to be talking to real-world machines, we need to enable
    // calculation of checksums in our protocols.
    //
    GlobalValue::Bind("ChecksumEnabled", BooleanValue(true));

    //
    // In such a simple topology, the use of the helper API can be a hindrance
    // so we drop down into the low level API and do it manually.
    //
    // First we need a single node.
    //
    NS_LOG_INFO("Create Node");
    Ptr<Node> node = CreateObject<Node>();

    bool logging = true;

    if (logging)
    {
        LogComponentEnable("UdpClient", LOG_LEVEL_INFO);
    }

    NS_LOG_INFO("Create Device");

    FdNetDeviceHelper* helper = nullptr;


        auto raw = new EmuFdNetDeviceHelper;
        raw->SetDeviceName(deviceName);
        helper = raw;

    if (helper == nullptr)
    {
        NS_ABORT_MSG(emuMode << " not supported.");
    }

    NetDeviceContainer devices = helper->Install(node);
    Ptr<NetDevice> device = devices.Get(0);
    device->SetAttribute("Address", Mac48AddressValue(Mac48Address("08:00:27:3a:aa:db")));

    NS_LOG_INFO("Add Internet Stack");
    InternetStackHelper internetStackHelper;
    internetStackHelper.Install(node);

    NS_LOG_INFO("Create IPv4 Interface");
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

    // NS_LOG_INFO("Create UdpServer application on node 1.");
    // uint16_t port = 8002;
    // UdpServerHelper server(port);
    // ApplicationContainer apps = server.Install(node);
    // apps.Start(Seconds(1.0));
    // apps.Stop(Seconds(100.0));

    Ipv4Address serverIp("192.168.56.107");
    NS_LOG_INFO("Create UdpClient application");
    uint32_t MaxPacketSize = 1024;
    Time interPacketInterval = Seconds(1.);
    uint32_t maxPacketCount = 100;
    UdpClientHelper client(serverIp, port);
    client.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
    client.SetAttribute("Interval", TimeValue(interPacketInterval));
    client.SetAttribute("PacketSize", UintegerValue(MaxPacketSize));
    ApplicationContainer apps = client.Install(node);
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(100.0));

    NS_LOG_INFO("Run Emulation in " << emuMode << " mode.");
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Done.");

    return 0;
}
