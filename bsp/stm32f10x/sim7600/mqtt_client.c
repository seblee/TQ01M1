/**
 ****************************************************************************
 * @Warning :Without permission from the author,Not for commercial use
 * @File    :
 * @Author  :Seblee
 * @date    :2018-09-11 11:11:25
 * @version :V1.0.0
 *************************************************
 * @brief   :
 ****************************************************************************
 * @Last Modified by: Seblee
 * @Last Modified time: 2018-09-11 18:18:35
 ****************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "mqtt_client.h"
#include "utils_hmac.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#ifndef mqtt_log
#define mqtt_log(N, ...) rt_kprintf("####[MQTT %s:%4d] " N "\r\n", __FILE__, __LINE__, ##__VA_ARGS__);
#endif /* at_log(...) */
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
int mqtt_client_init(rt_device_t dev)
{
    /*******init client parameter*********/
    mqtt_log("mqtt_client_init");
    rt_memset(&device_info, 0, sizeof(iotx_device_info_t));
    rt_strncpy(device_info.product_key, iot_productKey, PRODUCT_KEY_LEN);
    rt_strncpy(device_info.device_name, iot_devicename, DEVICE_NAME_LEN);
    rt_strncpy(device_info.device_secret, iot_secret, DEVICE_SECRET_LEN);
    rt_sprintf(device_info.device_id, iot_deviceid, DEVICE_ID_LEN);
    // mqtt_log("product_key:%s", device_info.product_key);
    // mqtt_log("device_name:%s", device_info.device_name);
    // mqtt_log("device_secret:%s", device_info.device_secret);
    // mqtt_log("device_id:%s", device_info.device_id);
    mqtt_setup_connect_info(&device_connect, &device_info);
    client_con.keepAliveInterval = 60;
    client_con.clientID.cstring = (char *)device_connect.client_id;
    client_con.username.cstring = (char *)device_connect.username;
    client_con.password.cstring = (char *)device_connect.password;
    // mqtt_log("clientID:%s", client_con.clientID.cstring);
    // mqtt_log("username:%s", client_con.username.cstring);
    // mqtt_log("password:%s", client_con.password.cstring);
    return 0;
}
void mqtt_setup_connect_info(iotx_conn_info_t *conn, iotx_device_info_t *device_info)
{
    char guider_sign[GUIDER_SIGN_LEN] = {0};
    char hmac_source[512] = {0};

    conn->port = aliyun_iot_port;
    rt_snprintf(conn->host_name, sizeof(conn->host_name), aliyun_domain, device_info->product_key);
    rt_snprintf(conn->username, sizeof(conn->username), "%s&%s", device_info->device_name, device_info->product_key);
    rt_snprintf(hmac_source, sizeof(hmac_source),
                "clientId%s"
                "deviceName%s"
                "productKey%s",
                device_info->device_id, device_info->device_name, device_info->product_key);
    // mqtt_log("host_name:%s", conn->host_name);
    // mqtt_log("username:%s", conn->username);
    // mqtt_log("hmac_source:%s", hmac_source);
    utils_hmac_md5(hmac_source, strlen(hmac_source),
                   guider_sign,
                   device_info->device_secret,
                   strlen(device_info->device_secret));
    rt_snprintf(conn->password, sizeof(conn->password),
                "%s",
                guider_sign);
    // mqtt_log("password:%s", conn->password);

    rt_snprintf(conn->client_id, sizeof(conn->client_id),
                "%s"
                "|securemode=%d"
                ",signmethod=%s"
                "|",
                device_info->device_id, SECURE_TLS, MD5_METHOD);
    // mqtt_log("client_id:%s", conn->client_id);
}

int mqtt_client_connect(rt_device_t dev, MQTTPacket_connectData *conn)
{
    rt_int16_t len;
    len = MQTTSerialize_connect(write_buffer, MSG_LEN_MAX, conn);
    if (len <= 0)
        return RT_ERROR;
    return RT_EOK;
}
