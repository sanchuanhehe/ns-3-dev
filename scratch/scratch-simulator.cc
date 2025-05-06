/*
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ScratchSimulator");

void
ThroughputMonitor(Ptr<PacketSink> sink, double startTime, double interval)
{
    static double lastTotalRx = 0;
    double currentTotalRx = sink->GetTotalRx();
    double throughput = (currentTotalRx - lastTotalRx) * 8 / (interval * 1e6); // Mbps
    lastTotalRx = currentTotalRx;

    NS_LOG_UNCOND(Simulator::Now().GetSeconds() << "s: Throughput = " << throughput << " Mbps");
    Simulator::Schedule(Seconds(interval), &ThroughputMonitor, sink, startTime, interval);
}

void
SnrCallback(Ptr<const Packet> packet, unsigned short channelFrequencyMhz, WifiTxVector txVector, MpduInfo mpduInfo, SignalNoiseDbm signalNoise, unsigned short channelWidth)
{
    double snr = signalNoise.signal - signalNoise.noise; // 计算 SNR
    NS_LOG_UNCOND(Simulator::Now().GetSeconds() << "s: SNR = " << snr << " dB");
}

void
SnrMonitor(Ptr<NetDevice> device)
{
    Ptr<WifiNetDevice> wifiDevice = DynamicCast<WifiNetDevice>(device);
    if (wifiDevice)
    {
        Ptr<WifiPhy> phy = wifiDevice->GetPhy();
        phy->TraceConnectWithoutContext("MonitorSnifferRx", MakeCallback(&SnrCallback));
    }
}

int
main(int argc, char* argv[])
{
    double simuTime = 35.0;
    double velocity = 2.0; // 速度，单位：米/秒
    double interval = 1.0; // 吞吐量监控的时间间隔（秒）

    // 创建节点
    NodeContainer wifiApNodes;
    wifiApNodes.Create(1);
    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(1);

    // 配置WiFi信道和物理层
    YansWifiPhyHelper phy;
    // 利用YansWifiChannelHelper进行物理层信道设置
    YansWifiChannelHelper channel;
    // 设置一个衰落模型以及这个衰落模型依赖的特定参数（Frequency是FriisPropagationLossModel计算衰减时用的一个参数）
    channel.AddPropagationLoss("ns3::FriisPropagationLossModel",
                               "Frequency",
                               DoubleValue(2412 * 1e6));
    // 设置传播时延模型为光速传播
    channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    // 创建信道实例，并设置在phyhelper里
    phy.SetChannel(channel.Create());
    // 设置物理层占用的{子信道，带宽，频段，主信道索引}
    phy.Set("ChannelSettings", StringValue("{0, 20, BAND_2_4GHZ, 0}"));

    // 配置WiFi MAC层和设备
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ax);
    wifi.SetRemoteStationManager("ns3::IdealWifiManager");

    WifiMacHelper wifiMac;
    Ssid ssid = Ssid("AP");

    wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer staDevices = wifi.Install(phy, wifiMac, wifiStaNodes);

    wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer apDevices = wifi.Install(phy, wifiMac, wifiApNodes);

    // 配置移动性
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0)); // AP位置
    positionAlloc->Add(Vector(5.0, 0.0, 0.0)); // STA初始位置
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    mobility.Install(wifiApNodes);
    mobility.Install(wifiStaNodes);

    Ptr<ConstantVelocityMobilityModel> mob =
        wifiStaNodes.Get(0)->GetObject<ConstantVelocityMobilityModel>();
    mob->SetVelocity(Vector(velocity, 0.0, 0.0));

    // 配置IP协议栈
    InternetStackHelper stack;
    stack.Install(wifiApNodes);
    stack.Install(wifiStaNodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(NetDeviceContainer(apDevices, staDevices));

    // 配置应用程序
    uint16_t port = 9;
    Address sinkAddress(InetSocketAddress(interfaces.GetAddress(1), port));
    PacketSinkHelper sinkHelper("ns3::UdpSocketFactory", sinkAddress);
    ApplicationContainer sinkApp = sinkHelper.Install(wifiStaNodes.Get(0));
    sinkApp.Start(Seconds(0.5));
    sinkApp.Stop(Seconds(simuTime));

    OnOffHelper onOffHelper("ns3::UdpSocketFactory", sinkAddress);
    onOffHelper.SetConstantRate(DataRate("300Mb/s"), 1420);
    onOffHelper.SetAttribute("StartTime", TimeValue(Seconds(0.5)));
    onOffHelper.SetAttribute("StopTime", TimeValue(Seconds(simuTime)));
    ApplicationContainer sourceApp = onOffHelper.Install(wifiApNodes.Get(0));

    // 监控吞吐量
    Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApp.Get(0));
    Simulator::Schedule(Seconds(interval), &ThroughputMonitor, sink, 0.5, interval);

    // 监控信噪比
    // Ptr<WifiPhyStateHelper> phyStateHelper = CreateObject<WifiPhyStateHelper>();
    SnrMonitor(apDevices.Get(0));

    // 运行仿真
    Simulator::Stop(Seconds(simuTime));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}