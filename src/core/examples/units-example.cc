/*
 * Copyright (c) 2024 University of Washington
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "ns3/assert.h"
#include "ns3/command-line.h"
#include "ns3/decibel.h"
#include "ns3/units.h"

#include <iostream>

/**
 * \defgroup unit-examples Demonstrate use and capabilities of ns-3 units
 * \ingroup core-examples
 */

/**
 * \file
 * \ingroup unit-examples
 * Demonstrate use and capabilities of ns-3 units
 */

using namespace ns3;
using namespace units;
using namespace units::literals;
using namespace units::length;
using namespace units::power;
using namespace units::dimensionless;

// Program formatted to produce Markdown text
int
main(int argc, char** argv)
{
    dB_t cmdLineDecibel{3};
    CommandLine cmd(__FILE__);
    cmd.Usage("This program is used to demonstrate strongly-typed units in ns-3.");
    cmd.AddValue("cmdLineDecibel", "Decibel variable for command line", cmdLineDecibel);
    cmd.Parse(argc, argv);

    std::cout << std::endl;
    std::cout << "Certain quantities in ns-3 could benefit from strongly-typed units:" << std::endl
              << std::endl;
    std::cout << "* Time (already covered)" << std::endl;
    std::cout << "* Frequency" << std::endl;
    std::cout << "* Power" << std::endl;
    std::cout << "* ..." << std::endl << std::endl;
    std::cout << "In this proposal, most units are provided by an imported header-only library"
              << std::endl;
    std::cout << "See src/core/model/units.h (or search for \"nholthaus/units\" on GitHub)."
              << std::endl;
    std::cout << "This example provides an overview of what porting to this might be like."
              << std::endl;
    std::cout << "To run this example yourself, type:" << std::endl << std::endl;
    std::cout << "    ./ns3 run units-example" << std::endl << std::endl;

    std::cout << "Units are defined in the \"units\" namespace.";
    std::cout << "They can be brought into the" << std::endl;
    std::cout << "current scope via one or more \"using\" directives, or can be referred to"
              << std::endl;
    std::cout << "by a fully qualified name:" << std::endl << std::endl;
    std::cout << "    using namespace units;" << std::endl;
    std::cout << "    using namespace units::literals;" << std::endl;
    std::cout << "    using namespace units::length;" << std::endl;
    std::cout << "    using namespace units::power;" << std::endl;
    std::cout << std::endl;
    std::cout << "Unit types begin with a lowercase letter and end with an underscore--"
              << std::endl;
    std::cout << "different from usual ns-3 naming conventions.  Examples:" << std::endl
              << std::endl;
    std::cout << "    meter_t distance{8};" << std::endl;
    std::cout << "    units::length::meter_t distance2{10.5};" << std::endl;
    std::cout << "    watt_t transmitPower{1};" << std::endl;
    std::cout << "    units::power::watt_t transmitPower2{2.5};" << std::endl;
    std::cout << std::endl;

    meter_t distance{8};
    [[maybe_unused]] units::length::meter_t distance2{10.5};
    [[maybe_unused]] watt_t transmitPower{1};
    [[maybe_unused]] units::power::watt_t transmitPower2{2.5};

    std::cout << "If you import the \"units::literals\" namespace, you can use literal syntax:"
              << std::endl
              << std::endl;
    std::cout << "    auto distance3 = 8_m;" << std::endl;
    std::cout << "    NS_ASSERT_MSG(distance3 == distance, \"Distance values are not equal\");"
              << std::endl;
    std::cout << std::endl;

    auto distance3 = 8_m;
    NS_ASSERT_MSG(distance3 == distance, "Distance values not equal");

    std::cout << "The underlying type of all of these units is the C++ double." << std::endl;
    std::cout << "You can extract this value using the to() method:" << std::endl << std::endl;
    std::cout << "    auto converted = distance3.to<double>();" << std::endl;
    std::cout << "    std::cout << Converted distance is \" << converted << \" m\" << std::endl;"
              << std::endl
              << std::endl;

    auto converted = distance3.to<double>();
    std::cout << "Converted distance is " << converted << " m" << std::endl << std::endl;

    std::cout << "One of the key features is that expressions with incompatible types" << std::endl;
    std::cout << "will not compile.  For example:" << std::endl << std::endl;
    std::cout << "    // will fail with: error: Units are not compatible. " << std::endl;
    std::cout << "    auto sum = distance2 + transmitPower2;" << std::endl << std::endl;
    std::cout << "and:" << std::endl << std::endl;
    std::cout << "    double doubleValue{4};" << std::endl;
    std::cout
        << "    // will fail with: error: Cannot add units with different linear/non-linear scales."
        << std::endl;
    std::cout << "    auto sumDouble = distance3 + doubleValue;" << std::endl << std::endl;

#ifdef WONT_COMPILE
    // will fail with: error: Units are not compatible.
    auto sum = distance2 + transmitPower2;
    double doubleValue{4};
    // will fail with: error: Cannot add units with different linear/non-linear scales.
    auto sumDouble = distance3 + doubleValue;
#endif

    std::cout << "Another feature is that arithmetic operations on different units with the same"
              << std::endl;
    std::cout << "underlying conceptual type (e.g., length) will work as expected," << std::endl;
    std::cout << "even if the units differ.  Below, we add one variable initialized" << std::endl;
    std::cout << "to 8 m with one initialized to 8 km:" << std::endl << std::endl;

    std::cout << "    auto distance4{8_km};" << std::endl;
    std::cout << "    std::cout << \"Sum of distances is \" << distance3 + distance4 << std::endl"
              << std::endl
              << std::endl;

    auto distance4{8_km};
    std::cout << "Sum of distances is " << distance3 + distance4 << std::endl << std::endl;

    std::cout << "In ns-3, handling of power values with linear and log scale are important."
              << std::endl;
    std::cout << "The units library supports power quantities like watts (_w) and" << std::endl;
    std::cout << "milliwatts (_milliwatt) as well as the logarithmic variants (_dBw, _dBm)."
              << std::endl
              << std::endl;

    std::cout << "    milliwatt_t txPwr{100}; // 100 mW" << std::endl;
    std::cout << "    std::cout << \"  txPwr = \" << txPwr << std::endl; // should print 100 mW"
              << std::endl;
    std::cout << "    dBm_t txPwrDbm(txPwr);  // 20 dBm" << std::endl;
    std::cout
        << "    std::cout << \"  txPwrDbm = \" << txPwrDbm << std::endl; // should print 20 dBm"
        << std::endl;
    std::cout << "    dBW_t txPwrDbW(txPwrDbm);  // -10 dBW" << std::endl;
    std::cout
        << "    std::cout << \"  txPwrDbW = \" << txPwrDbW << std::endl; // should print -10 dBW"
        << std::endl
        << std::endl;

    std::cout << "Below are the printouts from the running code:" << std::endl << std::endl;
    milliwatt_t txPwr{100};                              // 100 mW
    std::cout << "txPwr = " << txPwr << std::endl;       // should print 100 mW
    dBm_t txPwrDbm(txPwr);                               // 20 dBm
    std::cout << "txPwrDbm = " << txPwrDbm << std::endl; // should print 20 dBm
    dBW_t txPwrDbW(txPwrDbm);                            // -10 dBW
    std::cout << "txPwrDbW = " << txPwrDbW << std::endl; // should print -10 dBW
    std::cout << std::endl;

    std::cout << "We can add linear power values:" << std::endl << std::endl;
    std::cout << "txPwr + txPwr = " << txPwr + txPwr << std::endl; // OK; should print 200 mW
    std::cout << std::endl;
    std::cout << "We can scale linear power values:" << std::endl << std::endl;
    std::cout << "txPwr * 2 = " << 2 * txPwr << std::endl; // OK; should print 200 mW
    std::cout << std::endl;
    std::cout << "We can add logarithmic power values, but the resulting unit is strange:"
              << std::endl
              << std::endl;
    std::cout << "txPwrDbm + txPwrDbm = " << txPwrDbm + txPwrDbm << std::endl
              << std::endl; // OK; should print 40 dBm
    std::cout << "Note:  this is a bug that we should fix if we adopt this." << std::endl
              << std::endl;

    std::cout << "Adding linear and non-linear values will cause a compile-time error:" << std::endl
              << std::endl;
    std::cout << "    dBW_t loss{-20}; // equivalent to 10 mW" << std::endl;
    std::cout << "    std::cout << \"loss = \" << loss << std::endl; // -20 dBW = 10 mW"
              << std::endl;
    std::cout << "    #ifdef WONT_COMPILE" << std::endl;
    std::cout << "    std::cout << txPwr - loss  << std::endl; // Won't compile; mixing linear and "
                 "non-linear"
              << std::endl;
    std::cout << "    #endif" << std::endl << std::endl;

    dBW_t loss{-20};                             // equivalent to 10 mW
    std::cout << "loss = " << loss << std::endl; // -20 dBW = 10 mW
#ifdef WONT_COMPILE
    std::cout << txPwr - loss << std::endl; // Won't compile; mixing linear and non-linear
#endif
    std::cout << std::endl;

    std::cout << "We can solve this by converting the logarithmic quantity back to linear:"
              << std::endl
              << std::endl;

    std::cout << "    std::cout << \"txPwr - milliwatt_t(loss) = \" << txPwr - milliwatt_t(loss)  "
                 "<< std::endl; // OK, should print 90 mW"
              << std::endl
              << std::endl;
    std::cout << "Yields:" << std::endl << std::endl;
    std::cout << "txPwr - milliwatt_t(loss) = " << txPwr - milliwatt_t(loss) << std::endl
              << std::endl; // OK, should print 90 mW
    NS_ASSERT_MSG(txPwr - milliwatt_t(loss) == 90_mW, "Subtract 10 mW from 100 mW");

    std::cout << "Decibel (dB) is available in namespace units::dimensionless." << std::endl;
    std::cout << "We want to be able to add it to logarithmic power (but not linear power):"
              << std::endl
              << std::endl;

    std::cout << "    dB_t gain{10}" << std::endl;
    std::cout << "    std::cout << \"loss (-20 dBW) + gain (10 dB) = \" << loss + gain << std::endl"
              << std::endl;
    std::cout << "    #ifdef WONT_COMPILE" << std::endl;
    std::cout
        << "    std::cout << \"txPwr (100 mW) + gain (10 dB) = \" << txPwr + gain << std::endl"
        << std::endl;
    std::cout << "    #endif" << std::endl << std::endl;
    std::cout << "Yields:" << std::endl << std::endl;

    dB_t gain{10};
    std::cout << "loss (-20 dBW) + gain (10 dB) = " << loss + gain << std::endl;
#ifdef WONT_COMPILE
    std::cout << "txPwr (100 mW) + gain (10 dB) = " << txPwr + gain << std::endl;
#endif

    std::cout << std::endl;
    std::cout << "We want these types to be available to the ns-3 CommandLine" << std::endl;
    std::cout << "system and as Attribute values.  This is possible in the" << std::endl;
    std::cout << "usual way, as demonstrated by the Decibel value (src/core/model/decibel.h)."
              << std::endl
              << std::endl;

    std::cout << "The things needed to wrap these types are to define \"operator>>\"," << std::endl;
    std::cout << "and to use the ATTRIBUTE_* macros." << std::endl << std::endl;
    std::cout << "This example program demonstrates the use of a decibel value as a" << std::endl;
    std::cout << "CommandLine argument (--cmdLineDecibel).  Passing a plain double value"
              << std::endl;
    std::cout << "will raise an error about invalid values.  Instead, try this:" << std::endl
              << std::endl;
    std::cout << "    ./ns3 run units-example  -- --cmdLineDecibel=5_dB" << std::endl << std::endl;
    std::cout << "The value that you input will be printed below:" << std::endl << std::endl;
    std::cout << "cmdLineDecibel = " << cmdLineDecibel << std::endl << std::endl;
    std::cout << "Attribute values will look like the following (see wifi-phy.cc):" << std::endl
              << std::endl;
    std::cout << "Old code:" << std::endl << std::endl;

    std::cout << "        .AddAttribute(\"TxGain\"," << std::endl;
    std::cout << "                      \"Transmission gain (dB).\"," << std::endl;
    std::cout << "                      DoubleValue(0.0)," << std::endl;
    std::cout
        << "                      MakeDoubleAccessor(&WifiPhy::SetTxGain, &WifiPhy::GetTxGain),"
        << std::endl;
    std::cout << "                      MakeDoubleChecker<double>())" << std::endl << std::endl;

    std::cout << "New code:" << std::endl << std::endl;
    std::cout << "        .AddAttribute(\"TxGain\"," << std::endl;
    std::cout << "                      \"Transmission gain.\"," << std::endl;
    std::cout << "                      DecibelValue(0.0)," << std::endl;
    std::cout
        << "                      MakeDecibelAccessor(&WifiPhy::SetTxGain, &WifiPhy::GetTxGain),"
        << std::endl;
    std::cout << "                      MakeDecibelChecker())" << std::endl << std::endl;

    std::cout << "Client code will look like this (see wifi-phy-ofdma-test.cc)" << std::endl
              << std::endl;

    std::cout << "Old code:" << std::endl << std::endl;
    std::cout << "      phy->SetAttribute(\"TxGain\", DoubleValue(1.0));" << std::endl << std::endl;
    std::cout << "New code:" << std::endl << std::endl;
    std::cout << "      phy->SetAttribute(\"TxGain\", DecibelValue(units::dimensionless::dB_t(1)));"
              << std::endl
              << std::endl;

    std::cout << "Alternative new code (if \"using units::dimensionless;\" is added):" << std::endl
              << std::endl;
    std::cout << "      phy->SetAttribute(\"TxGain\", DecibelValue(dB_t(1)));" << std::endl
              << std::endl;
    std::cout << "Alternative new code (using the StringValue alternative):" << std::endl
              << std::endl;
    std::cout << "      phy->SetAttribute(\"TxGain\", StringValue(\"1_dB\"));" << std::endl
              << std::endl;

    return 0;
}
