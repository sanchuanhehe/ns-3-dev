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
#ifndef UNITS_ATTRIBUTES_H
#define UNITS_ATTRIBUTES_H

#include "units-angle.h"
#include "units-energy.h"

#include <ns3/attribute-helper.h>
#include <ns3/attribute.h>

namespace ns3
{

// See si-units-test-suite.cc for usages
ATTRIBUTE_HELPER_HEADER(dB);         // dBValue
ATTRIBUTE_HELPER_HEADER(dBm);        // dBmValue
ATTRIBUTE_HELPER_HEADER(mWatt);      // mWattValue
ATTRIBUTE_HELPER_HEADER(dBm_per_Hz); // dBm_per_HzValue
ATTRIBUTE_HELPER_HEADER(Hz);         // HzValue
ATTRIBUTE_HELPER_HEADER(degree);     // degreeValue
ATTRIBUTE_HELPER_HEADER(radian);     // radianValue

} // namespace ns3

#endif // UNITS_ATTRIBUTES_H
