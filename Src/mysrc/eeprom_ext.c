
#include "includes.h"
#include "defines.h"
#include "my_delay.h"
#include "eeprom_ext.h"
#include "global_var.h"
#include "main_func.h"
#include "serialflash.h"

void reset_i2c(){
//	SET_BIT((&hi2c1)->Instance->CR1, I2C_CR1_STOP);
//	delayms(1);
	
//	SET_BIT((&hi2c1)->Instance->CR1, I2C_CR1_SWRST);
//	delayms(1);
//	CLEAR_BIT((&hi2c1)->Instance->CR1, I2C_CR1_SWRST);
//	delayms(1);
//	flag.i2c_busy=0;
	
	HAL_NVIC_SystemReset();
}
/************************************* write to eeprom**********************************************/

char write_exteeprom_byte(unsigned char data,unsigned long address){
 
	
	SPI_Flash_Write(&data,address,1);
//  i2c_pwr_on;
//  iic_pwr_count=0;
//  delayms(30);
//  
//	flag.i2c_busy=1;
    
//  if(HAL_I2C_Mem_Write(&hi2c1,address_bus,(uint16_t)address,I2C_MEMADD_SIZE_16BIT,&data,1,i2c_timeout)!=HAL_OK){
//    /* Generate Stop */
//    reset_i2c();
//    return 0;
//  }
//  delayms(10);
//  flag.i2c_busy=0;
	
  return 1;
}
//------------------------------------------------------------------------------
char write_exteeprom_word(uint16_t data,unsigned long address){
	
	SPI_Flash_Write((uint8_t*)(&data),address,2);
  
//  i2c_pwr_on;
//  iic_pwr_count=0;
//  delayms(30);
//    
//  flag.i2c_busy=1;
//  if(HAL_I2C_Mem_Write(&hi2c1,address_bus,(uint16_t)address,I2C_MEMADD_SIZE_16BIT,(uint8_t*)(&data),2,i2c_timeout)!=HAL_OK){
//    
//    /* Generate Stop */
//    reset_i2c();
//    return 0;
//    
//  }  
//  delayms(10);
//  flag.i2c_busy=0;
  return 1;
}
//--------------------------------------------------------------------------
char write_exteeprom_dword(unsigned long data,unsigned long address){
	
	SPI_Flash_Write((uint8_t*)(&data),address,4);
  
//  i2c_pwr_on;
//  iic_pwr_count=0;
//  delayms(30);
//  flag.i2c_busy=1; 
//  if(HAL_I2C_Mem_Write(&hi2c1,address_bus,(uint16_t)address,I2C_MEMADD_SIZE_16BIT,(uint8_t*)&data,4,i2c_timeout)!=HAL_OK){
//    /* Generate Stop */
//    reset_i2c();
//    return 0;
//  }  
    
//  delayms(10);
//  flag.i2c_busy=0;
  return 1;
}
//-----------------------------------------------------------------
char write_exteeprom_float(float data,unsigned long address){
  char *temp=NULL;	
	
  temp=(char*)&data;
	
	SPI_Flash_Write((uint8_t*)(temp),address,4);
	
//  i2c_pwr_on;
//  iic_pwr_count=0;
//  delayms(30);
//  
//  flag.i2c_busy=1;
//  if(HAL_I2C_Mem_Write(&hi2c1,address_bus,(uint16_t)address,I2C_MEMADD_SIZE_16BIT,(uint8_t*)temp,4,i2c_timeout)!=HAL_OK){
//    /* Generate Stop */
//    reset_i2c();
//    return 0;
//  }
//  delayms(10);
//	flag.i2c_busy=0;
  return 1;
}
//----------------------------------------------------------------------------------------------
char write_exteeprom_ary(char *data_write,unsigned char cnt,unsigned long address){
	
	SPI_Flash_Write((uint8_t*)data_write,address,cnt);
  
//  i2c_pwr_on;
//  iic_pwr_count=0;
//  delayms(30);
//    
//  flag.i2c_busy=1;
//  for(i=0;i<cnt;i++){
//    if(HAL_I2C_Mem_Write(&hi2c1,address_bus,(uint16_t)(address+i),I2C_MEMADD_SIZE_16BIT,(uint8_t*)(data_write+i),1,i2c_timeout)!=HAL_OK){
//      /* Generate Stop */
//      reset_i2c();
//      return 0;
//    }
//    delayms(10);
//  }
//  flag.i2c_busy=0;
  return 1;
}
//************************************* read from eeprom**********************************************

unsigned char read_exteeprom_byte(unsigned long address){
  static unsigned char data_read ;
	
	SPI_Flash_Read(&data_read,address,1);
  
//  i2c_pwr_on;
//  iic_pwr_count=0;
//  delayms(30);
//  flag.i2c_busy=1;
//  if(HAL_I2C_Mem_Read(&hi2c1,address_bus,(uint16_t)address,I2C_MEMADD_SIZE_16BIT,&data_read,1,i2c_timeout)!=HAL_OK)
//  {
//    /* Generate Stop */
//    reset_i2c();
//    return 0;
//  }
//  flag.i2c_busy=0;
  return data_read;
}
//----------------------------------------------------------------------------------
uint16_t read_exteeprom_word(unsigned long address){
  
  static uint16_t data_read ;
//  uint8_t* point=NULL;
	
	SPI_Flash_Read((uint8_t*)(&data_read),address,2);
	
//  i2c_pwr_on;
//  iic_pwr_count=0;
//  delayms(30);
//  point=(uint8_t*)(&data_read);
//	
//	flag.i2c_busy=1;
//  if(HAL_I2C_Mem_Read(&hi2c1,address_bus,(uint16_t)address,I2C_MEMADD_SIZE_16BIT,point,1,i2c_timeout)!=HAL_OK){
//    /* Generate Stop */
//    reset_i2c();
//    return 0;
//  }
  
//  if(HAL_I2C_Mem_Read(&hi2c1,address_bus,(uint16_t)(address+1),I2C_MEMADD_SIZE_16BIT,point+1,1,i2c_timeout)!=HAL_OK){
//    /* Generate Stop */
//    reset_i2c();
//    return 0;
//  }
//  flag.i2c_busy=0;
  return data_read;
  
}
//----------------------------------------------------------------------------------
unsigned long read_exteeprom_dword(unsigned long address){
  
  unsigned long data_read=0 ;
	
	SPI_Flash_Read((uint8_t*)(&data_read),address,4);
  
//  i2c_pwr_on;
//  iic_pwr_count=0;
//  delayms(30);
//  flag.i2c_busy=1;
//  if(HAL_I2C_Mem_Read(&hi2c1,address_bus,(uint16_t)address,I2C_MEMADD_SIZE_16BIT,(uint8_t*)&data_read,4,i2c_timeout)!=HAL_OK){
//    /* Generate Stop */
//    reset_i2c();
//    return 0;
//  }
//  flag.i2c_busy=0;
  return data_read;
  
}

float read_exteeprom_float(unsigned long address){
  
  char *temp ;
  float data_read;
  temp=(char*)&data_read;
	
	SPI_Flash_Read((uint8_t*)(temp),address,4);
  
//  i2c_pwr_on;
//  iic_pwr_count=0;
//  delayms(30);
//  flag.i2c_busy=1;
//  if(HAL_I2C_Mem_Read(&hi2c1,address_bus,(uint16_t)address,I2C_MEMADD_SIZE_16BIT,temp,4,i2c_timeout)!=HAL_OK){
//    /* Generate Stop */
//    reset_i2c();
//    return 0;
//  }
  
//  flag.i2c_busy=0;
  return data_read;
  
}

void read_exteeprom_ary(char *data_read,unsigned char cnt,unsigned long address){
	
	
	SPI_Flash_Read((uint8_t*)data_read,address,cnt);
  
//  i2c_pwr_on;
//  iic_pwr_count=0;
//  delayms(30);
//  flag.i2c_busy=1;
//  if(HAL_I2C_Mem_Read(&hi2c1,address_bus,(uint16_t)address,I2C_MEMADD_SIZE_16BIT,(uint8_t*)data_read,cnt,i2c_timeout)!=HAL_OK){
//    /* Generate Stop */
//    reset_i2c();
//    
//  }
//  flag.i2c_busy=0;
  
}

//--------------------------------------------------------------------------------------------------------------------

void clear_eeprom(){
	
		
  uint16_t i;
	uint8_t temp[256];
	for(i=0;i<256;i++)temp[i]=0xFF;
	
	SPI_Flash_Write(temp,0,256);
	SPI_Flash_Write(temp,0x100,256);
	SPI_Flash_Write(temp,0x200,256);
	SPI_Flash_Write(temp,0x300,256);
	SPI_Flash_Write(temp,0x400,256);
	
//  
//  i2c_pwr_on;
//  iic_pwr_count=0;
//  delayms(30);
//  for(i=0;i<32;i++)buf[i]=0xFF;
//  flag.i2c_busy=1;
//  for(i=0;i<128;i++){
//    HAL_I2C_Mem_Write(&hi2c1,address_bus,i*32,I2C_MEMADD_SIZE_16BIT,buf,32,i2c_timeout);
//    delayms(10);
//  }
//	
//  
//	flag.i2c_busy=0;
}


