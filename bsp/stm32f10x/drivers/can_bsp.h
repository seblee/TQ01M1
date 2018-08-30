#ifndef __CAN_H__
#define __CAN_H__
#include "stm32f10x.h"

#define	CANID_PRIO_BITS	0x18000000
#define	CANID_RES_BITS	0x07800000
#define	CANID_RAF_BITS	0x00400000
#define	CANID_SA_BITS		0x003FC000
#define	CANID_DA_BITS		0x00003FC0
#define	CANID_EOSF_BITS	0x00000020
#define	CANID_SNF_BITS	0x0000001F

void drv_can_init(void);
void can_port_baudrate(void);
int32_t can_send(uint8_t sa, uint8_t da, const uint8_t* data, uint16_t length, uint8_t prio, uint8_t raf);
int32_t can_send_singleframe(uint8_t sa, uint8_t da, uint8_t type, const uint8_t* data, uint16_t length, uint8_t prio, uint8_t raf);

#endif //__CAN_H__
