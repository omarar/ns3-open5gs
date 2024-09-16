#ifndef RRC_STRUCT_H
#define RRC_STRUCT_H


#include "lte-pdcp-sap.h"
#include "lte-rlc-sap.h"


namespace ns3
{

struct TransmitPdcpPduParametersSerializable {
    uint32_t packetSize;
    uint16_t rnti;
    uint8_t lcid;
};

struct RrcPduData {
      LteRlcSapProvider::TransmitPdcpPduParameters rrcPacket;
};

struct RrcPduDataSerializable {
    TransmitPdcpPduParametersSerializable rrcPacket;
    uint8_t packetData[10000]; // Adjust size as needed
};


} // namespace ns3























#endif // RRC_STRUCT_H
