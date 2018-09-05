///*
// * FreeModbus Libary: STM32 Port
// * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
// *
// * This library is free software; you can redistribute it and/or
// * modify it under the terms of the GNU Lesser General Public
// * License as published by the Free Software Foundation; either
// * version 2.1 of the License, or (at your option) any later version.
// *
// * This library is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// * Lesser General Public License for more details.
// *
// * You should have received a copy of the GNU Lesser General Public
// * License along with this library; if not, write to the Free Software
// * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
// *
// * File: $Id: portserial.c,v 1.60 2013/08/13 15:07:05 Armink $
// */

//#include "port.h"

///* ----------------------- Modbus includes ----------------------------------*/
//#include "mb.h"
//#include "mbport.h"
///* ----------------------- static functions ---------------------------------*/
//static void prvvUARTTxReadyISR(void);
//static void prvvUARTRxISR(void);
///* ----------------------- Start implementation -----------------------------*/
//uint8_t isr_flag;

//void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
//{
//	if (xRxEnable)
//	{
//		/* 485通信时，等待串口移位寄存器中的数据发送完成后，再去使能485的接收、失能485的发送*/
//		while (!USART_GetFlagStatus(UART4,USART_FLAG_TC));
//		SLAVE_RS485_RECEIVE_MODE;
//		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
//	}
//	else
//	{
//		SLAVE_RS485_SEND_MODE;
//		USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
//	}
//	if (xTxEnable)
//	{
//		USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
//	}
//	else
//	{
//		USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
//	}
//}

//void vMBPortClose(void)
//{
//	USART_ITConfig(UART4, USART_IT_TXE | USART_IT_RXNE, DISABLE);
//	USART_Cmd(UART4, DISABLE);
//}
////默认一个从机 串口5 波特率可设置  奇偶检验可设置
//BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
//		eMBParity eParity)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	//======================时钟初始化=======================================
//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA ,	ENABLE);
//	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_UART4,ENABLE);
//
//	//======================IO初始化=========================================
//	//UART4_TX
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	//UART4_RX
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	//配置485发送和接收模式
//	//TODO   暂时先写B13 等之后组网测试时再修改
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	//======================串口初始化=======================================
//	USART_InitStructure.USART_BaudRate = ulBaudRate;
//	//设置校验模式
//	switch (eParity)
//	{
//	case MB_PAR_NONE: //无校验
//		USART_InitStructure.USART_Parity = USART_Parity_No;
//		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//		break;
//	case MB_PAR_ODD: //奇校验
//		USART_InitStructure.USART_Parity = USART_Parity_Odd;
//		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
//		break;
//	case MB_PAR_EVEN: //偶校验
//		USART_InitStructure.USART_Parity = USART_Parity_Even;
//		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
//		break;
//	default:
//		return FALSE;
//	}

//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_HardwareFlowControl =	USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//

//	ENTER_CRITICAL_SECTION(); //关全局中断

//	USART_Init(UART4, &USART_InitStructure);
//	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
//	USART_Cmd(UART4, ENABLE);

//	//=====================中断初始化======================================
//	//设置NVIC优先级分组为Group2：0-3抢占式优先级，0-3的响应式优先级
////	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

//	EXIT_CRITICAL_SECTION(); //开全局中断

//	return TRUE;
//}

//BOOL xMBPortSerialPutByte(CHAR ucByte)
//{
//	USART_SendData(UART4, ucByte);
//	return TRUE;
//}

//BOOL xMBPortSerialGetByte(CHAR * pucByte)
//{
//	*pucByte = USART_ReceiveData(UART4);
//	return TRUE;
//}

///*
// * Create an interrupt handler for the transmit buffer empty interrupt
// * (or an equivalent) for your target processor. This function should then
// * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
// * a new character can be sent. The protocol stack will then call
// * xMBPortSerialPutByte( ) to send the character.
// */
//static void prvvUARTTxReadyISR(void)
//{
//	pxMBFrameCBTransmitterEmpty();
//}

///*
// * Create an interrupt handler for the receive interrupt for your target
// * processor. This function should then call pxMBFrameCBByteReceived( ). The
// * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
// * character.
// */
//static void prvvUARTRxISR(void)
//{
//	pxMBFrameCBByteReceived();
//}
///*******************************************************************************
// * Function Name  : UART4_IRQHandler
// * Description    : This function handles UART4 global interrupt request.
// * Input          : None
// * Output         : None
// * Return         : None
// *******************************************************************************/
//void UART4_IRQHandler(void)
//{
//	extern volatile uint8_t rx1_cnt;
//	rt_interrupt_enter();
//	//溢出错误
//	if (USART_GetFlagStatus(UART4, USART_FLAG_ORE) == SET)
//	{
//		prvvUARTRxISR();
//	}
//	//接收中断
//	if (USART_GetITStatus(UART4, USART_IT_RXNE) == SET)
//	{
//		USART_ClearITPendingBit( UART4, USART_IT_RXNE);
//		prvvUARTRxISR();
//	}
//	//发送中断
//	if (USART_GetITStatus(UART4, USART_IT_TXE) == SET)
//	{
//		prvvUARTTxReadyISR();
//	}
//	rt_interrupt_leave();
//}

/*
 * FreeModbus Libary: STM32 Port
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
 * File: $Id: portserial.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "board.h"
/* ----------------------- static functions ---------------------------------*/
//static void prvvUARTTxReadyISR(void);
//static void prvvUARTRxISR(void);
/* ----------------------- Start implementation -----------------------------*/
uint8_t isr_flag;

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	if (xRxEnable)
	{
		/* 485通信时，等待串口移位寄存器中的数据发送完成后，再去使能485的接收、失能485的发送*/
		while (!USART_GetFlagStatus(USART3, USART_FLAG_TC))
			;
		SLAVE_RS485_RECEIVE_MODE;
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	}
	else
	{
		SLAVE_RS485_SEND_MODE;
		USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
	}
	if (xTxEnable)
	{
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
	}
	else
	{
		USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	}
}

void vMBPortClose(void)
{
	USART_ITConfig(USART3, USART_IT_TXE | USART_IT_RXNE, DISABLE);
	USART_Cmd(USART3, DISABLE);
}
//默认一个从机 串口3 波特率可设置  奇偶检验可设置
BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
					   eMBParity eParity)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//======================时钟初始化=======================================
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);

	//======================IO初始化=========================================
	//UART3_TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = UART3_GPIO_TX;
	GPIO_Init(UART3_GPIO, &GPIO_InitStructure);
	//UART3_RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = UART3_GPIO_RX;
	GPIO_Init(UART3_GPIO, &GPIO_InitStructure);
	//配置485发送和接收模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = UART3_DIR_GPIO_PIN;
	GPIO_Init(UART3_DIR_GPIO, &GPIO_InitStructure);
	//======================串口初始化=======================================
	USART_InitStructure.USART_BaudRate = ulBaudRate;
	//设置校验模式
	switch (eParity)
	{
	case MB_PAR_NONE: //无校验
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		break;
	case MB_PAR_ODD: //奇校验
		USART_InitStructure.USART_Parity = USART_Parity_Odd;
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		break;
	case MB_PAR_EVEN: //偶校验
		USART_InitStructure.USART_Parity = USART_Parity_Even;
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		break;
	default:
		return FALSE;
	}

	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	ENTER_CRITICAL_SECTION(); //关全局中断

	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART3, ENABLE);

	//=====================中断初始化======================================
	//设置NVIC优先级分组为Group2：0-3抢占式优先级，0-3的响应式优先级
	//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	EXIT_CRITICAL_SECTION(); //开全局中断

	return TRUE;
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
	USART_SendData(USART3, ucByte);
	return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR *pucByte)
{
	*pucByte = USART_ReceiveData(USART3);
	return TRUE;
}

/* 
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR(void)
{
	pxMBFrameCBTransmitterEmpty();
}

/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR(void)
{
	pxMBFrameCBByteReceived();
}
#ifndef RT_USING_UART3
/*******************************************************************************
 * Function Name  : USART3_IRQHandler
 * Description    : This function handles USART3 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void USART3_IRQHandler(void)
{
	extern volatile uint8_t rx1_cnt;
	rt_interrupt_enter();
	//溢出错误
	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) == SET)
	{
		prvvUARTRxISR();
	}
	//接收中断
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		prvvUARTRxISR();
	}
	//发送中断
	if (USART_GetITStatus(USART3, USART_IT_TXE) == SET)
	{
		prvvUARTTxReadyISR();
	}
	rt_interrupt_leave();
}

#endif
