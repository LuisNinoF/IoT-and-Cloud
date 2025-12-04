#include <adc_multi.h>
#include "stm32f4xx.h"
#include "timebase.h"

#define GPIOAEN		(1U<<0)
#define ADC1EN		(1U<<8)
#define ADC_CH0		(0U<<0)
#define ADC_CH1		(1U<<0)
#define ADC_CH4		(4U<<5)
#define AD_SEQ_LEN_1  0x00
#define AD_SEQ_LEN_2  (1U<<20)
#define CR2_ADCON	 (1U<<0)
#define CR2_CONT	 (1U<<1)
#define CR2_SWSTART	  (1U<<30)
#define SR_EOC		  (1U<<1)

void pa0_pa1_adc_init(void)
{
	/****Configure the ADC GPIO Pin**/
	/*Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;

	/*Set PA1 mode to analog mode*/
	GPIOA->MODER |= (1U<<2);
	GPIOA->MODER |= (1U<<3);

	/*Set PA4 mode to analog mode*/
	GPIOA->MODER |= (1U<<8);
	GPIOA->MODER |= (1U<<9);

	/****Configure the ADC Module**/
	/*Enable clock access to the ADC module*/
	RCC->APB2ENR |= ADC1EN;

	/*Set conversion sequence length*/
	//ADC1->SQR1 = AD_SEQ_LEN_1;
	ADC1->SQR1 = AD_SEQ_LEN_2;

	/*Enable scan mode*/
    ADC1->CR1 |= (1U << 8);

    /*Enable discontinuous mode on regular channels*/
    ADC1->CR1 |= (1U << 11);

    /*define the number of regular channels to be converted in discontinuous mode*/
    ADC1->CR1 |= (0U << 13);
    ADC1->CR1 |= (0U << 14);
    ADC1->CR1 |= (0U << 15);

	/*Set the End Of Conversion (EOC) bit after each conversion*/
	ADC1->CR2 |= (1U << 10);

	/*Set conversion sequence start*/
    ADC1->SQR3  = (1U<<0); //Channel 1
    ADC1->SQR3 |= (4U<<5); //Channel 4

    /*Sampling time for channels 1 and 4*/
    ADC1->SMPR2 |= (5U << (3*1));  // Channel 1
    ADC1->SMPR2 |= (5U << (3*4));  // Channel 4

	/*Enable ADC module*/
	ADC1->CR2 |= CR2_ADCON;
}


void start_conversion(void)
{
	/*Enable continuous conversion*/
	//ADC1->CR2 |=CR2_CONT;

	/*Start ADC conversion*/
	ADC1->CR2 |=CR2_SWSTART;
}


uint32_t adc_read(void)
{
	/*Wait for conversion to be complete*/
	while(!(ADC1->SR & SR_EOC)){}

	uint32_t val = ADC1->DR;

	ADC1->SR &=~ SR_EOC;

	/*Return value from data register*/
	return val;
}

void adc_read_multiple(uint32_t* values, int num_of_values)
{
	for(int i = 0; i < num_of_values; i++)
	{
	    ADC1->CR2 |= CR2_SWSTART;
		while(!(ADC1->SR & SR_EOC)){}
		values[i] = ADC1->DR;
	}
}
