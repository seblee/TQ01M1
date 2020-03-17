/**
 ****************************************************************************
 * @Warning Without permission from the author,Not for commercial use
 * @File    ble_key.c
 * @Author  xiaowine@cee0.com
 * @date    
 * @version V1.0
 *************************************************
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
#include "sys_status.h"
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
#define PARA_NUM 22
#define STATE_ADDR_START 500
#define STATE_NUM 8

static rt_uint8_t regMap[PARA_NUM + STATE_NUM][14] = {0};
/**********************key led beep*********************************************************/
_TKS_FLAGA_type keyState[3];
_TKS_FLAGA_type keyUpState[3];
volatile _TKS_FLAGA_type keyTrg[3];
volatile _TKS_FLAGA_type keyUpTrg[3];

#define I2CKEY1 keyState[0].bits.b0
#define I2CKEY2 keyState[0].bits.b1
#define I2CKEY3 keyState[0].bits.b2
#define I2CKEY4 keyState[0].bits.b3

#define KEY1Trg keyTrg[0].bits.b0
#define KEY2Trg keyTrg[0].bits.b1
#define KEY3Trg keyTrg[0].bits.b2
#define KEY4Trg keyTrg[0].bits.b3

#define KEY1Restain keyTrg[1].bits.b0
#define KEY2Restain keyTrg[1].bits.b1
#define KEY3Restain keyTrg[1].bits.b2
#define KEY4Restain keyTrg[1].bits.b3

#define KEY1UpTrg keyUpTrg[0].bits.b0
#define KEY2UpTrg keyUpTrg[0].bits.b1
#define KEY3UpTrg keyUpTrg[0].bits.b2
#define KEY4UpTrg keyUpTrg[0].bits.b3

#define KEY1UpRestain keyUpTrg[1].bits.b0
#define KEY2UpRestain keyUpTrg[1].bits.b1
#define KEY3UpRestain keyUpTrg[1].bits.b2
#define KEY4UpRestain keyUpTrg[1].bits.b3

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

rt_uint8_t beepCount = 0;

static rt_uint16_t u16WL = 0;
/****************************************************************************/
void refreshTxData(void);
rt_uint8_t getCheckSum(rt_uint8_t *data);
void keyRecOperation(_TKS_FLAGA_type *keyState);
void operateRxData(rt_uint8_t *rxData);
rt_uint8_t *getRegData(void);
static void caculateLed(void);
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
    rt_thread_t thread = rt_thread_create("i2c", i2c_thread_entry, RT_NULL, 4096, 25, 10);
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
        tx_buffer[2] = I2C_REG_UP;
        tx_buffer[3] = 14;
        rt_memcpy(&tx_buffer[4], regPoint, 14);
    }
    else
    {
        tx_buffer[2] = I2C_LED;
        tx_buffer[3] = 5;
        caculateLed();
        for (rt_uint8_t i = 0; i < 4; i++)
        {
            tx_buffer[4 + i] = ledState[i].byte;
        }
        tx_buffer[8] = beepCount;
        beepCount = 0;
        rt_memset(ledState, 0, sizeof(ledState));
    }
    tx_buffer[tx_buffer[3] + 4] = getCheckSum(tx_buffer);
}

void operateRxData(rt_uint8_t *rxData)
{
    if (getCheckSum(rxData) == *(rxData + *(rxData + 3) + 4))
    {
        switch (*(rxData + 2))
        {
        case I2C_IDEL:
            break;
        case I2C_KEY:
        {
            keyState[0].byte = *(rxData + 4);
            keyState[1].byte = *(rxData + 5);
            keyState[2].byte = *(rxData + 6) ^ 0x01;
            keyRecOperation(keyState);
            break;
        }
        case I2C_LED:
            break;
        case I2C_REG_UP:
            rt_kprintf("i2c_read I2C_REG_UP \n");
            break;
        case I2C_REG_DOWN:
        {
            rt_uint8_t length = *(rxData + 3) - 2;
            rt_uint8_t row, index;
            rt_uint16_t address = (*(rxData + 4) << 8) + *(rxData + 5);
            rt_kprintf("length:%d address:%d \n", length, address);
            cpad_eMBRegHoldingCB(rxData + 6, address, length / 2, CPAD_MB_REG_MULTIPLE_WRITE);
            if (address > STATE_ADDR_START)
            {
                row = (address - STATE_ADDR_START) / 6 + PARA_NUM;
            }
            else
            {
                row = (address - PARA_ADDR_START) / 6;
            }
            index = ((address - PARA_ADDR_START) % 6) * 2;

            rt_kprintf("((length > (12 - index)) ? (12 - index) : length):%d row:%d \n", (length > (12 - index)) ? (12 - index) : length, row);
            rt_memcpy(&regMap[row][index + 2], rxData + 6, (length > (12 - index)) ? (12 - index) : length);
            if ((12 - index) < length)
            {
                row++;
                rt_kprintf("(length + index - 12):%d row:%d \n", length + index - 12, row);
                rt_memcpy(&regMap[row][2], rxData + 12 - index, length + index - 12);
            }

            break;
        }
        default:
            break;
        }
    }
    else
    {
        for (char i = 0; i < 20; i++)
        {
            rt_kprintf("%02x ", *(rxData + i));
        }
        rt_kprintf("\r\n");
        rt_kprintf("***checkSum err \n");
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
#include "req_execution.h"
#include "local_status.h"
#include "global_var.h"
extern local_reg_st l_sys;
extern sys_reg_st g_sys;
void keyRecOperation(_TKS_FLAGA_type *keyState)
{
    static rt_uint8_t k_count[3] = {0};
    static rt_uint8_t k_Upcount[3] = {0};

    keyTrg[0].byte = keyState->byte & (keyState->byte ^ k_count[0]);
    k_count[0] = keyState->byte;
    keyTrg[1].byte = (keyState + 1)->byte & ((keyState + 1)->byte ^ k_count[1]);
    k_count[1] = (keyState + 1)->byte;
    keyTrg[2].byte = (keyState + 2)->byte & ((keyState + 2)->byte ^ k_count[2]);
    k_count[2] = (keyState + 2)->byte;

    for (rt_uint8_t i = 0; i < 3; i++)
    {
        keyUpState[i].byte = ~(keyState + i)->byte;
    }
    keyUpTrg[0].byte = keyUpState[0].byte & (keyUpState[0].byte ^ k_Upcount[0]);
    k_Upcount[0] = keyUpState[0].byte;
    keyUpTrg[1].byte = keyUpState[1].byte & (keyUpState[1].byte ^ k_Upcount[1]);
    k_Upcount[1] = keyUpState[1].byte;
    keyUpTrg[2].byte = keyUpState[2].byte & (keyUpState[2].byte ^ k_Upcount[2]);
    k_Upcount[2] = keyUpState[2].byte;

    if (KEY1Trg) //出水
    {

        if ((u16WL & D_L) == 0)
        {
            beepCount += 1;
        }
        else
        {
            g_sys.config.ComPara.u16Water_Mode |= WATER_NORMAL_ICE;
            g_sys.config.ComPara.u16Water_Flow = 5000;
        }

        rt_kprintf("key1\n");
    }
    if (KEY1UpTrg) //出水
    {

        g_sys.config.ComPara.u16Water_Mode &= ~WATER_NORMAL_ICE;
        g_sys.config.ComPara.u16Water_Flow = 0;
        rt_kprintf("KEY1UpTrg\n");
    }
    if (KEY2Trg)
    {
        if ((u16WL & D_L) == 0)
        {
            beepCount += 1;
        }
        else
        {
            if (l_sys.ChildLock_Key == 0)
            {
                beepCount += 1;
                childLockLed = STATE_LED_FLASH_2_T;
            }
            else
            {
                g_sys.config.ComPara.u16Water_Mode |= WATER_HEAT;
                g_sys.config.ComPara.u16Water_Flow = 5000;
            }
        }

        rt_kprintf("key2\n");
    }
    if (KEY2UpTrg) //出水
    {
        g_sys.config.ComPara.u16Water_Mode &= ~WATER_HEAT;
        g_sys.config.ComPara.u16Water_Flow = 0;
        rt_kprintf("KEY2UpTrg\n");
    }
    if (KEY3Trg) //制冰水
    {
        if (g_sys.config.ComPara.u16ColdWater_Mode)
        {
            g_sys.config.ComPara.u16ColdWater_Mode = ICE_NO;
        }
        else
        {
            g_sys.config.ComPara.u16ColdWater_Mode = NORMAL_ICE;
        }
        RAM_Write_Reg(86, g_sys.config.ComPara.u16ColdWater_Mode, 1);
        rt_kprintf("key3\n");
    }
    if (KEY4Trg)
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
        l_sys.ChildLock_Cnt[0] = 0;
        l_sys.ChildLock_Key = 1;
        l_sys.ChildLock_Cnt[1] = ChildKey_Lose;
        rt_kprintf("KEY4Restain\n");
    }

    if (BLEON)
    {
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
        rt_uint16_t address = (i < PARA_NUM) ? (i * 6 + PARA_ADDR_START) : ((i - PARA_NUM) * 6 + STATE_ADDR_START);
        cpad_eMBRegHoldingCB(temp, address, 6, CPAD_MB_REG_READ);
        if (rt_memcmp(&regMap[i][2], temp, 12) != 0)
        {
            regMap[i][0] = (address >> 8);
            regMap[i][1] = address & 0xff;
            rt_memcpy(&regMap[i][2], temp, 12);
            rt_kprintf("index:%04d\n", address - ((i < PARA_NUM) ? PARA_ADDR_START : 0));
            return &regMap[i][0];
        }
    }
    return RT_NULL;
}

static void caculateLed(void)
{
    // WaterLevel
    u16WL = Get_Water_level();
    if (u16WL & D_L)
    {
        levelL = STATE_LED_ON;
        if (l_sys.OutWater_Key & WATER_NORMAL_ICE)
        {
            normalWaterLed = STATE_LED_FLASH_2HZ;
        }
        else
        {
            normalWaterLed = STATE_LED_ON;
        }
        if (l_sys.HeatWater_st == HEAT_OUTWATER)
        {
            hotWaterLed = STATE_LED_FLASH_2HZ;
        }
        else
        {
            hotWaterLed = STATE_LED_ON;
        }
    }
    else
    {
        if (normalWaterLed < STATE_LED_FLASH_2_T)
            normalWaterLed = STATE_LED_OFF;
        hotWaterLed = STATE_LED_OFF;
        levelL = STATE_LED_OFF;
    }

    if (g_sys.config.ComPara.u16ColdWater_Mode == ICE_NO)
    {
        refrigerateLed = STATE_LED_OFF;
    }
    else
    {
        static rt_uint8_t ColdWaterStateBak = 0;
        if (l_sys.ColdWaterState == 1)
        {
            refrigerateLed = STATE_LED_FLASH_0_5HZ;
        }
        else if (l_sys.ColdWaterState == 2)
        {
            refrigerateLed = STATE_LED_FLASH_2HZ;
        }
        else if ((l_sys.ColdWaterState == 3) || (l_sys.ColdWaterState == 5))
        {
            refrigerateLed = STATE_LED_ON;
        }
        else if (l_sys.ColdWaterState == 4)
        {
            refrigerateLed = STATE_LED_ON;
            if (ColdWaterStateBak == 3)
            {
                beepCount += 3;
            }
        }
        else
        {
            refrigerateLed = STATE_LED_ON;
        }
        ColdWaterStateBak = l_sys.ColdWaterState;
    }
    if (l_sys.ChildLock_Key == 0)
    {
        if (childLockLed < STATE_LED_FLASH_2_T)
            childLockLed = STATE_LED_OFF;
    }
    else
    {
        childLockLed = STATE_LED_ON;
    }

    if ((g_sys.status.alarm_bitmap[0] == 0) && (g_sys.status.alarm_bitmap[1] == 0))
    {
        warningLed = STATE_LED_OFF;
    }
    else
    {
        warningLed = STATE_LED_ON;
    }

    if (u16WL & D_M)
    {
        levelH = STATE_LED_ON;
    }
    else
    {
        levelH = STATE_LED_OFF;
    }
    if (u16WL & D_ML)
    {
        levelM = STATE_LED_ON;
    }
    else
    {
        levelM = STATE_LED_OFF;
    }

    if (u16WL & D_L)
    {
        levelL = STATE_LED_ON;
    }
    else
    {
        levelL = STATE_LED_OFF;
    }
}
