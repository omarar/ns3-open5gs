//
// See README, LICENSE, and CONTRIBUTING files for licensing details.
//

#include "task.hpp"

#include <ns3/proto.hpp>
#include "../../../ns-3-dev/contrib/NGAP/src/gnb/rls/task.hpp" 
#include <ns3/constants.hpp>
#include <ns3/libc_error.hpp>
#include <arpa/inet.h>
#include <ns3/ASN_NGAP_QosFlowSetupRequestItem.h>
#include "ns3/fd-net-device-module.h"
#include <cstring>
#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/internet-module.h>
#include <sstream>
#include <iomanip>
std::string getIPAddress(const InetAddress &address) {
    char ipString[INET6_ADDRSTRLEN]; // INET6_ADDRSTRLEN is the maximum length of an IPv6 address string
    memset(ipString, 0, sizeof(ipString)); // Initialize the string buffer

    // Extract the IP address from the InetAddress structure
    const sockaddr *sockAddr = address.getSockAddr();
    if (sockAddr->sa_family == AF_INET) {
        // IPv4 address
        inet_ntop(AF_INET, &((sockaddr_in *)sockAddr)->sin_addr, ipString, INET_ADDRSTRLEN);
    } else if (sockAddr->sa_family == AF_INET6) {
        // IPv6 address
        inet_ntop(AF_INET6, &((sockaddr_in6 *)sockAddr)->sin6_addr, ipString, INET6_ADDRSTRLEN);
    }

    return std::string(ipString);
} 
namespace nr::gnb
{

GtpTask::GtpTask(TaskBase *base)
    : m_base{base}, m_udpServer{}, m_ueContexts{}, m_rateLimiter(std::make_unique<RateLimiter>()), m_pduSessions{},
      m_sessionTree{}
{
    m_logger = m_base->logBase->makeUniqueLogger("gtp");
}
// added functions
// Function to convert hexadecimal representation of IP address to dotted-decimal notation
std::string ConvertHexToIPv4(const std::string &hexAddress) {
    std::stringstream ss;
    ss << std::hex << hexAddress; // Input is in hexadecimal
    unsigned int ip;
    ss >> ip; // Convert hexadecimal to integer
    std::ostringstream result;
    result << ((ip >> 24) & 0xFF) << '.' // Extract and append each octet
           << ((ip >> 16) & 0xFF) << '.'
           << ((ip >> 8) & 0xFF) << '.'
           << (ip & 0xFF);
    return result.str(); // Return dotted-decimal notation
}
void GtpTask::HandleRead(ns3::Ptr<ns3::Socket> socket)
{
    ns3::Ptr<ns3::Packet> packet;
    ns3::Address from;
    while ((packet = socket->RecvFrom(from)))
    {
        ns3::InetSocketAddress address = ns3::InetSocketAddress::ConvertFrom(from);
        uint8_t *buffer = new uint8_t[packet->GetSize()];
        packet->CopyData(buffer, packet->GetSize());

        //-----------------------code just to print to verify
      
      if (packet->GetSize() > 0)
        {
            uint32_t receivedSize = packet->GetSize();
            uint8_t data[receivedSize+1];
            packet->CopyData(data, receivedSize);
            data[receivedSize] = '\0';
            std::cout <<"Received UDP packet, size: " << receivedSize << ", data: "<< data<< std::endl;
            ns3::Icmpv4Header icmpHeader;
            packet->PeekHeader(icmpHeader);
            std::cout << "ICMP type: " << static_cast<uint32_t>(icmpHeader.GetType()) << std::endl;
            std::cout << "ICMP code: " << static_cast<uint32_t>(icmpHeader.GetCode()) << std::endl;
        }
        //----------------------
        OctetView octetView(buffer, packet->GetSize());
        auto gtp = gtp::DecodeGtpMessage(octetView);

        // Process the GTP message as in handleUdpReceive
        switch (gtp->msgType)
        {
        case gtp::GtpMessage::MT_G_PDU: {
            auto sessionInd = m_sessionTree.findByDownTeid(gtp->teid);
            if (sessionInd == 0)
            {
                m_logger->err("TEID %d not found on GTP-U Downlink", gtp->teid);
                return;
            }

            if (m_rateLimiter->allowDownlinkPacket(sessionInd, gtp->payload.length()))
            {
                auto w = std::make_unique<NmGnbGtpToRls>(NmGnbGtpToRls::DATA_PDU_DELIVERY);
                w->ueId = GetUeId(sessionInd);
                w->psi = GetPsi(sessionInd);
                w->pdu = std::move(gtp->payload);
                m_base->rlsTask->push(std::move(w));
            }
            return;
        }
        case gtp::GtpMessage::MT_ECHO_REQUEST: {
            gtp::GtpMessage gtpResponse{};
            gtpResponse.msgType = gtp::GtpMessage::MT_ECHO_RESPONSE;
            gtpResponse.seq = gtp->seq;
            gtpResponse.payload = OctetString::FromOctet2({14, 0});

            OctetString gtpPdu;
            if (gtp::EncodeGtpMessage(gtpResponse, gtpPdu))
            {
                ns3::Ptr<ns3::Packet> responsePacket = ns3::Create<ns3::Packet>((const uint8_t*)gtpPdu.data(), gtpPdu.length());
                m_socket->SendTo(responsePacket, 0, address);
            }
            else
            {
                m_logger->err("Uplink data failure, GTP encoding failed");
            }
            return;
        }
        default: {
            m_logger->err("Unhandled GTP-U message type: %d", gtp->msgType);
            return;
        }
        }
    }
}
// added function
void GtpTask::onStart()
{
    try
    {
		std::string deviceName("enp0s9"); // Replace with the appropriate interface name
        std::string localAddress("192.168.56.108"); // Replace with the appropriate IP address
        std::string localGateway("192.168.56.108");
        uint16_t port = 2152; // GTP-U port
        ns3::Ipv4Address localIp(localAddress.c_str());
        ns3::Ipv4Mask localMask("255.255.255.0");
        ns3::GlobalValue::Bind("SimulatorImplementationType", ns3::StringValue("ns3::RealtimeSimulatorImpl"));
        ns3::GlobalValue::Bind("ChecksumEnabled", ns3::BooleanValue(true));
        ns3::Ptr<ns3::Node> node = ns3::CreateObject<ns3::Node>();
        ns3::FdNetDeviceHelper *helper = nullptr;
        auto raw = new ns3::EmuFdNetDeviceHelper;
        raw->SetDeviceName(deviceName);
        helper = raw;
        ns3::NetDeviceContainer devices = helper->Install(node);
		ns3::Ptr<ns3::NetDevice> device = devices.Get(0);
		device->SetAttribute("Address", ns3::Mac48AddressValue(ns3::Mac48Address("08:00:27:a0:58:ef"))); // Replace with the appropriate MAC address

		ns3::InternetStackHelper internetStackHelper;
		internetStackHelper.Install(node);

		ns3::Ptr<ns3::Ipv4> ipv4 = node->GetObject<ns3::Ipv4>();
		uint32_t interface = ipv4->AddInterface(device);
		ns3::Ipv4InterfaceAddress address = ns3::Ipv4InterfaceAddress(localIp, localMask);
		ipv4->AddAddress(interface, address);
		ipv4->SetMetric(interface, 1);
		ipv4->SetUp(interface);

		ns3::Ptr<ns3::SocketFactory> socketFactory = node->GetObject<ns3::UdpSocketFactory>();
		m_socket = socketFactory->CreateSocket();
		m_socket->Bind(ns3::InetSocketAddress(ns3::Ipv4Address::GetAny(), port));
		m_socket->SetRecvCallback(MakeCallback(&GtpTask::HandleRead, this));
    }
    catch (const std::exception &e)
    {
        m_logger->err("GTP/UDP task could not be created. %s", e.what());
    }
}

void GtpTask::onQuit()
{
    m_udpServer->quit();
    delete m_udpServer;

    m_ueContexts.clear();
}

void GtpTask::onLoop()
{
    auto msg = take();
    if (!msg)
        return;

    switch (msg->msgType)
    {
    case NtsMessageType::GNB_NGAP_TO_GTP: {
        auto &w = dynamic_cast<NmGnbNgapToGtp &>(*msg);
        switch (w.present)
        {
        case NmGnbNgapToGtp::UE_CONTEXT_UPDATE: {
            handleUeContextUpdate(*w.update);
            break;
        }
        case NmGnbNgapToGtp::UE_CONTEXT_RELEASE: {
            handleUeContextDelete(w.ueId);
            break;
        }
        case NmGnbNgapToGtp::SESSION_CREATE: {
            handleSessionCreate(w.resource);
            break;
        }
        case NmGnbNgapToGtp::SESSION_RELEASE: {
            handleSessionRelease(w.ueId, w.psi);
            break;
        }
        }
        break;
    }
    case NtsMessageType::GNB_RLS_TO_GTP: {
        auto &w = dynamic_cast<NmGnbRlsToGtp &>(*msg);
        switch (w.present)
        {
        case NmGnbRlsToGtp::DATA_PDU_DELIVERY: {
            handleUplinkData(w.ueId, w.psi, std::move(w.pdu));
            break;
        }
        }
        break;
    }
    case NtsMessageType::UDP_SERVER_RECEIVE:
        handleUdpReceive(dynamic_cast<udp::NwUdpServerReceive &>(*msg));
        break;
    default:
        m_logger->unhandledNts(*msg);
        break;
    }
}

void GtpTask::handleUeContextUpdate(const GtpUeContextUpdate &msg)
{
    if (!m_ueContexts.count(msg.ueId))
        m_ueContexts[msg.ueId] = std::make_unique<GtpUeContext>(msg.ueId);

    auto &ue = m_ueContexts[msg.ueId];
    ue->ueAmbr = msg.ueAmbr;

    updateAmbrForUe(ue->ueId);
}

void GtpTask::handleSessionCreate(PduSessionResource *session)
{
    if (!m_ueContexts.count(session->ueId))
    {
        m_logger->err("PDU session resource could not be created, UE context with ID[%d] not found", session->ueId);
        return;
    }

    uint64_t sessionInd = MakeSessionResInd(session->ueId, session->psi);
    m_pduSessions[sessionInd] = std::unique_ptr<PduSessionResource>(session);

    m_sessionTree.insert(sessionInd, session->downTunnel.teid);

    updateAmbrForUe(session->ueId);
    updateAmbrForSession(sessionInd);
}

void GtpTask::handleSessionRelease(int ueId, int psi)
{
    if (!m_ueContexts.count(ueId))
    {
        m_logger->err("PDU session resource could not be released, UE context with ID[%d] not found", ueId);
        return;
    }

    uint64_t sessionInd = MakeSessionResInd(ueId, psi);

    // Remove all session information from rate limiter
    m_rateLimiter->updateSessionUplinkLimit(sessionInd, 0);
    m_rateLimiter->updateUeDownlinkLimit(ueId, 0);

    // And remove from PDU session table
    if (m_pduSessions.count(sessionInd))
    {
        uint32_t teid = m_pduSessions[sessionInd]->downTunnel.teid;
        m_pduSessions.erase(sessionInd);

        // And remove from the tree
        m_sessionTree.remove(sessionInd, teid);
    }
}

void GtpTask::handleUeContextDelete(int ueId)
{
    // Find PDU sessions of the UE
    std::vector<uint64_t> sessions{};
    m_sessionTree.enumerateByUe(ueId, sessions);

    for (auto &session : sessions)
    {
        // Remove all session information from rate limiter
        m_rateLimiter->updateSessionUplinkLimit(session, 0);
        m_rateLimiter->updateUeDownlinkLimit(ueId, 0);

        // And remove from PDU session table
        uint32_t teid = m_pduSessions[session]->downTunnel.teid;
        m_pduSessions.erase(session);

        // And remove from the tree
        m_sessionTree.remove(session, teid);
    }

    // Remove all user information from rate limiter
    m_rateLimiter->updateUeUplinkLimit(ueId, 0);
    m_rateLimiter->updateUeDownlinkLimit(ueId, 0);

    // Remove UE context
    m_ueContexts.erase(ueId);
}

void GtpTask::handleUplinkData(int ueId, int psi, OctetString &&pdu)
{
    const uint8_t *data = pdu.data();

    // ignore non IPv4 packets
    if ((data[0] >> 4 & 0xF) != 4)
        return;

    uint64_t sessionInd = MakeSessionResInd(ueId, psi);

    if (!m_pduSessions.count(sessionInd))
    {
        m_logger->err("Uplink data failure, PDU session not found. UE[%d] PSI[%d]", ueId, psi);
        return;
    }

    auto &pduSession = m_pduSessions[sessionInd];

    if (m_rateLimiter->allowUplinkPacket(sessionInd, static_cast<int64_t>(pdu.length())))
    {
        gtp::GtpMessage gtp{};
        gtp.payload = std::move(pdu);
        gtp.msgType = gtp::GtpMessage::MT_G_PDU;
        gtp.teid = pduSession->upTunnel.teid;

        auto ul = std::make_unique<gtp::UlPduSessionInformation>();
        ul->qfi = static_cast<int>(pduSession->qosFlows->list.array[0]->qosFlowIdentifier);

        auto cont = std::make_unique<gtp::PduSessionContainerExtHeader>();
        cont->pduSessionInformation = std::move(ul);
        gtp.extHeaders.push_back(std::move(cont));

        OctetString gtpPdu;
        if (!gtp::EncodeGtpMessage(gtp, gtpPdu)) {
            m_logger->err("Uplink data failure, GTP encoding failed");
        } else {
            std::string hexAddress = pduSession->upTunnel.address.toHexString();
            std::string ipv4Address = ConvertHexToIPv4(hexAddress);
            ns3::InetSocketAddress remote = ns3::InetSocketAddress(ns3::Ipv4Address(ipv4Address.c_str()), cons::GtpPort);
            ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>((const uint8_t *)gtpPdu.data(), gtpPdu.length());
            m_socket->SendTo(packet, 0, remote);
        }
    }
}


void GtpTask::handleUdpReceive(const udp::NwUdpServerReceive &msg)
{
    OctetView buffer{msg.packet};
    auto gtp = gtp::DecodeGtpMessage(buffer);

    switch (gtp->msgType)
    {
    case gtp::GtpMessage::MT_G_PDU: {
        auto sessionInd = m_sessionTree.findByDownTeid(gtp->teid);
        if (sessionInd == 0)
        {
            m_logger->err("TEID %d not found on GTP-U Downlink", gtp->teid);
            return;
        }

        if (m_rateLimiter->allowDownlinkPacket(sessionInd, gtp->payload.length()))
        {
            auto w = std::make_unique<NmGnbGtpToRls>(NmGnbGtpToRls::DATA_PDU_DELIVERY);
            w->ueId = GetUeId(sessionInd);
            w->psi = GetPsi(sessionInd);
            w->pdu = std::move(gtp->payload);
            m_base->rlsTask->push(std::move(w));
        }
        return;
    }
    case gtp::GtpMessage::MT_ECHO_REQUEST: {
        gtp::GtpMessage gtpResponse{};
        gtpResponse.msgType = gtp::GtpMessage::MT_ECHO_RESPONSE;
        gtpResponse.seq = gtp->seq;
        gtpResponse.payload = OctetString::FromOctet2({14, 0});

        OctetString gtpPdu;
        if (gtp::EncodeGtpMessage(gtpResponse, gtpPdu)) {
            std::string ipAddress = getIPAddress(msg.fromAddress);
            ns3::InetSocketAddress remote(ns3::Ipv4Address(ipAddress.c_str()), cons::GtpPort);
            ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>((const uint8_t *)gtpPdu.data(), gtpPdu.length());
            m_socket->SendTo(packet, 0, remote);
        } else {
            m_logger->err("Uplink data failure, GTP encoding failed");
        }
        return;
    }
    default: {
        m_logger->err("Unhandled GTP-U message type: %d", gtp->msgType);
        return;
    }
    }
}

void GtpTask::updateAmbrForUe(int ueId)
{
    if (!m_ueContexts.count(ueId))
        return;

    auto &ue = m_ueContexts[ueId];
    m_rateLimiter->updateUeUplinkLimit(ueId, ue->ueAmbr.ulAmbr);
    m_rateLimiter->updateUeDownlinkLimit(ueId, ue->ueAmbr.dlAmbr);
}

void GtpTask::updateAmbrForSession(uint64_t pduSession)
{
    if (!m_pduSessions.count(pduSession))
        return;

    auto &sess = m_pduSessions[pduSession];
    m_rateLimiter->updateSessionUplinkLimit(pduSession, sess->sessionAmbr.ulAmbr);
    m_rateLimiter->updateSessionDownlinkLimit(pduSession, sess->sessionAmbr.dlAmbr);
}

} // namespace nr::gnb









/* /// old code
#include "task.hpp"

#include <ns3/proto.hpp>
#include "../../../ns-3-dev/contrib/NGAP/src/gnb/rls/task.hpp" 
#include <ns3/constants.hpp>
#include <ns3/libc_error.hpp>

#include <ns3/ASN_NGAP_QosFlowSetupRequestItem.h>

namespace nr::gnb
{

GtpTask::GtpTask(TaskBase *base)
    : m_base{base}, m_udpServer{}, m_ueContexts{}, m_rateLimiter(std::make_unique<RateLimiter>()), m_pduSessions{},
      m_sessionTree{}
{
    m_logger = m_base->logBase->makeUniqueLogger("gtp");
}

void GtpTask::onStart()
{
    try
    {
        m_udpServer = new udp::UdpServerTask(m_base->config->gtpIp, cons::GtpPort, this);
        m_udpServer->start();
    }
    catch (const LibError &e)
    {
        m_logger->err("GTP/UDP task could not be created. %s", e.what());
    }
}

void GtpTask::onQuit()
{
    m_udpServer->quit();
    delete m_udpServer;

    m_ueContexts.clear();
}

void GtpTask::onLoop()
{
    auto msg = take();
    if (!msg)
        return;

    switch (msg->msgType)
    {
    case NtsMessageType::GNB_NGAP_TO_GTP: {
        auto &w = dynamic_cast<NmGnbNgapToGtp &>(*msg);
        switch (w.present)
        {
        case NmGnbNgapToGtp::UE_CONTEXT_UPDATE: {
            handleUeContextUpdate(*w.update);
            break;
        }
        case NmGnbNgapToGtp::UE_CONTEXT_RELEASE: {
            handleUeContextDelete(w.ueId);
            break;
        }
        case NmGnbNgapToGtp::SESSION_CREATE: {
            handleSessionCreate(w.resource);
            break;
        }
        case NmGnbNgapToGtp::SESSION_RELEASE: {
            handleSessionRelease(w.ueId, w.psi);
            break;
        }
        }
        break;
    }
    case NtsMessageType::GNB_RLS_TO_GTP: {
        auto &w = dynamic_cast<NmGnbRlsToGtp &>(*msg);
        switch (w.present)
        {
        case NmGnbRlsToGtp::DATA_PDU_DELIVERY: {
            handleUplinkData(w.ueId, w.psi, std::move(w.pdu));
            break;
        }
        }
        break;
    }
    case NtsMessageType::UDP_SERVER_RECEIVE:
        handleUdpReceive(dynamic_cast<udp::NwUdpServerReceive &>(*msg));
        break;
    default:
        m_logger->unhandledNts(*msg);
        break;
    }
}

void GtpTask::handleUeContextUpdate(const GtpUeContextUpdate &msg)
{
    if (!m_ueContexts.count(msg.ueId))
        m_ueContexts[msg.ueId] = std::make_unique<GtpUeContext>(msg.ueId);

    auto &ue = m_ueContexts[msg.ueId];
    ue->ueAmbr = msg.ueAmbr;

    updateAmbrForUe(ue->ueId);
}

void GtpTask::handleSessionCreate(PduSessionResource *session)
{
    if (!m_ueContexts.count(session->ueId))
    {
        m_logger->err("PDU session resource could not be created, UE context with ID[%d] not found", session->ueId);
        return;
    }

    uint64_t sessionInd = MakeSessionResInd(session->ueId, session->psi);
    m_pduSessions[sessionInd] = std::unique_ptr<PduSessionResource>(session);

    m_sessionTree.insert(sessionInd, session->downTunnel.teid);

    updateAmbrForUe(session->ueId);
    updateAmbrForSession(sessionInd);
}

void GtpTask::handleSessionRelease(int ueId, int psi)
{
    if (!m_ueContexts.count(ueId))
    {
        m_logger->err("PDU session resource could not be released, UE context with ID[%d] not found", ueId);
        return;
    }

    uint64_t sessionInd = MakeSessionResInd(ueId, psi);

    // Remove all session information from rate limiter
    m_rateLimiter->updateSessionUplinkLimit(sessionInd, 0);
    m_rateLimiter->updateUeDownlinkLimit(ueId, 0);

    // And remove from PDU session table
    if (m_pduSessions.count(sessionInd))
    {
        uint32_t teid = m_pduSessions[sessionInd]->downTunnel.teid;
        m_pduSessions.erase(sessionInd);

        // And remove from the tree
        m_sessionTree.remove(sessionInd, teid);
    }
}

void GtpTask::handleUeContextDelete(int ueId)
{
    // Find PDU sessions of the UE
    std::vector<uint64_t> sessions{};
    m_sessionTree.enumerateByUe(ueId, sessions);

    for (auto &session : sessions)
    {
        // Remove all session information from rate limiter
        m_rateLimiter->updateSessionUplinkLimit(session, 0);
        m_rateLimiter->updateUeDownlinkLimit(ueId, 0);

        // And remove from PDU session table
        uint32_t teid = m_pduSessions[session]->downTunnel.teid;
        m_pduSessions.erase(session);

        // And remove from the tree
        m_sessionTree.remove(session, teid);
    }

    // Remove all user information from rate limiter
    m_rateLimiter->updateUeUplinkLimit(ueId, 0);
    m_rateLimiter->updateUeDownlinkLimit(ueId, 0);

    // Remove UE context
    m_ueContexts.erase(ueId);
}

void GtpTask::handleUplinkData(int ueId, int psi, OctetString &&pdu)
{
    const uint8_t *data = pdu.data();

    // ignore non IPv4 packets
    if ((data[0] >> 4 & 0xF) != 4)
        return;

    uint64_t sessionInd = MakeSessionResInd(ueId, psi);

    if (!m_pduSessions.count(sessionInd))
    {
        m_logger->err("Uplink data failure, PDU session not found. UE[%d] PSI[%d]", ueId, psi);
        return;
    }

    auto &pduSession = m_pduSessions[sessionInd];

    if (m_rateLimiter->allowUplinkPacket(sessionInd, static_cast<int64_t>(pdu.length())))
    {
        gtp::GtpMessage gtp{};
        gtp.payload = std::move(pdu);
        gtp.msgType = gtp::GtpMessage::MT_G_PDU;
        gtp.teid = pduSession->upTunnel.teid;

        auto ul = std::make_unique<gtp::UlPduSessionInformation>();
        // TODO: currently using first QSI
        ul->qfi = static_cast<int>(pduSession->qosFlows->list.array[0]->qosFlowIdentifier);

        auto cont = std::make_unique<gtp::PduSessionContainerExtHeader>();
        cont->pduSessionInformation = std::move(ul);
        gtp.extHeaders.push_back(std::move(cont));

        OctetString gtpPdu;
        if (!gtp::EncodeGtpMessage(gtp, gtpPdu))
            m_logger->err("Uplink data failure, GTP encoding failed");
        else
            m_udpServer->send(InetAddress(pduSession->upTunnel.address, cons::GtpPort), gtpPdu);
    }
}

void GtpTask::handleUdpReceive(const udp::NwUdpServerReceive &msg)
{
    OctetView buffer{msg.packet};
    auto gtp = gtp::DecodeGtpMessage(buffer);

    switch (gtp->msgType)
    {
    case gtp::GtpMessage::MT_G_PDU: {
        auto sessionInd = m_sessionTree.findByDownTeid(gtp->teid);
        if (sessionInd == 0)
        {
            m_logger->err("TEID %d not found on GTP-U Downlink", gtp->teid);
            return;
        }

        if (m_rateLimiter->allowDownlinkPacket(sessionInd, gtp->payload.length()))
        {
            auto w = std::make_unique<NmGnbGtpToRls>(NmGnbGtpToRls::DATA_PDU_DELIVERY);
            w->ueId = GetUeId(sessionInd);
            w->psi = GetPsi(sessionInd);
            w->pdu = std::move(gtp->payload);
            m_base->rlsTask->push(std::move(w));
        }
        return;
    }
    case gtp::GtpMessage::MT_ECHO_REQUEST: {
        gtp::GtpMessage gtpResponse{};
        gtpResponse.msgType = gtp::GtpMessage::MT_ECHO_RESPONSE;
        gtpResponse.seq = gtp->seq;
        gtpResponse.payload = OctetString::FromOctet2({14, 0});

        OctetString gtpPdu;
        if (gtp::EncodeGtpMessage(gtpResponse, gtpPdu))
            m_udpServer->send(msg.fromAddress, gtpPdu);
        else
            m_logger->err("Uplink data failure, GTP encoding failed");
        return;
    }
    default: {
        m_logger->err("Unhandled GTP-U message type: %d", gtp->msgType);
        return;
    }
    }
}

void GtpTask::updateAmbrForUe(int ueId)
{
    if (!m_ueContexts.count(ueId))
        return;

    auto &ue = m_ueContexts[ueId];
    m_rateLimiter->updateUeUplinkLimit(ueId, ue->ueAmbr.ulAmbr);
    m_rateLimiter->updateUeDownlinkLimit(ueId, ue->ueAmbr.dlAmbr);
}

void GtpTask::updateAmbrForSession(uint64_t pduSession)
{
    if (!m_pduSessions.count(pduSession))
        return;

    auto &sess = m_pduSessions[pduSession];
    m_rateLimiter->updateSessionUplinkLimit(pduSession, sess->sessionAmbr.ulAmbr);
    m_rateLimiter->updateSessionDownlinkLimit(pduSession, sess->sessionAmbr.dlAmbr);
}

} // namespace nr::gnb
*/
