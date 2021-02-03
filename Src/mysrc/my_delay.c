
#include "includes.h"
#include "my_delay.h"
#include "defines.h"
#include "main_func.h"


void delayms(uint16_t d){
unsigned long i;

  while(d){
    d--;
    tgl_wdt;
    for(i=0;i<5000;i++);//5000
  }
}

void delayus(uint32_t d){
  unsigned long i;
  
  while(d){
    d--;
    for(i=0;i<3;i++);//3
  }
}

//---------------------------------------
void DelaytimerUS(unsigned int dly){
	unsigned int timer=0;
  unsigned int d=0;

  d=dly;
  TIM3->CNT=0;
	while(timer<d){
    
    timer=TIM3->CNT;
		
  }
  
}
