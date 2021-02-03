#ifndef DEF_H
#define DEF_H

#define marv


#define  TRUE  1
#define  FALSE  0
#define defult_password 11111

/********************IO Definition*******************/

#define tgl_wdt wdt_refresh()

#define rf_pwr_on //GPIOE->ODR |= GPIO_PIN_2
#define rf_pwr_off //GPIOE->ODR &= ~GPIO_PIN_2

#define relay1_on GPIOB->ODR |= GPIO_PIN_8
#define relay2_on GPIOB->ODR |= GPIO_PIN_9


#define relay1_off GPIOB->ODR &= ~GPIO_PIN_8
#define relay2_off GPIOB->ODR &= ~GPIO_PIN_9


//#define door_sense (GPIOE->IDR & GPIO_PIN_12) 

#define ir_input_pin //(GPIOG->IDR & GPIO_PIN_9)
#define rain_input_pin (GPIOA->IDR & GPIO_PIN_8)
//#define rtc_input_pin //(GPIOE->IDR & GPIO_PIN_15)
//#define Rotary_input_pin_interrupt (GPIOE->IDR & GPIO_PIN_11)
//#define Rotary_input_pin_io (GPIOE->IDR & GPIO_PIN_12)

#define lcd_pwr_on LcdPwrOn()
#define lcd_pwr_off LcdPwrOff()

//#define peripheral_5v_on GPIOE->ODR |= GPIO_PIN_3
//#define peripheral_5v_off GPIOE->ODR &= ~GPIO_PIN_3

#define voice_pwr_on 
#define voice_pwr_off 

#define mmc_pwr_on //GPIOG->ODR &= ~GPIO_PIN_3
#define mmc_pwr_off //GPIOG->ODR |= GPIO_PIN_3

#define  analog_pwr_on
#define  analog_pwr_off

//#define mmc_detect (GPIOD->IDR & GPIO_PIN_3)

#define i2c_pwr_on 
#define i2c_pwr_off 

//#define i2c2_pwr_on //GPIOB->ODR &= ~GPIO_PIN_1
//#define i2c2_pwr_off //GPIOB->ODR |= GPIO_PIN_1


#define simpwr_on relay1_on
#define simpwr_off relay1_off

//#define remute_ir_clear (GPIOD->IDR & GPIO_PIN_14)

//#define lan_pwr_on GPIOG->ODR &= ~GPIO_PIN_2 
//#define lan_pwr_off GPIOG->ODR |= GPIO_PIN_2

//#define DHT11_pin GPIO_PIN_7
//#define DHT11_port GPIOC

//#define camera_on relay2_on
//#define camera_off relay2_off

//#define light_on GPIOE->ODR |= GPIO_PIN_2
//#define light_off GPIOE->ODR &= ~GPIO_PIN_2

#define snow_motor_on relay2_on
#define snow_motor_off relay2_off

//#define heater_on relay2_on
//#define heater_off relay2_off

//#define sht_on 
//#define sht_off 


//#define rs485_tx_mode GPIOE->ODR |= GPIO_PIN_4;GPIOD->ODR |= GPIO_PIN_7
//#define rs485_rx_mode GPIOD->ODR &= ~GPIO_PIN_7;GPIOE->ODR &= ~GPIO_PIN_4

//------------------keypad---------------------------------
#define up_key (GPIOB->IDR & GPIO_PIN_11)
#define down_key (GPIOB->IDR & GPIO_PIN_13)
#define right_key (GPIOB->IDR & GPIO_PIN_12)
#define left_key (GPIOB->IDR & GPIO_PIN_15)
#define ok_key (GPIOB->IDR & GPIO_PIN_14)
#define menu_key (GPIOA->IDR & GPIO_PIN_15)
#define esc_key (GPIOA->IDR & GPIO_PIN_0)

/**********************Defult Voice number in mmc ***************************/
#define rob_alarm  1

#define rob_alarm_cou 10

/******************address for variables that must be saved********/

#define password_ee_adr 100 //word
#define station_ee_adr 102//word


#define pwr_existence_ee_adr 105
#define st_type_ee_adr 108

#define log_time_ee_adr 109//2
#define fc_time_ee_adr 111
#define send_time_ee_adr 112
#define temp_difference_ee_adr 113
#define door_state_ee_adr 114
#define ext_device_ee_adr 115 //dword
#define max_water_level_ee_adr 119 //word
#define min_water_level_ee_adr 121 //word
#define water_level_ee_adr 123//word
#define rain_type_ee_adr 125
#define T_low_ee_adr 126
#define T_hi_ee_adr 127
#define Imax_ee_adr 128 //float
#define Imin_ee_adr 132 //float
#define sensor_level_ee_adr 136//word

#define IrIsLearned_ee_adr 150 
#define button_ir_ee_adr 151 //100 byte 149-249
#define base_ee_adr 252 //30 byte 250-280
#define temp_type_ee_adr 283
#define sample_count_ee_adr 284
#define AirAlpha_ee_adr 285
#define AirBeta_ee_adr 289
#define WindAlpha_ee_adr 293
#define WindBeta_ee_adr 297
#define WindCalibOk 301
#define AirCalibOk 302
#define wind_type_ee_adr 303
#define myip_ee_adr 304//4 byte
#define water_type_ee_adr 308
#define ADC1CalibOk 309
#define ADC2CalibOk 310
#define ADC3CalibOk 311
#define ADC1Scale_ee_adr 312 //4byte
#define ADC2Scale_ee_adr 316 //4byte
#define ADC3Scale_ee_adr 320 // 4byte

#define server_url_ee_adr 324 //30 byte
#define server_username_ee_adr 354 //30
#define server_password_ee_adr 384 //20
#define server_changed1_ee_adr 404 //1
#define server_changed2_ee_adr 405 //1
#define apn_name_ee_adr 406//30
#define server_filename_ee_adr 436 //50
#define server_path_ee_adr 486//50
#define server_phone_ee_adr 540//30 byte 

#define ADC4CalibOk 571
#define ADC5CalibOk 572
#define currentCalibOk 573
#define ADC4Scale_ee_adr 580 //4byte
#define ADC5Scale_ee_adr 585 //4byte
#define currentScale_ee_adr 590 // 4byte

#define RainRstTime_ee_adr 595  //2
#define rain_level_ee_adr 597  //4
#define RainEventTime_ee_adr 601  //2

#define SmsSendTime_ee_adr 603//2
#define ChargeReqStr_ee_adr 605//30

#define DeltaRainLevelEEadr 635//4
#define DeltaWlvlEEadr 640//3

#define LogParametersEEadr 643//2

#define RotaryResolution_ee_adr 645//2
#define RotaryDiameter_ee_adr 647//2

#define AdminMobile1_ee_adr 650//30
#define AdminMobile2_ee_adr 680//30
#define heater_control_ee_adr 715 //1

#define camera_log_time_ee_adr 716 //2

#define gprs_upload_time_ee_adr 718//2
#define gprs_auto_upload_ee_adr 720//1

#define upload_time_table0_ee_adr 722//2
#define upload_time_table1_ee_adr 724//2
#define upload_time_table2_ee_adr 726//2
#define upload_time_table3_ee_adr 728//2
#define upload_time_table4_ee_adr 730//2

#define ftp_path_separator_ee_adr 732//1

#define station_name_ee_adr 733 //30

//---------------------save last address in log section in ext eeprom------------
//#define start_log_adr 500
//#define log_adr1 450//long
//#define log_adr2 454//long
//#define log_adr3 458//long
//#define log_adr4 462//long

//------------------external eeprom devided to 4 block for faster access time-----------
////for 1024 eeprom
//#define block1 32993
//#define block2 65686
//#define block3 98379
//#define block4 131072

//for 32k eeprom
//#define block1 1249
//#define block2 2198
//#define block3 3147
//#define block4 4096

//-----------------statoin types ------------
#define rep 1
#define center_adr 1
//----------------external devices--------------
#define water_sensor 1
#define rain_sensor 2
#define temp_sensor 4
#define snow_motor 8
#define sd_card 16
#define gsm 32
#define bmp180_sensor 64
#define wind_sensor 128
#define TcpipInterface 256
#define camera 512
#define RadioModem 1024
#define wifi 2048
//--------------Log Parameters------------------
#define LogBatvolt 1
#define LogSolarvolt 2
#define LogRainlevel 4
#define LogWaterlevel 8
#define LogTemperature 16
#define LogWind 32
#define LogBMP180 64
//--------------Rain sensor com type----------
#define rain_rs485 2
#define rain_direct 1

#define rain_start 200
//------------Temperature Sensor Type--------------
#define PT100 1
#define NTC 2
#define SHT20 3

/*****************************time out ************/
#define i2c_timeout 100
//---------------Wind sensor type----------------
#define wind_direct 1
#define wind_omc160 2

//--------------------water sensor type--------------------
#define rotary_encoder 10
#define thalimedes 12
#define microsonic 14
#define orpheus 16 //Ott Ground Water

//------------------gprs setting ----------------------
#define gprs_upload_tabletime 1
#define gprs_upload_logfile 2
#define gprs_enable 4

#endif
