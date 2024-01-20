/*
 * Copyright (c) 2008 INRIA
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr> (original node-container.h)
 *         Nicola Baldo (wrote building-container.h based on node-container.h)
 */
#ifndef BUILDING_CONTAINER_H
#define BUILDING_CONTAINER_H

#include <ns3/building.h>
#include <ns3/object-container.h>

#include <stdint.h>
#include <vector>

namespace ns3
{

/**
 * @ingroup buildings
 *
 * @brief keep track of a set of building pointers.
 *
 * Some ns-3 helpers operate on more than one building at a time.  For example
 * a PositionAllocator may want to position nodes on a set of buildings.
 * The helper methods will then usually take a BuildingContainer as a
 * parameter.  BuildingContainers hold the multiple Ptr<Building> which are used
 * to refer to the buildings.
 */
class BuildingContainer : public ObjectContainer<Building>
{
  public:
    using ObjectContainer<Building>::Iterator;
    using ObjectContainer<Building>::ObjectContainer;
    using ObjectContainer<Building>::Add;
    using ObjectContainer<Building>::Create;
    using ObjectContainer<Building>::Clear;
    using ObjectContainer<Building>::Begin;
    using ObjectContainer<Building>::End;
    using ObjectContainer<Building>::Get;
    using ObjectContainer<Building>::GetN;
    using ObjectContainer<Building>::operator[];
    using ObjectContainer<Building>::GetAllItems;
    using ObjectContainer<Building>::Contains;

    /**
     * @brief Create a BuildingContainer that contains a list of _all_ buildings
     * stored in the ns3::BuildingList.
     *
     * Whenever a Building is created, a Ptr<Building> is added to a global list of all
     * buildings in the system.  It is sometimes useful to be able to get to all
     * buildings in one place.  This method creates a BuildingContainer that is
     * initialized to contain all of the simulation buildings,
     *
     * @returns a BuildingContainer which contains a list of all Buildings.
     */
    static BuildingContainer GetGlobal();
};

} // namespace ns3

#endif /* BUILDING_CONTAINER_H */
