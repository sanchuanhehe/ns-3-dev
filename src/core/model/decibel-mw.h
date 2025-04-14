/*
 * Copyright (c) 2008 INRIA
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Authors: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#ifndef NS_DECIBEL_MW_H
#define NS_DECIBEL_MW_H

#include "attribute-helper.h"
#include "attribute.h"
#include "double.h"
#include "units.h"

/**
 * @file
 * @ingroup attribute_dBm_t
 * attribute value declaration
 *
 * wraps units::power::dBm_t
 */

namespace ns3
{

ATTRIBUTE_VALUE_DEFINE_WITH_NAME(units::power::dBm_t, DecibelMw);
ATTRIBUTE_ACCESSOR_DEFINE(DecibelMw);
ATTRIBUTE_CHECKER_DEFINE_WITH_CONVERTER(units::power::dBm_t, DecibelMw, Double);

} // namespace ns3

#endif /* DECIBEL_MW_H */
