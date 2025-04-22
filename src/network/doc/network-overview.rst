.. include:: replace.txt

Node and NetDevices Overview
----------------------------

This chapter describes how |ns3| nodes are put together, and provides a
walk-through of how packets traverse an internet-based Node.

.. _node-architecture:

.. figure:: figures/node.*

    High-level node architecture

In |ns3|, nodes are instances of :cpp:class:`ns3::Node`. This class may be
subclassed, but instead, the conceptual model is that we *aggregate* or insert
objects to it rather than define subclasses.

One might think of a bare |ns3| node as a shell of a computer, to which one may
add NetDevices (cards) and other innards including the protocols and
applications. :ref:`node-architecture` illustrates that :cpp:class:`ns3::Node`
objects contain a list of :cpp:class:`ns3::Application` instances (initially,
the list is empty), a list of :cpp:class:`ns3::NetDevice` instances (initially,
the list is empty), a list of :cpp:class:`ns3::Node::ProtocolHandler` instances,
a unique integer ID, and a system ID (for distributed simulation).

The design tries to avoid putting too many dependencies on the class
:cpp:class:`ns3::Node`, :cpp:class:`ns3::Application`, or
:cpp:class:`ns3::NetDevice` for the following:

* IP version, or whether IP is at all even used in the :cpp:class:`ns3::Node`.
* implementation details of the IP stack.

From a software perspective, the lower interface of applications corresponds to
the C-based sockets API. The upper interface of :cpp:class:`ns3::NetDevice`
objects corresponds to the device independent sublayer of the Linux stack.
Everything in between can be aggregated and plumbed together as needed.

Let's look more closely at the protocol demultiplexer. We want incoming frames
at layer-2 to be delivered to the right layer-3 protocol such as IPv4. The
function of this demultiplexer is to register callbacks for receiving packets.
The callbacks are indexed based on the `EtherType
<http://en.wikipedia.org/wiki/EtherType>`_ in the layer-2 frame.

Many different types of higher-layer protocols may be connected to the
NetDevice, such as IPv4, IPv6, ARP, MPLS, IEEE 802.1x, and packet sockets.
Therefore, the use of a callback-based demultiplexer avoids the need to use a
common base class for all of these protocols, which is problematic because of
the different types of objects (including packet sockets) expected to be
registered there.

NetDevice States and Behavior
*****************************
This section is aimed at giving a general guideline on how NetDevices behave
when certain events happen. Linux distinguishes between the administrative state
and operational state of a NetDevice. Administrative state reflects whether
the user wants to use the NetDevice or not. In Linux, this state is the
result of ``ip`` or ``ifconfig`` command. For example, ``ip link set eno1 up`` or
``ip link set down`` using ``ip`` command. UP state means the user wants to
use the device for data transmission and DOWN state means the user doesn't want
to use this device. A NetDevice's administrative state can change to UP
in two ways:

* By means of explicit management action i.e. by the use of a command such as ``ip``.
* As a result of the system's initialization process.

To mimic an explicit management action in ns-3, user can use ``SetUp ()`` function to bring
the NetDevice to UP state and  ``SetDown()`` to bring the NetDevice to DOWN state.
When a Device transitions to UP state, whether or not the operational state will
change is dependent on the type of NetDevice in question. For example in ``CsmaNetDevice``,
if the channel is attached then operational state will change to UP immediately
whereas in ``PointToPointNetDevice``, operational state changes when the device is
connected and there is an active device on other the end as well. When a device is brought
to DOWN state, the operational state also changes to DOWN state.

.. _administrative-state-change:

.. figure:: figures/admin-state-diagram.*

    State Diagram showing administrative states transistions of a NetDevice

The transitions shown in the figure are listed below:

* **Add Device to Node**: This event is caused by function :cpp:class:`Node::AddDevice  ()`
  in ``node.cc``. Once a NetDevice object is created, it is added to a list
  of devices in the node. NetDeviceState object is created and aggregated
  to NetDevice object if the NetDevice wants to make use of features in
  NetDeviceState class. Helper classes take care of these tasks.
  NetDeviceState is initialized administratively UP state at the beginning of the
  simulation since we want to use the NetDevice from the get-go.

  .. note:: The INVALID state shown in the diagram is not a real state. When a NetDevice is created, it is not attached to any node. That is represented as INVALID state here. At this point :cpp:class:`NetDevice::GetNode (void)` will return NULL.

* **Activate Device**: This event is triggered by :cpp:class:`NetDeviceState::SetUp ()`
  in NetDeviceState class. This function acts as an administrative command.
  When one wants to bring up a NetDevice in the middle of simulation, this
  function can be used. At the start of the simulation, NetDevices are
  administratively UP so need explicitly call :cpp:class:`NetDeviceState::SetUp ()`.

* **Deactivate Device**: This event is triggered by :cpp:class:`NetDeviceState::SetDown ()` in
  NetDeviceState class. A user wanting to bring down a NetDevice can use this
  function.

* **Remove Device from Node**: There are no provisions to remove a NetDevice from a
  node right now.

For the device to be able to transmit and receive packets, it needs to be
connected to a channel. This ability of the device is represented by
operational states. A NetDevice has the following operational states:

* IF_OPER_UP
* IF_OPER_DOWN
* IF_OPER_DORMANT
* IF_OPER_LOWERLAYERDOWN

These operational states are based on the implementation of operational
states in Linux based on operational states mentioned in
RFC 2863: The Interfaces Group MIB. More information on operational
states can be found in below links:

* https://tools.ietf.org/html/rfc2863
* https://www.kernel.org/doc/Documentation/networking/operstates.txt


Some of the operational states are not used. A short description
of each operational state can be found below:

* **IF_OPER_UP**: A NetDevice transitions to this state when a
  channel (carrier) gets attached to it. The device is now UP and RUNNING.

* **IF_OPER_DOWN**: A NetDevice transitions to this state when the
  device loses its channel (carrier). The device is now UP but not
  RUNNING.

* **IF_OPER_DORMANT**: Interface is L1 up, but waiting for an external event,
  for example for a protocol to establish such as 802.1X.

* **IF_OPER_LOWERLAYERDOWN**: Useful only in stacked interfaces. An interface
  stacked on another interface that is in IF_OPER_DOWN shows this state
  (for example, VLAN).

The below mentioned operational states are part of RFC 2863 but are not used
due to reasons specified against them:

* **IF_OPER_UNKNOWN**:  Used for devices where RFC 2863 operational states are not
  implemented in their device drivers in Linux kernel. In ns-3, devices
  that does not use RFC 2863 operational states do not aggregate
  NetDeviceState object with them. This state is therefore not needed.

* **IF_OPER_NOTPRESENT**: Can be used to denote removed netdevices. Not used
  in linux kernel. Removed devices disappear and there is no need to
  maintain a state for them.

* IF_OPER_TESTING: Unused in Linux kernel. Testing mode; not relevant in ns-3 either.

If a device is UP and in IF_OPER_UP  state, it means that the device is
administratively enabled and notionally connected to a carrier and is
ready to transmit and receive packets.

A device is not usable by higher layers if it is not administratively
UP or if the device is not in IF_OPER_UP operational state.
