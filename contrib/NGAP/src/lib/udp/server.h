//
// 
//



#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/internet-module.h>
#include <ns3/socket.h>

#include <stdexcept>

#include "ns3/applications-module.h"
#include "ns3/fd-net-device-module.h"
#include "ns3/udp-socket-factory.h"
//#include <ns3/network.hpp>

namespace udp
{
using namespace ns3;
class UdpServer
  {
    public:
        UdpServer();
        UdpServer(const std::string &address, uint16_t port);
        int Receive(uint8_t *buffer, size_t bufferSize, int timeoutMs, Address &outPeerAddress) const;
        void Send(const Address &address, const uint8_t *buffer, size_t bufferSize) const;
        ~UdpServer();

    private:
        Ptr<Socket> ipv4Socket;
        Ptr<Socket> ipv6Socket;
        bool bound;
    };

} // namespace udp
