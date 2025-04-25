/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) Liangcheng Yu 2019
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Liangcheng Yu <liangcheng.yu46@gmail.com>
 * GSoC 2019 project Mentors:
 *          Dizhi Zhou, Mohit P. Tahiliani, Tom Henderson
 * 
 */

// Define the base class for data center network topology generators.

#ifndef DCN_TOPOLOGY_HELPER_H
#define DCN_TOPOLOGY_HELPER_H

#define MSG_NETDEVICES_MISSING "Please install NetDevices with the target L2 helper!"
#define MSG_NETDEVICES_CONFLICT "NetDevices installed already!"

#include "fatal-error.h"
#include "internet-stack-helper.h"
#include "ipv4-address-helper.h"
#include "ipv6-address-helper.h"
#include "traffic-control-helper.h"

namespace ns3 {

/**
 * \ingroup data-center
 *
 * \brief A helper to make it easier to create a data center networking topology
 */
class DcnTopologyHelper
{
public:
  DcnTopologyHelper ();

  virtual ~DcnTopologyHelper ();

  /**
   * \param stack an InternetStackHelper which is used to install
   *              on every node in the topology
   */
  virtual void InstallStack (InternetStackHelper& stack) = 0;

  /**
   * \param tchSwitch a TrafficControlHelper which is used to install
   *                   on every switches in the topology
   * \param tchServer a TrafficControlHelper which is used to install
   *                    on every servers in the topology
   */
  virtual void InstallTrafficControl (TrafficControlHelper& tchSwitch,
                                      TrafficControlHelper& tchServer) = 0;

  /**
   * Assigns IPv4 addresses to all the interfaces of switches and servers
   *
   * \param network an IPv4 address representing the network portion
   *                of the IPv4 address
   *
   * \param mask the mask length
   */
  virtual void AssignIpv4Addresses (Ipv4Address network, Ipv4Mask mask) = 0;

  /**
   * Assigns IPv6 addresses to all the interfaces of the switches and servers
   *
   * \param network an IPv6 address representing the network portion
   *                of the IPv6 address
   *
   * \param prefix the prefix length
   */
  virtual void AssignIpv6Addresses (Ipv6Address network, Ipv6Prefix prefix) = 0;

  /**
   * Sets up the node canvas locations for every node in the topology.
   * This is needed for use with the animation interface.
   *
   * \param ulx upper left x value
   * \param uly upper left y value
   * \param lrx lower right x value
   * \param lry lower right y value
   */
  virtual void BoundingBox (double ulx, double uly, double lrx, double lry) = 0;

};

} // namespace ns3

#endif /* DCN_TOPOLOGY_HELPER_H */
