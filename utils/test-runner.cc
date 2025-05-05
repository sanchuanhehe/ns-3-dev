/*
 * Copyright (c) 2009 University of Washington
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "ns3/test.h"

int
main(int argc, char* argv[])
{
    int b = argc;
    return ns3::TestRunner::Run(b, argv);
}
