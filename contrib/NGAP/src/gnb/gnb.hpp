//
// This file is a part of UERANSIM project.
// Copyright (c) 2023 ALİ GÜNGÖR.
//
// https://github.com/aligungr/UERANSIM/
// See README, LICENSE, and CONTRIBUTING files for licensing details.
//

#pragma once

#include "types.hpp"

#include <ns3/cli_cmd.hpp>
#include <ns3/monitor.hpp>
#include <ns3/logger.hpp>
#include <ns3/network.hpp>
#include "../../../ns-3-dev/contrib/NGAP/src/utils/nts.hpp" 

namespace nr::gnb
{

class GNodeB
{
  private:
    TaskBase *taskBase;

  public:
    GNodeB(GnbConfig *config, app::INodeListener *nodeListener, NtsTask *cliCallbackTask);
    virtual ~GNodeB();

  public:
    void start();
    void pushCommand(std::unique_ptr<app::GnbCliCommand> cmd, const InetAddress &address);
};

} // namespace nr::gnb