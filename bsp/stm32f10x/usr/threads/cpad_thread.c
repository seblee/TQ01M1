#include <rtthread.h>
#include "sys_conf.h"
#include "stm32f10x_can.h"
#include "can_bsp.h"
#include "kits/fifo.h"
#include "cpad_bsp.h"

#include "event_record.h"

#define CPAD_BAUDRATE 19200

//void cpad_thread_entry(void* parameter)
//{
//		rt_thread_delay(CPAD_THREAD_DELAY);
//		cpad_dev_init(CPAD_BAUDRATE);
//		while(1)
//		{
//				if(cpad_frame_recv() == 1)
//				{
//						cpad_frame_resolve();	
//				}				
//				rt_thread_delay(100);
//		}
//}
//ÊÖ³­Æ÷ÃüÁî²Ù×÷
void cpad_thread_entry(void* parameter)
{
		rt_thread_delay(CPAD_THREAD_DELAY);
		while(1)
		{
				cpad_ob_resolve();
				rt_thread_delay(100);
		}
}

