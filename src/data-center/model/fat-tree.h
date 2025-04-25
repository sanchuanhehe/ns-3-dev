/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 NITK Surathkal
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
 * Authors: Shravya K.S. <shravya.ks0@gmail.com>
 * Modified by Liangcheng Yu <liangcheng.yu46@gmail.com>
 * GSoC 2019 project Mentors:
 *          Dizhi Zhou, Mohit P. Tahiliani, Tom Henderson
 * 
 */

// Define an object to create a Fat tree topology.

#ifndef FAT_TREE_HELPER_H
#define FAT_TREE_HELPER_H

#include <vector>

#include "csma-helper.h"
#include "dcn-topology.h"
#include "fatal-error.h"
#include "internet-stack-helper.h"
#include "ipv4-address-helper.h"
#include "ipv6-address-helper.h"
#include "ipv4-interface-container.h"
#include "ipv6-interface-container.h"
#include "net-device-container.h"
#include "point-to-point-helper.h"
#include "traffic-control-helper.h"

namespace ns3 {

/**
 * \ingroup data-center
 *
 * \brief A helper to make it easier to create a Fat tree topology
 */
class FatTreeHelper : public DcnTopologyHelper
{
public:
  /**
   * Create a FatTreeHelper in order to easily create the IP layer Fat tree topology
   *
   * \param numPods total number of Pods in Fat tree
   *
   */
  FatTreeHelper (uint32_t numPods);

  virtual ~FatTreeHelper ();

  FatTreeHelper (const FatTreeHelper& helper);

  /**
   * \param col the column address of the desired edge switch
   *
   * \returns a pointer to the edge switch specified by the
   *          column address
   */
  Ptr<Node> GetEdgeSwitchNode (uint32_t col) const;

  /**
   * \param col the column address of the desired aggregate switch
   *
   * \returns a pointer to the aggregate switch specified by the column address
   */
  Ptr<Node> GetAggregateSwitchNode (uint32_t col) const;

  /**
   * \param col the column address of the desired core switch
   *
   * \returns a pointer to the core switch specified by the column address
   */
  Ptr<Node> GetCoreSwitchNode (uint32_t col) const;

  /**
   * \param col the column address of the desired server
   *
   * \returns a pointer to the server specified by the column address
   */
  Ptr<Node> GetServerNode (uint32_t col) const;

  /**
   * This returns an IPv4 address at the edge switch specified by
   * column address. Technically, an edge switch will have multiple
   * interfaces in the Fat tree; therefore, it also has multiple
   * IPv4 addresses. This method only returns one of the addresses.
   * The address being returned belongs to an interface which connects
   * the lowest index server to this switch.
   *
   * \param col the column address of the desired edge switch
   *
   * \returns Ipv4Address of one of the interfaces of the edge switch
   *          column address
   */
  Ipv4Address GetEdgeSwitchIpv4Address (uint32_t col) const;

  /**
   * This returns an IPv4 address at the aggregate switch specified by
   * column address. Technically, an aggregate switch will have multiple
   * interfaces in the Fat tree; therefore, it also has multiple IPv4
   * addresses. This method only returns one of the addresses. The address 
   * being returned belongs to an interface which connects the lowest index
   * server to this switch.
   *
   * \param col the column address of the desired aggregate switch
   *
   * \returns Ipv4Address of one of the interfaces of the aggregate switch
   *          column address
   */
  Ipv4Address GetAggregateSwitchIpv4Address (uint32_t col) const;

  /**
   * This returns an IPv4 address at the core switch specified by
   * column address. Technically, a core switch will have multiple
   * interfaces in the Fat tree; therefore, it also has multiple IPv4
   * addresses. This method only returns one of the addresses. The
   * address being returned belongs to an interface which connects the
   * lowest index server to this switch.
   *
   * \param col the column address of the desired core switch
   *
   * \returns Ipv4Address of one of the interfaces of the core switch
   *          column address
   */
  Ipv4Address GetCoreSwitchIpv4Address (uint32_t col) const;

  /**
   * This returns an IPv6 address at the edge switch specified by
   * column address. Technically, an edge switch will have multiple
   * interfaces in the Fat tree; therefore, it also has multiple
   * IPv6 addresses. This method only returns one of the addresses.
   * The address being returned belongs to an interface which connects
   * the lowest index server to this switch.
   *
   * \param col the column address of the desired edge switch
   *
   * \returns Ipv6Address of one of the interfaces of the edge switch
   *          column address
   */
  Ipv6Address GetEdgeSwitchIpv6Address (uint32_t col) const;

  /**
   * This returns an IPv6 address at the aggregate switch specified by
   * column address. Technically, an aggregate switch will have multiple
   * interfaces in the Fat tree; therefore, it also has multiple IPv6
   * addresses. This method only returns one of the addresses. The address 
   * being returned belongs to an interface which connects the lowest index
   * server to this switch.
   *
   * \param col the column address of the desired aggregate switch
   *
   * \returns Ipv6Address of one of the interfaces of the aggregate switch
   *          column address
   */
  Ipv6Address GetAggregateSwitchIpv6Address (uint32_t col) const;

  /**
   * This returns an IPv6 address at the core switch specified by
   * column address. Technically, a core switch will have multiple
   * interfaces in the Fat tree; therefore, it also has multiple IPv6
   * addresses. This method only returns one of the addresses. The
   * address being returned belongs to an interface which connects the
   * lowest index server to this switch.
   *
   * \param col the column address of the desired core switch
   *
   * \returns Ipv6Address of one of the interfaces of the core switch
   *          column address
   */
  Ipv6Address GetCoreSwitchIpv6Address (uint32_t col) const;

  /**
   * This returns an IPv4 address at the server specified by
   * the column address.
   *
   * \param col the column address of the desired server
   *
   * \returns Ipv4Address of one of the interfaces of the server
   *          specified by the column address
   */
  Ipv4Address GetServerIpv4Address (uint32_t col) const;

  /**
   * This returns an IPv6 address at the server specified by
   * the column address.
   *
   * \param col the column address of the desired server
   *
   * \returns Ipv6Address of one of the interfaces of the server
   *          specified by the column address
   */
  Ipv6Address GetServerIpv6Address (uint32_t col) const;

  /**
   * \param helperEdge the layer 2 helper which is used to install
   *                   on every edge links (server to edge switch links)
   * \param helperCore the layer 2 helper which is used to install
   *                   on every links in the network core (links between switches)
   */
  template <typename T>
  void InstallNetDevices (T helperEdge, T helperCore);

  // Inherited from the base class
  void InstallStack (InternetStackHelper& stack);

  void InstallTrafficControl (TrafficControlHelper& tchSwitch,
                              TrafficControlHelper& tchServer);

  void AssignIpv4Addresses (Ipv4Address network, Ipv4Mask mask);

  void AssignIpv6Addresses (Ipv6Address network, Ipv6Prefix prefix);

  void BoundingBox (double ulx, double uly, double lrx, double lry);

private:
  bool     m_l2Installed;                                      //!< If layer 2 components are installed
  uint32_t m_numPods;                                          //!< Number of pods
  std::vector<NetDeviceContainer> m_edgeSwitchDevices;         //!< Net Device container for edge switches and servers
  std::vector<NetDeviceContainer> m_aggregateSwitchDevices;    //!< Net Device container for aggregate switches and edge switches
  std::vector<NetDeviceContainer> m_coreSwitchDevices;         //!< Net Device container for core switches and aggregate switches
  Ipv4InterfaceContainer m_edgeSwitchInterfaces;               //!< IPv4 interfaces of edge switch
  Ipv4InterfaceContainer m_aggregateSwitchInterfaces;          //!< IPv4 interfaces of aggregate switch
  Ipv4InterfaceContainer m_coreSwitchInterfaces;               //!< IPv4 interfaces of core switch
  Ipv4InterfaceContainer m_serverInterfaces;                   //!< IPv4 interfaces of server
  Ipv6InterfaceContainer m_edgeSwitchInterfaces6;              //!< IPv6 interfaces of edge switch
  Ipv6InterfaceContainer m_aggregateSwitchInterfaces6;         //!< IPv6 interfaces of aggregate switch
  Ipv6InterfaceContainer m_coreSwitchInterfaces6;              //!< IPv6 interfaces of core switch
  Ipv6InterfaceContainer m_serverInterfaces6;                  //!< IPv6 interfaces of server
  NodeContainer m_edgeSwitches;                                //!< all the edge switches in the Fat tree
  NodeContainer m_aggregateSwitches;                           //!< all the aggregate switches in the Fat tree
  NodeContainer m_coreSwitches;                                //!< all the core switches in the Fat tree
  NodeContainer m_servers;                                     //!< all the servers in the Fat tree
};

template <typename T>
void
FatTreeHelper::InstallNetDevices (T helperEdge,
                                  T helperCore)
{
  if (m_l2Installed)
    {
      NS_FATAL_ERROR (MSG_NETDEVICES_CONFLICT);
      return;
    }  

  uint32_t numEdgeSwitches = m_numPods / 2;
  uint32_t numAggregateSwitches = m_numPods / 2;            // number of aggregate switches in a pod
  uint32_t numGroups = m_numPods / 2;                       // number of group of core switches
  uint32_t numCoreSwitches = m_numPods / 2;                 // number of core switches in a group

  // Connect servers to edge switches
  uint32_t hostId = 0;
  for (uint32_t i = 0; i < m_numPods * m_numPods / 2; i++)
    {
      for (uint32_t j = 0; j < numEdgeSwitches; j++)
        {
          NetDeviceContainer nd = helperEdge.Install (m_servers.Get (hostId), m_edgeSwitches.Get (i));
          m_edgeSwitchDevices[i].Add (nd.Get (0));
          m_edgeSwitchDevices[i].Add (nd.Get (1));
          hostId += 1;
        }
    }

  // Connect edge switches to aggregate switches
  for (uint32_t i = 0; i < m_numPods; i++)
    {
      for (uint32_t j = 0; j < numAggregateSwitches; j++)
        {
          for (uint32_t k = 0; k < numEdgeSwitches; k++)
            {
              NetDeviceContainer nd = helperCore.Install (m_edgeSwitches.Get (i * numEdgeSwitches + k),
                                                          m_aggregateSwitches.Get (i * numAggregateSwitches + j));
              m_aggregateSwitchDevices[i * numAggregateSwitches + j].Add (nd.Get (0));
              m_aggregateSwitchDevices[i * numAggregateSwitches + j].Add (nd.Get (1));
            }
        }
    }

  // Connect aggregate switches to core switches
  for (uint32_t i = 0; i < numGroups; i++)
    {
      for (uint32_t j = 0; j < numCoreSwitches; j++)
        {
          for (uint32_t k = 0; k < m_numPods; k++)
            {
              NetDeviceContainer nd = helperCore.Install (m_aggregateSwitches.Get (k * numAggregateSwitches + i),
                                                          m_coreSwitches.Get (i * numCoreSwitches + j));
              m_coreSwitchDevices[i * numCoreSwitches + j].Add (nd.Get (0));
              m_coreSwitchDevices[i * numCoreSwitches + j].Add (nd.Get (1));
            }
        }
    }
  m_l2Installed = true;
}

} // namespace ns3

#endif /* FAT_TREE_HELPER_H */
