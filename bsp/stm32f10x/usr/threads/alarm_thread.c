#include <rtthread.h>
#include "sys_conf.h"


void alarm_thread_entry(void* parameter)
{
	while(1)
	{
		rt_thread_delay(1000);	
	}
}
