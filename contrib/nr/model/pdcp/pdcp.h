#ifndef PDCP_HPP
#define PDCP_HPP

#include "ns3/lte-pdcp.h"
#include "ns3/lte-rlc.h"
#include "ns3/lte-rlc-um.h"
#include "ns3/lte-radio-bearer-tag.h"
// include lte mac

#include "ns3/lte-enb-rrc.h"
#include "ns3/lte-mac-sap.h"
#include "ns3/lte-enb-mac.h"

#include "ns3/nr-gnb-mac.h"
#include "ns3/nr-gnb-net-device.h"
#include "ns3/nr-gnb-phy.h"
namespace ns3
{
class Pdcp: public LtePdcp  {

friend class LtePdcp;
public:
    Pdcp();
    ~Pdcp();

    void TransmitPacket(Ptr<Packet> packet);
    void ReceivePacket(Ptr<Packet> packet);

public:
    Ptr<LtePdcp> pdcp;
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

};

void SetupPdcp(Ptr<LtePdcp> pdcp, uint16_t rnti, uint8_t lcid);

}
#endif // PDCP_H

