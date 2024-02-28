//
// This file is a part of UERANSIM project.
// Copyright (c) 2023 ALİ GÜNGÖR.
//
// https://github.com/aligungr/UERANSIM/
// See README, LICENSE, and CONTRIBUTING files for licensing details.
//

#pragma once

#include <ns3/server.hpp>
#include <ns3/nts.hpp>
#include <ns3/octet_string.hpp>

namespace udp
{

struct NwUdpServerReceive : NtsMessage
{
    OctetString packet;
    InetAddress fromAddress;

    explicit NwUdpServerReceive(OctetString &&packet, const InetAddress &fromAddress)
        : NtsMessage(NtsMessageType::UDP_SERVER_RECEIVE), packet(std::move(packet)), fromAddress(fromAddress)
    {
    }
};

class UdpServerTask : public NtsTask
{
  private:
    UdpServer *server;
    NtsTask *targetTask;

  public:
    explicit UdpServerTask(NtsTask *targetTask);
    UdpServerTask(const std::string &address, uint16_t port, NtsTask *targetTask);
    ~UdpServerTask() override;

  protected:
    void onStart() override;
    void onLoop() override;
    void onQuit() override;

  public:
    void send(const InetAddress &to, const OctetString &packet);
};

} // namespace udp
