/*
 * Copyright (c) 2008 INRIA
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Authors: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#ifndef NS_POWER_H
#define NS_POWER_H

#include "attribute-helper.h"
#include "attribute.h"
#include "double.h"
#include "units.h"

/**
 * \file
 * \ingroup attribute_watt_t
 * attribute value declaration
 *
 * wraps units::power::watt_t
 */

namespace ns3
{

ATTRIBUTE_VALUE_DEFINE_WITH_NAME(units::power::dBm_t, Power);
ATTRIBUTE_ACCESSOR_DEFINE(Power);
ATTRIBUTE_CHECKER_DEFINE_WITH_CONVERTER(units::power::dBm_t, Power, Double);

} // namespace ns3

#endif /* POWER_H */
