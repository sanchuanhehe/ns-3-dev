/*
 * Copyright (c) 2008 INRIA
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#ifndef NODE_CONTAINER_H
#define NODE_CONTAINER_H

#include "ns3/node.h"
#include "ns3/object-container.h"

#include <type_traits>

namespace ns3
{

/**
 * @brief keep track of a set of node pointers.
 *
 * Typically ns-3 helpers operate on more than one node at a time.  For example
 * a device helper may want to install devices on a large number of similar
 * nodes.  The helper Install methods usually take a NodeContainer as a
 * parameter.  NodeContainers hold the multiple Ptr<Node> which are used
 * to refer to the nodes.
 */
class NodeContainer : public ObjectContainer<Node>
{
  public:
    using ObjectContainer<Node>::Iterator;
    using ObjectContainer<Node>::iterator;
    using ObjectContainer<Node>::const_iterator;
    using ObjectContainer<Node>::ObjectContainer;
    using ObjectContainer<Node>::Add;
    using ObjectContainer<Node>::Create;
    using ObjectContainer<Node>::Clear;
    using ObjectContainer<Node>::Begin;
    using ObjectContainer<Node>::begin;
    using ObjectContainer<Node>::End;
    using ObjectContainer<Node>::end;
    using ObjectContainer<Node>::Get;
    using ObjectContainer<Node>::GetN;
    using ObjectContainer<Node>::operator[];
    using ObjectContainer<Node>::GetAllItems;
    using ObjectContainer<Node>::Contains;

    /**
     * @brief Create a NodeContainer that contains a list of _all_ nodes
     * created through NodeContainer::Create() and stored in the
     * ns3::NodeList.
     *
     * Whenever a Node is created, a Ptr<Node> is added to a global list of all
     * nodes in the system.  It is sometimes useful to be able to get to all
     * nodes in one place.  This method creates a NodeContainer that is
     * initialized to contain all of the simulation nodes,
     *
     * @returns a NodeContainer which contains a list of all Nodes.
     */
    static NodeContainer GetGlobal();

    /**
     * Create a NodeContainer with the requested number of Nodes.
     *
     * This is syntactic sugar for
     *
     * @code
     *   NodeContainer nodes;
     *   nodes.Create (size);
     *   // or nodes.Create (size, systemId);
     * @endcode
     *
     * @param [in] n The number of nodes to create.
     * @param [in] systemId The system id or rank associated with this node
     */
    explicit NodeContainer(uint32_t n, uint32_t systemId = 0);

    /**
     * @brief Create n nodes with specified systemId for distributed simulations
     * and append pointers to them to the end of this NodeContainer.
     *
     * Nodes are at the heart of any ns-3 simulation.  One of the first tasks that
     * any simulation needs to do is to create a number of nodes.  This method
     * automates that task, and adds the ability to specify systemId for
     * distributed simulations.
     *
     * @param n The number of Nodes to create
     * @param systemId The system id or rank associated with this node
     */
    void Create(uint32_t n, uint32_t systemId);

    /**
     * @brief Return true if container contains a Node with index id
     *
     * @param id Node Id
     * @return whether the NodeContainer contains a node with index id
     */
    bool Contains(uint32_t id) const;
};

} // namespace ns3

#endif /* NODE_CONTAINER_H */
