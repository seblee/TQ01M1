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
#include "mb_event_cpad.h"
#define I2C_ADDRESS 0x7e

#ifdef I2C_TOOLS_USE_SW_I2C
#define SDA_PORT_NUM 61
#define SCL_PORT_NUM 62
#else
#define I2C_DEVICE_NAME "i2c1"
#endif

rt_uint8_t recOK = 0;
static rt_uint8_t tx_buffer[20] = {1, 2, 3, 4, 5, 0, 7, 8, 9, 20};
static rt_uint8_t rx_buffer[20] = {20, 9, 8, 7, 6, 5, 4, 3, 2, 1};

#define PARA_ADDR_START 64
#define STATE_ADDR_START 500
static rt_uint8_t regMap[8 + 22][14] = {0};
static rt_uint8_t regIndex = 0;
/**********************key led beep*********************************************************/
_TKS_FLAGA_type keyState[3];
volatile _TKS_FLAGA_type keyTrg[3];

#define KEY1 keyTrg[0].bits.b0
#define KEY2 keyTrg[0].bits.b1
#define KEY3 keyTrg[0].bits.b2
#define KEY4 keyTrg[0].bits.b3

#define KEY1Restain keyTrg[1].bits.b0
#define KEY2Restain keyTrg[1].bits.b1
#define KEY3Restain keyTrg[1].bits.b2
#define KEY4Restain keyTrg[1].bits.b3

#define BLEON keyState[2].bits.b0
#define BLEONTrg keyTrg[2].bits.b0

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

rt_uint8_t beepCount = 0;
/****************************************************************************/
void refreshTxData(void);
rt_uint8_t getCheckSum(rt_uint8_t *data);
void keyRecOperation(_TKS_FLAGA_type *keyState);
void operateRxData(rt_uint8_t *rxData);
rt_uint8_t *getRegData(void);
/****************************************************************************/

static void i2c_thread_entry(void *para)
{
    rt_kprintf("i2c_thread_entry start \n");
    while (1)
    {
        /* 调用I2C设备接口传输数据 */
        refreshTxData();
        if (i2c_write(I2C_ADDRESS, tx_buffer, 20) == 1)
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
        if (i2c_read(I2C_ADDRESS, 0, rx_buffer, 20) == 20)
        {
            operateRxData(rx_buffer);
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
    rt_uint8_t *regPoint = RT_NULL;
    tx_buffer[0] = 0xff;
    tx_buffer[1] = 0xa5;

    if (BLEON)
    {
        regPoint = getRegData();
    }

    if (regPoint)
    {
        tx_buffer[2] = CMD_REG;
        tx_buffer[3] = 14;
        rt_memcpy(&tx_buffer[4], regPoint, 14);
    }
    else
    {
        tx_buffer[2] = CMD_LED;
        tx_buffer[3] = 5;
        for (rt_uint8_t i = 0; i < tx_buffer[3]; i++)
        {
            tx_buffer[4 + i] = ledState[i].byte;
        }
    }
    tx_buffer[tx_buffer[3] + 4] = getCheckSum(tx_buffer);
}

void operateRxData(rt_uint8_t *rxData)
{
    if (getCheckSum(rxData) == *(rxData + *(rxData + 3) + 4))
    {
        switch (*(rxData + 2))
        {
        case CMD_IDEL:
            break;
        case CMD_KEY:
            // rt_kprintf("i2c_read CMD_KEY %02x %02x %02x\n", *(rxData + 4), *(rxData + 5), *(rxData + 6));
            keyState[0].byte = *(rxData + 4);
            keyState[1].byte = *(rxData + 5);
            keyState[2].byte = *(rxData + 6) ^ 0x01;
            keyRecOperation(keyState);
            break;
        case CMD_LED:
            break;
        case CMD_REG:
            rt_kprintf("i2c_read CMD_REG \n");
            break;
        default:
            break;
        }
    }
}
rt_uint8_t getCheckSum(rt_uint8_t *data)
{
    rt_uint8_t checkSum = 0;
    for (rt_uint8_t i = 0; i < (*(data + 3) + 4); i++)
    {
        checkSum += *(data + i);
    }
    return checkSum;
}

void keyRecOperation(_TKS_FLAGA_type *keyState)
{
    static rt_uint8_t k_count[3] = {0};
    keyTrg[0].byte = keyState->byte & (keyState->byte ^ k_count[0]);
    k_count[0] = keyState->byte;
    keyTrg[1].byte = (keyState + 1)->byte & ((keyState + 1)->byte ^ k_count[1]);
    k_count[1] = (keyState + 1)->byte;
    keyTrg[2].byte = (keyState + 2)->byte & ((keyState + 2)->byte ^ k_count[2]);
    k_count[2] = (keyState + 2)->byte;
    if (KEY1)
    {
        rt_kprintf("key1\n");
    }
    if (KEY2)
    {
        rt_kprintf("key2\n");
    }
    if (KEY3)
    {
        rt_kprintf("key3\n");
    }
    if (KEY4)
    {
        rt_kprintf("key4\n");
    }

    if (KEY1Restain)
    {
        rt_kprintf("KEY1Restain\n");
    }
    if (KEY2Restain)
    {
        rt_kprintf("KEY2Restain\n");
    }
    if (KEY3Restain)
    {
        rt_kprintf("KEY3Restain\n");
    }
    if (KEY4Restain)
    {
        rt_kprintf("KEY4Restain\n");
    }

    if (BLEON)
    {
        rt_kprintf("BLEON\n");
    }
    if (BLEONTrg)
    {
        rt_memset(regMap, 0, sizeof(regMap));
        rt_kprintf("BLEON\n");
    }
}

rt_uint8_t *getRegData(void)
{
    rt_uint8_t temp[12];
    for (rt_uint8_t i = 0; i < 30; i++)
    {
        rt_uint16_t address = (i < 22) ? (i * 6 + PARA_ADDR_START) : ((i - 22) * 6 + STATE_ADDR_START);

        cpad_eMBRegHoldingCB(temp, address, 6, CPAD_MB_REG_READ);

        if (rt_memcmp(&regMap[i][2], temp, 12) != 0)
        {
            rt_memcpy(&regMap[i][2], temp, 12);
            regMap[i][0] = (address >> 8);
            regMap[i][1] = address & 0xff;
            return &regMap[i][0];
        }
    }
    return RT_NULL;
}
