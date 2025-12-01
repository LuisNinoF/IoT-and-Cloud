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

#endif
