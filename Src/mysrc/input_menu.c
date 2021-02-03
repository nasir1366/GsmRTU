#include "includes.h"
#include "my_delay.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defines.h"
#include "global_var.h"
#include "rtc.h"
#include "eeprom_ext.h"
#include "lcdlib.h"
#include "my_delay.h"
#include "input_menu.h"
#include "gsm.h"
#include "main_func.h"


/* Private variables ---------------------------------------------------------*/

//---FLAGS-----------
//#pragma anon_unions
struct {
   
  union{
    uint16_t blk1;
    struct{
			//general-------
			uint16_t upkey_press:1;     
    };	
  };
	 
  
}volatile flag_menu;


//-------------user code-------------------

