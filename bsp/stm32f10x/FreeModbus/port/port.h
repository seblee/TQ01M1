/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: port.h ,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

#ifndef _PORT_H
#define _PORT_H

#include <stm32f10x_conf.h>
#include "mbconfig.h"
#include <rthw.h>
#include <rtthread.h>

#include <assert.h>
#include <inttypes.h>

#define INLINE
#define PR_BEGIN_EXTERN_C \
    extern "C"            \
    {
#define PR_END_EXTERN_C }

// #define CONSOLE_RS485_SND_MODE GPIO_SetBits(GPIOD, GPIO_Pin_3)
// #define CONSOLE_RS485_RCV_MODE GPIO_ResetBits(GPIOD, GPIO_Pin_3)

// #define SLAVE_RS485_SEND_MODE GPIO_SetBits(GPIOE, GPIO_Pin_6) //监控口
// #define SLAVE_RS485_RECEIVE_MODE GPIO_ResetBits(GPIOE, GPIO_Pin_6)

#define CPAD_SLAVE_RS485_SEND_MODE GPIO_SetBits(GPIOD, GPIO_Pin_7) //HMI口
#define CPAD_SLAVE_RS485_RECEIVE_MODE GPIO_ResetBits(GPIOD, GPIO_Pin_7)

#define MASTER_RS485_SEND_MODE GPIO_SetBits(GPIOA, GPIO_Pin_11) //内部通信口
#define MASTER_RS485_RECEIVE_MODE GPIO_ResetBits(GPIOA, GPIO_Pin_11)

#define MASTER_RS485_SEND_MODE_01 GPIO_SetBits(GPIOA, GPIO_Pin_15) //内部通信口2
#define MASTER_RS485_RECEIVE_MODE_01 GPIO_ResetBits(GPIOA, GPIO_Pin_15)

// #define MASTER_RS485_SEND_MODE_02 GPIO_SetBits(GPIOD, GPIO_Pin_3) //内部通信口3
// #define MASTER_RS485_RECEIVE_MODE_02 GPIO_ResetBits(GPIOD, GPIO_Pin_3)

enum
{
    UPORT_MBMASTER = 1,
    UPORT_CPAD,
    UPORT_SLAVE,
    UPORT_PM25,
    UPORT_CONSEL,
    UPORT_MAX,
};

#define ENTER_CRITICAL_SECTION() EnterCriticalSection()
#define EXIT_CRITICAL_SECTION() ExitCriticalSection()

typedef uint8_t BOOL;

typedef unsigned char UCHAR;
typedef char CHAR;

typedef uint16_t USHORT;
typedef int16_t SHORT;

typedef uint32_t ULONG;
typedef int32_t LONG;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

void EnterCriticalSection(void);
void ExitCriticalSection(void);
void vMBDelay(ULONG nCount);

#endif
