//
// This file is a part of UERANSIM project.
// Copyright (c) 2023 ALİ GÜNGÖR.
//
// https://github.com/aligungr/UERANSIM/
// See README, LICENSE, and CONTRIBUTING files for licensing details.
//

#include "udp_task.hpp"

#include <cstdint>
#include <cstring>
#include <set>

#include "../../../ns-3-dev/contrib/NGAP/src/ue/nts.hpp" 
#include <ns3/common.hpp>
#include <ns3/constants.hpp>
//-- ns3::socket
#include "ns3/udp-socket-factory.h"
#include <ns3/core-module.h>
#include <ns3/internet-module.h>
#include <ns3/network-module.h>
#include <ns3/packet.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include "ns3/pdcp.h"


static constexpr const int BUFFER_SIZE = 16384;
static constexpr const int LOOP_PERIOD = 1000;
static constexpr const int RECEIVE_TIMEOUT = 200;
static constexpr const int HEARTBEAT_THRESHOLD = 2000; // (LOOP_PERIOD + RECEIVE_TIMEOUT)'dan büyük olmalı

namespace nr::ue
{

ns3::Pdcp& pdcpManager = ns3::Pdcp::GetInstance();

//OMAR
std::string inetAddressToString(const InetAddress& addr) {
    char addrStr[INET6_ADDRSTRLEN];
    if (addr.getIpVersion() == 4) {
        inet_ntop(AF_INET, &(((sockaddr_in*)&addr.storage)->sin_addr), addrStr, INET_ADDRSTRLEN);
    } else {
        inet_ntop(AF_INET6, &(((sockaddr_in6*)&addr.storage)->sin6_addr), addrStr, INET6_ADDRSTRLEN);
    }

    return std::string(addrStr) + ":" + std::to_string(addr.getPort());
}
//OMAR

RlsUdpTask::RlsUdpTask(TaskBase *base, RlsSharedContext *shCtx, const std::vector<std::string> &searchSpace)
    : m_server{}, m_ctlTask{}, m_shCtx{shCtx}, m_searchSpace{}, m_cells{}, m_cellIdToSti{}, m_lastLoop{},
      m_cellIdCounter{}
{
    m_logger = base->logBase->makeUniqueLogger(base->config->getLoggerPrefix() + "rls-udp");

    m_server = new udp::UdpServer();

    for (auto &ip : searchSpace)
        m_searchSpace.emplace_back(ip, cons::RadioLinkPort);

    m_simPos = Vector3{};
}

void RlsUdpTask::onStart()
{
}

//ORIGINAL
void RlsUdpTask::onLoop()
{
    auto current = utils::CurrentTimeMillis();
    if (current - m_lastLoop > LOOP_PERIOD)
    {
        m_lastLoop = current;
        heartbeatCycle(current, m_simPos);
    }

    uint8_t buffer[BUFFER_SIZE];
    InetAddress peerAddress;

    int size = m_server->Receive(buffer, BUFFER_SIZE, RECEIVE_TIMEOUT, peerAddress);
    if (size > 0)
    {
        auto rlsMsg = rls::DecodeRlsMessage(OctetView{buffer, static_cast<size_t>(size)});
        if (rlsMsg == nullptr)
            m_logger->err("Unable to decode RLS message");
        else
            receiveRlsPdu(peerAddress, std::move(rlsMsg));
    }
}

//GET&SET
/*
void RlsUdpTask::onLoop()
{
    auto current = utils::CurrentTimeMillis();
    if (current - m_lastLoop > LOOP_PERIOD)
    {
        m_lastLoop = current;
        heartbeatCycle(current, m_simPos);
    }

    uint8_t buffer[BUFFER_SIZE];
    InetAddress peerAddress;

    size_t length = pdcpManager.GetStreamLength();  // Get the global stream length

    //int size = m_server->Receive(buffer, BUFFER_SIZE, RECEIVE_TIMEOUT, peerAddress);

    const uint8_t* streamData;

    if (pdcpManager.GetStreamData() != nullptr && length > 0) {
        streamData = pdcpManager.GetStreamData(); // Get the global stream data
        //std::cout << "******* GNB ORIGINAL ONLOOP------- GetStreamData() !!!!!!!!!!!!!!!!!!!!!!: " << streamData << std::endl;
    }



    //if (size > 0)
    if (length > 0)
    {

        if (streamData != nullptr)
        {
            //auto rlsMsg = rls::DecodeRlsMessage(OctetView{buffer, static_cast<size_t>(size)});
            auto rlsMsg = rls::DecodeRlsMessage(OctetView{streamData, static_cast<size_t>(length)});
            if (rlsMsg == nullptr)
                m_logger->err("Unable to decode RLS message UE");
            else{
                //std::cout << " ---UE ON LOOP------BEFORE -----receiveRlsPdu-- " <<  std::endl;
                peerAddress=pdcpManager.GetAddress();
                receiveRlsPdu(peerAddress, std::move(rlsMsg));
                //std::cout << " ---UE ON LOOP-------AFTER ----receiveRlsPdu---" <<  std::endl;
              }
            }
    }
}
*/

/* //map
void RlsUdpTask::onLoop()
{
    auto current = utils::CurrentTimeMillis();

    // Check if it's time to run the heartbeat cycle
    if (current - m_lastLoop > LOOP_PERIOD)
    {
        m_lastLoop = current;
        heartbeatCycle(current, m_simPos);  // Heartbeat cycle logic remains unchanged
    }

    uint8_t buffer[BUFFER_SIZE];
    InetAddress peerAddress;  // Address of the peer we are "receiving" from

    // Convert InetAddress to string to use it as a key for message retrieval
    std::string key = inetAddressToString(peerAddress);

    // Check if we have a message stored in the map for this peer address
    if (messageMap.find(key) != messageMap.end()) {
        std::vector<uint8_t>& storedMessage = messageMap[key];

        // Simulate receiving the message into the buffer
        int size = static_cast<int>(storedMessage.size());
        std::cout << "--------------NEW------------------UE UDP TASK.CPP size = m_server->Receive storedMessage.size(): " << size << std::endl;

        if (size > BUFFER_SIZE) {
            m_logger->err("Message size exceeds buffer size.");
            return;
        }

        std::copy(storedMessage.begin(), storedMessage.end(), buffer);

        std::cout << "Message retrieved from internal map for address: " << key << std::endl;

        // Decode the RLS message
        auto rlsMsg = rls::DecodeRlsMessage(OctetView{buffer, static_cast<size_t>(size)});

        if (rlsMsg == nullptr) {
            m_logger->err("Unable to decode RLS message UE");
        } else {
            receiveRlsPdu(peerAddress, std::move(rlsMsg));  // Process the message
        }

        // Optionally, remove the message after processing
        // messageMap.erase(key);
    }
}*/

void RlsUdpTask::onQuit()
{
    delete m_server;
}

//ORIGINAL
void RlsUdpTask::sendRlsPdu(const InetAddress &addr, const rls::RlsMessage &msg)
{
    OctetString stream;
    rls::EncodeRlsMessage(msg, stream);

    m_server->Send(addr, stream.data(), static_cast<size_t>(stream.length()));
}

//Set&Get
/*
void RlsUdpTask::sendRlsPdu(const InetAddress &addr, const rls::RlsMessage &msg)
{
    OctetString stream;
    rls::EncodeRlsMessage(msg, stream);

    //m_server->Send(addr, stream.data(), static_cast<size_t>(stream.length()));

    //OMAR
    // Use the converted string as the key
    std::string key = inetAddressToString(addr);
    std::cout << "UE UDP_TASK.CPP MAP sendRlsPDU Key: " << key << std::endl;
    std::cout << "UE UDP_TASK.CPP MAP sendRlsPDU stream.length() //////// : " << stream.length() << std::endl;

    //std::vector<uint8_t> message(stream.data(), stream.data() + stream.length());
    std::vector<uint8_t> message;
    message.push_back(static_cast<uint8_t>(stream.length()));  // Store length as first byte
    message.insert(message.end(), stream.data(), stream.data() + stream.length()); // Append actual data

    pdcpManager.SetMessageMap(key, message);
    //messageMap[key] = message;
    pdcpManager.SetStreamLength(stream.length());  // Store the stream length globally
    std::cout << " UE Set stream length in PdcpManager: " << stream.length() << std::endl;

    //std::cout << "UE onloop() Message map size: " << pdcpManager.GetmessageMapSize() << std::endl;
    // OMAR

    if (stream.data() != nullptr)
    {//
        //pdcpManager.SetStreamData(static_cast<uint8_t*>(stream.data()));
        pdcpManager.SetStreamData(stream.data(), stream.length());  // Store data safely
        pdcpManager.SetAddress(addr);

        std::cout << "******** UE UDP TASK.CPP m_server->Send UE BUFFER stream.data(): " << stream.data() << "    " << addr.getSockAddr() << std::endl;
    }

    //m_server->Send(addr, stream.data(), static_cast<size_t>(stream.length()));
}
*/
void RlsUdpTask::send(int cellId, const rls::RlsMessage &msg)
{
    if (m_cellIdToSti.count(cellId))
    {
        auto sti = m_cellIdToSti[cellId];
        sendRlsPdu(m_cells[sti].address, msg);
    }
}

void RlsUdpTask::receiveRlsPdu(const InetAddress &addr, std::unique_ptr<rls::RlsMessage> &&msg)
{
    if (msg->msgType == rls::EMessageType::HEARTBEAT_ACK)
    {
        if (!m_cells.count(msg->sti))
        {
            m_cells[msg->sti].cellId = ++m_cellIdCounter;
            m_cellIdToSti[m_cells[msg->sti].cellId] = msg->sti;
        }

        int oldDbm = INT32_MIN;
        if (m_cells.count(msg->sti))
            oldDbm = m_cells[msg->sti].dbm;

        m_cells[msg->sti].address = addr;
        m_cells[msg->sti].lastSeen = utils::CurrentTimeMillis();

        int newDbm = ((const rls::RlsHeartBeatAck &)*msg).dbm;
        m_cells[msg->sti].dbm = newDbm;

        if (oldDbm != newDbm)
            onSignalChangeOrLost(m_cells[msg->sti].cellId);
        return;
    }

    if (!m_cells.count(msg->sti))
    {
        // if no HB-ACK received yet, and the message is not HB-ACK, then ignore the message
        return;
    }

    auto w = std::make_unique<NmUeRlsToRls>(NmUeRlsToRls::RECEIVE_RLS_MESSAGE);
    w->cellId = m_cells[msg->sti].cellId;
    w->msg = std::move(msg);
    m_ctlTask->push(std::move(w));
}



void RlsUdpTask::onSignalChangeOrLost(int cellId)
{
    int dbm = INT32_MIN;
    if (m_cellIdToSti.count(cellId))
    {
        auto sti = m_cellIdToSti[cellId];
        dbm = m_cells[sti].dbm;
    }

    auto w = std::make_unique<NmUeRlsToRls>(NmUeRlsToRls::SIGNAL_CHANGED);
    w->cellId = cellId;
    w->dbm = dbm;
    m_ctlTask->push(std::move(w));
}

void RlsUdpTask::heartbeatCycle(uint64_t time, const Vector3 &simPos)
{
    std::set<std::pair<uint64_t, int>> toRemove;

    for (auto &cell : m_cells)
    {
        auto delta = time - cell.second.lastSeen;
        if (delta > HEARTBEAT_THRESHOLD)
            toRemove.insert({cell.first, cell.second.cellId});
    }

    for (auto cell : toRemove)
    {
        m_cells.erase(cell.first);
        m_cellIdToSti.erase(cell.second);
    }

    for (auto cell : toRemove)
        onSignalChangeOrLost(cell.second);

    for (auto &addr : m_searchSpace)
    {
        rls::RlsHeartBeat msg{m_shCtx->sti};
        msg.simPos = simPos;
        sendRlsPdu(addr, msg);
    }
}

void RlsUdpTask::initialize(NtsTask *ctlTask)
{
    m_ctlTask = ctlTask;
}

} // namespace nr::ue
