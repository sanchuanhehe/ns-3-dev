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

// Implement an object to create a DCell topology.

#include <math.h>

#include "ns3/dcell.h"
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

NS_LOG_COMPONENT_DEFINE ("DCellHelper");

DCellHelper::DCellHelper (uint32_t nLevels,
                          uint32_t nServers)
  : m_l2Installed (false),
    m_numLevels (nLevels),
    m_numServersDCell0 (nServers)
{
  // Bounds check
  if (nServers < N_SERVER_MIN)
    {
      NS_FATAL_ERROR ("Insufficient number of servers for DCell.");
    }
  
  // Calculate and store the number of servers for each DCell level
  m_numServersByLevel.resize (m_numLevels+1);
  m_numServersByLevel[0] = m_numServersDCell0;
  for (uint32_t level = 1; level <= m_numLevels; level++)
    {
      uint32_t numDcells = m_numServersByLevel[level-1] + 1;
      uint32_t numServers = numDcells * m_numServersByLevel[level-1];
      m_numServersByLevel[level] = numServers;
    }

  m_servers.Create (m_numServersByLevel[nLevels]);
  m_switches.Create (m_numServersByLevel[nLevels]/nServers);
  m_serverDevices.resize (m_numServersByLevel[nLevels]);
  m_switchDevices.resize (m_numServersByLevel[nLevels]/nServers);
  m_serverInterfaces.resize (m_numServersByLevel[nLevels]);
  m_serverInterfaces6.resize (m_numServersByLevel[nLevels]);
  m_switchInterfaces.resize (m_numServersByLevel[nLevels]/nServers);
  m_switchInterfaces6.resize (m_numServersByLevel[nLevels]/nServers);

}

DCellHelper::~DCellHelper ()
{
}

DCellHelper::DCellHelper (const DCellHelper& helper)
: DcnTopologyHelper (helper),
  m_l2Installed (helper.m_l2Installed),
  m_numLevels (helper.m_numLevels),
  m_numServersDCell0 (helper.m_numServersDCell0),
  m_servers (helper.m_servers),
  m_switches (helper.m_switches)
{
  m_serverDevices =  helper.m_serverDevices; 
  m_switchDevices =  helper.m_switchDevices;  
  m_switchInterfaces =  helper.m_switchInterfaces; 
  m_serverInterfaces =  helper.m_serverInterfaces; 
  m_switchInterfaces6 =  helper.m_switchInterfaces6;  
  m_serverInterfaces6 =  helper.m_serverInterfaces6;
  m_numServersByLevel =  helper.m_numServersByLevel;  
}

void
DCellHelper::InstallStack (InternetStackHelper& stack)
{
  if (!m_l2Installed)
    {
      NS_LOG_WARN (MSG_NETDEVICES_MISSING);
    }
  
  stack.Install (m_servers);
  stack.Install (m_switches);
}

void
DCellHelper::InstallTrafficControl (TrafficControlHelper& tchSwitch,
                                    TrafficControlHelper& tchServer)
{
  if (!m_l2Installed)
    {
      NS_LOG_WARN (MSG_NETDEVICES_MISSING);
    }

  for (std::vector<NetDeviceContainer>::iterator it = m_switchDevices.begin() ; it != m_switchDevices.end(); it++)
    {
      tchSwitch.Install (*it);
    }
  for (std::vector<NetDeviceContainer>::iterator it = m_serverDevices.begin() ; it != m_serverDevices.end(); it++)
    {
      tchServer.Install (*it);
    }
}

void
DCellHelper::BoundingBox (double ulx, double uly,
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

  uint32_t numServers = m_numServersByLevel[m_numLevels];
  uint32_t numSwitches = m_numServersByLevel[m_numLevels]/m_numServersDCell0;

  // Place the servers at the border of the smaller circle
  double serverRadUnit = 2*3.14159265/numServers;
  double switchRadUnit = 2*3.14159265/numSwitches;
  double rServer = std::min (xDist, yDist)/2*0.8;
  double rSwitch = std::min (xDist, yDist)/2;
  double xCenter = std::min (xDist, yDist)/2;
  double yCenter = std::min (xDist, yDist)/2;
  double xLoc = 0.0;
  double yLoc = 0.0;
  for (uint32_t i = 0; i < numServers; ++i)
    {
      xLoc = xCenter + cos (serverRadUnit*(i)) * rServer;
      yLoc = yCenter + sin (serverRadUnit*(i)) * rServer;
      Ptr<Node> node = m_servers.Get (i);
      Ptr<ConstantPositionMobilityModel> loc = node->GetObject<ConstantPositionMobilityModel> ();
      if (loc == 0)
        {
          loc = CreateObject<ConstantPositionMobilityModel> ();
          node->AggregateObject (loc);
        }
      Vector locVec (xLoc, yLoc, 0);
      loc->SetPosition (locVec);
    }

  // Place the switches at the border of the larger circle
  for (uint32_t i = 0; i < numSwitches; ++i)
    {
      xLoc = xCenter + cos (switchRadUnit*(i)+serverRadUnit*(m_numServersDCell0/2)) * rSwitch;
      yLoc = yCenter + sin (switchRadUnit*(i)+serverRadUnit*(m_numServersDCell0/2)) * rSwitch;
      Ptr<Node> node = m_switches.Get (i);
      Ptr<ConstantPositionMobilityModel> loc = node->GetObject<ConstantPositionMobilityModel> ();
      if (loc == 0)
        {
          loc = CreateObject<ConstantPositionMobilityModel> ();
          node->AggregateObject (loc);
        }
      Vector locVec (xLoc, yLoc, 0);
      loc->SetPosition (locVec);
    }
}

void
DCellHelper::AssignIpv4Addresses (Ipv4Address network, Ipv4Mask mask)
{
  if (!m_l2Installed)
    {
      NS_LOG_WARN (MSG_NETDEVICES_MISSING);
    }

  NS_LOG_FUNCTION (this << network << mask);
  Ipv4AddressGenerator::Init (network, mask);
  Ipv4Address v4network;
  Ipv4AddressHelper addrHelper;

  // Assign ip for the links between servers and mini-switches
  uint32_t numDcell0 = m_numServersByLevel[m_numLevels]/m_numServersDCell0;
  for (uint32_t switchId = 0; switchId < numDcell0; switchId++)
    {
      v4network = Ipv4AddressGenerator::NextNetwork (mask);
      addrHelper.SetBase (v4network, mask);
      for (uint32_t serverId = 0; serverId < m_numServersDCell0; serverId++)
        {
          Ipv4InterfaceContainer ic = addrHelper.Assign (m_serverDevices[switchId*m_numServersDCell0+serverId].Get (0));
          m_serverInterfaces[switchId*m_numServersDCell0+serverId].Add (ic);
          ic = addrHelper.Assign (m_switchDevices[switchId].Get (serverId));
          m_switchInterfaces[switchId].Add (ic);                                        
        }
    }
  
  // Assign ip for server to server links belonging to DCells of different levels
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
          v4network = Ipv4AddressGenerator::NextNetwork (mask);
          addrHelper.SetBase (v4network, mask);          
          for (uint32_t j = i+1; j < numDcells; j++)
            {
              Ipv4InterfaceContainer ic = addrHelper.Assign (m_serverDevices[i*numServers+j-1].Get (level));
              m_serverInterfaces[i*numServers+j-1].Add (ic);
              ic = addrHelper.Assign (m_serverDevices[j*numServers+i].Get (level));
              m_serverInterfaces[j*numServers+i].Add (ic);
            }
        }
    }
}

void
DCellHelper::AssignIpv6Addresses (Ipv6Address addrBase, Ipv6Prefix prefix)
{
  if (!m_l2Installed)
    {
      NS_LOG_WARN (MSG_NETDEVICES_MISSING);
    }

  NS_LOG_FUNCTION (this << addrBase << prefix);
  Ipv6AddressGenerator::Init (addrBase, prefix);
  Ipv6Address v6network;
  Ipv6AddressHelper addrHelper;

  // Assign ip for the links between servers and mini-switches
  uint32_t numDcell0 = m_numServersByLevel[m_numLevels]/m_numServersDCell0;
  for (uint32_t switchId = 0; switchId < numDcell0; switchId++)
    {
      v6network = Ipv6AddressGenerator::NextNetwork (prefix);
      addrHelper.SetBase (v6network, prefix);
      for (uint32_t serverId = 0; serverId < m_numServersDCell0; serverId++)
        {
          Ipv6InterfaceContainer ic = addrHelper.Assign (m_serverDevices[switchId*m_numServersDCell0+serverId].Get (0));
          m_serverInterfaces6[switchId*m_numServersDCell0+serverId].Add (ic);
          ic = addrHelper.Assign (m_switchDevices[switchId].Get (serverId));
          m_switchInterfaces6[switchId].Add (ic);                                        
        }
    }
  
  // Assign ip for server to server links belonging to DCells of different levels
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
          v6network = Ipv6AddressGenerator::NextNetwork (prefix);
          addrHelper.SetBase (v6network, prefix);          
          for (uint32_t j = i+1; j < numDcells; j++)
            {
              Ipv6InterfaceContainer ic = addrHelper.Assign (m_serverDevices[i*numServers+j-1].Get (level));
              m_serverInterfaces6[i*numServers+j-1].Add (ic);
              ic = addrHelper.Assign (m_serverDevices[j*numServers+i].Get (level));
              m_serverInterfaces6[j*numServers+i].Add (ic);
            }
        }
    }

}

Ipv4InterfaceContainer
DCellHelper::GetServerIpv4Interfaces (uint32_t uid) const
{
  NS_LOG_FUNCTION (this << uid);
  return m_serverInterfaces[uid];
}

Ipv4InterfaceContainer
DCellHelper::GetSwitchIpv4Interfaces (uint32_t index) const
{
  NS_LOG_FUNCTION (this << index);
  return m_switchInterfaces[index];
}

Ipv6InterfaceContainer
DCellHelper::GetServerIpv6Interfaces (uint32_t uid) const
{
  NS_LOG_FUNCTION (this << uid);
  return m_serverInterfaces6[uid];
}

Ipv6InterfaceContainer
DCellHelper::GetSwitchIpv6Interfaces (uint32_t index) const
{
  NS_LOG_FUNCTION (this << index);
  return m_switchInterfaces6[index];
}

Ptr<Node>
DCellHelper::GetServerNode (uint32_t uid) const
{
  NS_LOG_FUNCTION (this << uid);
  return m_servers.Get (uid);
}

Ptr<Node>
DCellHelper::GetSwitchNode (uint32_t index) const
{
  NS_LOG_FUNCTION (this << index);
  return m_switches.Get(index);
}

Ipv4Address 
DCellHelper::GetServerIpv4Address (uint32_t uid, uint32_t level) const
{
  NS_LOG_FUNCTION (this << uid << level);
  return m_serverInterfaces[uid].GetAddress (level);
}

Ipv6Address 
DCellHelper::GetServerIpv6Address (uint32_t uid, uint32_t level) const
{
  NS_LOG_FUNCTION (this << uid << level);
  return m_serverInterfaces6[uid].GetAddress (level, 1);
}

Ipv4Address 
DCellHelper::GetSwitchIpv4Address (uint32_t index, uint32_t serverId) const
{
  NS_LOG_FUNCTION (this << index << serverId);
  return m_switchInterfaces[index].GetAddress (serverId);
}

Ipv6Address 
DCellHelper::GetSwitchIpv6Address (uint32_t index, uint32_t serverId) const
{
  NS_LOG_FUNCTION (this << index << serverId);
  return m_switchInterfaces6[index].GetAddress (serverId, 1);
}

} // namespace ns3
