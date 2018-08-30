#include <rtthread.h>
#include "mb.h"
#include "mb_m.h"


/**
  * @brief  modbus master poll thread
  * @param  none
  * @retval none
**/
//void modbus_master_thread_entry(void* parameter)
//{
//		eMBErrorCode    eStatus = MB_ENOERR;;
//		eStatus = eMBMasterInit(MB_RTU,2,4800,MB_PAR_NONE);
//		if(eStatus != MB_ENOERR)
//		{
//				rt_kprintf("MBM init fail\n");
//		}
//		eStatus = eMBMasterEnable();
//		if(eStatus != MB_ENOERR)
//		{
//				rt_kprintf("MBM enable fail\n");
//		}
//		while(1)
//		{ 
//				eStatus = eMBMasterPoll();	
//				if(eStatus != MB_ENOERR)
//				{
//						rt_kprintf("MBM poll err!\n");
//				}
//				rt_thread_delay(10);
//		}
//}
