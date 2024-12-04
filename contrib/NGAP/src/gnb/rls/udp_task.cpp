//
// This file is a part of UERANSIM project.
// Copyright (c) 2023 ALİ GÜNGÖR.
//
// https://github.com/aligungr/UERANSIM/
// See README, LICENSE, and CONTRIBUTING files for licensing details.
//
#include <queue>
#include <mutex>
#include "udp_task.hpp"

#include "../../../ns-3-dev/contrib/NGAP/src/gnb/nts.hpp"

#include <ns3/common.hpp>
#include <ns3/constants.hpp>
#include <ns3/libc_error.hpp>

#include <arpa/inet.h>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <set>
//#include "../../../ns-3-dev/contrib/nr/model/pdcp/pdcp.cpp"
#include <ns3/pdcp.h>


static constexpr const int BUFFER_SIZE = 16384;

static constexpr const int LOOP_PERIOD = 1000;
static constexpr const int RECEIVE_TIMEOUT = 200;
static constexpr const int HEARTBEAT_THRESHOLD = 2000; // (LOOP_PERIOD + RECEIVE_TIMEOUT)'dan büyük olmalı

static constexpr const int MIN_ALLOWED_DBM = -120;

static int EstimateSimulatedDbm(const Vector3 &myPos, const Vector3 &uePos)
{
    int deltaX = myPos.x - uePos.x;
    int deltaY = myPos.y - uePos.y;
    int deltaZ = myPos.z - uePos.z;

    int distance = static_cast<int>(std::sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ));
    if (distance == 0)
        return -1; // 0 may be confusing for people
    return -distance;
}

namespace nr::gnb
{

ns3::Pdcp& pdcpManager = ns3::Pdcp::GetInstance();

//std::unordered_map<int, rls::RlsMessage> m_msgMap;
//std::unordered_map<std::string, std::vector<uint8_t>> messageMap;

std::string inetAddressToString(const InetAddress& addr) {
    char addrStr[INET6_ADDRSTRLEN];
    if (addr.getIpVersion() == 4) {
        inet_ntop(AF_INET, &(((sockaddr_in*)&addr.storage)->sin_addr), addrStr, INET_ADDRSTRLEN);
    } else {
        inet_ntop(AF_INET6, &(((sockaddr_in6*)&addr.storage)->sin6_addr), addrStr, INET6_ADDRSTRLEN);
    }

    return std::string(addrStr) + ":" + std::to_string(addr.getPort());
}
InetAddress stringToInetAddress(const std::string& key) {
    size_t colonPos = key.find(":");
    std::string ipStr = key.substr(0, colonPos);
    uint16_t port = static_cast<uint16_t>(std::stoi(key.substr(colonPos + 1)));

    // Decide based on the type of address (IPv4 or IPv6)
    InetAddress addr;
    if (ipStr.find(":") != std::string::npos) {  // IPv6 case
        addr = InetAddress(ipStr, port);  // Assuming InetAddress constructor takes string and port
    } else {  // IPv4 case
        addr = InetAddress(ipStr, port);
    }
    return addr;
}


RlsUdpTask::RlsUdpTask(TaskBase *base, uint64_t sti, Vector3 phyLocation)
    : m_server{}, m_ctlTask{}, m_sti{sti}, m_phyLocation{phyLocation}, m_lastLoop{}, m_stiToUe{}, m_ueMap{}, m_newIdCounter{}
{
    m_logger = base->logBase->makeUniqueLogger("rls-udp");

    try
    {
        m_server = new udp::UdpServer(base->config->linkIp, cons::RadioLinkPort);
    }
    catch (const LibError &e)
    {
        m_logger->err("RLS failure [%s]", e.what());
        quit();
        return;
    }
}

void RlsUdpTask::onStart()
{
}
/*
void RlsUdpTask::onLoop()
{
    // Step 1: Periodic heartbeat cycle, unchanged
    auto current = utils::CurrentTimeMillis();
    if (current - m_lastLoop > LOOP_PERIOD)
    {
        m_lastLoop = current;
        heartbeatCycle(current);
    }

    // Step 2: Use a fixed peer address or determine it based on your setup
    InetAddress peerAddress; // You need to initialize this based on where messages are expected
    std::string key = inetAddressToString(peerAddress); // Generate key from InetAddress

    // Step 3: Retrieve the message using Pdcp's GetMessageMap
    std::vector<uint8_t> storedMessage =  pdcpManager.GetMessageMap(key);
    if (storedMessage.empty()) {
        m_logger->info("No message found in the map for key: " + key);
        return; // No message, skip processing
    }

    // Check if the message fits within the buffer size limits
    if (storedMessage.size() > BUFFER_SIZE) {
        m_logger->err("Message size exceeds buffer size.");
        return; // Skip oversized messages
    }

    // Step 4: Decode the RLS message
    auto rlsMsg = rls::DecodeRlsMessage(OctetView{storedMessage.data(), storedMessage.size()});
    if (rlsMsg == nullptr) {
        m_logger->err("Unable to decode RLS message GNB");
    } else {
        // Successfully decoded, call receiveRlsPdu
        receiveRlsPdu(peerAddress, std::move(rlsMsg));
    }
}
*/

// MAP onloop method 2
/*
void RlsUdpTask::onLoop()
{


    // Step 1: Periodic heartbeat cycle, unchanged
    auto current = utils::CurrentTimeMillis();
    if (current - m_lastLoop > LOOP_PERIOD)
    {
        m_lastLoop = current;
        heartbeatCycle(current);
    }

    //std::cout << "--- GNB onloop() Message map size: " << pdcpManager.GetmessageMapSize() << std::endl;



    // Step 2: Retrieve messages from the map (not from m_server->Receive anymore)

    for (auto it = messageMap.begin(); it != messageMap.end(); ) {
        const std::string& key = it->first;  // Get the key (address as string)
        InetAddress peerAddress = stringToInetAddress(key);  // Convert back to InetAddress

        // Get the message stored in the map
        std::vector<uint8_t>& message = it->second;

        // Step 3: Process the message (decode and call receiveRlsPdu)
        auto rlsMsg = rls::DecodeRlsMessage(OctetView{message.data(), message.size()});

        if (rlsMsg == nullptr) {
            m_logger->err("Unable to decode RLS message from map for GNB");
            // Optional: You may want to remove the message if it's not decodable
            it = messageMap.erase(it);  // Remove invalid message and update iterator
        } else {
            // Call receiveRlsPdu to process the decoded message
            receiveRlsPdu(peerAddress, std::move(rlsMsg));

            // Step 4: Once the message is processed, remove it from the map
            it = messageMap.erase(it);  // Erase the processed message and update iterator
        }
    }
}
*/
//MAP onloop
/*
void RlsUdpTask::onLoop()
{
    auto current = utils::CurrentTimeMillis();

    // Check if it's time to run the heartbeat cycle
    if (current - m_lastLoop > LOOP_PERIOD)
    {
        m_lastLoop = current;
        heartbeatCycle(current);
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
        if (size > BUFFER_SIZE) {
            m_logger->err("Message size exceeds buffer size.");
            return;
        }

        std::copy(storedMessage.begin(), storedMessage.end(), buffer);

        std::cout << "Message retrieved from internal map for address: " << key << std::endl;

        // Decode the RLS message
        auto rlsMsg = rls::DecodeRlsMessage(OctetView{buffer, static_cast<size_t>(size)});

        if (rlsMsg == nullptr) {
            m_logger->err("Unable to decode RLS message GNB");
        } else {
            receiveRlsPdu(peerAddress, std::move(rlsMsg));
        }

        // Optionally, remove the message after processing
        // messageMap.erase(key);
    }
}
*/

//ORIGINAL
void RlsUdpTask::onLoop()
{
    auto current = utils::CurrentTimeMillis();
    if (current - m_lastLoop > LOOP_PERIOD)
    {
        m_lastLoop = current;
        heartbeatCycle(current);
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

//Set&Get
/*
void RlsUdpTask::onLoop()
{
    auto current = utils::CurrentTimeMillis();
    if (current - m_lastLoop > LOOP_PERIOD)
    {
        m_lastLoop = current;
        heartbeatCycle(current);
    }

    uint8_t buffer[BUFFER_SIZE];
    InetAddress peerAddress;

   //std::cout << "GNB ORIGINAL ------- ONLOOP Peer Address:  " << inetAddressToString(peerAddress) << std::endl;
    //std::cout << "GNB ORIGINAL ------- BUFFER_SIZE :  " << BUFFER_SIZE << std::endl;

    size_t length = pdcpManager.GetStreamLength();  // Get the global stream length
    //std::cout << "GNB ORIGINAL ONLOOP------- GetStreamLength!!!!!!!!!!!!!!!!!!!!!!   :  " << length << std::endl;

    //uint8_t* streamData;
    const uint8_t* streamData;

    if (pdcpManager.GetStreamData() != nullptr && length > 0) {

        streamData = pdcpManager.GetStreamData(); // Get the global stream data

        //std::cout << "******* GNB ORIGINAL ONLOOP------- GetStreamData() !!!!!!!!!!!!!!!!!!!!!!: " << streamData << std::endl;
    }

    int size = m_server->Receive(buffer, BUFFER_SIZE, RECEIVE_TIMEOUT, peerAddress);

    //std::cout << "GNB ORIGINAL ONLOOP------- SIZE!!!!!!!!!!!!!!!!!!!!!!   :  " << size << std::endl;


    //if (size > 0)
    if (length > 0)
    {
        //std::cout << " -----------------ENTER GNB UDP TASK.CPP size > 0 ONLOOP " << std::endl;
      //  std::cout << " -----------------ENTER GNB UDP TASK.CPP size > 0 buffer streamData " << streamData << std::endl;


        //auto rlsMsg = rls::DecodeRlsMessage(OctetView{buffer, static_cast<size_t>(size)});
       if (streamData != nullptr)
       {
            //std::cout << " -----------------ENTER GNB UDP TASK.CPP streamData != nullptr streamData " << streamData << std::endl;
            auto rlsMsg = rls::DecodeRlsMessage(OctetView{streamData, static_cast<size_t>(length)});

            if (rlsMsg == nullptr)
                m_logger->err("Unable to decode RLS message GNB");
            else{
                //std::cout << " ---GNB ON LOOP------BEFORE -----receiveRlsPdu-- " <<  std::endl;
                peerAddress=pdcpManager.GetAddress();
                receiveRlsPdu(peerAddress, std::move(rlsMsg));
                std::cout << " ---GNB ON LOOP-------AFTER ----receiveRlsPdu---"<< peerAddress.getSockAddr() <<  std::endl;
            }
       }
    }
}
*/

void RlsUdpTask::onQuit()
{
    delete m_server;
}

// ORIGINAL RECEIVE
/*
void RlsUdpTask::receiveRlsPdu(const InetAddress &addr, std::unique_ptr<rls::RlsMessage> &&msg)
{
    // Convert InetAddress to string to use as a key
    std::string key = inetAddressToString(addr);
    std::cout << "GNB UDP TASK.CPP receiveRlsPDU Key: " << key << std::endl;

    if (msg->msgType == rls::EMessageType::HEARTBEAT)
    {
        int dbm = EstimateSimulatedDbm(m_phyLocation, ((const rls::RlsHeartBeat &)*msg).simPos);
        if (dbm < MIN_ALLOWED_DBM)
        {
            // if the simulated signal strength is such low, then ignore this message
            return;
        }

        if (m_stiToUe.count(msg->sti))
        {
            int ueId = m_stiToUe[msg->sti];
            m_ueMap[ueId].address = addr;
            m_ueMap[ueId].lastSeen = utils::CurrentTimeMillis();
        }
        else
        {
            int ueId = ++m_newIdCounter;

            m_stiToUe[msg->sti] = ueId;
            m_ueMap[ueId].address = addr;
            m_ueMap[ueId].lastSeen = utils::CurrentTimeMillis();

            auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::SIGNAL_DETECTED);
            w->ueId = ueId;
            m_ctlTask->push(std::move(w));
        }

        rls::RlsHeartBeatAck ack{m_sti};
        ack.dbm = dbm;

        sendRlsPdu(addr, ack);

        return;
    }

    if (!m_stiToUe.count(msg->sti))
    {
        // if no HB received yet, and the message is not HB, then ignore the message
        return;
    }

    auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::RECEIVE_RLS_MESSAGE);
    w->ueId = m_stiToUe[msg->sti];
    w->msg = std::move(msg);
    m_ctlTask->push(std::move(w));
}*/

// RECEIVE MAP
/*
void RlsUdpTask::receiveRlsPdu(const InetAddress &addr, std::unique_ptr<rls::RlsMessage> &&msg)
{
    // Step 1: Convert InetAddress to string to use as a key
    std::string key = inetAddressToString(addr);
    std::cout << "GNB receiveRlsPdu UDP_TASK.CPP receiveRlsPDU Key: " << key << std::endl;
*/
    // Step 2: Check if the message exists in the map
    /*
    if (messageMap.find(key) != messageMap.end()) {
        // Retrieve the message from the map
        std::vector<uint8_t> message = messageMap[key];

        // Decode the retrieved message
        auto decodedMsg = rls::DecodeRlsMessage(OctetView{message.data(), message.size()});
        if (decodedMsg == nullptr) {
            std::cout << "GNB UDP TASK: Failed to decode message from map." << std::endl;
            return;
        }

        msg = std::move(decodedMsg);  // Move the decoded message into msg for further processing
    } else {
        std::cout << "GNB UDP TASK: No message found in map for the given key." << std::endl;
        return;  // Exit if no message found for the address
    }
    */

    // Step 3: Process the RLS message (same logic as before)
/*    if (msg->msgType == rls::EMessageType::HEARTBEAT) {
        int dbm = EstimateSimulatedDbm(m_phyLocation, ((const rls::RlsHeartBeat &)*msg).simPos);
        if (dbm < MIN_ALLOWED_DBM) {
            // If the signal strength is too low, ignore the message
            return;
        }

        if (m_stiToUe.count(msg->sti)) {
            int ueId = m_stiToUe[msg->sti];
            m_ueMap[ueId].address = addr;
            m_ueMap[ueId].lastSeen = utils::CurrentTimeMillis();
        } else {
            // New UE detected, assign a new UE ID
            int ueId = ++m_newIdCounter;
            m_stiToUe[msg->sti] = ueId;
            m_ueMap[ueId].address = addr;
            m_ueMap[ueId].lastSeen = utils::CurrentTimeMillis();

            // Notify the control task about the new UE detection
            auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::SIGNAL_DETECTED);
            w->ueId = ueId;
            m_ctlTask->push(std::move(w));
        }

        // Send a heartbeat acknowledgment back
        rls::RlsHeartBeatAck ack{m_sti};
        ack.dbm = dbm;
        sendRlsPdu(addr, ack);

        return;
    }

    // Step 4: Handle non-heartbeat messages
    if (!m_stiToUe.count(msg->sti)) {
        // If no heartbeat was received yet, ignore non-heartbeat messages
        return;
    }

    auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::RECEIVE_RLS_MESSAGE);
    w->ueId = m_stiToUe[msg->sti];
    w->msg = std::move(msg);
    m_ctlTask->push(std::move(w));
}*/

//ORIGINAL RECEIVE
void RlsUdpTask::receiveRlsPdu(const InetAddress &addr, std::unique_ptr<rls::RlsMessage> &&msg)
{
    if (msg->msgType == rls::EMessageType::HEARTBEAT)
    {
        int dbm = EstimateSimulatedDbm(m_phyLocation, ((const rls::RlsHeartBeat &)*msg).simPos);
        if (dbm < MIN_ALLOWED_DBM)
        {
            // if the simulated signal strength is such low, then ignore this message
            return;
        }

        if (m_stiToUe.count(msg->sti))
        {
            int ueId = m_stiToUe[msg->sti];
            m_ueMap[ueId].address = addr;
            m_ueMap[ueId].lastSeen = utils::CurrentTimeMillis();
        }
        else
        {
            int ueId = ++m_newIdCounter;

            m_stiToUe[msg->sti] = ueId;
            m_ueMap[ueId].address = addr;
            m_ueMap[ueId].lastSeen = utils::CurrentTimeMillis();

            auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::SIGNAL_DETECTED);
            w->ueId = ueId;
            m_ctlTask->push(std::move(w));
        }

        rls::RlsHeartBeatAck ack{m_sti};
        ack.dbm = dbm;

        sendRlsPdu(addr, ack);
        return;
    }

    if (!m_stiToUe.count(msg->sti))
    {
        // if no HB received yet, and the message is not HB, then ignore the message
        return;
    }

    auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::RECEIVE_RLS_MESSAGE);
    w->ueId = m_stiToUe[msg->sti];
    w->msg = std::move(msg);
    m_ctlTask->push(std::move(w));
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
    // Print the InetAddress (assuming a method exists to convert addr to string)
    std::cout << "GNB....Sending PDU to address: " << addr.getSockAddr() << std::endl;

    // Print the RlsMessage details
    std::cout << "GNB....RlsMessage - Type: " << static_cast<int>(msg.msgType) << ", STI: " << msg.sti << std::endl;

    // Encode the RLS message
    OctetString stream;
    rls::EncodeRlsMessage(msg, stream);


    // OMAR
    // Use the converted string as the key
    //std::string key = inetAddressToString(addr);
    //std::cout << "GNB UDP TASK.CPP sendRlsPDU Key: " << key << std::endl;
    //std::vector<uint8_t> message(stream.data(), stream.data() + stream.length());
    //messageMap[key] = message;
    // OMAR

    std::cout << "GNB....RlsMessage in sendRlsPdu- BEFORE   m_server->Send(" << std::endl;

    //OMAR
    // Use the converted string as the key
    std::string key = inetAddressToString(addr);
    std::cout << "GNB UDP_TASK.CPP MAP sendRlsPDU Key: " << key << std::endl;
    std::cout << "GNB UDP_TASK.CPP MAP sendRlsPDU stream.length() //////// : " << stream.length() << std::endl;

    //std::vector<uint8_t> message(stream.data(), stream.data() + stream.length());
    //std::vector<uint8_t> message;
   // message.push_back(static_cast<uint8_t>(stream.length()));  // Store length as first byte
   // message.insert(message.end(), stream.data(), stream.data() + stream.length()); // Append actual data

    //pdcpManager.SetMessageMap(key, message);
    //messageMap[key] = message;
    pdcpManager.SetStreamLength(stream.length());  // Store the stream length globally

    std::cout << " GNB Set stream length in PdcpManager: " << stream.length() << std::endl;

    //std::cout << "UE onloop() Message map size: " << pdcpManager.GetmessageMapSize() << std::endl;
    // OMAR

    if (stream.data() != nullptr)
    {//
        //pdcpManager.SetStreamData(static_cast<uint8_t*>(stream.data()));
        pdcpManager.SetStreamData(stream.data(), stream.length());  // Store data safely
        pdcpManager.SetAddress(addr);
        std::cout << "******** GNB UDP TASK.CPP m_server->Send UE BUFFER stream.data(): " << stream.data() << std::endl;
    }

    //m_server->Send(addr, stream.data(), static_cast<size_t>(stream.length()));
    //m_server->Send(addr, stream.data(), static_cast<size_t>(stream.length()));

    std::cout << "GNB....RlsMessage in sendRlsPdu- AFTER  m_server->Send( " << std::endl;

 }
*/

void RlsUdpTask::heartbeatCycle(int64_t time)
{
    std::set<int> lostUeId{};
    std::set<uint64_t> lostSti{};

    for (auto &item : m_ueMap)
    {
        if (time - item.second.lastSeen > HEARTBEAT_THRESHOLD)
        {
            lostUeId.insert(item.first);
            lostSti.insert(item.second.sti);
        }
    }

    for (uint64_t sti : lostSti)
        m_stiToUe.erase(sti);

    for (int ueId : lostUeId)
        m_ueMap.erase(ueId);

    for (int ueId : lostUeId)
    {
        auto w = std::make_unique<NmGnbRlsToRls>(NmGnbRlsToRls::SIGNAL_LOST);
        w->ueId = ueId;
        m_ctlTask->push(std::move(w));
    }
}

void RlsUdpTask::initialize(NtsTask *ctlTask)
{
    m_ctlTask = ctlTask;
}

void RlsUdpTask::send(int ueId, const rls::RlsMessage &msg)
{
    if (ueId == 0)
    {
        for (auto &ue : m_ueMap)
            send(ue.first, msg);
        return;
    }

    if (!m_ueMap.count(ueId))
    {
        // ignore the message
        return;
    }

    sendRlsPdu(m_ueMap[ueId].address, msg);
    setMessageForUe(ueId, msg); // ADDED by omar

}

// SET/GET OMAR
void RlsUdpTask::setMessageForUe(int ueId, const rls::RlsMessage &msg) {
    std::cout << " ***** setMessageForUeSending ueId: " << ueId << std::endl;

    // Print the RlsMessage details
    std::cout << " ***** setMessageForUeRlsMessage - Type: " << static_cast<int>(msg.msgType) << ", STI: " << msg.sti << std::endl;
    m_msgMap.emplace(ueId, msg);
}

rls::RlsMessage RlsUdpTask::getMessageForUe(int ueId) const {
    auto it = m_msgMap.find(ueId);
    if (it != m_msgMap.end()) {
        return it->second;
    }
    // Handle case where the message doesn't exist
    throw std::runtime_error("Message not found for UE");
}

} // namespace nr::gnb
