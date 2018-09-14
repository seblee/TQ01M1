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
#ifndef transport_log
#define transport_log(N, ...) rt_kprintf("####[transport %s:%4d] " N "\r\n", __FILE__, __LINE__, ##__VA_ARGS__);
#endif /* transport_log(...) */
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

int transport_sendPacketBuffer(int sock, unsigned char *buf, int buflen)
{
	if (buflen == 0)
		return 0;
	if (buf == RT_NULL)
		return -RT_EEMPTY;
	return rt_device_write(write_device, 0, buf, buflen);
}

int transport_getdata(unsigned char *buf, int count)
{
	/**1 second timeout**/
	return sim7600_read_message(write_device, buf, count, 3000);
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
	transport_log("connect_ssl err:%d", err);
	if (err == RT_EOK)
	{
		err = at_wifi_set_CIPMODE_mode(dev);
		transport_log("set_CIPMODE err:%d", err);
	}
	if (err == RT_EOK)
	{
		/******start data transfer*********/
		err = at_wifi_send_message_ack_ok(dev, AT_WIFI_CIPSEND);
		transport_log("CIPSEND err:%d", err);
	}
	return err;
}

int transport_close(rt_device_t dev)
{
	int rc = 0;
	sprintf((char *)write_buffer, "+++\0");
	return rt_device_write(dev, 0, write_buffer, 3);
}
