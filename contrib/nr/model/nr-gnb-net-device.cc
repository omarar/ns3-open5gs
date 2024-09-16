/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-gnb-net-device.h"

#include "bandwidth-part-gnb.h"
#include "bwp-manager-gnb.h"
#include "nr-gnb-mac.h"
#include "nr-gnb-phy.h"

#include <ns3/abort.h>
#include <ns3/ipv4-l3-protocol.h>
#include <ns3/ipv6-l3-protocol.h>
#include <ns3/log.h>
#include <ns3/lte-enb-component-carrier-manager.h>
#include <ns3/lte-enb-rrc.h>
#include <ns3/object-map.h>


#include <algorithm>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

#include <unistd.h>
#include <string>
#include <ns3/gnb.hpp>
#include <ns3/base_app.hpp>
#include <ns3/cli_base.hpp>
#include <ns3/cli_cmd.hpp>
#include <ns3/proc_table.hpp>
#include <ns3/constants.hpp>
#include <ns3/io.hpp>
#include <ns3/options.hpp>
#include <ns3/yaml_utils.hpp>
#include <ns3/yaml.h>


static app::CliServer *g_cliServer = nullptr;
static nr::gnb::GnbConfig *g_refConfig = nullptr;
static std::unordered_map<std::string, nr::gnb::GNodeB *> g_gnbMap{};
static app::CliResponseTask *g_cliRespTask = nullptr;
static int j = 0;

static struct Options
{
    std::string configFile{};
    bool disableCmd{};
} g_options{};

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrGnbNetDevice");

NS_OBJECT_ENSURE_REGISTERED(NrGnbNetDevice);

TypeId
NrGnbNetDevice::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrGnbNetDevice")
            .SetParent<NrNetDevice>()
            .AddConstructor<NrGnbNetDevice>()
            .AddAttribute("LteEnbComponentCarrierManager",
                          "The component carrier manager associated to this EnbNetDevice",
                          PointerValue(),
                          MakePointerAccessor(&NrGnbNetDevice::m_componentCarrierManager),
                          MakePointerChecker<LteEnbComponentCarrierManager>())
            .AddAttribute("BandwidthPartMap",
                          "List of Bandwidth Part container.",
                          ObjectMapValue(),
                          MakeObjectMapAccessor(&NrGnbNetDevice::m_ccMap),
                          MakeObjectMapChecker<BandwidthPartGnb>())
            .AddAttribute("LteEnbRrc",
                          "The RRC layer associated with the ENB",
                          PointerValue(),
                          MakePointerAccessor(&NrGnbNetDevice::m_rrc),
                          MakePointerChecker<LteEnbRrc>());
    return tid;
}

NrGnbNetDevice::NrGnbNetDevice()
    : m_cellId(0)
{
    NS_LOG_FUNCTION(this);
}

NrGnbNetDevice::~NrGnbNetDevice()
{
    NS_LOG_FUNCTION(this);
}

Ptr<NrMacScheduler>
NrGnbNetDevice::GetScheduler(uint8_t index) const
{
    NS_LOG_FUNCTION(this);
    return m_ccMap.at(index)->GetScheduler();
}

void
NrGnbNetDevice::SetCcMap(const std::map<uint8_t, Ptr<BandwidthPartGnb>>& ccm)
{
    NS_ABORT_IF(m_ccMap.size() > 0);
    m_ccMap = ccm;
}

uint32_t
NrGnbNetDevice::GetCcMapSize() const
{
    return static_cast<uint32_t>(m_ccMap.size());
}

void
NrGnbNetDevice::RouteIngoingCtrlMsgs(const std::list<Ptr<NrControlMessage>>& msgList,
                                     uint8_t sourceBwpId)
{
    NS_LOG_FUNCTION(this);

    for (const auto& msg : msgList)
    {
        uint8_t bwpId = DynamicCast<BwpManagerGnb>(m_componentCarrierManager)
                            ->RouteIngoingCtrlMsgs(msg, sourceBwpId);
        m_ccMap.at(bwpId)->GetPhy()->PhyCtrlMessagesReceived(msg);
    }
}

void
NrGnbNetDevice::RouteOutgoingCtrlMsgs(const std::list<Ptr<NrControlMessage>>& msgList,
                                      uint8_t sourceBwpId)
{
    NS_LOG_FUNCTION(this);

    for (const auto& msg : msgList)
    {
        uint8_t bwpId = DynamicCast<BwpManagerGnb>(m_componentCarrierManager)
                            ->RouteOutgoingCtrlMsg(msg, sourceBwpId);
        NS_ASSERT_MSG(m_ccMap.size() > bwpId,
                      "Returned bwp " << +bwpId << " is not present. Check your configuration");
        NS_ASSERT_MSG(
            m_ccMap.at(bwpId)->GetPhy()->HasDlSlot(),
            "Returned bwp "
                << +bwpId
                << " has no DL slot, so the message can't go out. Check your configuration");
        m_ccMap.at(bwpId)->GetPhy()->EncodeCtrlMsg(msg);
    }
}

void
NrGnbNetDevice::DoInitialize()
{
    NS_LOG_FUNCTION(this);
    m_rrc->Initialize();

    NrNetDevice::DoInitialize();
}

void
NrGnbNetDevice::DoDispose()
{
    NS_LOG_FUNCTION(this);

    m_rrc->Dispose();
    m_rrc = nullptr;
    for (const auto& it : m_ccMap)
    {
        it.second->Dispose();
    }
    m_ccMap.clear();
    m_componentCarrierManager->Dispose();
    m_componentCarrierManager = nullptr;
    NrNetDevice::DoDispose();
}

Ptr<NrGnbMac>
NrGnbNetDevice::GetMac(uint8_t index) const
{
    return m_ccMap.at(index)->GetMac();
}

Ptr<NrGnbPhy>
NrGnbNetDevice::GetPhy(uint8_t index) const
{
    NS_LOG_FUNCTION(this);
    return m_ccMap.at(index)->GetPhy();
}

Ptr<BwpManagerGnb>
NrGnbNetDevice::GetBwpManager() const
{
    return DynamicCast<BwpManagerGnb>(m_componentCarrierManager);
}

uint16_t
NrGnbNetDevice::GetCellId() const
{
    NS_LOG_FUNCTION(this);
    return m_cellId;
}

std::vector<uint16_t>
NrGnbNetDevice::GetCellIds() const
{
    std::vector<uint16_t> cellIds;

    for (auto& it : m_ccMap)
    {
        cellIds.push_back(it.second->GetCellId());
    }
    return cellIds;
}

void
NrGnbNetDevice::SetCellId(uint16_t cellId)
{
    NS_LOG_FUNCTION(this);
    m_cellId = cellId;
}

uint16_t
NrGnbNetDevice::GetBwpId(uint8_t index) const
{
    NS_LOG_FUNCTION(this);
    return m_ccMap.at(index)->GetCellId();
}

uint16_t
NrGnbNetDevice::GetEarfcn(uint8_t index) const
{
    NS_LOG_FUNCTION(this);
    return m_ccMap.at(index)->GetDlEarfcn(); // Ul or Dl doesn't matter, they are the same
}

void
NrGnbNetDevice::SetRrc(Ptr<LteEnbRrc> rrc)
{
    m_rrc = rrc;
}

Ptr<LteEnbRrc>
NrGnbNetDevice::GetRrc()
{
    return m_rrc;
}

bool
NrGnbNetDevice::DoSend(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
    NS_LOG_FUNCTION(this << packet << dest << protocolNumber);
    NS_ABORT_MSG_IF(protocolNumber != Ipv4L3Protocol::PROT_NUMBER &&
                        protocolNumber != Ipv6L3Protocol::PROT_NUMBER,
                    "unsupported protocol " << protocolNumber
                                            << ", only IPv4 and IPv6 are supported");
    return m_rrc->SendData(packet);
}

void
NrGnbNetDevice::UpdateConfig()
{
    NS_LOG_FUNCTION(this);

    NS_ASSERT(!m_ccMap.empty());

    std::map<uint8_t, Ptr<ComponentCarrierBaseStation>> ccPhyConfMap;
    for (const auto& i : m_ccMap)
    {
        Ptr<ComponentCarrierBaseStation> c = i.second;
        ccPhyConfMap.insert(std::pair<uint8_t, Ptr<ComponentCarrierBaseStation>>(i.first, c));
    }

    m_rrc->ConfigureCell(ccPhyConfMap);
}

//--------------------------------------------------------- added script ------------------------------------------------------------------

std::string
NrGnbNetDevice::Get_ngapIp() const
{
    NS_LOG_FUNCTION(this);
    return ngapIp;
}

std::string
NrGnbNetDevice::Get_linkIp() const
{
    NS_LOG_FUNCTION(this);
    return linkIp;
}

std::string
NrGnbNetDevice::Get_gtpIp() const
{
    NS_LOG_FUNCTION(this);
    return gtpIp;
}
//// ------------------------------------------setters
void
NrGnbNetDevice::Set_ngapIp(std::string ngap) 
{
    NS_LOG_FUNCTION(this);
    ngapIp = ngap;
}

void
NrGnbNetDevice::Set_linkIp(std::string link) 
{
    NS_LOG_FUNCTION(this);
    linkIp = link;
}

void
NrGnbNetDevice::Set_gtpIp(std::string  gtp)
{
    NS_LOG_FUNCTION(this);
    gtpIp = gtp;
}




static nr::gnb::GnbConfig *ManualConfig(const std::string& ngapIp, const std::string& linkIp, const std::string& gtpIp)
{
    auto *result = new nr::gnb::GnbConfig();

    // Manual Configuration
    result->plmn.mcc = 999; //Mobile Country Code value
    result->plmn.mnc = 70; //Mobile Network Code value (2 or 3 digits)
    result->plmn.isLongMnc = false; // Assuming MNC is 2 digits

    result->nci = 0x000000010; //NR Cell Identity (36-bit)
    result->gnbIdLength = 32; //NR gNB ID length in bits
    result->tac = 1; //Tracking Area Code

    result->linkIp = linkIp;     // gNB's local IP address for Radio Link Simulation
    result->ngapIp = ngapIp;     // gNB's local IP address for N2 Interface
    result->gtpIp = gtpIp;       // gNB's local IP address for N3 Interface

    result->ignoreStreamIds = true;
    result->pagingDrx = EPagingDrx::V128;

    // Name construction based on PLMN information
    result->name = "ID-gnb-" + std::to_string(result->plmn.mcc) + "-" + std::to_string(result->plmn.mnc) + "-" +
                   std::to_string(result->getGnbId()); // NOTE: Avoid using "/" dir separator character. // gnb id in open5gs is derived from gnb id in ns3 

    // AMF Configurations
    nr::gnb::GnbAmfConfig amfConfig;
    amfConfig.address = "192.168.100.5"; // ip of core machine 192.168.56.109
    amfConfig.port = 38412;
    result->amfConfigs.push_back(amfConfig);

    // NSSAI Configurations
    SingleSlice nssai;
    nssai.sst = 1;
    result->nssai.slices.push_back(nssai);

    return result;
}

  void NrGnbNetDevice::ReceiveCommand(app::CliMessage &msg)
{
    if (msg.value.empty())
    {
        g_cliServer->sendMessage(app::CliMessage::Result(msg.clientAddr, ""));
        return;
    }

    std::vector<std::string> tokens{};

    auto exp = opt::PerformExpansion(msg.value, tokens);
    if (exp != opt::ExpansionResult::SUCCESS)
    {
        g_cliServer->sendMessage(app::CliMessage::Error(msg.clientAddr, "Invalid command: " + msg.value));
        return;
    }

    if (tokens.empty())
    {
        g_cliServer->sendMessage(app::CliMessage::Error(msg.clientAddr, "Empty command"));
        return;
    }

    std::string error{}, output{};
    auto cmd = app::ParseGnbCliCommand(std::move(tokens), error, output);
    if (!error.empty())
    {
        g_cliServer->sendMessage(app::CliMessage::Error(msg.clientAddr, error));
        return;
    }
    if (!output.empty())
    {
        g_cliServer->sendMessage(app::CliMessage::Result(msg.clientAddr, output));
        return;
    }
    if (cmd == nullptr)
    {
        g_cliServer->sendMessage(app::CliMessage::Error(msg.clientAddr, ""));
        return;
    }

    if (g_gnbMap.count(msg.nodeName) == 0)
    {
        g_cliServer->sendMessage(app::CliMessage::Error(msg.clientAddr, "Node not found: " + msg.nodeName));
        return;
    }

    auto *gnb = g_gnbMap[msg.nodeName];
    gnb->pushCommand(std::move(cmd), msg.clientAddr);
}

 void NrGnbNetDevice::Loop()
{
    if (!g_cliServer)
    {
        ::pause();
        return;
    }

    auto msg = g_cliServer->receiveMessage();
    if (msg.type == app::CliMessage::Type::ECHO)
    {
        g_cliServer->sendMessage(msg);
        return;
    }

    if (msg.type != app::CliMessage::Type::COMMAND)
        return;

    if (msg.value.size() > 0xFFFF)
    {
        g_cliServer->sendMessage(app::CliMessage::Error(msg.clientAddr, "Command is too large"));
        return;
    }

    if (msg.nodeName.size() > 0xFFFF)
    {
        g_cliServer->sendMessage(app::CliMessage::Error(msg.clientAddr, "Node name is too large"));
        return;
    }

    ReceiveCommand(msg);
}

void NrGnbNetDevice::generateIPAddress(uint16_t x, std::string& ngapIp, std::string& linkIp, std::string& gtpIp) {
	x=x+1; //to avoid conflicts
	
	if (x==2){
	    ngapIp = "192.168.100.4"; //192.168.56.106
		gtpIp ="192.168.33.3";  //192.168.56.108
		}
    else{
		ngapIp = "192.168.56.110"; //192.168.56.110
		gtpIp ="192.168.33.3"; //192.168.56.108
		}
 std::string ipBase = "127.0.0."; //this part of the function is obselete whatever it does is then discarded
    for (int i = 0; i < 2; ++i) {
        if (i == 0) {
             linkIp = "127.0.0.1";//ipBase + std::to_string(100 + x + i); // needs to be made automatic once bs selection is automized
        } 
    }
}

  void NrGnbNetDevice::ConnectOpen5gs() 
{
	std::cout<<"============================================="<<std::endl;
	static bool isFirstTime = true;
	if (isFirstTime){
    app::Initialize();}
    std::string ngapIp;
    std::string linkIp;
    std::string gtpIp;
    uint16_t x = GetCellId();
    generateIPAddress(x,ngapIp,linkIp,gtpIp); //generate the ip addresses needed for the bs to communicate with open5gs
    Set_gtpIp(gtpIp); //call the setters to save this info in the BS instance
    Set_linkIp(linkIp);
    Set_ngapIp(ngapIp);
    std::string ipBase = "127.0.0.";
    linkIp = ipBase + std::to_string(j+1);   
    j++;
    g_refConfig = ManualConfig(ngapIp,linkIp,gtpIp);   
    g_cliServer = new app::CliServer{};
    g_cliRespTask = new app::CliResponseTask(g_cliServer);
     
    auto *gnb = new nr::gnb::GNodeB(g_refConfig, nullptr, g_cliRespTask);
    g_gnbMap[g_refConfig->name] = gnb;

    app::CreateProcTable(g_gnbMap, g_cliServer->assignedAddress().getPort());
    g_cliRespTask->start();    

    gnb->start();
    isFirstTime = false;
    Loop();

}


//------------------------------------------------------ end of addedd script----------------------------------------------------------------


} // namespace ns3
