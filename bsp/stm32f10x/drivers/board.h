/*
 * File      : board.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-09-22     Bernard      add board.h to this bsp
 */

// <<< Use Configuration Wizard in Context Menu >>>
#ifndef __BOARD_H__
#define __BOARD_H__

#include "stm32f10x.h"

/******************************************************************************
                                位带操作
******************************************************************************/
///////////////////////////////////////////////////////////////
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))
//IO口地址映射
#define GPIOA_ODR_Addr (GPIOA_BASE + 12) //0x4001080C
#define GPIOB_ODR_Addr (GPIOB_BASE + 12) //0x40010C0C
#define GPIOC_ODR_Addr (GPIOC_BASE + 12) //0x4001100C
#define GPIOD_ODR_Addr (GPIOD_BASE + 12) //0x4001140C
#define GPIOE_ODR_Addr (GPIOE_BASE + 12) //0x4001180C
#define GPIOF_ODR_Addr (GPIOF_BASE + 12) //0x40011A0C
#define GPIOG_ODR_Addr (GPIOG_BASE + 12) //0x40011E0C

#define GPIOA_IDR_Addr (GPIOA_BASE + 8) //0x40010808
#define GPIOB_IDR_Addr (GPIOB_BASE + 8) //0x40010C08
#define GPIOC_IDR_Addr (GPIOC_BASE + 8) //0x40011008
#define GPIOD_IDR_Addr (GPIOD_BASE + 8) //0x40011408
#define GPIOE_IDR_Addr (GPIOE_BASE + 8) //0x40011808
#define GPIOF_IDR_Addr (GPIOF_BASE + 8) //0x40011A08
#define GPIOG_IDR_Addr (GPIOG_BASE + 8) //0x40011E08

//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)    BIT_ADDR(GPIOA_ODR_Addr, n) //输出
#define PAin(n)     BIT_ADDR(GPIOA_IDR_Addr, n)  //输入

#define PBout(n)    BIT_ADDR(GPIOB_ODR_Addr, n) //输出
#define PBin(n)     BIT_ADDR(GPIOB_IDR_Addr, n)  //输入

#define PCout(n)    BIT_ADDR(GPIOC_ODR_Addr, n) //输出
#define PCin(n)     BIT_ADDR(GPIOC_IDR_Addr, n)  //输入

#define PDout(n)    BIT_ADDR(GPIOD_ODR_Addr, n) //输出
#define PDin(n)     BIT_ADDR(GPIOD_IDR_Addr, n)  //输入

#define PEout(n)    BIT_ADDR(GPIOE_ODR_Addr, n) //输出
#define PEin(n)     BIT_ADDR(GPIOE_IDR_Addr, n)  //输入

#define PFout(n)    BIT_ADDR(GPIOF_ODR_Addr, n) //输出
#define PFin(n)     BIT_ADDR(GPIOF_IDR_Addr, n)  //输入

#define PGout(n)    BIT_ADDR(GPIOG_ODR_Addr, n) //输出
#define PGin(n)     BIT_ADDR(GPIOG_IDR_Addr, n)  //输入

/* board configuration */

/* whether use board external SRAM memory */
// <e>Use external SRAM memory on the board
//  <i>Enable External SRAM memory
#define STM32_EXT_SRAM          0
//  <o>Begin Address of External SRAM
//      <i>Default: 0x68000000
#define STM32_EXT_SRAM_BEGIN    0x68000000 /* the begining address of external SRAM */
//  <o>End Address of External SRAM
//      <i>Default: 0x68080000
#define STM32_EXT_SRAM_END      0x68080000 /* the end address of external SRAM */
// </e>

// <o> Internal SRAM memory size[Kbytes] <8-64>
//  <i>Default: 64
#define STM32_SRAM_SIZE         52
#define STM32_SRAM_END          (0x20000000 + STM32_SRAM_SIZE * 1024)

// <<< Use Configuration Wizard in Context Menu >>>

/* USART driver select. */
//#define RT_USING_UART1
//#define RT_USING_UART2
#define RT_USING_UART3
// #define RT_USING_UART4
#define RT_USING_UART5

void rt_hw_board_init(void);
void hw_drivers_init(void);
#endif /* __BOARD_H__ */
