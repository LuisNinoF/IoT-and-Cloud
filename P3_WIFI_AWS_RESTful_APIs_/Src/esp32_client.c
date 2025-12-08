#include "esp32_client.h"
#include <stdlib.h>
#include <math.h>

#define TEMP_BUFF_LNG_SZ		600
#define TEMP_BUFF2_SHT_SZ		30

/*Server address*/
#define SERVER_ADDRESS "YOUR-SERVER"

/*Macros for commonly used strings in the function*/
#define SSL_START_COMMAND "AT+CIPSTART=\"SSL\",\"%s\",443\r\n"
#define OK_RESPONSE "OK\r\n"
#define SEND_PROMPT ">"
#define SEND_OK_RESPONSE "SEND OK\r\n"
#define END_OF_HEADERS "\r\n\r\n"
#define LINE_TERMINATOR "\r\n"
#define CLOSED_RESPONSE "CLOSED\r\n"
#define CIPSEND_COMMAND "AT+CIPSEND=%d\r\n"
#define CIPCLOSE		"AT+CIPCLOSE\r\n"

#define HTTPS_GET_REQUEST 		"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n"
#define HTTPS_POST_REQUEST 		"POST %s HTTP/1.1\r\nHost: %s\r\nContent-Type: application/json\r\nContent-Length: %d\r\nConnection: close\r\n\r\n%s"
#define HTTPS_PUT_REQUEST 		"PUT %s HTTP/1.1\r\nHost: %s\r\nContent-Type: application/json\r\nContent-Length: %d\r\nConnection: close\r\n\r\n%s"
#define HTTPS_DELETE_REQUEST 	"DELETE %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n"

/*parse humidity and temperature data*/
int parse_sensor_data(const char* json_str, int* sensor_array)
{
	char temp_buf[32];
	char hum_buf[32];

	/*extract humidity data*/
	char* hum_ptr = strstr(json_str, "\"humidity\":");		//searches until humidity is found
	if(hum_ptr)
	{
		hum_ptr += 11;										//skips humidity + space
		sscanf(hum_ptr, "%31[^,}]", hum_buf);				//reads until , or }
		sensor_array[1] = (int)round(atof(hum_buf));		//type cast to int and store
	}
	else
	{
		return -1;
	}

	/*extract temperature data*/
	char* temp_ptr = strstr(json_str, "\"temperature\":");	//searches until temperature is found
	if(temp_ptr)
	{
		temp_ptr += 14;										//skips temperature + space
		sscanf(temp_ptr, "%31[^,}]", temp_buf);				//reads until , or }
		sensor_array[0] = (int)round(atof(temp_buf));		//type cast to int and store
	}
	else
	{
		return -1;
	}

	return 0;
}

/*delete request function*/
void esp32_client_delete(char *dest_buffer, const char *endpoint)
{
	/*Buffer to hold HTTPS request and other string*/
	char request_buffer[TEMP_BUFF_LNG_SZ] = {0};
	char send_command_buffer[TEMP_BUFF2_SHT_SZ] = {0};

	/*Clear esp uart buffer*/
	buffer_clear(esp32_port);

	/*Establish a SSL connection to the server on port 443, which is a secure connection*/
	snprintf(request_buffer, sizeof(request_buffer),SSL_START_COMMAND,SERVER_ADDRESS);
	buffer_send_string(request_buffer,esp32_port);

	/*Wait until connection is successfully established*/
	while(!is_response(OK_RESPONSE)){}

	/*Prepare the HTTPS DELETE request to retrieve the file*/
	snprintf(request_buffer, sizeof(request_buffer),HTTPS_DELETE_REQUEST, endpoint, SERVER_ADDRESS);

	/*Get the length of the prepared request*/
	int request_length = strlen(request_buffer);

	/*Prepare the AT+CIPSEND command with the request length */
	snprintf(send_command_buffer,sizeof(send_command_buffer),CIPSEND_COMMAND,request_length);

	/*Send the AT+CIPSEND command */
	buffer_send_string(send_command_buffer,esp32_port);

	/*Wait for the ESP to be ready to receive the HTTP request ">"*/
	while(!is_response(SEND_PROMPT)){}

	/*Send the actual HTTP GET request to the server*/
	buffer_send_string(request_buffer,esp32_port);

	/*Wait to confirm that the data was sent*/
	while(!is_response(SEND_OK_RESPONSE)){}

	/*Wait for the HTTP response header to finish*/
	while(!is_response(END_OF_HEADERS)){}

	/*Copy the data to destination buffer*/
	while(!copy_up_to_string(CLOSED_RESPONSE,dest_buffer)){}

	/*Close SSL connection*/
	buffer_send_string(CIPCLOSE, esp32_port);

}

/*put request function*/
void esp32_client_put(char *dest_buffer, const char *endpoint, const char *json_payload)
{
	/*Buffer to hold HTTPS request and other string*/
	char request_buffer[TEMP_BUFF_LNG_SZ] = {0};
	char send_command_buffer[TEMP_BUFF2_SHT_SZ] = {0};

	/*Clear esp uart buffer*/
	buffer_clear(esp32_port);

	/*Establish a SSL connection to the server on port 443, which is a secure connection*/
	snprintf(request_buffer, sizeof(request_buffer),SSL_START_COMMAND,SERVER_ADDRESS);
	buffer_send_string(request_buffer,esp32_port);

	/*Wait until connection is successfully established*/
	while(!is_response(OK_RESPONSE)){}

	/*Prepare the HTTPS PUT request*/
	snprintf(request_buffer, sizeof(request_buffer),HTTPS_PUT_REQUEST, endpoint, SERVER_ADDRESS, strlen(json_payload), json_payload);

	/*Get the length of the prepared request*/
	int request_length = strlen(request_buffer);

	/*Prepare the AT+CIPSEND command with the request length */
	snprintf(send_command_buffer,sizeof(send_command_buffer),CIPSEND_COMMAND,request_length);

	/*Send the AT+CIPSEND command */
	buffer_send_string(send_command_buffer,esp32_port);

	/*Wait for the ESP to be ready to receive the HTTP request ">"*/
	while(!is_response(SEND_PROMPT)){}

	/*Send the actual HTTP request to the server*/
	buffer_send_string(request_buffer,esp32_port);

	/*Wait to confirm that the data was sent*/
	while(!is_response(SEND_OK_RESPONSE)){}

	/*Wait for the HTTP response header to finish*/
	while(!is_response(END_OF_HEADERS)){}

	/*Copy the data to destination buffer*/
	while(!copy_up_to_string(CLOSED_RESPONSE,dest_buffer)){}

	/*Close SSL connection*/
	buffer_send_string(CIPCLOSE, esp32_port);
}

/*get request function*/
void esp32_client_get(char *dest_buffer, const char *endpoint)
{
	/*Buffer to hold HTTPS request and other string*/
	char request_buffer[TEMP_BUFF_LNG_SZ] = {0};
	char send_command_buffer[TEMP_BUFF2_SHT_SZ] = {0};

	/*Clear esp uart buffer*/
	buffer_clear(esp32_port);

	/*Establish a SSL connection to the server on port 443, which is a secure connection*/
	snprintf(request_buffer, sizeof(request_buffer),SSL_START_COMMAND,SERVER_ADDRESS);
	buffer_send_string(request_buffer,esp32_port);

	/*Wait until connection is successfully established*/
	while(!is_response(OK_RESPONSE)){}

	/*Prepare the HTTPS GET request to retrieve the file*/
	snprintf(request_buffer, sizeof(request_buffer),HTTPS_GET_REQUEST, endpoint, SERVER_ADDRESS);

	/*Get the length of the prepared request*/
	int request_length = strlen(request_buffer);

	/*Prepare the AT+CIPSEND command with the request length */
	snprintf(send_command_buffer,sizeof(send_command_buffer),CIPSEND_COMMAND,request_length);

	/*Send the AT+CIPSEND command */
	buffer_send_string(send_command_buffer,esp32_port);

	/*Wait for the ESP to be ready to receive the HTTP request ">"*/
	while(!is_response(SEND_PROMPT)){}

	/*Send the actual HTTP GET request to the server*/
	buffer_send_string(request_buffer,esp32_port);

	/*Wait to confirm that the data was sent*/
	while(!is_response(SEND_OK_RESPONSE)){}

	/*Wait for the HTTP response header to finish*/
	while(!is_response(END_OF_HEADERS)){}

	/*Copy the data to destination buffer*/
	while(!copy_up_to_string(CLOSED_RESPONSE,dest_buffer)){}

	/*Close SSL connection*/
	buffer_send_string(CIPCLOSE, esp32_port);
}

/*post request function*/
void esp32_client_post(char *dest_buffer, const char *endpoint, const char *json_payload)
{
	/*Buffer to hold HTTPS request and other string*/
	char request_buffer[TEMP_BUFF_LNG_SZ] = {0};
	char send_command_buffer[TEMP_BUFF2_SHT_SZ] = {0};

	/*Clear esp uart buffer*/
	buffer_clear(esp32_port);

	/*Establish a SSL connection to the server on port 443, which is a secure connection*/
	snprintf(request_buffer, sizeof(request_buffer),SSL_START_COMMAND,SERVER_ADDRESS);
	buffer_send_string(request_buffer,esp32_port);

	/*Wait until connection is successfully established*/
	while(!is_response(OK_RESPONSE)){}

	/*Prepare the HTTPS POST request*/
	snprintf(request_buffer, sizeof(request_buffer),HTTPS_POST_REQUEST, endpoint, SERVER_ADDRESS, strlen(json_payload), json_payload);

	/*Get the length of the prepared request*/
	int request_length = strlen(request_buffer);

	/*Prepare the AT+CIPSEND command with the request length */
	snprintf(send_command_buffer,sizeof(send_command_buffer),CIPSEND_COMMAND,request_length);

	/*Send the AT+CIPSEND command */
	buffer_send_string(send_command_buffer,esp32_port);

	/*Wait for the ESP to be ready to receive the HTTP request ">"*/
	while(!is_response(SEND_PROMPT)){}

	/*Send the actual HTTP request to the server*/
	buffer_send_string(request_buffer,esp32_port);

	/*Wait to confirm that the data was sent*/
	while(!is_response(SEND_OK_RESPONSE)){}

	/*Wait for the HTTP response header to finish*/
	while(!is_response(END_OF_HEADERS)){}

	/*Copy the data to destination buffer*/
	while(!copy_up_to_string(CLOSED_RESPONSE,dest_buffer)){}

	/*Close SSL connection*/
	buffer_send_string(CIPCLOSE, esp32_port);
}

/*get file from server*/
void esp32_client_get_file(char *dest_buffer, const char *storage_url)
{
	/*Buffer to hold HTTPS GET request and other string*/
	char request_buffer[TEMP_BUFF_LNG_SZ] = {0};
	char send_command_buffer[TEMP_BUFF2_SHT_SZ] = {0};

	/*Clear esp uart buffer*/
	buffer_clear(esp32_port);

	/*Establish a SSL connection to the server on port 443, which is a secure connection*/
	snprintf(request_buffer, sizeof(request_buffer),SSL_START_COMMAND,SERVER_ADDRESS);
	buffer_send_string(request_buffer,esp32_port);

	/*Wait until connection is successfully established*/
	while(!is_response(OK_RESPONSE)){}

	/*Prepare the HTTPS GET request to retrieve the file*/
	snprintf(request_buffer, sizeof(request_buffer),HTTPS_GET_REQUEST, storage_url, SERVER_ADDRESS);

	/*Get the length of the prepared request*/
	int request_length = strlen(request_buffer);

	/*Prepare the AT+CIPSEND command with the request length */
	snprintf(send_command_buffer,sizeof(send_command_buffer),CIPSEND_COMMAND,request_length);

	/*Send the AT+CIPSEND command */
	buffer_send_string(send_command_buffer,esp32_port);

	/*Wait for the ESP to be ready to receive the HTTP GET request ">"*/
	while(!is_response(SEND_PROMPT)){}

	/*Send the actual HTTP GET request to the server*/
	buffer_send_string(request_buffer,esp32_port);

	/*Wait to confirm that the data was sent*/
	while(!is_response(SEND_OK_RESPONSE)){}

	/*Wait for the HTTP response header to finish*/
	while(!is_response(END_OF_HEADERS)){}

	/*Copy the data to destination buffer*/
	while(!copy_up_to_string(CLOSED_RESPONSE,dest_buffer)){}

	/*Close SSL connection*/
	buffer_send_string(CIPCLOSE, esp32_port);
}
