
#include "includes.h"

#include "defines.h"
#include "global_var.h"
#include "my_delay.h"
#include "main_func.h"
#include "eeprom_ext.h"
#include "ir.h"

void HAL_SYSTICK_Callback(){
	static uint16_t pre;
	
	pre++;
  if(pre>1000){
			pre=0;
			if(fc_clr_time>1){rec_cou=0;fc_clr_time=0;}
			
			usart_rain_cou=0;
			p_rec_cou=0;
			flag.fc_net_busy=0;
			
//			if(flag.open_door==0){
				if(flag.ir_flag==1) time_out++;
				if(time_out>return_time){
					flag.main_menu=1;
					flag.menu_setting=0;
					flag.open_door=1;
					time_out=0;
					flag.ir_flag=0;
					display_count=4;
					return_time=60;
					clear_ir_flags();
				}
//			}
//			else{
//				if(valid_door_flag!=2){
//					if(flag.input_init_pass==1) time_out_pass++;
//					if(time_out_pass>5){
//						flag.input_init_pass=0;
//						flag.ir_flag=0;
//					}
//				}		
//			}	
			
			if(st_type!=rep && flag.comm==0){
				
				if(rf_pwr_count<fc_on_time){ rf_pwr_on;}
				
				else if (rf_pwr_count>=fc_on_time && rf_pwr_count<=(fc_on_time+fc_off_time)){ rf_pwr_off;}
				
				rf_pwr_count++;
				
				if( rf_pwr_count==(fc_on_time+fc_off_time) )  rf_pwr_count=0;
				
			}
			
			send_count++;
			count++;
			rain_cou++;
			DisplayTime++;
			if(rain_cou>RainEventTime && flag.start_rain_event==1){
				flag.start_rain_event=0;
				flag.stop_rain=1;
				minute_rain_cou=0;
				minute_rain=0;
				
				for(rain_register_index=0;rain_register_index<15;rain_register_index++)
					rain_register[rain_register_index]=0;
				
				rain_register_index=0;
				
			}
			if(flag.start_rain_event==1){
				minute_rain_cou++;
				if(minute_rain_cou>59){
					minute_rain_cou=0;
															
					rain_register[rain_register_index]=minute_rain;
					rain_register_index++;					
					if(rain_register_index>14)rain_register_index=0;
					flag.min_rain_check=1;
					
					minute_rain=0;
				}
			}
			
			//if(water_cou>900) flag.water_event=0;
			alarm_count++;
			wakeup_cou++;
			fc_clr_time++;	
			display_count++;
			if(display_count>16) display_count=0;
			flag.disp=0;
			loop_timeout++;
			iic_pwr_count++;
			iic_count++;
			temp_time++;
			SimCount++;
			TakePictureCount++;
			iic_busy_timeout++;
			test_mode_timeout++;
			if(test_mode_timeout>1800 && flag.test_mode==1){
				flag.test_mode=0;
				rain_level=0;
			}
			
			user_sms_timeout++;
			if(user_sms_timeout>1)user_sms_count=0;
			SetLevelBar_timeout++;
			if(SetLevelBar_timeout>1)SetLevelBar_count=0;
		}
		
		
}

/**************************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	
   if(GPIO_Pin==GPIO_PIN_8){
    if(rain_input_pin==0){
      delayms(50);
      
      rain_time += ((float)TIM2->CNT/31250);
      
      if(rain_time>0.5){
        
        correct_rain_level(rain_time,&rain_level);
				
				correct_rain_level(rain_time,&minute_rain);
				
        write_exteeprom_float(rain_level,rain_level_ee_adr);
								
				
				flag.detect_rain=1;
				
				rain_cou=0;
        
      }
      rain_time=0;
      TIM2->CNT=0;
            
    }
		
	}
	
	
	
	}

/*****************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	static uint16_t key_check;
	
  if(htim==&htim2){
    rain_time+=2;
  }
	else if(htim==&htim4){
				
		key_check++;
		
		if(key_check>15){
			key_check=0;
			scan_key();
		}
			
	}

}




