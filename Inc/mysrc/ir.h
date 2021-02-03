#ifndef IR_H
#define IR_H

#include "includes.h"


#define up 121
#define down 122
#define left 123
#define right 124
#define ok 11
#define SETUP 83
#define MENU  27
#define ret 14
#define play_key 50
#define stop_key 56
#define on 21
#define exit 22
//---------------------------------------------------------------------------------------------------------------------------------------------------
#define debounce_key 1
void ir_rec(void);
unsigned char processing_ir_keypad(void);
char IR_Learning(void);
void clear_ir_flags(void);
void scan_key(void);
void loadSettingSD(void);
#endif
