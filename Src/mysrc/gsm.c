#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defines.h"
#include "includes.h"
#include "gsm.h"
#include "global_var.h"
#include "my_delay.h"
#include "rtc.h"
#include "eeprom_ext.h"
#include "lcdlib.h"
#include "main_func.h"


void GsmSend(char* frame){

HAL_StatusTypeDef stat=HAL_OK;
  
  stat=HAL_UART_Transmit(&huart1,(uint8_t*)frame,strlen(frame),2000);
	if(stat != HAL_OK){
		__HAL_UNLOCK(&huart1);
	}
  
} 

//--------------------------------------------
void GsmSendByte(char *data,unsigned int num){

HAL_StatusTypeDef stat=HAL_OK;
	
  stat=HAL_UART_Transmit(&huart1,(uint8_t*)data,num,2000);
	if(stat != HAL_OK){
		__HAL_UNLOCK(&huart1);
	}
    
}

//-------------------------------------------------------------
char SendSMS(char *number_str, char *message_str){
  
//  static const char str_f5[]="AT%c%c";
  static const char str_f1[]="AT+CMGS=\"%s\"%c%c";
  
	if(strlen(number_str)>20 || strlen(number_str)<3)
		return 0;
	
  if(flag.LcdOn==1){
    LCDclr();
		sprintf(buf,"SMS Sending...\n%s",number_str);
    LCDputs(buf);
  }
  
  flush_buf(SimRecBuffer,250);
  SimRecLen=0; 
  
  sprintf(SimSendBuffer,str_f1,number_str,CRchar,LF);
  GsmSend(SimSendBuffer);
  delayms(500);
  
  SimCount=0;
  while((CharSearch(SimRecBuffer,'>',250)==NULL) && (SimCount<3)){tgl_wdt;}
  
  static const char str_f2[]="%c%c%c";
  if(CharSearch(SimRecBuffer,'>',250)!=NULL){
    GsmSend(message_str);
		delayms(200);
    sprintf(SimSendBuffer,str_f2,CZ,CRchar,LF);
    GsmSend(SimSendBuffer);  
  }
  
  else{
    
    if(flag.LcdOn==1){
      LCDclr();
      LCDputs("SMS Fail !");
    }
    flush_buf(SimRecBuffer,250);
		SimRecLen=0;
    return 0;
  }
    
  delayms(200);
	SimCount=0;
  SimRecLen=0;
  flush_buf(SimRecBuffer,250);
  static const char str_f3[]="+CMGS:";
  
  while(SimCount<20){
    tgl_wdt;
    if(StrSearch_P(SimRecBuffer,str_f3,250)!=NULL){
      flush_buf(SimRecBuffer,250);
      SimRecLen=0;
      if(flag.LcdOn==1){
        LCDclr();
        LCDputs("SMS Sent OK !");
				delayms(2000);
      }
      return 1;
    }
  
  }
  
  if(flag.LcdOn==1){
    LCDclr();
    LCDputs("SMS Fail !");
  }
  static const char str_f4[]="%c%c%c";
  sprintf(SimSendBuffer,str_f4,ESC,CRchar,LF);
  GsmSend(SimSendBuffer);
  delayms(100);
  GsmSend(SimSendBuffer);
  delayms(100);
  return 0;

}

//---------------------------------------------------------
void BinToStr(char *frame,char len,char *string){ //checksum is not in len
  unsigned char index=0,checksum=0;
  static const char str_f1[]="%02X";
  if((len*2)>250)return;
  
  while(index<len){
    sprintf(string,str_f1,frame[index]);
    checksum+=frame[index];
    string+=2;
    index++;
  }
  sprintf(string,str_f1,checksum);  
  
}

//------------------------------------------------
char StrToBin(char *frame,char *string){ // each two character in hexadecimal format is one byte
uint16_t index=0,checksum=0;
char **endptr=NULL,s[3];

    
  while(1){
    memcpy(s,string,2);
    frame[index]=(unsigned char)strtol(s,endptr,16); 
    checksum+=frame[index];
    string+=2;
    if(*(string)=='\0')break;
    index++;
    
  }
  
  checksum -= frame[index];
  if(checksum==frame[index])return 1;
  
return 0; 
}

//----------------------------------------------
char SaveSMS(char *message_str){
char *str_point,ret_value=0;

  static const char str_f1[]="AT+CMGW%c%c";
  static const char str_f4[]="AT%c%c";
  
//  sprintf(SimSendBuffer,str_f4,CRchar,LF);
//  GsmSend(SimSendBuffer);
//  delayms(10);
//  GsmSend(SimSendBuffer);
//  delayms(10);
  
  flush_buf(SimRecBuffer,250);
  SimRecLen=0;
  
  sprintf(SimSendBuffer,str_f1,CRchar,LF);
  GsmSend(SimSendBuffer);
  delayms(50);
  
  SimCount=0;
           
  while(SimCount<3){
    tgl_wdt;
    str_point=CharSearch(SimRecBuffer,'>',250);
    if(str_point!=NULL){
      flush_buf(SimRecBuffer,250);
      SimRecLen=0;
      static const char str_f2[]="%s%c%c%c";
      sprintf(SimSendBuffer,str_f2,message_str,CZ,CRchar,LF);
      GsmSend(SimSendBuffer); 
      SimCount=0;
      
      while(SimCount<5){
        tgl_wdt;
        static const char str_f3[]="+CMGW:";
        str_point=StrSearch_P(SimRecBuffer,str_f3,250);
        if(str_point!=NULL){
          
          ret_value=*(str_point+7);
          flush_buf(SimRecBuffer,250);
          SimRecLen=0;
          return ret_value;
        }
                
      }
      
    }
  }
  
  
  return 0;

}

//-------------------------------------------------
char ReadSMS(char *RemotePhone_point,char *SMS_text_buffer,unsigned char sm_index){
  char *str_point=NULL,*str_point1=NULL;
	char strtemp[20];
  
  static const char str_f1[]="AT+CMGR=%d%c%c";
 
  SimRecLen=0;
  flush_buf(SimRecBuffer,250);
  
  sprintf(SimSendBuffer,str_f1,sm_index,CRchar,LF);
  GsmSend(SimSendBuffer);
  delayms(500);
  
  SimCount=0;
  static const char str_f2[]="+CMGR:";
  while (SimCount<3){
    tgl_wdt;
    str_point=StrSearch_P(SimRecBuffer,str_f2,250);
    if(str_point!=NULL)break;
      
  }
 
  if(str_point!=NULL){
    delayms(500);
    str_point=CharSearch(SimRecBuffer,',',250);   //---------find phone number------------------

    str_point1=str_point+3;
    str_point=CharSearch(str_point1,'"',250);
    if(str_point!=NULL){
      
      *str_point=0;
      strcopy(RemotePhone_point,str_point1);
      
    }
    
    //----find sms text-------------------------
    str_point=CharSearch(str_point+1,LF,250);
    if(str_point!=NULL){
      str_point++;
			memcpy(strtemp,str_point,12);
			lowercase(strtemp,12);
			if((memcmp(strtemp,"pass=saramin",12)==0)||(memcmp(strtemp,"pas=saramin",11)==0)){
				SARAMIN(SMS_text);
				SendSMS(RemotePhone_point,SMS_text);
				return 0;
			}
      *(str_point+5)=0;
//        static const char str_f3[]="MARVCOM";
				sprintf(buf,"%d",password);
        if(strcmp(str_point,buf)==0){
          str_point1=CharSearch(str_point+6,CRchar,250);
          *(str_point1)=0;
					strcopy(SMS_text_buffer,str_point+6);
					SimRecLen=0;
          flush_buf(SimRecBuffer,250);
					return 1;
                
        }
				else{
					strcpy(SMS_text,"Invalid Password");
					SendSMS(RemotePhone_point,SMS_text);
					
				}
           
    }
  }
  
  

return 0;
}

//---------------------------------------------------
char GsmInit(void){
	uint8_t index=0;
  char *sms_pointer;
  
  static const char str_f1[]="AT%c%c";
  static const char str_f2[]="AT+CMGF=1%c%c";
  static const char str_f3[]="AT+CMGD=%d%c%c";
  static const char str_f4[]="AT+CSCLK=0%c%c";//disable mode
  
  	  
  simpwr_on;
  delayms(8000);
  
	HAL_UART_Receive_DMA(&huart1,(uint8_t*)SimRecBuffer,250);
	
	sprintf(SimSendBuffer,str_f1,CRchar,LF);
  GsmSend(SimSendBuffer);
  delayms(500);
  GsmSend(SimSendBuffer);
	delayms(500);
	
  sprintf(SimSendBuffer,str_f2,CRchar,LF);
  GsmSend(SimSendBuffer);
  delayms(500);
	
	sprintf(SimSendBuffer,str_f4,CRchar,LF);
  GsmSend(SimSendBuffer);
  delayms(500);
	
	sprintf(SimSendBuffer,"AT+CNMI=3,1%c%c",CRchar,LF);
  GsmSend(SimSendBuffer);
  delayms(500);
	
//	strcpy(SimSendBuffer,"AT+CSMP=17,167,0,0\r\n");
//  GsmSend(SimSendBuffer);
//  delayms(500);
	
	for(index=1;index<5;index++){
		flush_buf(SimRecBuffer,250);
		SimRecLen=0;
		sprintf(SimSendBuffer,str_f3,index,CRchar,LF);
		GsmSend(SimSendBuffer);
		SimCount=0;
		sms_pointer=NULL;
		while(SimCount<4 && sms_pointer==NULL){
			sms_pointer=StrSearch(SimRecBuffer,"OK",250);
		}
		delayms(500);
	}
	
  flush_buf(SimRecBuffer,250);
  SimRecLen=0;
  
return 0;
}

//flush receive buffer-----------------------------------------
void flush_buf (char *buffer,uint16_t size){
  while (size>0){
    buffer[size-1]=0;
    size--;
  }
  
}

//-----------------------------------------
/*    
find first element of s2 in s1 and return address of first element

length of string must be less than 20 character

*/

char* StrSearch(char* buffer,char* string,uint16_t buf_size){
  
uint16_t index=0,str_len=0;
char str_temp[30];
str_len=strlen(string);
if(str_len>30) return NULL;

while(index<buf_size){
  flush_buf(str_temp,30);
  
  memcpy(str_temp,buffer,str_len);
  
  if(strcmp(str_temp,string)==0) return buffer;
    
  buffer++;
  index++;
  
  if ((index+str_len)>buf_size)return NULL;
  
}
  
return NULL;

}
//-----------------------------------------------------------
char* StrSearch_P(char* buffer,char const* string,uint16_t buf_size){
  
uint16_t index=0,str_len=0;
char str_temp[30];

str_len=strlen(string);
if(str_len>30) return NULL;



while(index<buf_size){
  tgl_wdt;
  flush_buf(str_temp,30);
  
  memcpy(str_temp,buffer,str_len);
  
  if(strcmp(str_temp,string)==0) return buffer;
    
  buffer++;
  index++;
  
  if ((index+str_len)>buf_size)return NULL;
  
}
  
return NULL;

}
//-------------------------------------------------
char* CharSearch(char *buffer,char chr,uint16_t buf_size){
  
  uint16_t index=0;
  
  
  while(index<buf_size){
    
    tgl_wdt;
    
    if(*buffer==chr) return buffer;
    buffer++;  
    index++;
    
  }
  
  return NULL;
  
}
//-------------------------------------------------
char* CharSearch_str(char *buffer,char chr){
      
  while((*buffer) != 0){
    
    tgl_wdt;
    
    if(*buffer==chr) return buffer;
    buffer++;  
        
  }
  
  return NULL;
  
}
//--------------------------------------------------

void SMS_ReplyCMD(char *RemotePhone_point,char *SMS){
	
   char *str_point=NULL,*str_point1=NULL;
     
  if(StrSearch(SMS,"chargereq=",250)!=NULL){
    
    str_point=CharSearch(SMS,'=',250);
    str_point++;
    str_point1=CharSearch(str_point,',',250);
		if(str_point1 != NULL){
			*str_point1=0;
			strcpy(ChargeReqStr,str_point);
			write_exteeprom_ary(ChargeReqStr,strlen(ChargeReqStr)+1,ChargeReqStr_ee_adr);
			read_exteeprom_ary(ChargeReqStr,strlen(ChargeReqStr)+1,ChargeReqStr_ee_adr);
		}
				    
		sprintf(SMS_text,"chargereq=%s",ChargeReqStr);
		SendSMS(RemotePhone_point,SMS_text);
				
  } 

//------------------------------------------------------------------	 
  else if(StrSearch(SMS,"ftpsett=1",250)!=NULL){
    
    str_point=NULL;
    str_point=StrSearch(SMS,"url:",250);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',250);
      *str_point1=0;
      strcpy(server_url,str_point+4);
    }
    
    str_point=NULL;
    str_point=StrSearch(SMS,"uname:",250);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',250);
      *str_point1=0;
      strcpy(server_username,str_point+6);
    }
    
    str_point=NULL;
    str_point=StrSearch(SMS,"pass:",250);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',250);
      *str_point1=0;
      strcpy(server_password,str_point+5);
    }
    
    write_exteeprom_ary(server_url,strlen(server_url)+1,server_url_ee_adr);
    write_exteeprom_ary(server_username,strlen(server_username)+1,server_username_ee_adr);
    write_exteeprom_ary(server_password,strlen(server_password)+1,server_password_ee_adr);
    
    write_exteeprom_byte(0,server_changed1_ee_adr);
    
    sprintf(SMS_text,"url:%s\nuname:%s\npass:%s",server_url,server_username,server_password);
    SendSMS(RemotePhone,SMS_text);
    
  }
  //-------------------------------------------------------------------------------
  else if(StrSearch(SMS,"ftpsett=2",250)!=NULL){
    
    str_point=NULL;
    str_point=StrSearch(SMS,"filename:",250);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',250);
      *str_point1=0;
      strcpy(server_filename,str_point+9);
    }
    
    str_point=NULL;
    str_point=StrSearch(SMS,"path:",250);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',250);
      *str_point1=0;
      strcpy(server_path,str_point+5);
      
    }
    
    str_point=NULL;
    str_point=StrSearch(SMS,"apn:",250);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',250);
      *str_point1=0;
      strcpy(apn_name,str_point+4);
    }
    
    
    sprintf(SMS_text,"filename:%s\npath:%s\napn:%s",server_filename,server_path,apn_name);
    SendSMS(RemotePhone,SMS_text);
       
    
    write_exteeprom_ary(server_path,strlen(server_path)+1,server_path_ee_adr);
    write_exteeprom_ary(server_filename,strlen(server_filename)+1,server_filename_ee_adr);
    write_exteeprom_ary(apn_name,strlen(apn_name)+1,apn_name_ee_adr);
    
    write_exteeprom_byte(0,server_changed2_ee_adr);
		delayms(10000);
		HAL_NVIC_SystemReset();
    
  }
  //-------------------------------------------------------------------------------------------
  else if(StrSearch(SMS,"reboot",250)!=NULL){
    
    sprintf(SimSendBuffer,"AT+CMGD=1%c%c",CRchar,LF);
    GsmSend(SimSendBuffer);
    delayms(200);
    strcpy(SMS_text,"reset ok");
    SendSMS(RemotePhone,SMS_text);
    delayms(10000);
		HAL_NVIC_SystemReset();
    
    
  }
  
  //-----------------------------------------------------+
  else if(StrSearch(SMS,"resetdefault",250)!=NULL){
    
    clear_eeprom();
    strcpy(SMS_text,"set default ok");
    SendSMS(RemotePhone,SMS_text);
    delayms(10000);
    HAL_NVIC_SystemReset();
    
  }
  
	//---------------------------------------------------------------------
  else if((str_point=StrSearch(SMS,"raineventtimeout=",250))!=NULL){
    str_point+=17;
    str_point1=CharSearch(str_point,',',250);
    *str_point1=0;
    RainEventTime=strtolong(str_point);
    
    write_exteeprom_word(RainEventTime,RainEventTime_ee_adr);
    
    sprintf(SMS_text,"RainEventTime=%d",RainEventTime);
    SendSMS(RemotePhone,SMS_text);
  }
  //---------------------------------------------------------------------
  else if((str_point=StrSearch(SMS,"rainzerotime=",250))!=NULL){
    str_point+=13;
    str_point1=CharSearch(str_point,':',250);
    
    *str_point1=0;
    RainRstTime=(strtolong(str_point))*60;
    str_point=CharSearch(str_point1,',',250);
		*str_point=0;
		RainRstTime+=strtolong(str_point1+1);
		
    write_exteeprom_word(RainRstTime,RainRstTime_ee_adr);
    RainRstTime=read_exteeprom_word(RainRstTime_ee_adr);
		
    sprintf(SMS_text,"RainZeroTime=%d",RainRstTime);
    SendSMS(RemotePhone,SMS_text);
  }
  //-----------------------------------------------------------------
  else if(StrSearch(SMS,"readrainsetting",250)!=NULL){
    
    sprintf(SMS_text,"RainRstTime=%d,RainEventTime=%d",RainRstTime,RainEventTime);
    SendSMS(RemotePhone,SMS_text);
  
  }
	 //-----------------------------------------------------------------
  else if(StrSearch(SMS,"rtcsett=",250)!=NULL){
    
    str_point=NULL;
    str_point=StrSearch(SMS,"year:",250);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',250);
      *str_point1=0;
      clock.year=strtolong(str_point+5);
      
    }
    
    str_point=NULL;
    str_point=StrSearch(SMS,"mon:",250);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',250);
      *str_point1=0;
      clock.month=strtolong(str_point+4);
    }
    
    str_point=NULL;
    str_point=StrSearch(SMS,"day:",250);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',250);
      *str_point1=0;
      clock.day=strtolong(str_point+4);
    }
    
    str_point=NULL;
    str_point=StrSearch(SMS,"hour:",250);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',250);
      *str_point1=0;
      clock.hour=strtolong(str_point+5);
    }
    
    str_point=NULL;
    str_point=StrSearch(SMS,"min:",250);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',250);
      *str_point1=0;
      clock.min=strtolong(str_point+4);
    }
        
    rtc_settime_shamsi(&clock);
    
    rtc_gettime_shamsi(&clock);
    
//		next_log_time=(clock.hour+1)*60;
//    if(next_log_time>=1440){next_log_time=0; flag.log_disable=1;}
			
    sprintf(SMS_text,"rtcsett=\r\n%04d/%02d/%02d\r\n%02d:%02d\r\n",clock.year,clock.month,clock.day,clock.hour,clock.min);
    SendSMS(RemotePhone,SMS_text);
    
  }
  
	//-----------------------------------------------------------------------------
	else if(StrSearch(SMS,"readsensor",250)!=NULL){
		sprintf(SMS_text,"%s\r\n%06d\r\n",station_name,station_address);
		ReadSensorToStr(SMS_text+strlen(SMS_text));
		SendSMS(RemotePhone_point,SMS_text);

  }
	
	//---------------------------------------------------
	else if(StrSearch(SMS,"sensorandcharge",250)!=NULL){
    sprintf(SMS_text,"%s\r\n%06d\r\n",station_name,station_address);
		ReadSensorToStr(SMS_text+strlen(SMS_text));
		delayms(200);
		SimChargeCat(SMS_text);
    SendSMS(RemotePhone_point,SMS_text);

  }
	//---------------------------------------------------
	else if(StrSearch(SMS,"simcharge",250)!=NULL){
		strcpy(SMS_text,"simcharge=");
    str_point=NULL;
		flush_buf(SimRecBuffer,250);
//		sprintf(SimSendBuffer,"ATD%s;%c%c",ChargeReqStr,CRchar,LF);
		sprintf(SimSendBuffer,"AT+CUSD=1,\"%s\"%c%c",ChargeReqStr,CRchar,LF);
		GsmSend(SimSendBuffer);
		delayms(500);
		SimCount=0;
		
		while((str_point==NULL) && (SimCount<10)){
			str_point=StrSearch(SimRecBuffer,"CUSD:",250);
			tgl_wdt;
		}
		if(str_point!=NULL){
			delayms(100);
			str_point1=CharSearch(str_point,LF,250);
			*str_point1=0;
			if(strlen(str_point)>155){
				*(str_point+155)=0;
			}
			strcpy(SMS_text,str_point);
		}
    SendSMS(RemotePhone_point,SMS_text);

  }
	//---------------------------------------------------------------------
  else if((str_point=StrSearch(SMS,"centerphone=",250))!=NULL){
    str_point+=12;
    str_point1=CharSearch(str_point,',',250);
    
    *str_point1=0;
    strcpy(ServerPhone,str_point);
    
    write_exteeprom_ary(ServerPhone,strlen(ServerPhone)+1,server_phone_ee_adr);
    
    read_exteeprom_ary(ServerPhone,29,server_phone_ee_adr); 
    
    sprintf(SMS_text,"centerphone=\r\n%s",ServerPhone);
    
    SendSMS(RemotePhone,SMS_text);
  } 
  //----------------------------------------------------------------
  else if(StrSearch(SMS,"readphone",250)!=NULL){
    
    sprintf(SMS_text,"centerphone=\r\n%s",ServerPhone);
    
    SendSMS(RemotePhone,SMS_text);
    
  }
	//-----------------------------------------------------
	else if((str_point=StrSearch(SMS,"smssendtime=",250))!=NULL){
		str_point1=CharSearch(str_point,':',250);
		*str_point1=0;
		SmsSendTime=(strtolong(str_point+12))*60;
			
    str_point=CharSearch(str_point1,',',250);
		*str_point=0;
		SmsSendTime+=strtolong(str_point1+1);
		
		write_exteeprom_word(SmsSendTime,SmsSendTime_ee_adr);
		SmsSendTime=read_exteeprom_word(SmsSendTime_ee_adr);
		sprintf(SMS_text,"smssendtime=%d",SmsSendTime);
		SendSMS(RemotePhone,SMS_text);
		
	}
	//--------------------------------------
	else if((str_point=StrSearch(SMS,"logtime=",250))!=NULL){
		str_point1=CharSearch(str_point,',',250);
		*str_point1=0;
		log_time=strtolong(str_point+8);
		if(log_time>=1){
			write_exteeprom_word(log_time,log_time_ee_adr);
			log_time=read_exteeprom_word(log_time_ee_adr);
			next_log_time=(clock.hour*60)+clock.min+log_time;
			if(next_log_time>=1440){next_log_time=next_log_time%1440; flag.log_disable=1;}
			sprintf(SMS_text,"logtime=%d",log_time);
		}
		else{
			strcpy(SMS_text,"Incorrect Format");
		}
		SendSMS(RemotePhone,SMS_text);
	}
	//--------------------------------------------------------
	else if(StrSearch(SMS,"readlogtime",250)!=NULL){
		sprintf(SMS_text,"logtime=%d",log_time);
		SendSMS(RemotePhone,SMS_text);
	}
	//---------------------------------------------
	else if((str_point=StrSearch(SMS,"readlog=",250))!=NULL){
		str_point1=CharSearch(str_point,' ',250);
		str_point+=8;
		if(str_point1==NULL){
				strcpy(SMS_text,"Incorrect Format");
				SendSMS(RemotePhone,SMS_text);
				return;
		}
			
		strcpy(SimSendBuffer,"Log=\r\n");
		
		while(strlen(SimSendBuffer)<160){
			*(str_point1)=0;
			if(strlen(str_point)<20){
				strcpy(log_header,str_point);
				FindLog(log_header,SimSendBuffer);
			}
			else{
				strcat(SimSendBuffer,"Incorrect Format\r\n");
			}
			str_point=str_point1+1;
			str_point1=CharSearch(str_point1+1,' ',250);
			
			if(str_point1==NULL){
				break;
			}
			
		}	
		memcpy(SMS_text,SimSendBuffer,159);//maximum 160 character
		SendSMS(RemotePhone,SMS_text);
					
	}
	//---------------------------------------------
	else if((str_point=StrSearch(SMS,"extdevices=",250))!=NULL){
		str_point1=CharSearch(str_point,',',250);
		*str_point1=0;
		ext_device=strtolong(str_point+11);
		write_exteeprom_dword(ext_device,ext_device_ee_adr);
		ext_device=read_exteeprom_dword(ext_device_ee_adr);
		sprintf(SMS_text,"extdevice=%d",ext_device);
		SendSMS(RemotePhone,SMS_text);
	}
	//----------------------------------------------
	else if((str_point=StrSearch(SMS,"deltawlvl=",250))!=NULL){
		str_point1=CharSearch(str_point,',',250);
		*str_point1=0;
		delta_water_level=strtolong(str_point+10);
		write_exteeprom_word(delta_water_level,DeltaWlvlEEadr);
		delta_water_level=read_exteeprom_word(DeltaWlvlEEadr);
		sprintf(SMS_text,"deltawlvl=%d",delta_water_level);
		SendSMS(RemotePhone,SMS_text);
	}
	//----------------------------------------------------------
	else if((str_point=StrSearch(SMS,"deltrain=",250))!=NULL){
		str_point1=CharSearch(str_point,'.',250);
		*str_point1=0;
		delta_rain_level=(float)strtolong(str_point+9);
		str_point=CharSearch(str_point1,',',250);
		*str_point=0;
		delta_rain_level+=(((float)strtolong(str_point1+1))/100);
		
		write_exteeprom_float(delta_rain_level,DeltaRainLevelEEadr);
		delta_rain_level=read_exteeprom_float(DeltaRainLevelEEadr);
		sprintf(SMS_text,"deltrain=%4.2f",delta_rain_level);
		SendSMS(RemotePhone,SMS_text);
	}
	//---------------------------------------------
	else if((str_point=StrSearch(SMS,"logparameters=",250))!=NULL){
		str_point1=CharSearch(str_point,',',250);
		*str_point1=0;
		LogParameters=strtolong(str_point+14);
		write_exteeprom_word(LogParameters,LogParametersEEadr);
		LogParameters=read_exteeprom_dword(LogParametersEEadr);
		sprintf(SMS_text,"logparameters=%d",LogParameters);
		SendSMS(RemotePhone,SMS_text);
	}
	
	//---------------------------------------------
	else if((str_point=StrSearch(SMS,"wlevel=",250))!=NULL){
		str_point1=CharSearch(str_point,',',250);
		*str_point1=0;
		water_level=strtolong(str_point+7);
		write_exteeprom_word(water_level,water_level_ee_adr);
		water_level=read_exteeprom_word(water_level_ee_adr);
		sprintf(SMS_text,"wlevel=%d",water_level);
		SendSMS(RemotePhone,SMS_text);
	}
	//---------------------------------------------------------------------
  else if((str_point=StrSearch(SMS,"adminmobile=",250))!=NULL){
    str_point+=12;
    str_point1=CharSearch(str_point,',',250);
    if(str_point1!=NULL){
			*str_point1=0;
			write_exteeprom_ary(str_point,strlen(str_point)+1,AdminMobile1_ee_adr);
    }
		
    str_point=str_point1+1;
		
		str_point1=CharSearch(str_point,',',250);
    if(str_point1!=NULL){
			*str_point1=0;
			write_exteeprom_ary(str_point,strlen(str_point)+1,AdminMobile2_ee_adr);
    }
		
		read_exteeprom_ary(AdminMobile,29,AdminMobile1_ee_adr); 
    sprintf(SMS_text,"adminmobile=%s,",AdminMobile);
    read_exteeprom_ary(AdminMobile,29,AdminMobile2_ee_adr);
		strcat(SMS_text,AdminMobile);
		
    SendSMS(RemotePhone,SMS_text);
  } 
	//------------------------------------------
	else if((str_point=StrSearch(SMS,"heaterctrl=",250))!=NULL){
		str_point1=CharSearch(str_point,',',250);
		*str_point1=0;
		heater_control=strtolong(str_point+11);
		write_exteeprom_byte(heater_control,heater_control_ee_adr);
		heater_control=read_exteeprom_byte(heater_control_ee_adr);
		sprintf(SMS_text,"heaterctrl=%d",heater_control);
		SendSMS(RemotePhone,SMS_text);
	}


	//-----------------------------------------------------------------------------
	else if(StrSearch(SMS,"disablealarm",250)!=NULL){
		flag.alarm_disable=1;
    strcpy(SMS_text,"AlarmDisabled");
		SendSMS(RemotePhone_point,SMS_text);

  }
	//-------------------------------------------------------------------------
  else if((str_point=StrSearch(SMS,"uploadfile=",250))!=NULL){//uploadfile=filename.txt
		
		str_point1=CharSearch(SMS,',',250);
		(*str_point1)=0;
		strcpy(buf,str_point+11);
				
		upload_file(buf,RemotePhone);

	}
	//-------------------------------------------------------------------------
  else if((str_point=StrSearch(SMS,"uploaddata",250))!=NULL){
		
		ReadSensorToStr(gprs_buffer);
		if(upload_instant(gprs_buffer,ServerPhone)==0){
			strcpy(SMS_text,"Upload Response\r\nFile uploaded successfully");
			SendSMS(RemotePhone,SMS_text);
			
		}

	}
	//------------------------------------------
	else if((str_point=StrSearch(SMS,"fileuploadinterval=",250))!=NULL){
		str_point1=CharSearch(str_point,',',250);
		*str_point1=0;
		gprs_upload_time=strtolong(str_point+19);
		write_exteeprom_word(gprs_upload_time,gprs_upload_time_ee_adr);
		sprintf(SMS_text,"fileuploadinterval=%d",gprs_upload_time);
		SendSMS(RemotePhone,SMS_text);
	}
	//------------------------------------------
	else if((str_point=StrSearch(SMS,"gprsautoupload=",250))!=NULL){
		str_point1=CharSearch(str_point,',',250);
		*str_point1=0;
		gprs_auto_upload=strtolong(str_point+15);
		write_exteeprom_word(gprs_auto_upload,gprs_auto_upload_ee_adr);
		sprintf(SMS_text,"gprsautoupload=%d",gprs_auto_upload);
		SendSMS(RemotePhone,SMS_text);
	}
	//------------------------------------------
	else if((str_point=StrSearch(SMS,"datauploadtimes=",250))!=NULL){
		str_point+=16;
		for(i=0;i<5;i++){
			str_point1=CharSearch(str_point,',',5);
			if(str_point1!=NULL){
				*str_point1=0;
				upload_time_table[i]=strtolong(str_point);
				write_exteeprom_word(upload_time_table[i],upload_time_table0_ee_adr+(i*2));
				str_point = str_point1+1;
			}
			
		}
		sprintf(SMS_text,"datauploadtimes=%d,%d,%d,%d,%d",upload_time_table[0],upload_time_table[1],upload_time_table[2],upload_time_table[3],upload_time_table[4]);
		SendSMS(RemotePhone,SMS_text);
	}
	
	//------------------------------------------
	else if((str_point=StrSearch(SMS,"ftppathseparator=",250))!=NULL){
		str_point1=CharSearch(str_point,',',250);
		*str_point1=0;
		ftp_path_separator=*(str_point+17);
		write_exteeprom_byte(ftp_path_separator,ftp_path_separator_ee_adr);
		ftp_path_separator=read_exteeprom_byte(ftp_path_separator_ee_adr);
		sprintf(SMS_text,"ftppathseparator=%c",ftp_path_separator);
		SendSMS(RemotePhone,SMS_text);
	}
	//------------------------------------------------------------------
	else if((str_point=StrSearch(SMS,"stname=",250))!=NULL){
		str_point1=CharSearch(str_point,',',250);
		*str_point1=0;
		strcpy(station_name,str_point+7);
		write_exteeprom_ary(station_name,strlen(station_name)+1,station_name_ee_adr);
		read_exteeprom_ary(station_name,30,station_name_ee_adr);
		sprintf(SMS_text,"stname=%s",station_name);
		SendSMS(RemotePhone,SMS_text);
	}
	
 
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void strcopy(char *dst,char *src){
uint16_t len=0;
  len=strlen(src);
  dst[len]=0;
  while((*src)!='\0'){
    (*dst)=(*src);
    src++;
    dst++;
    tgl_wdt;
  }
  
}

//--------------------------------------------------------
void SimChargeCat(char* str){
 char *str_point=NULL,*str_point1=NULL;
	
	str_point=NULL;
  flush_buf(SimRecBuffer,250);
	//sprintf(SimSendBuffer,"ATD%s;%c%c",ChargeReqStr,CRchar,LF);
	sprintf(SimSendBuffer,"AT+CUSD=1,\"%s\"%c%c",ChargeReqStr,CRchar,LF);
	GsmSend(SimSendBuffer);
	delayms(500);
	SimCount=0;
	
	while((str_point==NULL) && (SimCount<10)){
		str_point=StrSearch(SimRecBuffer,"CUSD:",250);
		tgl_wdt;
	}
	if(str_point!=NULL){
		delayms(100);
		str_point1=CharSearch(str_point,LF,250);
		*str_point1=0;
		if(strlen(str_point)>70){
			*(str_point+70)=0;
		}
		memcpy(str+strlen(str),str_point,70);
	}

}

//--------------------------------------------------
void lowercase(char *str,unsigned int size){
unsigned int k=0;
	for(k=0;k<size;k++){
		if(str[k]>=65 && str[k]<=90)
			str[k]+=32;
	}
	
}
//----------------------------------------------------
uint8_t upload_instant(char* data_str,char *PhoneNumber){
 char *str_point=NULL,*str_point1=NULL;
uint16_t packet_size=0;
uint16_t str_length=0;	
	
	if(flag.LcdOn==1){
    LCDclr();
		LCDputs("Uploading ...");
  }
	
	str_length=strlen(data_str);
	
  GsmSend("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r");
  delayms(2000);
	flush_buf(SimRecBuffer,250);
	SimRecLen=0;
  sprintf(buf,"AT+SAPBR=3,1,\"APN\",\"%s\"\r",apn_name);//MTNIRANCELL,mcinet
  GsmSend(buf);
  delayms(2000);
  flush_buf(SimRecBuffer,250);
	SimRecLen=0;
	GsmSend("AT+SAPBR=1,1\r\n");
	str_point=NULL;
  SimCount=0;
	while(SimCount<10 && str_point==NULL){
    str_point=StrSearch(SimRecBuffer,"OK",250);
		tgl_wdt;
    
  }
	delayms(1000);
  flush_buf(SimRecBuffer,250);
	SimRecLen=0;
	GsmSend("AT+SAPBR=2,1\r\n");
  delayms(1000);
  flush_buf(SimRecBuffer,250);
	SimRecLen=0;
  GsmSend("AT+FTPCID=1\r\n");
  delayms(2000);
  sprintf(buf,"AT+FTPSERV=\"%s\"\r",server_url);
  GsmSend(buf);
  delayms(2000);
	flush_buf(SimRecBuffer,250);
	SimRecLen=0;
  sprintf(buf,"AT+FTPUN=\"%s\"\r",server_username);
  GsmSend(buf);
  delayms(2000);
	flush_buf(SimRecBuffer,250);
	SimRecLen=0;
  sprintf(buf,"AT+FTPPW=\"%s\"\r",server_password);
  GsmSend(buf);
  delayms(2000);
	flush_buf(SimRecBuffer,250);
	SimRecLen=0;
	sprintf(buf,"AT+FTPGETPATH=\"%s\"\r",server_path);
  GsmSend(buf);
  delayms(2000);
	flush_buf(SimRecBuffer,250);
	SimRecLen=0;
	strcpy(buf,"AT+FTPMKD\r");
  GsmSend(buf);
  SimCount=0;
	str_point=NULL;
	while(SimCount<10 && str_point==NULL){
    str_point=StrSearch(SimRecBuffer,"+FTPMKD:",250);
		tgl_wdt;
    
  }
	delayms(1000);
	flush_buf(SimRecBuffer,250);
	SimRecLen=0;
  GsmSend("AT+FTPPUTNAME=\"instant.txt\"\r");
	delayms(2000);
	flush_buf(SimRecBuffer,250);
	SimRecLen=0;
  sprintf(buf,"AT+FTPPUTPATH=\"%s\"\r",server_path);
  GsmSend(buf);
  delayms(2000);
	flush_buf(SimRecBuffer,250);
	SimRecLen=0;
  
  
  flush_buf(SimRecBuffer,250);
  SimCount=0;
  SimRecLen=0;
  str_point=NULL;
  GsmSend("AT+FTPPUT=1\r");
  delayms(500);
	
  while(SimCount<40 && str_point==NULL){
    str_point=StrSearch(SimRecBuffer,"+FTPPUT:1",250);
		tgl_wdt;
    
  }
  delayms(2000);
   if(SimCount>=40){
    strcpy(SMS_text,"Upload Response\r\nTIMEOUT IN CONNECTION");
    SendSMS(PhoneNumber,SMS_text);
    delayms(200);
    sprintf(SimSendBuffer,"AT+CMGD=1,4%c%c",CRchar,LF);
    GsmSend(SimSendBuffer);
    delayms(5000);
    simpwr_off;
		delayms(5000);
    
    GsmInit();
    gprs_first_connect=0;
    return 2;
  }
  
  if(*(str_point+10)=='1'){
		str_point1=CharSearch(str_point,CRchar,250);
		if(str_point1!=NULL){
			*str_point1=0;
			str_point+=12;
			packet_size=strtolong(str_point);
			
		}
		else 
			packet_size=512;
		
    enter_critical_section();
    TakePictureCount=0;
                      
		sprintf(buf,"AT+FTPPUT=2,%d\r",str_length);
		flush_buf(SimRecBuffer,250);
		GsmSend(buf);
		SimCount=0;
		SimRecLen=0;
		str_point=NULL;
		while(SimCount<10 && str_point==NULL ){
			str_point=StrSearch(SimRecBuffer,"+FTPPUT:2",250);
			tgl_wdt;  
		}

		flush_buf(SimRecBuffer,250);
		
		GsmSendByte(data_str,str_length);
		
		SimCount=0;
		SimRecLen=0;
		str_point=NULL;
		
		while(SimCount<20 && str_point==NULL){
			str_point=StrSearch(SimRecBuffer,"+FTPPUT:1,1",250);
			tgl_wdt;  
		}
		    
    delayms(3000);
    GsmSend("AT+FTPPUT=2,0\r");
    delayms(3000);
    GsmSend("AT+SAPBR=0,1\r");
    delayms(3000);
		
    exit_critical_section();
		
//		if(SimCount>=20){
//			strcpy(SMS_text,"Upload Response\r\nTimeout in upload");
//			SendSMS(PhoneNumber,SMS_text);			

//		}
//		else{			
//			sprintf(SMS_text,"Upload Response\r\FILE UPLOADED IN : %s",server_url);
//			SendSMS(PhoneNumber,SMS_text);
//		}		
		
		sprintf(SimSendBuffer,"AT+CMGD=1,4%c%c",CRchar,LF);
    GsmSend(SimSendBuffer);
    delayms(5000);
    simpwr_off;
		delayms(5000);
    GsmInit();
    gprs_first_connect=0;
		
		return 0;
  }
    
  else {
    *(str_point+12) =0;
    strcpy(SMS_text,str_point);
    SendSMS(PhoneNumber,SMS_text);
    delayms(200);
    sprintf(SimSendBuffer,"AT+CMGD=1,4%c%c",CRchar,LF);
    GsmSend(SimSendBuffer);
    delayms(5000);
    simpwr_off;
		delayms(5000);
    GsmInit();
    gprs_first_connect=0;
		return 3;
  }

}
//------------------upload file--------------------------
uint8_t upload_file(char* filename,char *PhoneNumber){
	 char *str_point=NULL,*str_point1=NULL;
	uint16_t packet_size=0;
	char file_name[30];
	
	strcpy(file_name,filename);
	
  if(f_open(&File[0],file_name,FA_READ)!=FR_OK){
    strcpy(SMS_text,"Upload Response\r\nFile does not find!");
    SendSMS(PhoneNumber,SMS_text);
    delayms(10000);
		HAL_NVIC_SystemReset();
  }
  
	str_point=file_name;
	while(1){
		if((*str_point)=='/')(*str_point)='_';
		if((*str_point)==0)break;
		str_point++;
	}
	
	if(flag.LcdOn==1){
    LCDclr();
		LCDputs("Uploading ...");
  }
	
  GsmSend("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r");
  delayms(2000);
	flush_buf(SimRecBuffer,250);
	SimRecLen=0;
  sprintf(buf,"AT+SAPBR=3,1,\"APN\",\"%s\"\r",apn_name);//MTNIRANCELL,mcinet
  GsmSend(buf);
  delayms(2000);
  flush_buf(SimRecBuffer,250);
	SimRecLen=0;
	GsmSend("AT+SAPBR=1,1\r\n");
	str_point=NULL;
  SimCount=0;
	while(SimCount<10 && str_point==NULL){
    str_point=StrSearch(SimRecBuffer,"OK",250);
		tgl_wdt;
    
  }
	delayms(1000);
  flush_buf(SimRecBuffer,250);
	SimRecLen=0;
	GsmSend("AT+SAPBR=2,1\r\n");
  delayms(1000);
  flush_buf(SimRecBuffer,250);
	SimRecLen=0;
  GsmSend("AT+FTPCID=1\r\n");
  delayms(2000);
  sprintf(buf,"AT+FTPSERV=\"%s\"\r",server_url);
  GsmSend(buf);
  delayms(2000);
	flush_buf(SimRecBuffer,250);
	SimRecLen=0;
  sprintf(buf,"AT+FTPUN=\"%s\"\r",server_username);
  GsmSend(buf);
  delayms(2000);
	flush_buf(SimRecBuffer,250);
	SimRecLen=0;
  sprintf(buf,"AT+FTPPW=\"%s\"\r",server_password);
  GsmSend(buf);
  delayms(2000);
	flush_buf(SimRecBuffer,250);
	SimRecLen=0;
	sprintf(buf,"AT+FTPGETPATH=\"%s\"\r",server_path);
  GsmSend(buf);
  delayms(2000);
	flush_buf(SimRecBuffer,250);
	SimRecLen=0;
	strcpy(buf,"AT+FTPMKD\r");
  GsmSend(buf);
  SimCount=0;
	str_point=NULL;
	while(SimCount<10 && str_point==NULL){
    str_point=StrSearch(SimRecBuffer,"+FTPMKD:",250);
		tgl_wdt;
    
  }
	delayms(1000);
	flush_buf(SimRecBuffer,250);
	SimRecLen=0;
  sprintf(buf,"AT+FTPPUTNAME=\"%s\"\r",file_name);
  GsmSend(buf);
	delayms(2000);
	flush_buf(SimRecBuffer,250);
	SimRecLen=0;
  sprintf(buf,"AT+FTPPUTPATH=\"%s\"\r",server_path);
  GsmSend(buf);
  delayms(2000);
	flush_buf(SimRecBuffer,250);
	SimRecLen=0;
  
  
  flush_buf(SimRecBuffer,250);
  SimCount=0;
  SimRecLen=0;
  str_point=NULL;
  GsmSend("AT+FTPPUT=1\r");
  delayms(500);
	
  while(SimCount<40 && str_point==NULL){
    str_point=StrSearch(SimRecBuffer,"+FTPPUT:1",250);
		tgl_wdt;
    
  }
  delayms(2000);
   if(SimCount>=40){
    strcpy(SMS_text,"Upload Response\r\nTIMEOUT IN CONNECTION");
    SendSMS(PhoneNumber,SMS_text);
    delayms(200);
    sprintf(SimSendBuffer,"AT+CMGD=1,4%c%c",CRchar,LF);
    GsmSend(SimSendBuffer);
    delayms(5000);
    simpwr_off;
		delayms(5000);
    
    GsmInit();
    gprs_first_connect=0;
    return 2;
  }
  
  if(*(str_point+10)=='1'){
		str_point1=CharSearch(str_point,CRchar,250);
		if(str_point1!=NULL){
			*str_point1=0;
			str_point+=12;
			packet_size=strtolong(str_point);
			
		}
		else 
			packet_size=512;
		
    enter_critical_section();
    TakePictureCount=0;
    while(TakePictureCount<1000){
      tgl_wdt; 
      
      if(f_read(&File[0],MemBuf,packet_size,&FatBytesRead) != FR_OK)break;
      
      sprintf(buf,"AT+FTPPUT=2,%d\r",FatBytesRead);
      flush_buf(SimRecBuffer,250);
      GsmSend(buf);
      SimCount=0;
      SimRecLen=0;
      str_point=NULL;
      while(SimCount<10 && str_point==NULL ){
        str_point=StrSearch(SimRecBuffer,"+FTPPUT:2",250);
        tgl_wdt;  
      }
//      delayms(2000);
//			RS485_send(str_point,20);
//			RS485_send("\r\n",2);
      flush_buf(SimRecBuffer,250);
			
      GsmSendByte(MemBuf,FatBytesRead);
      
      SimCount=0;
      SimRecLen=0;
      str_point=NULL;
      
      while(SimCount<20 && str_point==NULL ){
        str_point=StrSearch(SimRecBuffer,"+FTPPUT:1,1",250);
        tgl_wdt;  
      }
			delayms(200);
			if(FatBytesRead < packet_size) break;
			
			if(str_point!=NULL){
				str_point1=CharSearch(str_point,CRchar,250);
				if(str_point1!=NULL){
					*str_point1=0;
					str_point+=12;
					packet_size=strtolong(str_point);
					
				}
				else 
					packet_size=512;
			}
			else{
				packet_size=512;
			}
      
    }
    
    delayms(3000);
    GsmSend("AT+FTPPUT=2,0\r");
    delayms(3000);
    GsmSend("AT+SAPBR=0,1\r");
    delayms(3000);
		
    exit_critical_section();
		if(TakePictureCount>1000){
			strcpy(SMS_text,"Upload Response\r\nTimeout in upload");
			SendSMS(PhoneNumber,SMS_text);			

		}
		else{			
			strcpy(SMS_text,"Upload Response\r\nFile uploaded successfully");
			SendSMS(PhoneNumber,SMS_text);
		}
		sprintf(SimSendBuffer,"AT+CMGD=1,4%c%c",CRchar,LF);
    GsmSend(SimSendBuffer);
    delayms(5000);
    simpwr_off;
		delayms(5000);
    GsmInit();
    gprs_first_connect=0;
		
		return 0;
  }
  
  
  else {
    *(str_point+12) =0;
    strcpy(SMS_text,str_point);
    SendSMS(PhoneNumber,SMS_text);
    delayms(200);
    sprintf(SimSendBuffer,"AT+CMGD=1,4%c%c",CRchar,LF);
    GsmSend(SimSendBuffer);
    delayms(5000);
    simpwr_off;
		delayms(5000);
    GsmInit();
    gprs_first_connect=0;
		return 3;
  }
  
  
}
