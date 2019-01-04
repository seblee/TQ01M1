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
 * @Last Modified time: 2018-11-21 15:56:38
 ****************************************************************************
**/
#ifndef __MQTT_CLIENT_H_
#define __MQTT_CLIENT_H_
/* Private include -----------------------------------------------------------*/
#include "network.h"
#include "MQTTPacket.h"
#include "sys_conf.h"
#include <rtthread.h>
/* Private typedef -----------------------------------------------------------*/

typedef enum
{
    MCode_PLATFORM_INIT = 1,   /*{"TOPIC_PLATFORM_INIT"}*/
    MCode_QRCODE_GENERATE,     /*{"TOPIC_WATER_NOTICE"}*/
    MCode_WATER_NOTICE,        /*{"TOPIC_WATER_NOTICE"}*/
    MCode_WATER_STATUS,        /*{"TOPIC_WATER_STATUS"}*/
    MCode_PARAMETER_SET,       /*{"TOPIC_PARAMETER_SET"}*/
    MCode_PARAMETER_GET,       /*{"TOPIC_PARAMETER_GET"}*/
    MCode_PARAMETER_PUT,       /*{"TOPIC_PARAMETER_PUT"}*/
    MCode_REALTIME_REPORT,     /*{"TOPIC_REALTIME_REPORT"}*/
    MCode_TIMING_REPORT,       /*{"TOPIC_TIMING_REPORT"}*/
    MCode_DEVICE_UPGRADE = 36, /*{"TOPIC_DEVICE_UPGRADE"}*/

} __mcode_value_t;

typedef enum
{
    WATER_NOTICE = 0, /*{"TOPIC_WATER_NOTICE"}*/
    PARAMETER_SET,    /*{"TOPIC_PARAMETER_SET"}*/
    PARAMETER_GET,    /*{"TOPIC_PARAMETER_GET"}*/
    OTA_UPGRADE,      /*{"IOT_OTA_UPGRADE"}*/
} _topic_sub_enmu_t;

typedef enum
{
    PLATFORM_INIT = 0, /*{"TOPIC_PLATFORM_INIT"}*/
    WATER_STATUS,      /*{"TOPIC_WATER_STATUS"}*/
    PARAMETER_PUT,     /*{"TOPIC_PARAMETER_PUT"}*/
    REALTIME_REPORT,   /*{"TOPIC_REALTIME_REPORT"}*/
    TIMING_REPORT,     /*{"TOPIC_TIMING_REPORT"}*/
    DEVICE_UPGRADE,    /*{"TOPIC_DEVICE_UPGRADE"}*/
    OTA_INFORM,        /*{"IOT_OTA_INFORM"}*/
    OTA_PROGRESS,      /*{"IOT_OTA_PROGRESS"}*/
} _topic_pub_enmu_t;

enum MQTT_QoS
{
    MQTT_QOS0,
    MQTT_QOS1,
    MQTT_QOS2,
    MQTT_SUBFAIL = 0x80
};
/* Private define ------------------------------------------------------------*/

#define DEVICE_ID "cdtest0041"
#define REGISTER_PRODUCT_KEY "a1JOOi3mNEf"
#define REGISTER_PRODUCT_SECRET "BQjNWOG8EJWa4nFu"
#define REGISTER_DEVICE_NAME "register_device_test_04"

#define REGISTER_HOST "iot-auth.cn-shanghai.aliyuncs.com"
#define REGISTER_PATH "/auth/register/device"
#define REGISTER_PORT 443

#define IOT_SID_FLAG 0xA5A5
#define IOT_SN_FLAG 0x0505
/*****seblee *********/

/***********TQ************/
// #define DEVICE_NAME "cdtest004"
// #define PRODUCT_KEY "a1JOOi3mNEf"
// #define DEVICE_SECRET "WjzDAlsux7gBMfF31M9CSZ9LKmutISPe"

/******topics**********/

#define TOPIC_PLATFORM_INIT "/%s/%s/pay/client2Cloud/init"                /*{"TOPIC_PLATFORM_INIT"}*/
#define TOPIC_WATER_NOTICE "/%s/%s/pay/service2Cloud/get"                 /*{"TOPIC_WATER_NOTICE"}*/
#define TOPIC_WATER_STATUS "/%s/%s/pay/client2Cloud/getWaterStatus"       /*{"TOPIC_WATER_STATUS"}*/
#define TOPIC_PARAMETER_SET "/%s/%s/monitor/service2Cloud/setting"        /*{"TOPIC_PARAMETER_SET"}*/
#define TOPIC_PARAMETER_GET "/%s/%s/monitor/service2Cloud/getSetting"     /*{"TOPIC_PARAMETER_GET"}*/
#define TOPIC_PARAMETER_PUT "/%s/%s/monitor/client2Cloud/putSetting"      /*{"TOPIC_PARAMETER_PUT"}*/
#define TOPIC_REALTIME_REPORT "/%s/%s/monitor/client2Cloud/putRealStatus" /*{"TOPIC_REALTIME_REPORT"}*/
#define TOPIC_TIMING_REPORT "/%s/%s/monitor/client2Cloud/putTimingStatus" /*{"TOPIC_TIMING_REPORT"}*/
#define TOPIC_DEVICE_UPGRADE "/%s/%s/monitor/service2Cloud/upgrade"       /*{"TOPIC_DEVICE_UPGRADE"}*/
#define TOPIC_DEVICE_MOVE "/%s/%s/move"                                   /*{"DEVICE_MOVE"}*/
#define TOPIC_DEVICE_UPDATE "/%s/%s/update"                               /*{"DEVICE_UPDATE"}*/
#define TOPIC_DEVICE_ERR "/%s/%s/update/error"                            /*{"DEVICE_ERR"}*/
#define TOPIC_DEVICE_GET "/%s/%s/get"                                     /*{"DEVICE_GET"}*/
#define IOT_OTA_UPGRADE "/ota/device/upgrade/%s/%s"                       /*{"IOT_OTA_UPGRADE"}*/
#define IOT_OTA_INFORM "/ota/device/inform/%s/%s"                         /*{"IOT_OTA_INFORM"}*/
#define IOT_OTA_PROGRESS "/ota/device/progress/%s/%s"                     /*{"IOT_OTA_PROGRESS"}*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
int mqtt_client_init(MQTTClient *client, iotx_device_info_pt device_info_p);

rt_err_t mqtt_setup_connect_info(iotx_conn_info_t *conn, iotx_device_info_t *device_info);

unsigned short mqtt_client_packet_id(void);

rt_err_t network_get_register(iotx_device_info_pt device_info_p);
/*----------------------------------------------------------------------------*/

#endif
