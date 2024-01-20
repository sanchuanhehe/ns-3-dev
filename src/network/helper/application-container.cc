/*
 * Copyright (c) 2008 INRIA
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */

#include "application-container.h"

#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("ApplicationContainer");

void
ApplicationContainer::Start(Time start) const
{
    for (auto i = Begin(); i != End(); ++i)
    {
        Ptr<Application> app = *i;
        app->SetStartTime(start);
    }
}

void
ApplicationContainer::StartWithJitter(Time start, Ptr<RandomVariableStream> rv) const
{
    for (auto i = Begin(); i != End(); ++i)
    {
        Ptr<Application> app = *i;
        double value = rv->GetValue();
        NS_LOG_DEBUG("Start application at time " << start.GetSeconds() + value << "s");
        app->SetStartTime(start + Seconds(value));
    }
}

void
ApplicationContainer::Stop(Time stop) const
{
    for (auto i = Begin(); i != End(); ++i)
    {
        Ptr<Application> app = *i;
        app->SetStopTime(stop);
    }
}

} // namespace ns3
