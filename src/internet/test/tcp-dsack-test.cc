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

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TcpDsackTest");

/**
 * @ingroup internet-test
 *
 * @brief Test case for validating TCP D-SACK behavior.
 *
 * This class creates a sender-receiver TCP connection and checks whether
 * the TCP stack correctly enables SACK and D-SACK options based on input flags.
 */
class TcpDsackSackTest : public TcpGeneralTest
{
  public:
    /**
     * @brief Constructor for the D-SACK test case.
     * @param sack Whether SACK option is enabled.
     * @param dsack Boolean variable to enable or disable D-SACK.
     * @param msg Test message.
     */
    TcpDsackSackTest(bool sack, bool dsack, const std::string& msg);

    Ptr<TcpSocketMsgBase> CreateSenderSocket(Ptr<Node> node) override;

  protected:
    void Tx(const Ptr<const Packet> p, const TcpHeader& h, SocketWho who) override;

    /** @brief Indicates whether SACK is enabled. */
    bool m_sackState;
    /** @brief Indicates whether D-SACK is enabled. */
    bool m_dsackState;
};

TcpDsackSackTest::TcpDsackSackTest(bool sack, bool dsack, const std::string& msg)
    : TcpGeneralTest(msg),
      m_sackState(sack),
      m_dsackState(dsack)
{
}

Ptr<TcpSocketMsgBase>
TcpDsackSackTest::CreateSenderSocket(Ptr<Node> node)
{
    Ptr<TcpSocketMsgBase> socket = TcpGeneralTest::CreateSenderSocket(node);
    socket->SetAttribute("DSack", BooleanValue(m_dsackState));
    socket->SetAttribute("Sack", BooleanValue(m_sackState));
    return socket;
}

void
TcpDsackSackTest::Tx(const Ptr<const Packet> p, const TcpHeader& h, SocketWho who)
{
    if ((h.GetFlags() & TcpHeader::SYN))
    {
        std::cout << h.HasOption(TcpOption::SACKPERMITTED);
        NS_TEST_ASSERT_MSG_EQ(h.HasOption(TcpOption::SACKPERMITTED),
                              true,
                              "SackPermitted disabled but option enabled");
    }
}

/**
 * @ingroup internet-test
 * @ingroup tests
 *
 * @brief Testsuite for the D-SACK
 */
class TcpDsackTestSuite : public TestSuite
{
  public:
    TcpDsackTestSuite()
        : TestSuite("tcp-dsack-test", UNIT)
    {
        AddTestCase(new TcpDsackSackTest(true, true, "Sack enable"), TestCase::QUICK);
        AddTestCase(new TcpDsackSackTest(false, true, "Sack disable"), TestCase::QUICK);
    }
};

static TcpDsackTestSuite g_TcpDsackTestSuite; //!< Static variable for test initialization
