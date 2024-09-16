/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-ue-net-device.h"

#include "bandwidth-part-ue.h"
#include "bwp-manager-ue.h"
#include "nr-gnb-net-device.h"
#include "nr-ue-mac.h"
#include "nr-ue-phy.h"

#include <ns3/epc-ue-nas.h>
#include <ns3/ipv4-l3-protocol.h>
#include <ns3/ipv6-l3-protocol.h>
#include <ns3/lte-ue-component-carrier-manager.h>
#include <ns3/lte-ue-rrc.h>
#include <ns3/object-map.h>
#include <ns3/pointer.h>
#include <cstdlib> 
#include <string>  
#include <sstream> 

// NetAnim import
#include "ns3/netanim-module.h"

// UE UERANSIM import
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

#include <unistd.h>

#include <ns3/base_app.hpp>
#include <ns3/cli_base.hpp>
#include <ns3/cli_cmd.hpp>
#include <ns3/proc_table.hpp>
#include <ns3/ue_ctl.hpp>
#include <ns3/ue.hpp>
#include <ns3/common.hpp>
#include <ns3/concurrent_map.hpp>
#include <ns3/constants.hpp>
#include <ns3/options.hpp>
#include <ns3/yaml_utils.hpp>
#include <ns3/yaml.h>
uint16_t gNbNum;
uint16_t ueNumPergNb;
static app::CliServer *g_cliServer = nullptr;
static nr::ue::UeConfig *g_refConfig = nullptr;
static ConcurrentMap<std::string, nr::ue::UserEquipment *> g_ueMap{};
static app::CliResponseTask *g_cliRespTask = nullptr;
static int i1 = 0;
static int i2 = 1;
static uint64_t IMSI= 0;
static int associated = 1;
namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrUeNetDevice");

NS_OBJECT_ENSURE_REGISTERED(NrUeNetDevice);

TypeId
NrUeNetDevice::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrUeNetDevice")
            .SetParent<NrNetDevice>()
            .AddConstructor<NrUeNetDevice>()
            .AddAttribute("EpcUeNas",
                          "The NAS associated to this UeNetDevice",
                          PointerValue(),
                          MakePointerAccessor(&NrUeNetDevice::m_nas),
                          MakePointerChecker<EpcUeNas>())
            .AddAttribute("nrUeRrc",
                          "The RRC associated to this UeNetDevice",
                          PointerValue(),
                          MakePointerAccessor(&NrUeNetDevice::m_rrc),
                          MakePointerChecker<LteUeRrc>())
            .AddAttribute("Imsi",
                          "International Mobile Subscriber Identity assigned to this UE",
                          UintegerValue(0),
                          MakeUintegerAccessor(&NrUeNetDevice::m_imsi),
                          MakeUintegerChecker<uint64_t>())
            .AddAttribute("LteUeRrc",
                          "The RRC layer associated with the ENB",
                          PointerValue(),
                          MakePointerAccessor(&NrUeNetDevice::m_rrc),
                          MakePointerChecker<LteUeRrc>())
            .AddAttribute("LteUeComponentCarrierManager",
                          "The ComponentCarrierManager associated to this UeNetDevice",
                          PointerValue(),
                          MakePointerAccessor(&NrUeNetDevice::m_componentCarrierManager),
                          MakePointerChecker<LteUeComponentCarrierManager>())
            .AddAttribute("ComponentCarrierMapUe",
                          "List of all component Carrier.",
                          ObjectMapValue(),
                          MakeObjectMapAccessor(&NrUeNetDevice::m_ccMap),
                          MakeObjectMapChecker<BandwidthPartUe>());
    return tid;
}

NrUeNetDevice::NrUeNetDevice()
{
    NS_LOG_FUNCTION(this);
}

NrUeNetDevice::~NrUeNetDevice()
{
}

void
NrUeNetDevice::DoInitialize()
{
    NS_LOG_FUNCTION(this);

    m_rrc->Initialize();
}

void
NrUeNetDevice::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_rrc->Dispose();
    m_rrc = nullptr;

    m_targetEnb = nullptr;
    m_nas->Dispose();
    m_nas = nullptr;
    for (const auto& it : m_ccMap)
    {
        it.second->Dispose();
    }
    m_ccMap.clear();
    m_componentCarrierManager->Dispose();
    m_componentCarrierManager = nullptr;
    NrNetDevice::DoDispose();
}

std::map<uint8_t, Ptr<BandwidthPartUe>>
NrUeNetDevice::GetCcMap()
{
    NS_LOG_FUNCTION(this);
    return m_ccMap;
}

uint32_t
NrUeNetDevice::GetCcMapSize() const
{
    NS_LOG_FUNCTION(this);
    return m_ccMap.size();
}

void
NrUeNetDevice::EnqueueDlHarqFeedback(const DlHarqInfo& m) const
{
    NS_LOG_FUNCTION(this);

    auto ccManager = DynamicCast<BwpManagerUe>(m_componentCarrierManager);
    NS_ASSERT(ccManager != nullptr);
    uint8_t index = ccManager->RouteDlHarqFeedback(m);
    m_ccMap.at(index)->GetPhy()->EnqueueDlHarqFeedback(m);
}

void
NrUeNetDevice::RouteIngoingCtrlMsgs(const std::list<Ptr<NrControlMessage>>& msgList,
                                    uint8_t sourceBwpId)
{
    NS_LOG_FUNCTION(this);

    for (const auto& msg : msgList)
    {
        uint8_t bwpId = DynamicCast<BwpManagerUe>(m_componentCarrierManager)
                            ->RouteIngoingCtrlMsg(msg, sourceBwpId);
        m_ccMap.at(bwpId)->GetPhy()->PhyCtrlMessagesReceived(msg);
    }
}

void
NrUeNetDevice::RouteOutgoingCtrlMsgs(const std::list<Ptr<NrControlMessage>>& msgList,
                                     uint8_t sourceBwpId)
{
    NS_LOG_FUNCTION(this);

    for (const auto& msg : msgList)
    {
        uint8_t bwpId = DynamicCast<BwpManagerUe>(m_componentCarrierManager)
                            ->RouteOutgoingCtrlMsg(msg, sourceBwpId);
        NS_ASSERT_MSG(m_ccMap.size() > bwpId,
                      "Returned bwp " << +bwpId << " is not present. Check your configuration");
        NS_ASSERT_MSG(
            m_ccMap.at(bwpId)->GetPhy()->HasUlSlot(),
            "Returned bwp "
                << +bwpId
                << " has no UL slot, so the message can't go out. Check your configuration");
        m_ccMap.at(bwpId)->GetPhy()->EncodeCtrlMsg(msg);
    }
}

void
NrUeNetDevice::SetCcMap(std::map<uint8_t, Ptr<BandwidthPartUe>> ccm)
{
    NS_LOG_FUNCTION(this);
    NS_ABORT_IF(m_ccMap.size() > 0);
    m_ccMap = ccm;
}

uint32_t
NrUeNetDevice::GetCsgId() const
{
    NS_LOG_FUNCTION(this);
    return m_csgId;
}

void
NrUeNetDevice::SetCsgId(uint32_t csgId)
{
    NS_LOG_FUNCTION(this << csgId);
    m_csgId = csgId;
    UpdateConfig(); // propagate the change down to NAS and RRC
}

Ptr<NrUeMac>
NrUeNetDevice::GetMac(uint8_t index) const
{
    NS_LOG_FUNCTION(this);
    return m_ccMap.at(index)->GetMac();
}

void
NrUeNetDevice::UpdateConfig()
{
    NS_LOG_FUNCTION(this);
    m_nas->SetImsi(m_imsi);
    m_rrc->SetImsi(m_imsi);
    m_nas->SetCsgId(m_csgId); // this also handles propagation to RRC
}

bool
NrUeNetDevice::DoSend(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
    NS_LOG_FUNCTION(this << packet << dest << protocolNumber);
    NS_ABORT_MSG_IF(protocolNumber != Ipv4L3Protocol::PROT_NUMBER &&
                        protocolNumber != Ipv6L3Protocol::PROT_NUMBER,
                    "unsupported protocol " << protocolNumber
                                            << ", only IPv4 and IPv6 are supported");
    return m_nas->Send(packet, protocolNumber);
}

Ptr<NrUePhy>
NrUeNetDevice::GetPhy(uint8_t index) const
{
    NS_LOG_FUNCTION(this);
    return m_ccMap.at(index)->GetPhy();
}

Ptr<BwpManagerUe>
NrUeNetDevice::GetBwpManager() const
{
    NS_LOG_FUNCTION(this);
    return DynamicCast<BwpManagerUe>(m_componentCarrierManager);
}

Ptr<EpcUeNas>
NrUeNetDevice::GetNas() const
{
    NS_LOG_FUNCTION(this);
    return m_nas;
}

Ptr<LteUeRrc>
NrUeNetDevice::GetRrc() const
{
    NS_LOG_FUNCTION(this);
    return m_rrc;
}

uint64_t
NrUeNetDevice::GetImsi() const
{
    NS_LOG_FUNCTION(this);
    return m_imsi;
}

uint16_t
NrUeNetDevice::GetCellId() const
{
    auto gnb = GetTargetEnb();
    if (gnb)
    {
        return GetTargetEnb()->GetCellId();
    }
    else
    {
        return UINT16_MAX;
    }
}

void
NrUeNetDevice::SetTargetEnb(Ptr<NrGnbNetDevice> enb)
{
    NS_LOG_FUNCTION(this);
    m_targetEnb = enb;
}

Ptr<const NrGnbNetDevice>
NrUeNetDevice::GetTargetEnb() const
{
    NS_LOG_FUNCTION(this);
    return m_targetEnb;
}



// added code to conneect to open5gs-------------------------------------

 uint64_t NrUeNetDevice::Get_imsi_open5gs() const
{
    NS_LOG_FUNCTION(this);
    return m_imsi_open5gs;
}


 void NrUeNetDevice::set_imsi_open5gs(uint64_t IMSI) 
{
    NS_LOG_FUNCTION(this);
    m_imsi_open5gs = IMSI;
}

// LargeSum
static std::string LargeSum(std::string a, std::string b)
{
    if (a.length() > b.length())
        std::swap(a, b);

    std::string str;
    size_t n1 = a.length(), n2 = b.length();

    reverse(a.begin(), a.end());
    reverse(b.begin(), b.end());

    int carry = 0;
    for (size_t i = 0; i < n1; i++)
    {
        int sum = ((a[i] - '0') + (b[i] - '0') + carry);
        str.push_back(static_cast<char>((sum % 10) + '0'));
        carry = sum / 10;
    }
    for (size_t i = n1; i < n2; i++)
    {
        int sum = ((b[i] - '0') + carry);
        str.push_back(static_cast<char>((sum % 10) + '0'));
        carry = sum / 10;
    }
    if (carry)
        throw std::runtime_error("UE serial number overflow");
    reverse(str.begin(), str.end());
    return str;
}

// IncrementNumber
static void IncrementNumber(std::string &s, int delta)
{
    s = LargeSum(s, std::to_string(delta));
}

struct NwUeControllerCmd : NtsMessage
{
    enum PR
    {
        PERFORM_SWITCH_OFF,
    } present;

    // PERFORM_SWITCH_OFF
    nr::ue::UserEquipment *ue{};

    explicit NwUeControllerCmd(PR present) : NtsMessage(NtsMessageType::UE_CTL_COMMAND), present(present)
    {
    }
};

class UeControllerTask : public NtsTask
{
  protected:
    void onStart() override
    {
    }

    void onLoop() override
    {
        auto msg = take();
        if (msg == nullptr)
            return;
        if (msg->msgType == NtsMessageType::UE_CTL_COMMAND)
        {
            auto &w = dynamic_cast<NwUeControllerCmd &>(*msg);
            switch (w.present)
            {
            case NwUeControllerCmd::PERFORM_SWITCH_OFF: {
                std::string key{};
                g_ueMap.invokeForeach([&key, &w](auto &item) {
                    if (item.second == w.ue)
                        key = item.first;
                });

                if (key.empty())
                    return;

                if (g_ueMap.removeAndGetSize(key) == 0)
                    exit(0);

                delete w.ue;
                break;
            }
            }
        }
    }

    void onQuit() override
    {
    }
};


static UeControllerTask *g_controllerTask;

// UE CONFIG

static nr::ue::UeConfig *ManualUeConfig(uint16_t cell_ID, uint64_t IMSI, std::string IP)
{
    auto *result = new nr::ue::UeConfig();
    // IMSI number of the UE
    std::string supi_composed = "imsi-" + std::to_string(IMSI);
    result->supi = Supi::Parse(supi_composed);

    // Mobile Country Code and Mobile Network Code of HPLMN
    result->hplmn.mcc = 999;
    result->hplmn.mnc = 70;
    //result->hplmn.isLongMnc = true;

    // SUCI Protection Scheme
    result->protectionScheme = 0;

    // Home Network Public Key and ID for SUCI Profile A
    result->homeNetworkPublicKey = OctetString::FromHex("5a8d38864820197c3394b92613b20b91633cbd897119273bf8e4a6f4eec0a650");
    result->homeNetworkPublicKeyId = 1;

    // Routing Indicator
    result->routingIndicator = "0000";

    // Permanent subscription key, Operator code, OP type, AMF, IMEI, IMEISV

    result->key = OctetString::FromHex("465B5CE8B199B49FAA5F0A2EE238A6BC");
    result->opC = OctetString::FromHex("E8ED289DEBA952E4283B54E88E6183CA");
    result->opType = nr::ue::OpType::OPC;
    result->amf = OctetString::FromHex("8000");
    result->imei = "356938035643803";
    result->imeiSv = "4370816125816151";

    IncrementNumber(*result->imei, 1); //change the imei for every defined ue 
    IncrementNumber(*result->imeiSv, 1); // change the imeisv for every defined ue 
    // List of gNB IP addresses for 
    result->gnbSearchList.push_back(IP); // IP of UE's BS sent as an arguement retreived from getters
    // UAC Access Identities Configuration
    result->uacAic.mps = false;
    result->uacAic.mcs = false;

    // UAC Access Control Class
    result->uacAcc.normalCls = 0;
    result->uacAcc.cls11 = false;
    result->uacAcc.cls12 = false;
    result->uacAcc.cls13 = false;
    result->uacAcc.cls14 = false;
    result->uacAcc.cls15 = false;

    // Initial PDU sessions to be established
    nr::ue::SessionConfig session{};
    session.type = nas::EPduSessionType::IPV4;
    session.apn = "internet";
    
    SingleSlice slice{};
    slice.sst = 1;
    
    session.sNssai = slice;
    
    result->defaultSessions.push_back(session);

     // Configured NSSAI for this UE by HPLMN
     SingleSlice configuredNssai{};
     configuredNssai.sst=1;
     result->configuredNssai.slices.push_back(configuredNssai);

     // Default Configured NSSAI for this UE     
     SingleSlice defaultNssai{};
     defaultNssai.sst=1;
     defaultNssai.sd = octet3{1};
     result->defaultConfiguredNssai.slices.push_back(defaultNssai);     
       
     // Supported integrity algorithms by this UE
     result->supportedAlgs.nia1=true;
     result->supportedAlgs.nia2=true;
     result->supportedAlgs.nia3=true;

     // Supported encryption algorithms by this UE
     result->supportedAlgs.nea1=true;
     result->supportedAlgs.nea2=true;
     result->supportedAlgs.nea3=true;

     // Integrity protection maximum data rate for user plane
     result ->integrityMaxRate.uplinkFull=true;
     result ->integrityMaxRate.downlinkFull=true;

   return result;
}

void NrUeNetDevice::ReceiveCommand(app::CliMessage &msg)
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
    auto cmd = app::ParseUeCliCommand(std::move(tokens), error, output);
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

    auto *ue = g_ueMap.getOrDefault(msg.nodeName);
    if (ue == nullptr)
    {
        g_cliServer->sendMessage(app::CliMessage::Error(msg.clientAddr, "Node not found: " + msg.nodeName));
        return;
    }

    ue->pushCommand(std::move(cmd), msg.clientAddr);
}
void NrUeNetDevice::Loop()
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

// UeController 'g_ueController'
static class UeController : public app::IUeController
{
  public:
    void performSwitchOff(nr::ue::UserEquipment *ue) override
    {
        auto w = std::make_unique<NwUeControllerCmd>(NwUeControllerCmd::PERFORM_SWITCH_OFF);
        w->ue = ue;
        g_controllerTask->push(std::move(w));
    }
} g_ueController;


 void NrUeNetDevice::ConnectOpen5gs() 
{
    std::cout<<"============================================="<<std::endl;
    uint16_t cell_ID = GetCellId(); // get BS to which this UE is connected 

    //uint64_t IMSI = GetImsi(); //automatic procedure to configure the open5gs IMSI following the IMSI in ns3
    //this needed attachement to BS to be done before connection to open5gs which was refused by nokia

    //uint64_t IMSI_in_open5gs = 999700000000000 + IMSI; //the mcc and mnc are hard coded in this configuration
    //removed to make a manual assignment as per previous comment 
    
    IMSI = IMSI + 1; //ns3 allocates UE IDs following their creation: 1, 2, 3, etc .. this logic is kept to connect to open5gs
    
    
    uint64_t IMSI_in_open5gs = 999700000000000 + IMSI; 
    
    
    set_imsi_open5gs(IMSI_in_open5gs); //save the open5gs IMSI as a ue-net-device attribute
    
    // The automated process to get the connected gnb for the UE cannot be done since it was requested that connection to core occurs first. The following is a manual approximation.
    
   // Ptr<const NrGnbNetDevice> associated_gnb= GetTargetEnb(); //get a pointer to the associated gnb-net-device of this UE 
    
   // std::string IP = associated_gnb->Get_linkIp(); // link to the BS logically via this link 

    std::string ipBase = "127.0.0.";
    std::string IP;
    IP = ipBase + std::to_string(i2);
    
   // associated=associated+2;
    
    g_refConfig = ManualUeConfig(cell_ID, IMSI_in_open5gs,IP); // config information for ue to interface with open5gs

    g_controllerTask = new UeControllerTask();

    g_controllerTask->start();

    g_cliServer = new app::CliServer{};

    g_cliRespTask = new app::CliResponseTask(g_cliServer);

    //auto *config = GetConfigByUe(i);

    auto *ue = new nr::ue::UserEquipment(g_refConfig, &g_ueController, nullptr, g_cliRespTask);

    //g_ueMap.put(config->getNodeName(), ue);

    app::CreateProcTable(g_ueMap, g_cliServer->assignedAddress().getPort());

    g_cliRespTask->start();
    
    ue->start();
    
    // if (i==1){  
   // g_ueMap.invokeForeach([](const auto &ue) { ue.second->start(); });}

    i1++;
        if(i1 == ueNumPergNb) // allocation of UE IDs finished for BS1 
    { i1=0;
	  i2++;
  } 
    Loop();


}

//-----------------------------------------------------------------------

} // namespace ns3
