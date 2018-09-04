#include <rtthread.h>
#include "sys_conf.h"
#include "stm32f10x_can.h"
#include "can_bsp.h"
#include "kits/fifo.h"

#include "event_record.h"

void sim7600_thread_entry(void *parameter)
{
	rt_thread_delay(SIM7600_THREAD_DELAY);

	while (1)
	{
		rt_thread_delay(100);
	}
}
