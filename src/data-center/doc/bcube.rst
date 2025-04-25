.. include:: replace.txt
.. highlight:: cpp

BCube topology
--------------

BCube is a server-centric network topology designed to meet the requirements of 
Modular Data Centers. It consists of servers with multiple network ports connected 
to multiple layers of COTS (commodity off-the-shelf) mini-switches. Servers act as 
not only end hosts, but also relay nodes for each other. BCube supports various 
bandwidth-intensive applications by speeding-up one-to-one, one-to-several, and 
one-to-all traffic patterns, and by providing high network capacity for all-to-all 
traffic. BCube exhibits graceful performance degradation as the server and/or switch 
failure rate increases. This property is of special importance for shipping-container 
data centers, since once the container is sealed and operational, it becomes very 
difficult to repair or replace its components ([Ref1]_).

Construction of BCube
---------------------

BCube is a recursively defined structure. There are two types of devices in BCube: 
Servers with multiple ports, and switches that connect a constant number of servers.  
A BCube_{0} is simply n servers connecting to an n-port switch. A BCube1 is constructed 
from n BCube_{0}s and n n-port switches. More generically, a BCube_{k} (k ≥ 1)) is constructed 
from n BCube_{k−1}s and n^k n-port switches. Each server in a BCube_{k} has k + 1 ports, which 
are numbered from level-0 to level-k. It is easy to see that a BCube_{k} has n^(k+1) 
servers and k + 1 level of switches, with each level having n^k n-port switches.

The construction of a BCube_{k} is as follows. We number the n BCube_{k-1}s from 0 to n − 1 
and the servers in each BCube_{k-1} from 0 to n^k − 1. We then connect the level-k port 
of the i-th server (i ∈ [0, n^k − 1]) in the j-th BCube_{k-1} (j ∈ [0, n − 1]) to the j-th 
port of the i-th level-k switch. The links in BCube are bidirectional. In ns-3, all the
links are created and configured using the associated layer 2 helper object.

The BCube construction guarantees that switches only connect to servers and never 
directly connect to other switches. As a direct consequence, we can treat the switches 
as dummy crossbars that connect several neighboring servers and let servers relay traffic 
for each other ([Ref1]_). 

.. figure:: figures/bcube-animation.*
   :align: center
   :width: 500px
   :height: 400px

   An Example of BCube Topology with n=4, level k=1

Using the BCube
----------------------------

The BCubeHelper object can be instantiated by following statement.
  BCubeHelper bcube (nLevels, nServers);
  where,
  nLevels is number of levels (k)
  nServers is number of servers (n)

Examples
========

The BCube topology example could be found at ``src/netanim/examples/bcube-animation.cc``.

.. sourcecode:: bash

   $ ./waf configure --enable-examples
   $ NS_LOG="BCubeAnimation" ./waf --run "bcube-animation"

References
**********

.. [Ref1] Guo, Chuanxiong, et al. "BCube: a high performance, server-centric network architecture for modular data centers." ACM SIGCOMM Computer Communication Review 39.4 (2009): 63-74; Available online at `<https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/comm136-guo.pdf>`_.
