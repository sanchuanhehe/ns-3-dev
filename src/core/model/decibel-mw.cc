/*
 * Copyright (c) 2008 INRIA
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Authors: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#include "decibel-mw.h"

#include "fatal-error.h"
#include "log.h"

#include <cstdlib>

/**
 * \file
 * \ingroup attribute_DecibelMw
 * ns3::DecibelMwValue attribute value implementation.
 */

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("DecibelMw");

ATTRIBUTE_CHECKER_IMPLEMENT_WITH_CONVERTER(units::power::dBm_t, DecibelMw);
ATTRIBUTE_VALUE_IMPLEMENT_WITH_NAME(units::power::dBm_t, DecibelMw);

} // namespace ns3
