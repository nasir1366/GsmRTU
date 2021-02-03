#ifndef gsm_INCLUDED
#define gsm_INCLUDED

#include "includes.h"

#define CRchar 13
#define LF 10
#define CZ 26
#define ESC 27

char GsmInit(void);


void GsmSend(char*);

void GsmSendByte(char *,unsigned int);

char SendSMS(char *, char *);

char ReadSMS(char*,char*,unsigned char);

char SaveSMS(char*);

void BinToStr(char*,char,char*);

char StrToBin(char*,char*);

void SMS_ReplyCMD(char*,char*);

void flush_buf (char *,uint16_t);

char* StrSearch(char*,char*,uint16_t);

char* StrSearch_P(char*,char const*,uint16_t);

char* CharSearch(char *,char ,uint16_t );

 char* CharSearch_str(char *buffer,char chr);

void strcopy(char *,char *);

void SimChargeCat(char*);

void sendfile(char*);

void lowercase(char *str,unsigned int size);

uint8_t upload_instant(char* data_str,char *PhoneNumber);

uint8_t upload_file(char* filename,char *PhoneNumber);
  
#endif
