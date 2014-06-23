#ifndef I2CDev_h
#define I2CDev_h

#include <inttypes.h>
#include "stm32f4xx.h"
// if you have another system timer, comment line below then include it
#include "delay.h"

#ifndef USE_I2C2
#define USE_I2C2
#else
#error "I2C2 have been used."
#endif

#define I2C2_SDA_PIN GPIO_Pin_0
#define I2C2_SDA_PIN_SRC	GPIO_PinSource0
#define I2C2_SDA_PIN_AF		GPIO_AF_I2C2

#define I2C2_SCL_PIN GPIO_Pin_1
#define I2C2_SCL_PIN_SRC	GPIO_PinSource1
#define I2C2_SCL_PIN_AF		GPIO_AF_I2C2

#define I2C2_GPIO_PINS (I2C2_SDA_PIN | I2C2_SCL_PIN)
#define I2C2_GPIO_BASE GPIOF

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
	I2C_RESULT readBytes(uint8_t devAddr, uint8_t regAddr, uint16_t count, uint8_t * buf);
	I2C_RESULT writeBytes(uint8_t devAddr, uint8_t regAddr, uint16_t count, uint8_t * buf);
	I2C_RESULT writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitMask, uint8_t * buf);
};

#endif
