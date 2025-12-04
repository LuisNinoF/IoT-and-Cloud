#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>

void pa0_pa1_adc_init(void);
void start_conversion(void);
uint32_t adc_read(void);
void adc_read_multiple(uint32_t* values, int num_of_values);

#endif

