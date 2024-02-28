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
#include <thread>
#include <unordered_map>
#include <vector>

#include "../../../ns-3-dev/contrib/nr/helper/annex/gnb/nts.hpp"
#include <ns3/types.hpp>
#include <ns3/rls_pdu.hpp>
#include <ns3/server_task.hpp>
#include <ns3/logger.hpp>
#include <ns3/nts.hpp>

namespace nr::gnb
{

class GnbRlsTask : public NtsTask
{
  private:
    TaskBase *m_base;
    std::unique_ptr<Logger> m_logger;

    RlsUdpTask *m_udpTask;
    RlsControlTask *m_ctlTask;

    uint64_t m_sti;

    friend class GnbCmdHandler;

  public:
    explicit GnbRlsTask(TaskBase *base);
    ~GnbRlsTask() override = default;

  protected:
    void onStart() override;
    void onLoop() override;
    void onQuit() override;
};

} // namespace nr::gnb
