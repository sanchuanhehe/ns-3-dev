Undewater Acoustic Network (UAN) Framework
==========================================

The main goal of the UAN Framework is to enable researchers to
model a variety of underwater network scenarios.  The UAN model
is broken into four main parts:  The channel, PHY, MAC and
Autonomous Underwater Vehicle (AUV) models.

The need for underwater wireless communications exists in applications such as remote control in offshore oil industry [1]_, pollution monitoring in environmental systems, speech transmission between divers, mapping of the ocean floor, mine counter measures [2]_ [4]_, seismic monitoring of ocean faults as well as climate changes monitoring. Unfortunately, making on-field measurements is very expensive and there are no commonly accepted standard to base on. Hence, the priority to make research work going on, it is to realize a complete simulation framework that researchers can use to experiment, make tests and make performance evaluation and comparison.

The NS-3 UAN module is a first step in this direction, trying to offer a reliable and realistic tool.
In fact, the UAN module offers accurate modelling of the underwater acoustic channel, a model of the WHOI acoustic modem (one of the widely used acoustic modems) [6]_ validation of its
communications performance as well as some MAC protocols.

The UAN Framework is composed of two main parts:

* The AUV mobility models, including Electric motor propelled AUV (REMUS class [3]_ [4]_ ) and Seaglider [5]_ models
* The energy models, including AUV energy models, AUV energy sources (batteries) and an acoustic modem energy model

As enabling component for the energy models, a Li-Ion batteries from the energy module has been used.

The framework is designed to simulate the navigation and power consumption behaviour of REMUS class and Seaglider AUVs.
The communications stack, associated with the AUV, can be modified depending on simulation needs. Usually, the default underwater stack is being used,
composed of an half duplex acoustic modem, an Aloha MAC protocol and a generic physical layer.

Regarding the AUV energy consumption, the user should be aware that the level of accuracy differs for the two classes:

* Seaglider: High level of accuracy, thanks to the availability of detailed information on AUV's components and behaviour [5]_ [7]_.
Have been modeled both the navigation power consumption and the Li battery packs (according to [5]_).
* REMUS: Medium level of accuracy, due to the lack of publicly available information on AUV's components.
We have approximated the power consumption of the AUV's motor with a linear behaviour and, the energy source uses an ideal model (BasicEnergySource) with a power capacity equal to that specified in [4]_.

The source code for the UAN Framework lives in the directory
``src/uan`` and energy or battery related content in the ``src/energy``.


Scope and Limitations
---------------------

* Data logging cability is not supported.
* Dopler spread is not considered in the model.
* OFDM modulations are not included.
* The UAN framework only models PHY and MAC layers, therefore, application socket support (layer 4) is not included.


UAN Propagation Models
----------------------
Modelling of the underwater acoustic channel has been an active
area of research for quite some time.  Given the complications involved,
surface and bottom interactions, varying speed of sound, etc..., the detailed
models in use for ocean acoustics research are much too complex
(in terms of runtime) for use in network level simulations.  We have
attempted to provide the often used models as well as make an attempt to bridge, in part, the gap between
complicated ocean acoustic models and network level simulation.  The three propagation
models included are the ideal channel model, the Thorp propagation model and
the Bellhop propagation model (Available as an addition).

All of the Propagation Models follow the same simple interface in ``ns3::UanPropModel``.
The propagation models provide a power delay profile (PDP) and pathloss
information.  The PDP is retrieved using the GetPdp method which returns type UanPdp.
``ns3::UanPdp`` utilises a tapped delay line model for the acoustic channel.
The UanPdp class is a container class for Taps, each tap has a delay and amplitude
member corresponding to the time of arrival (relative to the first tap arrival time)
and amplitude.   The propagation model also provides pathloss between the source
and receiver in dB re 1uPa.  The PDP and pathloss can then be used to find the
received signal power over a duration of time (i.e. received signal power in
a symbol duration and ISI which interferes with neighbouring signals).  Both
UanPropModelIdeal and UanPropModelThorp return a single impulse for a PDP.

a) Ideal Channel Model ``ns3::UanPropModelIdeal``

The ideal channel model assumes 0 pathloss inside a cylindrical area with bounds
set by attribute.  The ideal channel model also assumes an impulse PDP.

b) Thorp Propagation Model ``ns3::UanPropModelThorp``

The Thorp Propagation Model calculates pathloss using the well-known Thorp approximation.
This model is similar to the underwater channel model implemented in ns2 as described here:

Harris, A. F. and Zorzi, M. 2007. Modeling the underwater acoustic channel in ns2. In Proceedings
of the 2nd international Conference on Performance Evaluation Methodologies and Tools
(Nantes, France, October 22 - 27, 2007). ValueTools, vol. 321. ICST (Institute for Computer
Sciences Social-Informatics and Telecommunications Engineering), ICST, Brussels, Belgium, 1-8.

The frequency used in calculation however, is the center frequency of the modulation as found from
ns3::UanTxMode.  The Thorp Propagation Model also assumes an impulse channel response.

c) Bellhop Propagation Model ``ns3::UanPropModelBh`` (Available as an addition)

The Bellhop propagation model reads propagation information from a database.  A configuration
file describing the location, and resolution of the archived information must be supplied via
attributes.  We have included a utility, create-dat, which can create these data files using the Bellhop
Acoustic Ray Tracing software (http://oalib.hlsresearch.com/).

The create-dat utility requires a Bellhop installation to run.  Bellhop takes
environment information about the channel, such as sound speed profile, surface height
bottom type, water depth, and uses a Gaussian ray tracing algorithm to determine
propagation information.  Arrivals from Bellhop are grouped together into equal length
taps (the arrivals in a tap duration are coherently summed).  The maximum taps are then
aligned to take the same position in the PDP.  The create-dat utility averages together
several runs and then normalizes the average such that the sum of all taps is 1.  The same
configuration file used to create the data files using create-dat should be passed via
attribute to the Bellhop Propagation Model.

The Bellhop propagation model is available as a patch.  The link address will be
made available here when it is posted online.  Otherwise email lentracy@gmail.com
for more information.

UAN PHY layer
-------------

The PHY has been designed to allow for relatively easy extension
to new networking scenarios.  We feel this is important as, to date,
there has been no commonly accepted network level simulation model
for underwater networks.  The lack of commonly accepted network simulation
tools has resulted in a wide array of simulators and models used to report
results in literature.  The lack of standardization makes comparing results
nearly impossible.

The main component of the PHY Model is the generic
PHY class, ``ns3::UanPhyGen``.  The PHY class's general responsibility
is to handle packet acquisition, error determination, and forwarding of successful
packets up to the MAC layer.  The Generic PHY uses two models for determination
of signal to noise ratio (SINR) and packet error rate (PER).  The
combination of the PER and SINR models determine successful reception
of packets.  The PHY model connects to the channel via a Transducer class.
The Transducer class is responsible for tracking all arriving packets and
departing packets over the duration of the events. How the PHY class and the PER and SINR models
respond to packets is based on the "Mode" of the transmission as described by the ``ns3::UanTxMode``
class.

When a MAC layer sends down a packet to the PHY for transmission it specifies a "mode number" to
be used for the transmission.  The PHY class accepts, as an attribute, a list of supported modes.  The
mode number corresponds to an index in the supported modes.  The UanTxMode contains simple modulation
information and a unique string id.  The generic PHY class will only acquire arriving packets which
use a mode which is in the supported modes list of the PHY.  The mode along with received signal power,
and other pertinent attributes (e.g. possibly interfering packets and their modes) are passed to the SINR
and PER models for calculation of SINR and probability of error.

Several simple example PER and SINR models have been created.
a) The PER models
- Default (simple) PER model (``ns3::UanPhyPerGenDefault``):  The Default PER model tests the packet against a threshold and
assumes error (with prob. 1) if the SINR is below the threshold or success if the SINR is above
the threshold
- Micromodem FH-FSK PER (``ns3::UanPhyPerUmodem``).  The FH-FSK PER model calculates probability of error assuming a
rate 1/2 convolutional code with constraint length 9 and a CRC check capable of correcting
up to 1 bit error.  This is similar to what is used in the receiver of the WHOI Micromodem.

b) SINR models
- Default Model (``ns3::UanPhyCalcSinrDefault``), The default SINR model assumes that all transmitted energy is captured at the receiver
and that there is no ISI.  Any received signal power from interferes acts as additional ambient noise.
- FH-FSK SINR Model (``ns3::UanPhyCalcSinrFhFsk``), The WHOI Micromodem operating in FH-FSK mode uses a predetermined hopping
pattern that is shared by all nodes in the network.  We model this by only including signal
energy receiving within one symbol time (as given by ``ns3::UanTxMode``) in calculating the
received signal power.  A channel clearing time is given to the FH-FSK SINR model via attribute.
Any signal energy arriving in adjacent signals (after a symbol time and the clearing time) is
considered ISI and is treated as additional ambient noise.   Interfering signal arrivals inside
a symbol time (any symbol time) is also counted as additional ambient noise
- Frequency filtered SINR (``ns3::UanPhyCalcSinrDual``).  This SINR model calculates SINR in the same manner
as the default model.  This model however only considers interference if there is an overlap in frequency
of the arriving packets as determined by UanTxMode.

In addition to the generic PHY a dual phy layer is also included (``ns3::UanPhyDual``).  This wraps two
generic phy layers together to model a net device which includes two receivers.  This was primarily
developed for UanMacRc, described in the next section.

UAN MAC Layers
--------------

Over the last several years there have been a myriad of underwater MAC proposals
in the literature.  We have included three MAC protocols with this distribution:
a) CW-MAC, a MAC protocol which uses a slotted contention window similar in nature to
the IEEE 802.11 DCF.  Nodes have a constant contention window measured in slot times (configured
via attribute).  If the channel is sensed busy, then nodes backoff by randomly (uniform distribution) choose
a slot to transmit in.  The slot time durations are also configured via attribute.  This MAC was described in

Parrish N.; Tracy L.; Roy S. Arabshahi P.; and Fox, W.,  System Design Considerations for Undersea Networks:
Link and Multiple Access Protocols , IEEE Journal on Selected Areas in Communications (JSAC), Special
Issue on Underwater Wireless Communications and Networks, Dec. 2008.

b) RC-MAC (``ns3::UanMacRc`` ``ns3::UanMacRcGw``) a reservation channel protocol which dynamically divides
the available bandwidth into a data channel and a control channel.  This MAC protocol
assumes there is a gateway node which all network traffic is destined for.  The current
implementation assumes a single gateway and a single network neighborhood (a single hop network).
RTS/CTS handshaking is used and time is divided into cycles.  Non-gateway nodes transmit RTS packets
on the control channel in parallel to data packet transmissions which were scheduled in the previous cycle
at the start of a new cycle, the gateway responds on the data channel with a CTS packet which includes
packet transmission times of data packets for received RTS packets in the previous cycle as well as bandwidth
allocation information.  At the end of a cycle ACK packets are transmitted for received data packets.

When a publication is available it will be cited here.

c) Simple ALOHA (``ns3::UanMacAloha``)  Nodes transmit at will.

AUV mobility models
-------------------

With the AUV mobility models the user is be able to:

* Program the AUV to navigate over a path of waypoints
* Control the velocity of the AUV
* Control the depth of the AUV
* Control the direction of the AUV
* Control the pitch of the AUV
* Tell the AUV to emerge or submerge to a specified depth


The implementation of a AUV navigation model involves two major categories of AUVs: electric motor propelled (like REMUS class [3]_ [4]_) and "sea gliders" [5]_.
The classic AUVs are submarine-like devices, propelled by an electric motor linked with a propeller. Instead, the "sea glider" class exploits small changes in its buoyancy that,
in conjunction with wings, can convert vertical motion to horizontal. Therefore, a glider will reach a point into the water by describing a "saw-tooth" like movement.
Modelling the AUV navigation, involves in considering a real-world AUV class thus, taking into account maximum speed, directional capabilities, emerging and submerging times.
Regarding the sea gliders, they are modelled with the characteristic saw-tooth movement and the AUV's speed driven by the buoyancy and glide angle.

.. _auvmobilitymodel:

.. figure:: figures/auvmobility-classes.*

    AUV's mobility model classes overview

An :cpp:class:`ns3::AuvMobilityModel` interface has been designed to give users a generic interface to access AUV's navigation functions.
The ``AuvMobilityModel`` interface is implemented by the ``RemusMobilityModel`` and the ``GliderMobilityModel`` classes. The AUV's mobility models organization it is shown in :ref:`auvmobilitymodel`.
Both models use a constant velocity movement, thus the AuvMobilityModel interface derives from the ConstantVelocityMobilityModel. The two classes hold the navigation parameters for the two different AUVs, like maximum pitch angles, maximum operating depth, maximum and minimum speed values. The Glider model holds also some extra parameters like maximum buoyancy values, and maximum and minimum glide slopes.
Both classes, ``RemusMobilityModel`` and ``GliderMobilityModel``, handle also the AUV power consumption, utilizing the relative power models.
Has been modified the ``WaypointMobilityModel`` to let it use a generic underlying ConstantVelocityModel to validate the waypoints and, to keep trace of the node's position. The default model is the classic ConstantVelocityModel but, for example in case of REMUS mobility model, the user can install the AUV mobility model into the waypoint model and then validating the waypoints against REMUS navigation constraints.


AUV Energy models
-----------------

The energy models have been designed as in the follows.


* Use a specific power profile for the acoustic modem
* Use a specific energy model for the AUV
* Trace the power consumption of AUV navigation, through AUV's energy model
* Trace the power consumption underwater acoustic communications, through acoustic modem power profile

We have integrated the Energy Model with the UAN module, to implement energy handling. We have implemented a specific energy model for the two AUV classes and, an energy source for Lithium batteries. This will be really useful for researchers to keep trace of the AUV operational life.
We have implemented also an acoustic modem power profile, to keep trace of its power consumption. This can be used to compare protocols specific power performance. In order to use such power profile, the acoustic transducer physical layer has been modified to use the modem power profile. We have decoupled the physical layer from the transducer specific energy model,
to let the users change the different energy models without changing the physical layer.

Basing on the Device Energy Model interface, it has been implemented a specific energy model for the two AUV classes (REMUS and Seaglider).
This models reproduce the AUV's specific power consumption to give users accurate information. This model can be naturally used to evaluates the AUV operating life, as well as mission-related power consumption, etc. Have been developed two AUV energy models:

* ``GliderEnergyModel``: Computes the power consumption of the vehicle based on the current buoyancy value and vertical speed [5]_
* ``RemusEnergyModel``: Computes the power consumption of the vehicle based on the current speed, as it is propelled by a brush-less electric motor
* ``AcousticModemEnergyModel``: Compute the power consumption of the acoustic modem set in the UAV.


Acoustic modem energy model
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Basing on the Device Energy Model interface, has been implemented a generic energy model for acoustic modem. The model allows to trace four modem's power-states: Sleep, Idle, Receiving, Transmitting. The default parameters for the energy model are set to fit those of the WHOI :math:`\mu`-modem. The class follows pretty closely the RadioEnergyModel class as the transducer behaviour is pretty close to that of a Wi-Fi radio.

The default power consumption values implemented into the model are as follows [6]_:

+--------------+---------------------+
| Modem State  | Power Consumption   |
+--------------+---------------------+
| TX           | 50 W                |
+--------------+---------------------+
| RX           | 158 mW              |
+--------------+---------------------+
| Idle         | 158 mW              |
+--------------+---------------------+
| Sleep        | 5.8 mW              |
+--------------+---------------------+


The UAN module PHY layer includes the implementation of an ``UpdatePowerConsumption`` method that takes the modem's state as parameter. It checks if an energy source is installed into the node and, if present,
use the ``AcousticModemEnergyModel`` to update the power consumption with the current modem's state.
The modem power consumption's update takes place whenever the modem changes its state. A user should take into account that, if the power consumption handling is enabled (if the node has an energy source installed), all the communications processes will terminate whether the node depletes all the energy source.


AUV Energy Sources
------------------

A generic Li-Ion battery model from the energy module (Panasonic CGR18650DA Li-Ion Battery) was used as an energy source.
The Electrochem 3B36 Lithium / Sulfuryl Chloride cells [7]_ was used as a battery model for the AUV Seagliders.



Usage
-----

The main way that users who write simulation scripts will typically
interact with the UAN Framework is through the helper API and through
the publicly visible attributes of the model.


Helpers
~~~~~~~

The helper API is defined in ``src/uan/helper/acoustic-modem-energy-model-helper.{cc,h}`` and in ``/src/uan/helper/...{cc,h}``.


**AcousticModemEnergyModelHelper:**

This helper installs ``AcousticModemEnergyModel`` into ``UanNetDevice`` objects only. It requires an ``UanNetDevice`` and an ``EnergySource`` as input objects.
The helper creates an ``AcousticModemEnergyModel`` with default parameters and associate it with the given energy source.
It configures an ``EnergyModelCallback`` and an ``EnergyDepletionCallback``. The depletion callback can be configured as a parameter.


**AuvGliderHelper:**

Installs into a node (or set of nodes) the Seaglider's features:

* Waypoint model with underlying glider mobility model
* Glider energy model
* Glider energy source
* Micro modem energy model

The glider mobility model is the GliderMobilityModel with default parameters.
The glider energy model is the GliderEnergyModel with default parameters.

Regarding the energy source, the Seaglider features two battery packs, one for motor power and one for digital-analog power.
Each pack is composed of 12 (10V) and 42 (24V) lithium chloride DD-cell batteries, respectively [5]_. The total power capacity is around 17.5 MJ (3.9 MJ + 13.6 MJ).
In the original version of the Seaglider there was 18 + 63 D-cell with a total power capacity of 10MJ.

The packs design is as follows:

* 10V - 3 in-series string x 4 strings = 12 cells - typical capacity ~100 Ah
* 24V - 7 in-series-strings x 6 strings = 42 cells - typical capacity ~150 Ah

Battery cells are Electrochem 3B36, with 3.6 V nominal voltage and 30.0 Ah nominal capacity.
The 10V battery pack is associated with the electronic devices, while the 24V one is associated with the pump motor.

The micro modem energy model is the MicroModemEnergyModel with default parameters.

**AuvRemusHelper:**

Install into a node (or set of nodes) the REMUS features:

* Waypoint model with REMUS mobility model validation
* REMUS energy model
* REMUS energy source
* Micro modem energy model

The REMUS mobility model is the RemusMobilityModel with default parameters.
The REMUS energy model is the RemusEnergyModel with default parameters.

Regarding the energy source, the REMUS features a rechargeable lithium ion battery pack rated 1.1 kWh @ 27 V (40 Ah) in operating conditions (specifications from [3]_ and Hydroinc European salesman).
Since more detailed information about battery pack were not publicly available, the energy source used is a BasicEnergySource.

The micro modem energy model is the MicroModemEnergyModel with default parameters.

Attributes
~~~~~~~~~~

Traces
~~~~~~


Examples and Tests
------------------

The example folder ``src/uan/examples/`` contain some basic code that shows how to set up and use the models.
Unit tests can be found in ``src/uan/test/`` folder.

Examples
~~~~~~~~

There are mobility related examples and UAN related ones.

The following list the AUV mobility examples:

* ``auv-energy-model.cc``: In this example we show the basic usage of an AUV energy model.
Specifically, we show how to create a generic node, adding to it a basic energy source
and consuming energy from the energy source. In this example we show the basic usage of
an AUV energy model.The Seaglider AUV power consumption depends on buoyancy and vertical speed values,
so we simulate a 20 seconds movement at 0.3 m/s of vertical speed and 138g of buoyancy.
Then a 20 seconds movement at 0.2 m/s of vertical speed and 138g of buoyancy and then a stop of 5 seconds.
The required energy will be drained by the model basing on the given buoyancy/speed values,
from the energy source installed onto the node. We finally register a callback to the TotalEnergyConsumption traced value.

* ``auv-mobility.cc``:  In this example we show how to use the AuvMobilityHelper to install an AUV mobility model into a (set of) node. Then we make the AUV to submerge to a depth of 1000 meters. We then set a callback function called on reaching of the target depth.
The callback then makes the AUV to emerge to water surface (0 meters). We set also a callback function called on reaching of the target depth.
The emerge callback then, stops the AUV. During the whole navigation process, the AUV's position is tracked by the TracePos function and plotted into a Gnuplot graph.

* ``waypoint-mobility.cc``: This example shows use the WaypointMobilityModel with a non-standard ConstantVelocityMobilityModel.
The example first creates a waypoint model with an underlying RemusMobilityModel setting the mobility trace with two waypoints.
Then creates a waypoint model with an underlying GliderMobilityModel setting the waypoints separately with the AddWaypoint method.
The AUV's position is printed out every seconds.

The following lists the UAN examples available:

* ``uan-energy-auv.cc``: This is a comprehensive example where all the project's components are used.
First, two nodes are set, one fixed surface gateway equipped with an acoustic modem and a moving Seaglider AUV with an acoustic modem too.
Using the waypoint mobility model with an underlying GliderMobilityModel, the glider is descended to -1000 meters and then emerge to the water surface.
The AUV sends a generic 17-bytes packet every 10 seconds during the navigation process. The gateway receives the packets and stores the total bytes amount.
At the end of the simulation are shown the energy consumptions of the two nodes and the networking stats.

Tests
~~~~~

The following tests have been written, which can be found in ``src/uan/tests/``:

Auv Energy Model Test (``auv-energy-model-test.cc``):

The energy consumption test do the following:

* creates a two node network, one surface gateway and one fixed node at -500 m of depth
* install the acoustic communication stack with energy consumption support into the nodes
* a packet is sent from the underwater node to the gateway
* it is verified that both, the gateway and the fixed node, have consumed the expected amount of energy from their sources

The energy depletion test do the following steps:

* create a node with an empty energy source
* try to send a packet
* verify that the energy depletion callback has been invoked

The Glider energy consumption test do the following:

* create a node with glider capabilities
* make the vehicle to move to a predetermined waypoint
* verify that the energy consumed for the navigation is correct, according to the glider specifications

The REMUS energy consumption test do the following:

* create a node with REMUS capabilities
* make the vehicle to move to a predetermined waypoint
* verify that the energy consumed for the navigation is correct, according to the REMUS specifications

Auv Mobility Test (``auv-mobility-test.cc``):

* Create a node with glider capabilities
* Set a specified velocity vector and verify if the resulting buoyancy is the one that is supposed to be
* Make the vehicle to submerge to a specified depth and verify if, at the end of the process the position is the one that is supposed to be
* Make the vehicle to emerge to a specified depth and verify if, at the end of the process the position is the one that is supposed to be
* Make the vehicle to navigate to a specified point, using direction, pitch and speed settings and, verify if at the end of the process the position is the one that is supposed to be
* Make the vehicle to navigate to a specified point, using a velocity vector and, verify if at the end of the process the position is the one that is supposed to be

The REMUS mobility model test do the following:

* Create a node with glider capabilities
* Make the vehicle to submerge to a specified depth and verify if, at the end of the process the position is the one that is supposed to be
* Make the vehicle to emerge to a specified depth and verify if, at the end of the process the position is the one that is supposed to be
* Make the vehicle to navigate to a specified point, using direction, pitch and speed settings and, verify if at the end of the process the position is the one that is supposed to be
* Make the vehicle to navigate to a specified point, using a velocity vector and, verify if at the end of the process the position is the one that is supposed to be

Validation
----------

Validation was done with the test described in the previous sections by comparing the results
with the REMUS specifications.


References
----------

[`1 <http://www.fig.net/pub/fig_2002/Ts4-4/TS4_4_bingham_etal.pdf>`_] BINGHAM, D.; DRAKE, T.; HILL, A.; LOTT, R.; The Application of Autonomous Underwater Vehicle (AUV) Technology in the Oil Industry – Vision and Experiences.
[`2 <http://oceanexplorer.noaa.gov/explorations/08auvfest/background/mines/mines.html>`_] AUVfest2008: Underwater mines
[`3 <https://tsd.huntingtoningalls.com/what-we-do/unmanned-systems/unmanned-underwater-vehicles/>`_] Hydroinc (acquired by Huntington Ingalls Industries) Products
[`4 <http://www.whoi.edu/page.do?pid=29856>`_] WHOI, Autonomous Underwater Vehicle, REMUS
[`5 <http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=972073&userType=inst>`_] Eriksen, C.C., T.J. Osse, R.D. Light, T. Wen, T.W. Lehman, P.L. Sabin, J.W. Ballard, and A.M.
       Chiodi. Seaglider: A Long-Range Autonomous Underwater Vehicle for Oceanographic Research,
       IEEE Journal of Oceanic Engineering, 26, 4, October 2001.
[`6 <http://ieeexplore.ieee.org/iel5/10918/34367/01639901.pdf>`_] L. Freitag, M. Grund, I. Singh, J. Partan, P. Koski, K. Ball, and W. Hole, The whoi
       micro-modem: an acoustic communications and navigation system for multiple platforms,
       In Proc. IEEE OCEANS05 Conf, 2005.
[`7 <http://www.electrochem.com.cn/products/Primary/HighRate/CSC/3B36.pdf>`_] Electrochem 3B36 Datasheet.

