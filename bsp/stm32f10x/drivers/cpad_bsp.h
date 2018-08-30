#ifndef __CPAD_H__
#define __CPAD_H__
#include "stm32f10x.h"
#include "sys_def.h"

void cpad_dev_init(uint16_t baudrate);
void cpad_uart_send(uint8_t* tx_buf_ptr, uint8_t tx_cnt);
uint8_t cpad_get_rx_fsm(void);
uint16_t cpad_get_comm_sts(void);
void cpad_uart_init(uint16_t baudrate);
uint16_t cpad_frame_recv(void);
uint16_t cpad_frame_resolve(void);
uint16_t cpad_ob_resolve(void);
void cpad_debug(void);
#endif //__CPAD_H__
