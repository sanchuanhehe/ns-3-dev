/*
 * Copyright (c) 2018 NITK Surathkal
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 *
 *
 * Authors: Shikha Bakshi <shikhabakshi912@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

#include "tcp-error-model.h"
#include "tcp-general-test.h"

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/simple-channel.h"
#include "ns3/tcp-header.h"
#include "ns3/tcp-tx-buffer.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TcpFackTest");

/**
 * @ingroup internet-test
 *
 * @brief Constructor for TcpFackTest that sets up the test scenario
 *        with a specific congestion control algorithm, sequence number
 *        to drop, and total number of packets to send.
 *
 * @param congControl The TypeId of the congestion control algorithm
 * @param seqToKill The sequence number of the packet to drop during the test
 * @param numOfPkts The total number of packets to send
 * @param msg A descriptive label for the test case
 */
class TcpFackTest : public TcpGeneralTest
{
  public:
    /**
     * @brief Constructor for TcpFackTest
     *
     * Initializes the test with given congestion control, starting sequence to drop,
     * number of packets to simulate loss for, and a description.
     *
     * @param congControl The TypeId of the congestion control algorithm
     * @param seqToKill The first sequence number to simulate packet drop for
     * @param numOfPkts Number of packets to drop
     * @param msg Descriptive message for this test case
     */
    TcpFackTest(TypeId congControl, uint32_t seqToKill, uint32_t numOfPkts, const std::string& msg);

    Ptr<ErrorModel> CreateSenderErrorModel() override;
    Ptr<ErrorModel> CreateReceiverErrorModel() override;

    Ptr<TcpSocketMsgBase> CreateSenderSocket(Ptr<Node> node) override;

  protected:
    void RcvAck(const Ptr<const TcpSocketState> tcb, const TcpHeader& h, SocketWho who) override;

    void CongStateTrace(const TcpSocketState::TcpCongState_t oldValue,
                        const TcpSocketState::TcpCongState_t newValue) override;

    /**
     * @ingroup internet-test
     *
     * @brief Callback function that is triggered when a packet is dropped
     *        during the simulation. Used for validating packet loss behavior.
     *
     * @param ipH IPv4 header of the dropped packet
     * @param tcpH TCP header of the dropped packet
     * @param p Pointer to the dropped packet
     */
    void PktDropped(const Ipv4Header& ipH, const TcpHeader& tcpH, Ptr<const Packet> p);
    void ConfigureProperties() override;
    void ConfigureEnvironment() override;

    uint32_t m_pktDropped;     //!< The packet has been dropped.
    uint32_t m_startSeqToKill; //!< Sequence number of the first packet to drop.
    uint32_t m_seqToKill;      //!< Sequence number to drop.
    uint32_t m_pkts;           //!< Number of packets to drop.
    uint32_t m_dupAckReceived; //!< DupACk received.
    uint32_t m_sndFack;        //!< Forward AckManagement.
    uint32_t m_unacked;        //!< First Byte of Unacknowledged data
    uint32_t m_pktSize;        //!< Sender Packet Size

    Ptr<TcpSeqErrorModel> m_errorModel; //!< Error model.
};

TcpFackTest::TcpFackTest(TypeId typeId,
                         uint32_t startSeqToKill,
                         uint32_t numOfPkts,
                         const std::string& msg)
    : TcpGeneralTest(msg),
      m_pktDropped(0),
      m_startSeqToKill(startSeqToKill),
      m_seqToKill(startSeqToKill),
      m_pkts(numOfPkts),
      m_dupAckReceived(0),
      m_sndFack(5001),
      m_unacked(1),
      m_pktSize(500)
{
    m_congControlTypeId = typeId;
}

void
TcpFackTest::ConfigureProperties()
{
    TcpGeneralTest::ConfigureProperties();
    SetInitialSsThresh(SENDER, 0);
    SetInitialCwnd(SENDER, 10);
    SetSegmentSize(SENDER, m_pktSize);
}

void
TcpFackTest::ConfigureEnvironment()
{
    TcpGeneralTest::ConfigureEnvironment();
    SetAppPktCount(10);
}

Ptr<ErrorModel>
TcpFackTest::CreateSenderErrorModel()
{
    return nullptr;
}

Ptr<ErrorModel>
TcpFackTest::CreateReceiverErrorModel()
{
    m_errorModel = CreateObject<TcpSeqErrorModel>();

    for (uint32_t i = 0; i < m_pkts; i++)
    {
        m_seqToKill = m_startSeqToKill + i * m_pktSize;
        m_errorModel->AddSeqToKill(SequenceNumber32(m_seqToKill));
        m_errorModel->SetDropCallback(MakeCallback(&TcpFackTest::PktDropped, this));
    }

    return m_errorModel;
}

Ptr<TcpSocketMsgBase>
TcpFackTest::CreateSenderSocket(Ptr<Node> node)
{
    Ptr<TcpSocketMsgBase> socket = TcpGeneralTest::CreateSenderSocket(node);
    socket->SetAttribute("MinRto", TimeValue(Seconds(10.0)));
    socket->SetAttribute("Fack", BooleanValue(true));
    return socket;
}

void
TcpFackTest::RcvAck(const Ptr<const TcpSocketState> tcb, const TcpHeader& h, SocketWho who)
{
    NS_LOG_FUNCTION(this << tcb << h << who);

    if (h.GetAckNumber().GetValue() == m_startSeqToKill && GetDupAckCount(SENDER) == 1 &&
        m_pktDropped == m_pkts)
    {
        m_sndFack = 5001;
    }
    Ptr<TcpTxBuffer> tx = GetTxBuffer(SENDER);
    m_unacked = tx->HeadSequence().GetValue();
}

// Trace TCP congestion state
void
TcpFackTest::CongStateTrace(const TcpSocketState::TcpCongState_t oldValue,
                            const TcpSocketState::TcpCongState_t newValue)
{
    NS_LOG_FUNCTION(this << oldValue << newValue);

    if (oldValue == TcpSocketState::CA_DISORDER && newValue == TcpSocketState::CA_RECOVERY)
    {
        NS_TEST_ASSERT_MSG_GT(m_sndFack - m_unacked, 3 * m_pktSize, "DISORDER to RECOVERY Invalid");
    }
}

// Count number of packets dropped
void
TcpFackTest::PktDropped(const Ipv4Header& ipH, const TcpHeader& tcpH, Ptr<const Packet> p)
{
    NS_LOG_FUNCTION(this << ipH << tcpH);
    m_pktDropped++;
}

/**
 * @ingroup internet-test
 *
 * @brief Test suite for validating the behavior of the TCP FACK implementation
 *        under controlled packet loss scenarios.
 */
class TcpFackTestSuite : public TestSuite
{
  public:
    TcpFackTestSuite()
        : TestSuite("tcp-fack-test", UNIT)
    {
        std::list<TypeId> types;
        types.insert(types.begin(), TcpNewReno::GetTypeId());

        for (auto it = types.begin(); it != types.end(); ++it)
        {
            AddTestCase(new TcpFackTest((*it), 2501, 4, "Fack testing"), TestCase::QUICK);
        }
    }
};

static TcpFackTestSuite g_TcpFackTestSuite; //!< Static variable for test initialization
