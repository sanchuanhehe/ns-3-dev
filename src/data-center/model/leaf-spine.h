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

// Define an object to create a leaf spine topology.

#ifndef LEAF_SPINE_HELPER_H
#define LEAF_SPINE_HELPER_H

#define NUM_SPINE_MIN 1
#define NUM_LEAF_MIN 2
#define NUM_SERVER_PER_LEAF_MIN 1

#include <vector>

#include "csma-helper.h"
#include "dcn-topology.h"
#include "fatal-error.h"
#include "internet-stack-helper.h"
#include "ipv4-address-helper.h"
#include "ipv4-interface-container.h"
#include "ipv6-address-helper.h"
#include "ipv6-interface-container.h"
#include "net-device-container.h"
#include "point-to-point-helper.h"
#include "traffic-control-helper.h"

namespace ns3 {

/**
 * \ingroup data-center
 *
 * \brief A helper to make it easier to create a leaf spine topology
 */
class LeafSpineHelper : public DcnTopologyHelper
{
public:
  /**
   * Create a LeafSpineHelper in order to easily create the IP layer leaf spine topology
   *
   * \param numSpine total number of spine switches in leaf spine
   * \param numLeaf total number of leaf switches in leaf spine
   * \param numServerPerLeaf number of servers under each leaf switch in leaf spine
   */
  LeafSpineHelper (uint32_t numSpine,
                   uint32_t numLeaf,
                   uint32_t numServerPerLeaf);

  virtual ~LeafSpineHelper ();

  LeafSpineHelper (const LeafSpineHelper& helper);

  /**
   * \param col the column address of the target leaf switch
   *
   * \returns a pointer to the leaf switch specified by the column address
   */
  Ptr<Node> GetLeafNode (uint32_t col) const;

  /**
   * \param col the column address of the desired spine switch
   *
   * \returns a pointer to the spine switch specified by the column address
   */
  Ptr<Node> GetSpineNode (uint32_t col) const;

  /**
   * \param col the column address of the desired server
   *
   * \returns a pointer to the server specified by the column address
   */
  Ptr<Node> GetServerNode (uint32_t col) const;

  /**
   * This returns an IPv4 address at the spine switch specified by
   * col and the interfaceIndex. Technically, a spine switch will 
   * have multiple interfaces connected to each leaf switches in the spine leaf; 
   * therefore, it also has multiple IPv4 addresses. 
   * The interfaceIndex is marked from 0 to m_numLeaf-1 left to right according to 
   * the leaf spine diagram.
   *
   * \param col the column address of the desired spine switch
   * \param interfaceIndex the index of the desired network interface
   *
   * \returns Ipv4Address of the target interfaces of the spine switch
   */
  Ipv4Address GetSpineIpv4Address (uint32_t col, uint32_t interfaceIndex) const;

  /**
   * This returns an IPv4 address container at the spine switch specified by
   * col. Technically, a leaf switch will have multiple interfaces connected to each 
   * leaf switches in the spine leaf; therefore, it also has multiple IPv4 addresses.
   *
   * \param col the column address of the desired spine switch
   *
   * \returns Ipv4InterfaceContainer storing all interfaces for the target spine switch
   */
  Ipv4InterfaceContainer GetSpineIpv4Interfaces (uint32_t col) const;

  /**
   * This returns an IPv4 address at the leaf switch specified by
   * col and the interfaceIndex. Technically, a leaf switch will 
   * have multiple interfaces connected to each spine switches in the spine leaf
   * and each server belonging to the leaf switch; therefore, it also has multiple 
   * IPv4 addresses. The interfaceIndex is marked from 0 to m_numServerPerLeaf-1
   * for each interface connected to the servers left to right and from m_numServerPerLeaf
   * to m_numServerPerLeaf to m_numServerPerLeaf+m_numSpine-1 for each interface connected
   * to the spine switches left to right according to the leaf spine diagram.
   *
   * \param col the column address of the desired leaf switch
   * \param interfaceIndex the index of the desired network interface
   *
   * \returns Ipv4Address of the target interfaces of the leaf switch
   */  
  Ipv4Address GetLeafIpv4Address (uint32_t col, uint32_t interfaceIndex) const;

  /**
   * This returns an IPv4 address container at the leaf switch specified by
   * col. Technically, a leaf switch will have multiple interfaces connected to each 
   * spine switches in the spine leaf and each server belonging to the leaf switch; 
   * therefore, it also has multiple IPv4 addresses.
   *
   * \param col the column address of the desired leaf switch
   *
   * \returns Ipv4InterfaceContainer storing all interfaces for the target leaf switch
   */
  Ipv4InterfaceContainer GetLeafIpv4Interfaces (uint32_t col) const;

  /**
   * This returns an IPv4 address at the server specified by
   * the col. There is only one interface for each server 
   * connected to the leaf switch (a.k.a. ToR switch).
   *
   * \param col the column address of the desired server
   *
   * \returns Ipv4Address of the target server
   */
  Ipv4Address GetServerIpv4Address (uint32_t col) const;

  /**
   * This returns an IPv6 address at the spine switch specified by
   * col and the interfaceIndex. Technically, a spine switch will 
   * have multiple interfaces connected to each leaf switches in the spine leaf; 
   * therefore, it also has multiple IPv6 addresses. 
   * The interfaceIndex is marked from 0 to m_numLeaf-1 left to right according 
   * to the leaf spine diagram.
   *
   * \param col the column address of the desired spine switch
   * \param interfaceIndex the index of the desired network interface
   *
   * \returns Ipv6Address of the target interfaces of the spine switch
   */
  Ipv6Address GetSpineIpv6Address (uint32_t col, uint32_t interfaceIndex) const;

  /**
   * This returns an IPv6 address container at the spine switch specified by
   * col. Technically, a leaf switch will have multiple interfaces connected to each 
   * leaf switches in the spine leaf; therefore, it also has multiple IPv6 addresses.
   *
   * \param col the column address of the desired spine switch
   *
   * \returns Ipv6InterfaceContainer storing all interfaces for the target spine switch
   */
  Ipv6InterfaceContainer GetSpineIpv6Interfaces (uint32_t col) const;

  /**
   * This returns an IPv6 address at the leaf switch specified by
   * col and the interfaceIndex. Technically, a leaf switch will 
   * have multiple interfaces connected to each spine switches in the spine leaf
   * and each server belonging to the leaf switch; therefore, it also has multiple 
   * IPv6 addresses. The interfaceIndex is marked from 0 to m_numServerPerLeaf-1
   * for each interface connected to the servers left to right and from m_numServerPerLeaf
   * to m_numServerPerLeaf to m_numServerPerLeaf+m_numSpine-1 for each interface connected
   * to the spine switches left to right according to the leaf spine diagram.
   *
   * \param col the column address of the desired leaf switch
   * \param interfaceIndex the index of the desired network interface
   *
   * \returns Ipv6Address of the target interfaces of the leaf switch
   */
  Ipv6Address GetLeafIpv6Address (uint32_t col, uint32_t interfaceIndex) const;

  /**
   * This returns an IPv6 address container at the leaf switch specified by
   * col. Technically, a leaf switch will have multiple interfaces connected to each 
   * spine switches in the spine leaf and each server belonging to the leaf switch; 
   * therefore, it also has multiple IPv6 addresses.
   *
   * \param col the column address of the desired leaf switch
   *
   * \returns Ipv6InterfaceContainer storing all interfaces for the target leaf switch
   */
  Ipv6InterfaceContainer GetLeafIpv6Interfaces (uint32_t col) const;

  /**
   * This returns an IPv6 address at the server specified by
   * the col. There is only one interface for each server 
   * connected to the leaf switch (a.k.a. ToR switch).
   *
   * \param col the column address of the desired server
   *
   * \returns Ipv6Address of the target server
   */
  Ipv6Address GetServerIpv6Address (uint32_t col) const;

  /**
   * \returns total number of spine switches
   */
  uint32_t  SpineCount () const;

  /**
   * \returns total number of leaf switches
   */
  uint32_t  LeafCount () const;

  /**
   * \returns total number of servers
   */
  uint32_t  ServerCount () const;

  /**
   * \returns total number of nodes
   */
  uint32_t  TotalCount () const;

  /**
   * \param helperEdge the layer 2 helper which is used to install
   *                   on every server to leaf switch links
   * \param helperCore the layer 2 helper which is used to install
   *                   on every links between spine switches and leaf switches
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
  bool     m_l2Installed;                            //!< If layer 2 components are installed
  uint32_t m_numSpine;                               //!< Number of spine switches
  uint32_t m_numLeaf;                                //!< Number of leaf switches
  uint32_t m_numServerPerLeaf;                       //!< Number of servers per leaf switch
  std::vector<NetDeviceContainer> m_spineDevices;    //!< Net Device container for each spine switch
  std::vector<NetDeviceContainer> m_leafDevices;     //!< Net Device container for each leaf switch
  std::vector<NetDeviceContainer> m_serverDevices;   //!< Net Device container for each server
  NodeContainer m_spineSwitches;                     //!< Node container for all spine switches
  NodeContainer m_leafSwitches;                      //!< Node container for all leaf switches
  NodeContainer m_servers;                           //!< Node container for all servers
  std::vector<Ipv4InterfaceContainer> m_spineInterfaces;         //!< IPv4 interfaces of each spine switch
  std::vector<Ipv4InterfaceContainer> m_leafInterfaces;          //!< IPv4 interfaces of each leaf switch
  std::vector<Ipv4InterfaceContainer> m_serverInterfaces;        //!< IPv4 interfaces of each server
  std::vector<Ipv6InterfaceContainer> m_spineInterfaces6;        //!< IPv6 interfaces of each spine switch
  std::vector<Ipv6InterfaceContainer> m_leafInterfaces6;         //!< IPv6 interfaces of each leaf switch
  std::vector<Ipv6InterfaceContainer> m_serverInterfaces6;       //!< IPv6 interfaces of each server

};

template <typename T>
void
LeafSpineHelper::InstallNetDevices (T helperEdge,
                                    T helperCore)
{
  if (m_l2Installed)
    {
      NS_FATAL_ERROR (MSG_NETDEVICES_CONFLICT);
      return;
    }   

  /*
    There are four types of NetDevice:
    - Server towards the leaf switch (ToR switch)
    - Leaf switch towards servers
    - Leaf switch towards spine switches
    - Spine switch towards leaf switches
   */
  // Connect servers to leaf switches
  uint32_t serverId = 0;
  for (uint32_t i = 0; i < m_numLeaf; i++)
    {
      for (uint32_t j = 0; j < m_numServerPerLeaf; j++)
        {
          NetDeviceContainer ndc = helperEdge.Install (m_servers.Get (serverId), m_leafSwitches.Get (i));
          m_serverDevices[serverId].Add (ndc.Get (0));
          m_leafDevices[i].Add (ndc.Get (1));
          serverId += 1;
        }
    }

  // Connect leaf switches to spine switches in a complete bipartie graph
  for (uint32_t i = 0; i < m_numSpine; i++)
    {
      for (uint32_t j = 0; j < m_numLeaf; j++)
        {
            NetDeviceContainer ndc = helperCore.Install (m_leafSwitches.Get (j),
                                                           m_spineSwitches.Get (i));
            m_leafDevices[j].Add (ndc.Get (0));
            m_spineDevices[i].Add (ndc.Get (1));
        }
    }
  m_l2Installed = true;
}

} // namespace ns3

#endif /* LEAF_SPINE_HELPER_H */
