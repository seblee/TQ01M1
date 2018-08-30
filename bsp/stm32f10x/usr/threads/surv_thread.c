#include <rtthread.h>
#include "sys_conf.h"

void surv_thread_entry(void* parameter)
{
		rt_thread_delay(SURV_THREAD_DELAY);
	while(1)
	{
		rt_thread_delay(1000);
	}
}

