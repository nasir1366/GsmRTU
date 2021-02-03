/****************************************Copyright (c)**************************************************
********************************************************************************************************/

#ifndef __F016BDRV_H 
#define __F016BDRV_H

#include "includes.h"

#define U8 uint8_t
#define U16 uint16_t
#define U32 uint32_t

#define NO_OF_SECTORS 0x3E00
#define ERASE_BLOCK_SIZE 0x200

#define FAT_OFFSET  0//0x400


#define M45P_WriteEnable		    0x06 
#define M45P_WriteDisable		    0x04 
#define M45P_ReadStatusReg		  0x05 
//#define M45P_WriteStatusReg	  0x01 
#define M45P_ReadData			      0x03 
#define M45P_FastReadData		    0x0B 
//#define M45P_FastReadDual		  0x3B 
#define M45P_PageProgram		    0x02 
#define M45P_PageErase			    0xDB 
#define M45P_SectorErase		    0x20  
//#define M45P_SectorErase		    0xD8  
#define M45P_ChipErase			    0xC7 
#define M45P_PowerDown			    0xB9 
#define M45P_ReleasePowerDown	  0xAB 
//#define M45P_ManufactDeviceID	0x90 
#define M45P_JedecDeviceID		  0x9F 

#define CE_Low()      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define CE_High()     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)

#define FLASH_HOLD_ENABLE      //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET) //PC7, LOW=HOLD
#define FLASH_HOLD_DISABLE      //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET) //PC7, HIGH=DISABLE
#define FLASH_WP_ENABLE      ///HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET) //PC8, LOW=WRITE PROTECT
#define FLASH_WP_DISABLE     // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET) //PC8, HIGH=DISABLE

#define HOLD1_Low()   
#define HOLD1_High()  

#define HOLD2_Low()   
#define HOLD2_High()  

U32  SPI_Flash_ReadID(void);  	
void SPI_Flash_Read(U8* pBuffer,U32 ReadAddr,U16 NumByteToRead);  
void SPI_Flash_Write(U8* pBuffer,U32 WriteAddr,U16 NumByteToWrite);
void SPI_Flash_Write_Page(U8* pBuffer,U32 WriteAddr,U16 NumByteToWrite);
void SPI_FLASH_PageErase(U32 PageAddr);
void SPI_FLASH_Init(void);
void SPI_Flash_Erase_Chip(void);
void SPI_FLASH_Write_Enable(void);   

#endif
