//
// This file is a part of UERANSIM project.
// Copyright (c) 2023 ALİ GÜNGÖR.
//
// https://github.com/aligungr/UERANSIM/
// See README, LICENSE, and CONTRIBUTING files for licensing details.
//

#pragma once

#include "udp_task.hpp"
#include <netinet/in.h> // for sockaddr_in
#include <sys/socket.h> // for socket functions and definitions
#include <unordered_map>
#include <vector>

#include "../../../ns-3-dev/contrib/NGAP/src/lib/rrc/rrc.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/ue/nts.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/ue/types.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/utils/nts.hpp" 
#include "ns3/packet.h"
#include <ns3/pdcp.h>  // Include the PDCP header

//---------------------------------- added code
/*
#include <cstring>
#include <algorithm>
#include <semaphore.h>
#include <cstdint>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>

#include <ns3/rrcstruct.h>
* */
//-------------------------------------

namespace nr::ue
{

class RlsControlTask : public NtsTask
{
  private:
    std::unique_ptr<Logger> m_logger;
    RlsSharedContext *m_shCtx;
    int m_servingCell;
    NtsTask *m_mainTask;
    RlsUdpTask *m_udpTask;
    std::unordered_map<uint32_t, rls::PduInfo> m_pduMap;
    std::unordered_map<int, std::vector<uint32_t>> m_pendingAck;
    std::unordered_map<uint32_t, ns3::Pdcp> m_pdcpMap;  // Map to hold PDCP instances for UEs
    std::map<int, bool> m_ueAttachmentState;       // Add a member to track attachment state

  public:
    explicit RlsControlTask(TaskBase *base, RlsSharedContext *shCtx);
    ~RlsControlTask() override = default;

  protected:
    void onStart() override;
    void onLoop() override;
    void onQuit() override;

  public:
    void initialize(NtsTask *mainTask, RlsUdpTask *udpTask);

  private:
    void handleRlsMessage(int cellId, rls::RlsMessage &msg);
    void handleSignalChange(int cellId, int dbm);
    void handleUplinkRrcDelivery(int cellId, uint32_t pduId, rrc::RrcChannel channel, OctetString &&data);
    void handleUplinkDataDelivery(int psi, OctetString &&data);
    void onAckControlTimerExpired();
    void onAckSendTimerExpired();
};

} // namespace nr::ue
