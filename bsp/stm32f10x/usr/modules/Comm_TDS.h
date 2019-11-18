#ifndef __COMM_TDS_H
#define __COMM_TDS_H


#include <rtthread.h>

#define TDS_HEAD	0xFF
#define TDS_01		0x01
#define TDS_02		0x02
#define TDS_03		0x03
#define TDS_04		0x04
#define TDS_END		0xEE

void TDS_thread_entry(void *parameter);

#endif
