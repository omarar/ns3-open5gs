//
// Omar code version 
//


#include "ctl_task.hpp"
#include <iostream>

#include <stdexcept>
#include <ns3/common.hpp>
#include <map>

#include "ns3/socket.h"
#include "../../../ns-3-dev/contrib/nr/model/pdcp/pdcp.cpp"

#include "ns3/lte-enb-rrc.h"



static constexpr const size_t MAX_PDU_COUNT = 4096;
static constexpr const int MAX_PDU_TTL = 3000;

static constexpr const int TIMER_ID_ACK_CONTROL = 1;
static constexpr const int TIMER_ID_ACK_SEND = 2;

static constexpr const int TIMER_PERIOD_ACK_CONTROL = 1500;
static constexpr const int TIMER_PERIOD_ACK_SEND = 2250;

namespace nr::gnb
{


// Modify the constructor to accept the PDCP reference
RlsControlTask::RlsControlTask(TaskBase* base, uint64_t sti)
    : m_sti{sti},
      m_mainTask{},
      m_udpTask{},
      m_pduMap{},
      m_pendingAck{},
      m_pdcp(nullptr)

{
    m_logger = base->logBase->makeUniqueLogger("rls-ctl");

}

// Add the setter for PDCP
/*
void
RlsControlTask::SetPdcp(ns3::Ptr<ns3::LtePdcp> pdcp)
{
    m_pdcp=pdcp;
}

ns3::Ptr<ns3::LtePdcp> RlsControlTask::GetPdcp()
{
    return m_pdcp;  // Return the PDCP instance
}
*/
void
RlsControlTask::initialize(NtsTask* mainTask, RlsUdpTask* udpTask)
{
    m_mainTask = mainTask;
    m_udpTask = udpTask;

//////
    /*
        //
        // Scenario parameters (that we will use inside this script):
        uint16_t gNbNum = 1;
        uint16_t ueNumPergNb = 2;
        bool logging = false;
        bool doubleOperationalBand = true;

    // Traffic parameters (that we will use inside this script):
    uint32_t udpPacketSizeULL = 100;
    uint32_t udpPacketSizeBe = 1252;
    uint32_t lambdaULL = 10000;
    uint32_t lambdaBe = 10000;

    // Simulation parameters. Please don't use double to indicate seconds; use
    // ns-3 Time values which use integers to avoid portability issues.


    // NR parameters (Reference: 3GPP TR 38.901 V17.0.0 (Release 17)
    // Table 7.8-1 for the power and BW).
    // In this example the BW has been split into two BWPs
    // We will take the input from the command line, and then we
    // will pass them inside the NR module.
    uint16_t numerologyBwp1 = 4;
    double centralFrequencyBand1 = 28e9;
    double bandwidthBand1 = 50e6;
    uint16_t numerologyBwp2 = 2;
    double centralFrequencyBand2 = 28.2e9;
    double bandwidthBand2 = 50e6;
    double totalTxPower = 35;

    // Where we will store the output files.
    std::string simTag = "default";
    std::string outputDir = "./";
    //

    ns3::Ptr<ns3::NrPointToPointEpcHelper> epcHelper = ns3::CreateObject<ns3::NrPointToPointEpcHelper> ();
    ns3::Ptr<ns3::IdealBeamformingHelper> idealBeamformingHelper = ns3::CreateObject<ns3::IdealBeamformingHelper>();

    nrHelper = ns3::CreateObject<ns3::NrHelper>();

    // Set EPC and beamforming helpers
    nrHelper->SetBeamformingHelper(idealBeamformingHelper);
    nrHelper->SetEpcHelper(epcHelper);

    m_gnbNodeContainer.Create(1);

    // Set mobility model
    ns3::MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    ns3::Ptr<ns3::ListPositionAllocator> positionAlloc = ns3::CreateObject<ns3::ListPositionAllocator>();
    positionAlloc->Add(ns3::Vector(0.0, 0.0, 10.0));  // gNB position
    mobility.SetPositionAllocator(positionAlloc);
    mobility.Install(m_gnbNodeContainer);

    double centralFrequency = 3.5e9;
    double bandwidth = 20e6;
    uint16_t losCondition = 0;

    ns3::BandwidthPartInfoPtrVector allBwps;

    NS_ABORT_IF(centralFrequency < 0.5e9 && centralFrequency > 100e9);

    ns3::BandwidthPartInfo::Scenario scenario =
        ns3::BandwidthPartInfo::Scenario(ns3::BandwidthPartInfo::UMi_StreetCanyon + losCondition);

    ns3::CcBwpCreator ccBwpCreator;
    const uint8_t numCcPerBand = 1;
    ns3::CcBwpCreator::SimpleOperationBandConf bandConf(centralFrequency,
                                                        bandwidth,
                                                        numCcPerBand,
                                                        scenario);
    ns3::OperationBandInfo band = ccBwpCreator.CreateOperationBandContiguousCc(bandConf);

    // Print values before installing gNB device
    std::cout << "m_nrHelper: " << nrHelper << std::endl;
    std::cout << "m_gnbNodes size: " << m_gnbNodeContainer.GetN() << std::endl;
    std::cout << "allBwps size: " << allBwps.size() << std::endl;
    std::cout << "Installing gNB Device" << std::endl;

    //double totalTxPower = 35;
    //double x = pow(10, totalTxPower / 10);
    //double totalBandwidth = 50e6;

    idealBeamformingHelper->SetAttribute("BeamformingMethod", ns3::TypeIdValue(ns3::TypeId::LookupByName("ns3::DirectPathBeamforming")));
    epcHelper->SetAttribute("S1uLinkDelay", ns3::TimeValue(ns3::MilliSeconds(0)));
    nrHelper->SetGnbAntennaAttribute("NumRows", ns3::UintegerValue(4));
    nrHelper->SetGnbAntennaAttribute("NumColumns", ns3::UintegerValue(8));
    nrHelper->SetGnbAntennaAttribute("AntennaElement", ns3::PointerValue(ns3::CreateObject<ns3::IsotropicAntennaModel>()));
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB", ns3::UintegerValue(0));

    nrHelper->SetChannelConditionModelAttribute("UpdatePeriod", ns3::TimeValue(ns3::MilliSeconds(0)));
    nrHelper->SetPathlossAttribute("ShadowingEnabled", ns3::BooleanValue(false));

    nrHelper->InitializeOperationBand(&band); //!!!
    allBwps = ns3::CcBwpCreator::GetAllBwps({band});
    //
    //ns3::Packet::EnableChecking();
    //ns3::Packet::EnablePrinting();

    // Beamforming method
    idealBeamformingHelper->SetAttribute("BeamformingMethod",
                                         ns3::TypeIdValue(ns3::DirectPathBeamforming::GetTypeId()));

    // Core latency
    epcHelper->SetAttribute("S1uLinkDelay", ns3::TimeValue(ns3::MilliSeconds(0)));

    // Antennas for all the gNbs
    nrHelper->SetGnbAntennaAttribute("NumRows", ns3::UintegerValue(4));
    nrHelper->SetGnbAntennaAttribute("NumColumns", ns3::UintegerValue(8));
    nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                     ns3::PointerValue(ns3::CreateObject<ns3::IsotropicAntennaModel>()));

    uint32_t bwpId = 0;
    // gNb routing between bearer type and bandwidth part
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB", ns3::UintegerValue(bwpId));
    // UE routing between bearer type and bandwidth part
    nrHelper->SetUeBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB", ns3::UintegerValue(bwpId));
    //nrHelper->InitializeOperationBand(&band);
    //allBwps = ns3::CcBwpCreator::GetAllBwps({band});

    //
    //m_gnbNetDeviceContainer = nrHelper->InstallGnbDevice(m_gnbNodeContainer, allBwps);
    //ns3::Ipv4AddressGenerator::Reset();

    int64_t randomStream = 1;
    randomStream += nrHelper->AssignStreams(m_gnbNetDeviceContainer, randomStream);

    for (auto it = m_gnbNetDeviceContainer.Begin(); it != m_gnbNetDeviceContainer.End(); ++it)
    {
        ns3::DynamicCast<ns3::NrGnbNetDevice>(*it)->UpdateConfig();
    }

    std::cout << "gNB Device installed successfully." << std::endl;
    std::cout << "m_gnbNetDeviceContainer======" << m_gnbNetDeviceContainer.GetN() << std::endl;
    //std::cout << "nr::gnb::m_gnbNetDeviceContainer======" << nr::gnb::m_gnbNetDeviceContainer.GetN() << std::endl;
    // Use NetworkResourceManager to store the gNB devices globally
    NetworkResourceManager::getInstance().setGnbNetDeviceContainer(1, m_gnbNetDeviceContainer);

    std::cout << "ueDevs======" << nr::ue::g_ueNetDeviceContainer.GetN() << std::endl;

    //nrHelper->AttachToClosestEnb(nr::ue::g_ueNetDeviceContainer, m_gnbNetDeviceContainer);

    */
//////
}

void
RlsControlTask::onStart()
{
    setTimer(TIMER_ID_ACK_CONTROL, TIMER_PERIOD_ACK_CONTROL);
    setTimer(TIMER_ID_ACK_SEND, TIMER_PERIOD_ACK_SEND);
}

void
RlsControlTask::onLoop()
{
    auto msg = take();
    if (!msg)
        return;

    switch (msg->msgType)
    {
    case NtsMessageType::GNB_RLS_TO_RLS: {
        auto& w = dynamic_cast<NmGnbRlsToRls&>(*msg);
        /*
        // Check PDCP map size before processing DOWNLINK_RRC
        if (w.present == NmGnbRlsToRls::DOWNLINK_RRC && ns3::Pdcp::GetMapSize() == 0) {
            std::cout << "Waiting for PDCP instances to be created..." << std::endl;
            return;  // Exit without processing until the PDCP map has instances
        }*/

        switch (w.present)
        {
        case NmGnbRlsToRls::SIGNAL_DETECTED:
            handleSignalDetected(w.ueId);
            break;
        case NmGnbRlsToRls::SIGNAL_LOST:
            handleSignalLost(w.ueId);
            break;
        case NmGnbRlsToRls::RECEIVE_RLS_MESSAGE:
            handleRlsMessage(w.ueId, *w.msg);
            break;
        case NmGnbRlsToRls::DOWNLINK_DATA:
            handleDownlinkDataDelivery(w.ueId, w.psi, std::move(w.data));
            break;
        case NmGnbRlsToRls::DOWNLINK_RRC:
            handleDownlinkRrcDelivery(w.ueId, w.pduId, w.rrcChannel, std::move(w.data));
            break;
        default:
            m_logger->unhandledNts(*msg);
            break;
        }
        break;
    }
    case NtsMessageType::TIMER_EXPIRED: {
        auto& w = dynamic_cast<NmTimerExpired&>(*msg);
        if (w.timerId == TIMER_ID_ACK_CONTROL)
        {
            setTimer(TIMER_ID_ACK_CONTROL, TIMER_PERIOD_ACK_CONTROL);
            onAckControlTimerExpired();
        }
        else if (w.timerId == TIMER_ID_ACK_SEND)
        {
            setTimer(TIMER_ID_ACK_SEND, TIMER_PERIOD_ACK_SEND);
            onAckSendTimerExpired();
        }
        break;
    }
    default:
        m_logger->unhandledNts(*msg);
        break;
    }
}

void
RlsControlTask::onQuit()
{
}

void
RlsControlTask::handleSignalDetected(int ueId)
{
    auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::SIGNAL_DETECTED);
    w->ueId = ueId;
    // Update the attachment state
    std::cerr << "handleSignalDetected UE " << ueId << std::endl;

    m_mainTask->push(std::move(w));
}

void
RlsControlTask::handleSignalLost(int ueId)
{
    auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::SIGNAL_LOST);
    w->ueId = ueId;
    m_mainTask->push(std::move(w));
}

void
RlsControlTask::handleRlsMessage(int ueId, rls::RlsMessage& msg)
{
    if (msg.msgType == rls::EMessageType::PDU_TRANSMISSION_ACK)
    {
        auto& m = (rls::RlsPduTransmissionAck&)msg;
        for (auto pduId : m.pduIds)
            m_pduMap.erase(pduId);
    }
    else if (msg.msgType == rls::EMessageType::PDU_TRANSMISSION)
    {
        auto& m = (rls::RlsPduTransmission&)msg;
        if (m.pduId != 0)
            m_pendingAck[ueId].push_back(m.pduId);

        if (m.pduType == rls::EPduType::DATA)
        {
            auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::UPLINK_DATA);
            w->ueId = ueId;
            w->psi = static_cast<int>(m.payload);
            w->data = std::move(m.pdu);
            m_mainTask->push(std::move(w));
        }
        else if (m.pduType == rls::EPduType::RRC)
        {
            auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::UPLINK_RRC);
            w->ueId = ueId;
            w->rrcChannel = static_cast<rrc::RrcChannel>(m.payload);
            w->data = std::move(m.pdu);
            m_mainTask->push(std::move(w));
        }
        else
        {
            m_logger->err("Unhandled RLS PDU type");
        }
    }
    else
    {
        m_logger->err("Unhandled RLS message type");
    }
}

void
RlsControlTask::handleDownlinkRrcDelivery(int ueId,
                                          uint32_t pduId,
                                          rrc::RrcChannel channel,
                                          OctetString&& data)
{

    std::cout << "°°°°°°°°°°°°°°°°°°°°°°°°°°°RlsControlTask::handleDownlinkRrcDelivery  for UE ID: " << ueId << std::endl;

    //ns3::Ptr<ns3::Pdcp> pdcpManager = ns3::Pdcp::GetInstance();  // Get the shared PDCP manager instance

    size_t mapSize = ns3::Pdcp::GetMapSize();  // Get the current map size
    //std::cout << "GNB CTL TASK %%%%%%%%%%%%%%%%%%%%%%%%%%%% Current size of m_UeId_PdcpMap: " << mapSize << std::endl;
    ns3::Pdcp& pdcpManager = ns3::Pdcp::GetInstance();
    pdcpManager.GetMapSize();
    //std::cout << "GNB CTL TASK %%%%%%%%%%%%%%%%%%%%%%%%%%%% Current size of m_UeId_PdcpMap: " << pdcpManager.GetMapSize() << std::endl;

    if (pdcpManager.HasPdcp(ueId)) {
        ns3::Ptr<ns3::LtePdcp> pdcp = pdcpManager.GetPdcp(ueId);
        std::cout << "************************************Retrieved PDCP for UE ID: " << ueId << std::endl;
    } else {
        std::cout << "No PDCP instance found for UE ID: " << ueId << std::endl;
    }

    std::cout << "__________________________PDU Info:" << data.length() << std::endl;
    //std::cout << "End Point ID: " << m_pduMap[pduId].endPointId << std::endl;
    //std::cout << "PDU ID: " << m_pduMap[pduId].id << std::endl;
    //std::cout << "PDU Size: " << m_pduMap[pduId].pdu.length() << std::endl;
    //std::cout << "RRC Channel: " << static_cast<int>(m_pduMap[pduId].rrcChannel) << std::endl;
    //std::cout << "Sent Time: " << m_pduMap[pduId].sentTime << std::endl;
    ///////
    if (ueId == 0 && pduId != 0)
    {
        // PDU ID must be not set in case of broadcast
        throw std::runtime_error("");
    }

    if (pduId != 0)
    {
        if (m_pduMap.count(pduId))
        {
            m_pduMap.clear();

            auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::RADIO_LINK_FAILURE);
            w->rlfCause = rls::ERlfCause::PDU_ID_EXISTS;
            m_mainTask->push(std::move(w));
            return;
        }

        if (m_pduMap.size() > MAX_PDU_COUNT)
        {
            m_pduMap.clear();

            auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::RADIO_LINK_FAILURE);
            w->rlfCause = rls::ERlfCause::PDU_ID_FULL;
            m_mainTask->push(std::move(w));
            return;
        }

        m_pduMap[pduId].endPointId = ueId;
        m_pduMap[pduId].id = pduId;
        m_pduMap[pduId].pdu = data.copy();
        m_pduMap[pduId].rrcChannel = channel;
        m_pduMap[pduId].sentTime = utils::CurrentTimeMillis();
        // Logging the PDU information
        std::cout << "handleDownlinkRrcDelivery PDU Info:" << std::endl;
        std::cout << "handleDownlinkRrcDelivery End Point ID: " << m_pduMap[pduId].endPointId << std::endl;
        std::cout << "handleDownlinkRrcDelivery PDU ID: " << m_pduMap[pduId].id << std::endl;
        std::cout << "handleDownlinkRrcDelivery PDU Size: " << m_pduMap[pduId].pdu.length() << std::endl;
        std::cout << "handleDownlinkRrcDelivery RRC Channel: " << static_cast<int>(m_pduMap[pduId].rrcChannel) << std::endl;
        std::cout << "handleDownlinkRrcDelivery Sent Time: " << m_pduMap[pduId].sentTime << std::endl;
    }
    //////////

    //////////
    // PDCP Layer integration: Transmit PDCP SDU
    std::cout << "gNB RLS 'ctl_task.cpp' data.length(): " << data.length() << std::endl;
    std::cout << "gNB RLS 'ctl_task.cpp' UE ID: " << ueId << std::endl;

    // Find the PDCP instance in the pdcp map for further processing
    if (data.length() > 0 && pdcpManager.HasPdcp(ueId))
    {

            std::cout << "RRC TO PDCP DOWNLINK for this UE = " << ueId << std::endl;
            ns3::Ptr<ns3::LtePdcp> pdcp = pdcpManager.GetPdcp(ueId);
            ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>(data.data(), data.length());

            // Add Don't Fragment tag
            ns3::SocketSetDontFragmentTag dfTag;
            dfTag.Enable();
            packet->AddPacketTag(dfTag);

            std::cout << "UERANSIM handleDownlinkRrcDelivery pdcp.TransmitPacket(packet);" << std::endl;

            std::cout <<"// Prepare parameters for PDCP transmission"<< std::endl;
            ns3::LtePdcpSapProvider::TransmitPdcpSduParameters params;
            params.pdcpSdu = packet;
            std::cout << "// Use LtePdcpSapProvider to transmit packet"<< std::endl;
            std::cout << " UERANSIM RRC pdcpManager.GetPdcp(ueId)->GetLtePdcpSapProvider() = " << pdcpManager.GetPdcp(ueId)->GetLtePdcpSapProvider()<< std::endl;

            //pdcpManager.GetPdcp(ueId)->GetLtePdcpSapProvider()->TransmitPdcpSdu(params);

            //pdcp.TransmitPacket(packet);

            //pdcp->TransmitPacket GetLtePdcpSapProvider()->TransmitPdcpSdu(params);
            //pdcpUERANSIM->GetLtePdcpSapProvider()->TransmitPdcpSdu(params);
            std::cout << "FINISH UERANSIM SAP  TransmitPacket FROM RRC UERANSIM" << std::endl;
        } else {
            std::cout << "Failed to find PDCP instance for UE ID: " << ueId << std::endl;
        }

        /////// 14/10/2024
/*
        // Assuming ueInfoMap is obtained via Pdcp instance:
        auto& ueInfoMap = ns3::Pdcp::GetInstance().GetUeInfoMap();

        // Check if `ueId` is already present, otherwise create a new entry
        if (ueInfoMap.find(ueId) == ueInfoMap.end())
        {
            ueInfoMap[ueId] = ns3::UeInfo();  // Create an empty UeInfo entry for the `ueId`
            ns3::UeInfo ueInfo;
            ueInfoMap[ueId] = std::move(ueInfo);
        }

        // Set or update the required information for the UE
        ns3::UeInfo& ueInfo = ueInfoMap[ueId];
*/


    ////  ////
    // Create and populate the RlsPduTransmission message
    rls::RlsPduTransmission msg{m_sti};
    msg.pduType = rls::EPduType::RRC;
    msg.pdu = std::move(data);
    msg.payload = static_cast<uint32_t>(channel);
    msg.pduId = pduId;

    // Print the RlsPduTransmission information
    //std::cout << "____RlsPduTransmission Info:" << std::endl;
    //std::cout << "____STI: " << msg.sti << std::endl;
    //std::cout << "____PDU Type: " << static_cast<int>(msg.pduType) << std::endl;
    //std::cout << "____PDU Size: " << msg.pdu.length() << std::endl;
    //std::cout << "____Payload: " << msg.payload << std::endl;
    //std::cout << "____PDU ID: " << msg.pduId << std::endl;
    // OMAR
/*
    // Check if UE is attached
    if (m_ueAttachmentState.find(ueId) != m_ueAttachmentState.end() && m_ueAttachmentState[ueId])
    {
    if (msg.pdu.length() > 0)
    {
        auto it = m_pdcpMap.find(ueId);
        if (it == m_pdcpMap.end())
        {
            // Create a new PDCP instance for this UE
            Pdcp newPdcp;

            m_pdcpMap[ueId] = newPdcp;
            it = m_pdcpMap.find(ueId);
        }

        if (it != m_pdcpMap.end())
        {
            auto& pdcp = it->second;
            ns3::Ptr<ns3::Packet> packet =
                ns3::Create<ns3::Packet>(msg.pdu.data(), msg.pdu.length());

            // Add Don't Fragment tag
            ns3::SocketSetDontFragmentTag dfTag;
            dfTag.Enable();
            packet->AddPacketTag(dfTag);

            std::cout << " SAP RRC UERANSIM pdcp.TransmitPacket(packet);" << std::endl;
            pdcp.TransmitPacket(packet);
            std::cout << "9999 FINISH UERANSIM SAP  TransmitPacket FROM RRC UERANSIM" << std::endl;
        }
        else
        {
            m_logger->err("PDCP instance not available for UE: %d", ueId);
        }
    }
}
*/


    std::cout << "handleDownlinkDataDelivery PDU Size: "  << msg.pdu.length() << "   - ueId" << ueId<< std::endl;


    m_udpTask->send(ueId, msg);  //RLS
}

/*
void RlsControlTask::handleDownlinkDataDelivery(int ueId, int psi, OctetString &&data)
{
    rls::RlsPduTransmission msg{m_sti};
    msg.pduType = rls::EPduType::DATA;
    msg.pdu = std::move(data);
    std::cout << "handleDownlinkDataDelivery PDU Size: " << msg.pdu.length() << std::endl;
    msg.payload = static_cast<uint32_t>(psi);
    msg.pduId = 0;

    m_udpTask->send(ueId, msg);
}
 */
void RlsControlTask::handleDownlinkDataDelivery(int ueId, int psi, OctetString &&data)
{
    ////
    // PDCP Layer integration: Transmit PDCP SDU
    std::cout << " ___UserPlane___handleDownlinkDataDelivery PDU Size: " << data.length() << std::endl;
    std::cout << "___UserPlane___handleDownlinkDataDelivery UE ID: " << ueId << std::endl;

    if (data.length() > 0 ) //&& m_ueAttachmentState[ueId]
    {
        /*
        auto it = m_pdcpMap.find(ueId);
        if (it == m_pdcpMap.end())
        {
            std::cout << "___UserPlane___Create a new PDCP instance for this UE: " << ueId << std::endl;
            //ns3::Pdcp newPdcp;
            m_pdcpMap[ueId] = m_pdcp;
            it = m_pdcpMap.find(ueId);
        }

        if (it != m_pdcpMap.end())
        {
            std::cout << "___UserPlane___DATA TO PDCP DOWNLINK for this UE: " << it->first << std::endl;
            auto& pdcp = it->second;
            ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>(data.data(), data.length());

            // Add Don't Fragment tag
            ns3::SocketSetDontFragmentTag dfTag;
            dfTag.Enable();
            packet->AddPacketTag(dfTag);

            std::cout << "___UserPlane___UERANSIM pdcp.TransmitPacket(packet);" << std::endl;
            //pdcp.TransmitPacket(packet);
        }
        else
        {
            m_logger->err("___UserPlane___PDCP instance not available for UE: %d", ueId);
        }
         */
    }

    ////////
    // Create and populate the RlsPduTransmission message
    rls::RlsPduTransmission msg{m_sti};
    msg.pduType = rls::EPduType::DATA;
    msg.pdu = std::move(data);
    std::cout << "___UserPlane___handleDownlinkDataDelivery PDU Size: " << msg.pdu.length() << std::endl;
    msg.payload = static_cast<uint32_t>(psi);
    msg.pduId = 0;

    m_udpTask->send(ueId, msg);  // RLS
}

void RlsControlTask::onAckControlTimerExpired()
{

    int64_t current = utils::CurrentTimeMillis();

    std::vector<uint32_t> transmissionFailureIds;
    std::vector<rls::PduInfo> transmissionFailures;

    for (auto &pdu : m_pduMap)
    {
        auto delta = current - pdu.second.sentTime;
        if (delta > MAX_PDU_TTL)
        {
            transmissionFailureIds.push_back(pdu.first);
            transmissionFailures.push_back(std::move(pdu.second));
        }
    }

    for (auto id : transmissionFailureIds)
        m_pduMap.erase(id);

    if (!transmissionFailures.empty())
    {
        auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::TRANSMISSION_FAILURE);
        w->pduList = std::move(transmissionFailures);
        m_mainTask->push(std::move(w));
    }

}

void RlsControlTask::onAckSendTimerExpired()
{

    auto copy = m_pendingAck;
    m_pendingAck.clear();

    for (auto &item : copy)
    {
        if (!item.second.empty())
            continue;

        rls::RlsPduTransmissionAck msg{m_sti};
        msg.pduIds = std::move(item.second);

        m_udpTask->send(item.first, msg); //RLS
    }

}



} // namespace nr::gnb








































// version using semaphores commented for possible re-usage later if need be.
/*
#include "ctl_task.hpp"
#include <iostream>

#include <stdexcept>
#include <ns3/common.hpp>
//---------------------------added
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstdlib> // For the system function
//----------------------
static constexpr const size_t MAX_PDU_COUNT = 4096;
static constexpr const int MAX_PDU_TTL = 3000;

static constexpr const int TIMER_ID_ACK_CONTROL = 1;
static constexpr const int TIMER_ID_ACK_SEND = 2;

static constexpr const int TIMER_PERIOD_ACK_CONTROL = 1500;
static constexpr const int TIMER_PERIOD_ACK_SEND = 2250;

namespace nr::gnb
{
//--------------------------- added test code

//----------------------------------------------------
RlsControlTask::RlsControlTask(TaskBase *base, uint64_t sti)
    : m_sti{sti}, m_mainTask{}, m_udpTask{}, m_pduMap{}, m_pendingAck{}
{
    m_logger = base->logBase->makeUniqueLogger("rls-ctl");
}

void RlsControlTask::initialize(NtsTask *mainTask, RlsUdpTask *udpTask)
{
    m_mainTask = mainTask;
    m_udpTask = udpTask;
}

void RlsControlTask::onStart()
{
    setTimer(TIMER_ID_ACK_CONTROL, TIMER_PERIOD_ACK_CONTROL);
    setTimer(TIMER_ID_ACK_SEND, TIMER_PERIOD_ACK_SEND);
}

void RlsControlTask::onLoop()
{
    auto msg = take();
    if (!msg)
        return;

    switch (msg->msgType)
    {
    case NtsMessageType::GNB_RLS_TO_RLS: {
        auto &w = dynamic_cast<NmGnbRlsToRls &>(*msg);
        switch (w.present)
        {
        case NmGnbRlsToRls::SIGNAL_DETECTED:
            handleSignalDetected(w.ueId);
            break;
        case NmGnbRlsToRls::SIGNAL_LOST:
            handleSignalLost(w.ueId);
            break;
        case NmGnbRlsToRls::RECEIVE_RLS_MESSAGE:
            handleRlsMessage(w.ueId, *w.msg);
            break;
        case NmGnbRlsToRls::DOWNLINK_DATA:
            handleDownlinkDataDelivery(w.ueId, w.psi, std::move(w.data));
            break;
        case NmGnbRlsToRls::DOWNLINK_RRC:
            handleDownlinkRrcDelivery(w.ueId, w.pduId, w.rrcChannel, std::move(w.data));
            break;
        default:
            m_logger->unhandledNts(*msg);
            break;
        }
        break;
    }
    case NtsMessageType::TIMER_EXPIRED: {
        auto &w = dynamic_cast<NmTimerExpired &>(*msg);
        if (w.timerId == TIMER_ID_ACK_CONTROL)
        {
            setTimer(TIMER_ID_ACK_CONTROL, TIMER_PERIOD_ACK_CONTROL);
            onAckControlTimerExpired();
        }
        else if (w.timerId == TIMER_ID_ACK_SEND)
        {
            setTimer(TIMER_ID_ACK_SEND, TIMER_PERIOD_ACK_SEND);
            onAckSendTimerExpired();
        }
        break;
    }
    default:
        m_logger->unhandledNts(*msg);
        break;
    }
}

void RlsControlTask::onQuit()
{

}

void RlsControlTask::handleSignalDetected(int ueId)
{
    auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::SIGNAL_DETECTED);
    w->ueId = ueId;
    m_mainTask->push(std::move(w));
}

void RlsControlTask::handleSignalLost(int ueId)
{
    auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::SIGNAL_LOST);
    w->ueId = ueId;
    m_mainTask->push(std::move(w));
}

void RlsControlTask::handleRlsMessage(int ueId, rls::RlsMessage &msg)
{
    if (msg.msgType == rls::EMessageType::PDU_TRANSMISSION_ACK)
    {
        auto &m = (rls::RlsPduTransmissionAck &)msg;
        for (auto pduId : m.pduIds)
            m_pduMap.erase(pduId);
    }
    else if (msg.msgType == rls::EMessageType::PDU_TRANSMISSION)
    {
        auto &m = (rls::RlsPduTransmission &)msg;
        if (m.pduId != 0)
            m_pendingAck[ueId].push_back(m.pduId);

        if (m.pduType == rls::EPduType::DATA)
        {
            auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::UPLINK_DATA);
            w->ueId = ueId;
            w->psi = static_cast<int>(m.payload);
            w->data = std::move(m.pdu);
            m_mainTask->push(std::move(w));
        }
        else if (m.pduType == rls::EPduType::RRC)
        {
            auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::UPLINK_RRC);
            w->ueId = ueId;
            w->rrcChannel = static_cast<rrc::RrcChannel>(m.payload);
            w->data = std::move(m.pdu);
            m_mainTask->push(std::move(w));
        }
        else
        {
            m_logger->err("Unhandled RLS PDU type");
        }
    }
    else
    {
        m_logger->err("Unhandled RLS message type");
    }
}

void RlsControlTask::handleDownlinkRrcDelivery(int ueId, uint32_t pduId, rrc::RrcChannel channel, OctetString &&data)
{
	//RrcPduData testPacket = CreateTestPacket(); //acceessed here mainly to test 
	//sendToNs3Pdcp(testPacket); //uncomment to use in downlink sense
    if (ueId == 0 && pduId != 0)
    {
        // PDU ID must be not set in case of broadcast
        throw std::runtime_error("");
    }

    if (pduId != 0)
    {
        if (m_pduMap.count(pduId))
        {
            m_pduMap.clear();

            auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::RADIO_LINK_FAILURE);
            w->rlfCause = rls::ERlfCause::PDU_ID_EXISTS;
            m_mainTask->push(std::move(w));
            return;
        }

        if (m_pduMap.size() > MAX_PDU_COUNT)
        {
            m_pduMap.clear();

            auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::RADIO_LINK_FAILURE);
            w->rlfCause = rls::ERlfCause::PDU_ID_FULL;
            m_mainTask->push(std::move(w));
            return;
        }

        m_pduMap[pduId].endPointId = ueId;
        m_pduMap[pduId].id = pduId;
        m_pduMap[pduId].pdu = data.copy();
        m_pduMap[pduId].rrcChannel = channel;
        m_pduMap[pduId].sentTime = utils::CurrentTimeMillis();
    }

    rls::RlsPduTransmission msg{m_sti};
    msg.pduType = rls::EPduType::RRC;
    msg.pdu = std::move(data);
    msg.payload = static_cast<uint32_t>(channel);
    msg.pduId = pduId;

    m_udpTask->send(ueId, msg);
}

void RlsControlTask::handleDownlinkDataDelivery(int ueId, int psi, OctetString &&data)
{
	//RrcPduData testPacket = CreateTestPacket();
	//sendToNs3Pdcp(testPacket);
    rls::RlsPduTransmission msg{m_sti};
    msg.pduType = rls::EPduType::DATA;
    msg.pdu = std::move(data);
    msg.payload = static_cast<uint32_t>(psi);
    msg.pduId = 0;

    m_udpTask->send(ueId, msg);
}

void RlsControlTask::onAckControlTimerExpired()
{
    int64_t current = utils::CurrentTimeMillis();

    std::vector<uint32_t> transmissionFailureIds;
    std::vector<rls::PduInfo> transmissionFailures;

    for (auto &pdu : m_pduMap)
    {
        auto delta = current - pdu.second.sentTime;
        if (delta > MAX_PDU_TTL)
        {
            transmissionFailureIds.push_back(pdu.first);
            transmissionFailures.push_back(std::move(pdu.second));
        }
    }

    for (auto id : transmissionFailureIds)
        m_pduMap.erase(id);

    if (!transmissionFailures.empty())
    {
        auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::TRANSMISSION_FAILURE);
        w->pduList = std::move(transmissionFailures);
        m_mainTask->push(std::move(w));
    }
}

void RlsControlTask::onAckSendTimerExpired()
{
    auto copy = m_pendingAck;
    m_pendingAck.clear();

    for (auto &item : copy)
    {
        if (!item.second.empty())
            continue;

        rls::RlsPduTransmissionAck msg{m_sti};
        msg.pduIds = std::move(item.second);

        m_udpTask->send(item.first, msg);
    }
}

//----------------------------- added code

/*
void RlsControlTask::sendToNs3Pdcp(RrcPduData packet) {
    // Acquire the semaphore
     sem_t *semaphore_dl = sem_open("/my_semaphore_dl", O_CREAT, 0666, 1);
    if (semaphore_dl == SEM_FAILED) {
        return;
    }
   // m_logger->info("Semaphore opened successfully");
    // Release if previously blocked
    //sem_close(semaphore);

    RrcPduData pdcpSduData = packet;


    int shmFd_dl = shm_open("/my_shared_memory_dl", O_RDWR | O_CREAT, 0666);
    if (shmFd_dl == -1) {
        sem_post(semaphore_dl); // Release the semaphore in case of error
        sem_close(semaphore_dl);
        return;
    }
    //    m_logger->info("Openned shared memory");

    if (ftruncate(shmFd_dl, sizeof(RrcPduData)) == -1) {
        sem_post(semaphore_dl); // Release the semaphore in case of error
        close(shmFd_dl);
        sem_close(semaphore_dl);
        return;
    }
     //  m_logger->info("Posted Semaphore");

    RrcPduData *sharedPdu = (RrcPduData*)mmap(0, sizeof(RrcPduData), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd_dl, 0);
    if (sharedPdu == MAP_FAILED) {
        sem_post(semaphore_dl); // Release the semaphore in case of error
        close(shmFd_dl);
        sem_close(semaphore_dl);
        return;
    }
      // m_logger->info("Created SharedPdu");
       
    *sharedPdu = pdcpSduData;
     
    munmap(sharedPdu, sizeof(RrcPduData));
    close(shmFd_dl);
    
    sem_post(semaphore_dl);
    sem_close(semaphore_dl);
  //  m_logger->info("Semaphore closed successfully");
   // const char* command = "./ns3 run cttc-3gpp-channel-simple-fdm"; // used for testing
   // int result = system(command);
         

}

//------------------------ other direction
// in UE CTL task
} 
*/
// namespace nr::gnb
