/*
 * Copyright (c) 2008 INRIA
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Authors: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#include "decibel-w.h"

#include "fatal-error.h"
#include "log.h"

#include <cstdlib>

/**
 * @file
 * @ingroup attribute_DecibelW
 * ns3::DecibelWValue attribute value implementation.
 */

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("DecibelW");

ATTRIBUTE_CHECKER_IMPLEMENT_WITH_CONVERTER(units::power::dBW_t, DecibelW);
ATTRIBUTE_VALUE_IMPLEMENT_WITH_NAME(units::power::dBW_t, DecibelW);

} // namespace ns3
