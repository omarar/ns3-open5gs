
#include "server.hpp"

#include <ns3/common.hpp>

#include <cstring>
#include <iostream>

namespace udp
{


//// Define a map to store messages by address
//mutable std::unordered_map<InetAddress, std::vector<uint8_t>> messageStore;

UdpServer::UdpServer() : sockets{Socket::CreateUdp4(), Socket::CreateUdp6()}
{
}

UdpServer::UdpServer(const std::string &address, uint16_t port) : sockets{Socket::CreateAndBindUdp({address, port})}
{
}

int UdpServer::Receive(uint8_t *buffer, size_t bufferSize, int timeoutMs, InetAddress &outPeerAddress) const
{
    auto socket = Socket::Select(sockets, {}, timeoutMs);
    if (!socket.hasFd())
        return 0;
    return socket.receive(buffer, bufferSize, timeoutMs, outPeerAddress);
}

void UdpServer::Send(const InetAddress &address, const uint8_t *buffer, size_t bufferSize) const
{

    int version = address.getIpVersion();
    if (version != 4 && version != 6)
        throw std::runtime_error{"UdpServer::Send failure: Invalid IP version"};

    for (const Socket &s : sockets)
    {
        if (s.hasFd() && s.getIpVersion() == version)
        {
            s.send(address, buffer, bufferSize);
            return;
        }
    }

    throw std::runtime_error{"UdpServer::Send failure: No IP socket found"};


}

//

//

UdpServer::~UdpServer()
{
    for (auto &s : sockets)
        s.close();
}

} // namespace udp


