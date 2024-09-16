//
// This file is a part of UERANSIM project.
// Copyright (c) 2023 ALİ GÜNGÖR.
//
// https://github.com/aligungr/UERANSIM/
// See README, LICENSE, and CONTRIBUTING files for licensing details.
//

#include "ctl_task.hpp"
#include <iostream>
// Include necessary headers for socket communication
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "ns3/socket.h"
#include <ns3/common.hpp>

static constexpr const size_t MAX_PDU_COUNT = 128;
static constexpr const int MAX_PDU_TTL = 3000;

static constexpr const int TIMER_ID_ACK_CONTROL = 1;
static constexpr const int TIMER_ID_ACK_SEND = 2;

static constexpr const int TIMER_PERIOD_ACK_CONTROL = 1500;
static constexpr const int TIMER_PERIOD_ACK_SEND = 2250;

namespace nr::ue
{

RlsControlTask::RlsControlTask(TaskBase *base, RlsSharedContext *shCtx)
    : m_shCtx{shCtx}, m_servingCell{}, m_mainTask{}, m_udpTask{}, m_pduMap{}, m_pendingAck{}
{
    m_logger = base->logBase->makeUniqueLogger(base->config->getLoggerPrefix() + "rls-ctl");
}

void RlsControlTask::initialize(NtsTask *mainTask, RlsUdpTask *udpTask)
{
    m_mainTask = mainTask;
    m_udpTask = udpTask;

    //std::cout << "gnbDevs======" << gnbDevs.GetN() << std::endl;

    /////////////////////////////////////
    /*
      // Simulation parameters
      uint16_t numerologyBwp = 4;
      double centralFrequency = 3.5e9;
      double bandwidth = 50e6;
      uint32_t udpPacketSize = 100;
      double totalTxPower = 23; // dBm for UEs

      // Create NR Helpers
      ns3::Ptr<ns3::NrPointToPointEpcHelper> epcHelper = ns3::CreateObject<ns3::NrPointToPointEpcHelper>();
      ns3::Ptr<ns3::IdealBeamformingHelper> idealBeamformingHelper = ns3::CreateObject<ns3::IdealBeamformingHelper>();

      nrHelper = ns3::CreateObject<ns3::NrHelper>();


      // Set EPC and Beamforming Helpers
      nrHelper->SetEpcHelper(epcHelper);
      nrHelper->SetBeamformingHelper(idealBeamformingHelper);

      // Configure Mobility Model for UE
      ns3::NodeContainer ueNodes;
      ueNodes.Create(1);

      ns3::MobilityHelper mobility;
      mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
      ns3::Ptr<ns3::ListPositionAllocator> positionAlloc = ns3::CreateObject<ns3::ListPositionAllocator>();
      positionAlloc->Add(ns3::Vector(20.0, 0.0, 1.5));  // UE Position
      mobility.SetPositionAllocator(positionAlloc);
      mobility.Install(ueNodes);

      // Set NR Configuration
      ns3::BandwidthPartInfoPtrVector allBwps;
      ns3::CcBwpCreator ccBwpCreator;
      const uint8_t numCcPerBand = 1;

      ns3::CcBwpCreator::SimpleOperationBandConf bandConf(centralFrequency, bandwidth, numCcPerBand, ns3::BandwidthPartInfo::UMi_StreetCanyon);
      ns3::OperationBandInfo band = ccBwpCreator.CreateOperationBandContiguousCc(bandConf);

      nrHelper->InitializeOperationBand(&band);
      allBwps = ns3::CcBwpCreator::GetAllBwps({band});

      // UE Antenna Configuration
      nrHelper->SetUeAntennaAttribute("NumRows", ns3::UintegerValue(2));
      nrHelper->SetUeAntennaAttribute("NumColumns", ns3::UintegerValue(4));
      nrHelper->SetUeAntennaAttribute("AntennaElement", ns3::PointerValue(ns3::CreateObject<ns3::IsotropicAntennaModel>()));
  */
    // Install NR Devices on UEs
    // ns3::NetDeviceContainer ueDevs = nrHelper->InstallUeDevice(ueNodes, allBwps);

    // Assign Streams (if needed)
    //    int64_t randomStream = 1;
    // randomStream += nrHelper->AssignStreams(ueDevs, randomStream);

    /*9
    for (auto it = ueDevs.Begin(); it != ueDevs.End(); ++it)
    {
        ns3::DynamicCast<ns3::NrUeNetDevice>(*it)->UpdateConfig();
    }
    std::cout << "UE Device installed successfully." << std::endl;


    // Setup Internet stack on UEs
    //ns3::InternetStackHelper internet;
    //internet.Install(ueNodes);


    // Setup Routing
    //ns3::Ipv4InterfaceContainer ueIpIface = epcHelper->AssignUeIpv4Address(ns3::NetDeviceContainer(ueDevs));
    //ns3::Ipv4StaticRoutingHelper ipv4RoutingHelper;
    //ns3::Ptr<ns3::Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting(ueNodes.Get(0)->GetObject<ns3::Ipv4>());
    //ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);

    // Logging
    m_logger->info("UE Initialized with position (20, 0, 1.5), frequency: " + std::to_string(centralFrequency) + " Hz, bandwidth: " + std::to_string(bandwidth) + " Hz.");

    // Retrieve the gNB NetDeviceContainer from NetworkResourceManager
    //const ns3::NetDeviceContainer& gnbDevs = gnb::NetworkResourceManager::getInstance().getGnbNetDeviceContainer();
    std::cout << "gnbDevs======" << gnbDevs.GetN() << std::endl;
    //g_ueNetDeviceContainer = ueDevs; // Assign to global container

    //std::cout << "gnbDevs======" << nr::gnb::g_gnbNetDeviceContainer.GetN() << std::endl;
    std::cout << "UEEEDevs======" << g_ueNetDeviceContainer.GetN() << std::endl;

    // Attach UE to the closest gNB using the shared global gnbDevs
    //nrHelper->AttachToClosestEnb(ueDevs, gnbDevs);
*/
    ///////////////////////////////////////////////////////////////////////////
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
    case NtsMessageType::UE_RLS_TO_RLS: {
        auto &w = dynamic_cast<NmUeRlsToRls &>(*msg);
        switch (w.present)
        {
        case NmUeRlsToRls::SIGNAL_CHANGED:
            handleSignalChange(w.cellId, w.dbm);
            break;
        case NmUeRlsToRls::RECEIVE_RLS_MESSAGE:
            handleRlsMessage(w.cellId, *w.msg);
            break;
        case NmUeRlsToRls::UPLINK_DATA:
            handleUplinkDataDelivery(w.psi, std::move(w.data));
            break;
        case NmUeRlsToRls::UPLINK_RRC:
            handleUplinkRrcDelivery(w.cellId, w.pduId, w.rrcChannel, std::move(w.data));
            break;
        case NmUeRlsToRls::ASSIGN_CURRENT_CELL:
            m_servingCell = w.cellId;
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

void RlsControlTask::handleRlsMessage(int cellId, rls::RlsMessage &msg)
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
            m_pendingAck[cellId].push_back(m.pduId);

        if (m.pduType == rls::EPduType::DATA)
        {
            if (cellId != m_servingCell)
            {
                // NOTE: Data packet may be received from a cell other than serving cell
                //  Ignore the packet if this is the case. Other cell can only send RRC, but not DATA
                return;
            }

            auto w = std::make_unique<NmUeRlsToRls>(NmUeRlsToRls::DOWNLINK_DATA);
            w->psi = static_cast<int>(m.payload);
            w->data = std::move(m.pdu);
            m_mainTask->push(std::move(w));
        }
        else if (m.pduType == rls::EPduType::RRC)
        {
            auto w = std::make_unique<NmUeRlsToRls>(NmUeRlsToRls::DOWNLINK_RRC);
            w->cellId = cellId;
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

void RlsControlTask::handleSignalChange(int cellId, int dbm)
{
    auto w = std::make_unique<NmUeRlsToRls>(NmUeRlsToRls::SIGNAL_CHANGED);
    w->cellId = cellId;
    w->dbm = dbm;
    m_mainTask->push(std::move(w));
}

void RlsControlTask::handleUplinkRrcDelivery(int cellId, uint32_t pduId, rrc::RrcChannel channel, OctetString &&data)
{
    if (pduId != 0)
    {
        if (m_pduMap.count(pduId))
        {
            m_pduMap.clear();

            auto w = std::make_unique<NmUeRlsToRls>(NmUeRlsToRls::RADIO_LINK_FAILURE);
            w->rlfCause = rls::ERlfCause::PDU_ID_EXISTS;
            m_mainTask->push(std::move(w));
            return;
        }

        if (m_pduMap.size() > MAX_PDU_COUNT)
        {
            m_pduMap.clear();

            auto w = std::make_unique<NmUeRlsToRls>(NmUeRlsToRls::RADIO_LINK_FAILURE);
            w->rlfCause = rls::ERlfCause::PDU_ID_FULL;
            m_mainTask->push(std::move(w));
            return;
        }

        m_pduMap[pduId].endPointId = cellId;
        m_pduMap[pduId].id = pduId;
        m_pduMap[pduId].pdu = data.copy();
        m_pduMap[pduId].rrcChannel = channel;
        m_pduMap[pduId].sentTime = utils::CurrentTimeMillis();
    }
    ///////////
    // PDCP Layer integration: Transmit PDCP SDU
    std::cout << "UE UPLINK 'ctl_task.cpp' data.length(): " << data.length() << std::endl;
    std::cout << "UE UPLINK 'ctl_task.cpp' Cell ID: " << cellId << std::endl;

    if (data.length() > 0)
    {
        auto it = m_pdcpMap.find(cellId);
        if (it == m_pdcpMap.end())
        {
            std::cout << "UE Create a new PDCP instance for this cell =" << cellId << std::endl;
            ns3::Pdcp newPdcp;
            m_pdcpMap[cellId] = newPdcp;
            it = m_pdcpMap.find(cellId);
        }

        if (it != m_pdcpMap.end())
        {
            std::cout << "UE PDCP to RRC UPLINK for this cell =" << it->first << std::endl;
            auto& pdcp = it->second;
            ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>(data.data(), data.length());

            // Add Don't Fragment tag
            ns3::SocketSetDontFragmentTag dfTag;
            dfTag.Enable();
            packet->AddPacketTag(dfTag);

            std::cout << "UERANSIM pdcp.TransmitPacket(packet);" << std::endl;
            pdcp.TransmitPacket(packet);
            std::cout << " SAP TransmitPacket FROM RRC UERANSIM" << std::endl;
        }
        else
        {
            m_logger->err("PDCP instance not available for cell: %d", cellId);
        }
    }
    ///////////
    rls::RlsPduTransmission msg{m_shCtx->sti};
    msg.pduType = rls::EPduType::RRC;
    msg.pdu = std::move(data);
    msg.payload = static_cast<uint32_t>(channel);
    msg.pduId = pduId;

    m_udpTask->send(cellId, msg);
}
///
void RlsControlTask::handleUplinkDataDelivery(int psi, OctetString &&data)
{
    // PDCP Layer integration: Transmit PDCP SDU
    std::cout << "UE UPLINK 'handleUplinkDataDelivery' data.length(): " << data.length() << std::endl;
    std::cout << "UE UPLINK 'handleUplinkDataDelivery' Serving Cell ID: " << m_servingCell << std::endl;

    if (data.length() > 0)
    {
        auto it = m_pdcpMap.find(m_servingCell);
        if (it == m_pdcpMap.end())
        {
            std::cout << "UE Create a new PDCP instance for the serving cell: " << m_servingCell << std::endl;
            ns3::Pdcp newPdcp;
            m_pdcpMap[m_servingCell] = newPdcp;
            it = m_pdcpMap.find(m_servingCell);
        }

        if (it != m_pdcpMap.end())
        {
            std::cout << "UE PDCP to RLC UPLINK for the serving cell: " << it->first << std::endl;
            auto& pdcp = it->second;
            ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>(data.data(), data.length());

            // Add Don't Fragment tag
            ns3::SocketSetDontFragmentTag dfTag;
            dfTag.Enable();
            packet->AddPacketTag(dfTag);

            std::cout << "UE UPLINK DATA pdcp.TransmitPacket(packet);" << std::endl;
            pdcp.TransmitPacket(packet);
            std::cout << "UE UPLINK DATA TransmitPacket FROM UE UPLINK DATA" << std::endl;
        }
        else
        {
            m_logger->err("PDCP instance not available for the serving cell: %d", m_servingCell);
        }
    }

    // Create and populate the RlsPduTransmission message
    rls::RlsPduTransmission msg{m_shCtx->sti};
    msg.pduType = rls::EPduType::DATA;
    msg.pdu = std::move(data);
    msg.payload = static_cast<uint32_t>(psi);
    msg.pduId = 0;

    m_udpTask->send(m_servingCell, msg);

}
///

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
        auto w = std::make_unique<NmUeRlsToRls>(NmUeRlsToRls::TRANSMISSION_FAILURE);
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

        rls::RlsPduTransmissionAck msg{m_shCtx->sti};
        msg.pduIds = std::move(item.second);

        m_udpTask->send(item.first, msg);
    }
}

/*
void RlsControlTask::ReceivePacketFromUERANSIM() //currently not being used in the context of this code 
{
    uint8_t buffer[4096];  // Adjust size as needed
    socklen_t addrLen = sizeof(m_clientAddr);
    int receivedSize = recvfrom(m_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&m_clientAddr, &addrLen);

    if (receivedSize > 0)
    {
        // Extract rnti, lcid, and packet data from the received message
        uint16_t rnti;
        uint8_t lcid;
        memcpy(&rnti, buffer, sizeof(rnti));
        memcpy(&lcid, buffer + sizeof(rnti), sizeof(lcid));

        uint32_t packetSize = receivedSize - sizeof(rnti) - sizeof(lcid);
        uint8_t *packetData = buffer + sizeof(rnti) + sizeof(lcid);

        // Create ns3::Packet from packet data
        ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>(packetData, packetSize);

        // Placeholder values for cellId, pduId, and channel
        int cellId = 0;  // Adjust accordingly
        uint32_t pduId = 0;  // Adjust accordingly
        rrc::RrcChannel channel;  // Define the appropriate channel

        // 
       // handleUplinkRrcDelivery(cellId, pduId, channel, std::move(packet));
    }
    else
    {
        std::cout<<"failed to receive correctly"<<std::endl;
    }
}


//-------------------- end of changes

//--- added to support ns3 functionality
/*
void PrintPacketInfo(const ns3::LteRlcSapProvider::TransmitPdcpPduParameters& pdcpPduParams) {
    std::cout << "Received PDCP PDU Information:" << std::endl;
    std::cout << "RNTI: " << pdcpPduParams.rnti << std::endl;
    std::cout << "LCID: " << static_cast<int>(pdcpPduParams.lcid) << std::endl;
    std::cout << "Packet Size: " << pdcpPduParams.pdcpPdu->GetSize() << " bytes" << std::endl;

    uint32_t packetSize = pdcpPduParams.pdcpPdu->GetSize();
    uint8_t *buffer = new uint8_t[packetSize];
    pdcpPduParams.pdcpPdu->CopyData(buffer, packetSize);

    std::cout << "Packet Content: ";
    for (uint32_t i = 0; i < packetSize; ++i) {
        std::cout << std::hex << static_cast<int>(buffer[i]) << " ";
    }
    std::cout << std::dec << std::endl;

    delete[] buffer;
}
void RlsControlTask::ReceiveFromNs3Lte(int cellId) {
    sem_t *semaphore = sem_open("/my_semaphore", 0);
    if (semaphore == SEM_FAILED) {
        m_logger->err("Failed to open semaphore: ", strerror(errno));
        return;
    }

    if (sem_wait(semaphore) == -1) {
        m_logger->err("sem_wait failed: ", strerror(errno));
        sem_close(semaphore);
        return;
    }

    int shmFd = shm_open("/my_shared_memory", O_RDONLY, 0644);
    if (shmFd == -1) {
        m_logger->err("Failed to open shared memory: ", strerror(errno));
        sem_post(semaphore);
        sem_close(semaphore);
        return;
    }

    ns3::RrcPduDataSerializable *sharedPdu = (ns3::RrcPduDataSerializable*)mmap(0, sizeof(ns3::RrcPduDataSerializable), PROT_READ, MAP_SHARED, shmFd, 0);
    if (sharedPdu == MAP_FAILED) {
        m_logger->err("Failed to map shared memory: ", strerror(errno));
        sem_post(semaphore);
        close(shmFd);
        sem_close(semaphore);
        return;
    }

    close(shmFd);

    if (sem_post(semaphore) == -1) {
        m_logger->err("sem_post failed: ", strerror(errno));
    }

    sem_close(semaphore);

    // Process the received PDCP PDU
    ns3::LteRlcSapProvider::TransmitPdcpPduParameters pdcpPduParams;
    pdcpPduParams.rnti = sharedPdu->rrcPacket.rnti;
    pdcpPduParams.lcid = sharedPdu->rrcPacket.lcid;
    
    // Create the packet from the serialized data
    ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>(sharedPdu->packetData, sharedPdu->rrcPacket.packetSize);
    pdcpPduParams.pdcpPdu = packet;

    // Now pdcpPduParams contains the reconstructed packet

  //testing print
      // PrintPacketInfo(pdcpPduParams);
     
    // Create an OctetString from the packet data
    OctetString octetString(sharedPdu->packetData, sharedPdu->packetData + sharedPdu->rrcPacket.packetSize);


    // Send the data up the UERANSIM stack
    handleUplinkDataDelivery(cellId, std::move(octetString));
   
    //unmap the memory
    munmap(sharedPdu, sizeof(ns3::RrcPduDataSerializable));


   // m_logger->info("Received and processed the PDCP PDU successfully");
}
*/
} // namespace nr::ue
