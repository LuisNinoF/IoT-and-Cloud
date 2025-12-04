/*
 * *** Purpose:
 * Demonstrate WIFI implementation to transmit multiple sensor data to a web server using an SQLite database for visualization
 *
 * *** How it works:
 * STM32 reads sensor data from multiple ADC channels.
 * ESP8266 operates as WIFI station and connects to access point.
 * STM32 sends the sensor data to a php endpoint over TCP using an HTTP POST request.
 * The php script stores the data in an SQLite database,
 * and a web page displays the latest readings in chart and table form.
 *
 * *** Note of use:
 * Update your WIFI credentials, server URL, and HTTP POST request file name.
 * Also ensure the web server has php and SQLite enabled and that all php files are copied to the server's document root.
 *
 * * *** Copyrights:
 * Created by Luis Nino
 * Last revision 2025.12.04
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

/*Declare Server address and HTTP POST request file
 *go to esp82xx_lib.h*/

/*Declare global variables*/
uint32_t sensor_value;
#define NUM_OF_SENSORS 2
uint32_t sensor_values[NUM_OF_SENSORS];

/*Declare function prototypes*/
void init_drivers(void);
void esp82xx_post_sensor_value(int *sensor_values);

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

		/*Send sensor data of multiple sensors to the server*/
		esp82xx_post_sensor_value((int *)sensor_values);

		/*Delay*/
		systick_delay_ms(100);
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

void esp82xx_post_sensor_value(int *sensor_values)
{
    /*1. Buffer to hold HTTP POST request and other strings*/
    char request_buffer[TEMP_BUFF_LNG_SZ] = {0};
    char send_command_buffer[TEMP_BUFF2_SHT_SZ] = {0};
    char post_data[TEMP_BUFF_LNG_SZ] = {0};

    /*2. Clear esp uart buffer*/
    buffer_clear(esp82xx_port);

	/*3. Establish a TCP connection to the server*/
    buffer_send_string(TCP_START_COMMAND, esp82xx_port);

    /*4. Wait until connection is successfully established*/
    while(!is_response(OK_RESPONSE)){}

    /*5. Prepare the POST data*/
    snprintf(post_data, sizeof(post_data), "key=4326&sensorvalue1=%d&sensorvalue2=%d", sensor_values[0], sensor_values[1]);

    /*6. Get the length of the data*/
    int data_length = strlen(post_data);

    /*7. Prepare the HTTP POST request*/
    snprintf(request_buffer, sizeof(request_buffer),
             "POST /insert.php HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Content-Type: application/x-www-form-urlencoded\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n\r\n"
             "%s", SERVER_ADDRESS, data_length, post_data);

    /*8. Get the length of the prepared request*/
    int request_length = strlen(request_buffer);

	/*9. Prepare the AT+CIPSEND command with the request length*/
    snprintf(send_command_buffer, sizeof(send_command_buffer), CIPSEND_COMMAND, request_length);

    /*10. Send the AT+CIPSEND command*/
    buffer_send_string(send_command_buffer, esp82xx_port);

    /*11. Wait for the ESP to be ready to receive the HTTP POST request*/
    while(!is_response(SEND_PROMPT)){}

    /*12. Send the actual HTTP POST request to the server*/
    buffer_send_string(request_buffer, esp82xx_port);

    /*13. Wait to confirm that the data was sent*/
    while(!is_response(SEND_OK_RESPONSE)){}

    /*14. Wait for the HTTP response header to finish*/
    while(!is_response(END_OF_HEADERS)){}
}

