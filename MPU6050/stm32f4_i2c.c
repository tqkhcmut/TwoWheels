#include "stm32f4_i2c.h"

/* This function issues a start condition and 
 * transmits the slave address + R/W bit
 * 
 * Parameters:
 * 		I2Cx --> the I2C peripheral e.g. I2C1
 * 		address --> the 7 bit slave address
 * 		direction --> the tranmission direction can be:
 * 						I2C_Direction_Tranmitter for Master transmitter mode
 * 						I2C_Direction_Receiver for Master receiver
 */
static I2C_RESULT I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction, uint32_t timeout)
{
	uint32_t tmpTime = millis();
	
	// wait until I2C1 is not busy anymore
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
	{
		if (millis() - tmpTime > timeout)
			return I2C_TIMEOUT;
	}
  
	// Send I2C1 START condition 
	I2C_GenerateSTART(I2Cx, ENABLE);
	  
	// wait for I2C1 EV5 --> Slave has acknowledged start condition
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if (millis() - tmpTime > timeout)
			return I2C_TIMEOUT;
	}
		
	// Send slave Address for write 
	I2C_Send7bitAddress(I2Cx, address, direction);
	  
	/* wait for I2C1 EV6, check if 
	 * either Slave has acknowledged Master transmitter or
	 * Master receiver mode, depending on the transmission
	 * direction
	 */ 
	if(direction == I2C_Direction_Transmitter){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		{
			if (millis() - tmpTime > timeout)
				return I2C_TIMEOUT;
		}
	}
	else if(direction == I2C_Direction_Receiver){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		{
			if (millis() - tmpTime > timeout)
				return I2C_TIMEOUT;
		}
	}
	
	return I2C_OK;
}

/* This function transmits one byte to the slave device
 * Parameters:
 *		I2Cx --> the I2C peripheral e.g. I2C1 
 *		data --> the data byte to be transmitted
 */
static I2C_RESULT I2C_write(I2C_TypeDef* I2Cx, uint8_t data, uint32_t timeout)
{
	uint32_t tmpTime = millis();
	
	I2C_SendData(I2Cx, data);
	// wait for I2C1 EV8_2 --> byte has been transmitted
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if (millis() - tmpTime > timeout)
			return I2C_TIMEOUT;
	}
	
	return I2C_OK;
}

/* This function reads one byte from the slave device 
 * and acknowledges the byte (requests another byte)
 */
static I2C_RESULT I2C_read_ack(I2C_TypeDef* I2Cx, uint8_t * p_data, uint32_t timeout)
{
	uint32_t tmpTime = millis();
	
	// enable acknowledge of recieved data
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) )
	{
		if (millis() - tmpTime > timeout)
			return I2C_TIMEOUT;
	}
	// read data from I2C data register and return data byte
	*p_data = I2C_ReceiveData(I2Cx);
	return I2C_OK;
}

/* This function reads one byte from the slave device
 * and doesn't acknowledge the recieved data 
 */
static I2C_RESULT I2C_read_nack(I2C_TypeDef* I2Cx, uint8_t * p_data, uint32_t timeout)
{
	uint32_t tmpTime = millis();
	
	// disabe acknowledge of received data
	// nack also generates stop condition after last byte received
	// see reference manual for more info
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	I2C_GenerateSTOP(I2Cx, ENABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) )
	{
		if (millis() - tmpTime > timeout)
			return I2C_TIMEOUT;
	}
	// read data from I2C data register and return data byte
	*p_data = I2C_ReceiveData(I2Cx);
	return I2C_OK;
}

/* This funtion issues a stop condition and therefore
 * releases the bus
 */
static void I2C_stop(I2C_TypeDef* I2Cx){
	// Send I2C1 STOP Condition 
	I2C_GenerateSTOP(I2Cx, ENABLE);
}

void STM32F4_I2C_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	
	// Enable peripherals
	RCC_AHB1PeriphClockCmd(I2C_SCL_RCCPERIPH, ENABLE);
	RCC_AHB1PeriphClockCmd(I2C_SDA_RCCPERIPH, ENABLE);
	
	/* GPIO Configuration:  I2C2 SCL (PB10) and I2C2 SDA (PB11) */
  GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(I2C_SDA_GPIO_BASE, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(I2C_SCL_GPIO_BASE, &GPIO_InitStructure); 

	// Connect to alternate functions
  GPIO_PinAFConfig(I2C_SDA_GPIO_BASE, I2C_SDA_PIN_SRC, I2C_SDA_PIN_AF);
  GPIO_PinAFConfig(I2C_SCL_GPIO_BASE, I2C_SCL_PIN_SRC, I2C_SCL_PIN_AF);
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	// deinit
	I2C_DeInit(SENSOR_I2C);
	
	// I2C configuration
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = SENSOR_I2C_SPEED;
	I2C_Init(SENSOR_I2C, &I2C_InitStructure);
	
	
	// Enable I2C2
	I2C_Cmd(SENSOR_I2C, ENABLE);
}

I2C_RESULT STM32F4_I2C_readBytes(uint8_t devAddr, uint8_t regAddr, uint16_t count, uint8_t * buf, uint32_t timeout)
{
	uint16_t cnt = 0;
	uint32_t tmpTime = millis();
	I2C_RESULT res;
	
	if (count == 0) return I2C_OK;
	if (count == 1)
	{
		// read single byte
		I2C_AcknowledgeConfig(SENSOR_I2C, ENABLE);
		
		res = I2C_start(SENSOR_I2C, devAddr, I2C_Direction_Transmitter, timeout);
		if (res != I2C_OK)
			return res;
		res = I2C_write(SENSOR_I2C, regAddr, timeout);
		if (res != I2C_OK)
			return res;
		
		res = I2C_start(SENSOR_I2C, devAddr, I2C_Direction_Receiver, timeout);
		if (res != I2C_OK)
			return res;
		res = I2C_read_nack(SENSOR_I2C, buf, timeout);
		if (res != I2C_OK)
			return res;
		I2C_stop(SENSOR_I2C);
		
		/* return a Reg value */
		return res; 
		
	}
	
	// read multi bytes
	I2C_AcknowledgeConfig(SENSOR_I2C, ENABLE);
	
	res = I2C_start(SENSOR_I2C, devAddr, I2C_Direction_Transmitter, timeout);
		if (res != I2C_OK)
			return res;
	res = I2C_write(SENSOR_I2C, regAddr, timeout);
		if (res != I2C_OK)
			return res;
	
	res = I2C_start(SENSOR_I2C, devAddr, I2C_Direction_Receiver, timeout);
		if (res != I2C_OK)
			return res;
	
	for (cnt = 0; cnt < count - 1; cnt++)
	{
		res = I2C_read_ack(SENSOR_I2C, buf + cnt, timeout);
		if (res != I2C_OK)
			return res;
	}
	res = I2C_read_nack(SENSOR_I2C, buf + cnt, timeout);
		if (res != I2C_OK)
			return res;
	
	I2C_stop(SENSOR_I2C);
	
	return res;
}

I2C_RESULT STM32F4_I2C_writeBytes(uint8_t devAddr, uint8_t regAddr, uint16_t count, uint8_t * buf, uint32_t timeout)
{
	uint16_t cnt = 0;
	uint32_t tmpTime = millis();
	I2C_RESULT res;
	
	if (count == 0)
		return I2C_FAIL;
	if (count == 1)
	{
		// write single byte
    I2C_AcknowledgeConfig(SENSOR_I2C, ENABLE);
		
		res = I2C_start(SENSOR_I2C, devAddr, I2C_Direction_Transmitter, timeout);
		if (res != I2C_OK)
			return res;
		res = I2C_write(SENSOR_I2C, regAddr, timeout);
		if (res != I2C_OK)
			return res;
		res = I2C_write(SENSOR_I2C, *buf, timeout);
		if (res != I2C_OK)
			return res;
		I2C_stop(SENSOR_I2C);
		
		return res;
	}
	// write multiple bytes
	I2C_AcknowledgeConfig(SENSOR_I2C, ENABLE);
		
	res = I2C_start(SENSOR_I2C, devAddr, I2C_Direction_Transmitter, timeout);
		if (res != I2C_OK)
			return res;
	res = I2C_write(SENSOR_I2C, regAddr, timeout);
		if (res != I2C_OK)
			return res;
	for (cnt = 0; cnt < count; cnt++)
	{
		res = I2C_write(SENSOR_I2C, *(buf + cnt), timeout);
		if (res != I2C_OK)
			return res;
	}
	I2C_stop(SENSOR_I2C);

	return res;
}

/**
  * @brief  Checks the Sensor status.
  * @param  None
  * @retval ErrorStatus: Sensor Status (ERROR or SUCCESS).
  */
I2C_RESULT Sensor_GetStatus(uint8_t devAddr, uint32_t timeout)
{
	uint32_t tmpTime = millis();
  
	/*!< Clear the SENSOR_I2C AF flag */
  I2C_ClearFlag(SENSOR_I2C, I2C_FLAG_AF);

  /*!< Enable SENSOR_I2C acknowledgement if it is already disabled by other function */
  I2C_AcknowledgeConfig(SENSOR_I2C, ENABLE);

  /*---------------------------- Transmission Phase ---------------------------*/

	I2C_start(SENSOR_I2C, devAddr, I2C_Direction_Transmitter, timeout);
	
  if (I2C_GetFlagStatus(SENSOR_I2C, I2C_FLAG_AF) != 0x00)
  {
		if (millis() - tmpTime > timeout)
			return I2C_TIMEOUT;
    return I2C_FAIL;
  }
  else
  {
    return I2C_OK;
  }
}

