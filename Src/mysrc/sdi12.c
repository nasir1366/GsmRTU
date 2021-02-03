#include "includes.h"
#include "defines.h"
#include "global_var.h"
#include "my_delay.h"
#include "sdi12.h"
#include "main_func.h"

GPIO_InitTypeDef GPIO_InitStruct;

void sdi_dir_out(void){
	
	HAL_GPIO_DeInit(GPIOE,GPIO_PIN_8);
	
	GPIO_InitStruct.Pin = GPIO_PIN_8;
		
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
}

void sdi_dir_in(void){
	HAL_GPIO_DeInit(GPIOE,GPIO_PIN_8);
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);


}
//-------------------------------------------------------------------
uint8_t ReadPinTimeout(GPIO_TypeDef* port,unsigned char pin,unsigned char stat,unsigned int timeout){
unsigned int timer=0;
unsigned int dly=0;

  dly=timeout;
  TIM3->CNT=0;
  timer=0;
  
 
    while(timer<dly){
      tgl_wdt;
      if(stat==0){
        if(port->IDR & (1<<pin))return 0;  
      }
      else if((port->IDR & (1<<pin))==0)return 0;
           
      timer=TIM3->CNT;
        
    }
 
return 1;
}
//----------------------------------------------
uint8_t parity_check(char ch){
uint8_t p=0,i=0;

for(i=1;i<=64;i<<=1){
  if(ch & i)p++;
  
}
if((p%2)==0)return 0;
else return 1;

}
//--------------------------------------------------
uint8_t thali_readresp(void){
  uint16_t i=0,j=0;
  uint16_t d=0;
  
    
  while (d != 10){
    if(ReadPinTimeout(sdi_port,sdi_pin,0,15500)==1){
      return 1;
    }
    
    DelaytimerUS(416); 
    
    d=0;
    for(i=0;i<7;i++){
      
      DelaytimerUS(833);
      if(!sdi_pin_read) d|=(1<<i);
            
    }
    DelaytimerUS(833);
    DelaytimerUS(833);    
    
    thali_buf[j]=(char)d;
    j++;
    if(j>=19){
      
      return 1;
    }
  }   
  return 0;
}
//----------------------------------------------
void thali_sendchar(char ch){
  uint16_t i=0;
  
    
  ch |= (parity_check(ch)<<7);
  
  
  sdi_pin_set;
  DelaytimerUS(833);
      
  for(i=1;i<=128;i<<=1){
    
    if(ch & i)sdi_pin_reset;
    else sdi_pin_set;
    DelaytimerUS(833);
  }
  
  sdi_pin_reset;
  DelaytimerUS(833);
    
  
}
//----------------------
void thali_sendcmd(char *str){
  
  sdi_dir_out();
  sdi_pin_set;
  DelaytimerUS(13000);//wakeup sensor
  
  sdi_pin_reset;
  DelaytimerUS(8330);
  
  while(*str!=0){
    thali_sendchar(*str);
    str++;
  }
  
  sdi_dir_in();
  
}


//----------------------------------------------

