#ifndef main_h
#define main_h

#ifdef __cplusplus
extern "C" {
#endif

	#include "stm32f4xx.h"
	#include "stm32f4xx_conf.h"
	#include "motor.h"
	#include "motordriver.h"
	#include "usart.h"
	#include "delay.h"
	#include "Utilities/STM32F4-Discovery/stm32f4_discovery_lis302dl.h"
	#include "Utilities/STM32F4-Discovery/stm32f4_discovery.h"
	
	
	Motor motorL;
	Motor motorR;

#ifdef _cplusplus
}
#endif

#endif
