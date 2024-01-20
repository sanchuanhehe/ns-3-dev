/*
 * Copyright (c) 2014 Wireless Communications and Networking Group (WCNG),
 * University of Rochester, Rochester, NY, USA.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Cristiano Tapparello <cristiano.tapparello@rochester.edu>
 */

#ifndef ENERGY_HARVESTER_CONTAINER_H
#define ENERGY_HARVESTER_CONTAINER_H

#include "ns3/energy-harvester.h"
#include "ns3/object-container.h"
#include "ns3/object.h"

#include <stdint.h>
#include <vector>

namespace ns3
{
namespace energy
{

class EnergyHarvester;

/**
 * @ingroup energy
 * @brief Holds a vector of ns3::EnergyHarvester pointers.
 *
 * EnergyHarvesterContainer returns a list of EnergyHarvester pointers
 * installed on a node. Users can use this list to access EnergyHarvester
 * objects to obtain the total energy harvested on a node easily.
 *
 * @see NetDeviceContainer
 *
 */
class EnergyHarvesterContainer : public ObjectContainer<EnergyHarvester>, public Object
{
  public:
    using ObjectContainer<EnergyHarvester>::Iterator;
    using ObjectContainer<EnergyHarvester>::ObjectContainer;
    using ObjectContainer<EnergyHarvester>::Add;
    using ObjectContainer<EnergyHarvester>::Create;
    using ObjectContainer<EnergyHarvester>::Clear;
    using ObjectContainer<EnergyHarvester>::Begin;
    using ObjectContainer<EnergyHarvester>::End;
    using ObjectContainer<EnergyHarvester>::Get;
    using ObjectContainer<EnergyHarvester>::GetN;
    using ObjectContainer<EnergyHarvester>::operator[];
    using ObjectContainer<EnergyHarvester>::GetAllItems;
    using ObjectContainer<EnergyHarvester>::Contains;

    /**
     * @brief Get the type ID.
     * @return The object TypeId.
     */
    static TypeId GetTypeId();

  private:
    void DoDispose() override;

    /**
     * @brief Calls Object::Initialize () for all EnergySource objects.
     */
    void DoInitialize() override;
};

} // namespace energy
} // namespace ns3

#endif /* defined(ENERGY_HARVESTER_CONTAINER_H) */
