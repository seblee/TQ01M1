/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

#ifndef __USART_H__
#define __USART_H__

#include <rthw.h>
#include <rtthread.h>
#include <stdint.h>


/* USART1 */
#define UART1_GPIO_TX GPIO_Pin_9
#define UART1_GPIO_RX GPIO_Pin_10
#define UART1_GPIO GPIOA
#define UART1_DIR_GPIO GPIOA
#define UART1_DIR_GPIO_PIN GPIO_Pin_11

/* USART2 */
#define UART2_GPIO_TX GPIO_Pin_5
#define UART2_GPIO_RX GPIO_Pin_6
#define UART2_GPIO GPIOD
#define UART2_DIR_GPIO GPIOD
#define UART2_DIR_GPIO_PIN GPIO_Pin_7

/* USART3_REMAP[1:0] = 00 */
#define UART3_GPIO_TX GPIO_Pin_8
#define UART3_GPIO_RX GPIO_Pin_9
#define UART3_GPIO GPIOD
#define UART3_DIR_GPIO GPIOE
#define UART3_DIR_GPIO_PIN GPIO_Pin_6

/* UART4 */ //SURV UART
#define UART4_GPIO_TX GPIO_Pin_10
#define UART4_GPIO_RX GPIO_Pin_11
#define UART4_GPIO GPIOC
#define UART4_DIR_GPIO GPIOA
#define UART4_DIR_GPIO_PIN GPIO_Pin_15

/* UART5 */ //CPAD UART
#define UART5_GPIO_TX GPIO_Pin_12
#define UART5_GPIO_RX GPIO_Pin_2
#define UART5_GPIO_T GPIOC
#define UART5_GPIO_R GPIOD
#define UART5_DIR_GPIO GPIOD
#define UART5_DIR_GPIO_PIN GPIO_Pin_3

#define UART_ENABLE_IRQ(n)            NVIC_EnableIRQ((n))
#define UART_DISABLE_IRQ(n)           NVIC_DisableIRQ((n))

#define HEATWRITE_NUM 18
#define HEATREAD_NUM 18
//读写参数
enum
{
    HEAT_WRITEPARA = 0,
    HEAT_READPARA,
};
enum
{
    HEAT_CLOSE = 0,
    HEAT_OPEN,
};

enum
{
    UART_HEAT = 0,
    UART_PM25,
    UART_NUM,
};
#define PROTOCOL_FRAME_MaxLen 20
typedef enum
{
    PROTOCOL_STACK_IDLE = 0, /*!< Receiver is in idle state. */
    PROTOCOL_STACK_CK,       /*!< Frame is beeing received. */
    PROTOCOL_STACK_RCV,      /*!< Frame is beeing received. */
    PROTOCOL_STACK_CS,       /*!< Frame is beeing received. */
    PROTOCOL_STACK_END,      /*!< If the frame is invalid. */
} UartRcvState;

typedef enum
{
    RECV_Wait,  //接收等待
    RECV_Going, //接收进行
    RECV_Over,  //接收完成
    SEND_Wait,  //发送等待
    SEND_Going, //发送进行
    SEND_Over,  //发送完成
    INIT_Apply  //初始化
} PROTOCOL_STATUS;

typedef struct
{
    uint8_t CommID;       // 串口号
    uint8_t Baudrate;     // 通道通讯的波特率
    uint8_t StatckStatus; // 指示当前协议栈解析状态(空闲->地址解析->)
    uint8_t StatckType;   // 地址类型(私有地址[bit0],全AA或部分AA为组地址[bit1],全99为广播地址[bit2],[bit3...bit6] 未使用,保留为0,帧接收成功[bit7])
    uint8_t DataCount;    // 接收到的数据帧长度()
} ProtocolLayer;

#define PROTOCOL_FRAME_ByteGap 250
#define PROTOCOL_FRAME_SendGap 10

extern ProtocolLayer Protocol[UART_NUM];
extern uint8_t g_ComBuff[UART_NUM][PROTOCOL_FRAME_MaxLen];
extern PROTOCOL_STATUS g_ComStat[UART_NUM]; //通讯通道工作状态

void rt_hw_usart_init(void);
extern void xPort_Usart_Init(uint8_t ucMB_Number);
extern uint8_t Heat_Send(uint8_t u8Type, uint8_t u8OC, uint8_t u8Temp, uint16_t u16WL);
extern void Comm_Service(void);
#endif
