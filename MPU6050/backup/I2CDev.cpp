#include "I2CDev.h"

I2CDev::I2CDev()
{
	IsBusy = 0;
	buff_length = 0;
	TimeOut = 0x100;
}

void I2CDev::Init(void)
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
	
	// Enable peripherals
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	
	// deinit
	I2C_DeInit(I2C2);
	
	// I2C configuration
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 100000;	// 400kHz
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
	
	// ENTR_CRT_SECTION();
 
  /* While the bus is busy */
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
 
  /* Send START condition */
  I2C_GenerateSTART(I2C2, ENABLE);
 
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
 
  /* Send MPU6050 address (0xD0) for write */ 
  I2C_Send7bitAddress(I2C2, devAddr, I2C_Direction_Transmitter);
 
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
 
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C2, ENABLE);
 
  /* Send the MPU6050_Magn's internal address to write to */
  I2C_SendData(I2C2, regAddr);
 
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
 
  /* Send STRAT condition a second time */
  I2C_GenerateSTART(I2C2, ENABLE);
 
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
 
  /* Send MPU6050 address for read */
  I2C_Send7bitAddress(I2C2, devAddr, I2C_Direction_Receiver);
 
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
 
  /* While there is data to be read */
  while(count)
  {
    if(count == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(I2C2, DISABLE);
 
      /* Send STOP Condition */
      I2C_GenerateSTOP(I2C2, ENABLE);
    }
 
    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))
    {
      /* Read a byte from the MPU6050 */
      *buf = I2C_ReceiveData(I2C2);
 
      /* Point to the next location where the byte read will be saved */
      buf++;
 
      /* Decrement the read bytes counter */
      count--;
    }
  }
 
  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2C2, ENABLE);
//  EXT_CRT_SECTION();
	
	// 
	IsBusy = 0;
	return I2C_OK;	
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
			if(millis() - tmpTime > TimeOut) 
			{
				IsBusy = 0;
				return I2C_TIMEOUT;
			}
		}
		
		
		/* Enable the I2C peripheral */
		I2C_GenerateSTART(I2C2, ENABLE);
		
		/* Test on SB Flag */
		tmpTime = millis();
		while (I2C_GetFlagStatus(I2C2,I2C_FLAG_SB) == RESET) 
		{
			if(millis() - tmpTime > TimeOut) 
			{
				IsBusy = 0;
				return I2C_TIMEOUT;
			}
		}
		
		/* Transmit the slave address and enable writing operation */
		I2C_Send7bitAddress(I2C2, devAddr, I2C_Direction_Transmitter);
		
		/* Test on ADDR Flag */
		tmpTime = millis();
		while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		{
			if(millis() - tmpTime > TimeOut) 
			{
				IsBusy = 0;
				return I2C_TIMEOUT;
			}
		}
		
		/* Transmit the first address for r/w operations */
		I2C_SendData(I2C2, regAddr);
		
		/* Test on TXE FLag (data sent) */
		tmpTime = millis();
		while ((!I2C_GetFlagStatus(I2C2,I2C_FLAG_TXE)) || (!I2C_GetFlagStatus(I2C2,I2C_FLAG_BTF)))  
		{
			if(millis() - tmpTime > TimeOut) 
			{
				IsBusy = 0;
				return I2C_TIMEOUT;
			}
		}  
			
		 /* Transmit the first address for r/w operations */
		I2C_SendData(I2C2, *buf);
		
		/* Test on TXE FLag (data sent) */
		tmpTime = millis();
		while ((!I2C_GetFlagStatus(I2C2,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(I2C2,I2C_FLAG_BTF)))  
		{
			if(millis() - tmpTime > TimeOut) 
			{
				IsBusy = 0;
				return I2C_TIMEOUT;
			}
		} 
			
		/*!< Send STOP Condition */
		I2C_GenerateSTOP(I2C2, ENABLE);
		 
			
		/* Wait to make sure that STOP control bit has been cleared */
		tmpTime = millis();
		while(I2C2->CR1 & I2C_CR1_STOP)
		{
			if(millis() - tmpTime > TimeOut) 
			{
				IsBusy = 0;
				return I2C_TIMEOUT;
			}
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

I2C_RESULT I2CDev::readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t bitLengh, uint8_t * buf)
{
	uint8_t bitMask = 1<<bitStart;
	uint8_t i = 0;
	while(i < bitLengh - bitStart + 1)
	{
		bitMask |= bitStart << i++;
	}
	return readBits(devAddr, regAddr, bitMask, buf);
}

I2C_RESULT I2CDev::readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitPos, uint8_t * buf)	// level: high or low
{
	return readBits(devAddr, regAddr, 1<<bitPos, buf);
}

I2C_RESULT I2CDev::readByte(uint8_t devAddr, uint8_t regAddr, uint8_t * buf)
{
	return readBytes(devAddr, regAddr, 1, buf);
}

I2C_RESULT I2CDev::writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t _byte)
{
	return writeBytes(devAddr, regAddr, 1, &_byte);
}

I2C_RESULT I2CDev::writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t bitLengh, uint8_t data)
{
	uint8_t bitMask = 1<<bitStart;
	uint8_t i = 0;
	while(i < bitLengh - bitStart + 1)
	{
		bitMask |= ((bitStart << i++) | data);
	}
	
	return writeBits(devAddr, regAddr, bitMask);
}

I2C_RESULT I2CDev::writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitPos, uint8_t level)
{
	uint8_t bitMask = 0;
	if (level)
		bitMask = 1<<bitPos;
	
	return writeBits(devAddr, regAddr, bitMask);
}

I2C_RESULT I2CDev::readWord(uint8_t devAddr, uint8_t regAddr, uint16_t * data)
{
	return I2C_OK;
}

I2C_RESULT I2CDev::writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data)
{
	return I2C_OK;
}

