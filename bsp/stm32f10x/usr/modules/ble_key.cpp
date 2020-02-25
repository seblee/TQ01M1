/**
 ****************************************************************************
 * @Warning Without permission from the author,Not for commercial use
 * @File    ble_key.c
 * @Author  xiaowine@cee0.com
 * @date    
 * @version V1.0
 *************************************************
 * @brief   标注系统信息
 ****************************************************************************
 * @attention 
 * Powered By Xiaowine
 * <h2><center>&copy;  Copyright(C) cee0.com 2015-2019</center></h2>
 * All rights reserved
 * 
**/

#include <rtthread.h>
#include <rtdevice.h>
#include "ble_key.h"
#include "i2c_utils.h"

#define I2C_ADDRESS 0x7e

#ifdef I2C_TOOLS_USE_SW_I2C
#define SDA_PORT_NUM 61
#define SCL_PORT_NUM 62
#else
#define I2C_DEVICE_NAME "i2c1"
#endif

unsigned char recOK = 0;
static unsigned char tx_buffer[10] = {1, 2, 3, 4, 5, 0, 7, 8, 9, 10};
static unsigned char rx_buffer[10] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
/**********************key led beep*********************************************************/
_TKS_FLAGA_type keyState[2];
volatile _TKS_FLAGA_type keyTrg[2];

#define KEY1 keyTrg[0].bits.b2
#define KEY2 keyTrg[0].bits.b3
#define KEY3 keyTrg[0].bits.b1
#define KEY4 keyTrg[0].bits.b0

#define KEY1Restain keyTrg[1].bits.b2
#define KEY2Restain keyTrg[1].bits.b3
#define KEY3Restain keyTrg[1].bits.b1
#define KEY4Restain keyTrg[1].bits.b0

_USR_FLAGA_type ledState[5];
#define led1State ledState[0].s4bits.s0
#define led2State ledState[0].s4bits.s1
#define led3State ledState[1].s4bits.s0
#define led4State ledState[1].s4bits.s1
#define led5State ledState[2].s4bits.s0
#define led6State ledState[2].s4bits.s1
#define led7State ledState[3].s4bits.s0
#define led8State ledState[3].s4bits.s1
#define led9State ledState[4].s4bits.s0

unsigned char beepCount = 0;

void refreshTxData(void);

static void i2c_thread_entry(void *para)
{
    rt_kprintf("i2c_thread_entry start \n");
    while (1)
    {
        /* 调用I2C设备接口传输数据 */
        refreshTxData();
        if (i2c_write(I2C_ADDRESS, tx_buffer, 10) == 1)
        {
           // rt_kprintf("i2c_write OK \n");
        }
        else
        {
            rt_thread_delay(rt_tick_from_millisecond(1000));
            rt_kprintf("i2c_write err \n");
            continue;
        }
        rt_thread_delay(rt_tick_from_millisecond(50));
        if (i2c_read(I2C_ADDRESS, 0, rx_buffer, 10) == 10)
        {
           rt_kprintf("i2c_read OK %02x,%02x\n",rx_buffer[0],rx_buffer[1]);
        }
        else
        {
            rt_thread_delay(rt_tick_from_millisecond(1000));
            rt_kprintf("i2c_read err \n");
            continue;
        }
        rt_thread_delay(rt_tick_from_millisecond(50));
    }
}

int i2cBleThreadInit(void)
{
#ifdef I2C_TOOLS_USE_SW_I2C
    if (i2c_init(SDA_PORT_NUM, SCL_PORT_NUM))
    {
        rt_kprintf("[i2c] failed to find bus with sda=%d scl=%d\n", SDA_PORT_NUM, SCL_PORT_NUM);
        return RT_ERROR;
    }
#else
    char name[RT_NAME_MAX];
    rt_strncpy(name, I2C_DEVICE_NAME, RT_NAME_MAX);
    if (i2c_init(name))
    {
        rt_kprintf("[i2c] failed to find bus %s\n", name);
        return RT_ERROR;
    }

#endif
    /* 创建 i2c 线程 */
    rt_thread_t thread = rt_thread_create("i2c", i2c_thread_entry, RT_NULL, 1024, 25, 10);
    /* 创建成功则启动线程 */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        rt_kprintf("thread_create i2c err \n");
        return RT_ERROR;
    }
    return RT_EOK;
}
INIT_APP_EXPORT(i2cBleThreadInit);

void refreshTxData(void)
{
}
