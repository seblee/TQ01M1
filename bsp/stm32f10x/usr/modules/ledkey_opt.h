#ifndef __LEDKEY_OPT_H
#define __LEDKEY_OPT_H
/**
 ****************************************************************************
 * @Warning Without permission from the author,Not for commercial use
 * @File    
 * @Author  xiaowine@cee0.com
 * @date    
 * @version V1.0
 *************************************************
 * @brief   -->>
 ****************************************************************************
 * @attention 
 * Powered By Xiaowine
 * <h2><center>&copy;  Copyright(C) cee0.com 2015-2019</center></h2>
 * All rights reserved
 * 
**/

#include <rtthread.h>

typedef struct
{
    unsigned char b0 : 1;
    unsigned char b1 : 1;
    unsigned char b2 : 1;
    unsigned char b3 : 1;
    unsigned char b4 : 1;
    unsigned char b5 : 1;
    unsigned char b6 : 1;
    unsigned char b7 : 1;
} _FLAG_bits;

typedef union {
    _FLAG_bits bits;
    unsigned char byte;
} _TKS_FLAGA_type;

typedef struct
{
    unsigned char s0 : 2;
    unsigned char s1 : 2;
    unsigned char s2 : 2;
    unsigned char s3 : 2;
} _STATE_bits;

typedef union {
    _STATE_bits sbits;
    unsigned char byte;
} _USR_FLAGA_type;

enum
{
    LED_OFF = 0x00,
    LED_ON = 0x01,
    LED_FLASH = 0x02,
};

#endif
