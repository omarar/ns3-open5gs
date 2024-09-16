
#include "task.hpp"

#include "../../../ns-3-dev/contrib/NGAP/src/gnb/ngap/task.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/lib/rrc/encode.hpp" 

namespace nr::gnb
{

void GnbRrcTask::handleRlsSapMessage(NmGnbRlsToRrc &msg)
{
    switch (msg.present)
    {
    case NmGnbRlsToRrc::SIGNAL_DETECTED: {
        m_logger->debug("UE[%d] new signal detected", msg.ueId);
        triggerSysInfoBroadcast();
        break;
    }
    case NmGnbRlsToRrc::UPLINK_RRC: {
        handleUplinkRrc(msg.ueId, msg.rrcChannel, msg.data);
        break;
    }
    }
}

} // namespace nr::gnb

