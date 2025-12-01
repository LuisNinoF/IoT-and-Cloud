/*
 * *** Purpose:
 * Demonstrate bare metal WIFI implementation to transmit multiple sensor data to the cloud
 *
 * *** How it works:
 * STM32 reads sensors data from multiple ADC channels.
 * ESP8266 operates as WIFI station and connects to access point.
 * Sensor data is sent from the STM32 to ThingSpeak cloud
 * via TCP connection using a HTTP GET request.
 *
 * *** Note of use:
 * You must update your WIFI credentials and ThingSpreak API Key in code.
 *
 * * *** Copyrights:
 * Created by Luis Nino
 * Last revision 2025.12.01
 *
 */

/*Include drivers*/
#include <adc_multi.h>
#include <esp82xx_driver.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"

#include "bsp.h"
#include "fpu.h"
#include "timebase.h"
#include "circular_buffer.h"
#include "esp82xx_lib.h"

/*Define WIFI credentials*/
#define _SSID		"YOUR-WIFI"
#define _PASSWORD	"YOUR-PASSWORD"

/*Declare API KEY*/
char *API_KEY_SINGLE_CH = "YOUR-API-KEY-SINGLE-CHANNEL";
char *API_KEY_DOUBLE_CH = "YOUR-API-KEY-DOUBLE-CHANNEL";

/*Declare global variables*/
uint32_t sensor_value;
#define NUM_OF_SENSORS 2
uint32_t sensor_values[NUM_OF_SENSORS];

/*Declare function prototypes*/
void init_drivers(void);
void espxx_thingspeak_send_multiple_data(char *apikey, int num_of_fields, uint32_t value[]);

/*Run Main*/
int main(void)
{
	/*Initialize drivers*/
	init_drivers();

	/*Initialize ESP8266*/
	esp8266_device_init(_SSID, _PASSWORD);

	while(1)
	{
		/*Read multiple sensors data from multiple channels of ADC*/
		adc_read_multiple(sensor_values, NUM_OF_SENSORS);

		/*Send sensor data of multiple sensors to ThingSpeak*/
		espxx_thingspeak_send_multiple_data(API_KEY_DOUBLE_CH, NUM_OF_SENSORS, sensor_values);

		/*Delay*/
		systick_delay_ms(1);
	}
}

void init_drivers(void)
{
	/*Leds and buttons*/
	led_init();
	button_init();

	/*FPU*/
	fpu_enable();

	/*Systick*/
	timebase_init();

	/*UART*/
	debug_uart_init();
	esp_uart_init();

	/*ADC*/
	pa0_pa1_adc_init();
}

/*Function to send multiple data to ThingSpeak website with ip address 184.106.153.149 via TCP connection using a HTTP GET request*/
void espxx_thingspeak_send_multiple_data(char *apikey, int num_of_fields, uint32_t value[])
{
	 char local_buffer1[100] ={0};
	 char local_buffer2[100] ={0};
	 char local_buffer3[100] ={0};

	 /*Establish TCP connection with ThingSpeak*/
	 buffer_send_string("AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n",esp82xx_port);

	 /*Wait for "OK" response*/
	 while(! (is_response("OK\r\n"))){}

	 sprintf(local_buffer1,"GET /update?api_key=%s",apikey);

	 for(int i=0 ;i<num_of_fields;i++)
	 {
		sprintf(local_buffer3,"&field%d=%lu",i+1,value[i]);
		strcat(local_buffer1,local_buffer3);
	 }

	 strcat(local_buffer1,"\r\n");
	 int len = strlen(local_buffer1);

	 sprintf(local_buffer2,"AT+CIPSEND=%d\r\n",len);

	 /*Send buffer length*/
	 buffer_send_string(local_buffer2,esp82xx_port);

	 /*Wait for ">" response*/
	 while(! (is_response(">"))){}

	 /*Send parameters*/
	 buffer_send_string(local_buffer1,esp82xx_port);

	 /*Wait for "SEND OK" response*/
	 while(! (is_response("SEND OK\r\n"))){}

	 /*Wait for "CLOSED" response*/
	 while(! (is_response("CLOSED"))){}

	 /*Clear buffers*/
	 buffer_reset(local_buffer1);
	 buffer_reset(local_buffer2);
	 buffer_reset(local_buffer3);

	 /*Re-initialize circular buffer*/
	 circular_buffer_init();
}

