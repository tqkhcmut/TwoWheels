#include "delay.h"


volatile uint32_t delayCounter = 0;
volatile uint32_t millisCounter = 0;

void delay_init(void)
{
  RCC_ClocksTypeDef RCC_Clocks;
	/* SysTick end of count event each 1ms */
  RCC_GetClocksFreq(&RCC_Clocks);
	if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000))
  { 
    /* Capture error */ 
    while (1);
  }
}

void _delay_ms(uint32_t time) // delay for miliseconds
{
	delayCounter = time;
	while(delayCounter);
}

void _delay_us(uint32_t time) // delay for microseconds
{
	uint32_t i = time * (SystemCoreClock / 10000000); // ~ us
	while(i--);
}

uint32_t millis(void)
{
	return millisCounter;
}

void setMillis(uint32_t time)
{
	millisCounter = time;
}

void delay_routine(void)
{
	if(delayCounter)
		delayCounter--;
	millisCounter++;
}
