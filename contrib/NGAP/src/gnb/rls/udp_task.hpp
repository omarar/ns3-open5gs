//
// This file is a part of UERANSIM project.
// Copyright (c) 2023 ALİ GÜNGÖR.
//
// https://github.com/aligungr/UERANSIM/
// See README, LICENSE, and CONTRIBUTING files for licensing details.
//

#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "../../../ns-3-dev/contrib/NGAP/src/gnb/types.hpp" 
#include <ns3/rls_pdu.hpp>
#include "../../../ns-3-dev/contrib/NGAP/src/lib/udp/server.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/utils/nts.hpp"


namespace nr::gnb
{

class RlsUdpTask : public NtsTask
{
  private:
    struct UeInfo
    {
        uint64_t sti{};
        InetAddress address;
        int64_t lastSeen{};
    };

  private:
    std::unique_ptr<Logger> m_logger;
    udp::UdpServer *m_server;
    NtsTask *m_ctlTask;
    uint64_t m_sti;
    Vector3 m_phyLocation;
    int64_t m_lastLoop;
    std::unordered_map<uint64_t, int> m_stiToUe;
    std::unordered_map<int, UeInfo> m_ueMap;
    int m_newIdCounter;

    std::unordered_map<int, rls::RlsMessage> m_msgMap; // Define the static map

    //std::unordered_map<std::string, std::vector<uint8_t>> messageMap;

    std::queue<std::vector<uint8_t>> messageQueue;
    std::mutex queueMutex;



  public:
    explicit RlsUdpTask(TaskBase *base, uint64_t sti, Vector3 phyLocation);
    ~RlsUdpTask() override = default;

  protected:
    void onStart() override;
    void onLoop() override;
    void onQuit() override;

  private:
    void receiveRlsPdu(const InetAddress &addr, std::unique_ptr<rls::RlsMessage> &&msg);
    void sendRlsPdu(const InetAddress &addr, const rls::RlsMessage &msg);

    void heartbeatCycle(int64_t time);

    //SET/GET OMAR
    //static std::unordered_map<int, rls::RlsMessage> m_msgMap;

  public:
    void initialize(NtsTask *ctlTask);
    void send(int ueId, const rls::RlsMessage &msg);

    //SET/GET OMAR
    void setMessageForUe(int ueId, const rls::RlsMessage &msg);
    rls::RlsMessage getMessageForUe(int ueId) const ;

};

} // namespace nr::gnb
