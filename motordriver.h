#ifndef motordriver_h
#define motordriver_h

#ifdef __cplusplus
extern "C" {
#endif
	
	#include "stm32f4xx.h"
	
	#ifndef USE_TIMER_3
	#define USE_TIMER_3
	#else
	#error "Timer 3 have been used"
	#endif
	
	#define PWM_MAX 3000 	// = ARR; PWM 896 Hz
	
	extern void motor_init(void);
	extern void motor_left_pwm(int pwmValue);
	extern void motor_left_setDir(unsigned char dir);
//	extern unsigned int motor_left_getEncoder(void);
	extern void motor_right_pwm(int pwmValue);
	extern void motor_right_setDir(unsigned char dir);
//	extern unsigned int motor_right_getEncoder(void);
	

#ifdef __cplusplus
}
#endif

#endif
