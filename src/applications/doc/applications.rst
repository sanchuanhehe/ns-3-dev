.. include:: replace.txt

3GPP HTTP applications
----------------------

Model Description
*****************

The model is a part of the applications library. The HTTP model is based on a commonly
used 3GPP model in standardization `[4]`_.

Design
======

This traffic generator simulates web browsing traffic using the Hypertext
Transfer Protocol (HTTP). It consists of one or more ``ThreeGppHttpClient``
applications which connect to a ``ThreeGppHttpServer`` application. The client
models a web browser which requests web pages to the server. The server
is then responsible to serve the web pages as requested. Please refer to
``ThreeGppHttpClientHelper`` and ``ThreeGppHttpServerHelper`` for usage instructions.

Technically speaking, the client transmits *request objects* to demand a
service from the server. Depending on the type of request received, the
server transmits either:

  - a *main object*, i.e., the HTML file of the web page; or
  - an *embedded object*, e.g., an image referenced by the HTML file.

The main and embedded object sizes are illustrated in figures :ref:`fig-http-main-object-size`
and :ref:`fig-http-embedded-object-size`.


.. _fig-http-main-object-size:

.. figure:: figures/http-main-object-size.*
   :figwidth: 15cm

   3GPP HTTP main object size histogram

.. _fig-http-embedded-object-size:

.. figure:: figures/http-embedded-object-size.*
   :figwidth: 15cm

   3GPP HTTP embedded object size histogram

\

A major portion of the traffic pattern is *reading time*, which does not
generate any traffic. Because of this, one may need to simulate a good
number of clients and/or sufficiently long simulation duration in order to
generate any significant traffic in the system. Reading time is illustrated in
:ref:`fig-http-reading-time`.

.. _fig-http-reading-time:

.. figure:: figures/http-reading-time.*
   :figwidth: 15cm

   3GPP HTTP reading time histogram


3GPP HTTP server description
############################

3GPP HTTP server is a model application which simulates the traffic of a web server. This
application works in conjunction with ``ThreeGppHttpClient`` applications.

The application works by responding to requests. Each request is a small
packet of data which contains ``ThreeGppHttpHeader``. The value of the *content type*
field of the header determines the type of object that the client is
requesting. The possible type is either a *main object* or an *embedded object*.

The application is responsible to generate the right type of object and send
it back to the client. The size of each object to be sent is randomly
determined (see ``ThreeGppHttpVariables``). Each object may be sent as multiple packets
due to limited socket buffer space.

To assist with the transmission, the application maintains several instances
of ``ThreeGppHttpServerTxBuffer``. Each instance keeps track of the object type to be
served and the number of bytes left to be sent.

The application accepts connection request from clients. Every connection is
kept open until the client disconnects.

Maximum transmission unit (MTU) size is configurable in ``ThreeGppHttpServer`` or in
``ThreeGppHttpVariables``. By default, the low variant is 536 bytes and high variant is 1460 bytes.
The default values are set with the intention of having a TCP header (size of which is 40 bytes) added
in the packet in such way that lower layers can avoid splitting packets. The change of MTU sizes
affects all TCP sockets after the server application has started. It is mainly visible in sizes of
packets received by ``ThreeGppHttpClient`` applications.

3GPP HTTP client description
############################

3GPP HTTP client is a model application which simulates the traffic of a web browser. This
application works in conjunction with an ThreeGppHttpServer application.

In summary, the application works as follows.

1. Upon start, it opens a connection to the destination web server
   (ThreeGppHttpServer).
2. After the connection is established, the application immediately requests
   a *main object* from the server by sending a request packet.
3. After receiving a main object (which can take some time if it consists of
   several packets), the application "parses" the main object. Parsing time
   is illustrated in figure :ref:`fig-http-parsing-time`.
4. The parsing takes a short time (randomly determined) to determine the
   number of *embedded objects* (also randomly determined) in the web page.
   Number of embedded object is illustrated in :ref:`fig-http-num-of-embedded-objects`.

    * If at least one embedded object is determined, the application requests
       the first embedded object from the server. The request for the next
       embedded object follows after the previous embedded object has been
       completely received.
    * If there is no more embedded object to request, the application enters
       the *reading time*.

5. Reading time is a long delay (again, randomly determined) where the
   application does not induce any network traffic, thus simulating the user
   reading the downloaded web page.
6. After the reading time is finished, the process repeats to step #2.

.. _fig-http-parsing-time:

.. figure:: figures/http-parsing-time.*
   :figwidth: 15cm

   3GPP HTTP parsing time histogram

.. _fig-http-num-of-embedded-objects:

.. figure:: figures/http-num-of-embedded-objects.*
   :figwidth: 15cm

   3GPP HTTP number of embedded objects histogram

The client models HTTP *persistent connection*, i.e., HTTP 1.1, where the
connection to the server is maintained and used for transmitting and receiving
all objects.

Each request by default has a constant size of 350 bytes. A ``ThreeGppHttpHeader``
is attached to each request packet. The header contains information
such as the content type requested (either main object or embedded object)
and the timestamp when the packet is transmitted (which will be used to
compute the delay and RTT of the packet).


References
==========

Many aspects of the traffic are randomly determined by ``ThreeGppHttpVariables``.
A separate instance of this object is used by the HTTP server and client applications.
These characteristics are based on a legacy 3GPP specification. The description
can be found in the following references: `[1]`_, `[2]`_, `[3]`_, and `[4]`_.


Usage
*****

The three-gpp-http-example can be referenced to see basic usage of the HTTP applications.
In summary, using the ``ThreeGppHttpServerHelper`` and ``ThreeGppHttpClientHelper`` allow the
user to easily install ``ThreeGppHttpServer`` and ``ThreeGppHttpClient`` applications to nodes.
The helper objects can be used to configure attribute values for the client
and server objects, but not for the ``ThreeGppHttpVariables`` object. Configuration of variables
is done by modifying attributes of ``ThreeGppHttpVariables``, which should be done prior to helpers
installing applications to nodes.

The client and server provide a number of ns-3 trace sources such as
"Tx", "Rx", "RxDelay", and "StateTransition" on the server side, and a large
number on the client side ("ConnectionEstablished",
"ConnectionClosed","TxMainObjectRequest", "TxEmbeddedObjectRequest",
"RxMainObjectPacket", "RxMainObject", "RxEmbeddedObjectPacket",
"RxEmbeddedObject", "Rx", "RxDelay", "RxRtt", "StateTransition").


Building the 3GPP HTTP applications
===================================

Building the applications does not require any special steps to be taken. It suffices to enable
the applications module.

Examples
========

For an example demonstrating HTTP applications
run::

  $ ./ns3 run 'three-gpp-http-example'

By default, the example will print out the web page requests of the client and responses of the
server and client receiving content packets by using LOG_INFO of ``ThreeGppHttpServer`` and ``ThreeGppHttpClient``.

Tests
=====

For testing HTTP applications, three-gpp-http-client-server-test is provided. Run::

  $ ./test.py -s three-gpp-http-client-server-test

The test consists of simple Internet nodes having HTTP server and client applications installed.
Multiple variant scenarios are tested: delay is 3ms, 30ms or 300ms, bit error rate 0 or 5.0*10^(-6),
MTU size 536 or 1460 bytes and either IPV4 or IPV6 is used. A simulation with each combination of
these parameters is run multiple times to verify functionality with different random variables.

Test cases themselves are rather simple: test verifies that HTTP object packet bytes sent match
total bytes received by the client, and that ``ThreeGppHttpHeader`` matches the expected packet.


NGMN traffic generators
-----------------------

Model Description
*****************

The NGMN traffic generators are a part of the applications library. In the following subsections are described the design,
different NGMN traffic generators being implemented, and the example and the test. More details and validation figures
can be found in `[5]`_.

Design
======
``TrafficGenerator`` class is created with an idea to serve as a base class for different traffic generators by allowing
a high-level of granularity of the possible configurations. It supports:

- A variable packet burst size to support a wide range of different use cases, from just several packets to whatever size
of the packet burst expressed in bytes. To support this it was necessary to allow specifying the packet burst size in
either the number of bytes or a number of packets. For this we have created the function ``GenerateNextPacketBurstSize``
which should be overridden by the child traffic generator class to generate the next packet burst size in bytes or in
packets and set the corresponding value by calling either ``SetPacketBurstSizeInBytes`` or ``SetPacketBurstSizeInPackets``.

- A variable packet arrival time to support different use cases needed by different traffic models. ``TrafficGenerator``
supports both, a deterministic and a variable packet arrival time. The function that should be implemented
by the child classes is called ``GetNextPacketTime``.

- A variable packet size to support different traffic models. ``TrafficGenerator`` supports both, a deterministic
and a variable packet arrival time. The function that should be implemented by the child classes is called ``GetNextPacketSize``.

Different traffic generators have been implemented by inheriting ``TrafficGenerator`` class and according to NGMN
Alliance `[3]`_, such as FTP, video streaming, gaming and VoIP. In the following subsection we describe
these models. ``TrafficGeneratorHelper`` helper class is created to to help with the installation of the
desired ``TrafficGenerator``` on the node container. It allows to configure the transport protocol (TCP/UDP),
and the remote address.

NGMN FTP traffic generator
##########################

``TrafficGeneratorNgmnFtpMulti`` class implements the NGMN FTP traffic model defined in the Annex A of the White Paper
by the NGMN Alliance `[3]`_. This file transfer application allows sending multiple files in a row where each file is of
a variable file size with a variable reading time. An FTP session is a sequence of file transfers separated by reading times.
The two main FTP session parameters are: 1) The size S of a file to be transferred. 2) The reading time D, i.e. the time
interval between the end of the download of the previous file and the user request for the next file. The file size follows
Truncated Lognormal Distribution, while the reading time follows Exponential Distribution.

``TrafficGeneratorNgmnFtpMulti`` class overrides ``GenerateNextPacketBurstSize`` to generate a packet burst size value
according to a truncated log-normal distribution. The reading time generation is implemented in ``GetNextReadingTime``
which is called each time that the file transfer is finished, i.e., the ``PacketBurstSent``` function is called, and then
the next file transfer is scheduled to start once the reading time finishes.

NGMN Video traffic generator
############################

``TrafficGeneratorNgmnVideo`` class implements the NGMN video application traffic model explained in Annex A of the White Paper by
the NGMN Alliance `[3]`_. As per this model, the video traffic is composed of frames. Each frame of video data arrives at
a regular interval T determined by the number of frames per second. Each frame is decomposed into a fixed number of slices,
each transmitted as a single packet. The size of these packets/slices is modeled to have a truncated Pareto distribution.
The video encoder introduces encoding delay intervals between the packets of a frame. These intervals are modeled by a
truncated Pareto distribution. The default configurations of distributions of packet size and packet time assume a source
video rate of 64 kbps.

``TrafficGeneratorNgmnVideo`` class implements functions ``GetNextPacketSize`` and ``GetNextPacketTime`` to allow a generation
of the packet size and the packet arrival time according to Pareto distribution, as defined in `[3]`_.
Once that the video frame is being generated and sent, the function ``PacketBurstSent`` is called,
which will schedule the next frame after a deterministic inter-frame interval time as specified by the NGMN document.
Notice that one could generate another kind of video traffic in which the inter-frame interval time would be variable and
follow a specific probability distribution.

NGMN gaming traffic generator
#############################

``TrafficGeneratorNgmnGaming`` class implements the NGMN gaming traffic models for both downlink and uplink explained
in Annex A of the White Paper by the NGMN Alliance `[3]`_. Gaming application in NGMN is defined for interactive
real-time services, in both downlink and uplink directions with a different parametrization and characterization for
each direction. To simulate the random timing relationship between client traffic packet arrival and uplink frame boundary,
the starting time of a network gaming mobile is uniformly distributed within [0, 40 ms], both for downlink and uplink gaming.
The gaming session is a sequence of packets with a certain inter-packet arrival time. A gaming session is characterized
by two parameters: the packet size (S) and the inter-packet arrival time (D). Additionally, a UDP header is added to
the packet size to account for the UDP header after header compression. The UDP header size is 2 Bytes both for downlink
and uplink gaming.

``TrafficGeneratorNgmnGaming`` class has a function ``GetInitialPacketArrivalTime`` to support an initial packet arrival
time generation according to the NGMN gaming model `[3]`_. Additionally, ``GetNextPacketSize`` and ``GetNextPacketTime``
are implemented to generate the packet size and the packet arrival time according to Fisher-Tippett distribution
(except for the packet arrival for the uplink which is defined to be deterministic).

NGMN VoIP traffic generator
###########################

``TrafficGeneratorNgmnVoip`` class implements the NGMN VoIP traffic model as defined in Annex B of the White Paper by
NGMN Alliance `[3]`_.  According to this document, the VOIP traffic can be modeled as a simple 2-state voice activity model.
The states are: 1) Inactive State, and 2) Active State. In the VoIP model, the probability of transitioning from state 1 (the active
speech state) to state 0 (the inactive or silent state) is equal to "a", while the probability of transitioning from state 0 to
state 1 is "c". The model is updated at the speech encoder frame rate R=1/T, where T is the encoder frame duration (typically, 20ms).
A 2-state model is extremely simplistic, and many more complex models are available. However, it is amenable to rapid analysis and
initial estimation of talk spurt arrival statistics and hence reservation activity. The main purpose of this traffic model
is not to favor any codec but to specify a model to obtain results that are comparable.

To support the transitions from the active to inactive state, and vice versa, the ``TrafficGeneratorNgmnVoip`` class
implements a simple state machine that is being updated after each encoder frame duration, i.e., a function called
``UpdateState`` generates a random value (between 0 and 1) based on the uniform random variable and compares the generated
value with the probabilities (``a`` or ``c``) in order to determine whether to stay in the same state or to transit to
another state. While being in the active state, the voice payload is being used as the packet size, while meanwhile in
the inactive state the SID payload is being used. While in active state the source rate is 12.2 kbps and based on this is
calculated the packet arrival time, while when in the inactive state the periodicity of the SID payload is 160 ms.

Example
=======
Example ``three-gpp-http-example.cc`` is created to show how to configure and use different types of traffic generators.
The example allows to configure the type of NGMN traffic and the protocol to be used, UDP or TCP. The statistics are
obtain periodically and written into the file.

Test
====
Test implemented in ``traffic-generator-test.cc`` tests that all the packets generated by the client application
(e.g., NGMN VoIP, NGMN VIDEO, NGMN GAMING, NGMN FTP, etc.) are correctly received by the server application and
tests whether the traffic generator works correctly with the different protocols (TCP/UCP).


References
----------

\

.. _`[1]`:

[1] 3GPP TR 25.892, "Feasibility Study for Orthogonal Frequency Division Multiplexing (OFDM) for UTRAN enhancement"

\

.. _`[2]`:

[2] IEEE 802.16m, "Evaluation Methodology Document (EMD)", IEEE 802.16m-08/004r5, July 2008.

\

.. _`[3]`:

[3] NGMN Alliance, "NGMN Radio Access Performance Evaluation Methodology", v1.0, January 2008.

\

.. _`[4]`:

[4] 3GPP2-TSGC5, "HTTP, FTP and TCP models for 1xEV-DV simulations", 2001.

\

.. _`5`:

[5] Biljana Bojovic and Sandra Lagen. "Enabling NGMN Mixed Traffic Models for ns-3", In Proceedings of the 2022 Workshop
on ns-3 (WNS3 '22). ACM, New York, NY, USA, 127–134. https://doi.org/10.1145/3532577.3532602
