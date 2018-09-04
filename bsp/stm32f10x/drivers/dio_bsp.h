#ifndef __DIO_H__
#define __DIO_H__
#include "stm32f10x.h"
#include "sys_conf.h"

enum
{
		DI_SOURCE_DOWN_BPOS=0,
		DI_SOURCE_MIDDLE_BPOS,
	  DI_SOURCE_UP_BPOS,	
		DI_DRINK_DOWN_BPOS,					
		DI_DRINK_MIDDLE_BPOS,						
		DI_DRINK_UP_BPOS,	
		DI_DRINK_MD_BPOS,				
		DI_HI_PRESS1_BPOS,							
		DI_HI_PRESS2_BPOS,				
		DI_Cold_1_BPOS,					
		DI_Heat_BPOS,	
		DI_Child_BPOS,					
		DI_OPEN_BPOS,		
		DI_Cold_2_BPOS,
		DI_FAN01_OD_BPOS,						
		DI_RESERVE_01,
		DI_RESERVE_02,
		DI_RESERVE_03,
		DI_POWER_LOSS_BPOS,//电源掉电
		DI_RESERVE_04,					
		DI_RESERVE_05,
		DI_RESERVE_06,
		DI_RESERVE_07,
		DI_MAX_BPOS,	
};

#define SLE1_PIN      GPIO_Pin_4
#define SLE2_PIN      GPIO_Pin_5
#define SLE_GPIO		  GPIOE

#define SLE1_READ     GPIO_ReadInputDataBit( SLE_GPIO, SLE1_PIN)
#define SLE2_READ     GPIO_ReadInputDataBit( SLE_GPIO, SLE2_PIN)

enum
{
		Com_Pad  	 =0x01,
		Start_Init =0x02,//上电初始化
};

#define DIN_WORD2  16	//2个字节
#define DO_WORD2   16	//2个字节

#define DO_MASK1   0xFFFF	//DO1 mask
#define DO_MASK2   0x3F	//DO2 mask
#define DO_POWER_CTR_ONLY   0x20	//只开电源

void drv_dio_init(void);
void di_sts_update(sys_reg_st*	gds_sys_ptr);
void dio_set_do(uint16_t channel_id, BitAction data);
//void slow_pwm_set(uint8_t channel, uint16_t dutycycle);
void led_toggle(void);
uint8_t GetSEL(void);

#endif //__DIO_H__
