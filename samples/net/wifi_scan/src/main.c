/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <zephyr.h>
#include <errno.h>
#include <misc/printk.h>
#include <string.h>

#include <wifi.h>

WIFI_APs_t APlist;

void main(void)
{
  int max_wifiNb = 10;
  printk("List available wifi networks\n");

  /*Initialize and use WIFI module */
  if(WIFI_Init() ==  WIFI_STATUS_OK)
  {
    printk("ES-WIFI Initialized.\n");

    while(WIFI_ListAccessPoints(&APlist, max_wifiNb) ==  WIFI_STATUS_OK)
      {
        int i=0;
        printk("Access Points list : \n");
        for(i=0;i<max_wifiNb;i++){
          printk(" %d: %s \n", i, APlist.ap[i].SSID);
        }
        printk("\n");
      }
  }
}



