#include "esp82xx_lib.h"

/*Macros for commonly used strings in the function*/
#define TCP_START_COMMAND "AT+CIPSTART=\"TCP\",\"" SERVER_ADDRESS "\",80\r\n"
#define OK_RESPONSE "OK\r\n"
#define SEND_PROMPT ">"
#define SEND_OK_RESPONSE "SEND OK\r\n"
#define END_OF_HEADERS "\r\n\r\n"
#define LINE_TERMINATOR "\r\n"
#define CLOSED_RESPONSE "CLOSED\r\n"
#define CIPSEND_COMMAND "AT+CIPSEND=%d\r\n"

/*Declare function prototypes*/
static void esp82xx_reset(void);
static void esp82xx_startup_test(void);
static void esp82xx_sta_mode(void);
static void esp82xx_ap_connect(char *ssid, char *password);
static void esp82xx_single_conn_en(void);

/*Initialize esp8266: reset, startup, sta mode (Wifi client), and ap connect (connect to Wifi)*/
void esp8266_device_init(char *ssid, char *password)
{
	/*Initialize circulat buffer*/
	circular_buffer_init();

	/*Initialize Reset (RS) pin */
	esp_rs_pin_init();
	esp_rs_pin_enable();

	/*Reset ESP82XX*/
	esp82xx_reset();

	/*Start up ESP82XX: Test that esp82 is responding*/
	esp82xx_startup_test();

	/*Set the ESP82XX as WIFI Client. Set station (STA) mode*/
	esp82xx_sta_mode();

	/*Connect to WIFI access point (AP)*/
	esp82xx_ap_connect(ssid, password);

	/*Enable single connection*/
	esp82xx_single_conn_en();
}

/*Start up ESP82XX. Test that esp82 is responding*/
 static void esp82xx_startup_test(void)
{
	/*Clear ESP uart buffer*/
	buffer_clear(esp82xx_port);

	/*Send test command*/
	buffer_send_string("AT\r\n",esp82xx_port);

	/*Wait for "OK" response*/
	while(!(is_response("OK\r\n"))){}

	buffer_send_string("AT startup test successful....\n\r",debug_port);
}

/*Reset ESP82XX*/
static void esp82xx_reset(void)
{
	/*Clear ESP uart buffer*/
	buffer_clear(esp82xx_port);

	/*Send test command*/
	buffer_send_string("AT+RST\r\n",esp82xx_port);

	/*Delay for 1 second*/
	systick_delay_ms(1000);

	/*Wait for "OK" response*/
	while(!(is_response("OK\r\n"))){}

	buffer_send_string("Reset was successful....\n\r",debug_port);
}

/*Set the ESP82XX as WIFI Client. Set station (STA) mode*/
static void esp82xx_sta_mode(void)
{
	/*Clear ESP uart buffer*/
	buffer_clear(esp82xx_port);

	/*Send test command*/
	buffer_send_string("AT+CWMODE=1\r\n",esp82xx_port);

	/*Wait for "OK" response*/
	while(!(is_response("OK\r\n"))){}

	buffer_send_string("STA Mode set successful....\n\r",debug_port);
}

/*Connect to WIFI access point (AP)*/
static void esp82xx_ap_connect(char *ssid, char *password)
{
	char data[80];

	/*Clear ESP uart buffer*/
	buffer_clear(esp82xx_port);

	buffer_send_string("Connecting to access point....\n\r",debug_port);

	/*Put ssid, password and command into one string packet*/
	sprintf(data,"AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,password);

	/*Send test command*/
	buffer_send_string(data,esp82xx_port);

	/*Wait for "OK" response*/
	while(!(is_response("OK\r\n"))){}

    sprintf(data,"Connected : \"%s\"\r\n",ssid);

	buffer_send_string(data,debug_port);
}

 /*Enable single connection mode*/
 static void esp82xx_single_conn_en(void)
 {
	/*Clear buffer*/
	buffer_clear(esp82xx_port);

	/*Send enable multiple connection command. =0 for single, = 1 for multiple connection*/
	buffer_send_string("AT+CIPMUX=0\r\n",esp82xx_port);

	/*Wait for "OK" response*/
	while(! (is_response("OK\r\n"))){}

	buffer_send_string("Single connection enabled....\n\r",debug_port);
 }

 /*Helper function to reset buffers in espxx_thingspeak_send function*/
 void buffer_reset(char *buffer)
 {
 	int len = strlen(buffer);

 	for(int i=0 ; i<len ; i++)
 	{
 		buffer[i] = '\0';
 	}
 }
