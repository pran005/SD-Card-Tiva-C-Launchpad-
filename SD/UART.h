/**
  ******************************************************************************
  * @file    UART.h 
  * @author  Pranjal Shrivastava, Aditi Bhatnagar
  * @version v2.0 - SD Card  
  * @date    30-September-2019
  * @brief   UART Header Definitions
  ******************************************************************************
**/
#ifndef _UART_H
#define _UART_H

#include <inttypes.h>

typedef signed char     sint8_t;
typedef unsigned char   uint8_t;

typedef signed short int      sint16_t;
typedef unsigned short int    uint16_t;

typedef signed  int    sint32_t;
typedef unsigned  int  uint32_t;

#define C_SINT8_MAX   0x7F
#define C_SINT8_MIN  -128

#define C_UINT8_MAX   0xFFu
#define C_UINT8_MIN   0x00u

#define C_SINT16_MAX  32767
#define C_SINT16_MIN -32768

#define C_UINT16_MAX  0xFFFFu
#define C_UINT16_MIN  0x00u

#define C_SINT32_MAX  2147483647
#define C_SINT32_MIN -2147483648

#define C_UINT32_MAX  0xFFFFFFFFu
#define C_UINT32_MIN  0x00u

#define C_DefaultDigitsToTransmit_U8          0xffu    // Will transmit the exact digits in the number
#define C_MaxDigitsToTransmit_U8              10u      // Max decimal/hexadecimal digits to be transmitted
#define C_NumOfBinDigitsToTransmit_U8         16u      // Max bits of a binary number to be transmitted
#define C_MaxDigitsToTransmitUsingPrintf_U8   C_DefaultDigitsToTransmit_U8 /* Max dec/hexadecimal digits to be displayed using printf */

void init_UART1(void) ; 
void init_UART0(void) ;
void UART0_Tx(char c) ; 
void UART0_str(char * string);
void UART1_Tx(char c);
void UART1_str(char * string) ;   
char UART0_RxChar(void) ; 
uint8_t UART0_RxString(char *ptr_string) ; 
void UART0_Printf(const char *argList, ...) ; 

#endif
