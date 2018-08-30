////ÁîµÂ≠êËÜ®ËÉÄÈòÄÂ∫ïÂ±ÇÈ©±Âä®
//#include "Moter.h"

//EEV_Pack eevPack[MAX_EEVNUM];

///***********¿¯¥≈ ˝–Ú±Ì°®***********/ //Àƒœ‡∞À≈ƒA-AB-B-BC-C-CD-D-DA
//uint16_t eev1_table[stepMAX] = {0x0800,0x0840,0x0040,0x0060,0x0020,0x0120,0x0100,0x0900};		//È°∫Êó∂Âä±Á£ÅÊòØÂºÄÈòÄÔºåÂèç‰πãÊòØÂÖ≥ÈòÄ	
////uint16_t eev2_table[stepMAX] = {0x0040,0x0060,0x0020,0x0030,0x0010,0x0018,0x0008,0x0048};

//static void eevHWcfgSt_Init(void)
//{
//	uint8_t i = 0;
//	eevPack[EEV1].eevHardWareCfg.eevRcc = RCC_EEV1;				//EEV1œ‡πÿ“˝Ω≈≥ı ºªØ
//	eevPack[EEV1].eevHardWareCfg.eevPort_A = GPIO_PORT_EEV1_A;
//	eevPack[EEV1].eevHardWareCfg.eevApin = GPIO_PIN_EEV1_A;
//	eevPack[EEV1].eevHardWareCfg.eevPort_B = GPIO_PORT_EEV1_B;
//	eevPack[EEV1].eevHardWareCfg.eevBpin = GPIO_PIN_EEV1_B;
//	eevPack[EEV1].eevHardWareCfg.eevPort_C = GPIO_PORT_EEV1_C;
//	eevPack[EEV1].eevHardWareCfg.eevCpin = GPIO_PIN_EEV1_C;
//	eevPack[EEV1].eevHardWareCfg.eevPort_D = GPIO_PORT_EEV1_D;
//	eevPack[EEV1].eevHardWareCfg.eevDpin = GPIO_PIN_EEV1_D;
//	eevPack[EEV1].eevHardWareCfg.eev_maskbit = EEV1_MASKBIT;
//	
////	eevPack[EEV2].eevHardWareCfg.eevRcc = RCC_EEV2;				//EEV2œ‡πÿ“˝Ω≈≥ı ºªØ
////	eevPack[EEV2].eevHardWareCfg.eevPort = GPIO_PORT_EEV2;
////	eevPack[EEV2].eevHardWareCfg.eevApin = GPIO_PIN_EEV2_A;
////	eevPack[EEV2].eevHardWareCfg.eevBpin = GPIO_PIN_EEV2_B;
////	eevPack[EEV2].eevHardWareCfg.eevCpin = GPIO_PIN_EEV2_C;
////	eevPack[EEV2].eevHardWareCfg.eevDpin = GPIO_PIN_EEV2_D;
////	eevPack[EEV2].eevHardWareCfg.eev_maskbit = EEV2_MASKBIT;
//	
//	for(i = 0;i < stepMAX;i++)
//	{
//		eevPack[EEV1].eevHardWareCfg.eev_table[i] = eev1_table[i];
////		eevPack[EEV2].eevHardWareCfg.eev_table[i] = eev2_table[i];
//	}
//}

//static void eev_gpio_init(struct eevHWcfg *ptr)
//{
////	GPIO_InitTypeDef GPIO_InitStruct;

////	RCC_AHBPeriphClockCmd(ptr->eevRcc,ENABLE);
////	GPIO_InitStruct.GPIO_Pin = ptr->eevApin | ptr->eevBpin | ptr->eevCpin | ptr->eevDpin;
////    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
////	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
////	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
////    GPIO_Init(ptr->eevPort, &GPIO_InitStruct);
////	GPIO_ResetBits(ptr->eevPort, (ptr->eevApin | ptr->eevBpin | ptr->eevCpin | ptr->eevDpin));	
//		GPIO_InitTypeDef  GPIO_InitStructure; 
//		RCC_APB2PeriphClockCmd(ptr->eevRcc,ENABLE);	
//	
//		GPIO_InitStructure.GPIO_Pin =  ptr->eevApin;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//		GPIO_Init(ptr->eevPort_A, &GPIO_InitStructure);

//		GPIO_InitStructure.GPIO_Pin =  ptr->eevBpin;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//		GPIO_Init(ptr->eevPort_B, &GPIO_InitStructure);

//		GPIO_InitStructure.GPIO_Pin =  ptr->eevCpin;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//		GPIO_Init(ptr->eevPort_C, &GPIO_InitStructure);
//		
//		GPIO_InitStructure.GPIO_Pin =  ptr->eevApin;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//		GPIO_Init(ptr->eevPort_D, &GPIO_InitStructure);

//	  GPIO_ResetBits(ptr->eevPort_A, ptr->eevApin);	
//	  GPIO_ResetBits(ptr->eevPort_B, ptr->eevBpin);	
//	  GPIO_ResetBits(ptr->eevPort_C, ptr->eevCpin);	
//	  GPIO_ResetBits(ptr->eevPort_D, ptr->eevDpin);			
//}

//static void eevMovInfoSt_Init(uint8_t index)
//{
//	eevPack[index].eevMoveInfo.last_dir = DIRNULL;
//	eevPack[index].eevMoveInfo.now_dir = DIRNULL;
//	eevPack[index].eevMoveInfo.last_status = STOP;
//	eevPack[index].eevMoveInfo.now_status= STOP;
//	eevPack[index].eevMoveInfo.movPluse = 0;
//	eevPack[index].eevMoveInfo.curPluse = 0;
//	eevPack[index].eevMoveInfo.beatIndex = stepA;		//Âä±Á£ÅAÁõ∏ÁöÑ‰∏ä‰∏ÄÈ°π
//}
////Êú¨ÂáΩÊï∞Âú®ËÆæÁΩÆÂêØÂä®ÊàñÂèçÂêëÊó∂ÔºåÂè™ÈúÄË¶ÅËÆæÁΩÆ1Ê¨°

//uint8_t setEevMovInfo(uint16_t dir,uint16_t steps,uint16_t status,uint8_t index)
//{
//	extern sys_reg_st g_sys;
//	if((status == RUN) && (steps != 0) && (index < MAX_EEVNUM) && (dir != DIRNULL))
//	{
//		if((steps <= g_sys.config.Moter.excAllOpenSteps)&&(eevPack[index].eevMoveInfo.movPluse<=1))			//¥Û”⁄≤Ω ˝ªÚ√¶
//		{
//			eevPack[index].eevMoveInfo.now_status= status;
//			eevPack[index].eevMoveInfo.now_dir = dir;
//			eevPack[index].eevMoveInfo.movPluse = steps;
//			eevPack[index].eevMoveInfo.curPluse = eevPack[index].eevMoveInfo.finalPluse;
//		}
//		else
//		{
//				return FALSE;
//		}
//	}
//	else         
//	{
//		eevPack[index].eevMoveInfo.now_status= STOP;
//		eevPack[index].eevMoveInfo.now_dir = DIRNULL;
//		eevPack[index].eevMoveInfo.movPluse = 0;
//	}
//	return TRUE;	
//}
////EEV∂Ø◊˜ «∑ÒÕÍ≥…
//uint8_t GetEevMovInfo(uint8_t index)
//{
//	if(eevPack[index].eevMoveInfo.movPluse<1)
//	{
//			return FALSE;			
//	}
//	return TRUE;	
//}

//void eev_init(void)
//{
//	uint8_t i;
//	eevHWcfgSt_Init();
//	for(i = EEV1;i < MAX_EEVNUM;i++)
//	{
//		eevMovInfoSt_Init(i);
//		eev_gpio_init(&eevPack[i].eevHardWareCfg);
//	}
//}

//void eev_run(EEV_Pack * eevPtr)
//{
//	extern sys_reg_st	g_sys;
//	static uint8_t stepIndex = 0;
//	static uint8_t StepReal = 0;
//	uint16_t porTmp = 0;
//	uint8_t Step_Temp = STEP_OFFSET;
//	
//	if(eevPtr->eevMoveInfo.last_status != eevPtr->eevMoveInfo.now_status)		
//	{
//		eevPtr->eevMoveInfo.last_status = eevPtr->eevMoveInfo.now_status;
//		eevPtr->eevMoveInfo.exc_holdCnt = MAX_EXCHOLD_CNT;
//	}
//	else
//	{
//		if((eevPtr->eevMoveInfo.last_dir != eevPtr->eevMoveInfo.now_dir)\
//			&&(eevPtr->eevMoveInfo.last_dir != DIRNULL) && (eevPtr->eevMoveInfo.now_dir != DIRNULL)) //ÂèçÂêëËøêÂä®
//		{
//			eevPtr->eevMoveInfo.last_dir = eevPtr->eevMoveInfo.now_dir;
//			eevPtr->eevMoveInfo.exc_holdCnt = MAX_EXCHOLD_CNT;
//		}
//	}
//	if(eevPtr->eevMoveInfo.exc_holdCnt != 0)						//Âú®ÁªìÊùüÁõ∏„ÄÅÂÅúÊ≠¢Áõ∏„ÄÅÊäòËøîÁõ∏ÂùáË¶ÅËøõË°åÂä±Á£Å‰øùÊåÅÔºå‰ª•ÂÖçÂ§±Ê≠•
//	{		
//		eevPtr->eevMoveInfo.exc_holdCnt--;
//		porTmp = GPIO_ReadOutputData(eevPtr->eevHardWareCfg.eevPort);
//		porTmp &= eevPtr->eevHardWareCfg.eev_maskbit;
//		porTmp |= eevPtr->eevHardWareCfg.eev_table[eevPtr->eevMoveInfo.beatIndex];
//		GPIO_Write(eevPtr->eevHardWareCfg.eevPort,porTmp);
//		eevPtr->eevMoveInfo.curPluse = eevPtr->eevMoveInfo.finalPluse;
//		stepIndex = eevPtr->eevMoveInfo.beatIndex;
//	}
//	else
//	{
//		if(eevPtr->eevMoveInfo.now_status == STOP)		
//		{
//			porTmp = GPIO_ReadOutputData(eevPtr->eevHardWareCfg.eevPort);
//			porTmp &= eevPtr->eevHardWareCfg.eev_maskbit;
//			GPIO_Write(eevPtr->eevHardWareCfg.eevPort,porTmp);
//			eevPtr->eevMoveInfo.last_status = STOP;
//			eevPtr->eevMoveInfo.now_dir = DIRNULL;
//			eevPtr->eevMoveInfo.last_dir= DIRNULL;
//			eevPtr->eevMoveInfo.movPluse = 0;
//			eevPtr->eevMoveInfo.finalPluse = eevPtr->eevMoveInfo.curPluse;
//		}
//		else
//		if(eevPtr->eevMoveInfo.now_status == RUN)     
//		{
//			eevPtr->eevMoveInfo.last_status = eevPtr->eevMoveInfo.now_status;
//			eevPtr->eevMoveInfo.last_dir = eevPtr->eevMoveInfo.now_dir;
////			if(eevPtr->eevMoveInfo.movPluse != 0)
////			{
////				eevPtr->eevMoveInfo.movPluse--;
////				if(eevPtr->eevMoveInfo.now_dir == DIROPEN)       //ÈòÄÂ¢ûÂä†ÁöÑÊñπÂêë
////				{
////					eevPtr->eevMoveInfo.curPluse++;
////					if(eevPtr->eevMoveInfo.curPluse >= g_sys.config.general.excAllOpenSteps)
////					{
////						eevPtr->eevMoveInfo.curPluse = g_sys.config.general.excAllOpenSteps;
////					}
////					stepIndex++;
////					stepIndex %= (stepMAX);
////				}
////				else
////				if(eevPtr->eevMoveInfo.now_dir == DIRCLOSE)      //ÈòÄÂáèÂ∞èÁöÑÊñπÂêë
////				{
////					if(eevPtr->eevMoveInfo.curPluse != 0)
////					{
////						eevPtr->eevMoveInfo.curPluse--;
////					}
////					if(stepIndex == stepA)
////					{
////						stepIndex = stepDA;
////					}
////					else
////					{ 
////						stepIndex--;
////					}		
////				}
////				if(stepIndex < stepMAX)
////				{
////					porTmp = GPIO_ReadOutputData(eevPtr->eevHardWareCfg.eevPort);
////					porTmp &= eevPtr->eevHardWareCfg.eev_maskbit;
////					porTmp |= eevPtr->eevHardWareCfg.eev_table[stepIndex];
////					GPIO_Write(eevPtr->eevHardWareCfg.eevPort,porTmp);   
////				}
////				else
////				{
////					stepIndex = stepA;
////				}
////				eevPtr->eevMoveInfo.beatIndex = stepIndex; 		
////			}
////			else
////			{
////				eevPtr->eevMoveInfo.now_status = STOP;
////				eevPtr->eevMoveInfo.finalPluse = eevPtr->eevMoveInfo.curPluse;		
////			}
////			g_sys.status.Test_Buff[6]=eevPtr->eevMoveInfo.movPluse;		

//			if(eevPtr->eevMoveInfo.movPluse >0)
//			{
//				if(eevPtr->eevMoveInfo.movPluse==1)
//				{
//					Step_Temp=1;						
//				}
//				else
//				{
//					Step_Temp=STEP_OFFSET;	
//				}
//				
//				eevPtr->eevMoveInfo.movPluse-=Step_Temp;
//				if(eevPtr->eevMoveInfo.now_dir == DIROPEN)       //ÈòÄÂ¢ûÂä†ÁöÑÊñπÂêë
//				{
//					eevPtr->eevMoveInfo.curPluse+=Step_Temp;
//					if(eevPtr->eevMoveInfo.curPluse >= g_sys.config.Moter.excAllOpenSteps)
//					{
//						eevPtr->eevMoveInfo.curPluse = g_sys.config.Moter.excAllOpenSteps;
//					}
//					stepIndex++;
//					stepIndex %= (stepMAX);
//				}
//				else
//				if(eevPtr->eevMoveInfo.now_dir == DIRCLOSE)      //ÈòÄÂáèÂ∞èÁöÑÊñπÂêë
//				{
//					if(eevPtr->eevMoveInfo.curPluse > 0)
//					{
//						eevPtr->eevMoveInfo.curPluse-=Step_Temp;
//					}
//					else
//					{
//						eevPtr->eevMoveInfo.curPluse=0;						
//					}
//					if(stepIndex == stepA)
//					{
//						stepIndex = stepDA;
//					}
//					else
//					{ 
//						stepIndex--;
//					}		
//				}
//				if(stepIndex < stepMAX)
//				{
//					porTmp = GPIO_ReadOutputData(eevPtr->eevHardWareCfg.eevPort);
//					porTmp &= eevPtr->eevHardWareCfg.eev_maskbit;
//					porTmp |= eevPtr->eevHardWareCfg.eev_table[stepIndex];
//					GPIO_Write(eevPtr->eevHardWareCfg.eevPort,porTmp);   
//				}
//				else
//				{
//					stepIndex = stepA;
//				}
//				eevPtr->eevMoveInfo.beatIndex = stepIndex; 		
//			}
//			else
//			{
//				eevPtr->eevMoveInfo.now_status = STOP;
//				eevPtr->eevMoveInfo.finalPluse = eevPtr->eevMoveInfo.curPluse;		
//			}
////				eevPtr->eevMoveInfo.finalPluse = eevPtr->eevMoveInfo.curPluse;		
////			g_sys.status.Test_Buff[6]=eevPtr->eevMoveInfo.movPluse;		
//		}
//	}
//}

//uint16_t getBeatIndex(uint8_t index)							//ªÒ»°µ±«∞À˘¥¶µƒ≈ƒ ˝(0-7)
//{
//	extern sys_reg_st	g_sys;
//	uint16_t ret = ABNORMAL_VALUE;
//	if(index < MAX_EEVNUM)
//	{
//		if((g_sys.config.dev_mask.eev >> index) & 0x0001)
//		{
//			ret = eevPack[index].eevMoveInfo.beatIndex;
//		}
//	}
//	return ret;								//@ g_sys.status.valve.valve_phase
//}

//uint16_t getFinalPluse(uint8_t index)						//ªÒ»°µ±«∞À˘‘⁄µƒ¬ˆ≥Â ˝
//{
//	extern sys_reg_st	g_sys;
//	uint16_t ret = ABNORMAL_VALUE;
//	if(index < MAX_EEVNUM)
//	{
//		if((g_sys.config.dev_mask.eev >> index) & 0x0001)
//		{
//			if(eevPack[index].eevMoveInfo.finalPluse >= g_sys.config.general.excAllOpenSteps)
//			{
//				eevPack[index].eevMoveInfo.finalPluse = g_sys.config.general.excAllOpenSteps;
//			}
//			ret = eevPack[index].eevMoveInfo.finalPluse;
//		}
//	}
//	return ret;								//@ g_sys.status.valve.valve_steps_cur
//}

//uint16_t getOpenValveDegree(uint8_t index)						//ªÒ»°µ±«∞µƒ∑ßø™∂»
//{
//	extern sys_reg_st	g_sys;
//	uint16_t ret = ABNORMAL_VALUE;
//	if(index < MAX_EEVNUM)
//	{
//		if((g_sys.config.dev_mask.eev >> index) & 0x0001)
//		{
//			if(eevPack[index].eevMoveInfo.finalPluse != ABNORMAL_VALUE)
//			{
//				ret = eevPack[index].eevMoveInfo.finalPluse * 100/ g_sys.config.general.excAllOpenSteps;		//¿©¥Û100 ±∂
//			}
//		}		
//	}	
//	return ret;								//@g_sys.status.valve.valve_opening_cur
//}

//uint16_t getPluseNumofOpenValve(uint16_t valve)
//{
//	extern sys_reg_st	g_sys;
//	uint16_t pulseTemp;
//	pulseTemp = (valve * g_sys.config.general.excAllOpenSteps) / 100;
//	return pulseTemp;
//}

//void eev1_timerInit(uint8_t ppsVal)
//{
//	uint16_t timPeriod = 0;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	/* TIM14 clock enable */
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);

////	timPeriod = 1000000/ ppsVal - 1;
//	timPeriod = 1000000/ ppsVal ;
//	/* Time base configuration */
//	TIM_TimeBaseStructure.TIM_Period = timPeriod - 1;
//	TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1;
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInit(EEV1_TIMER, &TIM_TimeBaseStructure);
//	
//	TIM_ClearITPendingBit(EEV1_TIMER, TIM_IT_Update);
//	TIM_ITConfig(EEV1_TIMER, TIM_IT_Update, ENABLE);
//	TIM_Cmd(EEV1_TIMER, ENABLE);
//	
//	NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//}

////void eev2_timerInit(uint8_t ppsVal)
////{
////	uint16_t timPeriod = 0;
////	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
////	NVIC_InitTypeDef NVIC_InitStructure;
////	
////	/* TIM15 clock enable */
////	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);

//////	timPeriod = 1000000/ ppsVal - 1;
////	timPeriod = 1000000/ ppsVal ;
////	/* Time base configuration */
////	TIM_TimeBaseStructure.TIM_Period = timPeriod - 1;
////	TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1;
////	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
////	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
////	TIM_TimeBaseInit(EEV2_TIMER, &TIM_TimeBaseStructure);
////	
////	TIM_ClearITPendingBit(EEV2_TIMER, TIM_IT_Update);
////	TIM_ITConfig(EEV2_TIMER, TIM_IT_Update, ENABLE);
////	TIM_Cmd(EEV2_TIMER, ENABLE);
////	
////	NVIC_InitStructure.NVIC_IRQChannel = TIM15_IRQn;
////	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
////	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
////	NVIC_Init(&NVIC_InitStructure);
////}

//void EEV1_IRQHandler(void)
//{
//	extern sys_reg_st	g_sys;
//	if(TIM_GetITStatus(EEV1_TIMER,TIM_IT_Update) != RESET)
//	{
//		if((g_sys.config.dev_mask.eev & (0x01 << EEV1)) != 0)	
//		{
//			eev_run(&eevPack[EEV1]);
//		}
//		TIM_ClearITPendingBit(EEV1_TIMER,TIM_IT_Update);
//	}
//}

////void EEV2_IRQHandler(void)
////{
////	extern sys_reg_st	g_sys;
////	if(TIM_GetITStatus(EEV2_TIMER,TIM_IT_Update) != RESET)
////	{
////		if((g_sys.config.dev_mask.eev & (0x01 << EEV2)) != 0)	
////		{
////			eev_run(&eevPack[EEV2]);
////		}
////		TIM_ClearITPendingBit(EEV2_TIMER,TIM_IT_Update);
////	}
////}

