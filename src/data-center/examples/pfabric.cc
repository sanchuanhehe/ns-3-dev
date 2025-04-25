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
#include "ns3/core-module.h"
#include "ns3/data-center-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-nix-vector-helper.h"
#include "ns3/ipv4-static-routing.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/traffic-control-module.h"

// The simulation provides an example of simulating data center networks covering the 
// configuration of network scheduling, load balancing, congestion control and so on. 
// The simulation setting approximiates the work in data center networking, pFabric: 
// Minimal Near-Optimal Datacenter Transport.

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Pfabric");

int main (int argc, char *argv[])
{
  uint32_t    seed=2019;

  uint32_t    numSpine = 4;
  uint32_t    numLeaf = 9;
  uint32_t    numServerPerLeaf = 16;
  uint32_t    leafBandwidth=10000;  // Mbps
  uint32_t    spineBandwidth=40000;  // Mbps
  uint32_t    leafDelay=100;	// ns
  uint32_t    spineDelay=10;	// ns  
  uint32_t    timeSimEnd = 5;
  uint32_t    numFlowsMax = 100;
  double      load = 0.5;  // Utilization of the network w.r.t. the aggregate server bandwidth, set 50% by default
  std::string workloadType = "DCTCP";
  // FifoQueueDisc or SjfQueueDisc
  // std::string coreQueueDiscName = "SjfQueueDisc";
  // std::string edgeQueueDiscName = "SjfQueueDisc";
  std::string coreQueueDiscName = "FifoQueueDisc";
  std::string edgeQueueDiscName = "FifoQueueDisc";  
  std::string loadbalancingName = "RandomEcmpRouting";

  CommandLine cmd;
  cmd.AddValue ("numSpine", "Number of spine switches", numSpine);
  cmd.AddValue ("numLeaf", "Number of leaf switches", numLeaf);
  cmd.AddValue ("numServerPerLeaf", "Number of servers per leaf switch", numServerPerLeaf);
	cmd.AddValue ("leafBandwidth", "Mbps", leafBandwidth);
	cmd.AddValue ("spineBandwidth", "Mbps", spineBandwidth);  
	cmd.AddValue ("leafDelay", "ns", leafDelay);
	cmd.AddValue ("spineDelay", "ns", spineDelay);  
	cmd.AddValue ("coreQueueDiscName", "The scheduling principle for the network core switches", coreQueueDiscName);
  cmd.AddValue ("edgeQueueDiscName", "The scheduling principle for the network edge hosts", edgeQueueDiscName);
	cmd.AddValue ("loadbalancingName", "The background load balancing method", loadbalancingName);
  cmd.AddValue ("workloadType", "Type of the flow size distribtion (VL2/DCTCP)", workloadType);

  cmd.AddValue ("numFlowsMax", "Maximum number of flows to simulate", numFlowsMax);
  cmd.AddValue ("simSeed", "Random seed", seed);
  cmd.AddValue ("timeSimEnd", "Simulation time [s]", timeSimEnd);  
  
  cmd.Parse (argc,argv);

  SeedManager::SetSeed (2019);

  // Create the point-to-point link helpers
  PointToPointHelper p2pLeafSpine;
  p2pLeafSpine.SetDeviceAttribute  ("DataRate", StringValue (std::to_string (spineBandwidth)+"Mbps"));
  p2pLeafSpine.SetChannelAttribute ("Delay", StringValue (std::to_string (spineDelay)+"ns"));
	p2pLeafSpine.SetDeviceAttribute ("Mtu", UintegerValue(1500));
  p2pLeafSpine.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("1p"));

  PointToPointHelper p2pServerLeaf;
  p2pServerLeaf.SetDeviceAttribute  ("DataRate", StringValue (std::to_string (leafBandwidth)+"Mbps"));
  p2pServerLeaf.SetChannelAttribute ("Delay", StringValue (std::to_string (leafDelay)+"ns"));
	p2pServerLeaf.SetDeviceAttribute ("Mtu", UintegerValue(1500));
  p2pServerLeaf.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("1p"));

  LeafSpineHelper p2pLeafSpineHelper (numSpine, numLeaf, numServerPerLeaf);

  p2pLeafSpineHelper.InstallNetDevices (p2pServerLeaf, p2pLeafSpine);

  // Configure the congestion control method
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (1458));
  Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (204800000));
  Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (204800000));
	Config::SetDefault ("ns3::TcpSocketBase::MinRto", TimeValue(MicroSeconds(200)));
	Config::SetDefault ("ns3::TcpSocket::InitialCwnd", UintegerValue (200));
	Config::SetDefault ("ns3::TcpSocketBase::Sack", BooleanValue (true));  
  // Use TcpNewReno as in pFabric, one could compare it with DCTCP
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));
  if (loadbalancingName.compare ("RandomEcmpRouting") == 0)
  	{
      // Diable fast retransmission for packet spraying as in pFabric
  		Config::SetDefault ("ns3::TcpSocketBase::ReTxThreshold", UintegerValue(std::numeric_limits<uint32_t>::max ()-1));
    }

  // Configure internet stack and set up the load balancing algorithm
  if (loadbalancingName.compare ("RandomEcmpRouting") == 0)
    {
      // pFabric uses the packet spraying
      Config::SetDefault ("ns3::Ipv4GlobalRouting::EcmpRoutingMode", EnumValue (Ipv4GlobalRouting::RandomEcmpRouting));
    }
  else if (loadbalancingName.compare ("FlowBasedEcmpRouting") == 0)
    {
      // Include the per-flow ECMP for comparison
      Config::SetDefault ("ns3::Ipv4GlobalRouting::EcmpRoutingMode", EnumValue (Ipv4GlobalRouting::FlowBasedEcmpRouting));
    }  
  InternetStackHelper stack;
  Ipv4GlobalRoutingHelper globalRouting;
  Ipv4ListRoutingHelper list;
  list.Add (globalRouting, 10);
	stack.SetRoutingHelper(list);      

  // Install Stack
  p2pLeafSpineHelper.InstallStack (stack);

  // Configure scheduling methods for all nodes
  // Large buffer size for conveniences given that the priority drop policy is not applied
  Config::SetDefault ("ns3::QueueBase::MaxSize", StringValue ("4294967295p"));
  TrafficControlHelper tchCore;
  TrafficControlHelper tchEdge;
  // pFabric uses fine-grained SJF scheduling, we include FifoQueueDisc for comparison
  if (coreQueueDiscName.compare ("SjfQueueDisc") == 0)
    {
      // SjfQueueDisc does not support priority dropping
      tchCore.SetRootQueueDisc ("ns3::SjfQueueDisc");
    }    
  else if (coreQueueDiscName.compare ("FifoQueueDisc") == 0)
    {
      tchCore.SetRootQueueDisc ("ns3::FifoQueueDisc");
    }
  else
    {
      NS_LOG_INFO("coreQueueDiscName out of the scope!");
    }

  if (edgeQueueDiscName.compare ("SjfQueueDisc") == 0)
    {
      // SjfQueueDisc does not support priority dropping
      tchEdge.SetRootQueueDisc ("ns3::SjfQueueDisc");
    } 
  else if (edgeQueueDiscName.compare ("FifoQueueDisc") == 0)
    {
      tchEdge.SetRootQueueDisc ("ns3::FifoQueueDisc");
    }
  else
    {
      NS_LOG_INFO("edgeQueueDiscName out of the scope!");
    }
  p2pLeafSpineHelper.InstallTrafficControl (tchCore, tchEdge);  

  // Assign ipv4 addresses
  p2pLeafSpineHelper.AssignIpv4Addresses (Ipv4Address ("10.0.0.0"), Ipv4Mask ("255.255.255.0"));

  /* 
   * Configure flows based on typical DCN workloads used by pFabric: 
   * - Data mining workload (a.k.a. VL2, mean flow size 2126KB), source: A.Greenberg, J.R.Hamilton, N.Jain, S.Kandula, C.Kim, P.Lahiri, D.A. Maltz, P. Patel, and S. Sengupta. VL2: a scalable and flexible data center network. In Proc. of SIGCOMM, 2009.
   * - Web search workload (a.k.a. DCTCP, mean flow size 1134KB), source: M. Alizadeh, A. Greenberg, D. A. Maltz, J. Padhye, P. Patel, B. Prabhakar, S. Sengupta, and M. Sridharan. Data center TCP (DCTCP). In Proc. of SIGCOMM, 2010.
   */
  NS_LOG_INFO("Configure random variables for synthesizing the traffic.");
  Ptr<ExponentialRandomVariable> flowInterval = CreateObject<ExponentialRandomVariable> ();
  Ptr<EmpiricalRandomVariable> flowSizeCdf = CreateObject<EmpiricalRandomVariable> ();
  double flowRate;  // Mean number of flow arrivals per second for the network
	if (workloadType.compare ("DCTCP") == 0)
    {
      flowRate = static_cast<double>((leafBandwidth*p2pLeafSpineHelper.ServerCount()*1000.0*load)/(8.0*1134));
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
	else if (workloadType.compare ("VL2") == 0) 
    {
      flowRate = static_cast<double>((leafBandwidth*p2pLeafSpineHelper.ServerCount()*1000.0*load)/(8.0*2126));
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
  clientHelper.SetAttribute ("PacketSize", UintegerValue (1458));
  clientHelper.SetAttribute ("DataRate", DataRateValue (std::to_string(leafBandwidth)+"Mbps"));

  ApplicationContainer sinkApps;
  double flowStart = 0;
  uint32_t port = 10000;
  uint32_t sourceId, destinationId, flowId;
  if ((workloadType.compare ("DCTCP") == 0) || (workloadType.compare ("VL2") == 0))
    {
    	for (flowId = 0; flowStart <= timeSimEnd ;flowId++, port++)
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
          if ((edgeQueueDiscName.compare ("SjfQueueDisc") == 0) || (coreQueueDiscName.compare ("SjfQueueDisc") == 0))
            {
              clientHelper.SetAttribute ("FlowSizeTagInclude", BooleanValue (true));
            }           
          clientHelper.Install (p2pLeafSpineHelper.GetServerNode (sourceId));

          destinationHelper.SetAttribute ("Local", remoteAddress);
          destinationHelper.SetAttribute ("StartTime", TimeValue(Seconds(0)));
          destinationHelper.SetAttribute ("StopTime", TimeValue(Seconds(timeSimEnd+10)));
          destinationHelper.Install (p2pLeafSpineHelper.GetServerNode (destinationId));
        }
    }
  else
    {
      NS_LOG_INFO("workloadType out of the scope!");
    }

  NS_LOG_INFO ("Populate routing tables.");
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Install FlowMon
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();

  NS_LOG_INFO ("Start running.");
  Simulator::Stop (Seconds(timeSimEnd+10));
  Simulator::Run ();

  // Calculate the average Flow Completion Time (FCT) and the slowdown for the performance evaluation.
  NS_LOG_INFO ("Calculate the flow completion time.");
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  uint32_t numFlows = 0;
  double sumFct =0;
  double sumSlowdown = 0;
  double tmpFct, tmpIdealFct;
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
      Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow (i->first);
      if ((tuple.destinationPort >= port) || (tuple.destinationPort < port-flowId-1))
        {
          continue;
        }
      numFlows += 1;
      tmpFct = i->second.timeLastRxPacket.GetSeconds () - i->second.timeFirstTxPacket.GetSeconds ();
      tmpIdealFct = double(i->second.txBytes*8.0/(leafBandwidth*1000000.0));
      sumFct += tmpFct;
      sumSlowdown += tmpFct/tmpIdealFct;
      std::cout << "Flow completion time for the flow " << i->first << " (" << tuple.sourceAddress << " -> " << 
        tuple.destinationAddress << "): " << tmpFct << "s; receiving bytes: " << i->second.rxBytes << "; transmitted bytes: "
        << i->second.txBytes << "; time first packet transmitted: " << i->second.timeFirstTxPacket << "s; time last packet received: "
        << i->second.timeLastRxPacket << std::endl;
    }
  std::cout << "Number of flows: " << numFlows << std::endl;
  std::cout << "Average flow completion time: " << sumFct/numFlows << std::endl;
  std::cout << "Average slowdown: " << sumSlowdown/numFlows << std::endl;

  Simulator::Destroy ();
  return 0;
}
