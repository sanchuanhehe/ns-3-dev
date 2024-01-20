/*
 * Copyright (c) 2008 INRIA
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */

#ifndef APPLICATION_CONTAINER_H
#define APPLICATION_CONTAINER_H

#include "ns3/application.h"
#include "ns3/nstime.h"
#include "ns3/object-container.h"
#include "ns3/random-variable-stream.h"

#include <stdint.h>

namespace ns3
{

/**
 * @brief holds a vector of ns3::Application pointers.
 *
 * Typically ns-3 Applications are installed on nodes using an Application
 * helper.  The helper Install method takes a NodeContainer which holds
 * some number of Ptr<Node>.  For each of the Nodes in the NodeContainer
 * the helper will instantiate an application, install it in a node and
 * add a Ptr<Application> to that application into a Container for use
 * by the caller.  This is that container used to hold the Ptr<Application>
 * which are instantiated by the Application helper.
 */
class ApplicationContainer : public ObjectContainer<Application>
{
  public:
    using ObjectContainer<Application>::Iterator;
    using ObjectContainer<Application>::ObjectContainer;
    using ObjectContainer<Application>::Add;
    using ObjectContainer<Application>::Create;
    using ObjectContainer<Application>::Clear;
    using ObjectContainer<Application>::Begin;
    using ObjectContainer<Application>::End;
    using ObjectContainer<Application>::Get;
    using ObjectContainer<Application>::GetN;
    using ObjectContainer<Application>::operator[];
    using ObjectContainer<Application>::GetAllItems;
    using ObjectContainer<Application>::Contains;

    /**
     * @brief Start all of the Applications in this container at the start time
     * given as a parameter.
     *
     * All Applications need to be provided with a starting simulation time and
     * a stopping simulation time.  The ApplicationContainer is a convenient
     * place for allowing all of the contained Applications to be told to wake
     * up and start doing their thing (Start) at a common time.
     *
     * This method simply iterates through the contained Applications and calls
     * their Application::SetStartTime() methods with the provided Time.
     *
     * @param start The Time at which each of the applications should start.
     */
    void Start(Time start) const;

    /**
     * @brief Start all of the Applications in this container at the start time
     * given as a parameter, plus some jitter.
     *
     * This method iterates through the contained Applications and calls
     * their Application::SetStartTime() methods with the provided start Time, plus
     * a jitter value drawn from the provided random variable.
     *
     * @param start The Time at which each of the applications should start.
     * @param rv The random variable that adds jitter (units of seconds)
     */
    void StartWithJitter(Time start, Ptr<RandomVariableStream> rv) const;

    /**
     * @brief Arrange for all of the Applications in this container to Stop()
     * at the Time given as a parameter.
     *
     * All Applications need to be provided with a starting simulation time and
     * a stopping simulation time.  The ApplicationContainer is a convenient
     * place for allowing all of the contained Applications to be told to shut
     * down and stop doing their thing (Stop) at a common time.
     *
     * This method simply iterates through the contained Applications and calls
     * their Application::SetStopTime() methods with the provided Time.
     *
     * @param stop The Time at which each of the applications should stop.
     */
    void Stop(Time stop) const;
};

} // namespace ns3

#endif /* APPLICATION_CONTAINER_H */
