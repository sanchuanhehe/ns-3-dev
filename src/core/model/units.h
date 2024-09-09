/*
 * Copyright (c) 2024 University of Washington
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ifndef NS3_UNITS_H
#define NS3_UNITS_H

#include "units-nholthaus.h"

#include <iostream>
#include <string>

namespace ns3
{
// Stream extraction operators must be defined for units that will be used
// within Attributes or ns-3 CommandLine

/**
 * \brief Stream extraction operator for units::dimensionless::dB_t
 * \param [in,out] is The stream
 * \param [out] decibel the output value
 * \return The stream
 */
inline std::istream&
operator>>(std::istream& is, units::dimensionless::dB_t& decibel)
{
    std::string value;
    is >> value;
    bool ok = true;
    auto pos = value.find('_');
    if (pos == std::string::npos)
    {
        ok = false;
    }
    auto unit = value.substr(pos + 1);
    if (unit != "dB")
    {
        ok = false;
    }
    auto number = value.substr(0, pos);
    if (!ok)
    {
        is.setstate(std::ios_base::failbit);
    }
    decibel = units::dimensionless::dB_t(std::strtod(number.c_str(), nullptr));
    return is;
}

/**
 * \brief Stream extraction operator for units::power::dBm_t
 * \param [in,out] is The stream
 * \param [out] decibel the output value
 * \return The stream
 */
inline std::istream&
operator>>(std::istream& is, units::power::dBm_t& dBm)
{
    std::string value;
    is >> value;
    bool ok = true;
    auto pos = value.find('_');
    if (pos == std::string::npos)
    {
        ok = false;
    }
    auto unit = value.substr(pos + 1);
    if (unit != "dBm")
    {
        ok = false;
    }
    auto number = value.substr(0, pos);
    if (!ok)
    {
        is.setstate(std::ios_base::failbit);
    }
    dBm = units::power::dBm_t(std::strtod(number.c_str(), nullptr));
    return is;
}

// aliases
using DBm = units::power::dBm_t;

} // namespace ns3

#endif // NS3_UNITS_H
