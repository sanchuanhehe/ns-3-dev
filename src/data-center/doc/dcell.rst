.. include:: replace.txt
.. highlight:: cpp

DCell topology
--------------

DCell topolgy is a scalable, recursively defined data center network architecture.
In DCell, a server is connected to several other servers and a mini-switch via bidirectional links. 
A high-level DCell is constructed from low-level DCells.
DCell_{0} is the building block to construct larger DCells. It has n servers and a mini-switch. 
All servers in DCell_{0} are connected to the mini-switch.
DCell_{1} is constructed using n + 1 DCell_{0}s. In DCell_{1}, each DCell_{0} is connected to all the other DCell_{0}s with one link.
In DCell_{1}, each DCell_{0}, if treated as a virtual node, is fully connected with every other virtual node to form a
complete graph.
For a level 2 or higher DCell_{k}, it is constructed in the same way to the above DCell_{1} construction.
Therefore, the number of level k-1 DCell in a DCell_{k}, (i.e., g_{k}), and the total number of servers in a DCell_{k} (i.e.,
t_{k}) follow the following equations:
g_{k} = t_{k-1} + 1;
t_{k} = g_{k} * t_{k-1}
for k > 0. DCell_{0} is a special case when g_{0} = 1 and t_{0} = n, with n being the number of servers in a DCell_{0} ([Ref1]_).

.. figure:: figures/dcell-animation.*
   :align: center
   :width: 500px
   :height: 400px

   An Example of DCell Topology with n=4, level k=1

Using the DCell
----------------------------

The DcellHelper object can be instantiated by following statement.
  DcellHelper dcell (numLevels, numServers);
  where,
  numLevels is the number of levels (k) and numServers is the number of servers in each DCell_{0}

Examples
========

The DCell topology example could be found at ``src/netanim/examples/dcell-animation.cc``.

.. sourcecode:: bash

   $ ./waf configure --enable-examples
   $ NS_LOG="DCellAnimation" ./waf --run "dcell-animation"

References
**********

.. [Ref1] Guo, Chuanxiong, et al. "Dcell: a scalable and fault-tolerant network structure for data centers." ACM SIGCOMM Computer Communication Review. Vol. 38. No. 4. ACM, 2008; Available online at `<http://www.sigcomm.org/sites/default/files/ccr/papers/2008/October/1402946-1402968.pdf>`_.