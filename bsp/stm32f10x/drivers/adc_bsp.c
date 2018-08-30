/**
  ******************************************************************************
  * @file    bsp_xxx.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   adc1 应用bsp / DMA 模式
  ******************************************************************************/ 
/* Private variables ---------------------------------------------------------*/
#include "adc_bsp.h"
#include <rtthread.h>

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)
__IO uint16_t ADC1ConvertedValue[AI_MAX_CNT];
__IO uint16_t ADC1Buff[AI_MAX_CNT * MAX_ADBUFEVERY];

/**
  * @brief  initialize ADC1 clock , gpio and configurate adc dma setting
  * @param  none
  * @retval none
  */	//模拟输入初始化
uint16_t drv_adc_dma_init(void)	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef   ADC_InitStructure;
	DMA_InitTypeDef   DMA_InitStructure;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
/* Enable peripheral clocks --------------------------------------------------*/
  /* Enable ADC1 and GPIOC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA| RCC_APB2Periph_GPIOC, ENABLE);

  /* Configure PC.01 and PC.04 (Channel11 and Channel14) as analog input -----*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 ; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);


  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC1ConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = AI_MAX_CNT;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; 
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);

  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = AI_MAX_CNT;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel11, channel14, channel16 and channel17 configurations */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1,  ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2,  ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3,  ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4,  ADC_SampleTime_239Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_3,  5,  ADC_SampleTime_239Cycles5);  
  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  
  /* Enable TempSensor and Vrefint channels: channel16 and Channel17 */
  ADC_TempSensorVrefintCmd(ENABLE);

  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);
  
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
     
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  
  /* Test on Channel 1 DMA1_FLAG_TC flag */
  while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
  
  /* Clear Channel 1 DMA1_FLAG_TC flag */
  DMA_ClearFlag(DMA1_FLAG_TC1);	
	return 0;
}




