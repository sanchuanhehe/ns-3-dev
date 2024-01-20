/*
 * Copyright (c) 2008 INRIA
 * Copyright (c) 2010 Network Security Lab, University of Washington, Seattle.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Authors: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 *          Sidharth Nabar <snabar@uw.edu>, He Wu <mdzz@u.washington.edu>
 */

#ifndef ENERGY_SOURCE_CONTAINER_H
#define ENERGY_SOURCE_CONTAINER_H

#include "ns3/energy-source.h"
#include "ns3/object-container.h"
#include "ns3/object.h"

#include <stdint.h>

namespace ns3
{
namespace energy
{

/**
 * @ingroup energy
 * @brief Holds a vector of ns3::EnergySource pointers.
 *
 * EnergySourceHelper returns a list of EnergySource pointers installed onto a
 * node. Users can use this list to access EnergySource objects to obtain total
 * energy consumption on a node easily.
 *
 * @see NetDeviceContainer
 *
 */
class EnergySourceContainer : public ObjectContainer<EnergySource>, public Object
{
  public:
    using ObjectContainer<EnergySource>::Iterator;
    using ObjectContainer<EnergySource>::ObjectContainer;
    using ObjectContainer<EnergySource>::Add;
    using ObjectContainer<EnergySource>::Create;
    using ObjectContainer<EnergySource>::Clear;
    using ObjectContainer<EnergySource>::Begin;
    using ObjectContainer<EnergySource>::End;
    using ObjectContainer<EnergySource>::Get;
    using ObjectContainer<EnergySource>::GetN;
    using ObjectContainer<EnergySource>::operator[];
    using ObjectContainer<EnergySource>::GetAllItems;
    using ObjectContainer<EnergySource>::Contains;

    /**
     * @brief Get the type ID.
     * @return The object TypeId.
     */
    static TypeId GetTypeId();

  private:
    void DoDispose() override;

    /**
     * @brief Calls Object::Start () for all EnergySource objects.
     */
    void DoInitialize() override;
};

} // namespace energy
} // namespace ns3

#endif /* ENERGY_SOURCE_CONTAINER_H */
