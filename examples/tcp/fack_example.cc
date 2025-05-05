/*
 * Copyright (c) 2018 NITK Surathkal
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *
 *
 * Authors: Shikha Bakshi <shikhabakshi912@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

// The network topology used in this example is based on the Fig. 1 described in
// Mathis, M., & Mahdavi, J. (1996, August).
// Forward acknowledgement: Refining TCP congestion control.
// In ACM SIGCOMM Computer Communication Review (Vol. 26, No. 4, pp. 281-291). ACM.

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/traffic-control-module.h"

#include <filesystem>
#include <iostream>

using namespace ns3;
using namespace ns3::SystemPath;

// All the results will be collected in a folder named fack in the ns-3 root directory
Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
std::string dir = "fack/";
double stopTime = 10;

// Trace cwnd
static void
CwndChange(Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
    *stream->GetStream() << Simulator::Now().GetSeconds() << " " << newCwnd / 1000.0 << std::endl;
}

void
TraceCwnd(uint32_t node, uint32_t cwndWindow, Callback<void, uint32_t, uint32_t> CwndTrace)
{
    Config::ConnectWithoutContext("/NodeList/" + std::to_string(node) +
                                      "/$ns3::TcpL4Protocol/SocketList/" +
                                      std::to_string(cwndWindow) + "/CongestionWindow",
                                  CwndTrace);
}

// Trace drops at queue
static void
DropAtQueue(Ptr<OutputStreamWrapper> stream, Ptr<const QueueDiscItem> item)
{
    *stream->GetStream() << Simulator::Now().GetSeconds() << " 1" << std::endl;
}

// Trace queue length
void
CheckQueueSize(Ptr<QueueDisc> queue)
{
    uint32_t qSize = queue->GetCurrentSize().GetValue();

    // check queue size every 1/100 of a second
    Simulator::Schedule(Seconds(0.001), &CheckQueueSize, queue);

    std::ofstream fPlotQueue(std::stringstream(dir + "queue.plotme").str().c_str(),
                             std::ios::out | std::ios::app);
    fPlotQueue << Simulator::Now().GetSeconds() << " " << qSize << std::endl;
    fPlotQueue.close();
}

void
InstallPacketSink(Ptr<Node> node, uint16_t port)
{
    PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApps = sink.Install(node);
    sinkApps.Start(Seconds(0));
    sinkApps.Stop(Seconds(stopTime));
}

void
InstallBulkSend(Ptr<Node> node,
                Ipv4Address address,
                uint16_t port,
                uint32_t nodeId,
                uint32_t cwndWindow,
                Callback<void, uint32_t, uint32_t> CwndTrace)
{
    BulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(address, port));

    source.SetAttribute("MaxBytes", UintegerValue(0));
    ApplicationContainer sourceApps = source.Install(node);
    Time timeToStart = Seconds(uv->GetValue(0, 1));
    sourceApps.Start(timeToStart);
    Simulator::Schedule(timeToStart + Seconds(0.001), &TraceCwnd, nodeId, cwndWindow, CwndTrace);
    sourceApps.Stop(Seconds(stopTime));
}

int
main(int argc, char* argv[])
{
    uint32_t stream = 1;
    std::string transport_prot = "TcpNewReno";
    std::string queue_disc_type = "FifoQueueDisc";
    bool fack = true;
    uint32_t dataSize = 1000;
    uint32_t delAckCount = 2;

    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y-%I-%M-%S", timeinfo);
    std::string currentTime(buffer);

    CommandLine cmd;
    cmd.AddValue("stream", "Seed value for random variable", stream);
    cmd.AddValue("transport_prot",
                 "Transport protocol to use: TcpNewReno, "
                 "TcpHybla, TcpHighSpeed, TcpHtcp, TcpVegas, TcpScalable, TcpVeno, "
                 "TcpBic, TcpYeah, TcpIllinois, TcpWestwoodPlus, TcpLedbat, "
                 "TcpLp",
                 transport_prot);
    cmd.AddValue("queue_disc_type",
                 "Queue disc type for gateway (e.g. ns3::CoDelQueueDisc)",
                 queue_disc_type);
    cmd.AddValue("dataSize", "Data packet size", dataSize);
    cmd.AddValue("delAckCount", "Delayed ack count", delAckCount);
    cmd.AddValue("stopTime",
                 "Stop time for applications / simulation time will be stopTime",
                 stopTime);
    cmd.AddValue("fack", "Enable/Disable FACK", fack);
    cmd.Parse(argc, argv);

    uv->SetStream(stream);
    transport_prot = std::string("ns3::") + transport_prot;
    queue_disc_type = std::string("ns3::") + queue_disc_type;

    TypeId qdTid;
    NS_ABORT_MSG_UNLESS(TypeId::LookupByNameFailSafe(queue_disc_type, &qdTid),
                        "TypeId " << queue_disc_type << " not found");

    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpNewReno::GetTypeId()));
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(dataSize));
    Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue(1 << 20));
    Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue(1 << 20));
    Config::SetDefault("ns3::TcpSocket::InitialCwnd", UintegerValue(10));
    Config::SetDefault("ns3::TcpSocketBase::Sack", BooleanValue(true));
    Config::SetDefault("ns3::TcpSocketBase::Fack", BooleanValue(fack));
    Config::SetDefault(queue_disc_type + "::MaxSize", QueueSizeValue(QueueSize("17p")));
    Config::SetDefault("ns3::BurstErrorModel::ErrorRate", DoubleValue(0.01));

    AsciiTraceHelper asciiTraceHelper;
    Ptr<OutputStreamWrapper> streamWrapper;

    // Select TCP variant

    TypeId tcpTid;
    NS_ABORT_MSG_UNLESS(TypeId::LookupByNameFailSafe(transport_prot, &tcpTid),
                        "TypeId " << transport_prot << " not found");
    Config::SetDefault("ns3::TcpL4Protocol::SocketType",
                       TypeIdValue(TypeId::LookupByName(transport_prot)));

    // Create nodes
    NodeContainer nodes;
    NodeContainer routers;
    nodes.Create(2);
    routers.Create(2);

    // Create point-to-point channels
    PointToPointHelper p2pSR1;
    PointToPointHelper p2pR;
    PointToPointHelper p2pSR2;
    p2pSR1.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    p2pSR1.SetChannelAttribute("Delay", StringValue("2ms"));

    p2pR.SetDeviceAttribute("DataRate", StringValue("1.5Mbps"));
    p2pR.SetChannelAttribute("Delay", StringValue("5ms"));

    p2pSR2.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    p2pSR2.SetChannelAttribute("Delay", StringValue("33ms"));

    // Create netdevices
    NetDeviceContainer S1R1;
    NetDeviceContainer R1R2;
    NetDeviceContainer R2S2;
    S1R1 = p2pSR1.Install(nodes.Get(0), routers.Get(0));
    R1R2 = p2pR.Install(routers.Get(0), routers.Get(1));
    R2S2 = p2pSR2.Install(routers.Get(1), nodes.Get(1));

    // Install Internet stack on the end nodes and the routers
    InternetStackHelper stack;
    stack.Install(nodes);
    stack.Install(routers);

    // Assign IP addresses
    Ipv4AddressHelper address;
    Ipv4InterfaceContainer S1Int;
    Ipv4InterfaceContainer S2Int;
    Ipv4InterfaceContainer routerInt;

    address.SetBase("10.1.1.0", "255.255.255.252");

    S1Int = address.Assign(S1R1);
    address.NewNetwork();
    routerInt = address.Assign(R1R2);
    address.NewNetwork();
    S2Int = address.Assign(R2S2);

    dir += (currentTime + "/");
    MakeDirectories(dir);
    MakeDirectories(dir + "cwndTraces/");
    MakeDirectories(dir + "queueTraces/");

    // Set Queue Disc
    TrafficControlHelper tch;
    tch.SetRootQueueDisc(queue_disc_type);

    QueueDiscContainer qd;
    tch.Uninstall(R1R2);
    qd = tch.Install(R1R2);
    Simulator::ScheduleNow(&CheckQueueSize, qd.Get(0));

    streamWrapper = asciiTraceHelper.CreateFileStream(dir + "queueTraces/drop.plotme");
    qd.Get(0)->TraceConnectWithoutContext("Drop", MakeBoundCallback(&DropAtQueue, streamWrapper));

    uint16_t port = 50000;

    // Install Sink application
    InstallPacketSink(nodes.Get(1), port);

    // Install BulkSend application on S1
    Ptr<OutputStreamWrapper> stream_cwnd =
        asciiTraceHelper.CreateFileStream(dir + "cwndTraces/cwnd.plotme");
    InstallBulkSend(nodes.Get(0),
                    S2Int.GetAddress(1),
                    port,
                    0,
                    0,
                    MakeBoundCallback(&CwndChange, stream_cwnd));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Stop(Seconds(stopTime));
    Simulator::Run();

    Simulator::Destroy();
    return 0;
}
