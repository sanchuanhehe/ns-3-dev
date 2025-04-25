/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2019 Liangcheng Yu
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

// Define an object to create a DCell topology.

#ifndef DCELL_HELPER_H
#define DCELL_HELPER_H

#define N_SERVER_MIN 1

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
 * \brief A helper to make it easier to create a DCell topology
 */
class DCellHelper : public DcnTopologyHelper
{
public:
  /**
   * Create a DCellHelper in order to easily create DCell topologies
   *
   * \param nLevels total number of levels in DCell
   * \param nServers total number of servers in DCell0
   * 
   */
  DCellHelper (uint32_t nLevels,
               uint32_t nServers);

  virtual ~DCellHelper ();

  DCellHelper (const DCellHelper& helper);

  /**
   * \param index the index of the desired mini-switch for the corresponding DCell0
   *
   * \returns a pointer to the switch specified by the index
   */
  Ptr<Node> GetSwitchNode (uint32_t index) const;

  /**
   * \param uid the unique ID of the desired server
   *
   * \returns a pointer to the server specified by the uid
   */
  Ptr<Node> GetServerNode (uint32_t uid) const;

  /**
   * This returns a set of IPv4 interfaces of the mini-switch specified by
   * the index address. Technically, a mini-switch will have
   * m_numServersDCell0 interfaces in each DCell; therefore, it also has
   * m_numServersDCell0 IPv4 addresses. This method returns the Ipv4InterfaceContainer
   * for all the interfaces towards the servers in the DCell0. 
   *
   * \param index the index of the desired mini-switch for the corresponding DCell0
   *
   * \returns Ipv4InterfaceContainer containing all the interfaces facing the servers 
   * inside the DCell0
   */
  Ipv4InterfaceContainer GetSwitchIpv4Interfaces (uint32_t index) const;

  /**
   * This returns a set of IPv6 interfaces of the mini-switch specified by
   * the index address. Technically, a mini-switch will have
   * m_numServersDCell0 interfaces in each DCell; therefore, it also has
   * m_numServersDCell0 IPv6 addresses. This method returns the Ipv6InterfaceContainer
   * for all the interfaces towards the servers in the DCell0. 
   *
   * \param index the index of the desired mini-switch for the corresponding DCell0
   *
   * \returns Ipv6InterfaceContainer containing all the interfaces facing the servers 
   * inside the DCell0
   */
  Ipv6InterfaceContainer GetSwitchIpv6Interfaces (uint32_t index) const;

  /**
   * This returns a set of IPv4 interfaces at the server specified by
   * the uid. Technically, a server will have m_numLevels+1 
   * interfaces where the first interface connects to the mini-switch, 
   * the second interface connects to another DCell0 server inside the DCell1,
   * the thirde interface connects to another DCell1 server inside the DCell2
   * and so forth. Therefore, it also has m_numLevels+1
   * IPv4 addresses. This method returns the Ipv4InterfaceContainer
   * for all the interfaces towards the mini-switch and the servers.
   *
   * \param uid the unique id of the server
   *
   * \returns Ipv4InterfaceContainer of the server specified by uid containing
   * the interface towards the mini-switch inside DCell0 and m_numLevels interfaces
   * towards the servers to form the full-mesh graph at different DCell levels
   */
  Ipv4InterfaceContainer GetServerIpv4Interfaces (uint32_t uid) const;

  /**
   * This returns a set of IPv6 interfaces at the server specified by
   * the uid. Technically, a server will have m_numLevels+1 
   * interfaces where the first interface connects to the mini-switch, 
   * the second interface connects to another DCell0 server inside the DCell1,
   * the thirde interface connects to another DCell1 server inside the DCell2
   * and so forth. Therefore, it also has m_numLevels+1
   * IPv6 addresses. This method returns the Ipv6InterfaceContainer
   * for all the interfaces towards the mini-switch and the servers.
   *
   * \param uid the unique id of the server
   *
   * \returns Ipv6InterfaceContainer of the server specified by uid containing
   * the interface towards the mini-switch inside DCell0 and m_numLevels interfaces
   * towards the servers to form the full-mesh graph at different DCell levels
   */
  Ipv6InterfaceContainer GetServerIpv6Interfaces (uint32_t uid) const;

  /**
   * This returns an IPv4 address at the server specified by
   * the uid and level. Each server connets to the mini-switch (level 0)
   * and the other servers to form the full mesh graph at different
   * DCell levels.
   *
   * \param uid the unique id of the desired server
   * \param level the level of the target interface
   * 
   * \returns Ipv4Address of the target interface of the target server
   */
  Ipv4Address GetServerIpv4Address (uint32_t uid, uint32_t level) const;

  /**
   * This returns an IPv6 address at the server specified by
   * the uid and level. Each server connets to the mini-switch (level 0)
   * and the other servers to form the full mesh graph at different
   * DCell levels.
   *
   * \param uid the unique id of the desired server
   * \param level the level of the target interface
   * 
   * \returns Ipv6Address of the target interface of the target server
   */
  Ipv6Address GetServerIpv6Address (uint32_t uid, uint32_t level) const;

  /**
   * This returns an IPv4 address at the mini-switch specified by
   * the index and serverId. Each mini-switch connets to m_numServersDCell0 servers,
   * therefore, there are m_numServersDCell0 interfaces inside DCell0 grouped 
   * by the mini-switch.
   *
   * \param index the index of the target mini-switch
   * \param serverId the server index inside DCell0
   * 
   * \returns Ipv4Address of the target interface of the target server.
   */
  Ipv4Address GetSwitchIpv4Address (uint32_t index, uint32_t serverId) const;

  /**
   * This returns an IPv6 address at the mini-switch specified by
   * the index and serverId. Each mini-switch connets to m_numServersDCell0 servers,
   * therefore, there are m_numServersDCell0 interfaces inside DCell0 grouped 
   * by the mini-switch.
   *
   * \param index the index of the target mini-switch
   * \param serverId the server index inside DCell0
   * 
   * \returns Ipv6Address of the target interface of the target server.
   */
  Ipv6Address GetSwitchIpv6Address (uint32_t index, uint32_t serverId) const;

  /**
   * \param helper the layer 2 helper which is used to install
   *               on every links in the DCell
   */
  template <typename T>
  void InstallNetDevices (T helper);

  // Inherited from the base class
  void InstallStack (InternetStackHelper& stack);

  void InstallTrafficControl (TrafficControlHelper& tchSwitch,
                              TrafficControlHelper& tchServer);

  void AssignIpv4Addresses (Ipv4Address network, Ipv4Mask mask);

  void AssignIpv6Addresses (Ipv6Address network, Ipv6Prefix prefix);

  void BoundingBox (double ulx, double uly, double lrx, double lry);

private:
  bool     m_l2Installed;                                       //!< If layer 2 components are installed
  uint32_t m_numLevels;                                         //!< number of levels
  uint32_t m_numServersDCell0;                                  //!< number of servers at Dcell0
  NodeContainer m_servers;                                      //!< all the servers in the DCell
  NodeContainer m_switches;                                     //!< all the switches in the DCell
  std::vector<NetDeviceContainer> m_serverDevices;              //!< Net Device container for servers
  std::vector<NetDeviceContainer> m_switchDevices;              //!< Net Device container for switches
  std::vector<Ipv4InterfaceContainer> m_switchInterfaces;       //!< IPv4 interfaces of switches
  std::vector<Ipv4InterfaceContainer> m_serverInterfaces;       //!< IPv4 interfaces of servers
  std::vector<Ipv6InterfaceContainer> m_switchInterfaces6;      //!< IPv6 interfaces of switches
  std::vector<Ipv6InterfaceContainer> m_serverInterfaces6;      //!< IPv6 interfaces of servers
  std::vector<uint32_t> m_numServersByLevel;                    //!< number of servers for each DCell level

};

template <typename T>
void
DCellHelper::InstallNetDevices (T helper)
{
  if (m_l2Installed)
    {
      NS_FATAL_ERROR (MSG_NETDEVICES_CONFLICT);
      return;
    }  

  // Connect servers to the mini-switches
  uint32_t numDcell0 = m_numServersByLevel[m_numLevels]/m_numServersDCell0;
  for (uint32_t switchId = 0; switchId < numDcell0; switchId++)
    {
      for (uint32_t serverId = 0; serverId < m_numServersDCell0; serverId++)
        {
          NetDeviceContainer nd = helper.Install (m_servers.Get (switchId*m_numServersDCell0+serverId),
                                                  m_switches.Get (switchId));
          m_serverDevices[switchId*m_numServersDCell0+serverId].Add (nd.Get (0));
          m_switchDevices[switchId].Add (nd.Get (1));
        }
    }
  
  // Connect DCells of different levels with server to server links
  uint32_t numDcells = 0;
  uint32_t numServers = 0;
  for (uint32_t level = 1; level <= m_numLevels; level++)
    {
      // Number of servers for each DCell at level-1
      numServers = m_numServersByLevel[level-1];
      // Number of DCells at level-1
      numDcells = m_numServersByLevel[m_numLevels] / numServers;
      for (uint32_t i = 0; i < numDcells; i++)
        {
          for (uint32_t j = i+1; j < numDcells; j++)
            {
              NetDeviceContainer nd = helper.Install (m_servers.Get (i*numServers+j-1),
                                                      m_servers.Get (j*numServers+i));
              m_serverDevices[i*numServers+j-1].Add (nd.Get(0));
              m_serverDevices[j*numServers+i].Add (nd.Get(1));                                        
            }
        }
    }

  m_l2Installed = true;
}

} // namespace ns3

#endif /* DCELL_HELPER_H */
