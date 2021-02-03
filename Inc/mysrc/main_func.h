#ifndef _MAIN_FUNC_INCLUDED_
#define _MAIN_FUNC_INCLUDED_

#include "includes.h"

void main_init(void);

void test_code(void);

void read_sensours(unsigned char*);

unsigned char log_data(void) ;  

void display(void);

void correct_rain_level(volatile float,volatile float*);

void event_process(void);

void read_water_level(void);

void amp_pwr(char stat);

void Error_Handle(void);

void wdt_refresh(void);

uint32_t strtolong(char* str);

void ReadSensorToStr(char *str);

uint8_t FindLog(char*,char*);

void log_event(char* log_str);

void enter_critical_section(void);

void exit_critical_section(void);

void SARAMIN(char *str);

#endif
