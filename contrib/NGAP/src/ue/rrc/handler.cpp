//
// This file is a part of UERANSIM project.
// Copyright (c) 2023 ALİ GÜNGÖR.
//
// https://github.com/aligungr/UERANSIM/
// See README, LICENSE, and CONTRIBUTING files for licensing details.
//

#include "task.hpp"
#include "../../../ns-3-dev/contrib/NGAP/src/lib/asn/utils.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/lib/rrc/encode.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/ue/nas/task.hpp" 
#include "../../../ns-3-dev/contrib/NGAP/src/ue/nts.hpp" 
#include <ns3/common.hpp>

#include <ns3/ASN_RRC_DLInformationTransfer-IEs.h>
#include <ns3/ASN_RRC_DLInformationTransfer.h>
#include <ns3/ASN_RRC_Paging.h>
#include <ns3/ASN_RRC_PagingRecord.h>
#include <ns3/ASN_RRC_PagingRecordList.h>
#include <ns3/ASN_RRC_RRCSetup-IEs.h>
#include <ns3/ASN_RRC_RRCSetup.h>
#include <ns3/ASN_RRC_RRCSetupComplete-IEs.h>
#include <ns3/ASN_RRC_RRCSetupComplete.h>
#include <ns3/ASN_RRC_RRCSetupRequest-IEs.h>
#include <ns3/ASN_RRC_RRCSetupRequest.h>
#include <ns3/ASN_RRC_UL-DCCH-Message.h>
#include <ns3/ASN_RRC_UL-DCCH-MessageType.h>
#include <ns3/ASN_RRC_ULInformationTransfer-IEs.h>
#include <ns3/ASN_RRC_ULInformationTransfer.h>

namespace nr::ue
{

void UeRrcTask::receivePaging(const ASN_RRC_Paging &msg)
{
    std::vector<GutiMobileIdentity> tmsiIds{};

    asn::ForeachItem(*msg.pagingRecordList, [&tmsiIds](auto &pagingRecord) {
        if (pagingRecord.ue_Identity.present == ASN_RRC_PagingUE_Identity_PR_ng_5G_S_TMSI)
        {
            auto recordTmsi = asn::GetOctetString(pagingRecord.ue_Identity.choice.ng_5G_S_TMSI);
            auto tmsiOs = BitBuffer{recordTmsi.data()};

            GutiMobileIdentity tmsi{};
            tmsi.amfSetId = tmsiOs.readBits(10);
            tmsi.amfPointer = tmsiOs.readBits(6);
            tmsi.tmsi = octet4{static_cast<uint32_t>(tmsiOs.readBitsLong(32) & 0xFFFFFFFFu)};

            tmsiIds.push_back(tmsi);
        }
    });

    auto w = std::make_unique<NmUeRrcToNas>(NmUeRrcToNas::PAGING);
    w->pagingTmsi = std::move(tmsiIds);
    m_base->nasTask->push(std::move(w));
}

} // namespace nr::ue