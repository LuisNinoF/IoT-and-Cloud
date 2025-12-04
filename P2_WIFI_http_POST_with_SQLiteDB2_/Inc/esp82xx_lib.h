#ifndef __ESP82XX_LIB_H
#define __ESP82XX_LIB_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "esp82xx_driver.h"
#include "circular_buffer.h"
#include "timebase.h"

void esp8266_device_init(char *ssid, char *password);
void buffer_reset(char *buffer);

#define esp82xx_port		SLAVE_DEV_PORT
#define debug_port			DEBUG_PORT

#define TEMP_BUFF_LNG_SZ		600
#define TEMP_BUFF2_SHT_SZ		30

/*Declare Server address and HTTP POST request file*/
#define HTTP_POST_REQUEST "/insert.php"
#define SERVER_ADDRESS "wifi-iot-architecture-ln.batcave.net"

/*Macros for commonly used strings in the esp82xx library*/
#define TCP_START_COMMAND "AT+CIPSTART=\"TCP\",\"" SERVER_ADDRESS "\",80\r\n"
#define OK_RESPONSE "OK\r\n"
#define SEND_PROMPT ">"
#define SEND_OK_RESPONSE "SEND OK\r\n"
#define END_OF_HEADERS "\r\n\r\n"
#define LINE_TERMINATOR "\r\n"
#define CLOSED_RESPONSE "CLOSED\r\n"
#define CIPSEND_COMMAND "AT+CIPSEND=%d\r\n"

#endif
