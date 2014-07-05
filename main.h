#ifndef main_h
#define main_h

#include <math.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "motor.h"
#include "motordriver.h"
#include "encoder.h"
#include "PID_v1.h"
#include "Kalman.h"
#include "abfilter.h"
#include "delay.h"
#include "Utilities/STM32F4-Discovery/stm32f4_discovery_lis302dl.h"
#include "Utilities/STM32F4-Discovery/stm32f4_discovery.h"


/* Exported macro ------------------------------------------------------------*/
#define ABS(x)         ((x < 0) ? (-x) : x)
#define MAX(a,b)       ((a < b) ? (b) : a)

extern __IO uint8_t g_pidEnable;

extern Motor motorL;
extern Motor motorR;

extern PID Bal_pid;

extern void TimeOutProcess(void);
#endif
