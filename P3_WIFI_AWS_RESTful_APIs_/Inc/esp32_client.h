#ifndef __ESP32_CLIENT_H_
#define __ESP32_CLIENT_H_

#include "esp32_lib.h"

void esp32_client_get_file(char *dest_buffer, const char *storage_url);

void esp32_client_post(char *dest_buffer, const char *endpoint, const char *json_payload);
void esp32_client_get(char *dest_buffer, const char *endpoint);
void esp32_client_put(char *dest_buffer, const char *endpoint, const char *json_payload);
void esp32_client_delete(char *dest_buffer, const char *endpoint);

int parse_sensor_data(const char* json_str, int* sensor_array);

#endif
