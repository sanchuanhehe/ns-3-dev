/*
 * Copyright (c) 2015 Universita' degli Studi di Napoli Federico II
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Stefano Avallone <stavallo@unina.it>
 */

#ifndef QUEUE_DISC_CONTAINER_H
#define QUEUE_DISC_CONTAINER_H

#include "ns3/object-container.h"
#include "ns3/queue-disc.h"

namespace ns3
{

/**
 * @ingroup traffic-control
 *
 * @brief Holds a vector of ns3::QueueDisc pointers.
 *
 * Typically ns-3 QueueDiscs are installed on net devices using a traffic control
 * helper. The helper Install method takes a NetDeviceContainer which holds
 * some number of Ptr<NetDevice>. For each of the net devices in the
 * NetDeviceContainer the helper will instantiate a queue disc and install
 * it to the net device. For each of the queue discs, the helper also
 * adds the queue disc into a Container for later use by the caller.
 * This is that container used to hold the Ptr<QueueDisc> which are
 * instantiated by the traffic control helper.
 */
using QueueDiscContainer = ObjectContainer<QueueDisc>;

} // namespace ns3

#endif /* QUEUE_DISC_CONTAINER_H */
