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

/* board configuration */

/* whether use board external SRAM memory */
// <e>Use external SRAM memory on the board
// 	<i>Enable External SRAM memory
#define STM32_EXT_SRAM          0
//	<o>Begin Address of External SRAM
//		<i>Default: 0x68000000
#define STM32_EXT_SRAM_BEGIN    0x68000000 /* the begining address of external SRAM */
//	<o>End Address of External SRAM
//		<i>Default: 0x68080000
#define STM32_EXT_SRAM_END      0x68080000 /* the end address of external SRAM */
// </e>

// <o> Internal SRAM memory size[Kbytes] <8-64>
//	<i>Default: 64
#define STM32_SRAM_SIZE         52
#define STM32_SRAM_END          (0x20000000 + STM32_SRAM_SIZE * 1024)

// <<< Use Configuration Wizard in Context Menu >>>

/* USART1 */
#define UART1_GPIO_TX		GPIO_Pin_9
#define UART1_GPIO_RX		GPIO_Pin_10
#define UART1_GPIO			GPIOA
#define UART1_DIR_GPIO	GPIOA
#define UART1_DIR_GPIO_PIN	GPIO_Pin_11

/* USART2 */
#define UART2_GPIO_TX	    GPIO_Pin_5
#define UART2_GPIO_RX	    GPIO_Pin_6
#define UART2_GPIO	    	GPIOD
#define UART2_DIR_GPIO		GPIOD
#define UART2_DIR_GPIO_PIN	GPIO_Pin_7

/* USART3_REMAP[1:0] = 00 */
#define UART3_GPIO_TX		GPIO_Pin_8
#define UART3_GPIO_RX		GPIO_Pin_9
#define UART3_GPIO			GPIOD
#define UART3_DIR_GPIO	GPIOE
#define UART3_DIR_GPIO_PIN	GPIO_Pin_6

/* UART4 */ //SURV UART
#define UART4_GPIO_TX		GPIO_Pin_10
#define UART4_GPIO_RX		GPIO_Pin_11
#define UART4_GPIO			GPIOC
#define UART4_DIR_GPIO	GPIOA
#define UART4_DIR_GPIO_PIN	GPIO_Pin_15

/* UART5 *///CPAD UART
#define UART5_GPIO_TX		GPIO_Pin_12
#define UART5_GPIO_RX		GPIO_Pin_2
#define UART5_GPIO_T		GPIOC
#define UART5_GPIO_R		GPIOD
#define UART5_DIR_GPIO	GPIOD
#define UART5_DIR_GPIO_PIN	GPIO_Pin_3


//#define CONSOLE_RS485_SND_MODE  GPIO_SetBits(GPIOD,GPIO_Pin_10)
//#define CONSOLE_RS485_RCV_MODE  GPIO_ResetBits(GPIOD,GPIO_Pin_10)


/* USART driver select. */
//#define RT_USING_UART1
//#define RT_USING_UART2
#define RT_USING_UART3
//#define RT_USING_UART4
#define RT_USING_UART5

void rt_hw_board_init(void);
void hw_drivers_init(void);
#endif /* __BOARD_H__ */
