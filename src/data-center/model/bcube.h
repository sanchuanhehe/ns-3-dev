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

// Define an object to create a BCube topology.

#ifndef BCUBE_HELPER_H
#define BCUBE_HELPER_H

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
 * \brief A helper to make it easier to create a BCube topology
 */
class BCubeHelper : public DcnTopologyHelper
{
public:
  /**
   * Create a BCubeHelper in order to easily create BCube topologies
   *
   * \param nLevels total number of levels in BCube
   * \param nServers total number of servers in one BCube
   * 
   */
  BCubeHelper (uint32_t nLevels,
               uint32_t nServers);

  virtual ~BCubeHelper ();

  BCubeHelper (const BCubeHelper& helper);

  /**
   * \param row the row address of the desired switch
   * \param col the column address of the desired switch
   *
   * \returns a pointer to the switch specified by the
   *          (row, col) address
   */
  Ptr<Node> GetSwitchNode (uint32_t row, uint32_t col) const;

  /**
   * \param col the column address of the desired server
   *
   * \returns a pointer to the server specified by the
   *          column address
   */
  Ptr<Node> GetServerNode (uint32_t col) const;

  /**
   * This returns an IPv4 address of the switch specified by
   * the (row, col) address. Technically, a switch will have
   * multiple interfaces in the BCube; therefore, it also has
   * multiple IPv4 addresses. This method only returns one of
   * the addresses. The address being returned belongs to an
   * interface which connects the lowest index server to this
   * switch.
   *
   * \param row the row address of the desired switch
   *
   * \param col the column address of the desired switch
   *
   * \returns Ipv4Address of one of the interfaces of the switch
   *          specified by the (row, col) address
   */
  Ipv4Address GetSwitchIpv4Address (uint32_t row, uint32_t col) const;

  /**
   * This returns an IPv6 address at the switch specified by
   * the (row, col) address. Technically, a switch will have
   * multiple interfaces in the BCube; therefore, it also has
   * multiple IPv6 addresses. This method only returns one of
   * the addresses. The address being returned belongs to an
   * interface which connects the lowest index server to this
   * switch.
   *
   * \param row the row address of the desired switch
   *
   * \param col the column address of the desired switch
   *
   * \returns Ipv6Address of one of the interfaces of the switch
   *          specified by the (row, col) address
   */
  Ipv6Address GetSwitchIpv6Address (uint32_t row, uint32_t col) const;

  /**
   * This returns an IPv4 address at the server specified by
   * the col address. Technically, a server will have multiple
   * interfaces in the BCube; therefore, it also has multiple
   * IPv4 addresses. This method only returns one of the addresses.
   * The address being returned belongs to an interface which
   * connects the lowest level switch to this server.
   *
   * \param col the column address of the desired server
   *
   * \returns Ipv4Address of one of the interfaces of the server
   *          specified by the column address
   */
  Ipv4Address GetServerIpv4Address (uint32_t col) const;

  /**
   * This returns an IPv6 address at the server specified by
   * the col address. Technically, a server will have multiple
   * interfaces in the BCube; therefore, it also has multiple
   * IPv6 addresses. This method only returns one of the addresses.
   * The address being returned belongs to an interface which
   * connects the lowest level switch to this server.
   *
   * \param col the column address of the desired server
   *
   * \returns Ipv6Address of one of the interfaces of the server
   *          specified by the column address
   */
  Ipv6Address GetServerIpv6Address (uint32_t col) const;

  /**
   * \param helper the layer 2 helper which is used to install
   *               on every server to switch links in the BCube
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
  uint32_t m_numLevels;                                         //!< number of levels (k)
  uint32_t m_numServers;                                        //!< number of servers (n)
  uint32_t m_numLevelSwitches;                                  //!< number of level switches
  std::vector<NetDeviceContainer> m_levelSwitchDevices;         //!< Net Device container for servers and switches
  std::vector<Ipv4InterfaceContainer> m_switchInterfaces;       //!< IPv4 interfaces of switch
  Ipv4InterfaceContainer m_serverInterfaces;                    //!< IPv4 interfaces of server
  std::vector<Ipv6InterfaceContainer> m_switchInterfaces6;      //!< IPv6 interfaces of switch
  Ipv6InterfaceContainer m_serverInterfaces6;                   //!< IPv6 interfaces of server
  NodeContainer m_switches;                                     //!< all the switches in the BCube
  NodeContainer m_servers;                                      //!< all the servers in the BCube
};

template <typename T>
void
BCubeHelper::InstallNetDevices (T helper)
{
  if (m_l2Installed)
    {
      NS_FATAL_ERROR (MSG_NETDEVICES_CONFLICT);
      return;
    }  

  uint32_t switchColId;
  // Configure the levels in BCube topology
  for (uint32_t level = 0; level < m_numLevels + 1; level++)
    {
      switchColId = 0;
      uint32_t val1 = pow (m_numServers, level);
      uint32_t val2 = val1 * m_numServers;
      // Configure the position of switches at each level of the topology
      for (uint32_t switches = 0; switches < m_numLevelSwitches; switches++)
        {
          uint32_t serverIndex = switches % val1 + switches / val1 * val2;
          // Connect nServers to every switch
          for (uint32_t servers = serverIndex; servers < (serverIndex + val2); servers += val1)
            {
              NetDeviceContainer nd = helper.Install (m_servers.Get (servers),
                                                      m_switches.Get (level * m_numLevelSwitches + switchColId));
              m_levelSwitchDevices[level * m_numLevelSwitches + switchColId].Add (nd.Get (0));
              m_levelSwitchDevices[level * m_numLevelSwitches + switchColId].Add (nd.Get (1));
            }
          switchColId += 1;
        }
    }
  m_l2Installed = true;
}

} // namespace ns3

#endif /* BCUBE_HELPER_H */
