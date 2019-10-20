/**
  ******************************************************************************
  * @file    main.c 
  * @author  Pranjal Shrivastava, Aditi Bhatnagar
  * @version v2.0 - SD Card
  * @date    12-October-2019
  * @brief   Main program body 
  ******************************************************************************
**/
	

#include <tm4c123gh6pm.h>
#include "sd.h"
#include "uart.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "fat32.h"
#include "spi.h"     

/*** 
				_______________________________________________
			 |														 									 |		  ________________ 
			 |		TM4C123GH6PM : Tiva C Series Launchpad     |		 | 		SD Card			|					 Vdd(3.3v)
			 |					 							SSI0       PA.4(MISO)  |<----| MISO	(DI/CMD)  |-----------| 
			 |						                       PA.5(MOSI)	 |---->| MOSI	(DAT0/DO)	|	
			 |																							 |		 |________________|	
			 |																							 |   	 |	   						|	
			 |						 											 PA.2(CLK)	 |---->| (S)CLK					|	
			 |									  							 PF.3(CS) 	 |---->| CD/DAT3        |
			 |_______________________________________________|     | 			16GB			| 					
																														 |________________|	
**/


#define FILE_READ         '1'
#define FILE_WRITE        '2'
#define FILE_COPY         '3'
#define FILE_DELETE       '4'
#define FILE_LIST         '5'
#define MEMORY_STATS	    '6'



int main()
{
    uint8_t returnStatus,sdcardType,option;
    char ch,sourceFileName[12],destFileName[12], str_buff[20] ;
    fileConfig_st *srcFilePtr,*destFilePtr;
    fileInfo fileList;
    uint32_t totalMemory,freeMemory;
	
	 
   init_UART1() ; 
   init_UART0() ;
    returnStatus = SD_Init(&sdcardType);
		UART0_str("\n\r\n\r***********Welcome to Embedded Systems (EC702): Special Assignment************** \r\n\r\n");
			UART0_str("\n\r\n\r                 SD Card Interface using FAT32 and SPI            \r\n\r\n");
			UART0_str("\r\nBY: \n\rAditi Bhatnagar  (16BEC008) - A1 Batch\r\n");
			UART0_str("Pranjal Shrivastava (16BEC020) - A2 Batch\r\n\r\n");
	
    if(returnStatus)
    {
        if(returnStatus == SDCARD_NOT_DETECTED)
        {
            UART0_str("\n\r SD card not detected..");
        }
        else if(returnStatus == SDCARD_INIT_FAILED)
        {
            UART0_str("\n\r Card Initialization failed..");
        }
        else if(returnStatus == SDCARD_FAT_INVALID)
        {
            UART0_str("\n\r Invalid Fat filesystem");
        }
        while(1);
    }
    else
    {
        UART0_str("\n\rSD Card Detected!");
    }

		
		
    while(1)
    {
        UART0_str("\n\r\n\rPress any key to continue");
        UART0_RxChar();
        UART0_str("\n\r\n\r------File options---------\n\r");
        UART0_str("1: Read File \n\r");
        UART0_str("2: Write File \n\r");
        UART0_str("3: File Copy \n\r");
        UART0_str("4: Delete File \n\r");
        UART0_str("5: Get File List \n\r");
        UART0_str("6: Card Memory Usage \n\r");
        UART0_str("--------------------------- \n\r");
        UART0_str("Choose one of the options: \n\r");


        do
        {
            option = UART0_RxChar();
        }while((option<'1') || (option>'6'));

        UART0_Tx(option);
        UART0_str("\n\r\n\r");

        switch(option)
        {

        case FILE_READ: /* Read a File */
            UART0_str("\n\rEnter File name max 11 chars with file extension: ");
            UART0_RxString(sourceFileName);

            srcFilePtr = FILE_Open(sourceFileName,READ,&returnStatus);
            if(srcFilePtr == 0)
            {
							UART0_str("\n\rFAILED : Couldnt open File");
            }
            else
            {
                 UART0_str("File Content: ");
                while(1)
                {
                    ch = FILE_GetCh(srcFilePtr);
                    if(ch == EOF)
                        break;
                    UART0_Tx(ch);
                }

                FILE_Close(srcFilePtr);
            }
            break;



        case FILE_WRITE: /* Write to a File */
            UART0_str("\n\rEnter File name max 11 chars with file extension: ");
            UART0_RxString(sourceFileName);

            srcFilePtr = FILE_Open(sourceFileName,WRITE,&returnStatus);
            if(srcFilePtr == 0)
            {
                UART0_str("\n\rFile Opening Failed");
            }
            else
            {
                UART0_str("\n\rEnter text ending with '>' :");
                while(1)
                {
                    ch = UART0_RxChar();
                    if(ch == '>')
                    {
                        FILE_PutCh(srcFilePtr,EOF);
                        break;
                    }
                    else
                    {   UART0_Tx(ch);
                    FILE_PutCh(srcFilePtr,ch);
                    }
                }
                UART0_str("\n\rData saved, closing the file.");
                FILE_Close(srcFilePtr);
            }
            break;



        case FILE_COPY: /* Copy a File  */
            UART0_str("\n\rEnter Source File name max 11 chars with file extension: ");
            UART0_RxString(sourceFileName);

            srcFilePtr = FILE_Open(sourceFileName,READ,&returnStatus);
            if(srcFilePtr == 0)
            {
                UART0_str("\n\rCan't Open Source File");
            }
            else
            {
                UART0_str("\n\rEnter Destination File name max 11 chars with file extension: ");
                UART0_RxString(destFileName);

                destFilePtr = FILE_Open(destFileName,WRITE,&returnStatus);

                if(destFilePtr == 0)
                {
                    UART0_str("\n\rCan't Open Destination File");

                }
                else
                {
                   UART0_str("\n\rCopying...");
                    while(1)
                    {
                        ch = FILE_GetCh(srcFilePtr);
                        FILE_PutCh(destFilePtr,ch);
                        if(ch == EOF)
                        {
                            break;
                        }
                    }
                    UART0_str("\n\rCopy Finished..");
                    FILE_Close(destFilePtr);
                }

                FILE_Close(srcFilePtr);
            }
            break;



        case FILE_DELETE: // Delete a file
            UART0_str("\n\rEnter File name to be deleted max 11 chars including file type: ");
            UART0_RxString(sourceFileName);

            UART0_str("\n\rDeleting File... ");
            returnStatus = FILE_Delete(sourceFileName);
            if(returnStatus == FAT32_FILE_OPENED_CANNOT_BE_DELETED)
            {
                UART0_str("\n\rFile is cannot be deleted, please close it properly.");
            }
            else
            {
                UART0_str("\n\rDone! File Deleted");
            }  			
            break;



        case FILE_LIST: // File names with size
						
						memset(str_buff,0,20) ; 
            while(1)
            {
                returnStatus = FILE_GetList(&fileList);
                if(returnStatus != FAT32_END_OF_FILE_LIST)
                {
										sprintf(str_buff,"\n\r%s   %d bytes",fileList.fileName,fileList.fileSize);
										UART0_str(str_buff) ;
                }
                else
                {
                    break;
                }
            }
            break;


        case MEMORY_STATS: // SD CARD memory Usage in Bytes in bytes
						memset(str_buff,0,100) ; 
						UART0_str("\n\rMemory Stats are being calculated...Please be Patient :)");
            FILE_GetMemoryStatics(&totalMemory,&freeMemory);
						sprintf(str_buff,"\n\rTotal memory: %u bytes\r\nFree memory: %u bytes",totalMemory,freeMemory);
            UART0_str(str_buff) ; 
            break;

        }// End of switch
    }// End of while
}// End of main

