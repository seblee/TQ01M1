#ifndef __ADC_H__
#define __ADC_H__
#include "stm32f10x.h"
#include "sys_conf.h"
#include "sys_def.h"

#define MAX_ADBUFEVERY		20

uint16_t drv_adc_dma_init(void);
void ai_sts_update(sys_reg_st*	gds_sys_ptr);

#endif //__ADC_H__
