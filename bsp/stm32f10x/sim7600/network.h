/**
 ****************************************************************************
 * @Warning :Without permission from the author,Not for commercial use
 * @File    :
 * @Author  :Seblee
 * @date    :2018-09-10 10:16:41
 * @version :V1.0.0
 *************************************************
 * @brief   :
 ****************************************************************************
 * @Last Modified by: Seblee
 * @Last Modified time: 2018-09-26 17:53:59
 ****************************************************************************
**/
#ifndef __SIM7600_H_
#define __SIM7600_H_
/* Private include -----------------------------------------------------------*/
#include "sys_conf.h"
#include <rtthread.h>
#include "board.h"
#include "MQTTPacket.h"
/* Private define ------------------------------------------------------------*/
/* From device.h */
#define PRODUCT_KEY_LEN (20)
#define DEVICE_NAME_LEN (32)
#define DEVICE_ID_LEN (64)
#define DEVICE_SECRET_LEN (64)

#define MODULE_VENDOR_ID (32) /* Partner ID */

#define HOST_ADDRESS_LEN (128)
#define HOST_PORT_LEN (8)
#define CLIENT_ID_LEN (256)
#define USER_NAME_LEN (512) /* Extend length for ID2 */
#define PASSWORD_LEN (256)  /* Extend length for ID2 */
#define AESKEY_STR_LEN (32)
#define AESKEY_HEX_LEN (128 / 8)

#define GUIDER_SIGN_LEN (256)

#define MSG_LEN_MAX 1024

#define SIM7600_DIR_PORT UART3_DIR_GPIO
#define SIM7600_DIR_PIN UART3_DIR_GPIO_PIN

#define SIM7600_DIR_WIFI GPIO_SetBits(SIM7600_DIR_PORT, SIM7600_DIR_PIN)
#define SIM7600_DIR_4G GPIO_ResetBits(SIM7600_DIR_PORT, SIM7600_DIR_PIN)

#define aliyun_domain "%s.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define aliyun_iot_port 1883

#define cliend_id "%s|securemode=%d,signmethod=%s|"

#define SHA_METHOD "hmacsha1"
#define MD5_METHOD "hmacmd5"

#define SECURE_TLS 2
#define SECURE_TCP 3

#define TIMING_INTERVAL_DEFAULT 1800
#define TIMING_INTERVAL_MIN 60
#define TIMING_INTERVAL_MAX 60000

#define REALTIME_INTERVAL_DEFAULT 10
#define REALTIME_INTERVAL_MIN 5
#define REALTIME_INTERVAL_MAX 1000
/* Private typedef -----------------------------------------------------------*/
typedef struct
{
    char product_key[PRODUCT_KEY_LEN + 1];
    char device_name[DEVICE_NAME_LEN + 1];
    char device_id[DEVICE_ID_LEN + 1];
    char device_secret[DEVICE_SECRET_LEN + 1];
} iotx_device_info_t, *iotx_device_info_pt;

typedef enum
{
    IOT_WIFI_MODE = 0,
    IOT_4G_MODE,
} _iot_wireless_style_t;

typedef struct
{
    uint16_t port;
    char host_name[HOST_ADDRESS_LEN + 1];
    char client_id[CLIENT_ID_LEN + 1];
    char username[USER_NAME_LEN + 1];
    char password[PASSWORD_LEN + 1];
    _iot_wireless_style_t style;
} iotx_conn_info_t, *iotx_conn_info_pt;

typedef struct
{
    const char *topic_str;
    rt_uint8_t dup;
    int qos;
    rt_uint8_t restained;
} iot_topic_param_t;
typedef enum
{
    IOT_POWERON = 0,
    IOT_PLATFORM_INIT,
    IOT_INIT_COMPL,
    IOT_PARAM_REPORT,
    IOT_REALTIME_REPORT,
    IOT_TIMING_REPORT,
    IOT_DEVICE_UPGRADE,
    IOT_IDEL,
} _iot_state_t;
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
extern rt_device_t write_device;

extern rt_uint8_t write_buffer[MSG_LEN_MAX];

extern rt_uint8_t read_buffer[MSG_LEN_MAX];

extern const char iot_deviceid[];

extern const char iot_devicename[];

extern const char iot_productKey[];

extern const char iot_secret[];

extern rt_mq_t rx_mq;

extern iotx_device_info_t device_info;

extern iotx_conn_info_t device_connect;

extern MQTTPacket_connectData client_con;

extern iot_topic_param_t iot_topics[];

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
rt_uint32_t network_send_message(rt_device_t dev, const char *senddata, rt_uint8_t **data);

rt_int32_t network_read_message(rt_device_t dev, rt_uint8_t *data, rt_int16_t len, rt_int32_t timeout);

void network_Serialize_init_json(char **datapoint);

void network_Serialize_para_json(char **datapoint);

rt_err_t network_water_notice_parse(const char *Str);

void network_Serialize_report_json(char **datapoint, rt_uint8_t topic_type);

void network_get_interval(rt_uint16_t *real, rt_uint16_t *timing);

rt_err_t network_parameter_get_parse(const char *Str);

rt_err_t network_parameter_set_parse(const char *Str);

/*----------------------------------------------------------------------------*/

#endif
