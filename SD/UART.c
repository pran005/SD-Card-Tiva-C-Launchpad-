/**
  ******************************************************************************
  * @file    UART.c 
  * @author  Pranjal Shrivastava, Aditi Bhatnagar
  * @version v1.0 - UART Basic Utilities  
  * @date    30-July-2019
  * @brief   UART0/1 Drivers
  ******************************************************************************
**/
	
#include <tm4c123gh6pm.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "UART.h"
#include <stdint.h>

/***************************************************************************************************************** 
	@Brief 			 : Initialize UART1 Module, Powers ON UART1, Configures Pins, Sets up Baudrate, FIFO length = 1 byte
	@retval			 : None 
	@Argumemnts  : None 
*******************************************************************************************************************/ 

void init_UART1(void)
{
	SYSCTL->RCGCUART |= (1<<1) ;                   								// Enable the Clock to UART1
  SYSCTL->RCGCGPIO |= (1<<1) ; 																 // Enable Clock to GPIOB 
	
	GPIOB->AFSEL |= (1<<1)|(1<<0); 															// Set GPIO PB.0 & PB.1 to Alternate Function
	GPIOB->PCTL  |= (1<<0)|(1<<4);														 // Select Alternate Function of PB.0 & PB.1 
	GPIOB->DEN 	 |= (1<<0)|(1<<1);														// PB.0 & PB.1 Digital Enable
	
	/** @BAUDRATE = 9660 bits per second, refer manual for calcultion **/
	
	UART1->CTL &= ~(1<<0);
	UART1->IBRD = 325;                                			// Write the integer portion of the BRD to the UARTIRD register
  UART1->FBRD = 33;                                      // Write the fractional portion of the BRD to the UARTFBRD register
  UART1->LCRH = (0x3<<5);                               // 8-bit, no parity, 1 stop bit
  
  UART1->CC = 0x0;																		 // Configure the UART clock source as system clock
  UART1->CTL = (1<<0)|(1<<8)|(1<<9);                  // UART1 Enable, Transmit Enable, Recieve Enable
}

/******************************************************************************************************************
	@Brief 			 : Initialize UART0 Module, Powers ON UART0, Configures Pins, Sets up Baudrate, FIFO length = 1 byte
	@retval			 : None 
	@Argumemnts  : None 
********************************************************************************************************************/ 

void init_UART0(void)
{
	SYSCTL->RCGCUART |= (1<<0);                    							   // Enable the UART0 and UART1 module using the RCGCUART register
  SYSCTL->RCGCGPIO |= (1<<0);                   				 		    // Enables the Port A and Port B GPIO via the RCGCGPIO register
																							
  GPIOA->AFSEL |= (1<<1)|(1<<0);                  		        // enables the alternate function on pin PA0 & PA1
  GPIOA->PCTL |= (1<<0)|(1<<4);                     	       // Configure the GPIOPCTL register to select UART0 in PA0 and PA1 
  GPIOA->DEN |= (1<<0)|(1<<1);                        	    // Enable the digital functionality in PA0 and PA1   
   
	/** @BAUDRATE = 9660 bits per second, refer manual for calcultion **/
		
  UART0->CTL &= ~(1<<0);                              	   // Disable UART0 by clearing UARTEN bit in the UARTCTL register
  UART0->IBRD = 325;                                	    // Write the integer portion of the BRD to the UARTIRD register 
  UART0->FBRD = 33;                           	         // Write the fractional portion of the BRD to the UARTFBRD registerer
  
  UART0->LCRH = (0x3<<5);             	           		 // 8-bit, no parity, 1 stop bit
  UART0->CC = 0x0;                                	  // Configure the UART clock source as system clock
  
  UART0->CTL = (1<<0)|(1<<8)|(1<<9);               	 // UART0 Enable, Transmit Enable, Recieve Enable

}

/***********************************************
	@Brief 			 : Transmits a Data Characater 
	@retval			 : None 
	@Argumemnts  : (char)Data to be transmitted
***********************************************/ 

void UART0_Tx(char c)                                                  
{
  while((UART0->FR &(1<<5)) != 0);                                      // waits the program till the previous transmission was completed
  UART0->DR =c;                                                         // Set UART0 DATA REGISTER is equal to c
}

/**************************************************** 
	@Brief 			 : Transmits a Data Integer/Num 
	@retval			 : None 
	@Argumemnts  : (int)Data to be transmitted with sign
******************************************************/ 
void UART0_Tx_num(int32_t m)  
{
	uint8_t i=0, sign=0,r,p  ;
	char str[100] ;	
	if(m==0)
	{ str[i] = '0' ;
		i++ ;
		str[i]= '\0' ;
	}
	if(m<0)
	{ m=-m ;
		sign=1 ;
	}
	while(m!=0)
	{
		r=m%10 ;
		str[i++] = r+ '0' ;
		m=m/10 ;
	}
	if(sign==1)
	str[i++] = '-' ;
	str[i++] = '\0' ;
	p=strlen(str) ; 
	for(int i=p-1 ; i>=0 ; i--)
	UART0_Tx(str[i]) ;
}

/***********************************************
	@Brief 			 : Transmits a String 
	@retval			 : None 
	@Argumemnts  : Ptr to string to be transmitted
************************************************/ 

void UART0_str(char * string)                                         
{
  while(*string)
  {
    UART0_Tx(*(string++));                                             // printChar is called with every increment in string pointer
  }
}

/********************************************
	@Brief 			 : Receives a charater 
	@retval			 : Recieved Character (1 byte) 
	@Argumemnts  : None
*********************************************/ 
char UART0_RxChar(void)
{
	while((UART0-> FR & (1<<6)) == 0) ; 
	return (char) UART0 -> DR ;
}

/******************************************************** 
	@Brief 			 : Receives a String 
	@retval			 : None 
	@Argumemnts  : Ptr to string buffer to save the string
*********************************************************/ 
uint8_t UART0_RxString(char *ptr_string)
{
    char ch;
    uint8_t len = 0;
    while(1)
    {
        ch=UART0_RxChar();   																		 // Receive a char
        UART0_Tx(ch);     																			// Echo back the received char

        if((ch=='\r') || (ch=='\n')) 													 // Read till enter key is pressed
        {   
		  if(len!=0)       																			  // Wait till atleast 1 char is received               
           {                                              
		       ptr_string[len]=0;          											 // once enter key is pressed null terminate the string and break the loop 			
               break;  
			}                   
        }
        else if((ch=='\b') && (len!=0))
        {
            len--;    																		 // If backspace is pressed then decrement the index to remove the old char
        }
        else
        {
            ptr_string[len]=ch; 												 //copy the char into string and increment the index
            len++;
        }
    }
  return len;   
}


