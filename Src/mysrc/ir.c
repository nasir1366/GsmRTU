#include "includes.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defines.h"
#include "ir.h"
#include "global_var.h"
#include "rtc.h"
#include "eeprom_ext.h"
#include "lcdlib.h"
#include "my_delay.h"
#include "gsm.h"
#include "main_func.h"


void setup_prog(void);
void menu_prog(void);
void select(void);
void pass_change(void);
char st_setting(void);
void time_setting(void);
// Variable Definiton------------------------------
unsigned char command_ir;
float bat_v,sol_v;
char v_int,v_float;
unsigned char temp_val,temp,UTP_H,UTP_L,LTP_H,LTP_L;

uint16_t prim_password,prim_station_address,max_water_val,min_water_val,prim_water_level;

signed char key,start_key,end_key,prim_T_low,prim_T_hi;

Rtc prim_clock;

volatile float adc_temp;
char ServerPhone_temp[30];

static const char erase[]="               ";
static const char sel[]="Saved Ok";

//-------STRUCTURE----------------------
//---FLAGS-----------
#pragma anon_unions
struct {
  
  union{
    uint16_t blk1;
    struct{
			//general--------
      uint16_t setup:1;
      uint16_t setup_main:1;
      uint16_t menu:1;
      uint16_t first_menu:1;
			uint16_t station_sett:1;
      uint16_t st_set:1;
      uint16_t st_set_main:1;
			uint16_t display_adr:1;
      uint16_t display_sensor:1;
			uint16_t invalid_remote:1;
			//password ---------------
      uint16_t pas:1;
      uint16_t chang_pas:1;
      uint16_t change_pass_routin:1;
			uint16_t new_pas:1;
      uint16_t emph:1;
      uint16_t pas_ok:1;
			
    };
  };
  
  union{
    uint16_t blk2;	
    struct{	
			//clock rtc-----------
      uint16_t time_sett_main:1;
      uint16_t time_sett:1;
      uint16_t time_set_enter:1;
      uint16_t t:1;
      uint16_t d:1;
			//temperature------------
      uint16_t temp_setting:1;
      uint16_t temp_set_main:1;
			uint16_t temp_calib:1;
			//adc calibration----------
			uint16_t ADCcalibration:1;
      uint16_t adc_calib_main:1;
			uint16_t adc_calib_firstpage:1;
      uint16_t adc1_calib:1;      
      uint16_t adc2_calib:1;
      uint16_t adc3_calib:1;
    };
  };
  
  union{
    uint16_t blk3;			
    struct{
			//add device
			uint16_t select_sensor:1;
			uint16_t add_external_device:1; 
      uint16_t add_device_main:1;
			// snow motor---------
      uint16_t snow_test:1;
			//station -------------
			uint16_t read_st:1;
      uint16_t read_st_main:1;      
			//water sensor--------
			uint16_t water_set_main:1;
      uint16_t max_water:1;
			uint16_t min_water:1;
			uint16_t sensor_level:1;
      uint16_t water_sensor_setting:1;
      uint16_t WaterSensorMain:1;
      uint16_t MaxCurrent:1;
      uint16_t MinCurrent:1;
			uint16_t MicrosonicMain:1;
			uint16_t microsonic_setting:1;
    };	
  };	
  union{
    uint16_t blk4;
    struct{
			
      //gsm module------------
      uint16_t Gsm_Setting:1;
      uint16_t Gsm_Main:1;
      uint16_t server_phone:1;
			uint16_t adminphone2:1;
			uint16_t adminphone1:1;
			//general-----------
			uint16_t in_number:1;
			uint16_t type_sel:1;
			//rain sensor------      	
      uint16_t heater_temp_low:1;
      uint16_t heater_temp_hi:1;
      uint16_t RainSensorMain:1;
			uint16_t Rain_sensor_setting:1;
			
			//---adc calibration------
			uint16_t adc4_calib:1;      
      uint16_t adc5_calib:1;
      uint16_t current_calib:1;
			
						     
    };	
  };
	union{
    uint16_t blk5;
    struct{
			//-----log time-----------
			uint16_t log_prog:1;
      uint16_t log_first_page:1;
      uint16_t log_main:1;
      uint16_t log_interval:1;
			uint16_t sample_cou:1;
      uint16_t SetWaterLevelBarMain:1;
			uint16_t SetWaterLevelBar:1;
			//-----Water Setting-----------
			uint16_t RotaryMain:1;
			uint16_t Rotary_setting:1;
			uint16_t rotary_resolution:1;
			uint16_t rotary_diameter:1;
			uint16_t rotary_set_level:1;
			
    };	
  };
	
	
	 
  
} volatile flag_ir;

//-----------------keypad----------
uint8_t key_pressed;


char str[12][35]; //KEY buffer for display

//-----------------scan_key--------------------------
void scan_key(void){
	
	if(left_key==0){
			delayms(debounce_key);
			if(left_key==0){
				
				SetLevelBar_count++;
				SetLevelBar_timeout=0;
				if(SetLevelBar_count>20){
					flag.ir_flag=1;
					flag_ir.SetWaterLevelBarMain=1;
					flag_ir.water_sensor_setting=1;
					SetLevelBar_count=0;
				}
				
				key_pressed=1;
				command_ir=left;	
				
			}	
			
		}
		else if(menu_key==0){
			delayms(debounce_key);
			if(menu_key==0){
				flag.ir_flag=1;
				key_pressed=1;
				command_ir=MENU;
								
			}	
			
		}
		
		else if(down_key==0){
			delayms(debounce_key);
			if(down_key==0){
				
				key_pressed=1;
				command_ir=down;
							
			}	
			
		}
		
		else if(right_key==0){
			delayms(debounce_key);
			if(right_key==0){
				key_pressed=1;
			  command_ir=right;
				
			}	
			
		}
		else if(ok_key==0){
			delayms(debounce_key);
			if(ok_key==0){
				command_ir=ok;
				key_pressed=1;
			}	
			
		}
		
		else if(esc_key==0){
			delayms(debounce_key);
			if(esc_key==0){
								
				user_sms_count++;
				user_sms_timeout=0;
				if(user_sms_count>20){
					flag.UserCheck=1;
					user_sms_count=0;
				}
									
				key_pressed=1;
				command_ir=exit;
			}	
			
		}
		
		else if(up_key==0){
			delayms(debounce_key);
			if(up_key==0){
				command_ir=up;
				key_pressed=1;
			}	
			
		}
		
		if(key_pressed == 1){
			key_pressed=0;
			
			DisplayTime=0;
			time_out=0;
			flag.ir_flag=1;
			
			if(flag.LcdOn==0){
        
        lcd_pwr_on;
        delayms(20);
        LCDinit();
        delayms(20);
        LCDclr();
        flag.LcdOn=1;
        flag.ir_flag=0; 
        display_count=4;
				if(valid_door_flag==2){
					LCDputs("INVALID USER\n     [     ]    ");
					LCDcursorOn();
					LCDGotoXY(6,1);
					strcpy(input_number_str,"00000");
					LCDputs(input_number_str);
					LCDGotoXY(6,1);
					number_limit=5;
					number_limit_index=0;
				}
      }
			
			processing_ir_keypad();
			command_ir=0;
		}
		

}
//-----------------input number---------------
void input_number(){
	
	if(command_ir==up){
		input_number_str[number_limit_index]++;
		LCDsendChar(input_number_str[number_limit_index]);
		LCDcursorLeft(1);
	}
	else if(command_ir==down){
		input_number_str[number_limit_index]--;
		LCDsendChar(input_number_str[number_limit_index]);
		LCDcursorLeft(1);
	}
	else if(command_ir==left){
		if(number_limit_index>0){
						
			LCDcursorLeft(1);
			input_number_str[number_limit_index-1]=input_number_str[number_limit_index];
			number_limit_index--;
			LCDsendChar(input_number_str[number_limit_index]);
			LCDcursorLeft(1);
		}		
			
	}
	else if(command_ir==right){
		if(number_limit_index<(number_limit-1)){
						
			LCDcursorRight(1);
			input_number_str[number_limit_index+1]=input_number_str[number_limit_index];
			number_limit_index++;
			LCDsendChar(input_number_str[number_limit_index]);
			LCDcursorLeft(1);
		}
	}
	

}
//**************************************Processing IR recieved data************************************************
unsigned char processing_ir_keypad(void){
  
  time_out=0;
	flag.menu_setting=1;
	
	if(flag.open_door==1 && command_ir==MENU){
		
		LCDclr();    
		LCDputsf("INPUT PASSWORD:\n     [     ]    ");
		LCDcursorOn();
		LCDGotoXY(6,1);
		strcpy(input_number_str,"00000");
		LCDputs(input_number_str);
		LCDGotoXY(6,1);
		number_limit=5;
		number_limit_index=0;
		
		flag.input_init_pass=1;
			
	}
	//------------------------input init password--------------------------------
  if(flag.input_init_pass==1){
		
		time_out_pass=0;
		input_number();
		if(command_ir==ok){
			input_pass=strtolong(input_number_str);
			if(input_pass==password){
				flag.input_init_pass=0;
				flag.open_door=0;
				command_ir=MENU;
			}
			else{
				LCDclr();    
				LCDputsf("INPUT PASSWORD:\n     [     ]    ");
				LCDcursorOn();
				LCDGotoXY(6,1);
				strcpy(input_number_str,"00000");
				LCDputs(input_number_str);
				LCDGotoXY(6,1);
				number_limit=5;
				number_limit_index=0;
			}
			
			if(valid_door_flag==2){
				if(password!=input_pass){
					flag.input_init_pass=1;
					number_limit_index=0;
					LCDGotoXY(6,1);
					strcpy(input_number_str,"00000");
					LCDputs(input_number_str);
					LCDGotoXY(6,1);
				}
			}
			
		}
		else if(command_ir==exit){
			flag.input_init_pass=0;
			flag.open_door=1;
			flag.main_menu=1;
			flag.ir_flag=0;
			flag.menu_setting=0;
		}
    
  }	
  //---------------------------------------------------------------------
  if(flag.open_door==0){
		
    if((command_ir==up || command_ir==down) && flag_ir.in_number==0){ 
      select();
     }
    
    else if(command_ir==MENU){
      flag_ir.first_menu=1;
      menu_prog();
    } 
    
       
    else if(flag_ir.menu==1 || flag_ir.display_adr==1 || flag_ir.display_sensor==1) menu_prog();
    
    else if(flag_ir.setup==1) setup_prog();
    
    else if (flag_ir.change_pass_routin==1) pass_change();
    
    else if(flag_ir.station_sett==1) st_setting();
		
		else if(flag_ir.time_sett==1) time_setting();
    
    
		else if(command_ir==exit){
			flag.ir_flag=0;
			clear_ir_flags();
		}
			
	}
	
  
  return 0;	
}

//***********************************************************************
void setup_prog (void){
  
  if(flag_ir.setup_main==1){
    
    clear_ir_flags();
    end_key=8;
    start_key=4;
    key=4;
    flag_ir.setup_main=0;
    flag_ir.setup=1;
    
    static const char str_f1[]="Change Pass";
    static const char str_f2[]="Station Sett";  
    static const char str_f3[]="Time Sett";
    static const char str_f4[]="Clear Mem" ;  
		static const char str_f5[]="Reboot" ;
    
    strcpy(str[4],str_f1);
    strcpy(str[5],str_f2);
    strcpy(str[6],str_f3);
    strcpy(str[7],str_f4);
		strcpy(str[8],str_f5);
        
    LCDclr();
    LCDputs(str[4]);
    LCDGotoXY(15,0);
    LCDsendChar(up_arrow);
    LCDGotoXY(15,1);
    LCDsendChar(down_arrow);
    
  }
  
  else if(flag_ir.setup==1){
    
    
    if(command_ir==ok)
      
    {
      
      switch(key){
      case 4:
        
        flag_ir.chang_pas=1;
        flag_ir.setup=0;
        pass_change();
        break;
        
      case 5:
        
        flag_ir.st_set_main=1;
        flag_ir.setup=0;
        st_setting();									
        break;
        
              
      case 6:
        flag_ir.time_sett_main=1;
        flag_ir.setup=0;	
        time_setting();
        break;
        
      case 7:
        clear_eeprom();
        LCDclr();
        LCDputs("Clear Ok!");
        delayms(1000);
        LCDclr();
        LCDputs("Device Reset");
        HAL_NVIC_SystemReset();
			case 8:
        LCDclr();
        LCDputs("Device Reset");
        HAL_NVIC_SystemReset();
      }	
    }
    
    else if(command_ir==exit){
      flag_ir.first_menu=1;
      menu_prog();
    }		
  }
  
}

void pass_change(void){
  
  if(flag_ir.chang_pas==1){
    
    flag_ir.pas=1;
    flag_ir.chang_pas=0;
    flag_ir.change_pass_routin=1;
    key=0;
    ir_cou=0;
    input_pass=0;
    LCDclr();
    static const char str_f6[]="Enter Old Pass:\n     [     ]    ";
    LCDputsf(str_f6);
		
		LCDcursorOn();
		LCDGotoXY(6,1);
		
		strcpy(input_number_str,"00000");
		LCDputs(input_number_str);
		LCDGotoXY(6,1);
		number_limit=5;
		number_limit_index=0;
		flag_ir.in_number=1;
  }	
  else if(flag_ir.pas==1){
    
		input_number();
		if(command_ir==ok){
			input_pass=strtolong(input_number_str);
			if(input_pass==password){
        delayms(200);
        LCDclr();
        static const char str_f7[]="Enter New Pass:\n     [     ]    ";
        LCDputsf(str_f7);
        flag_ir.new_pas=1;
        flag_ir.pas=0;
        input_pass=0;
        LCDcursorOn();
				LCDGotoXY(6,1);
				
				strcpy(input_number_str,"00000");
				LCDputs(input_number_str);
				LCDGotoXY(6,1);
				number_limit=5;
				number_limit_index=0;
				flag_ir.in_number=1;
      }
      else{
        LCDclr();
        static const char str_f8[]="WRONG PASS!";
        LCDputsf(str_f8);
				flag_ir.chang_pas=1;
        delayms(1000);
        pass_change();
        
      }
			
		}
		else if(command_ir==exit){
			flag_ir.setup_main=1;
			setup_prog();
		}    
   
  }
  //-----------------------------------------------------
  else if(flag_ir.new_pas==1){  
    
    input_number();
    if(command_ir==ok){
			input_pass=strtolong(input_number_str);
			LCDclr();
      static const char str_f9[]="REWRITE PASS:\n     [     ]    ";
      LCDputsf(str_f9);
      LCDcursorOn();
			LCDGotoXY(6,1);
			
			strcpy(input_number_str,"00000");
			LCDputs(input_number_str);
			LCDGotoXY(6,1);
			number_limit=5;
			number_limit_index=0;
			flag_ir.in_number=1;
      prim_password=input_pass;
			input_pass=0;
      flag_ir.emph=1;
      flag_ir.new_pas=0;
      delayms(200);
									
		}
		else if(command_ir==exit){
			flag_ir.setup_main=1;
			setup_prog();
		}
		
    
  }
  
  //---------------------------------------------------------------------
  
  else if(flag_ir.emph==1){
    
    input_number();
    if(command_ir==ok){
			input_pass=strtolong(input_number_str);
      if(input_pass==prim_password){
        flag_ir.emph=0;
        flag_ir.pas_ok=1;
        input_pass=0;
        delayms(200);
        LCDclr();
				LCDputs("SAVE OK");
				password=prim_password;
				flag_ir.pas_ok=0;
				prim_password=0;
				write_exteeprom_word(password,password_ee_adr);
				write_exteeprom_byte(0,32);
				flag_ir.change_pass_routin=0;
				flag_ir.setup_main=1;
				delayms(1000);
				setup_prog();        
        
      }
      else{
        flag_ir.emph=0;
        flag_ir.pas=0;
        ir_cou=0;
        input_pass=0;
        LCDclr();
        static const char str_f11[]="False Input";
        LCDputsf(str_f11);
        flag_ir.chang_pas=1;
        delayms(1000);
        pass_change();
        
      }
    }
		
		else if(command_ir==exit){
			flag_ir.setup_main=1;
			setup_prog();
		}
  }	
  //------------------------------------------
  
  else if(command_ir==exit){
    flag_ir.setup_main=1;
    setup_prog();
  }
  
}
//---------------------------------------------
char st_setting(void){
  
  if(flag_ir.st_set_main==1){
    
    ir_cou=0;
    key=5;
    start_key=5;
    flag_ir.st_set=1;
    flag_ir.station_sett=1;
    flag_ir.st_set_main=0;
    end_key=6;
    
		static const char str_f1[]="Test Mode" ;
		static const char str_f2[]="LoadSetting" ;	
		
    strcpy(str[5],str_f1);
    strcpy(str[6],str_f2);
		
		
    LCDclr();
    LCDputs(str[5]);
    LCDGotoXY(15,0);
    LCDsendChar(up_arrow);
    LCDGotoXY(15,1);
    LCDsendChar(down_arrow);
  }
  //----------------------------		 
  else if(command_ir==ok){
    
    if(flag_ir.st_set==1){
      ir_cou=0;
      LCDclr();			
      switch(key){
              
     
			case 5:
					if(flag.test_mode==0){
						flag.test_mode=1;
						test_mode_timeout=0;
						LCDGotoXY(0,1);
						LCDputs("Test Mode Active");
					}
					else{
						flag.test_mode=0;
						LCDGotoXY(0,1);
						LCDputs("Test Mode Deactive");
					}
			break;
			case 6:
					loadSettingSD();
					LCDGotoXY(0,1);
					LCDputs("Setting Loaded");
					delayms(1000);
					HAL_NVIC_SystemReset();
			break;
			
      }
    }
    
    
       
  }
  
  else if(command_ir==exit){
    if(flag_ir.st_set==1) {
      clear_ir_flags();
      flag_ir.setup_main=1;
      setup_prog();
    }
     
    
    
  }
  
  return 0;
}
/**********************************************************************/
void menu_prog(void){
 
static const char str_f1[]="Setup";
static const char str_f2[]="Display Spec";
static const char str_f3[]="Display Sensor";
static const char str_f4[]="Relay 2";

  
  if(flag_ir.first_menu==1){
    
    clear_ir_flags();
    end_key=7;
    start_key=4;
    key=4;
    flag_ir.menu=1;
    flag_ir.first_menu=0;
    strcpy(str[4],str_f1);
    strcpy(str[5],str_f2);
    strcpy(str[6],str_f3);
    strcpy(str[7],str_f4);
    
    
    LCDclr();
    LCDputs(str[4]);
    LCDGotoXY(15,0);
    LCDsendChar(up_arrow);
    LCDGotoXY(15,1);
    LCDsendChar(down_arrow);
        
  }
  
  else if(flag_ir.menu==1){
    if(command_ir==ok){
      switch (key){
      case 4:
        flag_ir.menu=0;
        flag_ir.setup_main=1;
        setup_prog();
        break;
      case 5:
        for(i=4;i<10;i++)strcpy(str[i],"");
        flag_ir.display_adr=1;
        flag_ir.menu=0;
        end_key=6;
        start_key=4;
        key=4;
        static const char str_f6[]="Device ID:\n";
        strcpy(str[4],str_f6);
        sprintf(str[4]+11,"%d",station_address);
        LCDclr();
        LCDputs(str[4]);
        LCDGotoXY(15,0);
        LCDsendChar(up_arrow);
        LCDGotoXY(15,1);
        LCDsendChar(down_arrow);
			
				sprintf(str[5],"Device Name\n%s",station_name);
				sprintf(str[6],"VERSION\n%s",firmware_version);
							
			
			/*
        static const char str_f7[]="St Type:Rep";
        static const char str_f8[]="St Type:End_St";
        
        if(st_type==rep)
          
          strcpy(str[5],str_f7);
        else
          
          strcpy(str[5],str_f8);
        
          static const char str_f9[]="Base Addr:";
          strcpy(str[6],str_f9);
          sprintf(str[6]+10,"\n%d %d %d %d",*((uint16_t*)(base_address+1)),*((uint16_t*)(base_address+3)),*((uint16_t*)(base_address+5)),*((uint16_t*)(base_address+7)));
        
          
        static const char str_f10[]="POWER TYPE =\n line";
        static const char str_f11[]="POWER TYPE =\n Solar";
        static const char str_f12[]="POWER TYPE =\n Solar & Line";
        static const char str_f13[]="POWER TYPE =\n No Power";
                
        switch(pwr_existence){
        case 1:
          strcpy(str[7],str_f10);
          
          break;
        case 2:
          strcpy(str[7],str_f11);
          
          break;
        case 3:
          strcpy(str[7],str_f12);
          break;
        default :
          strcpy(str[7],str_f13);
          
        }
        */
        break;
        
      case 6:
         
        for(i=4;i<10;i++)strcpy(str[i],"");
        end_key=3;
        start_key=4;
        key=4;
        flag_ir.display_sensor=1;
        flag_ir.menu=0;
        
                
				if(ext_device & water_sensor){
					end_key++;
					read_water_level();
					sprintf(str[end_key],"Water_Lvl= %d",water_level);
					
				}
				if(ext_device & rain_sensor){
					end_key++;
					sprintf(str[end_key],"Rain_Lvl=\n%5.3f",rain_level);
					
        }
				
//				if(ext_device & gsm){
//          end_key++;
//          flush_buf(SimRecBuffer,250);
//          SimRecLen=0;
//          sprintf(SimSendBuffer,"AT+CSQ%c%c",CRchar,LF);
//          GsmSend(SimSendBuffer);
//          delayms(1000);
//          point=CharSearch(SimRecBuffer,LF,250);
//          if(point!=NULL){
//            *(point+11)=0;
//            gsm_signal=(unsigned char)strtol(point+8,endptr,10);
//            gsm_signal=(gsm_signal*2)-114;
//            sprintf(str[end_key],"GSM SIGNAL:\n%d dbm",gsm_signal);
//          }                  
//        }
        
        break;
            
      case 7:
        
        if(flag_ir.snow_test==0){
          flag_ir.snow_test=1;
          snow_motor_on;
          LCDGotoXY(0,1);
          LCDputs("ON ");
                    
        }
        else{
          flag_ir.snow_test=0;
          snow_motor_off;
          LCDGotoXY(0,1);
          LCDputs("OFF");
        }
       break; 
        
        
      }
      
      
    }
    else if(command_ir==exit){
      
      flag.disp = 0;
      display_count=4;
			flag.main_menu=1;
      flag.ir_flag=0;
      flag_ir.menu=0;
			flag.open_door=1;
      clear_ir_flags();
			flag.menu_setting=0;
      
    }
    
    
  }
  else if(command_ir==exit) {
    if(flag_ir.display_adr==1 || flag_ir.display_sensor==1){
      flag_ir.first_menu=1;
      menu_prog();
    }
    
  }
  
}


//-----------------------------------------------------
void select(void){
  
  if(key!=0){
    
    
    if(command_ir==down){
      
      key++;
      if(key>end_key) key=start_key;
      
    }
    
    if(command_ir==up){
      
      key--;
      if(key<start_key) key=end_key;
      
    }
    LCDclr();
    LCDputs(str[key]);
    LCDGotoXY(15,0);
    LCDsendChar(up_arrow);
    LCDGotoXY(15,1);
    LCDsendChar(down_arrow);
    
  }
  
}

void clear_ir_flags(void){
  
  flag_ir.blk1=0;
  flag_ir.blk2=0;
  flag_ir.blk3=0;
  flag_ir.blk4=0;
	flag_ir.blk5=0;
  key=0;
  start_key=0;
  end_key=0;
	flag_ir.in_number=0;
  
}
//*********************************************************************
char IR_Learning(void){
  
char index=0;  
  
  flag.ir_learn=1;
  LCDclr();
  static const char str_f1[]="IR LEARNING";   
  LCDputsf(str_f1);
  
  for(index=0;index<7;index++){
   
	switch (index){
		case 0:
			LCDGotoXY(0,1);
			LCDputsf(erase);
			static const char str_f3[]="Press \"MENU\"";
			LCDGotoXY(0,1);
			LCDputsf(str_f3);  
			
			break;
		case 1:
			LCDGotoXY(0,1);
			LCDputsf(erase);
			static const char str_f4[]="Press \"UP\"";
			LCDGotoXY(0,1);
			LCDputsf(str_f4); 
			break;
		case 2:
			LCDGotoXY(0,1);
			LCDputsf(erase);
			static const char str_f5[]="Press \"DOWN\"";
			LCDGotoXY(0,1);
			LCDputsf(str_f5); 
			break;
		case 3:
			LCDGotoXY(0,1);
			LCDputsf(erase);
			static const char str_f6[]="Press \"OK\"";
			LCDGotoXY(0,1);
			LCDputsf(str_f6); 
			break;  
		case 4:
			LCDGotoXY(0,1);
			LCDputsf(erase);
			static const char str_f7[]="Press \"LEFT\"";
			LCDGotoXY(0,1);
			LCDputsf(str_f7); 
			break;
		case 5:
			LCDGotoXY(0,1);
			LCDputsf(erase);
			static const char str_f8[]="Press \"RIGHT\"";
			LCDGotoXY(0,1);
			LCDputsf(str_f8); 
			break;
		case 6:
			LCDGotoXY(0,1);
			LCDputsf(erase);
			static const char str_f9[]="Press \"ECS\"";
			LCDGotoXY(0,1);
			LCDputsf(str_f9); 
			break;
        
        
    }
      
      count=0;
      while((flag.ir_flag==0) && count<10)delayms(1);
      if(flag.ir_flag==1){
        button[index]=data_ir;
        data_ir=0;
        flag.ir_flag=0;
      }
      else return 0;
   
    
  }
  
  write_exteeprom_ary((char*)button,30,button_ir_ee_adr);
  
  
  LCDclr();
  static const char str_f10[]="LEARN OK";
  LCDputsf(str_f10); 
  delayms(2000);
  return 1;
}

//----------------------------------------------------
void loadSettingSD(void){
	char *str_point=NULL,*str_point1=NULL;
	uint32_t filesize=0;
	
	mmc_pwr_on;
	f_mount(&SDFatFs, (TCHAR const*)SDPath, 1);
	
	if(f_open(&File[0],"setting.txt",FA_READ)!=FR_OK) {
		LCDclr();
		LCDputs("File Open Fail");
    delayms(1000);
    mmc_pwr_off;
    return;
  }
	filesize=f_size(&File[0]);
	if(f_read(&File[0],MemBuf,filesize,&FatBytesRead) != FR_OK){
		LCDclr();
		LCDputs("File Read Fail");
    delayms(1000);
    mmc_pwr_off;
    return;
	}
	//-------------------------------------------------
	if((str_point=StrSearch(MemBuf,"extdevices=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		ext_device=strtolong(str_point+11);
		write_exteeprom_dword(ext_device,ext_device_ee_adr);
		ext_device=read_exteeprom_dword(ext_device_ee_adr);
		
	}
	//----------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"temptype=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		temp_type=strtolong(str_point+9);
		write_exteeprom_dword(temp_type,temp_type_ee_adr);
		temp_type=read_exteeprom_dword(temp_type_ee_adr);
		
	}
	
	//----------------------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"watersensortype=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		WaterSensorType=strtolong(str_point+16);
		write_exteeprom_byte(WaterSensorType,water_type_ee_adr);
		WaterSensorType=read_exteeprom_byte(water_type_ee_adr);
	}
	//-------------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"deltawlvl=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		delta_water_level=strtolong(str_point+10);
		write_exteeprom_word(delta_water_level,DeltaWlvlEEadr);
		delta_water_level=read_exteeprom_word(DeltaWlvlEEadr);
		
	}
	//----------------------------------------------------
	if((str_point=StrSearch(MemBuf,"Thi=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		T_hi=strtolong(str_point+4);
		write_exteeprom_byte(T_hi,T_hi_ee_adr);
		T_hi=read_exteeprom_byte(T_hi_ee_adr);
	}
	//------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"Tlow=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		T_low=strtolong(str_point+5);
		write_exteeprom_byte(T_low,T_low_ee_adr);
		T_low=read_exteeprom_byte(T_low_ee_adr);
	}
	
	//------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"raintype=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		rain_sensor_type=strtolong(str_point+9);
		write_exteeprom_byte(rain_sensor_type,rain_type_ee_adr);
		rain_sensor_type=read_exteeprom_byte(rain_type_ee_adr);
	}
	
	//------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"raineventtimeout=",filesize))!=NULL){
    str_point+=17;
    str_point1=CharSearch(str_point,',',filesize);
    *str_point1=0;
    RainEventTime=strtolong(str_point);
    
    write_exteeprom_word(RainEventTime,RainEventTime_ee_adr);
    
  }
  //---------------------------------------------------------------------
  if((str_point=StrSearch(MemBuf,"rainzerotime=",filesize))!=NULL){
    str_point+=13;
    str_point1=CharSearch(str_point,':',filesize);
    
    *str_point1=0;
    RainRstTime=(strtolong(str_point))*60;
    str_point=CharSearch(str_point1,',',filesize);
		*str_point=0;
		RainRstTime+=strtolong(str_point1+1);
		
    write_exteeprom_word(RainRstTime,RainRstTime_ee_adr);
    RainRstTime=read_exteeprom_word(RainRstTime_ee_adr);
		
  }
  //----------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"deltarain=",filesize))!=NULL){
		str_point1=CharSearch(str_point,'.',filesize);
		*str_point1=0;
		delta_rain_level=(float)strtolong(str_point+10);
		str_point=CharSearch(str_point1,',',filesize);
		*str_point=0;
		delta_rain_level+=(((float)strtolong(str_point1+1))/100);
		
		write_exteeprom_float(delta_rain_level,DeltaRainLevelEEadr);
		delta_rain_level=read_exteeprom_float(DeltaRainLevelEEadr);
		
	}
	//------------------------------------------------
	if((str_point=StrSearch(MemBuf,"rainlevel=",filesize))!=NULL){
		str_point1=CharSearch(str_point,'.',filesize);
		*str_point1=0;
		rain_level=(float)strtolong(str_point+10);
		str_point=CharSearch(str_point1,',',filesize);
		*str_point=0;
		rain_level+=(((float)strtolong(str_point1+1))/100);
		
		write_exteeprom_float(rain_level,rain_level_ee_adr);
		rain_level=read_exteeprom_float(rain_level_ee_adr);
		
	}
	
	//--------------------------------------------------------
	if((str_point1=StrSearch(MemBuf,"rtcsett=",filesize))!=NULL){
        
    str_point=StrSearch(str_point1,"year:",filesize);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',filesize);
      *str_point1=0;
      clock.year=strtolong(str_point+5);
      
    }
    
    str_point=StrSearch(str_point1,"mon:",filesize);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',filesize);
      *str_point1=0;
      clock.month=strtolong(str_point+4);
    }
    
    
    str_point=StrSearch(str_point1,"day:",filesize);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',filesize);
      *str_point1=0;
      clock.day=strtolong(str_point+4);
    }
    
    
    str_point=StrSearch(str_point1,"hour:",filesize);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',filesize);
      *str_point1=0;
      clock.hour=strtolong(str_point+5);
    }
    
    
    str_point=StrSearch(str_point1,"min:",filesize);
    if(str_point != NULL){
      str_point1=CharSearch(str_point,',',filesize);
      *str_point1=0;
      clock.min=strtolong(str_point+4);
    }
        
    rtc_settime_shamsi(&clock);
    
    rtc_gettime_shamsi(&clock);
    
    
  }
  	
//--------------------------------------------------------------

	if((str_point=StrSearch(MemBuf,"chargereq=",filesize))!=NULL){
        
    str_point+=10;
    str_point1=CharSearch(str_point,',',filesize);
		if(str_point1 != NULL){
			*str_point1=0;
			strcpy(ChargeReqStr,str_point);
			write_exteeprom_ary(ChargeReqStr,strlen(ChargeReqStr)+1,ChargeReqStr_ee_adr);
			read_exteeprom_ary(ChargeReqStr,strlen(ChargeReqStr)+1,ChargeReqStr_ee_adr);
		}
						
  }	
//-------------------------------------------------------------------	
	if((str_point=StrSearch(MemBuf,"centerphone=",filesize))!=NULL){
    str_point+=12;
    str_point1=CharSearch(str_point,',',filesize);
    
    *str_point1=0;
    strcpy(ServerPhone,str_point);
    
    write_exteeprom_ary(ServerPhone,strlen(ServerPhone)+1,server_phone_ee_adr);
    
    read_exteeprom_ary(ServerPhone,29,server_phone_ee_adr); 
        
  } 
  
	//-----------------------------------------------------
	if((str_point=StrSearch(MemBuf,"smssendtime=",filesize))!=NULL){
		str_point1=CharSearch(str_point,':',filesize);
		*str_point1=0;
		SmsSendTime=(strtolong(str_point+12))*60;
			
    str_point=CharSearch(str_point1,',',filesize);
		*str_point=0;
		SmsSendTime+=strtolong(str_point1+1);
		
		write_exteeprom_word(SmsSendTime,SmsSendTime_ee_adr);
		SmsSendTime=read_exteeprom_word(SmsSendTime_ee_adr);
				
	}
//-------------------------------------------------------------------------
   
  if((str_point=StrSearch(MemBuf,"ftpurl=",filesize))!=NULL){
        
		str_point1=CharSearch(str_point1,',',filesize);
		*str_point1=0;
		strcpy(server_url,str_point+7);
		write_exteeprom_ary(server_url,strlen(server_url)+1,server_url_ee_adr);
		write_exteeprom_byte(0,server_changed1_ee_adr);
	}
//-----------------------------------------------------------    
    
  if((str_point=StrSearch(MemBuf,"ftpuname=",filesize))!=NULL){
      str_point1=CharSearch(str_point,',',filesize);
      *str_point1=0;
      strcpy(server_username,str_point+9);
			write_exteeprom_ary(server_username,strlen(server_username)+1,server_username_ee_adr);
			write_exteeprom_byte(0,server_changed1_ee_adr);
	}
//--------------------------------------------------------    
    
    if((str_point=StrSearch(MemBuf,"ftppass=",filesize))!=NULL){
      str_point1=CharSearch(str_point,',',filesize);
      *str_point1=0;
      strcpy(server_password,str_point+8);
			write_exteeprom_ary(server_password,strlen(server_password)+1,server_password_ee_adr);
			write_exteeprom_byte(0,server_changed1_ee_adr);
		}
    
         
  //-------------------------------------------------------------------------------
 if((str_point=StrSearch(MemBuf,"ftpfilename=",filesize))!=NULL){
        
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		strcpy(server_filename,str_point+12);
		write_exteeprom_ary(server_filename,strlen(server_filename)+1,server_filename_ee_adr);
		write_exteeprom_byte(0,server_changed2_ee_adr);
  }
    
  //---------------------------------------------------
  if((str_point=StrSearch(MemBuf,"ftppath=",filesize))!=NULL){
        
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		strcpy(server_path,str_point+8);
				
		write_exteeprom_ary(server_path,strlen(server_path)+1,server_path_ee_adr);
		write_exteeprom_byte(0,server_changed2_ee_adr);
		
  }
	//---------------------------------------------------
  if((str_point=StrSearch(MemBuf,"apn=",filesize))!=NULL){
        
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		strcpy(apn_name,str_point+4);
		write_exteeprom_ary(apn_name,strlen(apn_name)+1,apn_name_ee_adr);
		write_exteeprom_byte(0,server_changed2_ee_adr);
		
  }
   
	
//-----------------------------------------------------------------
   
	if((str_point=StrSearch(MemBuf,"logtime=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		log_time=strtolong(str_point+8);
		write_exteeprom_word(log_time,log_time_ee_adr);
		log_time=read_exteeprom_word(log_time_ee_adr);
		next_log_time=(clock.hour*60)+clock.min+log_time;
    if(next_log_time>=1440){next_log_time=next_log_time%1440; flag.log_disable=1;}
		
	}
	
	//---------------------------------------------
	if((str_point=StrSearch(MemBuf,"LogParameters=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		LogParameters=strtolong(str_point+14);
		write_exteeprom_word(LogParameters,LogParametersEEadr);
				
	}
	//---------------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"adminmobile1=",filesize))!=NULL){
		str_point+=13;
    str_point1=CharSearch(str_point,',',filesize);
    
    *str_point1=0;
    
    write_exteeprom_ary(str_point,strlen(str_point)+1,AdminMobile1_ee_adr);
        				
	}
	//---------------------------------------------------------------
	if((str_point=StrSearch(MemBuf,"adminmobile2=",filesize))!=NULL){
		str_point+=13;
    str_point1=CharSearch(str_point,',',filesize);
    
    *str_point1=0;
    
    write_exteeprom_ary(str_point,strlen(str_point)+1,AdminMobile2_ee_adr);
        				
	}
	//---------------------------------------------------------------	
	if((str_point=StrSearch(MemBuf,"fileuploadinterval=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		gprs_upload_time=strtolong(str_point+19);
		write_exteeprom_word(gprs_upload_time,gprs_upload_time_ee_adr);
				
	}
	//---------------------------------------------------------------	
	if((str_point=StrSearch(MemBuf,"gprsautoupload=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		gprs_auto_upload=strtolong(str_point+15);
		write_exteeprom_word(gprs_auto_upload,gprs_auto_upload_ee_adr);
				
	}
	//---------------------------------------------------------------	
	if((str_point=StrSearch(MemBuf,"datauploadtimes=",filesize))!=NULL){//uploadtimes=t0,t1,t2,t3,t4,
		
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
				
	}
	
	//---------------------------------------------------------------	
	if((str_point=StrSearch(MemBuf,"ftppathseparator=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		ftp_path_separator=*(str_point+17);
		write_exteeprom_byte(ftp_path_separator,ftp_path_separator_ee_adr);
				
	}
	//---------------------------------------------------------------	
	if((str_point=StrSearch(MemBuf,"stname=",filesize))!=NULL){
		str_point1=CharSearch(str_point,',',filesize);
		*str_point1=0;
		strcpy(station_name,str_point+7);
		write_exteeprom_ary(station_name,strlen(station_name)+1,station_name_ee_adr);
				
	}
		
}
//----------------------------------------------------
void time_setting(void){
	
  if(flag_ir.time_sett_main==1){
    
    key=4;
    start_key=4;
    end_key=5;
    flag_ir.time_sett_main=0;
    flag_ir.time_sett=1;
    flag_ir.time_set_enter=1;
    static const char str_f1[]="Date Setting";
    static const char str_f2[]="Clock Setting";
    strcpy(str[4],str_f1);
    strcpy(str[5],str_f2);
    LCDclr();
    LCDputs(str[4]);
    LCDGotoXY(15,0);
    LCDsendChar(up_arrow);
    LCDGotoXY(15,1);
    LCDsendChar(down_arrow);
    
    
  }
  else if(flag_ir.time_set_enter==1 && command_ir==ok){
    flag_ir.time_set_enter=0;
    switch(key)	{
    case 4:
      LCDclr();
      static const char str_f3[]="Input Date:";
      LCDputsf(str_f3);
      flag_ir.d=1;
		
			LCDcursorOn();
			LCDGotoXY(0,1);
			strcpy(input_number_str,"00000000");
			LCDputs(input_number_str);
			LCDGotoXY(0,1);
			number_limit=8;
			number_limit_index=0;
			flag_ir.in_number=1;
      break;	
      
    case 5:
      LCDclr();
      static const char str_f4[]="Input Time:";
      LCDputsf(str_f4);
      flag_ir.t=1;
			
			LCDcursorOn();
			LCDGotoXY(0,1);
			strcpy(input_number_str,"0000");
			LCDputs(input_number_str);
			LCDcursorOn();
			LCDGotoXY(0,1);
			number_limit=4;
			number_limit_index=0;
			flag_ir.in_number=1;
      break;	
    }
    
    
  }
  
  else if(flag_ir.d==1){
    input_number();
    
    if(command_ir==ok){
			flag.rtcset=1;
			buf[4]=0;
			memcpy(buf,input_number_str,4);
			clock.year=strtolong(buf);
      buf[2]=0;
			memcpy(buf,input_number_str+4,2);
			clock.month=strtolong(buf);
			
			memcpy(buf,input_number_str+6,2);
			clock.day=strtolong(buf);
      
      rtc_settime_shamsi(&clock);          
      LCDclr();
      LCDputsf(sel);
      delayms(1000);
      			
      flag_ir.time_sett_main=1;
      flag_ir.d=0;
      time_setting();
      
    }
    else if(command_ir==exit){
      flag_ir.time_sett_main=1;
      flag_ir.d=0;
      time_setting();
    }
    
  }
  else if(flag_ir.t==1){
		input_number();
    if(command_ir==ok){
			buf[2]=0;
			memcpy(buf,input_number_str,2);
			clock.hour=strtolong(buf);
      
			memcpy(buf,input_number_str+2,2);
			clock.min=strtolong(buf);
			
			rtc_settime_shamsi(&clock);
      
//      next_log_time=(clock.hour*60)+clock.min+log_time;
//      if(next_log_time>1440){next_log_time=next_log_time%1440; flag.log_disable=1;}
      
      LCDclr();
      LCDputsf(sel);
      flag_ir.time_sett_main=1;
      flag_ir.t=0;
      delayms(1000);
      
      time_setting();
      
    }
    else if(command_ir==exit){
      flag_ir.time_sett_main=1;
      flag_ir.t=0;
      time_setting();	
    }
    
  }
  
  else if(command_ir==exit) {
    flag_ir.setup_main=1;
    setup_prog();
  }
  
}
