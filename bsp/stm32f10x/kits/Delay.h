#ifndef _DELAY_H
#define _DELAY_H
#include <stm32f10x.h>
void Delay_ms(unsigned long u32us);
void Delay_us(unsigned long u32us);
void Delay05us(void);

void Delay_sys(volatile unsigned long nTime);
void TimingDelay_Decrement(void);

extern void delay_init(void);
extern void delay_1us(uint32_t nus);
extern void delay_1ms(uint16_t nms);

#endif
