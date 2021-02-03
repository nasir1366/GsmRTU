/*--------------------------------------------------------------------------*/
/*  RTC controls                                                            */
#include "includes.h"
#include "defines.h"
#include "rtc.h"
#include "bcd.h"
#include "my_delay.h"
#include "global_var.h"

int16_t iYear;

int16_t iMonth;

int16_t iDay;

Rtc temp_clock;

HAL_StatusTypeDef stat;
//HAL_I2C_StateTypeDef i2c_stat;

void reset_i2c();

/*-------------------------------------------------*/
/* RTC functions                                   */


char rtc_gettime (volatile Rtc *rtc)
{
  uint8_t temp;
	static uint32_t rtc_cnt=0;
  
	
	rtc_cnt=(&hrtc)->Instance->CNTL;
	rtc_cnt=(uint32_t)(((&hrtc)->Instance->CNTH)<<16)|rtc_cnt;	
	
	rtc->sec=rtc_cnt%60;
	rtc->min=(rtc_cnt%3600)/60;
	rtc->hour=rtc_cnt/3600;
  return 0;  
//  if(HAL_I2C_Mem_Read(&hi2c1,rtc_adr,0,I2C_MEMADD_SIZE_8BIT,&temp,1,i2c_timeout)!=HAL_OK){
//    /* reinit i2c bus */
//    reset_i2c();
//    
//    return 0;
//  }
  rtc->sec=bcd2bin(temp);
  
//  if(HAL_I2C_Mem_Read(&hi2c1,rtc_adr,0x10,I2C_MEMADD_SIZE_8BIT,&temp,1,i2c_timeout)!=HAL_OK){
//    /* reinit i2c bus */
//    reset_i2c();
//    
//    return 0;
//  }
  rtc->min=bcd2bin(temp);
    
//  if(HAL_I2C_Mem_Read(&hi2c1,rtc_adr,0x20,I2C_MEMADD_SIZE_8BIT,&temp,1,i2c_timeout)!=HAL_OK){
//    /* reinit i2c bus */
//    reset_i2c();
//    
//    return 0;
//  }
  rtc->hour=bcd2bin(temp);
    
//  if(HAL_I2C_Mem_Read(&hi2c1,rtc_adr,0x40,I2C_MEMADD_SIZE_8BIT,&temp,1,i2c_timeout)!=HAL_OK){
//    /* reinit i2c bus */
//    reset_i2c();
//    
//    return 0;
//  }
  rtc->day=bcd2bin(temp);
  
//  if(HAL_I2C_Mem_Read(&hi2c1,rtc_adr,0x50,I2C_MEMADD_SIZE_8BIT,&temp,1,i2c_timeout)!=HAL_OK){
//    /* reinit i2c bus */
//    reset_i2c();
//    
//    return 0;
//  }
  rtc->month=bcd2bin(temp);
    
//  if(HAL_I2C_Mem_Read(&hi2c1,rtc_adr,0x60,I2C_MEMADD_SIZE_8BIT,&temp,1,i2c_timeout)!=HAL_OK){
//    /* reinit i2c bus */
//    reset_i2c();
//    
//    return 0;
//  }
  rtc->year=bcd2bin(temp)+2000;
   
    
  return 1;
  
}

char rtc_gettime_shamsi(volatile Rtc *rtc){
  
  if(rtc_gettime(&temp_clock)==0)
    return 0;
  MiladiToShamsi(temp_clock.day,temp_clock.month,temp_clock.year);
  rtc->day=iDay;
  rtc->month=iMonth;
  rtc->year=iYear;
  rtc->hour=temp_clock.hour;
  rtc->min=temp_clock.min;
  rtc->sec=temp_clock.sec;
  return 1;
}


char rtc_settime (volatile Rtc *rtc)
{
  uint8_t temp;
  
	flag.i2c_busy=1;
	
  temp=bin2bcd(rtc->sec);
//  if(HAL_I2C_Mem_Write(&hi2c1,rtc_adr,0,I2C_MEMADD_SIZE_8BIT,&temp,1,i2c_timeout)!=HAL_OK){
//    /* reinit i2c bus */
//    reset_i2c();
//    return 0;
//  }
  
  delayms(10);
  
  temp=bin2bcd(rtc->min);
//  if(HAL_I2C_Mem_Write(&hi2c1,rtc_adr,0x10,I2C_MEMADD_SIZE_8BIT,&temp,1,i2c_timeout)!=HAL_OK){
//    /* Generate Stop */
//    reset_i2c();
//    return 0;
//  }
  delayms(10);
  
  temp=bin2bcd(rtc->hour);
//  if(HAL_I2C_Mem_Write(&hi2c1,rtc_adr,0x20,I2C_MEMADD_SIZE_8BIT,&temp,1,i2c_timeout)!=HAL_OK){
//    /* Generate Stop */
//    reset_i2c();
//    return 0;
//  }
  delayms(10);
  
  temp=bin2bcd(rtc->day);
//  if(HAL_I2C_Mem_Write(&hi2c1,rtc_adr,0x40,I2C_MEMADD_SIZE_8BIT,&temp,1,i2c_timeout)!=HAL_OK){
//    /* Generate Stop */
//    reset_i2c();
//    return 0;
//  }
  delayms(10);
  
  temp=bin2bcd(rtc->month);
//  if(HAL_I2C_Mem_Write(&hi2c1,rtc_adr,0x50,I2C_MEMADD_SIZE_8BIT,&temp,1,i2c_timeout)!=HAL_OK){
//    /* Generate Stop */
//    reset_i2c();
//    return 0;
//  }
  delayms(10);
  
  temp=bin2bcd((rtc->year)-2000);
//  if(HAL_I2C_Mem_Write(&hi2c1,rtc_adr,0x60,I2C_MEMADD_SIZE_8BIT,&temp,1,i2c_timeout)!=HAL_OK){
//    /* Generate Stop */
//    reset_i2c();
//    return 0;
//  }
    
  delayms(10);
  flag.i2c_busy=0;
  return 1;
}

char rtc_settime_shamsi(volatile Rtc *rtc){
  
  temp_clock=*rtc;
  ShamsiToMiladi(temp_clock.day,temp_clock.month,temp_clock.year);	
  temp_clock.day=iDay;
  temp_clock.month=iMonth;
  temp_clock.year=iYear;
  
  if(rtc_settime(&temp_clock)==0)
    return 0;
  
  return 1;
}


char rtc_init (void)
{
  uint8_t temp=0;
  flag.i2c_busy=1;  
//  if(HAL_I2C_Mem_Write(&hi2c1,rtc_adr,0xF0,I2C_MEMADD_SIZE_8BIT,&temp,1,i2c_timeout)!=HAL_OK){
//    /* Generate Stop */
//    reset_i2c();
//    return 0;
//  }
  delayms(10);
  
  temp=0x23;//0x23 for 24 hour format & 2HZ Interrupt-------
//  if(HAL_I2C_Mem_Write(&hi2c1,rtc_adr,0xE0,I2C_MEMADD_SIZE_8BIT,&temp,1,i2c_timeout)!=HAL_OK){
//    /* Generate Stop */
//    reset_i2c();
//    return 0;
//  }
  
  
  delayms(10);
  flag.i2c_busy=0;
  return 1;
  
}

unsigned char MiladiIsLeap(int16_t miladiYear)

{
  
  if(((miladiYear % 100)!= 0 && (miladiYear % 4) == 0) || ((miladiYear % 100)== 0 && (miladiYear % 400) == 0))
    
    return TRUE;
  
  else
    
    return FALSE;
  
}



void MiladiToShamsi(int16_t iMiladiDay,int16_t iMiladiMonth,int16_t iMiladiYear)

{
 
  int16_t shamsiDay, shamsiMonth, shamsiYear;
  
  int16_t dayCount,farvardinDayDiff,deyDayDiff ;
  
  int16_t sumDayMiladiMonth[] = {0,31,59,90,120,151,181,212,243,273,304,334};
  
  int16_t sumDayMiladiMonthLeap[]= {0,31,60,91,121,152,182,213,244,274,305,335};
  
  
  farvardinDayDiff=79;
  
  
  
  if (MiladiIsLeap(iMiladiYear))
    
  {
    
    dayCount = sumDayMiladiMonthLeap[iMiladiMonth-1] + iMiladiDay;
    
  }
  
  else
    
  {
    
    dayCount = sumDayMiladiMonth[iMiladiMonth-1] + iMiladiDay;
    
  }
  
  if((MiladiIsLeap(iMiladiYear - 1)))
    
  {
    
    deyDayDiff = 11;
    
  }
  
  else
    
  {
    
    deyDayDiff = 10;
    
  }
  
  if (dayCount > farvardinDayDiff)
    
  {
    
    dayCount = dayCount - farvardinDayDiff;
    
    if (dayCount <= 186)
      
    {
      
      switch (dayCount%31)
        
      {
        
      case 0:
        
        shamsiMonth = dayCount / 31;
        
        shamsiDay = 31;
        
        break;
        
      default:
        
        shamsiMonth = (dayCount / 31) + 1;
        
        shamsiDay = (dayCount%31);
        
        break;
        
      }
      
      shamsiYear = iMiladiYear - 621;
      
    }
    
    else
      
    {
      
      dayCount = dayCount - 186;
      
      switch (dayCount%30)
        
      {
        
      case 0:
        
        shamsiMonth = (dayCount / 30) + 6;
        
        shamsiDay = 30;
        
        break;
        
      default:
        
        shamsiMonth = (dayCount / 30) + 7;
        
        shamsiDay = (dayCount%30);
        
        break;
        
      }
      
      shamsiYear = iMiladiYear - 621;
      
    }
    
  }
  
  else
    
  {
    
    dayCount = dayCount + deyDayDiff;
    
    
    
    switch (dayCount%30)
      
    {
      
    case 0 :
      
      shamsiMonth = (dayCount / 30) + 9;
      
      shamsiDay = 30;
      
      break;
      
    default:
      
      shamsiMonth = (dayCount / 30) + 10;
      
      shamsiDay = (dayCount%30);
      
      break;
      
    }
    
    shamsiYear = iMiladiYear - 622;
    
    
    
  }
  
  iYear = shamsiYear;
  
  iMonth = shamsiMonth;
  
  iDay = shamsiDay;
  
  
  
}




void ShamsiToMiladi(int16_t ShamsiDay,int16_t ShamsiMonth,int16_t ShamsiYear)
{
  
  
  int16_t          marchDayDiff, remainDay;
  int16_t          dayCount, miladiYear, i;
  
  
  // this buffer has day count of Miladi month from April to January for a none year.
  
  int16_t miladiMonth[12]  =  {30,31,30,31,31,30,31,30,31,31,28,31};
  miladiYear = ShamsiYear + 621;
  
  //Detemining the Farvardin the First
  
  if((MiladiIsLeap(miladiYear)))
  {
    //this is a Miladi leap year so Shamsi is leap too so the 1st of Farvardin is March 20 (3/20)
    marchDayDiff = 12;
  }
  else
  {
    //this is not a Miladi leap year so Shamsi is not leap too so the 1st of Farvardin is March 21 (3/21)
    marchDayDiff = 11;
  }
  
  // If next year is leap we will add one day to Feb.
  if((MiladiIsLeap(miladiYear+1)))
  {
    miladiMonth[10] = miladiMonth[10] + 1; //Adding one day to Feb
  }
  
  //Calculate the day count for input shamsi date from 1st Farvadin
  
  if((ShamsiMonth>=1)&&( ShamsiMonth<=6))
    dayCount = ((ShamsiMonth-1) * 31) + ShamsiDay;
  else
    dayCount =(6 * 31) + ((ShamsiMonth - 7) * 30) + ShamsiDay;
  
  //Finding the correspond miladi month and day
  
  if (dayCount <= marchDayDiff) //So we are in 20(for leap year) or 21for none leap year) to 31 march
  {
    iDay = dayCount + (31 - marchDayDiff);
    iMonth = 3;
    iYear=miladiYear;
  }
  else
    
  {
    remainDay = dayCount - marchDayDiff;
    
    
    i = 0; //starting from April
    
    while ((remainDay > miladiMonth[i]))
    {
      remainDay = remainDay - miladiMonth[i];
      i++;
    }
    iDay = remainDay;
    
    if (i > 8) // We are in the next Miladi Year
    {
      iMonth = i - 8;
      iYear =  miladiYear + 1;
    }
    else
    {
      iMonth = i + 4;
      iYear =  miladiYear;
    }
    
  }
  
}


