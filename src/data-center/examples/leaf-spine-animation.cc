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

#include <iostream>

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/data-center-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-nix-vector-helper.h"
#include "ns3/ipv4-static-routing.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/traffic-control-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LeafSpineAnimation");

int main (int argc, char *argv[])
{
  uint32_t    numSpine = 2;
  uint32_t    numLeaf = 4;
  uint32_t    numServerPerLeaf = 6;
  uint32_t    timeSimEnd = 10;
  uint32_t    numFlowsMax = 20;
  double      load = 0.5;  // Traffic load of the network w.r.t. the aggregate server bandwidth, set 50% by default
  std::string    workloadType = "VL2";

  // The name of the xml animation file to be visualized with ./NetAnim application
  std::string animFile = "leaf-spine-animation.xml";

  CommandLine cmd;
  cmd.AddValue ("numSpine", "Number of spine switches", numSpine);
  cmd.AddValue ("numLeaf", "Number of leaf switches", numLeaf);
  cmd.AddValue ("numServerPerLeaf", "Number of servers per leaf switch", numServerPerLeaf);
  cmd.AddValue ("timeSimEnd", "Simulation time [s]", timeSimEnd);
  cmd.AddValue ("numFlowsMax", "Maximum number of flows to simulate", numFlowsMax);
  cmd.AddValue ("workloadType", "Type of the flow size distribtion (VL2/DCTCP)", workloadType);
  cmd.AddValue ("animFile", "File Name for Animation Output", animFile);
  cmd.Parse (argc,argv);

  Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (1458));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("10Mbps"));
  SeedManager::SetSeed (2019);

  // Create the point-to-point link helpers
  PointToPointHelper p2pLeafSpine;
  p2pLeafSpine.SetDeviceAttribute  ("DataRate", StringValue ("40Mbps"));
  p2pLeafSpine.SetChannelAttribute ("Delay", StringValue ("1us"));

  PointToPointHelper p2pServerLeaf;
  p2pServerLeaf.SetDeviceAttribute  ("DataRate", StringValue ("10Mbps"));
  p2pServerLeaf.SetChannelAttribute ("Delay", StringValue ("10us"));

  LeafSpineHelper p2pLeafSpineHelper (numSpine, numLeaf, numServerPerLeaf);

  p2pLeafSpineHelper.InstallNetDevices (p2pServerLeaf, p2pLeafSpine);

  // Configure internet stack
  InternetStackHelper stack;
  Ipv4GlobalRoutingHelper globalRouting;
  Ipv4ListRoutingHelper list;
  list.Add (globalRouting, 0);
  // Use packet spraying
  Config::SetDefault ("ns3::Ipv4GlobalRouting::EcmpRoutingMode", EnumValue (Ipv4GlobalRouting::RandomEcmpRouting));
  stack.SetRoutingHelper (list);

  // Install Stack
  p2pLeafSpineHelper.InstallStack (stack);

  // Configure traffic control layer for all nodes
  TrafficControlHelper tchGlobal;
  uint16_t handle = tchGlobal.SetRootQueueDisc ("ns3::PfifoFastQueueDisc");
  tchGlobal.AddInternalQueues (handle, 3, "ns3::DropTailQueue", "MaxSize", StringValue ("1000p"));
  p2pLeafSpineHelper.InstallTrafficControl (tchGlobal, tchGlobal);

  p2pLeafSpineHelper.AssignIpv4Addresses (Ipv4Address ("10.0.0.0"),Ipv4Mask ("255.255.255.0"));

  /* 
   * Configure flows based on typical DCN workloads: 
   * - Data mining workload (a.k.a. VL2, mean flow size 2126KB), source: A.Greenberg, J.R.Hamilton, N.Jain, S.Kandula, C.Kim, P.Lahiri, D.A. Maltz, P. Patel, and S. Sengupta. VL2: a scalable and flexible data center network. In Proc. of SIGCOMM, 2009.
   * - Web search workload (a.k.a. DCTCP, mean flow size 1134KB), source: M. Alizadeh, A. Greenberg, D. A. Maltz, J. Padhye, P. Patel, B. Prabhakar, S. Sengupta, and M. Sridharan. Data center TCP (DCTCP). In Proc. of SIGCOMM, 2010.
   * - Others: an inter-rack flow and an intra-rack flow
   */

  NS_LOG_INFO("Configure random variables for synthesizing the traffic.");
  Ptr<ExponentialRandomVariable> flowInterval = CreateObject<ExponentialRandomVariable> ();
  Ptr<EmpiricalRandomVariable> flowSizeCdf = CreateObject<EmpiricalRandomVariable> ();
  double flowRate;  // Mean number of flow arrivals per second for the network
	if (workloadType.compare("DCTCP") == 0)
    {
      flowRate = static_cast<double>((10.0*1000.0*p2pLeafSpineHelper.ServerCount())/(8.0*1134)*load);
      flowInterval->SetAttribute ("Mean", DoubleValue (1.0/flowRate));
      flowSizeCdf->CDF (6000.0, 0.15);
      flowSizeCdf->CDF (13000.0, 0.2);
      flowSizeCdf->CDF (19000.0, 0.3);
      flowSizeCdf->CDF (33000.0, 0.4);
      flowSizeCdf->CDF (53000.0, 0.53);
      flowSizeCdf->CDF (133000.0, 0.6);
      flowSizeCdf->CDF (667000.0, 0.7);
      flowSizeCdf->CDF (1333000.0, 0.8);
      flowSizeCdf->CDF (3333000.0, 0.9);
      flowSizeCdf->CDF (6667000.0, 0.97);
      flowSizeCdf->CDF (20000000.0, 1.0);
  	}
	else if (workloadType.compare("VL2") == 0) 
    {
      flowRate = static_cast<double>((10.0*1000.0*p2pLeafSpineHelper.ServerCount())/(8.0*2126)*load);
      flowInterval->SetAttribute ("Mean", DoubleValue (1.0/flowRate));
      flowSizeCdf->CDF (1000.0, 0.5);
      flowSizeCdf->CDF (2000.0, 0.6);
      flowSizeCdf->CDF (3000.0, 0.7);
      flowSizeCdf->CDF (7000.0, 0.8);
      flowSizeCdf->CDF (267000.0, 0.9);
      flowSizeCdf->CDF (2107000.0, 0.95);
      flowSizeCdf->CDF (66667000.0, 0.99);
      flowSizeCdf->CDF (666667000.0, 1.0);
  	}

  NS_LOG_INFO("Simulate flows.");
  OnOffHelper clientHelper ("ns3::TcpSocketFactory", Address ());
  PacketSinkHelper destinationHelper ("ns3::TcpSocketFactory", Address ());
  clientHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  clientHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]")); 
  clientHelper.SetAttribute ("DataRate", DataRateValue (DataRate("10Mbps")));
  ApplicationContainer sinkApps;
  if ((workloadType.compare("DCTCP") == 0) || (workloadType.compare("VL2") == 0))
    {
    	double flowStart = 0;
      uint32_t port = 5000;
      uint32_t sourceId;
      uint32_t destinationId;
    	for (uint32_t flowId = 0; flowStart <= timeSimEnd ;flowId++, port++)
        {
          flowStart += flowInterval->GetValue();
          // Set the amount of data to send in bytes
          uint32_t flowSize = flowSizeCdf->GetInteger();
          // Make sure the number of simulated flows will not exceed numFlowsMax
          if (flowId == numFlowsMax)
            {
              break;
            }
          // Randomly select a sender
          sourceId = rand() % p2pLeafSpineHelper.ServerCount();
          // Randomly select a receiver
          destinationId = rand() % p2pLeafSpineHelper.ServerCount();
          // Make sure the sender id does not collide with the receiver id
          while (sourceId == destinationId)
            {
              destinationId = rand() % p2pLeafSpineHelper.ServerCount();
            }
          
          NS_LOG_INFO ("Configure the flow: server " << std::to_string(sourceId) << " => server " << std::to_string(destinationId) << " with size " \
                          << std::to_string(flowSize) << " bytes and starting time at " << std::to_string(flowStart) << "s.");
          AddressValue remoteAddress (InetSocketAddress (p2pLeafSpineHelper.GetServerIpv4Address (destinationId), port));
          clientHelper.SetAttribute ("Remote", remoteAddress);
          clientHelper.SetAttribute ("MaxBytes", UintegerValue(flowSize));
          clientHelper.SetAttribute ("StartTime", TimeValue(Seconds(flowStart)));
          clientHelper.Install (p2pLeafSpineHelper.GetServerNode (sourceId));

          destinationHelper.SetAttribute ("Local", remoteAddress);
          destinationHelper.SetAttribute ("StartTime", TimeValue(Seconds(0)));
          destinationHelper.SetAttribute ("StopTime", TimeValue(Seconds(timeSimEnd+10)));
          destinationHelper.Install (p2pLeafSpineHelper.GetServerNode (destinationId));
        }
    }
  else
    {
      // If non-DCN workload, simulate an inter-rack flow and an intra-rack flow
      ApplicationContainer clientApps;
      NS_LOG_INFO ("Configure the flow: server 0 => server 6.");
      OnOffHelper clientHelper0 ("ns3::TcpSocketFactory", Address());
      clientHelper0.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
      clientHelper0.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
      AddressValue remoteAddress0 (InetSocketAddress (p2pLeafSpineHelper.GetServerIpv4Address (6), 50000));
      clientHelper0.SetAttribute ("Remote", remoteAddress0);
      clientApps.Add (clientHelper0.Install (p2pLeafSpineHelper.GetServerNode (0)));

      NS_LOG_INFO ("Configure the flow: server 13 => server 15.");
      OnOffHelper clientHelper1 ("ns3::TcpSocketFactory", Address());
      clientHelper1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
      clientHelper1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
      AddressValue remoteAddress1 (InetSocketAddress (p2pLeafSpineHelper.GetServerIpv4Address (15), 50001));
      clientHelper1.SetAttribute ("Remote", remoteAddress1);
      clientApps.Add (clientHelper1.Install (p2pLeafSpineHelper.GetServerNode (13)));  

      NS_LOG_INFO ("Generate 5s of traffic.");
      clientApps.Start (Seconds (0.0));
      clientApps.Stop (Seconds (5.0));
    }

  NS_LOG_INFO ("Populate routing tables.");
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  NS_LOG_INFO ("Configure the animation.");
  p2pLeafSpineHelper.BoundingBox (1, 1, 100, 100);
  AnimationInterface anim (animFile);
  anim.EnablePacketMetadata ();
  anim.EnableIpv4L3ProtocolCounters (Seconds (0), Seconds (timeSimEnd));

  NS_LOG_INFO ("Start running.");
  Simulator::Stop (Seconds(timeSimEnd+10));
  Simulator::Run ();
  NS_LOG_INFO ("Animation Trace file created:" << animFile.c_str ());
  Simulator::Destroy ();
  return 0;
}
