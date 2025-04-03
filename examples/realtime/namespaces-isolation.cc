/*
 * Copyright (c) 2024 Firas Khamis
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
 * Author: Firas Khamis <firas.khamis@stud.uni-due.de>
 */


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/tap-bridge-module.h"


using namespace ns3;


/**
 * \file namespaces-isolation.cc
 *
 * This example shows how to use TapBridge to create a simulated switch (B)
 * forwarding packets between two real nodes (A/C).
 *
 * The network topology to achieve:
 *
 * @verbatim
 *            left                                            right
 *     __________________                               __________________
 *                       |                             |
 *              A        |                             |        C
 *           (ns-l)  ====|============  B  ============|====  (ns-r)
 *          10.1.1.1     |    10.1.1.2     10.1.2.1    |     10.1.2.2
 *     __________________|                             |__________________
 * @endverbatim
 *
 * The MAC addresses for the devices will automatically be given at the
 * beginning of the simulation:
 * Node A: 00:00:00:00:00:01
 * Node B: 00:00:00:00:00:02
 * Node C: 00:00:00:00:00:03
 *
 *
 * The following commands are to be executed independently from the
 * simulation on your machine *after* starting the script:
 *
 * @code{.sh}
 * ## Make sure the next commands are excecuted as sudo
 * sudo su
 *
 * ## Create two namespaces
 * ip netns add left
 * ip netns add right
 *
 * ## Move each of the network devices to a namespace
 * ip link set ns-l netns left
 * ip link set ns-r netns right
 *
 * ## Set up the interfaces
 * ip netns exec left ip link set ns-l up
 * ip netns exec left ip link set lo up
 * ip netns exec right ip link set ns-r up
 * ip netns exec right ip link set lo up
 *
 * ## Assign IP addresses
 * ip netns exec left ip address add 10.1.1.1/24 dev ns-l
 * ip netns exec right ip address add 10.1.2.2/24 dev ns-r
 *
 * ## Set default gateway
 * ip netns exec left route add default gw 10.1.1.2
 * ip netns exec right route add default gw 10.1.2.1
 *
 * ## Manually add ARP entries
 * ip netns exec left arp -s 10.1.1.2 00:00:00:00:00:02
 * ip netns exec right arp -s 10.1.2.1 00:00:00:00:00:02
 *
 * ## Ping to test the setup (from either namespace, NOT the host device)
 * ip netns exec (left/right) ping 10.1.1.1
 * ip netns exec (left/right) ping 10.1.2.2
 * @endcode
 *
 * Now both interfaces are isolated each in their own namespace and can
 * communicate through an NS-3 simulated node.
 * 
 * @code{.sh}
 * ## To remove the network namespaces after running the simulation
 * ip netns del left
 * ip netns del right
 * @endcode
 */


int
main (int argc, char *argv[])
{
    GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
    GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));


    // Create 3 nodes A, B and C
    NodeContainer nodes;
    nodes.Create (3);


    // Set up point-to-point links
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));


    NetDeviceContainer devices_a_b;
    devices_a_b = p2p.Install(nodes.Get(0), nodes.Get(1));


    NetDeviceContainer devices_b_c;
    devices_b_c = p2p.Install(nodes.Get(1), nodes.Get(2));


    InternetStackHelper stack;
    stack.Install(nodes);


    // Addresses for the devices A B
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces_a_b = address.Assign(devices_a_b);


    // Addresses for the devices B C
    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces_b_c = address.Assign(devices_b_c);


    // Set up net devices
    TapBridgeHelper tapBridgeLeft;
    tapBridgeLeft.SetAttribute("DeviceName", StringValue("ns-l"));
    tapBridgeLeft.SetAttribute("Mode", StringValue("ConfigureLocal"));
    tapBridgeLeft.Install(nodes.Get(0), devices_a_b.Get(0));
    TapBridgeHelper tapBridgeRight;
    tapBridgeRight.SetAttribute("DeviceName", StringValue("ns-r"));
    tapBridgeRight.SetAttribute("Mode", StringValue("ConfigureLocal"));
    tapBridgeRight.Install(nodes.Get(2), devices_b_c.Get(1));


    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    std::cout << "The script is running. Refer to the information in the code's comments." << std::endl;
    Simulator::Run ();
    Simulator::Destroy ();


    return 0;
}
