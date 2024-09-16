//
// This file is a part of UERANSIM project.
// Copyright (c) 2023 ALİ GÜNGÖR.
//
// https://github.com/aligungr/UERANSIM/
// See README, LICENSE, and CONTRIBUTING files for licensing details.
//

#pragma once

#include <ns3/usim.hpp>
#include <ns3/milenage.h>
#include <ns3/nas.hpp>
#include <ns3/mm.hpp>
#include <ns3/sm.hpp>
#include "../../../ns-3-dev/contrib/NGAP/src/ue/nts.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/ue/types.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/utils/nts.hpp" 

namespace nr::ue
{

class NasTask : public NtsTask
{
  private:
    TaskBase *base;
    std::unique_ptr<Logger> logger;

    NasTimers timers;
    NasMm *mm;
    NasSm *sm;
    Usim *usim;

    friend class UeCmdHandler;

  public:
    explicit NasTask(TaskBase *base);
    ~NasTask() override = default;

  protected:
    void onStart() override;
    void onLoop() override;
    void onQuit() override;

  private:
    void performTick();
};

} // namespace nr::ue