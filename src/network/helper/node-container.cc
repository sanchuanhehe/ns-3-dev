/*
 * Copyright (c) 2008 INRIA
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#include "node-container.h"

#include "ns3/node-list.h"

namespace ns3
{

NodeContainer
NodeContainer::GetGlobal()
{
    NodeContainer c;
    for (auto i = NodeList::Begin(); i != NodeList::End(); ++i)
    {
        c.Add(*i);
    }
    return c;
}

NodeContainer::NodeContainer(uint32_t n, uint32_t systemId /* = 0 */)
{
    Create(n, systemId);
}

void
NodeContainer::Create(uint32_t n, uint32_t systemId)
{
    for (uint32_t i = 0; i < n; i++)
    {
        Add(CreateObject<Node>(systemId));
    }
}

bool
NodeContainer::Contains(uint32_t id) const
{
    for (auto it = Begin(); it != End(); it++)
    {
        if ((*it)->GetId() == id)
        {
            return true;
        }
    }
    return false;
}

} // namespace ns3
