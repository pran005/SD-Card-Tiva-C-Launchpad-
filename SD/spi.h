/**
  ******************************************************************************
  * @file    spi.h 
  * @author  Pranjal Shrivastava, Aditi Bhatnagar
  * @version v2.0 - SD Card  
  * @date    12-October-2019
  * @brief   SPI Header Definitions
  ******************************************************************************
**/
#ifndef _spi_h
#define _spi_h

#include <stdint.h>


void SPI_EnableChipSelect(void); 
void SPI_DisableChipSelect(void) ; 
void init_SSI0(void) ; 
uint8_t SSI_Write(uint8_t data) ; 
uint8_t SSI_Read(void) ; 


#endif 

