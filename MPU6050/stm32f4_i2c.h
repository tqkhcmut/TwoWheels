#ifndef stm32f4_i2c_h
#define stm32f4_i2c_h

#ifdef __cplusplus
extern "C" {
#endif
	
	#include <inttypes.h>
	#include "stm32f4xx.h"
	#include "../delay.h"
	
	#ifndef USE_I2C2
	#define USE_I2C2
	#else
	#error "I2C2 have been used."
	#endif


	#define I2C_SCL_PIN GPIO_Pin_10
	#define I2C_SCL_PIN_SRC	GPIO_PinSource10
	#define I2C_SCL_PIN_AF		GPIO_AF_I2C2
	#define I2C_SCL_GPIO_BASE GPIOB
	#define I2C_SCL_RCCPERIPH RCC_AHB1Periph_GPIOB
	
	#define I2C_SDA_PIN GPIO_Pin_11
	#define I2C_SDA_PIN_SRC	GPIO_PinSource11
	#define I2C_SDA_PIN_AF		GPIO_AF_I2C2
	#define I2C_SDA_GPIO_BASE GPIOB
	#define I2C_SDA_RCCPERIPH RCC_AHB1Periph_GPIOB

	#define SENSOR_I2C	I2C2
	#define SENSOR_I2C_SPEED 100000 // 100 kHz
	
	#ifdef I2C_SENSORS
	typedef enum { SENSORS_FAIL = 0, SENSORS_OK, I2C_TIMEOUT } I2C_RESULT;
	#else
	typedef enum { I2C_FAIL = 0, I2C_OK, I2C_TIMEOUT } I2C_RESULT;
	#endif
	
	void STM32F4_I2C_Init(void);
	I2C_RESULT Sensor_GetStatus(uint8_t devAddr, uint32_t timeout);
	I2C_RESULT STM32F4_I2C_readBytes(uint8_t devAddr, uint8_t regAddr, uint16_t count, uint8_t * buf, uint32_t timeout);
	I2C_RESULT STM32F4_I2C_writeBytes(uint8_t devAddr, uint8_t regAddr, uint16_t count, uint8_t * buf, uint32_t timeout);
	
#ifdef __cplusplus
}
#endif

#endif
