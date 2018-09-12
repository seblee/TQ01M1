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
 * @Last Modified time: 2018-09-11 18:09:06
 ****************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "sys_conf.h"
#include <rtthread.h>
#include "sim7600.h"
#include "at_transfer.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

int transport_sendPacketBuffer(int sock, unsigned char *buf, int buflen)
{
	return rt_device_write(write_device, 0, buf, buflen);
}

int transport_getdata(unsigned char *buf, int count)
{
	/**1 second timeout**/
	return sim7600_read_message(write_device, buf, count, 1000);
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
int transport_open(rt_device_t dev, char *addr, int port)
{
	rt_err_t err = RT_EOK;
	/*******connect tcp/ssl************/
	err = at_wifi_connect_ssl(dev, addr, port);
	if (err == RT_EOK)
		err = at_wifi_set_CIPMODE_mode(dev);
	if (err == RT_EOK)
		err = at_wifi_set_CIPMODE_mode(dev);
}

int transport_close(rt_device_t dev)
{
	int rc = 0;

	return rc;
}
