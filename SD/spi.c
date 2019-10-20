/*******************************************************************************
  * @file    spi.c 
  * @author  Pranjal Shrivastava, Aditi Bhatnagar
  * @version v2.0 - SD Card  
  * @date    12-October-2019
  * @brief   SPI Driver
  ******************************************************************************
**/

#include "TM4C123.h"                    // Device header

#define	CS	(1U<<3)										 // Chip Select hai ye (PF.3)

/******************************************************************************* 
	@Brief 			 : You really want a brief for these functions?! Read their names! 
	@retval			 : None
	@Argumemnts  : None  
********************************************************************************/ 

void SPI_EnableChipSelect(void)
{
	 GPIOF -> DATA &= ~CS ;
}

void SPI_DisableChipSelect(void)
{
	 GPIOF -> DATA |= CS ;
}

/******************************************************************************************************** 
	@Brief 			 : Initialize SSI0 Module, Powers ON, Configures Pins, Sets up Clock, FIFO length = 1 byte
	@retval			 : None 
	@Argumemnts  : None 
*********************************************************************************************************/ 

void init_SSI0(void)
{
	
	SYSCTL -> RCGCSSI |= (1<<0) ; 					 															    //Enable SSI0 Clock	
	SYSCTL -> RCGCGPIO |= (1<<0) | (1<<5) ;									  					  	 //Enable GPIOA & GPIOF Clock
	while (!(SYSCTL -> PRGPIO & 0x21)) ; 											  				  	// wait for GPIOA to be peripheral ready  

	/** @NOTE : Refer main.c for connection diagram **/  
	
	GPIOA -> AFSEL |= (1<<2) | (1<<3) | (1<<4) | (1<<5) ;  	 
	GPIOA -> PCTL |= (1<<9) | (1<<13) | (1<<21) | (1<<17) ; 					 		// Pin Mux selection for SSI0  
	GPIOA -> AMSEL &= ~(0x3C) ; 													  			  		 // Disable Analog mode select for 2,3,5  
	GPIOA -> DEN |= (1<<2) | (1<<3) | (1<<5) | (1<<4)  ;  			   		  // Digital Enable for 2,3,5 
	
	/** GPIOF Configuraion for CS **/ 
	GPIOF -> DIR |= (1<<3) ; 																			 
	GPIOF -> DEN |= (1<<3) ;
	GPIOF -> AMSEL &= ~(1<<3) ; 	
	GPIOA -> DIR &= ~(1<<4) ; 
	
	SSI0 -> CR1 &= ~(1<<1) ; 																  			// Disable SSI0 
	SSI0 -> CR1 &= ~(1<<2) ; 					   					 					   		 // Master of Puppets
	
	SSI0 -> CC = (SSI0 -> CC & ~(0X0F)) + 0x00 ;			   		     // SYSCLK => Avoid modifying reserved bits  
	
	/** @CLOCK : Change kar, faster! **/ 
	SSI0 -> CPSR = (SSI0 -> CPSR & ~(0xFF))+ 0x0A ;			   		// 5MHz CLK = 5000000/10 ; BR=SysClk/(CPSDVSR * (1 + SCR)) 0x7D
  
	/** @MAGIX ***/ 
	SSI0 -> CR0 = ( 0x00000007 ) ; 													// SCR = 0 ; SPHA = 0 ; SPOL = 0 ; Sample on rising , shift on falling ; Freescale SPI Mode 
	SSI0 -> CR1 |= (1<<1) ; 													 		 // Enable SSI  
}

/******************************************************** 
	@Brief 			 : Transmits a byte of Data
	@retval			 : Transmitted Byte (required in SD Driver
	@Argumemnts  : Data to be transmitted (1 byte)  
********************************************************/ 

uint8_t SSI_Write(uint8_t data) 
{
	uint8_t rcv_data ; 
  while((SSI0->SR & (1<<1))==0)	;  									 // wait till Tx FIFO FULL 
	SSI0->DR = data;                        
	while((SSI0->SR & (1<<2))==0) ;									  // wait till Receiver Not Empty
	rcv_data = SSI0->DR ;  													 /** @PROTIP : Read DR to clear flags and avoid bugs in any SPI Driver **/ 
  return data ; 															
}


/************************************************************************************
	@Brief 			 : Receive a byte of Data, Transmits out 0xFF (Required in SD Inteface) 
	@retval			 : None
	@Argumemnts  : Data received (1 byte)  
*************************************************************************************/ 
uint8_t SSI_Read(void) 
{
  while((SSI0->SR & (1<<1))==0)	;  								// wait till Tx FIFO FULL 
	SSI0->DR = 0xFF ;                        
	while((SSI0->SR & (1<<2))==0) ;								 // wait till Receiver Not Empty
	return (uint8_t)SSI0 -> DR ; 
}


