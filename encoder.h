#ifndef encoder_h
#define encoder_h
#ifdef __cplusplus
extern "C" {
#endif
	
	#include "stm32f4xx.h"
	#include <inttypes.h>
	
	#ifndef USE_TIM2
	#define USE_TIM2
	#else
	#error "TIM2 have been used:
	#endif
	
	#ifndef USE_TIM4
	#define USE_TIM4
	#else
	#error "TIM4 have been used:
	#endif
	
	
	void Encoders_init(void);
	int16_t ENCL_getValue(void);
	int16_t ENCR_getValue(void);
	void Encoders_reset (void);
	void encodersRead (void);
	
#ifdef __cplusplus
}
#endif

#endif
