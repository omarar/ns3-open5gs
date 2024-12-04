//
// Version from Omar
//

#pragma once

#include "udp_task.hpp"


#include "../../../ns-3-dev/contrib/NGAP/src/gnb/nts.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/gnb/types.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/utils/nts.hpp" 
#include <ns3/pdcp.h>  // Include the PDCP header
#include <unordered_map>
#include <../../../ns-3-dev/contrib/NGAP/src/gnb/rrc/task.hpp>



namespace nr::gnb
{


class RlsControlTask : public NtsTask
{
  private:
    std::unique_ptr<Logger> m_logger;
    uint64_t m_sti;
    NtsTask *m_mainTask;
    RlsUdpTask *m_udpTask;
    std::unordered_map<uint32_t, rls::PduInfo> m_pduMap;
    std::unordered_map<int, std::vector<uint32_t>> m_pendingAck;
    //std::unordered_map<uint32_t, ns3::Ptr<ns3::LtePdcp>> m_pdcpMap;  // Map to hold PDCP instances for UEs
    //std::map<int, bool> m_ueAttachmentState;       // Add a member to track attachment state

    ns3::Ptr<ns3::LtePdcp> m_pdcp;
    //ns3::LtePdcp m_pdcp;
    //ns3::Ptr<ns3::Pdcp> m_pdcpManager;


  public:
    explicit RlsControlTask(TaskBase *base, uint64_t sti);
    ~RlsControlTask() override = default;

  protected:
    void onStart() override;
    void onLoop() override;
    void onQuit() override;

  public:
    void initialize(NtsTask *mainTask, RlsUdpTask *udpTask);
  //  void SetPdcp(ns3::Ptr<ns3::LtePdcp> pdcp);

   // ns3::Ptr<ns3::LtePdcp> GetPdcp();

  private:
    void handleSignalDetected(int ueId);
    void handleSignalLost(int ueId);
    void handleRlsMessage(int ueId, rls::RlsMessage &msg);
    void handleDownlinkRrcDelivery(int ueId, uint32_t pduId, rrc::RrcChannel channel, OctetString &&data);
    void handleDownlinkDataDelivery(int ueId, int psi, OctetString &&data);
    void onAckControlTimerExpired();
    void onAckSendTimerExpired();
};

} // namespace nr::gnb







































/*
#pragma once

#include "udp_task.hpp"

#include "../../../ns-3-dev/contrib/NGAP/src/gnb/nts.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/gnb/types.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/utils/nts.hpp" 

//---------------------------------- added code
#include <cstring>
#include <algorithm>
#include <semaphore.h>
#include <cstdint>
#include <semaphore.h> 
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>

//-------------------------------------
namespace nr::gnb
{
	*/
// Define RrcPduData struct
/*
struct TransmitPdcpPduParametersSerializable {
    uint32_t packetSize;
    uint16_t rnti;
    uint8_t lcid;
};

struct RrcPduData {
      LteRlcSapProvider::TransmitPdcpPduParameters rrcPacket;
};

struct RrcPduDataSerializable {
    TransmitPdcpPduParametersSerializable rrcPacket;
    uint8_t packetData[2048]; // Adjust size as needed
};
*/
/*
class RlsControlTask : public NtsTask
{
  private:
    std::unique_ptr<Logger> m_logger;
    uint64_t m_sti;
    NtsTask *m_mainTask;
    RlsUdpTask *m_udpTask;
    std::unordered_map<uint32_t, rls::PduInfo> m_pduMap;
    std::unordered_map<int, std::vector<uint32_t>> m_pendingAck;
    // added 
    // int sharedMemoryFd = -1;              // Shared memory file descriptor
   //  RrcPduData* sharedPdu = nullptr;  // Pointer to shared memory region
   // sem_t* semaphore = nullptr;           // Semaphore for synchronization
  public:
    explicit RlsControlTask(TaskBase *base, uint64_t sti);
    ~RlsControlTask() override = default;

  protected:
    void onStart() override;
    void onLoop() override;
    void onQuit() override;

  public:
    void initialize(NtsTask *mainTask, RlsUdpTask *udpTask);

  private:
    void handleSignalDetected(int ueId);
    void handleSignalLost(int ueId);
    void handleRlsMessage(int ueId, rls::RlsMessage &msg);
    void handleDownlinkRrcDelivery(int ueId, uint32_t pduId, rrc::RrcChannel channel, OctetString &&data);
    void handleDownlinkDataDelivery(int ueId, int psi, OctetString &&data);
    void onAckControlTimerExpired();
    void onAckSendTimerExpired();
    //---------------------------------- added code
 //   void sendToNs3Pdcp(RrcPduData packet);

};
} // namespace nr::gnb
*/
