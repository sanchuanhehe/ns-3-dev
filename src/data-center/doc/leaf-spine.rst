.. include:: replace.txt
.. highlight:: bash

Leaf Spine Topology
----------------

Model Description
*****************

Leaf-spine topology is most oftenly used for evaluating proposals (scheduling, rate control, load balancing,
routing) in data center networking. This helper is to make it easier to simulate with leaf-spine topology.
Leaf-spine topology is a two-tier Clos network shown in figure :ref:`fig-leaf-spine`, where the network consists 
of 2 layers of switches. Each spine switch is connected to each leaf switches (a.k.a. Top-of-Rack switch, or ToR switch) 
and each leaf switch is connected to a rack of servers. Within the leaf spine topology, two types of server-to-server
communication exist: inter-rack communication (source and destination servers belong to different racks)
and intra-rack communication (source and destination servers are under the same ToR switch), as shown in 
figure :ref:`fig-leaf-spine-animation`.

.. _fig-leaf-spine:

.. figure:: figures/leaf-spine.*
   :align: center
   :width: 500px
   :height: 400px

   Leaf Spine Topology ([Ref1]_)

.. _fig-leaf-spine-animation:

.. figure:: figures/leaf-spine-animation.*
   :align: center
   :width: 500px
   :height: 400px

   An Example of Packet Animation with Inter-rack and Intra-rack Flows

References
==========

.. [Ref1] BGP in the Data Center by Dinesh G. Dutt; Available online at `<https://www.oreilly.com/library/view/bgp-in-the/9781491983416/ch01.html>`_.

Examples
========

The example for simulating with leaf-spine topology helper is `leaf-spine-animation.cc` located in ``src/data-center/examples/``.
One could specify the workload to run the simulation (e.g., VL2, DCTCP)

.. sourcecode:: bash

   $ ./waf configure --enable-examples
   $ NS_LOG="LeafSpineAnimation" ./waf --run "leaf-spine-animation --workloadType=VL2"

It will generate the `leaf-spine-animation.xml` file which could be visualized via runnig `./NetAnim`.