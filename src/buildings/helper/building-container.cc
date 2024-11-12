/*
 * Copyright (c) 2008 INRIA
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr> (original node-container.cc)
 *         Nicola Baldo (wrote building-container.cc based on node-container.cc)
 */
#include "building-container.h"

#include "ns3/building-list.h"

namespace ns3
{

BuildingContainer
BuildingContainer::GetGlobal()
{
    BuildingContainer c;
    for (auto i = BuildingList::Begin(); i != BuildingList::End(); ++i)
    {
        c.Add(*i);
    }
    return c;
}

} // namespace ns3
