#include "includes.h"
#include "serialflash.h"
#include "my_delay.h"

extern SPI_HandleTypeDef hspi1;


/* Private typedef -----------------------------------------------------------*/
#define SPI_FLASH_PageSize    0x210

/* Private define ------------------------------------------------------------*/
#define READ       0xD2  /* Read from Memory instruction */
#define WRITE      0x82  /* Write to Memory instruction */

#define RDID       0x9F  /* Read identification */
#define RDSR       0xD7  /* Read Status Register instruction  */

#define SE         0x7C  /* Sector Erase instruction */
#define PE         0x81  /* Page Erase instruction */

#define RDY_Flag   0x80  /* Ready/busy(1/0) status flag */

#define Dummy_Byte 0xA5



/* SSPxSR - bit definitions. */
#define TFE     0x01
#define TNF     0x02
#define RNE     0x04
#define RFF     0x08
#define BSY     0x10

//void SPI_Flash_Init(void);
static U8	  SPI_Flash_ReadSR(void);    
//void  SPI_FLASH_Write_SR(U8 sr); 
void SPI_FLASH_Write_Enable(void); 
static void SPI_FLASH_Write_Disable(void);
static void SPI_Flash_Erase_Sector(U32 Dst_Addr);
static void SPI_Flash_Wait_Busy(void);           
static void SPI_Flash_PowerDown(void);           
static void SPI_Flash_WAKEUP(void);			  
static U8   SPIx_ReadWriteByte(U8 TxData);


void delay__us(int count)
{
  while(count--);
}
//--------------------------------
void  SPI_FLASH_Init(void)
{  
	/* Initialize and enable the SSP Interface module. */
  FLASH_HOLD_DISABLE;
	FLASH_WP_DISABLE;
	CE_High();
	
}
//--------------------------------
U8 SPIx_ReadWriteByte(U8 TxData)
{	
	uint8_t DR=0;
	
  HAL_SPI_TransmitReceive(&hspi1,&TxData,&DR,1,100);
	return DR;
}
//--------------------------------
U32 SPI_Flash_ReadID(void)
{
  U32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* Select the FLASH: Chip Select low */
	CE_Low();
 
  /* Send "RDID " instruction */
  SPIx_ReadWriteByte(M45P_JedecDeviceID);

  /* Read a byte from the FLASH */
  Temp0 = SPIx_ReadWriteByte(Dummy_Byte);

  /* Read a byte from the FLASH */
  Temp1 = SPIx_ReadWriteByte(Dummy_Byte);

  /* Read a byte from the FLASH */
  Temp2 = SPIx_ReadWriteByte(Dummy_Byte);
   
  /* Deselect the FLASH: Chip Select high */
  CE_High();				    
	
  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}
//--------------------------------
U8 SPI_Flash_ReadSR(void)   
{  
	U8 byte = 0;   
	CE_Low();                            
	SPIx_ReadWriteByte(M45P_ReadStatusReg);
	byte=SPIx_ReadWriteByte(0Xff);       
	CE_High();                           
	return byte;   
}
//--------------------------------
void SPI_FLASH_Write_Enable(void)   
{
	CE_Low();                            
  SPIx_ReadWriteByte(M45P_WriteEnable);
	CE_High();                           
}
//--------------------------------
void SPI_FLASH_Write_Disable(void)   
{  
	CE_Low();                            
  SPIx_ReadWriteByte(M45P_WriteDisable);
	CE_High();                           
}
//--------------------------------
void SPI_Flash_Read(U8* pBuffer,U32 ReadAddr,U16 NumByteToRead)   
{ 
 	U8 i=0xFF;    												    
	CE_Low();                            
  SPIx_ReadWriteByte(M45P_ReadData);       
  SPIx_ReadWriteByte((U8)((ReadAddr)>>16));
  SPIx_ReadWriteByte((U8)((ReadAddr)>>8));   
  SPIx_ReadWriteByte((U8)ReadAddr);  
	
//  for(i=0;i<NumByteToRead;i++)
//	{ 
//   pBuffer[i]=SPIx_ReadWriteByte(0XFF); 
//  }
	
	HAL_SPI_Receive(&hspi1,pBuffer,NumByteToRead,1000);
	CE_High();                            
}
//--------------------------------
void SPI_Flash_Wait_Busy(void)   
{
 while ((SPI_Flash_ReadSR() & 0x01) != 0); 
} 
//--------------------------------
void SPI_Flash_Write_Page(U8* pBuffer,U32 WriteAddr,U16 NumByteToWrite)
{
// 	U16 i;  
  SPI_FLASH_Write_Enable();                  //SET WEL 
	CE_Low();                            
  SPIx_ReadWriteByte(M45P_PageProgram);      
  SPIx_ReadWriteByte((U8)((WriteAddr)>>16)); 
  SPIx_ReadWriteByte((U8)((WriteAddr)>>8));   
  SPIx_ReadWriteByte((U8)WriteAddr);   
//  for(i=0;i<NumByteToWrite;i++)SPIx_ReadWriteByte(pBuffer[i]);
	HAL_SPI_Transmit(&hspi1,pBuffer,NumByteToWrite,1000);
	
	CE_High();                    
	SPI_Flash_Wait_Busy();				
}
//--------------------------------
void SPI_Flash_Write_NoCheck(U8* pBuffer,U32 WriteAddr,U16 NumByteToWrite)   
{ 			 		 
	U16 pageremain;	   
	pageremain=256-WriteAddr%256; 
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;
	while(1)
	{	   
	 SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
	 if(NumByteToWrite==pageremain)break;
	 else //NumByteToWrite>pageremain
	 {
		pBuffer+=pageremain;
		WriteAddr+=pageremain;	

		NumByteToWrite-=pageremain;			  
		if(NumByteToWrite>256)pageremain=256;
		else pageremain=NumByteToWrite; 	  
	 }
	};	    
}
//--------------------------------
void SPI_Flash_Erase_Sector(U32 Dst_Addr)   
{   
	Dst_Addr *= 4096;
  SPI_FLASH_Write_Enable();                  //SET WEL 	 
  CE_Low();                           
  SPIx_ReadWriteByte(M45P_SectorErase);     
  SPIx_ReadWriteByte((U8)((Dst_Addr)>>16)); 
  SPIx_ReadWriteByte((U8)((Dst_Addr)>>8));   
  SPIx_ReadWriteByte((U8)Dst_Addr);  
	CE_High();                          
  SPI_Flash_Wait_Busy();   				  
}
//--------------------------------
void SPI_Flash_Erase_Chip(void)   
{   
	SPI_FLASH_Write_Enable();                  //SET WEL 	 
  CE_Low();                           
  SPIx_ReadWriteByte(M45P_ChipErase);     
  CE_High();
	/*
  while ((SPI_Flash_ReadSR() & 0x01) != 0)
  {
   LPC_WDT->WDFEED = 0xAA;		// Feeding sequence //
   LPC_WDT->WDFEED = 0x55;  
  };
	*/
  SPI_Flash_Wait_Busy();   				  
}
//--------------------------------
void SPI_FLASH_PageErase(U32 PageAddr)
{
  /* Enable the write access to the FLASH */
  SPI_FLASH_Write_Enable();

  /* Sector Erase */
  /* Select the FLASH: Chip Select low */
  CE_Low();             
  /* Send Sector Erase instruction */
  SPIx_ReadWriteByte(M45P_PageErase);
  /* Send SectorAddr high nibble address byte */
  SPIx_ReadWriteByte((PageAddr & 0xFF0000) >> 16);
  /* Send SectorAddr medium nibble address byte */
  SPIx_ReadWriteByte((PageAddr & 0xFF00) >> 8);
  /* Send SectorAddr low nibble address byte */
  SPIx_ReadWriteByte(PageAddr & 0xFF);
  /* Deselect the FLASH: Chip Select high */
  CE_High();            

  /* Wait the end of Flash writing */
  SPI_Flash_Wait_Busy();
}
//--------------------------------
  		   
static U8 SPI_FLASH_BUF[ERASE_BLOCK_SIZE+10];

void SPI_Flash_Write(U8* pBuffer,U32 WriteAddr,U16 NumByteToWrite)   
{ 
//	U8 SPI_FLASH_BUF[ERASE_BLOCK_SIZE];
	U32 secpos;
	U16 secoff;
	U16 secremain;	   
 	U16 i;    

	secpos = WriteAddr / ERASE_BLOCK_SIZE;
	secoff = WriteAddr % ERASE_BLOCK_SIZE;
	secremain = ERASE_BLOCK_SIZE - secoff;

	if(NumByteToWrite <= secremain)
	 secremain = NumByteToWrite;
	while(1) 
	{	
	 SPI_Flash_Read(SPI_FLASH_BUF,secpos * ERASE_BLOCK_SIZE,ERASE_BLOCK_SIZE);
	 for(i = 0;i < secremain;i++)
	 {
		if(SPI_FLASH_BUF[secoff+i] != 0XFF)
		 break;
	 }
	 if(i < secremain)
	 {
		SPI_Flash_Erase_Sector(secpos);
		for(i = 0;i < secremain;i++)	   
		{
		 SPI_FLASH_BUF[i + secoff] = pBuffer[i];	  
		}
		SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos * ERASE_BLOCK_SIZE,ERASE_BLOCK_SIZE);

	 }
	 else 
		SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);
	 if(NumByteToWrite == secremain)
		break;
	 else
	 {
		secpos++;
		secoff = 0;
   	pBuffer += secremain;  
		WriteAddr += secremain;
		NumByteToWrite -= secremain;			
		if(NumByteToWrite > ERASE_BLOCK_SIZE)
		 secremain = ERASE_BLOCK_SIZE;
		else 
		 secremain = NumByteToWrite;			
	 }	 
	};	 	 
}
//--------------------------------
void SPI_Flash_PowerDown(void)   
{ 
  CE_Low();                           
  SPIx_ReadWriteByte(M45P_PowerDown); 
	CE_High();                            
  delay__us(3);                        
}

//--------------------------------
void SPI_Flash_WAKEUP(void)   
{  
  CE_Low();                            
  SPIx_ReadWriteByte(M45P_ReleasePowerDown);   //  send M45P_PowerDown command 0xAB    
	CE_High();                            
  delay__us(3);                        
}

//--------------------------------









