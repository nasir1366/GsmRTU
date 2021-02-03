#ifndef SDI12_H
#define SDI12_H

#include "includes.h"
#include "defines.h"
#include "global_var.h"


#define sdi_pin_read (GPIOE->IDR & GPIO_PIN_8)
#define sdi_pin_set (GPIOE->ODR |= GPIO_PIN_8)
#define sdi_pin_reset (GPIOE->ODR &= ~GPIO_PIN_8)

#define sdi_port GPIOE 
#define sdi_pin 8




void sdi_dir_out(void);
void sdi_dir_in(void);
uint8_t ReadPinTimeout(GPIO_TypeDef* port,unsigned char pin,unsigned char stat,unsigned int timeout);
uint8_t parity_check(char ch);
uint8_t thali_readresp(void);
void thali_sendchar(char ch);
void thali_sendcmd(char *str);

#endif
