#ifndef RTC_DEFINED
#define RTC_DEFINED


#include "includes.h"

#define TRUE 1
#define FALSE 0

#define rtc_adr 0x64

typedef struct {
	uint16_t year;	/* 2000..2099 */
	uint8_t	month;	/* 1..12 */
	uint8_t	day;	/* 1.. 31 */
	uint8_t	hour;	/* 0..23 */
	uint8_t	min;	/* 0..59 */
	uint8_t	sec;	/* 0..59 */
} Rtc;


char rtc_init (void);						/* Initialize RTC */
char rtc_gettime (volatile Rtc*);					/* Get time */
char rtc_gettime_shamsi(volatile Rtc *);
char rtc_settime (volatile Rtc*);				/* Set time */
char rtc_settime_shamsi(volatile Rtc *);
void MiladiToShamsi(int16_t iMiladiDay,int16_t iMiladiMonth,int16_t iMiladiYear);
void ShamsiToMiladi(int16_t ShamsiDay,int16_t ShamsiMonth,int16_t ShamsiYear);
unsigned char MiladiIsLeap(int16_t miladiYear);


#endif
