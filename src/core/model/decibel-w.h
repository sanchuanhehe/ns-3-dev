/*
 * Copyright (c) 2008 INRIA
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Authors: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#ifndef NS_DECIBEL_W_H
#define NS_DECIBEL_W_H

#include "attribute-helper.h"
#include "attribute.h"
#include "double.h"
#include "units.h"

/**
 * @file
 * @ingroup attribute_dBW_t
 * attribute value declaration
 *
 * wraps units::power::dBW_t
 */

namespace ns3
{

ATTRIBUTE_VALUE_DEFINE_WITH_NAME(units::power::dBW_t, DecibelW);
ATTRIBUTE_ACCESSOR_DEFINE(DecibelW);
ATTRIBUTE_CHECKER_DEFINE_WITH_CONVERTER(units::power::dBW_t, DecibelW, Double);

} // namespace ns3

#endif /* DECIBEL_W_H */
