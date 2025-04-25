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

#include "ns3/constant-position-mobility-model.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-generator.h"
#include "ns3/ipv6-address-generator.h"
#include "ns3/leaf-spine.h"
#include "ns3/log.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/point-to-point-net-device.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LeafSpineHelper");

LeafSpineHelper::LeafSpineHelper (uint32_t numSpine,
                                  uint32_t numLeaf,
                                  uint32_t numServerPerLeaf)
  : m_l2Installed (false), m_numSpine (numSpine), m_numLeaf (numLeaf), m_numServerPerLeaf (numServerPerLeaf)
{
  // Bounds check
  if (numSpine < NUM_SPINE_MIN)
    {
      NS_FATAL_ERROR ("The minimum number of spine switches is " << NUM_SPINE_MIN << ".");
    }
  if (numLeaf < NUM_LEAF_MIN)
    {
      NS_FATAL_ERROR ("The minimum number of leaf switches is " << NUM_LEAF_MIN << ".");
    }  
  if (numServerPerLeaf < NUM_SERVER_PER_LEAF_MIN)
    {
      NS_FATAL_ERROR ("The minimum number of servers per leaf switch is " << NUM_SERVER_PER_LEAF_MIN << ".");
    }

  // Initialize NetDeviceContainer
  m_spineDevices.resize (m_numSpine);
  m_leafDevices.resize (m_numLeaf);
  m_serverDevices.resize (m_numLeaf * m_numServerPerLeaf);
  // Initialize Ipv4InterfaceContainer
  m_spineInterfaces.resize (m_numSpine);
  m_leafInterfaces.resize (m_numLeaf);
  m_serverInterfaces.resize (m_numLeaf * m_numServerPerLeaf);
  // Initialize Ipv6InterfaceContainer
  m_spineInterfaces6.resize (m_numSpine);
  m_leafInterfaces6.resize (m_numLeaf);
  m_serverInterfaces6.resize (m_numLeaf * m_numServerPerLeaf);  
  // Initialize NodeContainer
  m_spineSwitches.Create (m_numSpine);
  m_leafSwitches.Create (m_numLeaf);
  m_servers.Create (m_numServerPerLeaf * m_numLeaf);

}

LeafSpineHelper::~LeafSpineHelper ()
{
}

LeafSpineHelper::LeafSpineHelper (const LeafSpineHelper& helper)
: DcnTopologyHelper (helper),
  m_l2Installed (helper.m_l2Installed),
  m_numSpine (helper.m_numSpine),
  m_numLeaf (helper.m_numLeaf),
  m_numServerPerLeaf (helper.m_numServerPerLeaf),
  m_spineSwitches (helper.m_spineSwitches),
  m_leafSwitches (helper.m_leafSwitches),
  m_servers (helper.m_servers)
{
  m_serverDevices = helper.m_serverDevices; 
  m_leafDevices = helper.m_leafDevices;  
  m_leafDevices = helper.m_leafDevices; 
  m_serverDevices = helper.m_serverDevices; 
  m_spineInterfaces = helper.m_spineInterfaces;  
  m_serverInterfaces6 = helper.m_serverInterfaces6;
  m_leafInterfaces = helper.m_leafInterfaces; 
  m_serverInterfaces = helper.m_serverInterfaces; 
  m_spineInterfaces6 = helper.m_spineInterfaces6;             
  m_leafInterfaces6 = helper.m_leafInterfaces6;  
}

void
LeafSpineHelper::InstallStack (InternetStackHelper& stack)
{
  if (!m_l2Installed)
    {
      NS_LOG_WARN (MSG_NETDEVICES_MISSING);
    }

  stack.Install (m_spineSwitches);
  stack.Install (m_leafSwitches);
  stack.Install (m_servers);
}

void
LeafSpineHelper::InstallTrafficControl (TrafficControlHelper& tchSwitch,
                                        TrafficControlHelper& tchServer)
{
  if (!m_l2Installed)
    {
      NS_LOG_WARN (MSG_NETDEVICES_MISSING);
    }

  for (std::vector<NetDeviceContainer>::iterator it = m_spineDevices.begin() ; it != m_spineDevices.end(); it++)
  {
    tchSwitch.Install (*it);
  }
  for (std::vector<NetDeviceContainer>::iterator it = m_leafDevices.begin() ; it != m_leafDevices.end(); it++)
  {
    tchSwitch.Install (*it);
  }
  for (std::vector<NetDeviceContainer>::iterator it = m_serverDevices.begin() ; it != m_serverDevices.end(); it++)
  {
    tchServer.Install (*it);
  }      
}

void
LeafSpineHelper::BoundingBox (double ulx, double uly,
                              double lrx, double lry)
{
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

  double xServerUnit = xDist / (m_numServerPerLeaf*m_numLeaf-1);
  double xLeafSwitchUnit = xDist / (m_numLeaf-1);
  double xSpineSwitchUnit = xDist / (m_numSpine-1);
  // 2 layers of switches and 1 layer of servers
  double yServer = yDist / 3;
  double yLeafSwitch = 2*yDist / 3;
  double ySpineSwitch = yDist;

  // Locate the servers
  double xLoc = 0.0;
  double yLoc = yServer;
  for (uint32_t i = 0; i < (m_numServerPerLeaf*m_numLeaf); i++)
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
      xLoc += xServerUnit;
    }

  // Locate the leaf switches
  xLoc = 0.0;
  yLoc = yLeafSwitch;
  for (uint32_t i = 0; i < m_numLeaf; i++)
    {
      Ptr<Node> node = m_leafSwitches.Get (i);
      Ptr<ConstantPositionMobilityModel> loc = node->GetObject<ConstantPositionMobilityModel> ();
      if (loc == 0)
        {
          loc = CreateObject<ConstantPositionMobilityModel> ();
          node->AggregateObject (loc);
        }
      Vector locVec (xLoc, yLoc, 0);
      loc->SetPosition (locVec);
      xLoc += xLeafSwitchUnit;
    }

  // Place the spine switches
  xLoc = 0.0;
  yLoc = ySpineSwitch;
  for (uint32_t i = 0; i < m_numSpine; i++)
    {
      Ptr<Node> node = m_spineSwitches.Get (i);
      Ptr<ConstantPositionMobilityModel> loc = node->GetObject<ConstantPositionMobilityModel> ();
      if (loc == 0)
        {
          loc = CreateObject<ConstantPositionMobilityModel> ();
          node->AggregateObject (loc);
        }
      Vector locVec (xLoc, yLoc, 0);
      loc->SetPosition (locVec);
      xLoc += xSpineSwitchUnit;
    }
}

void
LeafSpineHelper::AssignIpv4Addresses (Ipv4Address network, Ipv4Mask mask)
{
  if (!m_l2Installed)
    {
      NS_LOG_WARN (MSG_NETDEVICES_MISSING);
    }

  Ipv4AddressGenerator::Init (network, mask);
  Ipv4Address address;
  Ipv4AddressHelper addressHelper;

  // Assign addresses for each server cluster
  for (uint32_t i = 0; i < m_numLeaf; i++)
    {
      address = Ipv4AddressGenerator::NextNetwork (mask);
      addressHelper.SetBase (address, mask);
      for (uint32_t j = 0; j < m_numServerPerLeaf; j++)
        {
          m_leafInterfaces[i].Add (addressHelper.Assign (m_leafDevices[i].Get(j)));
          m_serverInterfaces[j+i*m_numServerPerLeaf].Add (addressHelper.Assign (m_serverDevices[j+i*m_numServerPerLeaf]));
        }
    }
  
  // Assign addresses for spine switches and leaf switches
  address = Ipv4AddressGenerator::NextNetwork (mask);
  addressHelper.SetBase (address, mask);
  for (uint32_t i = 0; i < m_numSpine; i++)
    {
      for (uint32_t j = 0; j < m_numLeaf; j++)
        {
          m_spineInterfaces[i].Add (addressHelper.Assign (m_spineDevices[i].Get(j)));
          m_leafInterfaces[j].Add (addressHelper.Assign (m_leafDevices[j].Get(i+m_numServerPerLeaf)));        
        }
    }
}

void
LeafSpineHelper::AssignIpv6Addresses (Ipv6Address addrBase, Ipv6Prefix prefix)
{
  if (!m_l2Installed)
    {
      NS_LOG_WARN (MSG_NETDEVICES_MISSING);
    }

  Ipv6AddressGenerator::Init (addrBase, prefix);
  Ipv6Address address;
  Ipv6AddressHelper addressHelper;  

  // Assign addresses for each server cluster
  for (uint32_t i = 0; i < m_numLeaf; i++)
    {
      address = Ipv6AddressGenerator::NextNetwork (prefix);
      addressHelper.SetBase (address, prefix);
      for (uint32_t j = 0; j < m_numServerPerLeaf; j++)
        {
          m_leafInterfaces6[i].Add (addressHelper.Assign (m_leafDevices[i].Get(j)));
          m_serverInterfaces6[j+i*m_numLeaf].Add (addressHelper.Assign (m_serverDevices[j+i*m_numLeaf]));
        }
    }
  
  // Assign addresses for spine switches and leaf switches
  address = Ipv6AddressGenerator::NextNetwork (prefix);
  addressHelper.SetBase (address, prefix);
  for (uint32_t i = 0; i < m_numSpine; i++)
  {
    for (uint32_t j = 0; j < m_numLeaf; j++)
      {
        m_spineInterfaces6[i].Add (addressHelper.Assign (m_spineDevices[i].Get(j)));
        m_leafInterfaces6[j].Add (addressHelper.Assign (m_leafDevices[j].Get(i+m_numServerPerLeaf)));        
      }
  }
}

Ipv4Address
LeafSpineHelper::GetServerIpv4Address (uint32_t col) const
{
  if (col >= (m_numServerPerLeaf * m_numLeaf))
    {
      NS_FATAL_ERROR ("Server address exceeds the maximum " << std::to_string(m_numServerPerLeaf * m_numLeaf-1) << ".");
    }
  return m_serverInterfaces[col].GetAddress (0);
}

Ipv4Address
LeafSpineHelper::GetLeafIpv4Address (uint32_t col, uint32_t interfaceIdx) const
{
  if (col >= m_numLeaf)
    {
      NS_FATAL_ERROR ("Leaf switch address exceeds the maximum" << std::to_string(m_numLeaf - 1) << ".");
    }
  if (interfaceIdx >= m_numServerPerLeaf + m_numSpine)
    {
      NS_FATAL_ERROR ("Leaf switch interface index exceeds the maximum" << std::to_string(m_numServerPerLeaf + m_numSpine - 1) << ".");
    }  
  return m_leafInterfaces[col].GetAddress (interfaceIdx);
}

Ipv4InterfaceContainer
LeafSpineHelper::GetLeafIpv4Interfaces (uint32_t col) const
{
  if (col >= m_numLeaf)
    {
      NS_FATAL_ERROR ("Leaf switch address exceeds the maximum" << std::to_string(m_numLeaf - 1) << ".");
    }
  return m_leafInterfaces[col];  
}

Ipv4Address
LeafSpineHelper::GetSpineIpv4Address (uint32_t col, uint32_t interfaceIdx) const
{
  if (col >= m_numSpine)
    {
      NS_FATAL_ERROR ("Spine switch address exceeds the maximum" << std::to_string(m_numSpine - 1) << ".");
    }
  if (interfaceIdx >= m_numLeaf)
    {
      NS_FATAL_ERROR ("Spine switch interface index exceeds the maximum" << std::to_string(m_numLeaf -1) << ".");
    }    
  return m_spineInterfaces[col].GetAddress (interfaceIdx);
}

Ipv4InterfaceContainer
LeafSpineHelper::GetSpineIpv4Interfaces (uint32_t col) const
{
  if (col >= m_numSpine)
    {
      NS_FATAL_ERROR ("Spine switch address exceeds the maximum" << std::to_string(m_numSpine - 1) << ".");
    }
  return m_spineInterfaces[col];  
}

Ipv6Address
LeafSpineHelper::GetServerIpv6Address (uint32_t col) const
{
  if (col >= m_numServerPerLeaf * m_numLeaf)
    {
      NS_FATAL_ERROR ("Server address exceeds the maximum" << std::to_string(m_numServerPerLeaf * m_numLeaf - 1) << ".");
    }
  return m_serverInterfaces6[col].GetAddress (0, 1);
}

Ipv6Address
LeafSpineHelper::GetLeafIpv6Address (uint32_t col, uint32_t interfaceIdx) const
{
  if (col >= m_numLeaf)
    {
      NS_FATAL_ERROR ("Leaf switch address exceeds the maximum" << std::to_string(m_numLeaf - 1) << ".");
    }
  if (interfaceIdx >= m_numServerPerLeaf + m_numSpine)
    {
      NS_FATAL_ERROR ("Leaf switch interface index exceeds the maximum" << std::to_string(m_numServerPerLeaf + m_numSpine - 1) << ".");
    }
  return m_leafInterfaces6[col].GetAddress (interfaceIdx, 1);
}

Ipv6InterfaceContainer
LeafSpineHelper::LeafSpineHelper::GetLeafIpv6Interfaces (uint32_t col) const
{
  if (col >= m_numLeaf)
    {
      NS_FATAL_ERROR ("Leaf switch address exceeds the maximum" << std::to_string(m_numLeaf - 1) << ".");
    }
  return m_leafInterfaces6[col];  
}

Ipv6Address
LeafSpineHelper::GetSpineIpv6Address (uint32_t col, uint32_t interfaceIdx) const
{
  if (col >= m_numSpine)
    {
      NS_FATAL_ERROR ("Spine switch address exceeds the maximum" << std::to_string(m_numSpine - 1) << ".");
    }
  if (interfaceIdx >= m_numLeaf)
    {
      NS_FATAL_ERROR ("Spine switch interface index exceeds the maximum" << std::to_string(m_numLeaf - 1) << ".");
    }
  return m_spineInterfaces6[col].GetAddress (interfaceIdx, 1);
}

Ipv6InterfaceContainer
LeafSpineHelper::GetSpineIpv6Interfaces (uint32_t col) const
{
  if (col >= m_numSpine)
    {
      NS_FATAL_ERROR ("Spine switch address exceeds the maximum" << std::to_string(m_numSpine - 1) << ".");
    }
  return m_spineInterfaces6[col];  
}

Ptr<Node>
LeafSpineHelper::GetServerNode (uint32_t col) const
{
  if (col >= m_numServerPerLeaf * m_numLeaf)
    {
      NS_FATAL_ERROR ("Server address exceeds the maximum" << std::to_string(m_numServerPerLeaf * m_numLeaf - 1) << ".");
    }
  return m_servers.Get (col);
}

Ptr<Node>
LeafSpineHelper::GetLeafNode (uint32_t col) const
{
  if (col >= m_numLeaf)
    {
      NS_FATAL_ERROR ("Leaf switch address exceeds the maximum" << std::to_string(m_numLeaf - 1) << ".");
      return 0;
    }
  return m_leafSwitches.Get (col);
}

Ptr<Node>
LeafSpineHelper::GetSpineNode (uint32_t col) const
{
  if (col >= m_numSpine)
    {
      NS_FATAL_ERROR ("Spine switch address exceeds the maximum" << std::to_string(m_numSpine - 1) << ".");
      return 0;
    }    
  return m_spineSwitches.Get (col);
}

uint32_t
LeafSpineHelper::SpineCount () const
{
  return m_numSpine;
}

uint32_t
LeafSpineHelper::LeafCount () const
{
  return m_numLeaf;
}

uint32_t
LeafSpineHelper::ServerCount () const
{
  return m_numServerPerLeaf * m_numLeaf;
}

uint32_t
LeafSpineHelper::TotalCount () const
{
  return m_numSpine + m_numLeaf + m_numServerPerLeaf * m_numLeaf;
}

} // namespace ns3
