/*
 * Copyright (c) 2024 Jiwoong Lee
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
#include <ns3/log.h>
#include <ns3/object.h>
#include <ns3/test.h>
#include <ns3/units-aliases.h>
#include <ns3/units-angle.h>
#include <ns3/units-attributes.h>
#include <ns3/units-energy.h>
#include <ns3/units-frequency.h>

#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SiUnitsTest");

class TestCaseSiUnits : public TestCase
{
  public:
    TestCaseSiUnits()
        : TestCase("SiUnits")
    {
    }

  private:
    void Unit_degree() // NOLINT(readability-identifier-naming)
    {
        NS_TEST_ASSERT_MSG_EQ(degree{1}, degree{1.0}, "");
        NS_TEST_ASSERT_MSG_EQ(degree{-1}, degree{-1.0}, "");
        NS_TEST_ASSERT_MSG_EQ(0_degree, -(0_degree), "");
        NS_TEST_ASSERT_MSG_EQ(0_degree, 0.0_degree, "");

        NS_TEST_ASSERT_MSG_EQ((30_degree == 30.0_degree), true, "");
        NS_TEST_ASSERT_MSG_EQ((30_degree != 40.0_degree), true, "");
        NS_TEST_ASSERT_MSG_EQ((30_degree < 40_degree), true, "");
        NS_TEST_ASSERT_MSG_EQ((30_degree <= 40_degree), true, "");
        NS_TEST_ASSERT_MSG_EQ((30_degree <= 30_degree), true, "");
        NS_TEST_ASSERT_MSG_EQ((40_degree > 30_degree), true, "");
        NS_TEST_ASSERT_MSG_EQ((40_degree >= 30_degree), true, "");
        NS_TEST_ASSERT_MSG_EQ((30_degree >= 30_degree), true, "");

        NS_TEST_ASSERT_MSG_EQ((30_degree + 40_degree), 70_degree, "");
        NS_TEST_ASSERT_MSG_EQ((100_degree + 150_degree), 250_degree, "");
        NS_TEST_ASSERT_MSG_EQ((100_degree - 150_degree), -50_degree, "");
        NS_TEST_ASSERT_MSG_EQ((100_degree - 350_degree), -250_degree, "");

        NS_TEST_ASSERT_MSG_EQ((300_degree * 2.5), 750_degree, "");
        NS_TEST_ASSERT_MSG_EQ((2.5 * 300_degree), 750_degree, "");
        NS_TEST_ASSERT_MSG_EQ((300_degree / 4.0), 75_degree, "");

        NS_TEST_ASSERT_MSG_EQ(degree{100}.normalize(), 100_degree, "");
        NS_TEST_ASSERT_MSG_EQ(degree{170}.normalize(), 170_degree, "");
        NS_TEST_ASSERT_MSG_EQ(degree{190}.normalize(), -(170_degree), "");
        NS_TEST_ASSERT_MSG_EQ(degree{370}.normalize(), 10_degree, "");
        NS_TEST_ASSERT_MSG_EQ(degree{-100}.normalize(), -(100_degree), "");
        NS_TEST_ASSERT_MSG_EQ(degree{-170}.normalize(), -(170_degree), "");
        NS_TEST_ASSERT_MSG_EQ(degree{-190}.normalize(), 170_degree, "");
        NS_TEST_ASSERT_MSG_EQ(degree{-370}.normalize(), -(10_degree), "");

        NS_TEST_ASSERT_MSG_EQ((123.4_degree).str(), "123.4 degree", "");
        NS_TEST_ASSERT_MSG_EQ(degree::from_radian(radian{M_PI}), 180_degree, "");
        NS_TEST_ASSERT_MSG_EQ(degree{180}.to_radian(), radian{M_PI}, "");
        NS_TEST_ASSERT_MSG_EQ(degree{180}.in_radian(), M_PI, "");
        NS_TEST_ASSERT_MSG_EQ(degree{123.4}.in_degree(), 123.4, "");
    }

    void Unit_radian() // NOLINT(readability-identifier-naming)
    {
        NS_TEST_ASSERT_MSG_EQ(radian{1}, radian{1.0}, "");
        NS_TEST_ASSERT_MSG_EQ(radian{-1}, radian{-1.0}, "");
        NS_TEST_ASSERT_MSG_EQ(0_radian, -(0_radian), "");
        NS_TEST_ASSERT_MSG_EQ(0_radian, 0.0_radian, "");

        NS_TEST_ASSERT_MSG_EQ((30_radian == 30.0_radian), true, "");
        NS_TEST_ASSERT_MSG_EQ((30_radian != 40.0_radian), true, "");
        NS_TEST_ASSERT_MSG_EQ((30_radian < 40_radian), true, "");
        NS_TEST_ASSERT_MSG_EQ((30_radian <= 40_radian), true, "");
        NS_TEST_ASSERT_MSG_EQ((30_radian <= 30_radian), true, "");
        NS_TEST_ASSERT_MSG_EQ((40_radian > 30_radian), true, "");
        NS_TEST_ASSERT_MSG_EQ((40_radian >= 30_radian), true, "");
        NS_TEST_ASSERT_MSG_EQ((30_radian >= 30_radian), true, "");

        NS_TEST_ASSERT_MSG_EQ((30_radian + 40_radian), 70_radian, "");
        NS_TEST_ASSERT_MSG_EQ((100_radian + 150_radian), 250_radian, "");
        NS_TEST_ASSERT_MSG_EQ((100_radian - 150_radian), -50_radian, "");
        NS_TEST_ASSERT_MSG_EQ((100_radian - 350_radian), -250_radian, "");

        NS_TEST_ASSERT_MSG_EQ((300_radian * 2.5), 750_radian, "");
        NS_TEST_ASSERT_MSG_EQ((2.5 * 300_radian), 750_radian, "");
        NS_TEST_ASSERT_MSG_EQ((300_radian / 4.0), 75_radian, "");

        // Normalization is subject to the floating-point precision error. Adopt the rough
        // comparison at will.
        NS_TEST_ASSERT_MSG_EQ(radian{0.75 * M_PI}.normalize(), radian{0.75 * M_PI}, "");
        NS_TEST_ASSERT_MSG_EQ(radian{1.25 * M_PI}.normalize(), radian{-0.75 * M_PI}, "");
        NS_TEST_ASSERT_MSG_EQ(radian{2.00 * M_PI}.normalize(), radian{0.00 * M_PI}, "");
        NS_TEST_ASSERT_MSG_EQ_TOL(radian{2.25 * M_PI}.normalize().in_radian(),
                                  radian{0.25 * M_PI}.in_radian(),
                                  1e-10, // sufficient resolution
                                  "");
        NS_TEST_ASSERT_MSG_EQ(radian{-0.75 * M_PI}.normalize(), radian{-0.75 * M_PI}, "");
        NS_TEST_ASSERT_MSG_EQ(radian{-1.25 * M_PI}.normalize(), radian{0.75 * M_PI}, "");
        NS_TEST_ASSERT_MSG_EQ(radian{-2.00 * M_PI}.normalize(), radian{0.00 * M_PI}, "");
        NS_TEST_ASSERT_MSG_EQ(radian{-2.25 * M_PI}.normalize(), radian{-0.25 * M_PI}, "");

        NS_TEST_ASSERT_MSG_EQ((123.4_radian).str(), "123.4 radian", "");
        NS_TEST_ASSERT_MSG_EQ(radian::from_degree(180_degree), radian{M_PI}, "");
        NS_TEST_ASSERT_MSG_EQ(radian{M_PI}.to_degree(), 180_degree, "");
        NS_TEST_ASSERT_MSG_EQ(radian{M_PI}.in_degree(), 180, "");
        NS_TEST_ASSERT_MSG_EQ(radian{123.4}.in_radian(), 123.4, "");
    }

    void Unit_dB() // NOLINT(readability-identifier-naming)
    {
        // Notations
        NS_TEST_ASSERT_MSG_EQ(dB{0}, dB{0.}, "");
        NS_TEST_ASSERT_MSG_EQ(dB{0}, dB{0.0}, "");
        NS_TEST_ASSERT_MSG_EQ(dB{0}, dB{-0}, "");
        NS_TEST_ASSERT_MSG_EQ(dB{0}, 0_dB, "");
        NS_TEST_ASSERT_MSG_EQ(dB{0}, 0._dB, "");
        NS_TEST_ASSERT_MSG_EQ(dB{0}, 0.0_dB, "");
        NS_TEST_ASSERT_MSG_EQ(dB{0}, -0_dB, "");
        NS_TEST_ASSERT_MSG_EQ(dB{0}, -0._dB, "");
        NS_TEST_ASSERT_MSG_EQ(dB{0}, -0.0_dB, "");
        NS_TEST_ASSERT_MSG_EQ(dB{0}, dB(0.0), "");
        NS_TEST_ASSERT_MSG_EQ(dB{0}, dB{0_dB}, "");
        NS_TEST_ASSERT_MSG_EQ(dB{0}, dB(0_dB), "");

        // Equality, inequality
        NS_TEST_ASSERT_MSG_EQ(dB{10}, 10_dB, "");
        NS_TEST_ASSERT_MSG_EQ(dB{-10}, -10_dB, "");
        NS_TEST_ASSERT_MSG_EQ((dB{10} != 10_dB), false, "");
        NS_TEST_ASSERT_MSG_EQ((dB{10} == 20.0_dB), false, ""); // NOLINT
        NS_TEST_ASSERT_MSG_EQ((dB{10} != 20_dB), true, "");    // NOLINT

        // Comparison
        NS_TEST_ASSERT_MSG_LT(1_dB, 2_dB, "");
        NS_TEST_ASSERT_MSG_GT(2_dB, 1_dB, "");
        NS_TEST_ASSERT_MSG_LT_OR_EQ(1_dB, 1_dB, "");
        NS_TEST_ASSERT_MSG_LT_OR_EQ(1_dB, 2_dB, "");
        NS_TEST_ASSERT_MSG_GT_OR_EQ(2_dB, 1_dB, "");
        NS_TEST_ASSERT_MSG_GT_OR_EQ(2_dB, 2_dB, "");
        NS_TEST_ASSERT_MSG_LT(-1_dB, 2_dB, "");
        NS_TEST_ASSERT_MSG_GT(2_dB, -1_dB, "");
        NS_TEST_ASSERT_MSG_EQ((10_dB < 20_dB), true, "");
        NS_TEST_ASSERT_MSG_EQ((10_dB <= 20_dB), true, "");
        NS_TEST_ASSERT_MSG_EQ((10_dB > 20_dB), false, "");
        NS_TEST_ASSERT_MSG_EQ((10_dB >= 20_dB), false, "");

        // Arithmetic
        NS_TEST_ASSERT_MSG_EQ((1_dB + 2_dB), 3_dB, "");
        NS_TEST_ASSERT_MSG_EQ((3_dB - 1_dB), 2_dB, "");
        NS_TEST_ASSERT_MSG_EQ((3_dB - 9_dB), -6_dB, "");
        NS_TEST_ASSERT_MSG_EQ((5_dB += 10_dB), 15_dB, "");
        NS_TEST_ASSERT_MSG_EQ((5_dB -= 10_dB), -5_dB, "");
        NS_TEST_ASSERT_MSG_EQ(-8_dB, (0_dB - 8_dB), "");

        // Utilities
        NS_TEST_ASSERT_MSG_EQ(dB{123}.str(), "123.0 dB", "");    // NOLINT
        NS_TEST_ASSERT_MSG_EQ(dB{123.45}.val, 123.45, "");       // NOLINT
        NS_TEST_ASSERT_MSG_EQ(dB{123.45}.str(), "123.5 dB", ""); // NOLINT
        NS_TEST_ASSERT_MSG_EQ(dB{20}.to_linear(), 100.0, "");
    }

    void Unit_mWatt() // NOLINT
    {
        // Notations
        NS_TEST_ASSERT_MSG_EQ(mWatt{0}, 0_mWatt, "");

        // Equality, inequality
        NS_TEST_ASSERT_MSG_EQ_TOL(1_mWatt, 1e9_pWatt, 1_pWatt, ""); // NOLINT

        // Comparison
        NS_TEST_ASSERT_MSG_LT(1_mWatt, 2_mWatt, "");
        NS_TEST_ASSERT_MSG_GT(2_mWatt, 1_mWatt, "");
        NS_TEST_ASSERT_MSG_LT_OR_EQ(1_mWatt, 1_mWatt, "");
        NS_TEST_ASSERT_MSG_LT_OR_EQ(1_mWatt, 2_mWatt, "");
        NS_TEST_ASSERT_MSG_GT_OR_EQ(2_mWatt, 1_mWatt, "");
        NS_TEST_ASSERT_MSG_GT_OR_EQ(2_mWatt, 2_mWatt, "");

        // Arithmetic
        NS_TEST_ASSERT_MSG_EQ((1_mWatt + 2_mWatt), 3_mWatt, "");
        NS_TEST_ASSERT_MSG_EQ((3_mWatt - 1_mWatt), 2_mWatt, "");
        NS_TEST_ASSERT_MSG_EQ((3_mWatt - 9_mWatt), -6_mWatt, "");
        NS_TEST_ASSERT_MSG_EQ((5_mWatt += 10_mWatt), 15_mWatt, "");
        NS_TEST_ASSERT_MSG_EQ((5_mWatt -= 10_mWatt), -5_mWatt, "");
        NS_TEST_ASSERT_MSG_EQ(-8_mWatt, (0_mWatt - 8_mWatt), "");

        // Utilities
        NS_TEST_ASSERT_MSG_EQ(mWatt{123}.str(), "123.0 mWatt", "");    // NOLINT
        NS_TEST_ASSERT_MSG_EQ(mWatt{123.45}.str(), "123.5 mWatt", ""); // NOLINT
        NS_TEST_ASSERT_MSG_EQ(mWatt{100}.in_dBm(), 20.0, "");
        NS_TEST_ASSERT_MSG_EQ(mWatt{123.45}.in_Watt(), 0.12345, ""); // NOLINT
        NS_TEST_ASSERT_MSG_EQ(mWatt{123.45}.in_mWatt(), 123.45, ""); // NOLINT
    }

    void Unit_Watt() // NOLINT
    {
        // Notations
        NS_TEST_ASSERT_MSG_EQ(Watt{0}, 0_Watt, "");
        NS_TEST_ASSERT_MSG_EQ(Watt{0}, Watt{0.}, "");
        NS_TEST_ASSERT_MSG_EQ(Watt{0}, Watt{0.0}, "");
        NS_TEST_ASSERT_MSG_EQ(Watt{0}, Watt{-0}, "");
        NS_TEST_ASSERT_MSG_EQ(Watt{0}, 0_Watt, "");
        NS_TEST_ASSERT_MSG_EQ(Watt{0}, 0._Watt, "");
        NS_TEST_ASSERT_MSG_EQ(Watt{0}, 0.0_Watt, "");
        NS_TEST_ASSERT_MSG_EQ(Watt{0}, -0_Watt, "");
        NS_TEST_ASSERT_MSG_EQ(Watt{0}, -0._Watt, "");
        NS_TEST_ASSERT_MSG_EQ(Watt{0}, -0.0_Watt, "");

        // Equality, inequality
        NS_TEST_ASSERT_MSG_EQ(Watt{10}, 10_Watt, "");
        NS_TEST_ASSERT_MSG_EQ(Watt{-10}, -10_Watt, "");
        NS_TEST_ASSERT_MSG_EQ((Watt{10} != 10_Watt), false, "");
        NS_TEST_ASSERT_MSG_EQ((Watt{10} == 20.0_Watt), false, ""); // NOLINT
        NS_TEST_ASSERT_MSG_EQ((Watt{10} == 10.0_Watt), true, "");  // NOLINT
        NS_TEST_ASSERT_MSG_EQ((Watt{10} != 20_Watt), true, "");

        // Comparison
        NS_TEST_ASSERT_MSG_LT(1_Watt, 2_Watt, "");
        NS_TEST_ASSERT_MSG_GT(2_Watt, 1_Watt, "");
        NS_TEST_ASSERT_MSG_LT_OR_EQ(1_Watt, 1_Watt, "");
        NS_TEST_ASSERT_MSG_LT_OR_EQ(1_Watt, 2_Watt, "");
        NS_TEST_ASSERT_MSG_GT_OR_EQ(2_Watt, 1_Watt, "");
        NS_TEST_ASSERT_MSG_GT_OR_EQ(2_Watt, 2_Watt, "");
        NS_TEST_ASSERT_MSG_LT(-2_Watt, 1_Watt, "");
        NS_TEST_ASSERT_MSG_LT(-2_Watt, -1_Watt, "");
        NS_TEST_ASSERT_MSG_GT(-1_Watt, -2_Watt, "");
        NS_TEST_ASSERT_MSG_GT(1_Watt, -2_Watt, "");
        NS_TEST_ASSERT_MSG_EQ((10_Watt < 20_Watt), true, "");
        NS_TEST_ASSERT_MSG_EQ((10_Watt <= 20_Watt), true, "");
        NS_TEST_ASSERT_MSG_EQ((10_Watt > 20_Watt), false, "");
        NS_TEST_ASSERT_MSG_EQ((10_Watt >= 20_Watt), false, "");

        // Arithmetic
        NS_TEST_ASSERT_MSG_EQ((1_Watt + 2_Watt), 3_Watt, "");
        NS_TEST_ASSERT_MSG_EQ((3_Watt - 1_Watt), 2_Watt, "");
        NS_TEST_ASSERT_MSG_EQ((3_Watt - 9_Watt), -6_Watt, "");
        NS_TEST_ASSERT_MSG_EQ((5_Watt += 10_Watt), 15_Watt, "");
        NS_TEST_ASSERT_MSG_EQ((5_Watt -= 10_Watt), -5_Watt, "");
        NS_TEST_ASSERT_MSG_EQ(-8_Watt, (0_Watt - 8_Watt), "");

        // Utilities
        NS_TEST_ASSERT_MSG_EQ(Watt{123}.str(), "123.0 Watt", "");    // NOLINT
        NS_TEST_ASSERT_MSG_EQ(Watt{123.45}.str(), "123.5 Watt", ""); // NOLINT
        NS_TEST_ASSERT_MSG_EQ(Watt{100}.in_dBm(), 50.0, "");
        NS_TEST_ASSERT_MSG_EQ(Watt{1.2345}.in_mWatt(), 1234.5, ""); // NOLINT
        NS_TEST_ASSERT_MSG_EQ(Watt{123.45}.in_Watt(), 123.45, "");  // NOLINT
    }

    void Unit_dBm() // NOLINT
    {
        // Notations
        NS_TEST_ASSERT_MSG_EQ(dBm{0}, dBm{0.}, "");
        NS_TEST_ASSERT_MSG_EQ(dBm{0}, dBm{0.0}, "");
        NS_TEST_ASSERT_MSG_EQ(dBm{0}, dBm{-0}, "");
        NS_TEST_ASSERT_MSG_EQ(dBm{0}, 0_dBm, "");
        NS_TEST_ASSERT_MSG_EQ(dBm{0}, 0._dBm, "");
        NS_TEST_ASSERT_MSG_EQ(dBm{0}, 0.0_dBm, "");
        NS_TEST_ASSERT_MSG_EQ(dBm{0}, -0_dBm, "");
        NS_TEST_ASSERT_MSG_EQ(dBm{0}, -0._dBm, "");
        NS_TEST_ASSERT_MSG_EQ(dBm{0}, -0.0_dBm, "");

        // Equality, inequality

        NS_TEST_ASSERT_MSG_EQ(dBm{10}, 10_dBm, "");
        NS_TEST_ASSERT_MSG_EQ(dBm{-10}, -10_dBm, "");
        NS_TEST_ASSERT_MSG_EQ((dBm{10} != 10_dBm), false, "");
        NS_TEST_ASSERT_MSG_EQ((dBm{10} == 20.0_dBm), false, ""); // NOLINT
        NS_TEST_ASSERT_MSG_EQ((dBm{10} != 20_dBm), true, "");

        // Comparison
        NS_TEST_ASSERT_MSG_LT(1_dBm, 2_dBm, "");
        NS_TEST_ASSERT_MSG_GT(2_dBm, 1_dBm, "");
        NS_TEST_ASSERT_MSG_LT_OR_EQ(1_dBm, 1_dBm, "");
        NS_TEST_ASSERT_MSG_LT_OR_EQ(1_dBm, 2_dBm, "");
        NS_TEST_ASSERT_MSG_GT_OR_EQ(2_dBm, 1_dBm, "");
        NS_TEST_ASSERT_MSG_GT_OR_EQ(2_dBm, 2_dBm, "");
        NS_TEST_ASSERT_MSG_LT(-1_dBm, 2_dBm, "");
        NS_TEST_ASSERT_MSG_GT(2_dBm, -1_dBm, "");
        NS_TEST_ASSERT_MSG_EQ((10_dBm < 20_dBm), true, "");
        NS_TEST_ASSERT_MSG_EQ((10_dBm <= 20_dBm), true, "");
        NS_TEST_ASSERT_MSG_EQ((10_dBm > 20_dBm), false, "");
        NS_TEST_ASSERT_MSG_EQ((10_dBm >= 20_dBm), false, "");

        // Arithmetic
        NS_TEST_ASSERT_MSG_EQ_TOL((20_dBm + 20_dBm).val, (23.0102999566_dBm).val, 1e-4, "");
        NS_TEST_ASSERT_MSG_EQ_TOL((20_dBm += 20_dBm).val, (23.0102999566_dBm).val, 1e-4, "");
        NS_TEST_ASSERT_MSG_EQ_TOL((20_dBm - 10_dBm).val, (19.5424250944_dBm).val, 1e-4, "");
        NS_TEST_ASSERT_MSG_EQ_TOL((20_dBm -= 10_dBm).val, (19.5424250944_dBm).val, 1e-4, "");
        NS_TEST_ASSERT_MSG_EQ_TOL((20_dBm - 19_dBm).val, (13.1317467562_dBm).val, 1e-4, "");

        // Utilities
        NS_TEST_ASSERT_MSG_EQ(dBm{123}.str(), "123.0 dBm", "");    // NOLINT
        NS_TEST_ASSERT_MSG_EQ(dBm{123.45}.str(), "123.5 dBm", ""); // NOLINT
        NS_TEST_ASSERT_MSG_EQ(dBm{20}.in_mWatt(), 100.0, "");
        // Need tolerance due to math precision error on M1 Ultra with --ffast-math
        NS_TEST_ASSERT_MSG_EQ_TOL(dBm{20}.in_Watt(), 0.1, 1e-10, "");
        NS_TEST_ASSERT_MSG_EQ(dBm{123.45}.in_dBm(), 123.45, ""); // NOLINT
    }

    void Unit_dBm_and_dB() // NOLINT
    {
        NS_TEST_ASSERT_MSG_EQ((10_dBm + 20_dB), 30_dBm, "");
        NS_TEST_ASSERT_MSG_EQ((10_dBm - 20_dB), -10_dBm, "");
        NS_TEST_ASSERT_MSG_EQ((10_dB + 20_dBm), 30_dBm, "");  // Commutativity
        NS_TEST_ASSERT_MSG_EQ((10_dB - 20_dBm), -10_dBm, ""); // Commutativity
    }

    void Unit_mWatt_and_Watt() // NOLINT
    {
        // Equality, inequality
        NS_TEST_ASSERT_MSG_EQ(mWatt{0}, 0_Watt, "");
        NS_TEST_ASSERT_MSG_EQ(Watt{10}, 10000_mWatt, "");
        NS_TEST_ASSERT_MSG_EQ((Watt{1} != 1000_mWatt), false, "");
        NS_TEST_ASSERT_MSG_EQ((Watt{2} == 1000_mWatt), false, ""); // NOLINT
        NS_TEST_ASSERT_MSG_EQ((mWatt{1} == 0.001_Watt), true, ""); // NOLINT
        NS_TEST_ASSERT_MSG_EQ((mWatt{10} != 20_Watt), true, "");

        // Comparison
        NS_TEST_ASSERT_MSG_LT(1_mWatt, 2_Watt, "");
        NS_TEST_ASSERT_MSG_GT(2_mWatt, 0.001_Watt, "");
        NS_TEST_ASSERT_MSG_LT_OR_EQ(1000_mWatt, 1_Watt, "");
        NS_TEST_ASSERT_MSG_GT_OR_EQ(2_mWatt, 0.001_Watt, "");
        NS_TEST_ASSERT_MSG_EQ((10_mWatt < 20_Watt), true, "");
        NS_TEST_ASSERT_MSG_EQ((2000_mWatt <= 2_Watt), true, "");
        NS_TEST_ASSERT_MSG_EQ((10_mWatt > 10_Watt), false, "");
        NS_TEST_ASSERT_MSG_EQ((10_mWatt >= 20_Watt), false, "");
        NS_TEST_ASSERT_MSG_LT(1_Watt, 2000_mWatt, "");
        NS_TEST_ASSERT_MSG_GT(2_Watt, 0.001_mWatt, "");
        NS_TEST_ASSERT_MSG_LT_OR_EQ(0.001_Watt, 1_mWatt, "");
        NS_TEST_ASSERT_MSG_GT_OR_EQ(2_Watt, 2_mWatt, "");
        NS_TEST_ASSERT_MSG_EQ((0.1_Watt < 200_mWatt), true, "");
        NS_TEST_ASSERT_MSG_EQ((2_Watt <= 2000_mWatt), true, "");
        NS_TEST_ASSERT_MSG_EQ((0.1_Watt > 100_mWatt), false, "");
        NS_TEST_ASSERT_MSG_EQ((1_Watt >= 2000_mWatt), false, "");

        // Arithmetic
        NS_TEST_ASSERT_MSG_EQ((1_mWatt + 2_Watt), 2001_mWatt, "");
        NS_TEST_ASSERT_MSG_EQ((3_mWatt - 0.001_Watt), 2_mWatt, "");
        NS_TEST_ASSERT_MSG_EQ((5_mWatt += 0.01_Watt), 15_mWatt, "");
        NS_TEST_ASSERT_MSG_EQ((5_mWatt -= 0.002_Watt), 3_mWatt, "");
        NS_TEST_ASSERT_MSG_EQ(8_mWatt, (8_mWatt - 0_Watt), "");
        NS_TEST_ASSERT_MSG_EQ((1_Watt + 2_mWatt), 1002_mWatt, "");
        NS_TEST_ASSERT_MSG_EQ((0.03_Watt - 1_mWatt), 29_mWatt, "");
        NS_TEST_ASSERT_MSG_EQ((1_Watt += 0.01_Watt), 1.01_Watt, "");
        NS_TEST_ASSERT_MSG_EQ((4_Watt -= 0.2_Watt), 3.8_Watt, "");
        NS_TEST_ASSERT_MSG_EQ(8_Watt, (8_Watt - 0_mWatt), "");
    }

    void Conversion() // NOLINT
    {
        // dBm-mWatt
        NS_TEST_ASSERT_MSG_EQ(20_dBm, (100_mWatt).to_dBm(), "");
        NS_TEST_ASSERT_MSG_EQ(20_dBm, dBm::from_mWatt(100_mWatt), "");
        NS_TEST_ASSERT_MSG_EQ((20_dBm).to_mWatt(), 100_mWatt, "");
        NS_TEST_ASSERT_MSG_EQ(mWatt::from_dBm(20_dBm), 100_mWatt, "");

        // dBm-Watt
        NS_TEST_ASSERT_MSG_EQ(10_dBm, (0.01_Watt).to_dBm(), "");
        NS_TEST_ASSERT_MSG_EQ(10_dBm, dBm::from_Watt(0.01_Watt), "");
        NS_TEST_ASSERT_MSG_EQ((10_dBm).to_Watt(), 0.01_Watt, "");
        NS_TEST_ASSERT_MSG_EQ(Watt::from_dBm(10_dBm), 0.01_Watt, "");

        // Watt-mWatt
        NS_TEST_ASSERT_MSG_EQ(0.1_Watt, (100_mWatt).to_Watt(), "");
        NS_TEST_ASSERT_MSG_EQ(0.1_Watt, Watt::from_mWatt(100_mWatt), "");
        NS_TEST_ASSERT_MSG_EQ((0.1_Watt).to_mWatt(), 100_mWatt, "");
        NS_TEST_ASSERT_MSG_EQ(mWatt::from_Watt(0.1_Watt), 100_mWatt, "");
    }

    void Unit_Hz() // NOLINT
    {
        NS_TEST_ASSERT_MSG_EQ(Hz{123}, 123_Hz, "");
        NS_TEST_ASSERT_MSG_EQ(Hz{123.45}, 123.45_Hz, "");
        NS_TEST_ASSERT_MSG_EQ((-123_Hz), Hz{-123}, "");
        NS_TEST_ASSERT_MSG_EQ(Hz{123000}, 123_kHz, "");
        NS_TEST_ASSERT_MSG_EQ(Hz{123000000}, 123_MHz, "");
        NS_TEST_ASSERT_MSG_EQ(Hz{123000000000}, 123_GHz, "");
        NS_TEST_ASSERT_MSG_EQ(Hz{123000000000000}, 123_THz, "");
        NS_TEST_ASSERT_MSG_EQ(Hz{123000000000000}, 123000000000000_Hz, "");

        NS_TEST_ASSERT_MSG_EQ(10_Hz + 20_Hz, 30_Hz, "");
        NS_TEST_ASSERT_MSG_EQ(10_MHz - 20_MHz, -10_MHz, "");
        NS_TEST_ASSERT_MSG_EQ((10_MHz - 20_MHz != 40_MHz), true, "");
        NS_TEST_ASSERT_MSG_EQ((10_MHz - 20_MHz == 40_MHz), false, "");
        NS_TEST_ASSERT_MSG_EQ((10_kHz < 20_kHz), true, "");
        NS_TEST_ASSERT_MSG_EQ((10_kHz <= 20_kHz), true, "");
        NS_TEST_ASSERT_MSG_EQ((10_kHz <= 10_kHz), true, "");

        NS_TEST_ASSERT_MSG_EQ((10_kHz > 20_kHz), false, "");
        NS_TEST_ASSERT_MSG_EQ((10_kHz >= 20_kHz), false, "");
        NS_TEST_ASSERT_MSG_EQ((10_kHz >= 10_kHz), true, "");

        NS_TEST_ASSERT_MSG_EQ((10_Hz += 100_Hz), 110_Hz, "");
        NS_TEST_ASSERT_MSG_EQ((10_Hz -= 100_Hz), -90_Hz, "");
        NS_TEST_ASSERT_MSG_EQ((1_kHz / 4), 250_Hz, "");
        NS_TEST_ASSERT_MSG_EQ((1_kHz / 4_Hz), 250.0, "");
        NS_TEST_ASSERT_MSG_EQ((1_kHz * 4), 4_kHz, "");
        NS_TEST_ASSERT_MSG_EQ((4 * 1_kHz), 4_kHz, "");
        NS_TEST_ASSERT_MSG_EQ((1_Hz * MilliSeconds(1)), 0.001, "");
        NS_TEST_ASSERT_MSG_EQ((1_kHz * MilliSeconds(1)), 1.0, "");
        NS_TEST_ASSERT_MSG_EQ((1_MHz * MilliSeconds(1)), 1000.0, "");
        NS_TEST_ASSERT_MSG_EQ((MilliSeconds(1) * 1_MHz), 1000.0, "");
        NS_TEST_ASSERT_MSG_EQ((MilliSeconds(1) * 1_kHz), 1.0, "");
        NS_TEST_ASSERT_MSG_EQ((MilliSeconds(1) * 1_Hz), 0.001, "");

        NS_TEST_ASSERT_MSG_EQ((123_Hz).str(), "123 Hz", "");
        NS_TEST_ASSERT_MSG_EQ((123_kHz).str(), "123 kHz", "");
        NS_TEST_ASSERT_MSG_EQ((123_MHz).str(), "123 MHz", "");
        NS_TEST_ASSERT_MSG_EQ((123_GHz).str(), "123 GHz", "");
        NS_TEST_ASSERT_MSG_EQ((123_THz).str(), "123 THz", "");
        NS_TEST_ASSERT_MSG_EQ((123000_THz).str(), "123000 THz", "");

        NS_TEST_ASSERT_MSG_EQ((123_GHz).in_Hz(), 123000000000, "");
        NS_TEST_ASSERT_MSG_EQ((123_GHz).in_kHz(), 123000000.0, "");
        NS_TEST_ASSERT_MSG_EQ((123_GHz).in_MHz(), 123000.0, "");
        NS_TEST_ASSERT_MSG_EQ((123.45e6_kHz).in_Hz(), 123450000000, "");
        NS_TEST_ASSERT_MSG_EQ((123.45e6_kHz).in_kHz(), 123450000, "");
        NS_TEST_ASSERT_MSG_EQ((123.45e6_kHz).in_MHz(), 123450, "");
        NS_TEST_ASSERT_MSG_EQ((123.456789e6_kHz).in_MHz(), 123456.789, "");

        NS_TEST_ASSERT_MSG_EQ(kHz(123.4), 123.4_kHz, "");
        NS_TEST_ASSERT_MSG_EQ(MHz(123.4), 123.4_MHz, "");
        NS_TEST_ASSERT_MSG_EQ(GHz(123.4), 123.4_GHz, "");
        NS_TEST_ASSERT_MSG_EQ(THz(123.4), 123.4_THz, "");
        NS_TEST_ASSERT_MSG_EQ(kHz(123), 123_kHz, "");
        NS_TEST_ASSERT_MSG_EQ(MHz(123), 123_MHz, "");
        NS_TEST_ASSERT_MSG_EQ(GHz(123), 123_GHz, "");
        NS_TEST_ASSERT_MSG_EQ(THz(123), 123_THz, "");

        NS_TEST_ASSERT_MSG_EQ(kHz(123.4), 123400_Hz, "");
        NS_TEST_ASSERT_MSG_EQ(MHz(123.4), 123400000_Hz, "");
        NS_TEST_ASSERT_MSG_EQ(GHz(123.4), 123400000000_Hz, "");
        NS_TEST_ASSERT_MSG_EQ(THz(123.4), 123400000000000_Hz, "");
    }

    void Unit_mWatt_and_double() // NOLINT
    {
        // Arithmetic
        NS_TEST_ASSERT_MSG_EQ((1_mWatt * 2.0), 2_mWatt, "");
        NS_TEST_ASSERT_MSG_EQ((1_mWatt / 2.0), 0.5_mWatt, "");
    }

    void Unit_double_and_mWatt() // NOLINT
    {
        // Arithmetic
        NS_TEST_ASSERT_MSG_EQ((2.0 * 1_mWatt), 2_mWatt, "");
    }

    void Unit_dBm_per_Hz() // NOLINT
    {
        NS_TEST_ASSERT_MSG_EQ(dBm_per_Hz{-43.21}, -43.21_dBm_per_Hz, ""); // NOLINT

        // Utilities
        NS_TEST_ASSERT_MSG_EQ(dBm_per_Hz{123}.val, 123.0, "");               // NOLINT
        NS_TEST_ASSERT_MSG_EQ(dBm_per_Hz{123}.str(), "123.0 dBm/Hz", "");    // NOLINT
        NS_TEST_ASSERT_MSG_EQ(dBm_per_Hz{123.45}.val, 123.45, "");           // NOLINT
        NS_TEST_ASSERT_MSG_EQ(dBm_per_Hz{123.45}.str(), "123.5 dBm/Hz", ""); // NOLINT

        NS_TEST_ASSERT_MSG_EQ(dBm_per_Hz::AveragePsd(-20_dBm, 1_MHz),
                              dBm_per_Hz{-80},
                              "");                                                  // NOLINT
        NS_TEST_ASSERT_MSG_EQ(dBm_per_Hz{-80.0}.OverBandwidth(1_MHz), -20_dBm, ""); // NOLINT
        NS_TEST_ASSERT_MSG_EQ(dBm_per_Hz{123.45}.in_dBm(), 123.45, "");             // NOLINT
    }

    void Vectors()
    {
        { // dB Empty vector
            std::vector<double> tvs;
            auto got1 = dB::from_doubles(tvs);
            auto got2 = dB::to_doubles(got1);
            auto got3 = dB::from_doubles(got2);
            NS_TEST_ASSERT_MSG_EQ((tvs == got2), true, "vector of double's do not match");
            NS_TEST_ASSERT_MSG_EQ((got1 == got3), true, "vector of dB's do not match");
            for (auto idx = 0; idx < tvs.size(); ++idx)
            {
                NS_TEST_ASSERT_MSG_EQ(got1[idx].val, tvs[idx], "");
            }
        }

        std::vector<double> tvs = {0.1, -0.2, 1.3, -4.5, 5.6e7, -8e-9};
        { // dB
            auto got1 = dB::from_doubles(tvs);
            auto got2 = dB::to_doubles(got1);
            auto got3 = dB::from_doubles(got2);
            NS_TEST_ASSERT_MSG_EQ((tvs == got2), true, "vector of double's do not match");
            NS_TEST_ASSERT_MSG_EQ((got1 == got3), true, "vector of dB's do not match");
            for (auto idx = 0; idx < tvs.size(); ++idx)
            {
                NS_TEST_ASSERT_MSG_EQ(got1[idx].val, tvs[idx], "");
            }
        }

        { // dBm
            auto got1 = dBm::from_doubles(tvs);
            auto got2 = dBm::to_doubles(got1);
            auto got3 = dBm::from_doubles(got2);
            NS_TEST_ASSERT_MSG_EQ((tvs == got2), true, "vector of double's do not match");
            NS_TEST_ASSERT_MSG_EQ((got1 == got3), true, "vector of dBm's do not match");
            for (auto idx = 0; idx < tvs.size(); ++idx)
            {
                NS_TEST_ASSERT_MSG_EQ(got1[idx].val, tvs[idx], "");
            }
        }

        { // mWatt
            auto got1 = mWatt::from_doubles(tvs);
            auto got2 = mWatt::to_doubles(got1);
            auto got3 = mWatt::from_doubles(got2);
            NS_TEST_ASSERT_MSG_EQ((tvs == got2), true, "vector of double's do not match");
            NS_TEST_ASSERT_MSG_EQ((got1 == got3), true, "vector of mWatt's do not match");
            for (auto idx = 0; idx < tvs.size(); ++idx)
            {
                NS_TEST_ASSERT_MSG_EQ(got1[idx].val, tvs[idx], "");
            }
        }

        { // Watt
            auto got1 = Watt::from_doubles(tvs);
            auto got2 = Watt::to_doubles(got1);
            auto got3 = Watt::from_doubles(got2);
            NS_TEST_ASSERT_MSG_EQ((tvs == got2), true, "vector of double's do not match");
            NS_TEST_ASSERT_MSG_EQ((got1 == got3), true, "vector of Watt's do not match");
            for (auto idx = 0; idx < tvs.size(); ++idx)
            {
                NS_TEST_ASSERT_MSG_EQ(got1[idx].val, tvs[idx], "");
            }
        }

        { // dBm_per_Hz
            auto got1 = dBm_per_Hz::from_doubles(tvs);
            auto got2 = dBm_per_Hz::to_doubles(got1);
            auto got3 = dBm_per_Hz::from_doubles(got2);
            NS_TEST_ASSERT_MSG_EQ((tvs == got2), true, "vector of double's do not match");
            NS_TEST_ASSERT_MSG_EQ((got1 == got3), true, "vector of dBm_per_Hz's do not match");
            for (auto idx = 0; idx < tvs.size(); ++idx)
            {
                NS_TEST_ASSERT_MSG_EQ(got1[idx].val, tvs[idx], "");
            }
        }

        { // Hz
            std::vector<double> tvs = {1, -2, 3000, -4000000};
            auto got1 = Hz::from_doubles(tvs);
            auto got2 = Hz::to_doubles(got1);
            auto got3 = Hz::from_doubles(got2);
            NS_TEST_ASSERT_MSG_EQ((tvs == got2), true, "vector of double's do not match");
            NS_TEST_ASSERT_MSG_EQ((got1 == got3), true, "vector of Hz's do not match");
            for (auto idx = 0; idx < tvs.size(); ++idx)
            {
                NS_TEST_ASSERT_MSG_EQ(got1[idx].val, tvs[idx], "");
            }
        }
    }

    virtual void DoRun()
    {
        Unit_degree();
        Unit_radian();

        Unit_dB();
        Unit_dBm();
        Unit_dBm_and_dB();
        Unit_mWatt();
        Unit_Watt();
        Unit_mWatt_and_Watt();
        Unit_mWatt_and_double();
        Unit_double_and_mWatt();
        Conversion();
        Unit_Hz();
        Unit_dBm_and_dB();
        Vectors();
    }
};

class AttributeMock : public Object
{
  public:
    static TypeId GetTypeId()
    {
        static TypeId tid = //
            TypeId("ns3:AttributeMock")
                .SetParent<Object>()
                .SetGroupName("AttributeMock")
                .AddConstructor<AttributeMock>()
                .AddAttribute("dB",
                              "help message for dB",
                              dBValue(0_dB),
                              MakedBAccessor(&AttributeMock::m_dB),
                              MakedBChecker())
                .AddAttribute("dBm",
                              "help message for dBm",
                              dBmValue(20_dBm),
                              MakedBmAccessor(&AttributeMock::m_dBm),
                              MakedBmChecker())
                .AddAttribute("mWatt",
                              "help message for mWatt",
                              mWattValue(100_mWatt),
                              MakemWattAccessor(&AttributeMock::m_mWatt),
                              MakemWattChecker())
                .AddAttribute("dBm_per_Hz",
                              "help message for dBm_per_Hz",
                              dBm_per_HzValue(0.0004),
                              MakedBm_per_HzAccessor(&AttributeMock::m_dBm_per_Hz),
                              MakedBm_per_HzChecker())
                .AddAttribute("Hz",
                              "help message for Hz",
                              HzValue(415000_Hz),
                              MakeHzAccessor(&AttributeMock::m_Hz),
                              MakeHzChecker())
                .AddAttribute("degree",
                              "help message for degree",
                              degreeValue(720_degree),
                              MakedegreeAccessor(&AttributeMock::m_degree),
                              MakedegreeChecker())
                .AddAttribute("radian",
                              "help message for radian",
                              radianValue(20_radian),
                              MakeradianAccessor(&AttributeMock::m_radian),
                              MakeradianChecker());
        return tid;
    }

    dB m_dB{};
    dBm m_dBm{};
    mWatt m_mWatt{};
    dBm_per_Hz m_dBm_per_Hz{};
    Hz m_Hz{};
    degree m_degree{};
    radian m_radian{};
};

class TestCaseSiUnitsAttributes : public TestCase

{
  public:
    TestCaseSiUnitsAttributes()
        : TestCase("SiUnitsAttributes")
    {
    }

  private:
    virtual void DoRun()
    {
        auto mock = CreateObject<AttributeMock>();

        {
            auto want = 9_dB;
            mock->SetAttribute("dB", dBValue(want));
            NS_TEST_ASSERT_MSG_EQ(mock->m_dB, want, "");
        }
        {
            auto want = 20_dBm;
            mock->SetAttribute("dBm", dBmValue(want));
            NS_TEST_ASSERT_MSG_EQ(mock->m_dBm, want, "");
        }
        {
            auto want = 100_mWatt;
            mock->SetAttribute("mWatt", mWattValue(want));
            NS_TEST_ASSERT_MSG_EQ(mock->m_mWatt, want, "");
        }
        {
            auto want = 0.0001_dBm_per_Hz;
            mock->SetAttribute("dBm_per_Hz", dBm_per_HzValue(want));
            NS_TEST_ASSERT_MSG_EQ(mock->m_dBm_per_Hz, want, "");
        }
        {
            auto want = 365_Hz;
            mock->SetAttribute("Hz", HzValue(want));
            NS_TEST_ASSERT_MSG_EQ(mock->m_Hz, want, "");
        }
        {
            auto want = 720_degree;
            mock->SetAttribute("degree", degreeValue(want));
            NS_TEST_ASSERT_MSG_EQ(mock->m_degree, want, "");
        }
        {
            auto want = 2.4_radian;
            mock->SetAttribute("radian", radianValue(want));
            NS_TEST_ASSERT_MSG_EQ(mock->m_radian, want, "");
        }
    }
};

class SiUnitsTestSuite : public TestSuite
{
  public:
    SiUnitsTestSuite()
        : TestSuite("si-units-test", UNIT)
    {
        AddTestCase(new TestCaseSiUnits, TestCase::QUICK);
        AddTestCase(new TestCaseSiUnitsAttributes, TestCase::QUICK);
    }
};

static SiUnitsTestSuite g_siUnitsTestSuite;
