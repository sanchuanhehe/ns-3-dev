/*
 * Copyright (c) 2024 Tom Henderson
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

#include "ns3/test.h"
#include "ns3/units.h"

#include <type_traits>

using namespace ns3;
using namespace units;
using namespace units::dimensionless;
using namespace units::frequency;
using namespace units::literals;
using namespace units::power;

// Note: The units library nholthaus/units maintains its own unit test suite,
// based on Google C++ Test framework.  Below tests are for ns-3-specific usage.

/**
 * \defgroup units-tests Tests for units
 * \ingroup units
 * \ingroup tests
 */

/**
 * \ingroup units-tests
 * Test case for frequency units
 */
class UnitsFrequencyTestCase : public TestCase
{
  public:
    UnitsFrequencyTestCase();

  private:
    void DoRun() override;
};

UnitsFrequencyTestCase::UnitsFrequencyTestCase()
    : TestCase("Test units for frequency")
{
}

void
UnitsFrequencyTestCase::DoRun()
{
    hertz_t fiveHz{5};
    auto fiveHzTwo{5_Hz};
    NS_TEST_ASSERT_MSG_EQ(fiveHz, fiveHzTwo, "Check literal initialization");
    NS_TEST_ASSERT_MSG_EQ(fiveHz.to<double>(), 5, "Check double conversion");

    megahertz_t fiveMHz{5};
    auto fiveMHzTwo{5_MHz};
    NS_TEST_ASSERT_MSG_EQ(fiveMHz, fiveMHzTwo, "Check literal initialization");
    NS_TEST_ASSERT_MSG_EQ(fiveMHz.to<double>(), 5, "Check double conversion");

    auto tenMHz = 2 * fiveMHz;
    megahertz_t tenMHzTwo{10};
    NS_TEST_ASSERT_MSG_EQ(tenMHz, tenMHzTwo, "Check multiplication by scalar");
    auto fiveMHzThree = tenMHz / 2;
    NS_TEST_ASSERT_MSG_EQ(fiveMHz, fiveMHzThree, "Check division by scalar");

    hertz_t sum = fiveMHz + fiveHz;
    NS_TEST_ASSERT_MSG_EQ(sum.to<double>(), 5000005, "Check addition of compatible units");
    NS_TEST_ASSERT_MSG_EQ(sum, hertz_t(5000005), "Check addition of compatible units");
    NS_TEST_ASSERT_MSG_EQ(sum, megahertz_t(5.000005), "Check addition of compatible units");

    hertz_t difference = fiveMHz - fiveHz;
    NS_TEST_ASSERT_MSG_EQ(difference.to<double>(),
                          4999995,
                          "Check subtraction of compatible units");
    hertz_t negativeDifference = fiveHz - fiveMHz;
    NS_TEST_ASSERT_MSG_EQ(negativeDifference.to<double>(),
                          -4999995,
                          "Frequency is allowed to be negative");

    hertz_t halfHz{0.5};
    NS_TEST_ASSERT_MSG_EQ(halfHz.to<double>(), 0.5, "Check fractional frequency");
}

/**
 * \ingroup units-tests
 * Test case for power units
 */
class UnitsPowerTestCase : public TestCase
{
  public:
    UnitsPowerTestCase();

  private:
    void DoRun() override;
};

UnitsPowerTestCase::UnitsPowerTestCase()
    : TestCase("Test units for power")
{
}

void
UnitsPowerTestCase::DoRun()
{
    milliwatt_t hundredmw{100};
    milliwatt_t hundredmwTwo{100_mW};
    NS_TEST_ASSERT_MSG_EQ(hundredmw, hundredmwTwo, "Check literal initialization");
    NS_TEST_ASSERT_MSG_EQ(hundredmw.to<double>(), 100, "Check double conversion");
    watt_t hundredmwThree(hundredmw);
    watt_t hundredmwFour(0.1);
    NS_TEST_ASSERT_MSG_EQ(hundredmwThree, hundredmwFour, "Check unit conversion");
    NS_TEST_ASSERT_MSG_EQ(hundredmwThree.to<double>(), 0.1, "Check double conversion");
    watt_t oneWatt{1};
    watt_t sum = oneWatt + hundredmw;
    NS_TEST_ASSERT_MSG_EQ(sum.to<double>(), 1.1, "Check sum of compatible units");
    watt_t difference = oneWatt - hundredmw;
    NS_TEST_ASSERT_MSG_EQ(difference.to<double>(), 0.9, "Check difference of compatible units");

    dBm_t hundredmwDbm(hundredmw);
    NS_TEST_ASSERT_MSG_EQ(hundredmwDbm.to<double>(), 20, "Check mW to dBm conversion");
    NS_TEST_ASSERT_MSG_EQ(milliwatt_t(hundredmwDbm),
                          hundredmw,
                          "Check conversion from dBm back to mW");

    dB_t tenDb(10);
    dBm_t oneWattTwo = tenDb + hundredmwDbm;
    NS_TEST_ASSERT_MSG_EQ(oneWatt, oneWattTwo, "Check addition of dB to dBm");

    dBm_t tenmwDbm(10);
    auto differenceDbm = hundredmwDbm - tenmwDbm;
    NS_TEST_ASSERT_MSG_EQ(tenDb, differenceDbm, "Check subtraction of dBm values");
    NS_TEST_ASSERT_MSG_EQ((std::is_same_v<decltype(differenceDbm), dB_t>),
                          true,
                          "Check that (dBm - dBm) produces a variable of type dB");
}

/**
 * \ingroup units-tests
 * TestSuite for units
 */
class UnitsTestSuite : public TestSuite
{
  public:
    UnitsTestSuite();
};

UnitsTestSuite::UnitsTestSuite()
    : TestSuite("units", Type::UNIT)
{
    AddTestCase(new UnitsFrequencyTestCase, TestCase::Duration::QUICK);
    AddTestCase(new UnitsPowerTestCase, TestCase::Duration::QUICK);
}

/**
 * \ingroup units-tests
 * Static variable for test initialization
 */
static UnitsTestSuite unitsTestSuite_g;
