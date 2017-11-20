.. _wifi_scan:

WIFI scan example
#################

Overview
********

To be used with Inventek wifi chip. It will scan wifi networks every 5 seconds.

The source code for this sample application can be found at:
:file:`samples/net/wifi_scan`.

Requirements
************

- board with Inventek wifi chip. (for example, disco_l475_iot1)


Building and Running
********************

.. code-block:: console

    $ cd samples/net/wifi_scan
    $ mkdir build && cd build
    $ cmake -DBOARD=disco_l475_iot1 ..
    $ make

Sample Output
=============

.. code-block:: console

    Access Points list : 
     0: XXX 
     1: YYY 
     2: ZZZ 
     3:  
     4:  
     5:  
     6:  
     7:  
     8: 
     9: 
    <repeats endlessly every 5 seconds>

