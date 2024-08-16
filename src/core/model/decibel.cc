/*
 * Copyright 2024 University of Washington
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#include "decibel.h"

#include "fatal-error.h"
#include "log.h"

#include <cstdlib>

/**
 * \file
 * \ingroup attribute_Decibel
 * ns3::DecibelValue attribute value implementation.
 */

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Decibel");

ATTRIBUTE_CHECKER_IMPLEMENT_WITH_CONVERTER(units::dimensionless::dB_t, Decibel);
ATTRIBUTE_VALUE_IMPLEMENT_WITH_NAME(units::dimensionless::dB_t, Decibel);

} // namespace ns3
