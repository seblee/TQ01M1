#include <rtthread.h>
#include "i2c_bsp.h"

void syslog_thread_entry(void* parameter)
{
		rt_uint8_t 	i,temp;
		rt_uint8_t	i2c_wr_buf[256];
		rt_uint8_t	i2c_rd_buf[256]; 
		
	
		rt_thread_delay(2000);
		for(i=0;i<255;i++)
			i2c_wr_buf[i] = i;
// 		I2C_EE_BufWrite(i2c_wr_buf, EEP_Firstpage, 255);
// 		I2C_EE_BufRead(i2c_rd_buf, EEP_Firstpage , 255);
// 		for(i=0;i<255;i++)
// 			rt_kprintf(" %d ", i2c_rd_buf[i]);
		while(1)
		{
			//rt_kprintf("flash\n");
			rt_thread_delay(1000);
		};
}

