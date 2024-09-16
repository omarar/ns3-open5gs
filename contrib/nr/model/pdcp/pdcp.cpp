#include "pdcp.h"
#include "ns3/simulator.h"
#include "ns3/lte-radio-bearer-tag.h"

namespace ns3
{
void SetupPdcp(ns3::Ptr<ns3::LtePdcp> pdcp, uint16_t rnti, uint8_t lcid) {
    pdcp->SetRnti(rnti);
    pdcp->SetLcId(lcid);
    // Log the values
    std::cout << "PDCP Setup: RNTI=" << rnti << ", LCID=" << static_cast<int>(lcid) << std::endl;
}


Pdcp::Pdcp() {
    // Create instances of the MAC and PHY layers for NR gNB
    nrMac = ns3::CreateObject<ns3::NrGnbMac>();
    //nrNetDevice = ns3::CreateObject<ns3::NrGnbNetDevice>();
    nrPhy = ns3::CreateObject<ns3::NrGnbPhy>();

    //m_mac = new ns3::EnbMacMemberLteMacSapProvider<ns3::LteEnbMac>(reinterpret_cast<ns3::LteEnbMac*>(this));

    //ns3::Ptr<ns3::LteEnbMac> enbMac = ns3::CreateObject<ns3::LteEnbMac>();

    ns3::LteEnbMac* enbMac = new ns3::LteEnbMac();

    // Create an instance of EnbMacMemberLteMacSapProvider with the LteEnbMac pointer
    m_mac = new ns3::EnbMacMemberLteMacSapProvider<ns3::LteEnbMac>(enbMac);

    // Assign the created MAC provider instance to m_macSapProvider
    m_macSapProvider = m_mac;
    // Create EnbMacMemberLteMacSapProvider instance using CreateObject
    //m_macSapProvider = new ns3::EnbMacMemberLteMacSapProvider<ns3::LteEnbMac>(this);


    //m_mac = ns3::CreateObject<ns3::EnbMacMemberLteMacSapProvider<ns3::LteEnbMac>>(enbMac);

    // Assign the created MAC provider instance to m_macSapProvider
    //m_macSapProvider = m_mac;


    //m_mac = new ns3::EnbMacMemberLteMacSapProvider<ns3::LteEnbMac>(enbMac);

    //m_macSapProvider = m_mac; // Assign the created MAC provider instance to m_macSapProvider

    pdcp = ns3::CreateObject<ns3::LtePdcp>();
    //rlc = ns3::CreateObject<ns3::LteRlcAm>();
    rlc = ns3::CreateObject<ns3::LteRlcUm>();  // Use LteRlcUm instead of LteRlcAm
    //Ptr<LteEnbRrc> m_rrc;

    //m_rrc = ns3::CreateObject<ns3::LteEnbRrc>();

    //uint16_t rnti = 1; // Example RNTI value
    //uint8_t lcid = 4;  // Example LCID value
    //SetupPdcp(pdcp, rnti, lcid);

    //rlc->SetLteMacSapProvider (m_rrc->m_macSapProvider);

    pdcp->SetLteRlcSapProvider(rlc->GetLteRlcSapProvider());
    rlc->SetLteRlcSapUser(pdcp->GetLteRlcSapUser());

    //m_macSapProvider = reinterpret_cast<ns3::LteMacSapProvider*>(rlc->GetLteMacSapProvider());
    //std::cout << "_________________PDCP.cpp m_macSapProvider = " <<m_macSapProvider << std::endl;
    //rlc->SetLteMacSapProvider (m_rrc->m_macSapProvider);

    // Set the MAC SAP Provider in RLC
    rlc->SetLteMacSapProvider(m_macSapProvider);

    // Log the MAC SAP Provider
    std::cout << "MAC SAP Provider set in RLC: " << m_macSapProvider << std::endl;

}

Pdcp::~Pdcp() {
    pdcp = nullptr;
    rlc = nullptr;
}



void Pdcp::TransmitPacket(ns3::Ptr<ns3::Packet> packet) {
    ns3::LtePdcpSapProvider::TransmitPdcpSduParameters params;

    params.pdcpSdu = packet;
    pdcp->GetLtePdcpSapProvider()->TransmitPdcpSdu(params);
}

void Pdcp::ReceivePacket(ns3::Ptr<ns3::Packet> packet) {
    pdcp->DoReceivePdu(packet);
}
}

