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
 * @Last Modified time: 2018-09-10 18:20:36
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

rt_size_t transport_sendPacketBuffer(rt_device_t dev, unsigned char *buf, int buflen)
{
	rt_size_t rc;
	rc = rt_device_write(dev, 0, buf, buflen);
	return rc;
}

int transport_getdata(unsigned char *buf, int count)
{
	//	memcpy(buf, (void *)&uart_rx_buffer[read_buf_len], count);
	//	read_buf_len += count;
	return count;
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
int transport_open(rt_device_t dev, char *addr, int port)
{
	int rc = 0;
	/*******wifi mode*********/
	//DIR_WIFI_MODE:
	SIM7600_DIR_WIFI;
	/****test AT************/
	if (at_wifi_send_message_ack_ok(dev, AT_WIFI_SYNC) == RT_EOK)
		goto DIR_4G_MODE;
	/*****check wifi state****************/
	if (at_wifi_get_cipstatus(dev) != 2)
	{ /**add wifi connect code**/
	}
	goto connect_tcp;
/******4G MODE***************/
DIR_4G_MODE:
	SIM7600_DIR_4G;
connect_tcp:
	/*******connect tcp/ssl************/
	if (at_wifi_connect_ssl(dev, addr, port) == RT_EOK)
		rc = RT_EOK;
	return rc;
}

int transport_close(rt_device_t dev)
{
	int rc = 0;

	return rc;
}
