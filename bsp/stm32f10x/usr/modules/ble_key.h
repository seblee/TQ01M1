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
    STATE_LED_FLASH_2_T, //闪烁两下
};
enum
{
    I2C_IDEL,
    I2C_KEY,
    I2C_LED,
    I2C_REG_UP,
    I2C_REG_DOWN,
};

extern _TKS_FLAGA_type keyState[3];
extern volatile _TKS_FLAGA_type keyTrg[3];

// #define I2CCold1 keyState[0].bits.b0
// #define I2CHeat keyState[0].bits.b1
// #define I2CKEY3 keyState[0].bits.b2
// #define I2CKEY4 keyState[0].bits.b3

extern _USR_FLAGA_type ledState[5];
#define normalWaterLed ledState[0].s4bits.s0
#define hotWaterLed ledState[0].s4bits.s1
#define refrigerateLed ledState[1].s4bits.s0
#define childLockLed ledState[1].s4bits.s1
#define warningLed ledState[2].s4bits.s0
#define levelH ledState[2].s4bits.s1
#define levelM ledState[3].s4bits.s0
#define levelL ledState[3].s4bits.s1

#endif
