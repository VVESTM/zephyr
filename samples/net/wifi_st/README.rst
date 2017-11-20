.. _wifi_st:

WIFI ST example
#################

Overview
********

To be used with Inventek wifi chip. 
This demo is coming from STMicroelectronics Cube examples, WiFi_HTTP_Server demo.
It launch a wifi server on the board, then, on the same network, open a web page 
with server address and follow instructions. You will be able to blink the led.

The source code for this sample application can be found at:
:file:`samples/net/wifi_st`.

Requirements
************

- board with Inventek wifi chip. (for example, disco_l475_iot1)


Building and Running
********************

.. code-block:: console

    $ cd samples/net/wifi_st
    $ mkdir build && cd build
    $ cmake -DBOARD=disco_l475_iot1 ..
    $ make


