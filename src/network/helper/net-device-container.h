/*
 * Copyright (c) 2008 INRIA
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#ifndef NET_DEVICE_CONTAINER_H
#define NET_DEVICE_CONTAINER_H

#include "ns3/net-device.h"
#include "ns3/object-container.h"

namespace ns3
{

/**
 * @brief holds a vector of ns3::NetDevice pointers
 *
 * Typically ns-3 NetDevices are installed on nodes using a net device
 * helper.  The helper Install method takes a NodeContainer which holds
 * some number of Ptr<Node>.  For each of the Nodes in the NodeContainer
 * the helper will instantiate a net device, add a MAC address and a queue
 * to the device and install it to the node.  For each of the devices, the
 * helper also adds the device into a Container for later use by the caller.
 * This is that container used to hold the Ptr<NetDevice> which are
 * instantiated by the device helper.
 */
using NetDeviceContainer = ObjectContainer<NetDevice>;

} // namespace ns3

#endif /* NET_DEVICE_CONTAINER_H */
