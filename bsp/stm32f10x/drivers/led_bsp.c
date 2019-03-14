/*
 * File      : led.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */
#include <rtthread.h>
#include "led_bsp.h"
#include "Delay.h"
#include "port.h"
#include "sys_conf.h"

#define led1_rcc RCC_APB2Periph_GPIOC
#define led1_gpio GPIOC
#define led1_pin (GPIO_Pin_13)

/**
  * @brief  LED initialization
  * @param  none
  * @retval none
  */
void drv_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(led1_rcc, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = led1_pin;
    GPIO_Init(led1_gpio, &GPIO_InitStructure);
}

/**
  * @brief  turn LED on
  * @param  none
  * @retval none
  */
void led_on(uint32_t n)
{
    switch (n)
    {
    case 0:
        GPIO_SetBits(led1_gpio, led1_pin);
        break;
    default:
        break;
    }
}

/**
  * @brief  turn LED off
  * @param  none
  * @retval none
  */
void led_off(uint32_t n)
{
    switch (n)
    {
    case 0:
        GPIO_ResetBits(led1_gpio, led1_pin);
        break;
    default:
        break;
    }
}

///*****************WS2811************************/

/******************************************************************
//   初始化 LED 数据 输出 口
      PA.0口
******************************************************************/
void Led_Gpio_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*****************************************************************
//   ws2811 LED显示 的 驱动 函数
      传 8位 数据 高低电平 协议 看 
      规格书 
******************************************************************/
void sendA_2811_8bits(uint8_t dat) //A??
{
    uint8_t bit;
    for (bit = 0; bit < 8; bit++)
    {
        if (dat & 0x80) //"1"
        {
            A_SET_DATA;
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            A_CLR_DATA;
            __nop();
            __nop();
            __nop();
        }
        else //"0"
        {
            A_SET_DATA;
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            A_CLR_DATA;
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();

            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            __nop();
            //					    __nop();__nop();__nop();__nop();
        }
        dat <<= 1;
    }
}
/*****************************************************************
//   向 LED 传送 3个 8位 数据 分别 代表 
     R G B 颜色 
******************************************************************/
void sendA_2811_24bits(Color_st Color)
{
    //	  sendA_2811_8bits(Color.red);//B
    //	  sendA_2811_8bits(Color.green);//R
    //	  sendA_2811_8bits(Color.blue);//G
    //硬件输出反向，顺序调换
    sendA_2811_8bits(Color.blue);  //B
    sendA_2811_8bits(Color.red);   //R
    sendA_2811_8bits(Color.green); //G
}

/******************************************************************
//   ws2811 LED帧间隔
         复位 信号
*******************************************************************/
void RESET_LED()
{
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
    __nop();
}
/*********************************************************************
//    测 试 用 的函数
      LED颜色定义
**********************************************************************/
#define COLOR_NUM 12
static const Temp_Color_st LED_Color[COLOR_NUM] =
    {
        // 			Temp	  	Color
        //		//      B    R    G
        //		{100,{0xAA,0x00,0x00}},//10℃深蓝
        //		{150,{0xFF,0x00,0x00}},//15℃深蓝
        //		{180,{0xFF,0x00,0x33}},//18℃深蓝
        //		{200,{0xFF,0x00,0xAA}},//20℃浅蓝
        //		{220,{0xFF,0x00,0xEE}},//22℃浅蓝
        //		{230,{0xBB,0x22,0xEE}},//23℃浅蓝
        //		{240,{0x00,0xCC,0xFF}},//24℃浅蓝
        //		{250,{0x00,0xEE,0xBB}},//25℃橘黄
        //		{280,{0x00,0xEE,0x44}},//28℃橘黄
        //		{300,{0x00,0xEE,0x22}},//30℃橘红
        //		{320,{0x00,0xEE,0x00}},//32℃红色
        //		{350,{0x00,0xFF,0x00}},//35℃深红
        //      R    G    B
        {100, {0x00, 0x00, 0xAA}}, //10℃深蓝
        {150, {0x00, 0x00, 0xFF}}, //15℃深蓝
        {180, {0x00, 0x33, 0xFF}}, //18℃深蓝
        {200, {0x00, 0xAA, 0xFF}}, //20℃浅蓝
        {220, {0x00, 0xEE, 0xFF}}, //22℃浅蓝
        {230, {0x22, 0xEE, 0xBB}}, //23℃浅蓝
        {240, {0xCC, 0xFF, 0x00}}, //24℃浅蓝
        {250, {0xEE, 0xBB, 0x00}}, //25℃橘黄
        {280, {0xEE, 0x44, 0x00}}, //28℃橘黄
        {300, {0xEE, 0x22, 0x00}}, //30℃橘红
        {320, {0xEE, 0x00, 0x00}}, //32℃红色
        {350, {0xFF, 0x00, 0x00}}, //35℃深红
};

void LED_Test(uint8_t Type)
{
    uint8_t i;
    uint8_t LED_N;
    uint8_t Temp_Color = 0;
    static uint8_t Last_Color = 0;
    static uint16_t Ctrl_Temp = 0;
    extern sys_reg_st g_sys;
    extern sys_reg_st g_sys;

    if (g_sys.config.general.LED_Num == 0)
    {
        return;
    }
    switch (Type) //类型
    {
    case RED:
        Temp_Color = COLOR_NUM - 1;
        break;
    case OTHER:
        switch (g_sys.config.algorithm.ctrl_target_mode) //温度模式
        {
        case 0:
            Ctrl_Temp = g_sys.status.sys_tem_hum.return_air_temp;
            break;
        case 1:
            Ctrl_Temp = g_sys.status.sys_tem_hum.supply_air_temp;
            break;
        case 2:
            Ctrl_Temp = g_sys.status.sys_tem_hum.remote_air_temp;
            break;
        default:
            Ctrl_Temp = g_sys.status.sys_tem_hum.return_air_temp;
            break;
        }

        if (Ctrl_Temp <= LED_Color[0].Temp)
        {
            Temp_Color = 0;
        }
        else
        {
            for (i = COLOR_NUM - 1; i > 0; i--)
            {
                if (Ctrl_Temp >= LED_Color[i].Temp)
                {
                    Temp_Color = i;
                    break;
                }
            }
        }
        break;
    default:
        Temp_Color = 0;
        break;
    }
    //		rt_kprintf("Ctrl_Temp = %d,Temp_Color = %d\n",Ctrl_Temp,Temp_Color);

    if (Last_Color == Temp_Color)
    {
        return;
    }
    //		rt_kprintf("Last_Color = %d\n",Last_Color);
    Last_Color = Temp_Color;
    LED_N = g_sys.config.general.LED_Num;
    ENTER_CRITICAL_SECTION(); //关全局中断
    for (i = 0; i <= LED_N; i++)
    {
        sendA_2811_24bits(LED_Color[Temp_Color].Color);
        RESET_LED();
    }
    //		switch(Color)
    //		{
    //			case RED:
    //					for(i=0;i<=LED_N;i++)
    //					{
    //							sendA_2811_24bits(0x00,0xFF,0x00);//红
    //							RESET_LED();
    //					}
    //					break;
    //			case ORANGE:
    //					for(i=0;i<=LED_N;i++)
    //					{
    //							sendA_2811_24bits(0x00,0xEE,0x76);//橙
    //			//		    sendA_2811_24bits(0x00,0x00,0xFF);//绿
    //							RESET_LED();
    //					}
    //					break;
    //			case BLUE:
    //					for(i=0;i<=LED_N;i++)
    //					{
    //					    sendA_2811_24bits(0xFF,0x00,0x00);//蓝
    //							RESET_LED();
    //					}
    //					break;
    //			default:
    //					for(i=0;i<=LED_N;i++)
    //					{
    //					    sendA_2811_24bits(0xFF,0x00,0x00);//蓝
    //							RESET_LED();
    //					}
    //					break;
    //		}
    EXIT_CRITICAL_SECTION(); //开全局中断
    //		PixelRound(1,0,0,20);
}

//uint16_t PixelBuffer[404] = {0};
//uint16_t PixelPointer = 0;

//void LED_SPI_LowLevel_Init(void)
//{
//    uint16_t i = 0;

//    GPIO_InitTypeDef  GPIO_InitStructure;
//    SPI_InitTypeDef   SPI_InitStructure;
//    DMA_InitTypeDef   DMA_InitStructure;
//
//    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
//
//    DMA_DeInit(DMA1_Channel5);
//    DMA_InitStructure.DMA_BufferSize = 0;
//    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (SPI2->DR);
//    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)PixelBuffer;
//    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
//    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
//    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//    DMA_Init(DMA1_Channel5, &DMA_InitStructure); /* DMA1 CH3 = MEM -> DR */

//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);

//		/*
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//		*/

////    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_0);
//    /* GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_0); */

//    SPI_I2S_DeInit(SPI2);

//    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
//    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
//    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
//    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
//    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
//    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
//    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; /* 64MHz / 8 = 8MHz */
//    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//    SPI_InitStructure.SPI_CRCPolynomial = 7;
//    SPI_Init(SPI2, &SPI_InitStructure);

//    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);

//    SPI_Cmd(SPI2, ENABLE);

//    for (i = 0; i < 404; i++)
//    {
//        PixelBuffer[i] = 0xAAAA;
//    }

//    PixelPointer = 0;

//}

//void LED_SPI_WriteByte(uint16_t Data)
//{
//    /* Wait until the transmit buffer is empty */
//    /*
//    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
//    {
//    }
//    */

//    PixelBuffer[PixelPointer] = Data;
//    PixelPointer++;

//    /* Send the byte */
//    /* SPI_I2S_SendData16(SPI1, Data); */
//}

//void LED_SPI_SendBits(uint8_t bits)
//{
//    int zero = 0x7000;  //111000000000000
//    int one = 0x7F00;  //111111100000000
//    int i = 0x00;

//    for (i = 0x80; i >= 0x01; i >>= 1)
//    {
//        LED_SPI_WriteByte((bits & i) ? one : zero);
//    }
//}

//void LED_SPI_SendPixel(Color_st pixel)
//{
//    /*
//     r7,r6,r5,r4,r3,r2,r1,r0,g7,g6,g5,g4,g3,g2,g1,g0,b7,b6,b5,b4,b3,b2,b1,b0
//     \_____________________________________________________________________/
//                               |      _________________...
//                               |     /   __________________...
//                               |    /   /   ___________________...
//                               |   /   /   /
//                              RGB,RGB,RGB,RGB,...,STOP
//    */

//    /*
//    	BUG Fix : Actual is GRB,datasheet is something wrong.
//    */
//    LED_SPI_SendBits(pixel.green);
//    LED_SPI_SendBits(pixel.red);
//    LED_SPI_SendBits(pixel.blue);
//}

//ErrorStatus LED_SPI_Update(Color_st buffer[], uint32_t length)
//{
//    uint8_t i = 0;
//    uint8_t m = 0;
//    if(DMA_GetCurrDataCounter(DMA1_Channel3) == 0)
//    {

//        for (i = 0; i < length; i++)
//        {
//            LED_SPI_SendPixel(buffer[i]);
//        }

//        if(length < 16)
//        {
//            for(i = 16 - length; i < length; i++)
//            {
//                for(m = 0; m < 3; m++)
//                {
//                    LED_SPI_SendBits(0x00);
//                }
//            }
//        }

//        for (i = 0; i < 20; i++)
//        {
//            LED_SPI_WriteByte(0x00);
//        }

//        PixelPointer = 0;

//        DMA_Cmd(DMA1_Channel3, DISABLE);
//        DMA_ClearFlag(DMA1_FLAG_TC3);
//        DMA_SetCurrDataCounter(DMA1_Channel3, 386);
//        DMA_Cmd(DMA1_Channel3, ENABLE);

//        return SUCCESS;
//    }
//    else
//    {
//        return ERROR;
//    }
//}

//struct Pixel LEDPixel[16] = {0x00};

//void PixelRound(uint8_t r,uint8_t g,uint8_t b,uint16_t pluse){
//	uint8_t i = 0;
//
//	for(i=0;i<16;i++){ /* ???? */
//		LEDPixel[i].red = 0x00;
//		LEDPixel[i].green = 0x00;
//		LEDPixel[i].blue = 0x00;
//	}
//
//	for(i=0;i<16;i++){
//		if(r == 1){
//			LEDPixel[i].red = 0xFF;
//		}
//		if(g == 1){
//			LEDPixel[i].green = 0xFF;
//		}
//		if(b == 1){
//			LEDPixel[i].blue = 0xFF;
//		}
//		LED_SPI_Update(LEDPixel,16);
////		Delay_sys(pluse);
//		Delay_us(pluse);
//	}
//}
