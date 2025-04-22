/*
 * Copyright (c) 2020 Tom Henderson
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
 */

#include "ns3/net-device-state.h"
#include "ns3/simple-channel.h"
#include "ns3/simple-net-device.h"
#include "ns3/test.h"
#include "ns3/traced-callback.h"

using namespace ns3;

/**
 * \ingroup network-test
 * \ingroup tests
 *
 * \brief This test suite demonstrates and checks the basic operation of the
 * NetDeviceState class.  A NetDeviceState object can be aggregated
 * to any NetDevice type.  Any number of objects can listen for state
 * changes.  Usually, the NetDevice or associated Channel will cause
 * state transitions (and will call the public methods of NetDeviceState
 * to change its state), but this basic test just hooks two listeners
 * to the state change trace source, and uses the public API to invoke
 * state changes that are checked in the listening methods.
 */
class NetDeviceStateTest : public TestCase
{
  public:
    /**
     * \brief Function 1 to catch StateChange trace in NetDeviceState
     *
     * \param isUp Administrative state of NetDevice
     * \param opState Operational state of NetDevice
     */
    void Listener1(bool isUp, NetDeviceState::OperationalState opState);

    /**
     * \brief Function 2 to catch StateChange trace in NetDeviceState
     *
     * \param isUp Administrative state of NetDevice
     * \param opState Operational state of NetDevice
     */
    void Listener2(bool isUp, NetDeviceState::OperationalState opState);

    /**
     * \brief Run the actual test case. Implementation of base class function.
     */
    void DoRun() override;
    NetDeviceStateTest();

  private:
    uint16_t m_listener1Count{0}; //!< counter for Listener1
    uint16_t m_listener2Count{0}; //!< counter for Listener2
};

NetDeviceStateTest::NetDeviceStateTest()
    : TestCase("NetDeviceState basic test")
{
}

void
NetDeviceStateTest::DoRun()
{
    Ptr<SimpleNetDevice> device = CreateObject<SimpleNetDevice>();
    Ptr<NetDeviceState> state = CreateObject<NetDeviceState>();
    // A helper will typically aggregate this object to the NetDevice instance
    device->AggregateObject(state);

    // More than one object can listen to the trace source
    state->TraceConnectWithoutContext("StateChange",
                                      MakeCallback(&NetDeviceStateTest::Listener1, this));
    state->TraceConnectWithoutContext("StateChange",
                                      MakeCallback(&NetDeviceStateTest::Listener2, this));

    // Check the expected initial state
    NS_TEST_EXPECT_MSG_EQ(state->IsUp(), true, "NetDeviceState created in down state");
    NS_TEST_EXPECT_MSG_EQ(state->GetOperationalState(),
                          NetDeviceState::IF_OPER_DOWN,
                          "State created in operational state other than down");

    // Check the operation of state transitions
    state->SetUp();
    NS_TEST_EXPECT_MSG_EQ(state->IsUp(), true, "NetDeviceState failed to transition to up");
    state->SetDown();
    NS_TEST_EXPECT_MSG_EQ(state->IsUp(), false, "NetDeviceState failed to transition to down");
    state->SetOperationalState(NetDeviceState::OperationalState::IF_OPER_UP);
    NS_TEST_EXPECT_MSG_EQ(state->GetOperationalState(),
                          NetDeviceState::IF_OPER_UP,
                          "State failed to transition to operational up");
    NS_TEST_EXPECT_MSG_EQ(m_listener1Count, 2, "Expected three transitions");
    NS_TEST_EXPECT_MSG_EQ(m_listener2Count, 2, "Expected three transitions");
}

void
NetDeviceStateTest::Listener1(bool isUp, NetDeviceState::OperationalState opState)
{
    m_listener1Count++;
}

void
NetDeviceStateTest::Listener2(bool isUp, NetDeviceState::OperationalState opState)
{
    m_listener2Count++;
}

/**
 * \ingroup network-test
 * \ingroup tests
 *
 * \brief NetDeviceState TestSuite: Test out NetDeviceState capabilities.
 */
class NetDeviceStateTestSuite : public TestSuite
{
  public:
    NetDeviceStateTestSuite()
        : TestSuite("net-device-state", UNIT)
    {
        AddTestCase(new NetDeviceStateTest, TestCase::QUICK);
    }
};

static NetDeviceStateTestSuite
    g_netDeviceStateTestSuite; //!< Static variable for test initialization
