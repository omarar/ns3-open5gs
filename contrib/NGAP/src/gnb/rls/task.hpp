// code omar 
#pragma once

#include "ctl_task.hpp"
#include "udp_task.hpp"

#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../../../ns-3-dev/contrib/NGAP/src/gnb/nts.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/gnb/types.hpp" 
#include <ns3/rls_pdu.hpp>
#include <ns3/server_task.hpp>
#include <ns3/logger.hpp>
#include "../../../ns-3-dev/contrib/NGAP/src/utils/nts.hpp" 

//#include <../../../ns-3-dev/contrib/NGAP/src/lib/pdcp/pdcp.hpp>  // Include the PDCP header


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

    //std::unordered_map<uint32_t, Pdcp> m_pdcpMap;  // Map to hold PDCP instances for UEs


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



/*
#pragma once

#include "ctl_task.hpp"
#include "udp_task.hpp"

#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../../../ns-3-dev/contrib/NGAP/src/gnb/nts.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/gnb/types.hpp" 
#include <ns3/rls_pdu.hpp>
#include <ns3/server_task.hpp>
#include <ns3/logger.hpp>
#include "../../../ns-3-dev/contrib/NGAP/src/utils/nts.hpp" 

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

*/
