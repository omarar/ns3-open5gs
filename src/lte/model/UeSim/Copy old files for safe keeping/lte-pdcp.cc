/*
 * Copyright (c) 2011-2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#include "lte-pdcp.h"

#include "lte-pdcp-header.h"
#include "lte-pdcp-sap.h"
#include "lte-pdcp-tag.h"

#include "ns3/log.h"
#include "ns3/simulator.h"


//---------------------------added
//#include <sys/mman.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <semaphore.h>
//#include <unistd.h>
//----------------------
namespace ns3
{

NS_LOG_COMPONENT_DEFINE("LtePdcp");

/// LtePdcpSpecificLteRlcSapUser class
class LtePdcpSpecificLteRlcSapUser : public LteRlcSapUser
{
  public:
    /**
     * Constructor
     *
     * \param pdcp PDCP
     */
    LtePdcpSpecificLteRlcSapUser(LtePdcp* pdcp);

    // Interface provided to lower RLC entity (implemented from LteRlcSapUser)
    void ReceivePdcpPdu(Ptr<Packet> p) override;

  private:
    LtePdcpSpecificLteRlcSapUser();
    LtePdcp* m_pdcp; ///< the PDCP
};

LtePdcpSpecificLteRlcSapUser::LtePdcpSpecificLteRlcSapUser(LtePdcp* pdcp)
    : m_pdcp(pdcp)
{
}

LtePdcpSpecificLteRlcSapUser::LtePdcpSpecificLteRlcSapUser()
{
}

void
LtePdcpSpecificLteRlcSapUser::ReceivePdcpPdu(Ptr<Packet> p)
{
    m_pdcp->DoReceivePdu(p);
}

///////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED(LtePdcp);

LtePdcp::LtePdcp()
    : m_pdcpSapUser(nullptr),
      m_rlcSapProvider(nullptr),
      m_rnti(0),
      m_lcid(0),
      m_txSequenceNumber(0),
      m_rxSequenceNumber(0)
{
    NS_LOG_FUNCTION(this);
    m_pdcpSapProvider = new LtePdcpSpecificLtePdcpSapProvider<LtePdcp>(this);
    m_rlcSapUser = new LtePdcpSpecificLteRlcSapUser(this);
}

LtePdcp::~LtePdcp()
{
    NS_LOG_FUNCTION(this);
}

TypeId
LtePdcp::GetTypeId()
{
    static TypeId tid = TypeId("ns3::LtePdcp")
                            .SetParent<Object>()
                            .SetGroupName("Lte")
                            .AddTraceSource("TxPDU",
                                            "PDU transmission notified to the RLC.",
                                            MakeTraceSourceAccessor(&LtePdcp::m_txPdu),
                                            "ns3::LtePdcp::PduTxTracedCallback")
                            .AddTraceSource("RxPDU",
                                            "PDU received.",
                                            MakeTraceSourceAccessor(&LtePdcp::m_rxPdu),
                                            "ns3::LtePdcp::PduRxTracedCallback");
    return tid;
}

void
LtePdcp::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete (m_pdcpSapProvider);
    delete (m_rlcSapUser);
}

void
LtePdcp::SetRnti(uint16_t rnti)
{
    NS_LOG_FUNCTION(this << (uint32_t)rnti);
    m_rnti = rnti;
}

void
LtePdcp::SetLcId(uint8_t lcId)
{
    NS_LOG_FUNCTION(this << (uint32_t)lcId);
    m_lcid = lcId;
}

void
LtePdcp::SetLtePdcpSapUser(LtePdcpSapUser* s)
{
    NS_LOG_FUNCTION(this << s);
    m_pdcpSapUser = s;
}

LtePdcpSapProvider*
LtePdcp::GetLtePdcpSapProvider()
{
    NS_LOG_FUNCTION(this);
    return m_pdcpSapProvider;
}

void
LtePdcp::SetLteRlcSapProvider(LteRlcSapProvider* s)
{
    NS_LOG_FUNCTION(this << s);
    m_rlcSapProvider = s;
}

LteRlcSapUser*
LtePdcp::GetLteRlcSapUser()
{
    NS_LOG_FUNCTION(this);
    return m_rlcSapUser;
}

LtePdcp::Status
LtePdcp::GetStatus() const
{
    Status s;
    s.txSn = m_txSequenceNumber;
    s.rxSn = m_rxSequenceNumber;
    return s;
}

void
LtePdcp::SetStatus(Status s)
{
    m_txSequenceNumber = s.txSn;
    m_rxSequenceNumber = s.rxSn;
}

////////////////////////////////////////
// Example of creating a test packet with data
/* //code for print a test packet for testing purposes
void PrintRrcPduData(const RrcPduData& data) {
    std::cout << "ueId: " << data.ueId << ", size: " << data.size << ", data: ";
    for (uint32_t i = 0; i < data.size; ++i) {
        std::cout << std::hex << static_cast<int>(data.data[i]) << " ";
    }
    std::cout << std::dec << std::endl; // Reset stream to decimal
}*/
/* old code for packet creating 
RrcPduData CreateTestPacket() {
    // Initialize RrcPduData
    RrcPduData pdcpSduData;
    pdcpSduData.ueId = 12345;
   // pdcpSduData.channel = 1; // Assuming 1 is a valid channel value
    std::string testData = "This is a test packet";
    pdcpSduData.size = testData.size();
    std::memcpy(pdcpSduData.data, testData.data(), pdcpSduData.size);

    // Create ns3::Packet from RrcPduData
    Ptr<Packet> packet = Create<Packet>((uint8_t*)&pdcpSduData, sizeof(pdcpSduData));
    return pdcpSduData;
}*/
/*
RrcPduData CreatePacket() {
    // Initialize RrcPduData
    RrcPduData pdcpSduData;
    return pdcpSduData;
}
*/
void
LtePdcp::DoTransmitPdcpSdu(LtePdcpSapProvider::TransmitPdcpSduParameters params)
{
	std::cout<<"Entered DoTransmitPdcpSdu"<<std::endl;
    NS_LOG_FUNCTION(this << m_rnti << static_cast<uint16_t>(m_lcid) << params.pdcpSdu->GetSize());
    Ptr<Packet> p = params.pdcpSdu;
    // Sender timestamp
    PdcpTag pdcpTag(Simulator::Now());

    LtePdcpHeader pdcpHeader;
    pdcpHeader.SetSequenceNumber(m_txSequenceNumber);

    m_txSequenceNumber++;
    if (m_txSequenceNumber > m_maxPdcpSn)
    {
        m_txSequenceNumber = 0;
    }

    pdcpHeader.SetDcBit(LtePdcpHeader::DATA_PDU);

    NS_LOG_LOGIC("PDCP header: " << pdcpHeader);
    p->AddHeader(pdcpHeader);
    p->AddByteTag(pdcpTag, 1, pdcpHeader.GetSerializedSize());

    m_txPdu(m_rnti, m_lcid, p->GetSize());

    LteRlcSapProvider::TransmitPdcpPduParameters txParams;
    txParams.rnti = m_rnti;
    txParams.lcid = m_lcid;
    txParams.pdcpPdu = p;

    m_rlcSapProvider->TransmitPdcpPdu(txParams);
    
  //  RrcPduData UERNPacket = CreatePacket();
  //  UERNPacket.rrcPacket = txParams;
 
  //  SendToUeransim(UERNPacket);   //added 
}

void
LtePdcp::DoReceivePdu(Ptr<Packet> p)
{
    NS_LOG_FUNCTION(this << m_rnti << (uint32_t)m_lcid << p->GetSize());
   // ReceiveFromUeransim(); 
    // Receiver timestamp
    PdcpTag pdcpTag;
    Time delay;
    p->FindFirstMatchingByteTag(pdcpTag);
    delay = Simulator::Now() - pdcpTag.GetSenderTimestamp();
    m_rxPdu(m_rnti, m_lcid, p->GetSize(), delay.GetNanoSeconds());

    LtePdcpHeader pdcpHeader;
    p->RemoveHeader(pdcpHeader);
    NS_LOG_LOGIC("PDCP header: " << pdcpHeader);

    m_rxSequenceNumber = pdcpHeader.GetSequenceNumber() + 1;
    if (m_rxSequenceNumber > m_maxPdcpSn)
    {
        m_rxSequenceNumber = 0;
    }

    LtePdcpSapUser::ReceivePdcpSduParameters params;
    params.pdcpSdu = p;
    params.rnti = m_rnti;
    params.lcid = m_lcid;
    m_pdcpSapUser->ReceivePdcpSdu(params);
}

//---------------------------- added for NR compatibility----------------------------------
/*
void LtePdcp::ReceiveFromUeransim()
{
    // Wait for the semaphore
    sem_t *semaphore_dl1 = sem_open("/my_semaphore_dl", 0); // Open existing semaphore
    if (semaphore_dl1 == SEM_FAILED) {
        NS_FATAL_ERROR("Failed to open semaphore: " << strerror(errno));
        return;
    }
    sem_wait(semaphore_dl1);
   std::cout<<"semaphore openned "<<std::endl;
    // Access shared memory to get RrcPduData
    int shmFd_dl1 = shm_open("/my_shared_memory_dl", O_RDONLY, 0);
    if (shmFd_dl1 == -1) {
        NS_FATAL_ERROR("Failed to open shared memory: " << strerror(errno));
        sem_post(semaphore_dl1); // Release the semaphore in case of error
        return;
    }
    std::cout<<"shmfd openned "<<std::endl;
    RrcPduData *sharedPdu = (RrcPduData*)mmap(0, sizeof(RrcPduData), PROT_READ, MAP_SHARED, shmFd_dl1, 0);
    if (sharedPdu == MAP_FAILED) {
        NS_FATAL_ERROR("Failed to map shared memory: " << strerror(errno));
        sem_post(semaphore_dl1); // Release the semaphore in case of error
        close(shmFd_dl1);
        return;
    }
   std::cout<<"shared pdu created"<<std::endl;

    // Unmap and close shared memory
    munmap(sharedPdu, sizeof(RrcPduData));
    close(shmFd_dl1);
    

    // Signal the semaphore (optional)
    sem_post(semaphore_dl1);
    
}



//--------------------------other direction 
/*
void LtePdcp::SendToUeransim(RrcPduData packet) {
    sem_t *semaphore = sem_open("/my_semaphore", O_CREAT, 0666, 1);
    if (semaphore == SEM_FAILED) {
        NS_FATAL_ERROR("Failed to open semaphore: " << strerror(errno));
        return;
    }

    RrcPduDataSerializable pdcpSduData;
    pdcpSduData.rrcPacket.rnti = packet.rrcPacket.rnti;
    pdcpSduData.rrcPacket.lcid = packet.rrcPacket.lcid;

    // Serialize the packet data
    uint32_t packetSize = packet.rrcPacket.pdcpPdu->GetSize();
    pdcpSduData.rrcPacket.packetSize = packetSize;
    packet.rrcPacket.pdcpPdu->CopyData(pdcpSduData.packetData, packetSize);

    int shmFd = shm_open("/my_shared_memory", O_RDWR | O_CREAT, 0666);
    if (shmFd == -1) {
        NS_FATAL_ERROR("Failed to open shared memory: " << strerror(errno));
        sem_post(semaphore); // Release the semaphore in case of error
        sem_close(semaphore);
        return;
    }

    if (ftruncate(shmFd, sizeof(RrcPduDataSerializable)) == -1) {
        NS_FATAL_ERROR("Failed to set the size of the shared memory: " << strerror(errno));
        sem_post(semaphore); // Release the semaphore in case of error
        close(shmFd);
        sem_close(semaphore);
        return;
    }

    std::cout << "sizeof(RrcPduDataSerializable) = " << sizeof(RrcPduDataSerializable) << std::endl;

    RrcPduDataSerializable *sharedPdu = (RrcPduDataSerializable*)mmap(0, sizeof(RrcPduDataSerializable), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (sharedPdu == MAP_FAILED) {
        NS_FATAL_ERROR("Failed to map shared memory: " << strerror(errno));
        sem_post(semaphore); // Release the semaphore in case of error
        close(shmFd);
        sem_close(semaphore);
        return;
    }

   // std::cout << "mmap success, sharedPdu = " << sharedPdu << std::endl;

    *sharedPdu = pdcpSduData;
  //  std::cout << "assignment success" << std::endl;

    munmap(sharedPdu, sizeof(RrcPduDataSerializable));
    close(shmFd);

    sem_post(semaphore);
    sem_close(semaphore);
  //  std::cout << "Packet sent to UERANSIM" << std::endl;
}
////////////////////////////////////////////


/*
// NrPdcpSpecificNrRlcSapUser class
class NrPdcpSpecificNrRlcSapUser : public LteRlcSapUser {
public:

    NrPdcpSpecificNrRlcSapUser(NrPdcp* pdcp);

    // Interface provided to lower RLC entity (implemented from NrRlcSapUser)
    void ReceivePdcpPdu(Ptr<Packet> p) override;

private:
    NrPdcpSpecificNrRlcSapUser();
    NrPdcp* m_pdcp; ///< the PDCP
};

NrPdcpSpecificNrRlcSapUser::NrPdcpSpecificNrRlcSapUser(NrPdcp* pdcp)
    : m_pdcp(pdcp) {}

NrPdcpSpecificNrRlcSapUser::NrPdcpSpecificNrRlcSapUser() {}

void NrPdcpSpecificNrRlcSapUser::ReceivePdcpPdu(Ptr<Packet> p) {
    m_pdcp->DoReceivePdu(p);
}


NrPdcp::NrPdcp()
    : m_pdcpSapUser(nullptr),
      m_rlcSapProvider(nullptr),
      m_rnti(0),
      m_lcid(0),
      m_txSequenceNumber(0),
      m_rxSequenceNumber(0) {
    NS_LOG_FUNCTION(this);
    m_pdcpSapProvider = new LtePdcpSpecificLtePdcpSapProvider<NrPdcp>(this); //change later on ?
    m_rlcSapUser = new NrPdcpSpecificNrRlcSapUser(this);
}

NrPdcp::~NrPdcp() {
    NS_LOG_FUNCTION(this);
}

TypeId NrPdcp::GetTypeId() {
    static TypeId tid = TypeId("ns3::NrPdcp")
                            .SetParent<Object>()
                            .SetGroupName("Nr")
                            .AddTraceSource("TxPDU",
                                            "PDU transmission notified to the RLC.",
                                            MakeTraceSourceAccessor(&NrPdcp::m_txPdu),
                                            "ns3::NrPdcp::PduTxTracedCallback")
                            .AddTraceSource("RxPDU",
                                            "PDU received.",
                                            MakeTraceSourceAccessor(&NrPdcp::m_rxPdu),
                                            "ns3::NrPdcp::PduRxTracedCallback");
    return tid;
}

void NrPdcp::DoDispose() {
    NS_LOG_FUNCTION(this);
    delete (m_pdcpSapProvider);
    delete (m_rlcSapUser);
}

void NrPdcp::SetRnti(uint16_t rnti) {
    NS_LOG_FUNCTION(this << (uint32_t)rnti);
    m_rnti = rnti;
}

void NrPdcp::SetLcId(uint8_t lcId) {
    NS_LOG_FUNCTION(this << (uint32_t)lcId);
    m_lcid = lcId;
}

void NrPdcp::SetNrPdcpSapUser(LtePdcpSapUser* s) {
    NS_LOG_FUNCTION(this << s);
    m_pdcpSapUser = s;
}

LtePdcpSapProvider* NrPdcp::GetNrPdcpSapProvider() {
    NS_LOG_FUNCTION(this);
    return m_pdcpSapProvider;
}

void NrPdcp::SetNrRlcSapProvider(LteRlcSapProvider* s) {
    NS_LOG_FUNCTION(this << s);
    m_rlcSapProvider = s;
}

LteRlcSapUser* NrPdcp::GetNrRlcSapUser() {
    NS_LOG_FUNCTION(this);
    return m_rlcSapUser;
}

NrPdcp::Status NrPdcp::GetStatus() const {
    Status s;
    s.txSn = m_txSequenceNumber;
    s.rxSn = m_rxSequenceNumber;
    return s;
}

void NrPdcp::SetStatus(Status s) {
    m_txSequenceNumber = s.txSn;
    m_rxSequenceNumber = s.rxSn;
}

////////////////////////////////////////

void NrPdcp::DoTransmitPdcpSdu(LtePdcpSapProvider::TransmitPdcpSduParameters params) {
    NS_LOG_FUNCTION(this << m_rnti << static_cast<uint16_t>(m_lcid) << params.pdcpSdu->GetSize());
    Ptr<Packet> p = params.pdcpSdu;

    // Sender timestamp
    PdcpTag pdcpTag(Simulator::Now());

    LtePdcpHeader pdcpHeader;
    pdcpHeader.SetSequenceNumber(m_txSequenceNumber);

    m_txSequenceNumber++;
    if (m_txSequenceNumber > m_maxPdcpSn) {
        m_txSequenceNumber = 0;
    }

    pdcpHeader.SetDcBit(LtePdcpHeader::DATA_PDU);

    NS_LOG_LOGIC("PDCP header: " << pdcpHeader);
    p->AddHeader(pdcpHeader);
    p->AddByteTag(pdcpTag, 1, pdcpHeader.GetSerializedSize());

    m_txPdu(m_rnti, m_lcid, p->GetSize());

    LteRlcSapProvider::TransmitPdcpPduParameters txParams;
    txParams.rnti = m_rnti;
    txParams.lcid = m_lcid;
    txParams.pdcpPdu = p;

    m_rlcSapProvider->TransmitPdcpPdu(txParams);
}

void NrPdcp::DoReceivePdu(Ptr<Packet> p) {
    NS_LOG_FUNCTION(this << m_rnti << (uint32_t)m_lcid << p->GetSize());

    // Receiver timestamp
    PdcpTag pdcpTag;
    Time delay;
    p->FindFirstMatchingByteTag(pdcpTag);
    delay = Simulator::Now() - pdcpTag.GetSenderTimestamp();
    m_rxPdu(m_rnti, m_lcid, p->GetSize(), delay.GetNanoSeconds());

    LtePdcpHeader pdcpHeader;
    p->RemoveHeader(pdcpHeader);
    NS_LOG_LOGIC("PDCP header: " << pdcpHeader);

    m_rxSequenceNumber = pdcpHeader.GetSequenceNumber() + 1;
    if (m_rxSequenceNumber > m_maxPdcpSn) {
        m_rxSequenceNumber = 0;
    }

    LtePdcpSapUser::ReceivePdcpSduParameters params;
    params.pdcpSdu = p;
    params.rnti = m_rnti;
    params.lcid = m_lcid;
    m_pdcpSapUser->ReceivePdcpSdu(params);
}


*/


} // namespace ns3
