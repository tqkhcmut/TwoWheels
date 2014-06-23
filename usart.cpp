#include "usart.h"

void miniDelay(void)
{
	int i = 1000;
	while(i--);
}

void USART_init(USART_TypeDef * base)
{
	// USART init
	USART_InitTypeDef USART_InitStructure;
	
	USART_InitStructure.USART_BaudRate            = 115200;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(base, &USART_InitStructure);
	USART_ITConfig(base, USART_IT_RXNE, DISABLE);
 	USART_ITConfig(base, USART_IT_TXE, DISABLE);
	USART_Cmd(base, ENABLE);
}

void USART_sendChar(char c)
{
	USART_SendData(USART1, c);
	miniDelay();
}

void USART_sendStr(char Str[])
{
	while(*Str)
	{
		USART_SendData(USART1, *Str++);
		miniDelay();
	}
}

void USART_sendNum(long num)
{
	unsigned int tmp = 100000000;
	if (num < 0)
	{
		USART_SendData(USART1, '-');
		miniDelay();
		num = 0 - num;
	}
	if (num == 0)
	{
		USART_SendData(USART1, '0');
		return;
	}
	while (num != 0)
	{
		if (num/tmp != 0)
		{
			USART_SendData(USART1, num/tmp+0x30);
			miniDelay();
		}
		tmp = tmp/10;
	}
}

void USART_sendFloat(float num)
{
	int __int = (int) num;
	USART_sendNum(__int);
	USART_sendChar('.');
	__int = (int)((num-__int)*100);
	if (__int < 0)
		__int = 0;
	USART_sendNum(__int);
}

void USART_sendByte(uint8_t b, BYTE_FORMAT f)
{
	uint8_t bitMask = 1;
	uint8_t i;
	switch (f)
	{
		case BIN:
			for (i = 8; i > 0; i--)
			{
				USART_sendChar((b&(bitMask << i)) ? '1' : '0');
			}
			break;
		case OCT:
			break;
		case DEC:
			USART_sendNum(b);
			break;
		case HEX:
			if(b/16 < 10){
				USART_sendChar(0x30 + b/16);
			}else{
				USART_sendChar(0x37 + b/16);
			}

			if(b%16 < 10){
				USART_sendChar(0x30 + b%16);
			}else{
				USART_sendChar(0x37 + b%16);
			}
			break;
		default:
			break;
	}
}


