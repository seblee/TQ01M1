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
 * @Last Modified time: 2018-10-09 13:14:32
 ****************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "sys_conf.h"
#include <rtthread.h>
#include "network.h"
#include "at_transfer.h"
#include "SIMCOM_GPRS.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#ifndef transport_log
#define transport_log(N, ...) rt_kprintf("####[transport %s:%4d] " N "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
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
    return network_read_message(write_device, buf, count, 1000);
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
int transport_open(rt_device_t dev, iotx_conn_info_t *conn)
{
    rt_err_t err = RT_EOK;

    if (conn->style == IOT_WIFI_MODE)
    {
        /*******connect tcp/ssl************/
        err = at_wifi_CIPSTART(dev, REMOTE_TCP, conn->host_name, conn->port);
        transport_log("connect_ssl err:%d", err);
        if (err == RT_EOK)
        {
            err = at_wifi_set_CIPMODE_mode(dev, 1);
            transport_log("set_CIPMODE err:%d", err);
        }
        if (err == RT_EOK)
        {
            /******start data transfer*********/
            err = at_wifi_send_message_ack(dev, AT_WIFI_CIPSEND, "OK");
            transport_log("CIPSEND err:%d", err);
        }
    }
    else if (conn->style == IOT_4G_MODE)
    {
        // SIMCOM_CCH(&g_SIMCOM_Handle ,conn->host_name, conn->port);

        SIMCOM_CIPNETWORK(&g_SIMCOM_Handle, TRUE, conn->host_name, conn->port);
    }
    return err;
}

int transport_close(rt_device_t dev)
{
    int rc;
    sprintf((char *)write_buffer, "+++\0");
    rc = rt_device_write(dev, 0, write_buffer, 3);
    rt_thread_delay(1000);
    return rc;
}
