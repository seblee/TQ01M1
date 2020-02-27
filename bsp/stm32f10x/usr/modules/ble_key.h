#ifndef __BLE_KEY_H
#define __BLE_KEY_H
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
} _STATE_2bits;

typedef struct
{
    unsigned char s0 : 4;
    unsigned char s1 : 4;
} _STATE_4bits;

typedef union {
    _STATE_2bits s2bits;
    _STATE_4bits s4bits;
    unsigned char byte;
} _USR_FLAGA_type;

enum
{
    STATE_LED_OFF,
    STATE_LED_ON,
    STATE_LED_FLASH_2HZ,
    STATE_LED_FLASH_1HZ,
    STATE_LED_FLASH_0_5HZ,
};
enum
{
    CMD_IDEL,
    CMD_KEY,
    CMD_LED,
    CMD_REG,
};

#endif
