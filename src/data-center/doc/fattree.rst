.. include:: replace.txt
.. highlight:: cpp

FatTree topology
--------------

Fattree topolgy is a scalable, commodity data center network architecture. Fattree
architecture handles the oversubscription and cross section bandwidth problem faced 
by the legacy three-tier Data Center Network architecture. Fattree employs commodity 
network switches based architecture using Clos topology. Fat tree has identical 
bandwidth at any bisections. The fat tree topology offers 1:1 oversubscription ratio 
and full bisection bandwidth. Each layer has the same aggregated bandwidth. It can be 
built using cheap devices with uniform capacity. Each port supports same speed as end 
host. All devices can transmit at line speed if packets are distributed uniform along 
available paths. It provides great scalability as k-port switch supports k^3/4 servers ([Ref1]_).

Construction of FatTree
---------------------

A fattree consits of four-layers consisting of core-switches at the topmost level, 
aggregation switches below core-switches, edge switches below aggregation switches 
and servers at the bottom level. For a fattree with k number of pods, each pod consists 
of (k/2)^2 servers and 2 layers of k/2 k-port switches(edge switches and aggregation 
switches). Each edge switch connects to k/2 servers & k/2 aggregation switches. Each 
aggregation switch connects to k/2 edge and k/2 core switches. There are (k/2)^2 core 
switches and each core switch connects to k pods. In ns-3, all the links  are created 
and configured using the associated layer 2 helper object ([Ref1]_).

.. figure:: figures/fat-tree-animation.*
   :align: center
   :width: 500px
   :height: 400px

   An Example of FatTree Topology with k=4

Using the FatTree
----------------------------

The FatTreeHelper object can be instantiated by following statement.
  FatTreeHelper fattree (nPods);
  where,
  nLevels is number of pods (k)

Examples
========

The FatTree topology example could be found at ``src/netanim/examples/fat-tree-animation.cc``.

.. sourcecode:: bash

   $ ./waf configure --enable-examples
   $ NS_LOG="FatTreeAnimation" ./waf --run "fat-tree-animation"

References
**********

.. [Ref1] Al-Fares, Mohammad, Alexander Loukissas, and Amin Vahdat. "A scalable, commodity data center network architecture." ACM SIGCOMM Computer Communication Review. Vol. 38. No. 4. ACM, 2008; Available online at `<http://ccr.sigcomm.org/online/files/p63-alfares.pdf>`_.