#include "stm32f10x.h"
#include <rtthread.h>
#include <components.h>
#include "global_var.h"

/****************************************************
PWM channel map
PWM_AO1	<->	TIM4_CH3 PB_8
****************************************************/


#define AO_CHAN_MAX_NUM		1	

static void pwm_rcc_conf(void);
static void pwm_gpio_conf(void);
static void drv_pwm_conf(uint32_t freq);


/**
  * @brief  set designated channel's pwm wave duty cycle, range is [0,100]
  * @param  channel: range from 1 to 7
	* @param  duty_cycle:	range from 0%~100%, 1% step size
  * @retval 
			@arg 1: success
			@arg 0: error	
  */
uint16_t pwm_conf(uint16_t channel, uint16_t duty_cycle)
{    
	uint16_t ret = 0;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
  TIM_OCInitStructure.TIM_Pulse = duty_cycle*10;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;	
	
	if(duty_cycle > 100)
		return 0;
	if((channel == 0)||(channel > AO_CHAN_MAX_NUM))
		return 0;	
	switch (channel)
	{
		case ( 1 ):
		{			
			TIM_OC4Init(TIM8, &TIM_OCInitStructure);
			TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);
			ret = 1;
			break;
		}
		default:
		{
			ret = 0;
			break;
		}
	}
	return ret;
}


/**
  * @brief  PWM timer parameter configuration
  * @param  none
	* @param  none
  * @retval none
  */
uint16_t pwm_set_ao(uint8_t channel, uint16_t ao_data)
{
		if(channel>AO_CHAN_MAX_NUM)
		{
				return 0;
		}
		else 
		{
				if(ao_data>100)
				{
						return 0;						
				}
				else
				{
					rt_kprintf("ao_data = %d\n",ao_data);	
						pwm_conf(channel,(100-ao_data));
						return 1;
				}
		}
}

/**
  * @brief  ao driver initialization
  * @param  none
  * @retval none
  */
void drv_pwm_init(void)
{
		uint16_t i;
		pwm_rcc_conf();
		pwm_gpio_conf();
		drv_pwm_conf(100000);					//set frequency to 100Hz
		for(i=1;i<=AO_CHAN_MAX_NUM;i++)								//reset pwm initial state to output 0
		{
				pwm_set_ao(i,0);
		}
}

/**
  * @brief  PWM timer parameter configuration
  * @param  none
	* @param  none
  * @retval none
  */
static void drv_pwm_conf(uint32_t freq)
{	 
	uint16_t PrescalerValue = 0;
	uint16_t ccr_value = 500;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* -----------------------------------------------------------------------
    TIM8 Configuration: generate 4 PWM signals with 4 different duty cycles:
    The TIM8CLK frequency is set to SystemCoreClock (Hz), to get TIM8 counter
    clock at 24 MHz the Prescaler is computed as following:
     - Prescaler = (TIM8CLK / TIM8 counter clock) - 1
    SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
    and Connectivity line devices and to 24 MHz for Low-Density Value line and
    Medium-Density Value line devices

    The TIM8 is running at 36 KHz: TIM8 Frequency = TIM8 counter clock/(ARR + 1)
                                                  = 24 MHz / 666 = 36 KHz
    TIM8 Channel1 duty cycle = (TIM8_CCR1/ TIM8_ARR)* 100 = 50%
    TIM8 Channel2 duty cycle = (TIM8_CCR2/ TIM8_ARR)* 100 = 37.5%
    TIM8 Channel3 duty cycle = (TIM8_CCR3/ TIM8_ARR)* 100 = 25%
    TIM8 Channel4 duty cycle = (TIM8_CCR4/ TIM8_ARR)* 100 = 12.5%
  ----------------------------------------------------------------------- */
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / freq) - 1;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 999;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
  TIM_OCInitStructure.TIM_Pulse = ccr_value;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;		
	

  TIM_OC4Init(TIM8, &TIM_OCInitStructure);

//   TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIM8, ENABLE);

  /* TIM8 enable counter */
  TIM_Cmd(TIM8, ENABLE);
	TIM_CtrlPWMOutputs(TIM8, ENABLE);
}


/**
  * @brief  PWM GPIO and timer RCC	 configuration
  * @param  none
	* @param  none
  * @retval none
  */
static void pwm_rcc_conf(void)
{
  /* TIM8 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8 , ENABLE);
	
  /* GPIOC and GPIOD clock enable */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO , ENABLE);
}

/**
  * @brief PWM GPIO configuration.
  * @param  None
  * @retval None
  */
static void pwm_gpio_conf(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}




/*****************************************PWM Pluse****************************************/

static void CNT_Pluse_Rcc_Conf(void)
{
  /* TIM4 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
  /* GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO , ENABLE);
}

static void CNT_Pluse_Gpio_Conf(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
}

static void CNT_Pluse_Time_Conf(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_DeInit(TIM1);
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period=60000;
	TIM_TimeBaseStructure.TIM_Prescaler=0x00;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	TIM_ETRClockMode2Config(TIM1,TIM_ExtTRGPSC_OFF,TIM_ExtTRGPolarity_NonInverted,2);//ExtTRGFilter:Íâ²¿´¥·¢ÂË²¨Æ÷,·¶Î§0-0xF,Ô¤¶¨10
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ClearITPendingBit(TIM1,TIM_IT_Update); 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
	TIM_SetCounter(TIM1,0);
	TIM_Cmd(TIM1,ENABLE);
	
}
void Drv_CNT_Pluse_Init(void)
{
		CNT_Pluse_Rcc_Conf();
		CNT_Pluse_Gpio_Conf();
		CNT_Pluse_Time_Conf();	
}

uint16_t Read_Pluse_Cnt(void)
{
		uint16_t u16Cnt;
	
		u16Cnt=TIM_GetCounter(TIM1);
		return u16Cnt;
}

void Clear_Pluse_Cnt(uint16_t *u16Cnt)
{
	TIM_ClearITPendingBit(TIM1,TIM_IT_Update); 
	TIM_SetCounter(TIM1,0);
	*u16Cnt=0;
	return;
}

void TIM1_UP_IRQHandler(void)
{
		extern sys_reg_st		g_sys;
	 if(TIM_GetITStatus(TIM1,TIM_IT_Update)!=RESET)
//	 if(TIM_GetITStatus(TIM1,TIM_IT_Update)==SET) //
	 {
//			g_sys.status.ComSta.u16Pluse_CNT++;
			TIM_ClearITPendingBit(TIM1,TIM_IT_Update);	 
	 }
}


FINSH_FUNCTION_EXPORT(pwm_set_ao, reset pwm parameter.);
