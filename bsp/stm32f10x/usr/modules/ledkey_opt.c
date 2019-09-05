/**
 ****************************************************************************
 * @Warning Without permission from the author,Not for commercial use
 * @File    ledkey_opt.c
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

#include "ledkey_opt.h"
#include "local_status.h"
#define SAMPLE_UART_NAME "uart4"
/* 串口接收消息结构*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};
/* 串口设备句柄 */
static rt_device_t serial;
/* 消息队列控制块 */
static struct rt_messagequeue rx_mq;

unsigned char recOK = 0;
static unsigned char tx_buffer[10] = {0xa7, 0xf3, 0xaa, 0x04, 0x06};

/**********************key led*********************************************************/
_TKS_FLAGA_type keyState[2];
volatile _TKS_FLAGA_type keyTrg[2];

#define KEY1 keyTrg[0].bits.b2
#define KEY2 keyTrg[0].bits.b3
#define KEY3 keyTrg[0].bits.b1

#define KEY1Restain keyTrg[1].bits.b2
#define KEY2Restain keyTrg[1].bits.b3
#define KEY3Restain keyTrg[1].bits.b1

_USR_FLAGA_type ledState[2];
#define led1State ledState[0].sbits.s0
#define led2State ledState[0].sbits.s1
#define led3State ledState[0].sbits.s2
#define led4State ledState[0].sbits.s3
#define led5State ledState[1].sbits.s0
#define led6State ledState[1].sbits.s1
#define led7State ledState[1].sbits.s2
#define led8State ledState[1].sbits.s3

/*************************function******************************************************/

/* 接收数据回调函数 */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    rt_err_t result;
    msg.dev = dev;
    msg.size = size;

    result = rt_mq_send(&rx_mq, &msg, sizeof(msg));
    if (result == -RT_EFULL)
    {
        /* 消息队列满 */
        rt_kprintf("message queue full！\n");
    }
    return result;
}
/***
 * 
 * 
 * 
 **/

static void receiveData(unsigned char data)
{
    static unsigned char count = 0;
    if ((count == 0) && (data == 0xA7))
    {
        count++;
    }
    else if ((count == 1) && (data == 0xF2))
    {
        count++;
    }
    else if (count == 2)
    {
        keyState[0].byte = data;
        count++;
    }
    else if (count == 3)
    {
        keyState[1].byte = data;
        count = 0;
        recOK = 1;
    }
    else
    {
        count = 0;
    }
}
/****
 * 
 * 
 * 
 * 
**/
#include "req_execution.h"
#include "local_status.h"
#include "global_var.h"
extern local_reg_st l_sys;
extern sys_reg_st g_sys;
void keyRecOperation(_TKS_FLAGA_type *keyState)
{

    static rt_uint8_t k_count[2] = {0};
    keyTrg[0].byte = keyState->byte & (keyState->byte ^ k_count[0]);
    k_count[0] = keyState->byte;
    keyTrg[1].byte = (keyState + 1)->byte & ((keyState + 1)->byte ^ k_count[1]);
    k_count[1] = (keyState + 1)->byte;
    if (KEY1)
    {
        rt_kprintf("key1\n");
    }
    if (KEY2)
    {
        l_sys.OutWater_Key ^= WATER_NORMAL_ICE;
    }
    if (KEY3)
    {
        if (g_sys.config.ComPara.u16ColdWater_Mode)
        {
            led7State = LED_OFF;
            g_sys.config.ComPara.u16ColdWater_Mode = ICE_NO;
        }
        else
        {
            led7State = LED_ON;
            g_sys.config.ComPara.u16ColdWater_Mode = NORMAL_ICE;
        }
        // RAM_Write_Reg(86, g_sys.config.ComPara.u16ColdWater_Mode, 1);
        rt_kprintf("key3\n");
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
}
/**
 * 
 * 
 * 
**/
#include "sys_status.h"
void ledSendOperation(void)
{
    if (l_sys.OutWater_OK == WATER_READ)
    {
        led6State = LED_FLASH;
    }
    else
    {
        led6State = LED_ON;
    }
    if (sys_get_do_sts(DO_UV1_BPOS))
    {
        led4State = LED_FLASH;
    }
    else
    {
        led4State = LED_OFF;
    }

    tx_buffer[2] = ledState[0].byte;
    tx_buffer[3] = ledState[1].byte;
}
/***
 * 
 * 
 * 
 */
static void serial_thread_entry(void *parameter)
{
    struct rx_msg msg;
    rt_err_t result;
    rt_uint32_t rx_length;
    static unsigned char rx_buffer[RT_SERIAL_RB_BUFSZ + 1];

    rt_kprintf("**************************************start ledkey thread**************************************\r\n");

    while (1)
    {
        rt_memset(&msg, 0, sizeof(msg));
        /* 从消息队列中读取消息*/
        result = rt_mq_recv(&rx_mq, &msg, sizeof(msg), 1000);
        if (result == RT_EOK)
        {
            /* 从串口读取数据*/
            rx_length = rt_device_read(msg.dev, 0, rx_buffer, msg.size);
            for (char i = 0; i < rx_length; i++)
            {
                receiveData(rx_buffer[i]);
            }
            if (recOK)
            {
                keyRecOperation(keyState);
                ledSendOperation();
                rt_device_write(serial, 0, tx_buffer, 5);
                recOK = 0;
            }

            /* 打印数据 */
            rt_memset(rx_buffer, 0, sizeof(rx_buffer));
        }
    }
}

int ledKeyStart(void)
{
    rt_err_t ret = RT_EOK;
    char uart_name[RT_NAME_MAX];
    static char msg_pool[256];
    char str[] = "hello RT-Thread!\r\n";

    rt_strncpy(uart_name, SAMPLE_UART_NAME, RT_NAME_MAX);

    /* 查找串口设备 */
    serial = rt_device_find(uart_name);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", uart_name);
        return RT_ERROR;
    }

    /* 初始化消息队列 */
    rt_mq_init(&rx_mq, "rx_mq",
               msg_pool,              /* 存放消息的缓冲区 */
               sizeof(struct rx_msg), /* 一条消息的最大长度 */
               sizeof(msg_pool),      /* 存放消息的缓冲区大小 */
               RT_IPC_FLAG_FIFO);     /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

    /* 以 DMA 接收及轮询发送方式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_DMA_RX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_input);
    /* 发送字符串 */
    rt_device_write(serial, 0, str, (sizeof(str) - 1));

    /* 创建 serial 线程 */
    rt_thread_t thread = rt_thread_create("serial", serial_thread_entry, RT_NULL, 1024, 25, 10);
    /* 创建成功则启动线程 */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        rt_kprintf("serial thread_startup failed!\n");
        ret = RT_ERROR;
    }

    return ret;
}
