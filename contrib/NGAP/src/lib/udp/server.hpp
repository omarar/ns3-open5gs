#pragma once

#include "ns3/network.hpp"

#include <string>
#include <unordered_map>

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

    //std::unordered_map<InetAddress, std::vector<uint8_t>> messageStore;


    int Receive(uint8_t *buffer, size_t bufferSize, int timeoutMs, InetAddress &outPeerAddress) const;
    void Send(const InetAddress &address, const uint8_t *buffer, size_t bufferSize) const;
    void sendStore(const InetAddress& address, const uint8_t* buffer, size_t bufferSize) const;
    int receiveStore(uint8_t* buffer,
                     size_t bufferSize,
                     int timeoutMs,
                     InetAddress& outPeerAddress) const;
};

} // namespace udp

