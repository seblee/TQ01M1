#include <rtthread.h>
#include <mb.h>

/**
  * @brief  modbus slave poll thread
  * @param  none
  * @retval none
**/
//void modbus_slave_thread_entry(void* parameter)
//{
//		eMBErrorCode    eStatus = MB_ENOERR;
//		eStatus = eMBInit(MB_RTU, 1, 1, 9600,  MB_PAR_NONE);
//		if(eStatus != MB_ENOERR)
//		{
//				rt_kprintf("MB_SLAVE init failed\n");
//		}
//		eStatus = eMBEnable();			
//		if(eStatus != MB_ENOERR)
//		{
//				rt_kprintf("MB_SLAVE enable failed\n");	
//		}
//		while(1)
//		{
//				eStatus = eMBPoll();
//				if(eStatus != MB_ENOERR)
//				{
//						rt_kprintf("MB_SLAVE enable failed\n");	
//				}					
//				rt_thread_delay(10);
//		}
//}

