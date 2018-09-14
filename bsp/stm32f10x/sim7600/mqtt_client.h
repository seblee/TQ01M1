/**
 ****************************************************************************
 * @Warning :Without permission from the author,Not for commercial use
 * @File    :
 * @Author  :Seblee
 * @date    :2018-09-11 11:11:19
 * @version :V1.0.0
 *************************************************
 * @brief   :
 ****************************************************************************
 * @Last Modified by: Seblee
 * @Last Modified time: 2018-09-11 16:14:43
 ****************************************************************************
**/
#ifndef __MQTT_CLIENT_H_
#define __MQTT_CLIENT_H_
/* Private include -----------------------------------------------------------*/
#include "sim7600.h"
#include "MQTTPacket.h"
#include "sys_conf.h"
#include <rtthread.h>
/* Private typedef -----------------------------------------------------------*/
typedef enum
{
    TQ_PLATFORM_INIT = 0,
    TQ_WATER_NOTICE,
} __mqtt_topic_t;
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
int mqtt_client_init(rt_device_t dev);
void mqtt_setup_connect_info(iotx_conn_info_t *conn, iotx_device_info_t *device_info);
int mqtt_client_connect(rt_device_t dev, MQTTPacket_connectData *conn);
rt_err_t mqtt_client_subscription(__mqtt_topic_t topic, iotx_device_info_pt iotx_dev_info);
/*----------------------------------------------------------------------------*/

#endif
