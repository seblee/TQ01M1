#include <rtthread.h>
#include "sys_conf.h"

static void fifo_init(void)
{
	fifo16_cb_td alarm_buf_cb;
	uint8 fifo16_init(&alarm_buf_cb, 8, 8);
}

log_