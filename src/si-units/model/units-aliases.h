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
#ifndef UNITS_ALIASES_H
#define UNITS_ALIASES_H

#include <cinttypes>
#include <limits>

namespace ns3
{

const auto ONE_KILO = 1000L;
const auto ONE_MEGA = ONE_KILO * ONE_KILO;
const auto ONE_GIGA = ONE_MEGA * ONE_KILO;
const auto ONE_TERA = ONE_GIGA * ONE_KILO;
const auto ONE_PETA = ONE_TERA * ONE_KILO;

// Small numbers
constexpr double EPSILON = std::numeric_limits<double>::epsilon(); // Minimum resolution
constexpr double PPM = 0.000001; // Part Per Million. Accommodate a 6 digit below point

// Weak-type aliases to assist 2-pass migration

using dB_t = double;
using dBm_t = double;

using meter_t = double;
using ppm_t = double;

using ampere_t = double;
using joule_t = double;
using volt_t = double;

} // namespace ns3

#endif // UNITS_ALIASES_H
