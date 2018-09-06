/**
 ****************************************************************************
 * @Warning :Without permission from the author,Not for commercial use
 * @File    :
 * @Author  :Seblee
 * @date    :2018-09-06 14:44:59
 * @version :V 1.0.0
 *************************************************
 * @brief   :
 ****************************************************************************
 * @Last Modified by: Seblee
 * @Last Modified time: 2018-09-06 15:38:16
 ****************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "sys_conf.h"
#include <rtthread.h>
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

rt_size_t transport_sendPacketBuffer(rt_device_t dev, unsigned char *buf, int buflen)
{
	rt_size_t rc;
	rc = rt_device_write(dev, 0, buf, buflen);
	return rc;
}

int transport_getdata(unsigned char *buf, int count)
{
//	memcpy(buf, (void *)&uart_rx_buffer[read_buf_len], count); //(void*)强转why?
//	read_buf_len += count;
//	return count;
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
int transport_open(char *addr, int port)
{
	int rc = 0;

	return rc;
}

int transport_close(void)
{
	int rc = 0;

	return rc;
}
