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
 * @Last Modified time: 2018-09-26 17:54:23
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
    PLATFORM_INIT = 0, /*{"TOPIC_PLATFORM_INIT"}*/
    WATER_NOTICE,      /*{"TOPIC_WATER_NOTICE"}*/
    WATER_STATUS,      /*{"TOPIC_WATER_STATUS"}*/
    PARAMETER_SET,     /*{"TOPIC_PARAMETER_SET"}*/
    PARAMETER_GET,     /*{"TOPIC_PARAMETER_GET"}*/
    PARAMETER_PUT,     /*{"TOPIC_PARAMETER_PUT"}*/
    REALTIME_REPORT,   /*{"TOPIC_REALTIME_REPORT"}*/
    TIMING_REPORT,     /*{"TOPIC_TIMING_REPORT"}*/
    DEVICE_UPGRADE,    /*{"TOPIC_DEVICE_UPGRADE"}*/
    DEVICE_MOVE,       /*{"DEVICE_MOVE"}*/
    DEVICE_UPDATE,     /*{"DEVICE_UPDATE"}*/
    DEVICE_ERR,        /*{"DEVICE_ERR"}*/
    DEVICE_GET,        /*{"DEVICE_GET"}*/
} _topic_enmu_t;
enum QoS
{
    MQTT_QOS0,
    MQTT_QOS1,
    MQTT_QOS2,
    MQTT_SUBFAIL = 0x80
};
/* Private define ------------------------------------------------------------*/
#define DEVICE_ID "cdtest0041"

/*****seblee *********/
#define DEVICE_NAME "HelloWorld"
#define PRODUCT_KEY "rl0bGtKFCYA"
#define DEVICE_SECRET "gfp06h1QZxZXefWEEYweaMnsLxJU3lvp"
/***********TQ************/
// #define DEVICE_NAME "cdtest004"
// #define PRODUCT_KEY "a1JOOi3mNEf"
// #define DEVICE_SECRET "WjzDAlsux7gBMfF31M9CSZ9LKmutISPe"
/***highLevel***/
// #define DEVICE_NAME "TQ_Client01"
// #define PRODUCT_KEY "a12Ou4Hjw3M"
// #define DEVICE_SECRET "0ptX7AzSYlcMnVjFkVitKtJS20UoWcVB"

/******topics**********/
#define TOPIC_PLATFORM_INIT "/" PRODUCT_KEY "/" DEVICE_NAME "/pay/client2Cloud/init"                /*{"TOPIC_PLATFORM_INIT"}*/
#define TOPIC_WATER_NOTICE "/" PRODUCT_KEY "/" DEVICE_NAME "/pay/service2Cloud/get"                 /*{"TOPIC_WATER_NOTICE"}*/
#define TOPIC_WATER_STATUS "/" PRODUCT_KEY "/" DEVICE_NAME "/pay/client2Cloud/getWaterStatus"       /*{"TOPIC_WATER_STATUS"}*/
#define TOPIC_PARAMETER_SET "/" PRODUCT_KEY "/" DEVICE_NAME "/monitor/service2Cloud/setting"        /*{"TOPIC_PARAMETER_SET"}*/
#define TOPIC_PARAMETER_GET "/" PRODUCT_KEY "/" DEVICE_NAME "/monitor/service2Cloud/getSetting"     /*{"TOPIC_PARAMETER_GET"}*/
#define TOPIC_PARAMETER_PUT "/" PRODUCT_KEY "/" DEVICE_NAME "/monitor/client2Cloud/putSetting"      /*{"TOPIC_PARAMETER_PUT"}*/
#define TOPIC_REALTIME_REPORT "/" PRODUCT_KEY "/" DEVICE_NAME "/monitor/client2Cloud/putRealStatus" /*{"TOPIC_REALTIME_REPORT"}*/
#define TOPIC_TIMING_REPORT "/" PRODUCT_KEY "/" DEVICE_NAME "/monitor/client2Cloud/putTimingStatus" /*{"TOPIC_TIMING_REPORT"}*/
#define TOPIC_DEVICE_UPGRADE "/" PRODUCT_KEY "/" DEVICE_NAME "/monitor/service2Cloud/upgrade"       /*{"TOPIC_DEVICE_UPGRADE"}*/
#define TOPIC_DEVICE_MOVE "/" PRODUCT_KEY "/" DEVICE_NAME "/move"                                   /*{"DEVICE_MOVE"}*/
#define TOPIC_DEVICE_UPDATE "/" PRODUCT_KEY "/" DEVICE_NAME "/update"                               /*{"DEVICE_UPDATE"}*/
#define TOPIC_DEVICE_ERR "/" PRODUCT_KEY "/" DEVICE_NAME "/update/error"                            /*{"DEVICE_ERR"}*/
#define TOPIC_DEVICE_GET "/" PRODUCT_KEY "/" DEVICE_NAME "/get"                                     /*{"DEVICE_GET"}*/
/***highLevel***/
// #define TOPIC_PLATFORM_INIT "/" PRODUCT_KEY "/" DEVICE_NAME "/user/pay/client2Cloud/init"                /*{"TOPIC_PLATFORM_INIT"}*/
// #define TOPIC_WATER_NOTICE "/" PRODUCT_KEY "/" DEVICE_NAME "/user/pay/service2Cloud/get"                 /*{"TOPIC_WATER_NOTICE"}*/
// #define TOPIC_WATER_STATUS "/" PRODUCT_KEY "/" DEVICE_NAME "/user/pay/client2Cloud/getWaterStatus"       /*{"TOPIC_WATER_STATUS"}*/
// #define TOPIC_PARAMETER_SET "/" PRODUCT_KEY "/" DEVICE_NAME "/user/monitor/service2Cloud/setting"      /*{"TOPIC_PARAMETER_SET"}*/
// #define TOPIC_PARAMETER_GET "/" PRODUCT_KEY "/" DEVICE_NAME "/user/monitor/service2Cloud/getSetting"     /*{"TOPIC_PARAMETER_GET"}*/
// #define TOPIC_PARAMETER_PUT "/" PRODUCT_KEY "/" DEVICE_NAME "/user/monitor/client2Cloud/putSetting"      /*{"TOPIC_PARAMETER_PUT"}*/
// #define TOPIC_REALTIME_REPORT "/" PRODUCT_KEY "/" DEVICE_NAME "/user/monitor/client2Cloud/putRealStatus" /*{"TOPIC_REALTIME_REPORT"}*/
// #define TOPIC_TIMING_REPORT "/" PRODUCT_KEY "/" DEVICE_NAME "/user/monitor/client2Cloud/putTimingStatus" /*{"TOPIC_TIMING_REPORT"}*/
// #define TOPIC_DEVICE_UPGRADE "/" PRODUCT_KEY "/" DEVICE_NAME "/user/monitor/service2Cloud/upgrade"       /*{"TOPIC_DEVICE_UPGRADE"}*/
// #define TOPIC_DEVICE_MOVE "/" PRODUCT_KEY "/" DEVICE_NAME "/user/move"                                   /*{"DEVICE_MOVE"}*/
// #define TOPIC_DEVICE_UPDATE "/" PRODUCT_KEY "/" DEVICE_NAME "/user/update"                               /*{"DEVICE_UPDATE"}*/
// #define TOPIC_DEVICE_ERR "/" PRODUCT_KEY "/" DEVICE_NAME "/user/update/error"                            /*{"DEVICE_ERR"}*/
// #define TOPIC_DEVICE_GET "/" PRODUCT_KEY "/" DEVICE_NAME "/user/get"                                     /*{"DEVICE_GET"}*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
int mqtt_client_init(rt_device_t dev);

void mqtt_setup_connect_info(iotx_conn_info_t *conn, iotx_device_info_t *device_info);

int mqtt_client_connect(rt_device_t dev, MQTTPacket_connectData *conn);

rt_err_t mqtt_client_subscribe(_topic_enmu_t subsc, iotx_device_info_pt iotx_dev_info);

rt_err_t mqtt_client_subscribe_topics(void);

rt_err_t mqtt_packet_read_operation(void);

unsigned short mqtt_client_packet_id(void);

rt_err_t mqtt_client_ping(void);

rt_err_t mqtt_client_publish(char *topic, rt_uint8_t dup, int qos, rt_uint8_t restained, rt_uint8_t *msg, rt_uint16_t msg_len);

rt_err_t mqtt_client_publish_topics(void);

rt_err_t mqtt_client_publish_parameter(void);

rt_err_t mqtt_client_find_topic(char *topic);

rt_err_t mqtt_client_publish_report(_topic_enmu_t topic_type);

rt_err_t mqtt_client_receive_publish(const char *c, rt_uint16_t len);

rt_err_t mqtt_client_MQTTPuback(rt_uint8_t *c, rt_uint16_t len, unsigned int msgId, enum msgTypes type);

/*----------------------------------------------------------------------------*/

#endif
