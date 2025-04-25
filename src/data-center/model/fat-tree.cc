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

// Implement an object to create a Fat tree topology.

#include "ns3/string.h"
#include "ns3/vector.h"
#include "ns3/log.h"

#include "ns3/fat-tree.h"
#include "ns3/object.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-generator.h"
#include "ns3/ipv6-address-generator.h"
#include "ns3/constant-position-mobility-model.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("FatTreeHelper");

FatTreeHelper::FatTreeHelper (uint32_t numPods)
  : m_l2Installed (false),
    m_numPods (numPods)
{
  // Bounds check
  if (numPods == 0)
    {
      NS_FATAL_ERROR ("Need more pods for FatTree.");
    }
  if (numPods % 2 != 0)
    {
      NS_FATAL_ERROR ("Number of pods should be even in FatTree.");
    }
  uint32_t numEdgeSwitches = m_numPods / 2;
  uint32_t numAggregateSwitches = m_numPods / 2;            // number of aggregate switches in a pod
  uint32_t numGroups = m_numPods / 2;                       // number of group of core switches
  uint32_t numCoreSwitches = m_numPods / 2;                 // number of core switches in a group
  uint32_t numServers = m_numPods * m_numPods * m_numPods / 4;  // number of servers in the entire network
  m_edgeSwitchDevices.resize (m_numPods * numEdgeSwitches);
  m_aggregateSwitchDevices.resize (m_numPods * numAggregateSwitches);
  m_coreSwitchDevices.resize (numGroups * numCoreSwitches);

  m_servers.Create (numServers);
  m_edgeSwitches.Create (numEdgeSwitches * m_numPods);
  m_aggregateSwitches.Create (numAggregateSwitches * m_numPods);
  m_coreSwitches.Create (numCoreSwitches * numGroups);    
}

FatTreeHelper::FatTreeHelper (const FatTreeHelper& helper)
: DcnTopologyHelper (helper),
  m_l2Installed (helper.m_l2Installed),
  m_numPods (helper.m_numPods),
  m_edgeSwitchInterfaces (helper.m_edgeSwitchInterfaces),
  m_aggregateSwitchInterfaces (helper.m_aggregateSwitchInterfaces),
  m_coreSwitchInterfaces (helper.m_coreSwitchInterfaces),
  m_serverInterfaces (helper.m_serverInterfaces),
  m_edgeSwitchInterfaces6 (helper.m_edgeSwitchInterfaces6),
  m_aggregateSwitchInterfaces6 (helper.m_aggregateSwitchInterfaces6),
  m_coreSwitchInterfaces6 (helper.m_coreSwitchInterfaces6),
  m_serverInterfaces6 (helper.m_serverInterfaces6),
  m_edgeSwitches (helper.m_edgeSwitches),
  m_aggregateSwitches (helper.m_aggregateSwitches),
  m_coreSwitches (helper.m_coreSwitches),
  m_servers (helper.m_servers)  
{
  m_edgeSwitchDevices = helper.m_edgeSwitchDevices; 
  m_aggregateSwitchDevices = helper.m_aggregateSwitchDevices;  
  m_coreSwitchDevices = helper.m_coreSwitchDevices; 
}

FatTreeHelper::~FatTreeHelper ()
{
}

void
FatTreeHelper::InstallStack (InternetStackHelper& stack)
{
  if (!m_l2Installed)
    {
      NS_LOG_WARN (MSG_NETDEVICES_MISSING);
    }

  stack.Install (m_servers);
  stack.Install (m_edgeSwitches);
  stack.Install (m_aggregateSwitches);
  stack.Install (m_coreSwitches);
}

void
FatTreeHelper::InstallTrafficControl (TrafficControlHelper& tchSwitch,
                                      TrafficControlHelper& tchServer)
{
  if (!m_l2Installed)
    {
      NS_LOG_WARN (MSG_NETDEVICES_MISSING);
    }

  for (uint32_t i = 0; i < m_edgeSwitchDevices.size (); ++i)
    {
      for (uint32_t j = 0; j < m_edgeSwitchDevices[i].GetN (); j += 2)
        {
          tchServer.Install (m_edgeSwitchDevices[i].Get (j));
          tchSwitch.Install (m_edgeSwitchDevices[i].Get (j + 1));
        }
    }    
  for (std::vector<NetDeviceContainer>::iterator it = m_coreSwitchDevices.begin() ; it != m_coreSwitchDevices.end(); it++)
    {
      tchServer.Install (*it);
    } 

  for (std::vector<NetDeviceContainer>::iterator it = m_edgeSwitchDevices.begin() ; it != m_edgeSwitchDevices.end(); it++)
    {
      tchSwitch.Install (*it);
    }
}

void
FatTreeHelper::BoundingBox (double ulx, double uly,
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

  uint32_t numServers = m_numPods * m_numPods * m_numPods / 4;
  uint32_t numSwitches = m_numPods * m_numPods / 2;

  double xServerAddr = xDist / numServers;
  double xEdgeSwitchAddr = xDist / numSwitches;
  double xAggregateSwitchAddr = xDist / numSwitches;
  double xCoreSwitchAddr = xDist / (numSwitches / 2);
  double yAddr = yDist / 4;  // 3 layers of switches and 1 layer of servers

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
      if (i % 2 == 0)
        {
          xLoc += 3 * xServerAddr;
        }
      else
        {
          xLoc += 1.1 * xServerAddr;
        }
    }

  yLoc -= yAddr;

  // Place the edge switches
  xLoc = xEdgeSwitchAddr;
  for (uint32_t i = 0; i < numSwitches; ++i)
    {
      Ptr<Node> node = m_edgeSwitches.Get (i);
      Ptr<ConstantPositionMobilityModel> loc = node->GetObject<ConstantPositionMobilityModel> ();
      if (loc == 0)
        {
          loc = CreateObject<ConstantPositionMobilityModel> ();
          node->AggregateObject (loc);
        }
      Vector locVec (xLoc, yLoc, 0);
      loc->SetPosition (locVec);
      xLoc += 2 * xEdgeSwitchAddr;
    }

  yLoc -= yAddr;

  // Place the aggregate switches
  xLoc = xAggregateSwitchAddr;
  for (uint32_t i = 0; i < numSwitches; ++i)
    {
      Ptr<Node> node = m_aggregateSwitches.Get (i);
      Ptr<ConstantPositionMobilityModel> loc = node->GetObject<ConstantPositionMobilityModel> ();
      if (loc == 0)
        {
          loc = CreateObject<ConstantPositionMobilityModel> ();
          node->AggregateObject (loc);
        }
      Vector locVec (xLoc, yLoc, 0);
      loc->SetPosition (locVec);
      xLoc += 2 * xAggregateSwitchAddr;
    }

  yLoc -= yAddr;

  // Place the core switches
  xLoc = xCoreSwitchAddr;
  for (uint32_t i = 0; i < numSwitches / 2; ++i)
    {
      Ptr<Node> node = m_coreSwitches.Get (i);
      Ptr<ConstantPositionMobilityModel> loc = node->GetObject<ConstantPositionMobilityModel> ();
      if (loc == 0)
        {
          loc = CreateObject<ConstantPositionMobilityModel> ();
          node->AggregateObject (loc);
        }
      Vector locVec (xLoc, yLoc, 0);
      loc->SetPosition (locVec);
      xLoc += 2 * xCoreSwitchAddr;
    }
}

void
FatTreeHelper::AssignIpv4Addresses (Ipv4Address network, Ipv4Mask mask)
{
  if (!m_l2Installed)
    {
      NS_LOG_WARN (MSG_NETDEVICES_MISSING);
    }

  NS_LOG_FUNCTION (this << network << mask);
  Ipv4AddressGenerator::Init (network, mask);
  Ipv4Address v4network;
  Ipv4AddressHelper addrHelper;

  for (uint32_t i = 0; i < m_edgeSwitchDevices.size (); ++i)
    {
      for (uint32_t j = 0; j < m_edgeSwitchDevices[i].GetN (); j += 2)
        {
          v4network = Ipv4AddressGenerator::NextNetwork (mask);
          addrHelper.SetBase (v4network, mask);
          Ipv4InterfaceContainer ic = addrHelper.Assign (m_edgeSwitchDevices[i].Get (j));
          m_serverInterfaces.Add (ic);
          ic = addrHelper.Assign (m_edgeSwitchDevices[i].Get (j + 1));
          m_edgeSwitchInterfaces.Add (ic);
        }
    }

  for (uint32_t i = 0; i < m_aggregateSwitchDevices.size (); ++i)
    {
      v4network = Ipv4AddressGenerator::NextNetwork (mask);
      addrHelper.SetBase (v4network, mask);
      for (uint32_t j = 0; j < m_aggregateSwitchDevices[i].GetN (); j += 2)
        {
          Ipv4InterfaceContainer ic = addrHelper.Assign (m_aggregateSwitchDevices[i].Get (j));
          m_edgeSwitchInterfaces.Add (ic);
          ic = addrHelper.Assign (m_aggregateSwitchDevices[i].Get (j + 1));
          m_aggregateSwitchInterfaces.Add (ic);
        }
    }

  for (uint32_t i = 0; i < m_coreSwitchDevices.size (); ++i)
    {
      v4network = Ipv4AddressGenerator::NextNetwork (mask);
      addrHelper.SetBase (v4network, mask);
      for (uint32_t j = 0; j < m_coreSwitchDevices[i].GetN (); j += 2)
        {
          Ipv4InterfaceContainer ic = addrHelper.Assign (m_coreSwitchDevices[i].Get (j));
          m_aggregateSwitchInterfaces.Add (ic);
          ic = addrHelper.Assign (m_coreSwitchDevices[i].Get (j + 1));
          m_coreSwitchInterfaces.Add (ic);
        }
    }
}

void
FatTreeHelper::AssignIpv6Addresses (Ipv6Address addrBase, Ipv6Prefix prefix)
{
  if (!m_l2Installed)
    {
      NS_LOG_WARN (MSG_NETDEVICES_MISSING);
    }

  NS_LOG_FUNCTION (this << addrBase << prefix);
  Ipv6AddressGenerator::Init (addrBase, prefix);
  Ipv6Address v6network;
  Ipv6AddressHelper addrHelper;

  for (uint32_t i = 0; i < m_edgeSwitchDevices.size (); ++i)
    {
      v6network = Ipv6AddressGenerator::NextNetwork (prefix);
      addrHelper.SetBase (v6network, prefix);
      for (uint32_t j = 0; j < m_edgeSwitchDevices[i].GetN (); j += 2)
        {
          Ipv6InterfaceContainer ic = addrHelper.Assign (m_edgeSwitchDevices[i].Get (j));
          m_serverInterfaces6.Add (ic);
          ic = addrHelper.Assign (m_edgeSwitchDevices[i].Get (j + 1));
          m_edgeSwitchInterfaces6.Add (ic);
        }
    }

  for (uint32_t i = 0; i < m_aggregateSwitchDevices.size (); ++i)
    {
      v6network = Ipv6AddressGenerator::NextNetwork (prefix);
      addrHelper.SetBase (v6network, prefix);
      for (uint32_t j = 0; j < m_aggregateSwitchDevices[i].GetN (); j += 2)
        {
          Ipv6InterfaceContainer ic = addrHelper.Assign (m_aggregateSwitchDevices[i].Get (j));
          m_edgeSwitchInterfaces6.Add (ic);
          ic = addrHelper.Assign (m_aggregateSwitchDevices[i].Get (j + 1));
          m_aggregateSwitchInterfaces6.Add (ic);
        }
    }

  for (uint32_t i = 0; i < m_coreSwitchDevices.size (); ++i)
    {
      v6network = Ipv6AddressGenerator::NextNetwork (prefix);
      addrHelper.SetBase (v6network, prefix);
      for (uint32_t j = 0; j < m_coreSwitchDevices[i].GetN (); j += 2)
        {
          Ipv6InterfaceContainer ic = addrHelper.Assign (m_coreSwitchDevices[i].Get (j));
          m_aggregateSwitchInterfaces6.Add (ic);
          ic = addrHelper.Assign (m_coreSwitchDevices[i].Get (j + 1));
          m_coreSwitchInterfaces6.Add (ic);
        }
    }

}

Ipv4Address
FatTreeHelper::GetServerIpv4Address (uint32_t col) const
{
  NS_LOG_FUNCTION (this << col);
  return m_serverInterfaces.GetAddress (col);
}

Ipv4Address
FatTreeHelper::GetEdgeSwitchIpv4Address (uint32_t col) const
{
  NS_LOG_FUNCTION (this << col);
  return m_edgeSwitchInterfaces.GetAddress (col);
}

Ipv4Address
FatTreeHelper::GetAggregateSwitchIpv4Address (uint32_t col) const
{
  NS_LOG_FUNCTION (this << col);
  return m_aggregateSwitchInterfaces.GetAddress (col);
}

Ipv4Address
FatTreeHelper::GetCoreSwitchIpv4Address (uint32_t row) const
{
  NS_LOG_FUNCTION (this << row);
  return m_coreSwitchInterfaces.GetAddress (row);
}

Ipv6Address
FatTreeHelper::GetServerIpv6Address (uint32_t row) const
{
  NS_LOG_FUNCTION (this << row);
  return m_serverInterfaces6.GetAddress (row, 1);
}

Ipv6Address
FatTreeHelper::GetEdgeSwitchIpv6Address (uint32_t row) const
{
  NS_LOG_FUNCTION (this << row);
  return m_edgeSwitchInterfaces6.GetAddress (row, 1);
}

Ipv6Address
FatTreeHelper::GetAggregateSwitchIpv6Address (uint32_t row) const
{
  NS_LOG_FUNCTION (this << row);
  return m_aggregateSwitchInterfaces6.GetAddress (row, 1);
}

Ipv6Address
FatTreeHelper::GetCoreSwitchIpv6Address (uint32_t row) const
{
  NS_LOG_FUNCTION (this << row);
  return m_coreSwitchInterfaces6.GetAddress (row, 1);
}

Ptr<Node>
FatTreeHelper::GetServerNode (uint32_t row) const
{
  NS_LOG_FUNCTION (this << row);
  return m_servers.Get (row);
}

Ptr<Node>
FatTreeHelper::GetEdgeSwitchNode (uint32_t row) const
{
  NS_LOG_FUNCTION (this << row);
  return m_edgeSwitches.Get (row);
}

Ptr<Node>
FatTreeHelper::GetAggregateSwitchNode (uint32_t row) const
{
  NS_LOG_FUNCTION (this << row);
  return m_aggregateSwitches.Get (row);
}

Ptr<Node>
FatTreeHelper::GetCoreSwitchNode (uint32_t row) const
{
  NS_LOG_FUNCTION (this << row);
  return m_coreSwitches.Get (row);
}

} // namespace ns3
