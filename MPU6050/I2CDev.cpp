#include "I2CDev.h"

I2CDev::I2CDev()
{
	IsBusy = 0;
	buff_length = 0;
	TimeOut = 0;
}

void I2CDev::Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	
	// Enable peripherals
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	/* GPIOB Configuration:  I2C2 SDA (PF0) and I2C2 SCL (PF1) */
  GPIO_InitStructure.GPIO_Pin = I2C2_GPIO_PINS;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(I2C2_GPIO_BASE, &GPIO_InitStructure); 

	// Connect to alternate functions
  GPIO_PinAFConfig(I2C2_GPIO_BASE, I2C2_SDA_PIN_SRC, I2C2_SDA_PIN_AF);
  GPIO_PinAFConfig(I2C2_GPIO_BASE, I2C2_SCL_PIN_SRC, I2C2_SCL_PIN_AF);
	
	// I2C configuration
	I2C_InitStructure.I2C_Mode = I2C_Mode_SMBusHost;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 100000;	// 100kHz
	I2C_Init(I2C2, &I2C_InitStructure);
	
	// Enable I2C2
	I2C_Cmd(I2C2, ENABLE);
}

I2C_RESULT I2CDev::readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitMask, uint8_t * buf)
{
	// busy check
	while(IsBusy);
	
	if (readBytes(devAddr, regAddr, 1, buf) == I2C_OK)
	{
		buf[0] &= bitMask;
		
		IsBusy = 0;
		return I2C_OK;
	}
	
	// else
	return I2C_FAIL;
}

I2C_RESULT I2CDev::readBytes(uint8_t devAddr, uint8_t regAddr, uint16_t count, uint8_t * buf)
{
	uint32_t tmpTime = millis();
	// busy check
	while(IsBusy);
	IsBusy = 1;
	
	if (count == 1) // multi-bytes haven't been implemented yet
	{
		while (I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY)) 
		{
			if(millis() - tmpTime > TimeOut) return I2C_TIMEOUT;
		}
		
		/* Enable the I2C peripheral */
		I2C_GenerateSTART(I2C2, ENABLE);
		
		/* Test on SB Flag */
		tmpTime = millis();
		while (!I2C_GetFlagStatus(I2C2,I2C_FLAG_SB)) 
		{
			if(millis() - tmpTime > TimeOut) return I2C_TIMEOUT;
		}
		
		/* Send device address for write */
		I2C_Send7bitAddress(I2C2, devAddr, I2C_Direction_Transmitter);
		
		/* Test on ADDR Flag */
		tmpTime = millis();
		while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) 
		{
			if(millis() - tmpTime > TimeOut) return I2C_TIMEOUT;
		}
		
		/* Send the device's internal address to write to */
		I2C_SendData(I2C2, regAddr);  
		
		/* Test on TXE FLag (data sent) */
		tmpTime = millis();
		while ((!I2C_GetFlagStatus(I2C2,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(I2C2,I2C_FLAG_BTF)))  
		{
			if(millis() - tmpTime > TimeOut) return I2C_TIMEOUT;
		}
		
		/* Send START condition a second time */  
		I2C_GenerateSTART(I2C2, ENABLE);
		
		/* Test on SB Flag */
		tmpTime = millis();
		while (!I2C_GetFlagStatus(I2C2,I2C_FLAG_SB)) 
		{
			if(millis() - tmpTime > TimeOut) return I2C_TIMEOUT;
		}
		
		/* Send Sensor address for read */
		I2C_Send7bitAddress(I2C2, devAddr, I2C_Direction_Receiver);
		
		/* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
		tmpTime = millis();
		while(I2C_GetFlagStatus(I2C2, I2C_FLAG_ADDR) == RESET)
		{
			if(millis() - tmpTime > TimeOut) return I2C_TIMEOUT;
		}     
		
		/*!< Disable Acknowledgement */
		I2C_AcknowledgeConfig(I2C2, DISABLE);   

		/* Call User callback for critical section start (should typically disable interrupts) */
	//     Sensor_EnterCriticalSection_UserCallback();
		
		/* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
		(void)I2C2->SR1;
		(void)I2C2->SR2;
		
		/*!< Send STOP Condition */
		I2C_GenerateSTOP(I2C2, ENABLE);
	 
		/* Call User callback for critical section end (should typically re-enable interrupts) */
	//     Sensor_ExitCriticalSection_UserCallback();
		
		/* Wait for the byte to be received */
		tmpTime = millis();
		while(I2C_GetFlagStatus(I2C2, I2C_FLAG_RXNE) == RESET)
		{
			if(millis() - tmpTime > TimeOut) return I2C_TIMEOUT;
		}
		
		/*!< Read the byte received from the EEPROM */
		*buf = I2C_ReceiveData(I2C2);
		
		/* Wait to make sure that STOP control bit has been cleared */
		tmpTime = millis();
		while(I2C2->CR1 & I2C_CR1_STOP)
		{
			if(millis() - tmpTime > TimeOut) return I2C_TIMEOUT;
		}  
		
		/*!< Re-Enable Acknowledgement to be ready for another reception */
		I2C_AcknowledgeConfig(I2C2, ENABLE);
		
		// 
		IsBusy = 0;
		return I2C_OK;
	}
	
	// 
	IsBusy = 0;
	return I2C_FAIL;	
}

I2C_RESULT I2CDev::writeBytes(uint8_t devAddr, uint8_t regAddr, uint16_t count, uint8_t * buf)
{
	uint32_t tmpTime = 0;
	// busy check
	while(IsBusy);
	IsBusy = 1;
	
	if (count == 1) // multi-bytes haven't been implemented yet
	{
		/* Test on BUSY Flag */
		tmpTime = millis();
		while (I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY)) 
		{
			if(millis() - tmpTime > TimeOut) return I2C_TIMEOUT;
		}
		
		
		/* Enable the I2C peripheral */
		I2C_GenerateSTART(I2C2, ENABLE);
		
		/* Test on SB Flag */
		tmpTime = millis();
		while (I2C_GetFlagStatus(I2C2,I2C_FLAG_SB) == RESET) 
		{
			if(millis() - tmpTime > TimeOut) return I2C_TIMEOUT;
		}
		
		/* Transmit the slave address and enable writing operation */
		I2C_Send7bitAddress(I2C2, devAddr, I2C_Direction_Transmitter);
		
		/* Test on ADDR Flag */
		tmpTime = millis();
		while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		{
			if(millis() - tmpTime > TimeOut) return I2C_TIMEOUT;
		}
		
		/* Transmit the first address for r/w operations */
		I2C_SendData(I2C2, regAddr);
		
		/* Test on TXE FLag (data sent) */
		tmpTime = millis();
		while ((!I2C_GetFlagStatus(I2C2,I2C_FLAG_TXE)) || (!I2C_GetFlagStatus(I2C2,I2C_FLAG_BTF)))  
		{
			if(millis() - tmpTime > TimeOut) return I2C_TIMEOUT;
		}  
			
		 /* Transmit the first address for r/w operations */
		I2C_SendData(I2C2, *buf);
		
		/* Test on TXE FLag (data sent) */
		tmpTime = millis();
		while ((!I2C_GetFlagStatus(I2C2,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(I2C2,I2C_FLAG_BTF)))  
		{
			if(millis() - tmpTime > TimeOut) return I2C_TIMEOUT;
		} 
			
		/*!< Send STOP Condition */
		I2C_GenerateSTOP(I2C2, ENABLE);
		 
			
		/* Wait to make sure that STOP control bit has been cleared */
		tmpTime = millis();
		while(I2C2->CR1 & I2C_CR1_STOP)
		{
			if(millis() - tmpTime > TimeOut) return I2C_TIMEOUT;
		}  
			
		// 
		IsBusy = 0;
		return I2C_OK;
	}
	
	// 
	IsBusy = 0;
	return I2C_FAIL;
}

I2C_RESULT I2CDev::writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitMask)
{
	// busy check
	while(IsBusy);
	
	if (readBytes(devAddr, regAddr, 1, buff) == I2C_OK)
		buff[0] |= bitMask;
	else return I2C_FAIL;
	
	return writeBytes(devAddr, regAddr, 1, buff);
}

