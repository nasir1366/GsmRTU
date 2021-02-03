#ifndef _EEPROM_EXT_INCLUDED_
#define _EEPROM_EXT_INCLUDED_

#include "includes.h"

#ifndef address_bus
#define address_bus 0xA6
#endif

           

//************************************************************************************************************************


char write_exteeprom_byte(unsigned char,unsigned long );
char write_exteeprom_word(uint16_t ,unsigned long );
char write_exteeprom_dword(unsigned long ,unsigned long );
char write_exteeprom_float(float ,unsigned long );
char write_exteeprom_ary(char* ,unsigned char ,unsigned long );
//---------------------------------------------------------------
unsigned char read_exteeprom_byte(unsigned long );
uint16_t read_exteeprom_word(unsigned long );
unsigned long read_exteeprom_dword(unsigned long );
float read_exteeprom_float(unsigned long );
void read_exteeprom_ary(char*,unsigned char ,unsigned long );
//-----------------------------------------------------------------
void clear_eeprom(void);

#endif
