/**
 ****************************************************************************
 * @Warning :Without permission from the author,Not for commercial use
 * @File    :
 * @Author  :Seblee
 * @date    :2018-09-10 10:27:53
 * @version :V1.0.0
 *************************************************
 * @brief   :
 ****************************************************************************
 * @Last Modified by: Seblee
 * @Last Modified time: 2018-09-10 18:12:48
 ****************************************************************************
**/
#ifndef __AT_TRANSFER_H_
#define __AT_TRANSFER_H_
/* Private include -----------------------------------------------------------*/
#include "sys_conf.h"
#include <rtthread.h>
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
extern const char AT_WIFI_SYNC[];
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
rt_err_t at_wifi_send_message_ack_ok(rt_device_t dev, const char *AT_command);
rt_err_t at_wifi_get_cipstatus(rt_device_t dev);
rt_err_t at_wifi_connect_ssl(rt_device_t dev, char *host, int port);
/*----------------------------------------------------------------------------*/
#endif
