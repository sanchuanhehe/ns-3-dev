/*
 * Copyright 2024 University of Washington
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ifndef NS_DECIBEL_H
#define NS_DECIBEL_H

#include "attribute-helper.h"
#include "attribute.h"
#include "double.h"
#include "units.h"

/**
 * @file
 * @ingroup attribute_dB_t
 * attribute value declaration
 *
 * wraps units::dimensionless::dB_t
 */

namespace ns3
{

ATTRIBUTE_VALUE_DEFINE_WITH_NAME(units::dimensionless::dB_t, Decibel);
ATTRIBUTE_ACCESSOR_DEFINE(Decibel);
ATTRIBUTE_CHECKER_DEFINE_WITH_CONVERTER(units::dimensionless::dB_t, Decibel, Double);

} // namespace ns3

#endif /* DECIBEL_H */
