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

#include <ns3/rls_pdu.hpp>
#include "../../../ns-3-dev/contrib/NGAP/src/lib/udp/server.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/ue/types.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/utils/nts.hpp"
//#include "../../../ns-3-dev/contrib/nr/model/pdcp/pdcp.h"

namespace nr::ue
{

class RlsUdpTask : public NtsTask
{
  private:
    struct CellInfo
    {
        InetAddress address;
        int64_t lastSeen{};
        int dbm{};
        int cellId{};
    };

  private:
    std::unique_ptr<Logger> m_logger;
    udp::UdpServer *m_server;
    NtsTask *m_ctlTask;
    RlsSharedContext* m_shCtx;
    std::vector<InetAddress> m_searchSpace;
    std::unordered_map<uint64_t, CellInfo> m_cells;
    std::unordered_map<int, uint64_t> m_cellIdToSti;
    int64_t m_lastLoop;
    Vector3 m_simPos;
    int m_cellIdCounter;

    std::queue<std::vector<uint8_t>> messageQueue;
    std::mutex queueMutex;


    //std::unordered_map<std::string, std::vector<uint8_t>> messageMap;


    friend class UeCmdHandler;

  public:
    explicit RlsUdpTask(TaskBase *base, RlsSharedContext* shCtx, const std::vector<std::string> &searchSpace);
    ~RlsUdpTask() override = default;

  protected:
    void onStart() override;
    void onLoop() override;
    void onQuit() override;

  private:
    void sendRlsPdu(const InetAddress &addr, const rls::RlsMessage &msg);
    void receiveRlsPdu(const InetAddress &addr, std::unique_ptr<rls::RlsMessage> &&msg);
    void onSignalChangeOrLost(int cellId);
    void heartbeatCycle(uint64_t time, const Vector3 &simPos);

  public:
    void initialize(NtsTask *ctlTask);
    void send(int cellId, const rls::RlsMessage &msg);
};

} // namespace nr::ue
