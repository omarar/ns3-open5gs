#include "pdcp.h"
#include "ns3/simulator.h"
#include "ns3/lte-radio-bearer-tag.h"
#include <map>
#include <unordered_map>



namespace ns3
{

std::unordered_map<int, Ptr<LtePdcp>> Pdcp::m_UeId_PdcpMap;  // Define the static map
std::unordered_map<std::string, std::vector<uint8_t>> Pdcp::messageMap;

/////
Pdcp& Pdcp::GetInstance() {
    static Pdcp instance;
    return instance;
}

void Pdcp::SetPdcp(int ueId, ns3::Ptr<ns3::LtePdcp> pdcpInstance) {
    m_UeId_PdcpMap[ueId] = pdcpInstance;
}

// Method to get PDCP based on UE ID
ns3::Ptr<ns3::LtePdcp> Pdcp::GetPdcp(int ueId) {
    return m_UeId_PdcpMap[ueId];
}


/////////////
void Pdcp::SetMessageMap(std::string key, std::vector<uint8_t> message) {
    messageMap[key] = message;
}

std::vector<uint8_t> Pdcp::GetMessageMap(std::string key) {
    return messageMap[key];
}
void Pdcp::SetStreamLength(size_t length) {
    streamLength = length;  // Set the length directly
}
size_t Pdcp::GetStreamLength() const {
    return streamLength;  // Retrieve the stored length
}

//
/*
void Pdcp::SetStreamData(uint8_t* data) {
    streamData = data;
}

uint8_t* const Pdcp::GetStreamData()  const {
    return streamData;
}*/
void Pdcp::SetStreamData(const uint8_t* data, size_t length) {
    if (data == nullptr || length == 0) {
        std::cerr << "Invalid data or length for SetStreamData." << std::endl;
        return;
    }

    // Copy the data into the internal vector
    streamData.assign(data, data + length);
    streamLength = length;  // Store the length for later retrieval

    std::cout << "Set stream data with length: " << streamLength << std::endl;
}

const uint8_t* Pdcp::GetStreamData() const {
    if (streamData.empty()) {
        //std::cerr << "Warning: GetStreamData called but streamData is empty." << std::endl;
        return nullptr;
    }
    return streamData.data();  // Return a const pointer to the internal data
}
//
void Pdcp::SetAddress(const InetAddress &addr) {
    storedAddress = addr;
}

InetAddress Pdcp::GetAddress() const {
    return storedAddress;
}

///

/////////////

// Method to check if UE ID exists
bool Pdcp::HasPdcp(int ueId) {
    return m_UeId_PdcpMap.find(ueId) != m_UeId_PdcpMap.end();
}

size_t Pdcp::GetMapSize() {
    size_t mapSize = m_UeId_PdcpMap.size();
    //std::cout << "Current size of m_UeId_PdcpMap: " << mapSize << std::endl;
    return mapSize;
}

size_t Pdcp::GetmessageMapSize() {
    size_t messageMapSize = messageMap.size();
    //std::cout << "Current size of m_UeId_PdcpMap: " << mapSize << std::endl;
    return messageMapSize;
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

    //pdcp = ns3::CreateObject<ns3::LtePdcp>();     //////
    //rlc = ns3::CreateObject<ns3::LteRlcAm>();
    rlc = ns3::CreateObject<ns3::LteRlcUm>();  // Use LteRlcUm instead of LteRlcAm
    //Ptr<LteEnbRrc> m_rrc;

    //m_rrc = ns3::CreateObject<ns3::LteEnbRrc>();

    //uint16_t rnti = 1; // Example RNTI value
    //uint8_t lcid = 4;  // Example LCID value
    //SetupPdcp(pdcp, rnti, lcid);

    //rlc->SetLteMacSapProvider (m_rrc->m_macSapProvider);

   // pdcp->SetLteRlcSapProvider(rlc->GetLteRlcSapProvider());//
    //rlc->SetLteRlcSapUser(pdcp->GetLteRlcSapUser());//

    //m_macSapProvider = reinterpret_cast<ns3::LteMacSapProvider*>(rlc->GetLteMacSapProvider());
    //std::cout << "_________________PDCP.cpp m_macSapProvider = " <<m_macSapProvider << std::endl;
    //rlc->SetLteMacSapProvider (m_rrc->m_macSapProvider);

    // Set the MAC SAP Provider in RLC
    rlc->SetLteMacSapProvider(m_macSapProvider);

    // Log the MAC SAP Provider
    std::cout << "MAC SAP Provider set in RLC: " << m_macSapProvider << std::endl;



}

Pdcp::~Pdcp() {
    //pdcp = nullptr;
    rlc = nullptr;
}

void Pdcp::TransmitPacket(ns3::Ptr<ns3::Packet> packet) {
    ns3::LtePdcpSapProvider::TransmitPdcpSduParameters params;

    params.pdcpSdu = packet;
    pdcp->GetLtePdcpSapProvider()->TransmitPdcpSdu(params);
}
void TransmitPdcpSduWrapper(ns3::Ptr<ns3::LtePdcp> pdcp, ns3::LtePdcpSapProvider::TransmitPdcpSduParameters params) {
    pdcp->GetLtePdcpSapProvider()->TransmitPdcpSdu(params);
}
void Pdcp::ReceivePacket(ns3::Ptr<ns3::Packet> packet) {
    //pdcp->DoReceivePdu(packet);//
}



}

