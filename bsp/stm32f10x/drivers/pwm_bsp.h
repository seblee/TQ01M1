#ifndef __PWN_H__
#define __PWN_H__
#include "stdint.h"

void drv_pwm_init(void);
uint16_t pwm_set_ao(uint8_t channel, uint16_t ao_data);
extern void Drv_CNT_Pluse_Init(void);
extern uint16_t Read_Pluse_Cnt(void);
extern void Clear_Pluse_Cnt(uint16_t *u16Cnt);
#endif //__PWN_H__
