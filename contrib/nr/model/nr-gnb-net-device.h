/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_ENB_NET_DEVICE_H
#define NR_ENB_NET_DEVICE_H

#include "nr-net-device.h"
// addded//

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

//-----------
namespace ns3
{

class Packet;
class PacketBurst;
class Node;
class NrGnbPhy;
class NrGnbMac;
class LteEnbRrc;
class BandwidthPartGnb;
class LteEnbComponentCarrierManager;
class BwpManagerGnb;
class NrMacScheduler;

/**
 * \ingroup gnb
 * \brief The NrGnbNetDevice class
 *
 * This class represent the GNB NetDevice.
 */
class NrGnbNetDevice : public NrNetDevice
{
  public:
    static TypeId GetTypeId();

    NrGnbNetDevice();

    ~NrGnbNetDevice() override;

    Ptr<NrMacScheduler> GetScheduler(uint8_t index) const;

    Ptr<NrGnbMac> GetMac(uint8_t index) const;

    Ptr<NrGnbPhy> GetPhy(uint8_t index) const;

    Ptr<BwpManagerGnb> GetBwpManager() const;

    uint16_t GetBwpId(uint8_t index) const;

    /**
     * \return the cell id
     */
    uint16_t GetCellId() const;

    /**
     * \return the cell ids belonging to this gNB
     */
    std::vector<uint16_t> GetCellIds() const;

    /**
     * \brief Set this gnb cell id
     * \param cellId the cell id
     */
    void SetCellId(uint16_t cellId);

    uint16_t GetEarfcn(uint8_t index) const;

    void SetRrc(Ptr<LteEnbRrc> rrc);

    Ptr<LteEnbRrc> GetRrc();

    void SetCcMap(const std::map<uint8_t, Ptr<BandwidthPartGnb>>& ccm);

    /**
     * \brief Get the size of the component carriers map
     * \return the number of cc that we have
     */
    uint32_t GetCcMapSize() const;

    /**
     * \brief The gNB received a CTRL message list.
     *
     * The gNB should divide the messages to the BWP they pertain to.
     *
     * \param msgList Message list
     * \param sourceBwpId BWP Id from which the list originated
     */
    void RouteIngoingCtrlMsgs(const std::list<Ptr<NrControlMessage>>& msgList, uint8_t sourceBwpId);

    /**
     * \brief Route the outgoing messages to the right BWP
     * \param msgList the list of messages
     * \param sourceBwpId the source bwp of the messages
     */
    void RouteOutgoingCtrlMsgs(const std::list<Ptr<NrControlMessage>>& msgList,
                               uint8_t sourceBwpId);

    /**
     * \brief Update the RRC config. Must be called only once.
     */
    void UpdateConfig();
    /// added open5gs
    void ConnectOpen5gs();
    void ReceiveCommand(app::CliMessage &msg);
    void Loop();
    std::string Get_ngapIp() const;
    std::string Get_linkIp() const;
    std::string Get_gtpIp() const;
    void Set_ngapIp(std::string ngapIp);
    void Set_linkIp(std::string linkIp);
    void Set_gtpIp(std::string ngapIp);
    void generateIPAddress(uint16_t x, std::string& ngapIp, std::string& linkIp, std::string& gtpIp);
  protected:
    void DoInitialize() override;

    void DoDispose() override;
    bool DoSend(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) override;

  private:
    Ptr<LteEnbRrc> m_rrc;

    uint16_t m_cellId; //!< Cell ID. Set by the helper.
    ////added
    std::string ngapIp;
    std::string linkIp;
    std::string gtpIp;
    /// end of added 
    std::map<uint8_t, Ptr<BandwidthPartGnb>> m_ccMap; /**< ComponentCarrier map */

    Ptr<LteEnbComponentCarrierManager>
        m_componentCarrierManager; ///< the component carrier manager of this eNb
};

} // namespace ns3

#endif /* NR_ENB_NET_DEVICE_H */
