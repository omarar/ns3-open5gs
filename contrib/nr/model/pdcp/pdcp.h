#ifndef PDCP_HPP
#define PDCP_HPP

#include "ns3/lte-pdcp.h"
#include "ns3/lte-rlc.h"
#include "ns3/lte-rlc-um.h"
#include "ns3/lte-radio-bearer-tag.h"

#include "../../../NGAP/src/lib/rls/rls_pdu.hpp"

#include "ns3/lte-enb-mac.h"
#include "ns3/lte-enb-rrc.h"
#include "ns3/lte-mac-sap.h"
#include "ns3/nr-gnb-mac.h"
#include "ns3/nr-gnb-net-device.h"
#include "ns3/nr-gnb-phy.h"

#include <unordered_map>

namespace ns3
{
class Pdcp: public LtePdcp  {

  private:

    static std::unordered_map<int, Ptr<LtePdcp>> m_UeId_PdcpMap;
    static std::unordered_map<std::string, std::vector<uint8_t>> messageMap;
    size_t streamLength = 0;

    //uint8_t* streamData = nullptr;
    std::vector<uint8_t> streamData;
    InetAddress storedAddress;


    // Pdcp();

  public:
    Ptr<LtePdcp> pdcp;
    static Pdcp& GetInstance();

    void SetPdcp(int ueId, ns3::Ptr<ns3::LtePdcp> pdcp);
    ns3::Ptr<ns3::LtePdcp> GetPdcp(int ueId);
    bool HasPdcp(int ueId);
    static size_t GetMapSize();
    //
    static size_t GetmessageMapSize();
    void SetMessageMap(std::string key, std::vector<uint8_t> message);
    std::vector<uint8_t> GetMessageMap(std::string key);

    void SetStreamLength(size_t length);
    size_t GetStreamLength() const;


    //void SetStreamData(uint8_t* data);
    //uint8_t* const GetStreamData() const;

    void SetStreamData(const uint8_t* data, size_t length);
    const uint8_t* GetStreamData() const ;

    void SetAddress(const InetAddress &addr);
    InetAddress GetAddress() const ;

    //

    friend class LtePdcp;


  public:
    Pdcp();
    ~Pdcp();

    void TransmitPdcpSduWrapper(Ptr<ns3::LtePdcp> pdcp, LtePdcpSapProvider::TransmitPdcpSduParameters params);
    void TransmitPacket(Ptr<Packet> packet);
    void ReceivePacket(Ptr<Packet> packet);

public:

    // Store PDCP instances and other attributes like IMSI
    //std::unordered_map<int, ns3::Ptr<ns3::LtePdcp>> m_UeId_PdcpMap;
    //std::unordered_map<int, int> m_imsiMap;  // Map for storing IMSI or other attributes

    //Ptr<LtePdcp> pdcp;
    //ns3::Ptr<ns3::LteRlcAm> rlc;
    Ptr<LteRlcUm> rlc;

    //ns3::Ptr<ns3::LteEnbRrc> m_rrc;
    //ns3::Ptr<ns3::UeManager> ueManager;
    LteMacSapProvider* m_macSapProvider;

    //ns3::EnbMacMemberLteMacSapProvider<ns3::LteEnbMac> m_mac;
    EnbMacMemberLteMacSapProvider<LteEnbMac>* m_mac;

    Ptr<NrGnbMac> nrMac;
    Ptr<NrGnbNetDevice> nrNetDevice;
    Ptr<NrGnbPhy> nrPhy;

    //ns3::NrGnbMac nrMac;
    //ns3::Ptr<ns3::NrGnbNetDevice> nrNetDevice;
    //ns3::Ptr<ns3::NrGnbPhy> nrPhy;



    //ns3::EnbMacMemberLteMacSapProvider<ns3::LteEnbMac>* m_mac;

    //ns3::Ptr<ns3::LteMacSapProvider> macSapProvider;




    //void SetupPdcp(Ptr<LtePdcp> pdcp, uint16_t rnti, uint8_t lcid);
};



}
#endif // PDCP_H

