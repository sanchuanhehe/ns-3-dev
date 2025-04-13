/*
 * Copyright (c) 2018 University of Washington
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Sébastien Deronne <sebastien.deronne@gmail.com>
 */

#include "obss-pd-algorithm.h"

#include "ns3/decibel-mw.h"
#include "ns3/double.h"
#include "ns3/eht-phy.h"
#include "ns3/log.h"
#include "ns3/wifi-net-device.h"
#include "ns3/wifi-phy.h"
#include "ns3/wifi-utils.h"
#include "ns3/units.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("ObssPdAlgorithm");
NS_OBJECT_ENSURE_REGISTERED(ObssPdAlgorithm);

TypeId
ObssPdAlgorithm::GetTypeId()
{
    static ns3::TypeId tid =
        ns3::TypeId("ns3::ObssPdAlgorithm")
            .SetParent<Object>()
            .SetGroupName("Wifi")
            .AddAttribute("ObssPdLevel",
                          "The current OBSS PD level (dBm).",
                          DecibelMwValue(units::power::dBm_t(-82.0)),
                          MakeDecibelMwAccessor(&ObssPdAlgorithm::SetObssPdLevel,
                                             &ObssPdAlgorithm::GetObssPdLevel),
                          MakeDecibelMwChecker(units::power::dBm_t(-101), units::power::dBm_t(-62)))
            .AddAttribute("ObssPdLevelMin",
                          "Minimum value (dBm) of OBSS PD level.",
                          DecibelMwValue(units::power::dBm_t(-82.0)),
                          MakeDecibelMwAccessor(&ObssPdAlgorithm::m_obssPdLevelMin),
                          MakeDecibelMwChecker(units::power::dBm_t(-101), units::power::dBm_t(-62)))
            .AddAttribute("ObssPdLevelMax",
                          "Maximum value (dBm) of OBSS PD level.",
                          DecibelMwValue(units::power::dBm_t(-62.0)),
                          MakeDecibelMwAccessor(&ObssPdAlgorithm::m_obssPdLevelMax),
                          MakeDecibelMwChecker(units::power::dBm_t(-101), units::power::dBm_t( -62)))
            .AddAttribute("TxPowerRefSiso",
                          "The SISO reference TX power level (dBm).",
                          DecibelMwValue(units::power::dBm_t(21)),
                          MakeDecibelMwAccessor(&ObssPdAlgorithm::m_txPowerRefSiso),
                          MakeDecibelMwChecker())
            .AddAttribute("TxPowerRefMimo",
                          "The MIMO reference TX power level (dBm).",
                          DecibelMwValue(units::power::dBm_t(25)),
                          MakeDecibelMwAccessor(&ObssPdAlgorithm::m_txPowerRefMimo),
                          MakeDecibelMwChecker())
            .AddTraceSource("Reset",
                            "Trace CCA Reset event",
                            MakeTraceSourceAccessor(&ObssPdAlgorithm::m_resetEvent),
                            "ns3::ObssPdAlgorithm::ResetTracedCallback");
    return tid;
}

void
ObssPdAlgorithm::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_device = nullptr;
}

void
ObssPdAlgorithm::ConnectWifiNetDevice(const Ptr<WifiNetDevice> device)
{
    NS_LOG_FUNCTION(this << device);
    m_device = device;
    auto phy = device->GetPhy();
    if (phy->GetStandard() >= WIFI_STANDARD_80211be)
    {
        auto ehtPhy = DynamicCast<EhtPhy>(device->GetPhy()->GetPhyEntity(WIFI_MOD_CLASS_EHT));
        NS_ASSERT(ehtPhy);
        ehtPhy->SetObssPdAlgorithm(this);
    }
    auto hePhy = DynamicCast<HePhy>(device->GetPhy()->GetPhyEntity(WIFI_MOD_CLASS_HE));
    NS_ASSERT(hePhy);
    hePhy->SetObssPdAlgorithm(this);
}

void
ObssPdAlgorithm::ResetPhy(HeSigAParameters params)
{
    units::power::dBm_t txPowerMaxSiso{0};
    units::power::dBm_t txPowerMaxMimo{0};
    bool powerRestricted = false;
    // Fetch my BSS color
    Ptr<HeConfiguration> heConfiguration = m_device->GetHeConfiguration();
    NS_ASSERT(heConfiguration);
    uint8_t bssColor = heConfiguration->m_bssColor;
    NS_LOG_DEBUG("My BSS color " << (uint16_t)bssColor << " received frame "
                                 << (uint16_t)params.bssColor);

    Ptr<WifiPhy> phy = m_device->GetPhy();
    if ((m_obssPdLevel > m_obssPdLevelMin) && (m_obssPdLevel <= m_obssPdLevelMax))
    {
        txPowerMaxSiso = m_txPowerRefSiso - (m_obssPdLevel - m_obssPdLevelMin);
        txPowerMaxMimo = m_txPowerRefMimo - (m_obssPdLevel - m_obssPdLevelMin);
        powerRestricted = true;
    }
    m_resetEvent(bssColor, params.rssi, powerRestricted, txPowerMaxSiso, txPowerMaxMimo);
    phy->ResetCca(powerRestricted, txPowerMaxSiso, txPowerMaxMimo);
}

void
ObssPdAlgorithm::SetObssPdLevel(units::power::dBm_t level)
{
    NS_LOG_FUNCTION(this << level);
    m_obssPdLevel = level;
}

units::power::dBm_t
ObssPdAlgorithm::GetObssPdLevel() const
{
    return m_obssPdLevel;
}

} // namespace ns3
