/*
 * Copyright (c) 2008 INRIA
 * Copyright (c) 2010 Network Security Lab, University of Washington, Seattle.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Authors: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 *          Sidharth Nabar <snabar@uw.edu>, He Wu <mdzz@u.washington.edu>
 */

#ifndef DEVICE_ENERGY_MODEL_CONTAINER_H
#define DEVICE_ENERGY_MODEL_CONTAINER_H

#include "device-energy-model.h"

#include "ns3/object-container.h"

namespace ns3
{
namespace energy
{

/**
 * @ingroup energy
 * @brief Holds a vector of ns3::DeviceEnergyModel pointers.
 *
 * DeviceEnergyModelContainer returns a list of DeviceEnergyModel pointers
 * installed on a node. Users can use this list to access DeviceEnergyModel
 * objects to obtain total device energy consumption on a node easily.
 *
 * @see NetDeviceContainer
 *
 */
using DeviceEnergyModelContainer = ObjectContainer<DeviceEnergyModel>;

} // namespace energy
} // namespace ns3

#endif /* DEVICE_ENERGY_MODEL_CONTAINER_H */
