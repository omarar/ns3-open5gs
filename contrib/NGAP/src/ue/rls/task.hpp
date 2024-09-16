//
// This file is a part of UERANSIM project.
// Copyright (c) 2023 ALİ GÜNGÖR.
//
// https://github.com/aligungr/UERANSIM/
// See README, LICENSE, and CONTRIBUTING files for licensing details.
//

#pragma once

#include "ctl_task.hpp"
#include "udp_task.hpp"

#include <memory>
#include <optional>
#include <thread>
#include <unordered_map>
#include <vector>

#include <ns3/rls_pdu.hpp>
#include "../../../ns-3-dev/contrib/NGAP/src/lib/rrc/rrc.hpp" 
#include <ns3/server_task.hpp>
#include "../../../ns-3-dev/contrib/NGAP/src/ue/types.hpp" 
#include <ns3/common_types.hpp>
#include <ns3/logger.hpp>
#include "../../../ns-3-dev/contrib/NGAP/src/utils/nts.hpp" 

namespace nr::ue
{

class UeRlsTask : public NtsTask
{
  private:
    TaskBase *m_base;
    std::unique_ptr<Logger> m_logger;

    RlsSharedContext* m_shCtx;
    RlsUdpTask *m_udpTask;
    RlsControlTask *m_ctlTask;

    friend class UeCmdHandler;

  public:
    explicit UeRlsTask(TaskBase *base);
    ~UeRlsTask() override = default;

  protected:
    void onStart() override;
    void onLoop() override;
    void onQuit() override;
};

} // namespace nr::ue