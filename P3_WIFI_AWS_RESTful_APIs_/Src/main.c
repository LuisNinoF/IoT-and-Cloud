/*
 * *** Purpose:
 * Demonstrate WIFI implementation to transmit sensor data to AWS using DynamoDB, Lambda and API Gateway services.
 *
 * *** How it works:
 * STM32 reads sensor data from multiple ADC channels.
 * ESP32 operates as WIFI station and connects to access point.
 * STM32 sends the sensor data to AWS using an HTTPS (SSL/TLS over TCP) POST request.
 * AWS API Gateway triggers Lambda functions to POST, GET, PUT or DELETE data in a NoSQL DynamoDB database.
 * Each temperature and humidity data set is sent via POST and verified via GET requests.
 *
 * *** Note of use:
 * Update WIFI credentials and ensure the server address and AWS endpoints correspond to your application.
 *
 * * *** Copyrights:
 * Created by Luis Nino
 * Last revision 2025.12.08
 *
 */

// Include drivers
#include <stdint.h>
#include "stm32f4xx.h"

#include "bsp.h"
#include "fpu.h"
#include "timebase.h"
#include "uart.h"
#include "adc_multi.h"
#include "circular_buffer.h"
#include "esp32_lib.h"
#include "esp32_client.h"

/*Define WIFI credentials*/
#define _SSID		"YOUR-WIFI"
#define _PASSWORD	"YOUR-PASSWORD"

/*Define SERVER address*/
/*refer to "esp32_client.c"*/

/*Declare endpoints*/
#define POST_ENDPOINT	"/development/data"
#define GET_ENDPOINT	"/development/data/%s/latest"
#define PUT_ENDPOINT	"/development/data"
#define DELETE_ENDPOINT	"/development/data/%s/delete?timestamp=%d"

/*Declare debug message to compare post and get values*/
#define DGB_MESSAGE		"Temperature -> post: %d, get: %d. Humidity -> post: %d, get: %d\r\n"

/*Declare global variables*/
#define RESPONSE_SIZE 1000
char response[RESPONSE_SIZE];
char payload[128] = {0};
char debug_message[128];

uint32_t sensors_reading[2];
int temperature_value;
int humidity_value;

int timestamp;
int new_temperature_value;
int new_humidity_value;
int delete_timestamp;

int check_sensor_values[2];

/*Declare function prototypes*/
void init_drivers(void);
void post_request(char *dest_buffer, const char *endpoint, const char *json_payload, int temperature_value, int humidity_value, char *device_name);
void get_request(char *dest_buffer, const char *endpoint, char *device_name);
void put_request(char *dest_buffer, const char *endpoint, const char *json_payload, int timestamp, int new_temperature_value, int new_humidity_value, char *device_name);
void delete_request(char *dest_buffer, const char *endpoint, char *device_name, int delete_timestamp);
void parse_and_verify(const char* response, int* check_sensor_values);

/* Run Main*/
int main(void)
{
	/*Initialize generic drivers*/
	init_drivers();

	/*Initialize esp32: set up and connect to Wifi network*/
	esp32_init(_SSID, _PASSWORD);

	while(1)
	{
		/*Read sensor values from ADC*/
		adc_read_multiple(sensors_reading, 2);
		temperature_value = sensors_reading[0]*100/4095;
		humidity_value = sensors_reading[1]*100/4095;

		/*Post request*/
		post_request(response, POST_ENDPOINT, payload, temperature_value, humidity_value, "STM32_005");

		/*Get request*/
		get_request(response, GET_ENDPOINT, "STM32_005");

		/*Parse sensor data from get request response and compare to posted values to verify that data was sent successfully*/
		parse_and_verify(response, check_sensor_values);

		/*Delay of 2 seconds*/
		systick_delay_ms(2000);
	}
}

/*Function to initialize generic drivers*/
void init_drivers(void)
{
	led_init();
	button_init();
	fpu_enable();
	timebase_init();
	debug_uart_init();
	esp_uart_init();
	pa0_pa1_adc_init();
	start_conversion();
}

void post_request(char *dest_buffer, const char *endpoint, const char *json_payload, int temperature_value, int humidity_value, char *device_name)
{
	memset(dest_buffer, 0, RESPONSE_SIZE);  // Clear before use
	snprintf(payload, sizeof(payload),"{\"device_id\": \"%s\", \"temperature\": %d, \"humidity\": %d}", device_name, temperature_value, humidity_value);
	esp32_client_post(dest_buffer, endpoint, payload);
	buffer_send_string(dest_buffer, debug_port);
	systick_delay_ms(500);
}

void get_request(char *dest_buffer, const char *endpoint, char *device_name)
{
	memset(dest_buffer, 0, RESPONSE_SIZE);  // Clear before use
	char full_get_endpoint[128];
	snprintf(full_get_endpoint, sizeof(full_get_endpoint), endpoint, device_name);
	esp32_client_get(dest_buffer, full_get_endpoint);
	buffer_send_string(dest_buffer, debug_port);
	systick_delay_ms(500);
}

void put_request(char *dest_buffer, const char *endpoint, const char *json_payload, int timestamp, int new_temperature_value, int new_humidity_value, char *device_name)
{
	memset(dest_buffer, 0, RESPONSE_SIZE);  // Clear before use
	snprintf(payload, sizeof(payload),"{\"device_id\": \"%s\", \"timestamp\": %d, \"temperature\": %d, \"humidity\": %d}", device_name, timestamp, new_temperature_value, new_humidity_value);
	esp32_client_put(dest_buffer, endpoint, payload);
	buffer_send_string(dest_buffer, debug_port);
	systick_delay_ms(500);
}

void delete_request(char *dest_buffer, const char *endpoint, char *device_name, int delete_timestamp)
{
	memset(dest_buffer, 0, RESPONSE_SIZE);  // Clear before use
	char full_get_endpoint[128];
	snprintf(full_get_endpoint, sizeof(full_get_endpoint), endpoint, device_name, delete_timestamp);
	esp32_client_delete(dest_buffer, full_get_endpoint);
	buffer_send_string(dest_buffer, debug_port);
	systick_delay_ms(500);
}

void parse_and_verify(const char* response, int* check_sensor_values)
{
	if(parse_sensor_data(response, check_sensor_values) == 0)
	{
		snprintf(debug_message, sizeof(debug_message), DGB_MESSAGE, temperature_value, check_sensor_values[0], humidity_value, check_sensor_values[1]);
		buffer_send_string(debug_message, debug_port);

		if(temperature_value == check_sensor_values[0] && humidity_value == check_sensor_values[1])
		{
			buffer_send_string("Post request verified via get request \r\n", debug_port);
		}
		else
		{
			buffer_send_string("Post request NOT verified via get request \r\n", debug_port);
		}
	}
	else
	{
		buffer_send_string("Parse failed\r\n", debug_port);
	}
}

