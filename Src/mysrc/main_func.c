
#include "includes.h"
#include "fatfs.h"

#include "defines.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global_var.h"
#include "my_delay.h"
#include "eeprom_ext.h"
#include "lcdlib.h"
#include "main_func.h"
#include "gsm.h"
#include "sdi12.h"
#include "serialflash.h"

void main_init(void){
static uint32_t flash_id=0;
	
//	HAL_IWDG_Start(&hiwdg);
	
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim4);
	
	
	HAL_NVIC_DisableIRQ(TIM4_IRQn);//disable key input
		
	rtc_init();	
		
	rtc_gettime_shamsi(&clock);
	cur_clock=clock;
		
	lcd_pwr_on;
	LCDinit();
	LCDclr();
	LCDcursorOFF();
	LCDputs("Boot Device ");
	valid_door_flag=1;
	flag.main_menu=1;
	flag.open_door=1;
	flag.door=1;
	flag.LcdOn=1;
	delayms(1000);
	

//	if(mmc_detect==0){
		
		flash_id=SPI_Flash_ReadID();
		
		f_mount(&SDFatFs, (TCHAR const*)SDPath, 1);
		sprintf((char*)log_buffer,"%04d_%02d.txt",clock.year,clock.month);
		res=f_open(&File[0],log_buffer,FA_WRITE | FA_OPEN_ALWAYS);
		
		LCDclr();
		if(res==0){
			LCDputs("SD Card Mount OK");
			f_close(&File[0]);
			
		}
		else{
			sprintf(buf,"SD Card Fail %d",res);
			LCDputs(buf);
		}
	
		delayms(1000);
//	}
	
  
  //------------------station name-------------------------------
	if(read_exteeprom_byte(station_name_ee_adr)!=0xFF){
		read_exteeprom_ary((unsigned char*)station_name,30,station_name_ee_adr);
				
	}
			
  //--------------------if external device connected--------------
  ext_device=read_exteeprom_dword(ext_device_ee_adr);
  
  if(ext_device==0xFFFFFFFF){
    ext_device=0;
    write_exteeprom_dword(ext_device,ext_device_ee_adr);
    
  }
  else {
//		ext_device |= gsm;//  gsm modem is fixed in device
				
    if((ext_device&temp_sensor)==temp_sensor){ 
      temp_type=read_exteeprom_byte(temp_type_ee_adr);
      if(temp_type & PT100){
        temp_difference=read_exteeprom_byte(temp_difference_ee_adr);
        analog_pwr_on;
      }
    }  
    
    if(ext_device & water_sensor){
      WaterSensorType=read_exteeprom_byte(water_type_ee_adr);
      if(WaterSensorType==microsonic){
        
        max_water_level=read_exteeprom_word(max_water_level_ee_adr);
        min_water_level=read_exteeprom_word(min_water_level_ee_adr);
        SensorLevel=read_exteeprom_word(sensor_level_ee_adr);
        //      Imax=read_exteeprom_float(Imax_ee_adr);
        //      Imin=read_exteeprom_float(Imin_ee_adr);
        
        
      }
			if(WaterSensorType == rotary_encoder){
				RotaryResolution=read_exteeprom_word(RotaryResolution_ee_adr);
				RotaryDiameter=read_exteeprom_word(RotaryDiameter_ee_adr);
				if(RotaryResolution==0xFFFF) RotaryResolution=200;//Pulse
				if(RotaryDiameter==0xFFFF) RotaryDiameter=20;//cm
				RotaryScale=RotaryResolution/RotaryDiameter;
				water_level=(int16_t)read_exteeprom_word(water_level_ee_adr);
				if(water_level>1000 || water_level<(0-1000))water_level=0;
				TIM1->CNT=water_level*RotaryScale;
				cur_water_level=water_level;
			}
			
			last_water_level=water_level;	
			cur_water_level_display=water_level;
			delta_water_level=read_exteeprom_word(DeltaWlvlEEadr);
			
    }
    
    if(ext_device & rain_sensor){
      T_hi=read_exteeprom_byte(T_hi_ee_adr);
      T_low=read_exteeprom_byte(T_low_ee_adr);
			
			rain_sensor_type=read_exteeprom_byte(rain_type_ee_adr);
     
      rain_level=read_exteeprom_float(rain_level_ee_adr);
      if(rain_level>100 || rain_level<0)rain_level=0;
      
      RainRstTime=read_exteeprom_word(RainRstTime_ee_adr);
            
      RainEventTime=read_exteeprom_word(RainEventTime_ee_adr);
      if(RainEventTime>30000)RainEventTime=900;
			
			if(read_exteeprom_dword(DeltaRainLevelEEadr)!=0xFFFFFFFF){
				delta_rain_level=read_exteeprom_float(DeltaRainLevelEEadr);
      }
			else
				delta_rain_level=1000;//unreachable value
						
			last_rain_level=rain_level;
      cur_rain_level=rain_level;      
			
			heater_control = read_exteeprom_byte(heater_control_ee_adr);
    }
    
    if(ext_device & gsm){
			LCDclr();
			LCDputs("GSM Init...");
			GsmInit();
      read_exteeprom_ary(ServerPhone,29,server_phone_ee_adr); 
			read_exteeprom_ary(ChargeReqStr,29,ChargeReqStr_ee_adr);
      gsm_off_time=clock.hour;
			SmsSendTime=read_exteeprom_word(SmsSendTime_ee_adr);
			

			
		}

    
	           
    
  }
  
  
  //if password changed from defult password address 64000 would be 0 else it would be 255 and password would be defult password--
  if(read_exteeprom_byte(32)==255){ 
    password=defult_password;
    write_exteeprom_word(password,password_ee_adr);
    write_exteeprom_byte(0,32);
    
  }
  else if(read_exteeprom_byte(32)==0)
    password = read_exteeprom_word(password_ee_adr);
  
  else {
    write_exteeprom_byte(0,32); 
    
    password=defult_password;
    write_exteeprom_word(password,password_ee_adr);
  }
  
  //-----------------------------------------------log_time setting-------------------------------
  if(read_exteeprom_word(log_time_ee_adr)==0xFFFF){
    log_time=60;
    write_exteeprom_word(log_time,log_time_ee_adr);
    
  }
  else log_time = read_exteeprom_word(log_time_ee_adr);
	
	if(read_exteeprom_word(LogParametersEEadr)==0xFFFF){
    LogParameters=5;
    write_exteeprom_word(LogParameters,LogParametersEEadr);
    
  }
  else LogParameters = read_exteeprom_word(LogParametersEEadr);
	
	
	
//	flash_id=SPI_Flash_ReadID();
 hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
	

HAL_NVIC_EnableIRQ(TIM4_IRQn);//enable input key


}

//*************************************************
void test_code(void){
	
	/**********eeprom test****************/
static	uint8_t byte_write=0x10;
static	uint16_t word_write=0x20;
static	uint32_t dword_write=0x30;
static	float flt_write=2.35;

static	uint8_t byte_read;
static	uint16_t word_read;
static	uint32_t dword_read;
static	float flt_read;
/*************************************/
	
	
static	uint8_t wbuf[20]="HELLOooooooooooo";
static	uint8_t rbuf[20];
	
static RTC_TimeTypeDef rtctime,rtctime1;
static	RTC_DateTypeDef rtcdate,rtcdate1;
static	uint32_t dayelapsed,cnth,cntl;
static	HAL_StatusTypeDef rtcstate;
static	uint32_t fatctl;
static	uint32_t backupreg;


static	uint32_t flash_id,flashadr;
	
static uint32_t ctrl;
	
	
//	if(ctrl==1){
//			ctrl=0;
//			res=f_mount(&SDFatFs,(TCHAR const*)SDPath,1);
//	//	res=f_mkfs((TCHAR const*)SDPath,0,512);
//	//	res=f_mount(&SDFatFs,(TCHAR const*)SDPath,1);
//			res=f_open(&File[0],"user.txt",FA_WRITE | FA_OPEN_ALWAYS);

//			res=f_write(&File[0],wbuf,10,&FatBytesWritten);
//			f_close(&File[0]);
//		}
//		if(ctrl==2){
//			ctrl=0;
//			res=f_open(&File[0],"user.txt",FA_READ);
//			res=f_read(&File[0],rbuf,10,&FatBytesRead);
//			f_close(&File[0]);
//		}
//		
//		if(fatctl==3){
//			rtctime.Hours=23;
//			rtctime.Minutes=40;
//			rtctime.Seconds=45;
//			rtcdate.Date=2;
//			rtcdate.Month=11;
//			rtcdate.Year=15;
//			HAL_RTC_SetTime(&hrtc,&rtctime,RTC_FORMAT_BIN);
//			HAL_RTC_SetDate(&hrtc,&rtcdate,RTC_FORMAT_BIN);
//			backupreg=HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
//			HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,0x32F1);
//		}
//		if(fatctl==4){
//			fatctl=0;
//			HAL_RTC_SetDate(&hrtc,&rtcdate,RTC_FORMAT_BIN);
//		}
//		if(fatctl==5){
//			fatctl=0;
//			HAL_RTC_SetTime(&hrtc,&rtctime,RTC_FORMAT_BIN);
//		}
//		if(fatctl==6){
//			
//			fatctl=0;
//			SPI_Flash_Write(wbuf,flashadr,FatBytesWritten);
//			
//			
//		}
//		if(fatctl==7){
//			fatctl=0;
//			SPI_Flash_Read(rbuf,flashadr,FatBytesWritten);
//			
//		}
//for(FatBytesWritten=0;FatBytesWritten<512;FatBytesWritten++)wbuf[FatBytesWritten]=FatBytesWritten;			

//		HAL_RTC_GetTime(&hrtc,&rtctime1,RTC_FORMAT_BIN);
//		rtcstate=HAL_RTC_GetDate(&hrtc,&rtcdate1,RTC_FORMAT_BIN);


/**************eeprom test******************************************/
write_exteeprom_byte(byte_write,10);
byte_read=read_exteeprom_byte(10);

write_exteeprom_word(word_write,20);
word_read=read_exteeprom_word(20);

write_exteeprom_dword(dword_write,30);
dword_read=read_exteeprom_dword(30);

write_exteeprom_float(flt_write,40);
flt_read=read_exteeprom_float(40);

write_exteeprom_ary(wbuf,10,50);
read_exteeprom_ary(rbuf,10,50);


clear_eeprom();

byte_read=read_exteeprom_byte(10);
word_read=read_exteeprom_word(20);
dword_read=read_exteeprom_dword(30);
flt_read=read_exteeprom_float(40);
read_exteeprom_ary(rbuf,10,50);



//while(1){
//	delayms(500);
//	relay2_on;
//	delayms(500);
//	relay2_off;
//	
//}
	
}

//----------------------------------------------------------
void ReadSensorToStr(char *str){
	
	uint16_t str_len=0;
	
	sprintf(str,"%04d/%02d/%02d,%02d:%02d,\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min);

  if(ext_device & water_sensor){
    read_water_level();
		str_len=strlen(str);
		sprintf(str+str_len,"Wlvl=%d,\r\n",water_level);
    
  }
  
  if(ext_device & rain_sensor){
		str_len=strlen(str);
		sprintf(str+str_len,"Rn=%4.2f,\r\n",rain_level);
  }	
	
	if(flag.test_mode==1){
		strcat(str,"TestMode,\r\n");
	}
	
	str_len=strlen(str);
	*(str+str_len)=0;
	
//	str_len=strlen(str);
//	strcpy(str+str_len,"\r\n");
//	*(str+str_len)='\r';
//	*(str+str_len+1)='\n';
//	*(str+str_len+2)=0;

}

//------------------------------------------------------

unsigned char log_data(void){
    
  UINT n;
	
	uint16_t str_len=0,temp_int=0,temp_flt=0;
  	  

	
	sprintf((char*)log_buffer,"%04d/%02d/%02d,%02d:%02d,",clock.year,clock.month,clock.day,clock.hour,clock.min);
	
	
	
	if(LogParameters & LogWaterlevel){
		read_water_level();
		str_len=strlen((char*)log_buffer);
		sprintf((char*)log_buffer+str_len,"Wlvl=%d,",water_level);
		
	}
	if(LogParameters & LogRainlevel){
		temp_int=rain_level;
		temp_flt=(uint16_t)((rain_level-temp_int)*100);
		str_len=strlen((char*)log_buffer);
		sprintf((char*)log_buffer+str_len,"Rn=%d.%d,",temp_int,temp_flt);
		
//		if(rain_level>(last_rain_level+delta_rain_level)){
//			strcat((char*)log_buffer,"Flood,");
//			SendSMS((char*)ServerPhone,(char*)log_buffer);				
//		}
//		
//		last_rain_level=rain_level;
		
	}
	
	if(flag.test_mode==1){
		strcat((char*)log_buffer,"TestMode,");
	}
	
	strcat((char*)log_buffer,"\r\n");
	
	//Save to memory
	sprintf((char*)buf,"%04d_%02d.txt",clock.year,clock.month);
	
	enter_critical_section();
	
	f_mount(&SDFatFs, (TCHAR const*)SDPath, 1);
	
	res=f_open(&File[0],(char*)buf,FA_WRITE | FA_OPEN_ALWAYS);

	if(res==FR_OK){
					
		f_lseek(&File[0],f_size(&File[0]));
		f_write(&File[0],log_buffer,strlen((char*)log_buffer),&n);
		delayms(50);
		f_close(&File[0]);
		delayms(50);

	}
	else{
		sprintf(SMS_text,"SD Card Fail:%d",res);
		SendSMS((char*)ServerPhone,SMS_text);
		delayms(10000);
		HAL_NVIC_SystemReset();
	}
        
//  }
  
	exit_critical_section();
	
  return 1;	
}
//-----------------------------------------------------


void display(void){
	
	if(valid_door_flag==1 && flag.LcdOn==1 && flag.menu_setting==0){
		LCDcursorOFF();
		if(flag.main_menu==1){
			flag.main_menu=0;
			sprintf(buf,"%s\n%02d:%02d O %02d/%02d/%02d",title_str,clock.hour,clock.min,clock.year%100,clock.month,clock.day);
			LCDclr();
			LCDputs(buf);		
			return;			
		}
		
		if(ext_device&water_sensor){
			
			if(cur_water_level_display!=water_level){
				cur_water_level_display=water_level;
//				sprintf(buf,"    GSM RTU\nWATER LEVEL:%dcm",water_level);
//				LCDclr();
				sprintf(buf,"WLVL:%dcm    ",water_level);
				LCDGotoXY(0,2);
				LCDputs(buf);
				
			}
		}					   
		if(ext_device&rain_sensor){
		 
			if(cur_rain_level!= rain_level){
				sprintf(buf,"%s\nRAIN LEVEL:%5.3fmm",title_str,rain_level);
				cur_rain_level=rain_level;
				LCDclr();
				LCDputs(buf);
			}
		}		
		
		if(clock.sec != cur_clock.sec){
			cur_clock=clock;
//			sprintf(buf,"%s\n%02d:%02d O %02d/%02d/%02d",title_str,clock.hour,clock.min,clock.year%100,clock.month,clock.day);
			sprintf(buf,"%s\n  %d:%d:%d    ",title_str,clock.hour,clock.min,clock.sec);
//			LCDclr();
			LCDputs(buf);
		}
				
	}
	
	else if(valid_door_flag==2 && flag.LcdOn==1 && flag.ir_flag==0){
		
		if(flag.main_menu==1){
			flag.main_menu=0;
			
			LCDclr();
			LCDputs("Invalid User !");		
			return;			
		}
	}
 
}	


//*****************************************************************************
void correct_rain_level(volatile float time,volatile float*level){
  
  if(time<=0.7) *level+=0.143;
  
  else if(time<=0.8 && time>0.7) *level+=0.1389;
  else if(time<=0.9 && time>0.8) *level+=0.135;
  else if(time<=1.1 && time>0.9) *level+=0.1316;
  else if(time<=1.3 && time>1.1) *level+=0.128;
  else if(time<=1.5 && time>1.3) *level+=0.125;
  else if(time<=1.8 && time>1.5) *level+=0.122;
  else if(time<=2.1 && time>1.8) *level+=0.119;
  else if(time<=2.7 && time>2.1) *level+=0.1163;
  else if(time<=3.1 && time>2.7) *level+=0.1136;
  else if(time<=4 && time>3.1)   *level+=0.111;
  else if(time<=6.5 && time>4.0) *level+=0.1087;
  else if(time<=7.5 && time>6.5) *level+=0.1075;
  else if(time<=8.5 && time>7.5) *level+=0.1064;
  else if(time<=10 && time>8.5)  *level+=0.1042;
  else if(time<=12 && time>10)   *level+=0.102;
  
  else if(time>12) *level+=0.1;
  
  
}
//***********************************************************************



void event_process(void){
  

 
	//--------------log user check -------------------------
	if(flag.UserCheck==1){
		flag.rob=0;
		amp_pwr(0);
		valid_door_flag=1;
		flag.main_menu=1;
		
		
		flag.UserCheck=0;
		sprintf((char*)SMS_text,"User check at %d/%d/%d %d:%d\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min);
		
		f_mount(&SDFatFs,(TCHAR const*)SDPath,1);
		res=f_open(&File[0],"user.txt",FA_WRITE | FA_OPEN_ALWAYS);
		if(res==FR_OK){
			f_lseek(&File[0],f_size(&File[0]));
			f_write(&File[0],SMS_text,strlen((char*)SMS_text),&FatBytesWritten);
			delayms(50);
			f_close(&File[0]);
			delayms(50);

		}
		else{
			LCDclr();
			LCDputs("log fail...!");
			delayms(1000);
		}
		
		if(ext_device & gsm){
			
			if(SendSMS(ServerPhone,SMS_text)==1){
				delayms(1000);
				LCDclr();
				LCDputs("USER SMS SENT OK!");
				delayms(2000);
			}
		}
	}
	
  //----------------------if rain detected --------------------
  if(flag.detect_rain==1 && flag.start_rain_event==0){
    
		flag.start_rain_event=1;
    flag.detect_rain=0;
    rain_cou=0;
		
		
		sprintf(buf,"%04d/%02d/%02d,%02d:%02d:%02d,Rn=%4.2f,RainStart,\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,rain_level);				
		log_event(buf);
		
		if(ext_device & gsm){
			sprintf(SMS_text,"%04d/%02d/%02d,\r\n%02d:%02d:%02d,\r\nRn=%4.2f,\r\nRainStart,\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,rain_level);		
			SendSMS(ServerPhone,SMS_text);
			delayms(2000);
			read_exteeprom_ary(AdminMobile,29,AdminMobile1_ee_adr); 
			SendSMS(AdminMobile,SMS_text);
			delayms(2000);
			read_exteeprom_ary(AdminMobile,29,AdminMobile2_ee_adr); 
			SendSMS(AdminMobile,SMS_text);
			delayms(2000);
		}
			
   }
	
	if(flag.stop_rain==1){ 
		flag.stop_rain=0;
		
		sprintf(buf,"%04d/%02d/%02d,%02d:%02d:%02d,Rn=%4.2f,RainStop,\r\n\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,rain_level);				
		log_event(buf);
		
		if(ext_device & gsm){
			sprintf(SMS_text,"%04d/%02d/%02d,\r\n%02d:%02d:%02d,\r\nRn=%4.2f,\r\nRainStop,\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,rain_level);		
			SendSMS(ServerPhone,SMS_text);
			delayms(2000);
			read_exteeprom_ary(AdminMobile,29,AdminMobile1_ee_adr); 
			SendSMS(AdminMobile,SMS_text);
			delayms(2000);
			read_exteeprom_ary(AdminMobile,29,AdminMobile2_ee_adr); 
			SendSMS(AdminMobile,SMS_text);
			delayms(2000);
			
		}
	}
	
	if(flag.detect_rain==1){
		flag.detect_rain=0;
		sprintf(buf,"%04d/%02d/%02d,%02d:%02d:%02d,Rn=%4.2f,\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,rain_level);				
		log_event(buf);
		
	}
	
	if(flag.min_rain_check==1){
		flag.min_rain_check=0;
		rain_sum=0;
		for(i=0;i<15;i++){
			rain_sum+=rain_register[i];
		}
		if(rain_sum > delta_rain_level){
			for(i=0;i<15;i++)
					rain_register[i]=0;
				
			rain_register_index=0;
			
			sprintf(buf,"%04d/%02d/%02d,%02d:%02d:%02d,Rn=%4.2f,RainAlarm,\r\n\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,rain_level);				
			log_event(buf);
			
			if(ext_device & gsm){
				sprintf(SMS_text,"%04d/%02d/%02d,\r\n%02d:%02d:%02d,\r\nRn=%4.2f,\r\nRainAlarm,\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,rain_level);		
				SendSMS(ServerPhone,SMS_text);
				delayms(2000);
				read_exteeprom_ary(AdminMobile,29,AdminMobile1_ee_adr); 
				SendSMS(AdminMobile,SMS_text);
				delayms(2000);
				read_exteeprom_ary(AdminMobile,29,AdminMobile2_ee_adr); 
				SendSMS(AdminMobile,SMS_text);
				delayms(2000);
				
			}
						
		}
	}

		
}
  


/*******************************************************************/

uint32_t strtolong(char* str){
  uint8_t sign=0;
  uint32_t value=0;
	
  if((*str)=='-') {
		sign=1;
		str++;
	}
	
  while (*str != 0){
    if((*str)>='0' && (*str)<='9')
			value = (value*10) + (*str)-0x30 ;
    else 
			return 0;    
		
    str++;
  }
	if(sign==1)
		value=0-value;
	
  return value;
}

//******************************************************************
void read_water_level(void){
  char *point;
  

  
    flush_buf(thali_buf,20);
    strcpy(buf,"0M!");
    thali_sendcmd(buf);
    delayms(2000);
    strcpy(buf,"0D0!");
    thali_sendcmd(buf);
    
    if(thali_readresp()==0){
      point=CharSearch(thali_buf,CRchar,19);
      if(point!=NULL){
        *point=0;
        water_level=(unsigned int)strtolong(thali_buf+2);
        if(thali_buf[1]=='-'){
          water_level=0-water_level;          
        }
        
      } 
    }
    
  
  
}

////////////////////////////////////////////////////////////
void avg_buf(uint16_t *bufd,unsigned char *bufs,int16_t buf_size,int16_t count,char div){
  int16_t i;
  if(div==0){
    for(i=0;i<buf_size;i++){
      tgl_wdt;
      bufd[i]+=bufs[i];
    }
  }
  else{
    for(i=0;i<buf_size;i++){
      tgl_wdt;
      bufd[i]/=count;
    }
    for(i=0;i<buf_size;i++){
      tgl_wdt;
      bufs[i]=(unsigned char)bufd[i];
    }
  }
}



/********************* sound player rutines******************************/
void amp_pwr(char stat){
  
}  

/////////////////////////////////////////////////////////////////

void Error_Handle(void)
{
  if( flag.LcdOn==1){
    LCDclr();
    LCDputs("Error . . . !");
    delayms(1000);
  }
  while(1)
  {
    
    HAL_Delay(40);
  }
}
/***************************tglwdt********************/
void wdt_refresh(void){
    
//  HAL_IWDG_Refresh(&hiwdg);
	
}
//---------------------Find Log Record---------
uint8_t FindLog(char* header,char* str){
 char *str_point=NULL,*str_point1=NULL;
uint8_t header_copy[20],HeaderLength;
uint32_t file_pointer=0;

strcpy(header_copy,header);
str_point=header_copy;
HeaderLength=strlen(header);	
	
	str_point1=CharSearch(str_point,'/',250);
	if(str_point1==NULL){
		strcat(str,"Incorrect Format\r\n");
		return 0;
	}
	*str_point1=0;
	strcpy(buf,str_point);
	strcat(buf,"_");
	str_point=CharSearch(str_point1+1,'/',250);
	if(str_point==NULL){
		strcat(str,"Incorrect Format\r\n");
		return 0;
	}
	*str_point=0;
	strcat(buf,str_point1+1);
	strcat(buf,".txt");
//	f_mount(&SDFatFs, (TCHAR const*)SDPath, 1);
	delayms(100);
	res=f_open(&File[0],(char*)buf,FA_READ);
	if(res!=0){
		sprintf(buf,"Error:%d\r\n",res);
		strcat(str,buf);
		delayms(100);
		f_close(&File[0]);
		delayms(100);
		return 0;
	}
	
	str_point=NULL;
	loop_timeout=0;
	while(loop_timeout<60){
		tgl_wdt;
		if(str_point==NULL){
			f_lseek(&File[0],file_pointer);
			delayms(20);
			f_read(&File[0],MemBuf,1024,&FatBytesRead);
			str_point=MemBuf;
		}
		else
			str_point++;
		
		if(memcmp(str_point,header,HeaderLength)==0){
			str_point1=CharSearch(str_point,LF,((MemBuf+1024)-str_point));
			if(str_point1 != NULL){
				*(str_point1+1)=0;
				strcat(str,str_point);
				delayms(100);
				f_close(&File[0]);
				delayms(100);
				return 1;
			}
//			else{
//				strcat(str,"Record corrupted!\r\n");
//				return 0;
//			}
		}
		
		str_point=CharSearch(str_point,LF,((MemBuf+1024)-str_point));
		
		if(str_point==NULL){
			if(FatBytesRead<1024){
				strcat(str,"Record does not find!\r\n");
				delayms(100);
				f_close(&File[0]);
				delayms(100);
				return 0;
			}
			else
				file_pointer+=512;
			
		}
					
	} 
	delayms(100);
	f_close(&File[0]);
	delayms(100);
	return 0;
}


//********************************************
void log_event(char* log_str){
	
	//Save to memory
	enter_critical_section();
	
	res=f_open(&File[0],"RnLog.txt",FA_WRITE | FA_OPEN_ALWAYS);

	if(res==FR_OK){
					
		f_lseek(&File[0],f_size(&File[0]));
		f_write(&File[0],log_str,strlen(log_str),&FatBytesWritten);
		delayms(50);
		f_close(&File[0]);
		delayms(50);

	}
	else{
		sprintf(SMS_text,"SD Card Fail:%d",res);
		SendSMS((char*)ServerPhone,SMS_text);
		delayms(10000);
		HAL_NVIC_SystemReset();
	}
	
	exit_critical_section();

}
//-------------------------------------------------
void enter_critical_section(void){
	HAL_NVIC_DisableIRQ(TIM2_IRQn);
	HAL_NVIC_DisableIRQ(TIM4_IRQn);
	HAL_NVIC_DisableIRQ(EXTI2_IRQn);
	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
}

void exit_critical_section(void){
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}
//-----------------------------------------------
void SARAMIN(char *str){
	
	uint16_t str_len=0;
	
	sprintf(str,"%04d/%02d/%02d %02d:%02d:%02d\r\n%s\r\n%06d\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min,clock.sec,station_name,station_address);
  
	if(ext_device & rain_sensor){
		
		str_len=strlen(str);
		sprintf(str+str_len,"TRAIN=%4.1f\r\nNRAIN=%3.1f\r\n",rain_level,minute_rain);
  }	
	
	if(ext_device & water_sensor){
		read_water_level();
		str_len=strlen(str);
		sprintf(str+str_len,"Wlvl=%d\r\n",water_level);
			
	}
	
	

}
//*************************************************************************
void rtc_event_calculation(void){
	rtc_gettime_shamsi(&clock);
	if(clock.min != minute_changed){
		minute_changed=clock.min;
		time24=(uint16_t)clock.hour*60+(uint16_t)clock.min;
		
		if(log_time>=1){
			if((time24 % log_time)==0){
				flag.log_start=1;
			}
		}	
		if(camera_log_time>=1){
			if((time24 % camera_log_time)==0){
				flag.log_picture=1;
			}
		}	
		
		if(time24 == RainRstTime){
			rain_level=0;
			write_exteeprom_float(rain_level,rain_level_ee_adr);
		}
		
		if(gprs_auto_upload & gprs_enable){
			if(gprs_auto_upload & gprs_upload_logfile){
				if(gprs_upload_time>=1){
					if((time24 % gprs_upload_time)==0){
						flag.upload_log=1;
					}
					
				}
			}
			if(gprs_auto_upload & gprs_upload_tabletime){
				if((time24==upload_time_table[0])||(time24==upload_time_table[1])||
					(time24==upload_time_table[2])||(time24==upload_time_table[3])||
					(time24==upload_time_table[4])){
						
					flag.upload_in_schedule=1;	
					
				}
			}
		
		}
		
	}
					


}