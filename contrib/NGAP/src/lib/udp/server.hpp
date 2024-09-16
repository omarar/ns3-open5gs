#pragma once

#include <string>
#include "ns3/network.hpp"

namespace udp
{

class UdpServer
{
  private:
    std::vector<::Socket> sockets;

  public:
    UdpServer();
    UdpServer(const std::string &address, uint16_t port);
    ~UdpServer();

    int Receive(uint8_t *buffer, size_t bufferSize, int timeoutMs, InetAddress &outPeerAddress) const;
    void Send(const InetAddress &address, const uint8_t *buffer, size_t bufferSize) const;
};

} // namespace udp

