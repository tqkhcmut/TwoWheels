#ifndef I2CDev_h
#define I2CDev_h

#include <inttypes.h>
#include "stm32f4xx.h"
// if you have another system timer, comment line below then include it
#include "../delay.h"

#ifndef USE_I2C2
#define USE_I2C2
#else
#error "I2C2 have been used."
#endif

#define I2C_SDA_PIN GPIO_Pin_11
#define I2C_SDA_PIN_SRC	GPIO_PinSource11
#define I2C_SDA_PIN_AF		GPIO_AF_I2C2
#define I2C_SDA_GPIO_BASE GPIOB
#define I2C_SDA_RCCPERIPH RCC_AHB1Periph_GPIOB

#define I2C_SCL_PIN GPIO_Pin_10
#define I2C_SCL_PIN_SRC	GPIO_PinSource10
#define I2C_SCL_PIN_AF		GPIO_AF_I2C2
#define I2C_SCL_GPIO_BASE GPIOB
#define I2C_SCL_RCCPERIPH RCC_AHB1Periph_GPIOB

//#define I2C2_GPIO_PINS (I2C2_SDA_PIN | I2C2_SCL_PIN)
//#define I2C2_GPIO_BASE GPIOA

#ifdef I2C_SENSORS
typedef enum { SENSORS_FAIL = 0, SENSORS_OK, TIMEOUT } I2C_RESULT;
#else
typedef enum { I2C_FAIL = 0, I2C_OK, I2C_TIMEOUT } I2C_RESULT;
#endif

class I2CDev
{
	private:
	uint8_t buff[16];
	int32_t buff_length;
	uint8_t IsBusy;
	uint32_t TimeOut;
	
	public:
		I2CDev();
	void Init(void);
	I2C_RESULT readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitMask, uint8_t * buf);
	I2C_RESULT readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t bitLengh, uint8_t * buf);
	I2C_RESULT readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitPos, uint8_t * buf);	// level: high or low
	I2C_RESULT readBytes(uint8_t devAddr, uint8_t regAddr, uint16_t count, uint8_t * buf);
	I2C_RESULT readByte(uint8_t devAddr, uint8_t regAddr, uint8_t * buf);
	I2C_RESULT writeBytes(uint8_t devAddr, uint8_t regAddr, uint16_t count, uint8_t * buf);
	I2C_RESULT writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t _byte);
	I2C_RESULT writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitMask);
	I2C_RESULT writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t bitLengh, uint8_t data);
	I2C_RESULT writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitPos, uint8_t level);
	I2C_RESULT readWord(uint8_t devAddr, uint8_t regAddr, uint16_t * data);
	I2C_RESULT writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
};

#endif
