/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#if 1
#define SYS_LOG_DOMAIN "wifi_st"
#define NET_SYS_LOG_LEVEL SYS_LOG_LEVEL_DEBUG
#define NET_LOG_ENABLED 1
#endif

#include <zephyr.h>
#include <errno.h>
#include <misc/printk.h>
#include <string.h>

#include <wifi.h>


#include <board.h>
#include <device.h>
#include <gpio.h>

struct device *dev;

typedef enum
{
  WS_IDLE = 0,
  WS_CONNECTED,
  WS_DISCONNECTED,
  WS_ERROR,
} WebServerState_t;

#define mySSID     "vince" 
#define PASSWORD "testvince"
#define PORT           80

#define WIFI_WRITE_TIMEOUT 10000
#define WIFI_READ_TIMEOUT  10000

static   uint8_t http[1024];
static   uint8_t resp[1024];
uint16_t respLen;
uint8_t  IP_Addr[4]; 
uint8_t  MAC_Addr[6]; 
int32_t Socket = -1;
static   WebServerState_t  State = WS_ERROR;
static ADC_HandleTypeDef    AdcHandle;
char     ModuleName[32];

/**
  * @brief  Send HTML page
  * @param  None
  * @retval None
  */
static WIFI_Status_t SendWebPage(uint8_t ledIsOn, uint8_t temperature)
{
  uint8_t  temp[50];
  uint16_t SentDataLength;
  WIFI_Status_t ret;
  
  /* construct web page content */
  strcpy((char *)http, (char *)"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n");
  strcat((char *)http, (char *)"<html>\r\n<body>\r\n");
  strcat((char *)http, (char *)"<title>STM32 Web Server</title>\r\n");
  strcat((char *)http, (char *)"<h2>InventekSys : Web Server using Es-Wifi with STM32</h2>\r\n");
  strcat((char *)http, (char *)"<br /><hr>\r\n");
  strcat((char *)http, (char *)"<p><form method=\"POST\"><strong>Temp: <input type=\"text\" size=2 value=\"");
  sprintf((char *)temp, "%d", temperature);
  strcat((char *)http, (char *)temp);
  strcat((char *)http, (char *)"\"> <sup>O</sup>C");
  
  if (ledIsOn) 
  {
    strcat((char *)http, (char *)"<p><input type=\"radio\" name=\"radio\" value=\"0\" >LED off");
    strcat((char *)http, (char *)"<br><input type=\"radio\" name=\"radio\" value=\"1\" checked>LED on");
  } 
  else 
  {
    strcat((char *)http, (char *)"<p><input type=\"radio\" name=\"radio\" value=\"0\" checked>LED off");
    strcat((char *)http, (char *)"<br><input type=\"radio\" name=\"radio\" value=\"1\" >LED on");
  }

  strcat((char *)http, (char *)"</strong><p><input type=\"submit\"></form></span>");
  strcat((char *)http, (char *)"</body>\r\n</html>\r\n");
  
  ret = WIFI_SendData(0, (uint8_t *)http, strlen((char *)http), &SentDataLength, WIFI_WRITE_TIMEOUT); 
  
  if((ret == WIFI_STATUS_OK) && (SentDataLength != strlen((char *)http)))
  {
    ret = WIFI_STATUS_ERROR;
  }
    
  return ret;
}

/**
  * @brief  Send HTML page
  * @param  None
  * @retval None
  */
static void WebServerProcess(void)
{
  uint8_t LedState = 0, temp;
  
  switch(State)
  {
  case WS_IDLE:
    Socket = 0;
    WIFI_StartServer(Socket, WIFI_TCP_PROTOCOL, "", PORT);
    
    if(Socket != -1)
    {
      printk("> HTTP Server Started \n");  
      State = WS_CONNECTED;
    }
    else
    {
      printk("> ERROR : Connection cannot be established.\n");      
      State = WS_ERROR;
    }    
    break;
    
  case WS_CONNECTED:
    
    WIFI_ReceiveData(Socket, resp, 1200, &respLen, WIFI_READ_TIMEOUT);
    
    if( respLen > 0)
    {
      if(strstr((char *)resp, "GET")) /* GET: put web page */
      {
        temp = 12;//TEMP_SENSOR_GetValue();
        if(SendWebPage(LedState, temp) != WIFI_STATUS_OK)
        {
          printk("> ERROR : Cannot send web page\n"); 
          State = WS_ERROR;
        }
      }
      else if(strstr((char *)resp, "POST"))/* POST: received info */
      {
          if(strstr((char *)resp, "radio"))
          {          
            if(strstr((char *)resp, "radio=0"))
            {
              LedState = 0;
              //BSP_LED_Off(LED2);
              printk("set led to off\n"); 
              gpio_pin_write(dev, LED0_GPIO_PIN, 0);
            }
            else if(strstr((char *)resp, "radio=1"))
            {
              LedState = 1;
              //BSP_LED_On(LED2);
              printk("set led to on\n"); 
              gpio_pin_write(dev, LED0_GPIO_PIN, 1);
            } 
            
           temp = 15;//TEMP_SENSOR_GetValue();
            if(SendWebPage(LedState, temp) != WIFI_STATUS_OK)
            {
              printk("> ERROR : Cannot send web page\n");
              State = WS_ERROR;
          }
        }
      }
    }
    if(WIFI_StopServer(Socket) == WIFI_STATUS_OK)
    {
      WIFI_StartServer(Socket, WIFI_TCP_PROTOCOL, "", PORT);
    }
    else
    {
      State = WS_ERROR;  
    }
    break;
  case WS_ERROR:   
  default:
    break;
  }
}


void main(void)
{
  printk("wifi_st test application...\n");

  dev = device_get_binding(LED0_GPIO_PORT);
  /* Set LED pin as output */
  gpio_pin_configure(dev, LED0_GPIO_PIN, GPIO_DIR_OUT);


  /*Initialize and use WIFI module */
  if(WIFI_Init() ==  WIFI_STATUS_OK)
  {
    printk("ES-WIFI Initialized.\n");

      if(WIFI_GetMAC_Address(MAC_Addr) == WIFI_STATUS_OK)
      {       
        printk("> es-wifi module MAC Address : %X:%X:%X:%X:%X:%X\n",     
               MAC_Addr[0],
               MAC_Addr[1],
               MAC_Addr[2],
               MAC_Addr[3],
               MAC_Addr[4],
               MAC_Addr[5]);   
      }
      else
      {
        printk("> ERROR : CANNOT get MAC address\n");
      }
    
   if( WIFI_Connect(mySSID, PASSWORD, WIFI_ECN_WPA2_PSK) == WIFI_STATUS_OK)
    {
      printk("> es-wifi module connected \n");
      
      if(WIFI_GetIP_Address(IP_Addr) == WIFI_STATUS_OK)
      {
      printk("> es-wifi module got IP Address : %d.%d.%d.%d\n",     
               IP_Addr[0],
               IP_Addr[1],
               IP_Addr[2],
               IP_Addr[3]); 
        
      printk(">Start HTTP Server... \n");
      printk(">Wait for connection...  \n");
        State = WS_IDLE;
        
      }
      else
      {    
    printk("> ERROR : es-wifi module CANNOT get IP address\n");
      }
    }
    else
    {
      
 printk("> ERROR : es-wifi module NOT connected\n");
    }
  }
  else
  {   
    printk("> ERROR : WIFI Module cannot be initialized.\n"); 
  }

  while(1)
  {
      WebServerProcess ();
  }

}


