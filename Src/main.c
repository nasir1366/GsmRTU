/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "fatfs.h"
#include "usb_device.h"

/* USER CODE BEGIN Includes */
#include "includes.h"
#include "defines.h"
#include "lcdlib.h"
#include "serialflash.h"
#include "rtc.h"
#include "main_func.h"
#include "gsm.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

IWDG_HandleTypeDef hiwdg;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

const uint32_t station_address=500;
char station_name[30]="MARV ELECTRONIC";
const char firmware_version[]="V4.01";

#ifdef marv
const char title_str[]="     MARV RTU";
#endif

#ifdef saramin
const char title_str[]="    SARAMIN RTU";
#endif

ADC_ChannelConfTypeDef adConfig;

volatile Rtc clock,cur_clock;

// server listen port for www

#define MYWWWPORT 80

// global packet buffer

#define BUFFER_SIZE 550


//static uint8_t tcp_buf[BUFFER_SIZE+1];

// Counter & Index Var ------------------------
volatile uint8_t play_count, alarm_count, resend,len,display_count,iic_count,rec_cou,rf_pwr_count,
time_out,time_out_pass,send_time,p_rec_cou,p_len,loop_timeout,ir_cou,iic_pwr_count,
fc_clr_time,next_msg,usart_rain_cou,ReadWaterTime,log_period,minute_changed,iic_busy_timeout;


volatile uint16_t send_count,count,rain_cou,wakeup_cou,i,j,wind_timeout,test_mode_timeout,temp_time;  

//Normal Var------------------------------------------------------------  
volatile uint8_t st_type,pwr_existence,
fc_on_time,fc_off_time,cur_minute,save_door_state,
valid_door_flag,snow_mtr_time=11,cur_resend,rain_sensor_type,fc_delay,temp_type,
sound_number,WindSensorType;


volatile uint16_t password,input_pass,alarm,log_time,next_log_time,sample_count,start_time,stop_time,
return_time=60,WaterSensorTime,DisplayTime,wind_dir,cur_wind_dir,wind_speed,cur_wind_speed,LogParameters,
time24;


volatile uint32_t write_adr,read_adr,data_ir,button[20],ext_device,AirPressure,cur_air_pressure;

volatile float wind_time,WindAlpha,WindBeta,AirAlpha,AirBeta,utp,ltp;
//-------------Rain Sensor Var--------------------------
volatile float rain_level,cur_rain_level,last_rain_level,delta_rain_level,rain_time,minute_rain;
volatile float rain_register[20],rain_sum;
volatile uint8_t rain_register_index;
volatile uint16_t RainRstTime,RainRstFlag;
volatile uint16_t RainEventTime=900,minute_rain_cou;

volatile uint8_t heater_control;
signed char heater_temp;
//----------------Analog input var--------------------
float ADC1Scale,ADC2Scale,ADC3Scale,ADC4Scale,ADC5Scale,currentScale;
//---------water sensor var--------------------------
unsigned char WaterSensorType;
volatile int16_t  water_level,last_water_level,delta_water_level,cur_water_level_display,max_water_level,
min_water_level,SensorLevel,RotaryResolution,RotaryDiameter,RotaryScale,cur_water_level;
float Imin,Imax;
uint16_t volatile SetLevelBar_count,SetLevelBar_timeout;

//-----------Camera Module Variables------------- 
volatile unsigned int PictureSize,PictureSizeCounter,TakePictureCount,CamRecLen,
camera_log_time;
char CameraBuffer[550],CameraBuffer1[550],MemBuf[1310]/*1310*/,*CameraBufPointer=CameraBuffer;
volatile unsigned char CamBufOvf,PictureRecProcess,rs485_select;

unsigned char gprs_first_connect,gprs_auto_upload;

char server_url[50],server_username[30],server_password[20],server_filename[50],server_path[100],apn_name[30];
char temp_filename[40];

char camera_reset[4]={0x56,0,0x26,0};
char camera_TakePicture[5]={0x56,0,0x36,0x1,0};
char camera_ReadFileSize[5]={0x56,0,0x34,0x1,0};
char camera_ReadFileContent[16]={0x56,0,0x32,0xc,0,0xa,0,0,0,0,0,0,0,0,0,0xa};
char camera_ChangePicSizeLarge[5]={0x56,0,0x54,1,0};
char camera_ChangePicSizeSmall[5]={0x56,0,0x54,1,0x11};
char camera_changeboud[7]={0x56,0,0x24,3,1,0xAE,0xC8};

//--------bmp variabls-----------
volatile int16_t AC1,AC2,AC3,B1,B2,MB,MC,MD;
volatile uint16_t AC4,AC5,AC6;
volatile int32_t UP=0,UT=0,B3,B4=0,B5=0,B6=0,B7=0,X1=0,X2=0,X3=0,T=0;
volatile float P;

/////ADXL345 --------------------------------------------
uint16_t Accel_Xout_Val=0, Accel_Yout_Val=0, Accel_Zout_Val=0;
int16_t Accel_Xout_Normalized_Val=0, Accel_Yout_Normalized_Val=0, Accel_Zout_Normalized_Val=0;
uint8_t Offset_X_Val=0,Offset_Y_Val=0,Offset_Z_Val=0;
float Accel_XAngle=0, Accel_YAngle=0, Accel_ZAngle=0;

//***---------------temperature & humidity sensor ------------------
signed char temp_in,cur_temp_in,temp_out,cur_temp_out,temp_difference,T_low,T_hi;
uint8_t sht_rec_buffer[15],sht_send_buffer[5],temp_rec_len,humidity;
//------------thalimedes --------------
unsigned char thali_buf[20];
//buffer Definition ***********************

char p_rec_buf[15],p_send_bufer[15],fc301d_send_buffer[255],fc301d_rec_buffer[255],
log_buffer[100],base_address[30],log_header[20];

uint8_t usart2_rx[2],usart3_rx[2];
//uint16_t log_avg[40];
char buf[255];
//-------gsm module---------------
char SimSendBuffer[255],SimRecBuffer[255],ServerPhone[30],RemotePhone[30]
,SMS_text[255],ChargeReqStr[30],gprs_buffer[250];
char AdminMobile[30];
volatile uint8_t gsm_off_time,SmsSendFlag,CUSD_cnt;
volatile int8_t gsm_signal,SMS_number,ftp_path_separator;
volatile uint16_t SimCount,SmsSendTime,SimRecLen,user_sms_count,user_sms_timeout,gprs_upload_time;
volatile uint16_t upload_time_table[5];
//--------keypad string-----------
char input_number_str[20];
uint8_t number_limit,number_limit_index;
//pointer-------------------------
uint8_t *rec_data,*data,**endptr;
char *sms_pointer,*sms_pointer1,*str_point;
//FatFs variable definition---------------------

FATFS SDFatFs;  /* File system object for SD card logical drive */
uint8_t SDPath[4]; /* SD card logical drive path */

FRESULT res;                                          /* FatFs function common result code */

FATFS Fatfs[_VOLUMES];		/* File system object for each logical drive */
FIL File[2];				/* File object */
DIR dir[5];
UINT FatBytesWritten;
UINT FatBytesRead;

//Structures & Bit Variables -----------------------------------------------------------------
#pragma anon_unions
union {
  
  uint8_t msg;
  struct {
    
    uint8_t num:7;
    uint8_t device:1;
  }; 
}volatile play_buffer[20];



//Flag Defines-------------------
//#pragma anon_unions
struct{
  
  struct {
    uint16_t open_door:1;
    uint16_t main_menu:1;
    uint16_t ir_flag:1;
    uint16_t comm:1;
    uint16_t ack:1;
    uint16_t ply_alarm:1;
    uint16_t rob:1;
    uint16_t wakeup:1;
    uint16_t door:1;
    uint16_t temp_control:1;
    uint16_t fc_net_busy:1;
    uint16_t LcdOn:1;
    uint16_t bmp180_timeout:1;
    uint16_t pcf_timeout:1;
    uint16_t disp:1;
  };
  
  struct{
    uint16_t read_state:1;
    uint16_t config_pwr:1;
    uint16_t pwr_config_read:1;
    uint16_t read_log:1;
    uint16_t config_rtc:1;
    uint16_t rtc_read:1;
    uint16_t config_log_time:1;
    uint16_t log_time_read:1;
    uint16_t config_fc_cycle:1;
    uint16_t fc_cycle_read:1;
    uint16_t config_send_time:1;
    uint16_t send_time_read:1;
    uint16_t rtcset:1;
    uint16_t dmatranfercplt:1;
    
    
  };
  struct{		
    uint16_t fc301d_reply_cmd:1;
    uint16_t display_level:1;
    uint16_t aburt:1;
    uint16_t spi_rec:1;
    uint16_t input_init_pass:1;
    uint16_t r:1;
    uint16_t detect_rain:1;
    uint16_t start_rain_event:1;
    uint16_t rec_rain_usart:1;
    uint16_t refresh:1;
    uint16_t rtc_update:1;
    uint16_t log_disable:1;
    uint16_t ir_learn:1;
    uint16_t WaterSensorOn:1; 
		uint16_t test_mode:1;
		uint16_t i2c_busy:1;
    
  };
	struct{		
    uint16_t UserCheck:1;
    uint16_t SetWaterLevelBar:1;
		uint16_t min_changed:1;
		uint16_t log_start:1;
		uint16_t stop_rain:1;
		uint16_t min_rain_check:1;
		uint16_t log_picture:1;
		uint16_t levelevent:1;
		uint16_t menu_setting:1;
		uint16_t alarm_disable:1;
		uint16_t upload_log:1;
		uint16_t upload_in_schedule:1;
		
  };
  
  
}volatile flag;






//FatFs variable definition---------------------

FATFS SDFatFs;  /* File system object for SD card logical drive */
uint8_t SDPath[4]; /* SD card logical drive path */

FRESULT res;                                          /* FatFs function common result code */

FATFS Fatfs[_VOLUMES];		/* File system object for each logical drive */
FIL File[2];				/* File object */
DIR dir[5];
UINT FatBytesWritten;
UINT FatBytesRead;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_IWDG_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM4_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */


  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
//  MX_RTC_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  MX_FATFS_Init();
  MX_IWDG_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();

  /* USER CODE BEGIN 2 */
	
	main_init();
//	test_code();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		tgl_wdt;
		
		rtc_gettime(&clock);

    //--------------------------clock update after 6 month---------------		
    if(iic_busy_timeout>60){
			flag.i2c_busy=0;
		}
		
    if(clock.hour==0 && clock.min==0){
      
      flag.log_disable=0; //enable clock compare for data logging
          
      if(clock.day==1 && clock.month==1){
        clock.hour++;
        rtc_settime_shamsi(&clock);
      }
      
      else if(clock.day==1 && clock.month==7 && flag.rtc_update==0){
        flag.rtc_update=1;
        clock.hour=23;
        clock.day=31;
        clock.month=6;
        rtc_settime_shamsi(&clock);
        
      }
      
    }
    
    if(clock.hour==1 && flag.rtc_update==1) flag.rtc_update=0;
    
    //------------------------------------ in time processing--------------------------------
    	
		
		if(flag.log_start==1){
			flag.log_start=0;
      		
			log_data();
			flag.log_disable=1;
			
    }
		
		 if(ext_device & gsm){
      
			if((clock.hour)*60+clock.min != SmsSendTime)SmsSendFlag=0;
    
			if(((clock.hour)*60+clock.min == SmsSendTime)&&(SmsSendFlag==0)){
				SmsSendFlag=1;
				ReadSensorToStr((char*)SMS_text);
				SimChargeCat(SMS_text);
        SendSMS((char*)ServerPhone,(char*)SMS_text);
				
			}
					            
      if(gsm_off_time!=clock.hour){
        gsm_off_time=clock.hour;
        simpwr_off;
        delayms(5000);
        GsmInit();
        flush_buf(SMS_text,250);
        
      }
      
       if((sms_pointer=StrSearch_P(SimRecBuffer,"+CMTI:",250))!=NULL){  //+CMTI:
         delayms(2000);
				 sms_pointer=CharSearch_str(sms_pointer,',');
				 if(sms_pointer != NULL){
				 sms_pointer++;
				 sms_pointer1=CharSearch_str(sms_pointer,CRchar);
				 if(sms_pointer1 != NULL){
						 *sms_pointer1=0;
						 SMS_number=strtolong(sms_pointer);
		
							if(ReadSMS(RemotePhone,SMS_text,SMS_number)==1){
		
								if(flag.LcdOn==1 && flag.ir_flag==0){
									LCDclr();
									sprintf(buf,"SMS Receive:\n    %s",RemotePhone);
									LCDputs(buf);
									
								}
								SMS_ReplyCMD(RemotePhone,SMS_text);
								delayms(200);
						}
					}
				}
				for(i=1;i<5;i++){
					flush_buf(SimRecBuffer,250);
					sprintf(SimSendBuffer,"AT+CMGD=%d%c%c",i,CRchar,LF);
					GsmSend(SimSendBuffer);
					SimCount=0;
					sms_pointer=NULL;
					while(SimCount<3 && sms_pointer==NULL){
						sms_pointer=StrSearch(SimRecBuffer,"OK",250);
					}
					delayms(200);
				}
        flush_buf(SimRecBuffer,250);
        flush_buf(SMS_text,250);
        SimRecLen=0;
                
      }
      
    }
		
		if(flag.LcdOn==1){
      display();
      if(DisplayTime>600){
        lcd_pwr_off;
        flag.LcdOn=0;
      }
    }
				
		
		
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC
                              |RCC_PERIPHCLK_USB;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Common config 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* IWDG init function */
static void MX_IWDG_Init(void)
{

  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* RTC init function */
static void MX_RTC_Init(void)
{

  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef DateToUpdate;

    /**Initialize RTC Only 
    */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initialize RTC and set the Time and Date 
    */
  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2){
  sTime.Hours = 0x1;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 0x1;
  DateToUpdate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,0x32F2);
  }

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 768;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 62500;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM4 init function */
static void MX_TIM4_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 2400;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 100;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_DB4_Pin|LCD_DB5_Pin|LCD_DB6_Pin|LCD_DB7_Pin 
                          |LCD_RW_Pin|LCD_EN_Pin|LCD_RS_Pin|LCD_PWR_Pin 
                          |Relay1_Pin|Relay2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ESC_KEY_Pin MENU_KEY_Pin */
  GPIO_InitStruct.Pin = ESC_KEY_Pin|MENU_KEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : FLASH_CS_Pin */
  GPIO_InitStruct.Pin = FLASH_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(FLASH_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_DB4_Pin LCD_DB5_Pin LCD_DB6_Pin LCD_DB7_Pin 
                           LCD_RW_Pin LCD_EN_Pin LCD_RS_Pin LCD_PWR_Pin 
                           Relay1_Pin Relay2_Pin */
  GPIO_InitStruct.Pin = LCD_DB4_Pin|LCD_DB5_Pin|LCD_DB6_Pin|LCD_DB7_Pin 
                          |LCD_RW_Pin|LCD_EN_Pin|LCD_RS_Pin|LCD_PWR_Pin 
                          |Relay1_Pin|Relay2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : UP_KEY_Pin RIGHT_KEY_Pin DOWN_KEY_Pin OK_KEY_Pin 
                           LEFT_KEY_Pin SDI12_Pin */
  GPIO_InitStruct.Pin = UP_KEY_Pin|RIGHT_KEY_Pin|DOWN_KEY_Pin|OK_KEY_Pin 
                          |LEFT_KEY_Pin|SDI12_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : RainPulse_Pin */
  GPIO_InitStruct.Pin = RainPulse_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RainPulse_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
