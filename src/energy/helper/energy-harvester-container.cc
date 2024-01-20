/*
 * Copyright (c) 2014 Wireless Communications and Networking Group (WCNG),
 * University of Rochester, Rochester, NY, USA.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Cristiano Tapparello <cristiano.tapparello@rochester.edu>
 */

#include "energy-harvester-container.h"

#include "ns3/log.h"
#include "ns3/names.h"

namespace ns3
{
namespace energy
{

NS_LOG_COMPONENT_DEFINE("EnergyHarvesterContainer");
NS_OBJECT_ENSURE_REGISTERED(EnergyHarvesterContainer);

TypeId
EnergyHarvesterContainer::GetTypeId()
{
    static TypeId tid = TypeId("ns3::energy::EnergyHarvesterContainer")
                            .AddDeprecatedName("ns3::EnergyHarvesterContainer")
                            .SetParent<Object>()
                            .SetGroupName("Energy")
                            .AddConstructor<EnergyHarvesterContainer>();
    return tid;
}

/*
 * Private functions start here.
 */

void
EnergyHarvesterContainer::DoDispose()
{
    // call Object::Dispose for all EnergyHarvester objects
    for (auto i = Begin(); i != End(); i++)
    {
        (*i)->Dispose();
    }
    Clear();
}

void
EnergyHarvesterContainer::DoInitialize()
{
    // call Object::Initialize for all EnergyHarvester objects
    for (auto i = Begin(); i != End(); i++)
    {
        (*i)->Initialize();
    }
}

} // namespace energy
} // namespace ns3
