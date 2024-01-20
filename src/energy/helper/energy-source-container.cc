/*
 * Copyright (c) 2008 INRIA
 * Copyright (c) 2010 Network Security Lab, University of Washington, Seattle.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Authors: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 *          Sidharth Nabar <snabar@uw.edu>, He Wu <mdzz@u.washington.edu>
 */

#include "energy-source-container.h"

namespace ns3
{
namespace energy
{

NS_OBJECT_ENSURE_REGISTERED(EnergySourceContainer);

TypeId
EnergySourceContainer::GetTypeId()
{
    static TypeId tid = TypeId("ns3::energy::EnergySourceContainer")
                            .AddDeprecatedName("ns3::EnergySourceContainer")
                            .SetParent<Object>()
                            .SetGroupName("Energy")
                            .AddConstructor<EnergySourceContainer>();
    return tid;
}

/*
 * Private functions start here.
 */
void
EnergySourceContainer::DoDispose()
{
    // call Object::Dispose for all EnergySource objects
    for (auto i = Begin(); i != End(); i++)
    {
        (*i)->DisposeDeviceModels();
        (*i)->Dispose();
    }
    Clear();
}

void
EnergySourceContainer::DoInitialize()
{
    // call Object::Start for all EnergySource objects
    for (auto i = Begin(); i != End(); i++)
    {
        (*i)->Initialize();
        (*i)->InitializeDeviceModels();
    }
}

} // namespace energy
} // namespace ns3
