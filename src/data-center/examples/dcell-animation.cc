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

#include <iostream>

#include "ns3/applications-module.h"
#include "ns3/dcell.h"
#include "ns3/core-module.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-nix-vector-helper.h"
#include "ns3/ipv4-static-routing.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DCellAnimation");

int main (int argc, char *argv[])
{
  Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (512));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("2Mbps"));

  uint32_t    nLevels = 1;
  uint32_t    nServers = 4;
  std::string animFile = "dcell-animation.xml";   // Name of file for animation output

  CommandLine cmd;
  cmd.AddValue ("nLevels", "Number of levels", nLevels);
  cmd.AddValue ("nSevers", "Number of servers", nServers);
  cmd.AddValue ("animFile", "File Name for Animation Output", animFile);
  cmd.Parse (argc, argv);

  DCellHelper p2pDcell (nLevels, nServers);

  // Create the point-to-point link helpers
  PointToPointHelper p2pHelper;
  p2pHelper.SetDeviceAttribute  ("DataRate", StringValue ("10Mbps"));
  p2pHelper.SetChannelAttribute ("Delay", StringValue ("1ms"));
  // Install NetDevices
  p2pDcell.InstallNetDevices (p2pHelper);

  // Install Stack
  InternetStackHelper internet;
  Ipv4NixVectorHelper nixRouting;
  Ipv4StaticRoutingHelper staticRouting;

  Ipv4ListRoutingHelper list;
  list.Add (staticRouting, 0);
  list.Add (nixRouting, 10);
  internet.SetRoutingHelper (list);

  p2pDcell.InstallStack (internet);
  p2pDcell.AssignIpv4Addresses (Ipv4Address ("10.0.0.0"), Ipv4Mask ("/16"));

  // Configure a test flow from server 0 in DCell0 to server 6 in DCell1
  OnOffHelper clientHelper ("ns3::UdpSocketFactory", Address ());
  clientHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  clientHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  ApplicationContainer clientApps;
  AddressValue remoteAddress (InetSocketAddress (p2pDcell.GetServerIpv4Address (6, 0), 5001));
  clientHelper.SetAttribute ("Remote", remoteAddress);
  clientApps.Add (clientHelper.Install (p2pDcell.GetServerNode (0)));

  uint16_t port = 50001;
  Address sinkLocalAddress (InetSocketAddress (p2pDcell.GetServerIpv4Address (6, 0), port));
  PacketSinkHelper sinkHelper ("ns3::UdpSocketFactory", sinkLocalAddress);
  ApplicationContainer sinkApp = sinkHelper.Install (p2pDcell.GetServerNode (6));

  clientApps.Start (Seconds (1.0));
  clientApps.Stop (Seconds (10.0));

  sinkApp.Start (Seconds (0.0));
  sinkApp.Stop (Seconds (10.0));

  // Set the bounding box for animation
  p2pDcell.BoundingBox (1, 1, 100, 100);

  // Create the animation object and configure for specified output
  AnimationInterface anim (animFile);
  anim.EnablePacketMetadata (); // Optional
  anim.EnableIpv4L3ProtocolCounters (Seconds (0), Seconds (10)); // Optional

  // Set up the actual simulation
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Run ();
  std::cout << "Animation Trace file created:" << animFile.c_str () << std::endl;
  Simulator::Destroy ();
  return 0;
}
