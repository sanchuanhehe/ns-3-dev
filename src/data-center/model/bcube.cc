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

// Implement an object to create a BCube topology.

#include "ns3/bcube.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-generator.h"
#include "ns3/ipv6-address-generator.h"
#include "ns3/log.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/string.h"
#include "ns3/vector.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("BCubeHelper");

BCubeHelper::BCubeHelper (uint32_t nLevels,
                          uint32_t nServers)
  : m_l2Installed (false),
    m_numLevels (nLevels),
    m_numServers (nServers)
{
  // Bounds check
  if (nServers < N_SERVER_MIN)
    {
      NS_FATAL_ERROR ("Insufficient number of servers for BCube.");
    }

  m_numLevelSwitches = pow (nServers, nLevels);
  m_levelSwitchDevices.resize ((nLevels + 1) * m_numLevelSwitches);
  m_switchInterfaces.resize ((nLevels + 1) * m_numLevelSwitches);

  // Number of servers = pow (nServers, nLevels + 1)
  //                   = nServers * pow (nServers, nLevels)
  //                   = nServers * numLevelSwitches
  m_servers.Create (nServers * m_numLevelSwitches);

  // Number of switches = (nLevels + 1) * pow (nServers, nLevels)
  //                    = (nLevels + 1) * numLevelSwitches
  m_switches.Create ((nLevels + 1) * m_numLevelSwitches);
}

BCubeHelper::~BCubeHelper ()
{
}

BCubeHelper::BCubeHelper (const BCubeHelper& helper)
: DcnTopologyHelper (helper),
  m_l2Installed (helper.m_l2Installed),
  m_numLevels (helper.m_numLevels),
  m_numServers (helper.m_numServers),
  m_numLevelSwitches (helper.m_numLevelSwitches),
  m_serverInterfaces (helper.m_serverInterfaces),
  m_serverInterfaces6 (helper.m_serverInterfaces6),
  m_switches (helper.m_switches),
  m_servers (helper.m_servers)  
{
  m_levelSwitchDevices = helper.m_levelSwitchDevices; 
  m_switchInterfaces = helper.m_switchInterfaces;  
  m_switchInterfaces6 = helper.m_switchInterfaces6; 
}

void
BCubeHelper::InstallStack (InternetStackHelper& stack)
{
  if (!m_l2Installed)
    {
      NS_LOG_WARN (MSG_NETDEVICES_MISSING);
    }
  
  stack.Install (m_servers);
  stack.Install (m_switches);
}

void
BCubeHelper::InstallTrafficControl (TrafficControlHelper& tchSwitch,
                                    TrafficControlHelper& tchServer)
{
  if (!m_l2Installed)
    {
      NS_LOG_WARN (MSG_NETDEVICES_MISSING);
    }

  for (uint32_t i = 0; i < m_levelSwitchDevices.size (); ++i)
    {
      for (uint32_t j = 0; j < m_levelSwitchDevices[i].GetN (); j += 2)
        {
          tchServer.Install (m_levelSwitchDevices[i].Get (j));
          tchSwitch.Install (m_levelSwitchDevices[i].Get (j + 1));
        }
    }        
}

void
BCubeHelper::BoundingBox (double ulx, double uly,
                          double lrx, double lry)
{
  NS_LOG_FUNCTION (this << ulx << uly << lrx << lry);
  double xDist;
  double yDist;
  if (lrx > ulx)
    {
      xDist = lrx - ulx;
    }
  else
    {
      xDist = ulx - lrx;
    }
  if (lry > uly)
    {
      yDist = lry - uly;
    }
  else
    {
      yDist = uly - lry;
    }

  uint32_t val = pow (m_numServers, m_numLevels);
  uint32_t numServers = val * m_numServers;
  double xServerAdder = xDist / numServers;
  double xSwitchAdder = m_numServers * xServerAdder;
  double yAdder = yDist / (m_numLevels + 2);

  // Place the servers
  double xLoc = 0.0;
  double yLoc = yDist;
  for (uint32_t i = 0; i < numServers; ++i)
    {
      Ptr<Node> node = m_servers.Get (i);
      Ptr<ConstantPositionMobilityModel> loc = node->GetObject<ConstantPositionMobilityModel> ();
      if (loc == 0)
        {
          loc = CreateObject<ConstantPositionMobilityModel> ();
          node->AggregateObject (loc);
        }
      Vector locVec (xLoc, yLoc, 0);
      loc->SetPosition (locVec);
      xLoc += 2 * xServerAdder;
    }

  yLoc -= yAdder;

  // Place the switches
  for (uint32_t i = 0; i < m_numLevels + 1; ++i)
    {
      if (m_numServers % 2 == 0)
        {
          xLoc = xSwitchAdder / 2 + xServerAdder;
        }
      else
        {
          xLoc = xSwitchAdder / 2 + xServerAdder / 2;
        }
      for (uint32_t j = 0; j < val; ++j)
        {
          Ptr<Node> node = m_switches.Get (i * val + j);
          Ptr<ConstantPositionMobilityModel> loc = node->GetObject<ConstantPositionMobilityModel> ();
          if (loc == 0)
            {
              loc = CreateObject<ConstantPositionMobilityModel> ();
              node->AggregateObject (loc);
            }
          Vector locVec (xLoc, yLoc, 0);
          loc->SetPosition (locVec);

          xLoc += 2 * xSwitchAdder;
        }
      yLoc -= yAdder;
    }
}

void
BCubeHelper::AssignIpv4Addresses (Ipv4Address network, Ipv4Mask mask)
{
  if (!m_l2Installed)
    {
      NS_LOG_WARN (MSG_NETDEVICES_MISSING);
    }

  NS_LOG_FUNCTION (this << network << mask);
  Ipv4AddressGenerator::Init (network, mask);
  Ipv4Address v4network;
  Ipv4AddressHelper addrHelper;

  for (uint32_t i = 0; i < m_levelSwitchDevices.size (); ++i)
    {
      v4network = Ipv4AddressGenerator::NextNetwork (mask);
      addrHelper.SetBase (v4network, mask);
      for (uint32_t j = 0; j < m_levelSwitchDevices[i].GetN (); j += 2)
        {
          Ipv4InterfaceContainer ic = addrHelper.Assign (m_levelSwitchDevices[i].Get (j));
          m_serverInterfaces.Add (ic);
          ic = addrHelper.Assign (m_levelSwitchDevices[i].Get (j + 1));
          m_switchInterfaces[i].Add (ic);
        }
    }
}

void
BCubeHelper::AssignIpv6Addresses (Ipv6Address addrBase, Ipv6Prefix prefix)
{
  if (!m_l2Installed)
    {
      NS_LOG_WARN (MSG_NETDEVICES_MISSING);
    }

  NS_LOG_FUNCTION (this << addrBase << prefix);
  Ipv6AddressGenerator::Init (addrBase, prefix);
  Ipv6Address v6network;
  Ipv6AddressHelper addrHelper;

  for (uint32_t i = 0; i < m_levelSwitchDevices.size (); ++i)
    {
      v6network = Ipv6AddressGenerator::NextNetwork (prefix);
      addrHelper.SetBase (v6network, prefix);
      for (uint32_t j = 0; j < m_levelSwitchDevices[i].GetN (); j += 2)
        {
          Ipv6InterfaceContainer ic = addrHelper.Assign (m_levelSwitchDevices[i].Get (j));
          m_serverInterfaces6.Add (ic);
          ic = addrHelper.Assign (m_levelSwitchDevices[i].Get (j + 1));
          m_switchInterfaces6[i].Add (ic);
        }
    }
}

Ipv4Address
BCubeHelper::GetServerIpv4Address (uint32_t col) const
{
  NS_LOG_FUNCTION (this << col);
  return m_serverInterfaces.GetAddress (col);
}

Ipv4Address
BCubeHelper::GetSwitchIpv4Address (uint32_t row, uint32_t col) const
{
  NS_LOG_FUNCTION (this << row << col);
  return m_switchInterfaces[row].GetAddress (col);
}

Ipv6Address
BCubeHelper::GetServerIpv6Address (uint32_t col) const
{
  NS_LOG_FUNCTION (this << col);
  return m_serverInterfaces6.GetAddress (col, 1);
}

Ipv6Address
BCubeHelper::GetSwitchIpv6Address (uint32_t row, uint32_t col) const
{
  NS_LOG_FUNCTION (this << row << col);
  return m_switchInterfaces6[row].GetAddress (col, 1);
}

Ptr<Node>
BCubeHelper::GetServerNode (uint32_t col) const
{
  NS_LOG_FUNCTION (this << col);
  return m_servers.Get (col);
}

Ptr<Node>
BCubeHelper::GetSwitchNode (uint32_t row, uint32_t col) const
{
  NS_LOG_FUNCTION (this << row << col);
  uint32_t numLevelSwitches = pow (m_numServers, m_numLevels);
  return m_switches.Get (row * numLevelSwitches + col);
}

} // namespace ns3
