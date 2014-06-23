#ifndef _usart_h_
#define _usart_h_

#ifdef __cplusplus
 extern "C" {
#endif
	 
//#define USE_USART

#include "stm32f4xx.h"

typedef enum 
{
	BIN = 0,
	OCT,
	DEC,
	HEX
} BYTE_FORMAT;


void USART_init(USART_TypeDef * base);
void USART_sendChar(char c);
void USART_sendStr(char Str[]);
void USART_sendNum(long num);
void USART_sendFloat(float num);
void USART_sendByte(uint8_t b, BYTE_FORMAT f);

#ifdef __cplusplus
}
#endif

#endif

