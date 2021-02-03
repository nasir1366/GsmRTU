//*****************************************************************************
//
// File Name	: 'lcd_lib.c'
// Title		: 8 and 4 bit LCd interface
// Author		: Scienceprog.com - Copyright (C) 2007
// Created		: 2007-03-29
// Revised		: 2007-08-08
// Version		: 1.0
// Target MCU	: Atmel AVR series
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************
#include "my_delay.h"
#include "lcdlib.h"


uint8_t lcdprocess;

void LCDport(uint8_t);

//define 8 custom LCD chars
const uint8_t LcdCustomChar[]=
{
  0,0,4,14,31,0,0,0, // 0.up arrow
  0,0,31,14,4,0,0,0, // 1.down arrow
  0,2,6,14,6,2,0,0, // 2.rewind arrow
  0,8,12,14,12,8,0,0,  // 3.fast-forward arrow
  
  4,14,21,4,4,4,4,0, // 4.up pointer
  4,4,4,4,21,14,4,0, // 5.down pointer
  0,4,8,31,8,4,0,0, // 6.rewind pointer
  //0,4,2,31,2,4,0,0 // 7.fast-forward pointer
  28,20,28,0,0,0,0 //centigrade dot 
    
};

void LCDport(uint8_t data){
  
#ifdef LCD_4bit
	
	
	if((data & (1<<7))==(1<<7)) LCD_D7_PORT |= LCD_D7_PIN;	
	else LCD_D7_PORT &= ~LCD_D7_PIN;
	
	if((data & (1<<6))==(1<<6)) LCD_D6_PORT |= LCD_D6_PIN;
	else LCD_D6_PORT &= ~LCD_D6_PIN;
	
	if((data & (1<<5))==(1<<5)) LCD_D5_PORT |= LCD_D5_PIN;
	else LCD_D5_PORT &= ~LCD_D5_PIN;
	
	if((data & (1<<4))==(1<<4)) LCD_D4_PORT |= LCD_D4_PIN;
	else LCD_D4_PORT &= ~LCD_D4_PIN;
			
  
#else
  
  if((data & (1<<7))==(1<<7)) LCD_D7_PORT |= LCD_D7_PIN;
  else LCD_D7_PORT &= ~LCD_D7_PIN;
  
  if((data & (1<<6))==(1<<6)) LCD_D6_PORT |= LCD_D6_PIN;
  else LCD_D6_PORT &= ~LCD_D6_PIN;
  
  if((data & (1<<5))==(1<<5)) LCD_D5_PORT |= LCD_D5_PIN;
  else LCD_D5_PORT &= ~LCD_D5_PIN;
  
  if((data & (1<<4))==(1<<4)) LCD_D4_PORT |= LCD_D4_PIN;
  else LCD_D4_PORT &= ~LCD_D4_PIN;
  
  if((data & (1<<3))==(1<<3)) LCD_D3_PORT |= LCD_D3_PIN;
  else LCD_D3_PORT &= ~LCD_D3_PIN;
  
  if((data & (1<<2))==(1<<2)) LCD_D2_PORT |= LCD_D2_PIN;
  else LCD_D2_PORT &= ~LCD_D2_PIN;
  
  if((data & (1<<1))==(1<<1)) LCD_D1_PORT |= LCD_D1_PIN;
  else LCD_D1_PORT &= ~LCD_D1_PIN;
  
  if((data & (1<<0))==(1<<0)) LCD_D0_PORT |= LCD_D0_PIN;
  else LCD_D0_PORT &= ~LCD_D0_PIN;
  
  
#endif
}

void LCDsendChar(uint8_t ch)		//Sends Char to LCD
{
  
#ifdef LCD_4bit
  //4 bit part
  if(lcdprocess==0){
		lcdprocess=1;
		LCDport(ch&0xF0);
		LCD_RS_PORT|=LCD_RS_PIN;
		delayms(1);
		LCD_E_PORT|=LCD_E_PIN;		
		delayms(1);
		LCD_E_PORT&=~LCD_E_PIN;	
		delayus(200);
		LCD_RS_PORT&=~LCD_RS_PIN;
		delayms(1);
		LCDport((ch&0x0F)<<4);
		LCD_RS_PORT|=LCD_RS_PIN;
		delayus(200);
		LCD_E_PORT|=LCD_E_PIN;		
		delayms(1);
		LCD_E_PORT&=~LCD_E_PIN;
		delayus(200);
		LCD_RS_PORT&=~LCD_RS_PIN;
		delayms(1);
		lcdprocess=0;
	}
#else
  //8 bit part
  LCDport(ch);
  LCD_RS_PORT|=LCD_RS_PIN;
  LCD_E_PORT|=LCD_E_PIN;		
  delayms(1);
  LCD_E_PORT&=~LCD_E_PIN;
  LCD_RS_PORT&=~LCD_RS_PIN;
  delayms(1);
#endif
}
void LCDsendCommand(uint8_t cmd)	//Sends Command to LCD
{
#ifdef LCD_4bit	
  //4 bit part
	if(lcdprocess==0){
		lcdprocess=1;
		LCDport(cmd&0xF0);
		LCD_E_PORT|=LCD_E_PIN;		
		delayms(1);
		LCD_E_PORT&=~LCD_E_PIN;
		delayms(1);
		LCDport((cmd&0x0F)<<4);	
		LCD_E_PORT|=LCD_E_PIN;		
		delayms(1);
		LCD_E_PORT&=~LCD_E_PIN;
		delayms(1);
		lcdprocess=0;
	}
#else
  //8 bit part
  LCDport(cmd);
  LCD_E_PORT|=LCD_E_PIN;		
  delayms(1);
  LCD_E_PORT&=~LCD_E_PIN;
  delayms(1);	
#endif
}
void LCDinit(void)//Initializes LCD
{
#ifdef LCD_4bit	
  //4 bit part
  delayms(15);
  
  //---------one------
  LCD_D4_PORT |= LCD_D4_PIN;
  LCD_D5_PORT |= LCD_D5_PIN;
  LCD_D6_PORT &= ~LCD_D6_PIN;
  LCD_D7_PORT &= ~LCD_D7_PIN;
  
 // LCD_RW_PORT &= ~LCD_RW_PIN;
  LCD_RS_PORT &= ~LCD_RS_PIN;	
  
  LCD_E_PORT |= LCD_E_PIN;
  delayms(1);
  LCD_E_PORT &= ~LCD_E_PIN;
  delayms(1);
  //-----------two-----------
  LCD_D4_PORT |= LCD_D4_PIN;
  LCD_D5_PORT |= LCD_D5_PIN;
  LCD_D6_PORT &= ~LCD_D6_PIN;
  LCD_D7_PORT &= ~LCD_D7_PIN;
  
  //LCD_RW_PORT &= ~LCD_RW_PIN;
  LCD_RS_PORT &= ~LCD_RS_PIN;
  
  LCD_E_PORT |= LCD_E_PIN;
  delayms(1);
  LCD_E_PORT &= ~LCD_E_PIN;
  delayms(1);
  //-------three-------------
  LCD_D4_PORT &= ~LCD_D4_PIN;
  LCD_D5_PORT |=  LCD_D5_PIN;
  LCD_D6_PORT &= ~LCD_D6_PIN;
  LCD_D7_PORT &= ~LCD_D7_PIN;
  
 // LCD_RW_PORT &= ~LCD_RW_PIN;
  LCD_RS_PORT &= ~LCD_RS_PIN;
  
  LCD_E_PORT |= LCD_E_PIN;
  delayms(1);
  LCD_E_PORT &= ~LCD_E_PIN;
  delayms(1);
  //--------4 bit--dual line---------------
  LCDsendCommand(0x28);
  //-----increment address, invisible cursor shift------
  LCDsendCommand(0x0C);
  //init 8 custom chars
  uint8_t ch=0, chn=0;
  while(ch<64)
  {
    LCDdefinechar((LcdCustomChar+ch),chn++);
    ch=ch+8;
  }
  
  
#else
  //8 bit part
  delayms(15);
  
  //---------one------
  LCD_D0_PORT &= ~LCD_D0_PIN;
  LCD_D1_PORT &= ~LCD_D1_PIN;
  LCD_D2_PORT &= ~LCD_D2_PIN;
  LCD_D3_PORT &= ~LCD_D3_PIN;
  LCD_D4_PORT |=  LCD_D4_PIN;
  LCD_D5_PORT |=  LCD_D5_PIN;
  LCD_D6_PORT &= ~LCD_D6_PIN;
  LCD_D7_PORT &= ~LCD_D7_PIN;
  
  LCD_RW_PORT &= ~LCD_RW_PIN;
  LCD_RS_PORT &= ~LCD_RS_PIN;
  
  LCD_E_PORT |= LCD_E_PIN;
  delayms(1);
  LCD_E_PORT &= ~LCD_E_PIN;
  delayms(1);
  //-----------two-----------
  LCD_E_PORT |= LCD_E_PIN;
  delayms(1);
  LCD_E_PORT &= ~LCD_E_PIN;
  delayms(1);
  //-------three-------------
  LCD_E_PORT |= LCD_E_PIN;
  delayms(1);
  LCD_E_PORT &= ~LCD_E_PIN;
  delayms(1);
  //--------8 bit dual line----------
  LCD_D3_PORT |=  LCD_D3_PIN;
  LCD_D4_PORT |=  LCD_D4_PIN;
  LCD_D5_PORT |=  LCD_D5_PIN;
  
  LCD_E_PORT |= LCD_E_PIN;
  delayms(1);
  LCD_E_PORT &= ~LCD_E_PIN;
  delayms(1);
  //-----increment address, invisible cursor shift------
  LCD_D2_PORT |=  LCD_D2_PIN;
  LCD_D3_PORT |=  LCD_D3_PIN;
  LCD_D4_PORT &= ~LCD_D4_PIN;
  LCD_D5_PORT &= ~LCD_D5_PIN;
  
  LCD_E_PORT |= LCD_E_PIN;
  delayms(1);
  LCD_E_PORT &= ~LCD_E_PIN;
  delayms(1);
  //init custom chars
  // 	uint8_t ch=0, chn=0;
  // 	while(ch<64)
  // 	{
  // 		LCDdefinechar((LcdCustomChar+ch),chn++);
  // 		ch=ch+8;
  // 	}
  
#endif
}			
void LCDclr(void)				//Clears LCD
{
  LCDsendCommand(1<<LCD_CLR);
  delayms(10);
}
void LCDhome(void)			//LCD cursor home
{
  LCDsendCommand(1<<LCD_HOME);
  delayms(10);
}
void LCDputs(char *s)
/* print string on lcd */
{
  while (*s)
  {
    
    if(*s=='\n') 
      LCDGotoXY(0,1);
    
    else
      LCDsendChar(*s);
    
    s++;
  }
}
void LCDGotoXY(uint8_t x, uint8_t y)	//Cursor to X Y position
{
  register uint8_t DDRAMAddr;
  // remap lines into proper order
  switch(y)
  {
  case 0: DDRAMAddr = LCD_LINE0_DDRAMADDR+x; break;
  case 1: DDRAMAddr = LCD_LINE1_DDRAMADDR+x; break;
  case 2: DDRAMAddr = LCD_LINE2_DDRAMADDR+x; break;
  case 3: DDRAMAddr = LCD_LINE3_DDRAMADDR+x; break;
  default: DDRAMAddr = LCD_LINE0_DDRAMADDR+x;
  }
  // set data address
  LCDsendCommand(1<<LCD_DDRAM | DDRAMAddr);
  
}
//Copies string from flash memory to LCD at x y position
//const uint8_t welcomeln1[] PROGMEM="AVR LCD DEMO\0";
//CopyStringtoLCD(welcomeln1, 3, 1);	
void LCDputsf(char const * FlashLoc)
{
  uint8_t i;
  /*LCDGotoXY(x,y);*/
  for(i=0;FlashLoc[i];i++)
  {
    if(FlashLoc[i]=='\n') 
      LCDGotoXY(0,1);
    
    else
      LCDsendChar(FlashLoc[i]);	
    
    
  }
}
//defines char symbol in CGRAM
/*
const uint8_t backslash[] PROGMEM= 
{
0b00000000,//back slash
0b00010000,
0b00001000,
0b00000100,
0b00000010,
0b00000001,
0b00000000,
0b00000000
};
LCDdefinechar(backslash,0);
*/
void LCDdefinechar(const uint8_t *pc,uint8_t char_code){
  uint8_t a, pcc;
  uint16_t i;
  a=(char_code<<3)|0x40;
  for (i=0; i<8; i++){
    pcc=pc[i];
    LCDsendCommand(a++);
    LCDsendChar(pcc);
  }
}

void LCDshiftLeft(uint8_t n)	//Scrol n of characters Right
{
  for (uint8_t i=0;i<n;i++)
  {
    LCDsendCommand(0x1E);
  }
}
void LCDshiftRight(uint8_t n)	//Scrol n of characters Left
{
  for (uint8_t i=0;i<n;i++)
  {
    LCDsendCommand(0x18);
  }
}
void LCDcursorOn(void) //displays LCD cursor
{
  LCDsendCommand(0x0E);
}
void LCDcursorOnBlink(void)	//displays LCD blinking cursor
{
  LCDsendCommand(0x0F);
}
void LCDcursorOFF(void)	//turns OFF cursor
{
  LCDsendCommand(0x0C);
}
void LCDblank(void)		//blanks LCD
{
  LCDsendCommand(0x08);
}
void LCDvisible(void)		//Shows LCD
{
  LCDsendCommand(0x0C);
}
void LCDcursorLeft(uint8_t n)	//Moves cursor by n poisitions left
{
  for (uint8_t i=0;i<n;i++)
  {
    LCDsendCommand(0x10);
  }
}
void LCDcursorRight(uint8_t n)	//Moves cursor by n poisitions left
{
  for (uint8_t i=0;i<n;i++)
  {
    LCDsendCommand(0x14);
  }
}
void LCDprogressBar(uint8_t progress, uint8_t maxprogress, uint8_t length)
{
  uint8_t i;
  uint16_t pixelprogress;
  uint8_t c;
  
  // draw a progress bar displaying (progress / maxprogress)
  // starting from the current cursor position
  // with a total length of "length" characters
  // ***note, LCD chars 0-5 must be programmed as the bar characters
  // char 0 = empty ... char 5 = full
  
  // total pixel length of bargraph equals length*PROGRESSPIXELS_PER_CHAR;
  // pixel length of bar itself is
  pixelprogress = ((progress*(length*PROGRESSPIXELS_PER_CHAR))/maxprogress);
  
  // print exactly "length" characters
  for(i=0; i<length; i++)
  {
    // check if this is a full block, or partial or empty
    // (u16) cast is needed to avoid sign comparison warning
    if( ((i*(uint16_t)PROGRESSPIXELS_PER_CHAR)+5) > pixelprogress )
    {
      // this is a partial or empty block
      if( ((i*(uint16_t)PROGRESSPIXELS_PER_CHAR)) > pixelprogress )
      {
        // this is an empty block
        // use space character?
        c = 0;
      }
      else
      {
        // this is a partial block
        c = pixelprogress % PROGRESSPIXELS_PER_CHAR;
      }
    }
    else
    {
      // this is a full block
      c = 5;
    }
    
    // write character to display
    LCDsendChar(c);
  }
  
}

//------------------------------------------------
void LcdPwrOn(){
	GPIOB->ODR |= GPIO_PIN_6;
		
}

void LcdPwrOff(){
	
	GPIOB->ODR &= ~(GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10|GPIO_PIN_6);
	
}
