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
#include "transport.h"
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
                device_info->device_id, SECURE_TCP, MD5_METHOD);
    // mqtt_log("client_id:%s", conn->client_id);
}

int mqtt_client_connect(rt_device_t dev, MQTTPacket_connectData *conn)
{
    rt_int16_t len;

    len = MQTTSerialize_connect(write_buffer, MSG_LEN_MAX, conn);
    if (len <= 0)
    {
        mqtt_log("Serialize connect packet failed,len = %d", len);
        return -RT_ERROR;
    }

    mqtt_log("MQTTSerialize_connect done");

    if (transport_sendPacketBuffer(0, write_buffer, len) == len)
    {
        mqtt_log("send mqtt connect packet done");
    }
    else
        goto exit;
    /* wait for connack */
    rt_memset(write_buffer, 0, sizeof(write_buffer));
    if (MQTTPacket_read(write_buffer, sizeof(write_buffer), transport_getdata) == CONNACK)
    {
        unsigned char sessionPresent, connack_rc;
        mqtt_log("get CONNACK packet");
        if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, write_buffer, sizeof(write_buffer)) != 1 || connack_rc != 0)
        {
            mqtt_log("Unable to connect, return code %d\n", connack_rc);
            goto exit;
        }
    }
    else
        goto exit;
    mqtt_log("MQTT connect sucess!!!");
    return RT_EOK;
exit:
    return -RT_ERROR;
}
/**
 ****************************************************************************
 * @Function : rt_err_t mqtt_client_subscribe(__mqtt_topic_t topic, iotx_device_info_pt iotx_dev_info)
 * @File     : mqtt_client.c
 * @Program  : topic:swith topic
 * @Created  : 2018-09-14 by seblee
 * @Brief    : 
 * @Version  : V1.0
**/
rt_err_t mqtt_client_subscribe(const char *subsc, iotx_device_info_pt iotx_dev_info)
{
    int req_qos = 0, rc;

    MQTTString topicString;
    topicString.cstring = (char *)subsc;
    rc = MQTTSerialize_subscribe(write_buffer, MSG_LEN_MAX, 0, 3, 1, &topicString, &req_qos);

    mqtt_log("len:%d，write_buffer:%s", rc, write_buffer + 5);
    transport_sendPacketBuffer(0, write_buffer, rc);
    mqtt_log("send mqtt subscription packet done");

    rt_memset(write_buffer, 0, rc);
    if (MQTTPacket_read(write_buffer, MSG_LEN_MAX, transport_getdata) == SUBACK) /* wait for suback */
    {
        mqtt_log("get SUBACK");
        unsigned short submsgid;
        int subcount;
        int granted_qos;

        rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, write_buffer, sizeof(write_buffer));
        if (granted_qos != 0)
        {
            mqtt_log("granted qos != 0, %d\n", granted_qos);
            if (granted_qos == 0x80)
                goto exit;
        }
    }
    else
        goto exit;
    mqtt_log("subscribe topic:%s sucess!!!", subsc);
    return RT_EOK;
exit:
    mqtt_log("RT_ERROR");
    return -RT_ERROR;
}
/**
 ****************************************************************************
 * @Function : rt_err_t mqtt_client_subscribe_topics(void)
 * @File     : mqtt_client.c
 * @Program  : none
 * @Created  : 2018-09-17 by seblee
 * @Brief    : subscribe all topics
 * @Version  : V1.0
**/
rt_err_t mqtt_client_subscribe_topics(void)
{
    rt_err_t rc;
    /*******TQ_WATER_NOTICE********/
    rt_thread_delay(1000);
    rc = mqtt_client_subscribe(TOPIC_WATER_NOTICE, &device_info);
    if (rc != RT_EOK)
    {
        mqtt_log("TOPIC_WATER_NOTICE,RT_ERROR:%d", rc);
        goto exit;
    }
    rt_thread_delay(1000);
    rc = mqtt_client_subscribe(TOPIC_WATER_STATUS, &device_info);
    if (rc != RT_EOK)
    {
        mqtt_log("TOPIC_WATER_STATUS,RT_ERROR:%d", rc);
        goto exit;
    }
    rt_thread_delay(1000);
    rc = mqtt_client_subscribe(TOPIC_PARAMETER_GET, &device_info);
    if (rc != RT_EOK)
    {
        mqtt_log("TOPIC_PARAMETER_GET,RT_ERROR:%d", rc);
        goto exit;
    }
    rt_thread_delay(1000);
    rc = mqtt_client_subscribe(TOPIC_DEVICE_UPGRADE, &device_info);
    if (rc != RT_EOK)
    {
        mqtt_log("TOPIC_DEVICE_UPGRADE,RT_ERROR:%d", rc);
        goto exit;
    }
    rt_thread_delay(1000);
    rc = mqtt_client_subscribe(TOPIC_DEVICE_MOVE, &device_info);
    if (rc != RT_EOK)
    {
        mqtt_log("TOPIC_DEVICE_MOVE,RT_ERROR:%d", rc);
        goto exit;
    }
    rt_thread_delay(1000);
    rc = mqtt_client_subscribe(TOPIC_DEVICE_GET, &device_info);
    if (rc != RT_EOK)
    {
        mqtt_log("TOPIC_DEVICE_GET,RT_ERROR:%d", rc);
        goto exit;
    }
exit:
    return rc;
}
/**
 ****************************************************************************
 * @Function : rt_err_t mqtt_packet_read_operation(void)
 * @File     : mqtt_client.c
 * @Program  : none
 * @Created  : 2018-09-17 by seblee
 * @Brief    : read mqtt packet and operation
 * @Version  : V1.0
**/
rt_err_t mqtt_packet_read_operation(void)
{
    rt_err_t rc = -1;
    rc = MQTTPacket_read(write_buffer, MSG_LEN_MAX, transport_getdata);
    if (rc > 0)
    {
        switch (rc)
        {
        case CONNECT:
            break;
        case CONNACK:
            break;
        case PUBLISH:
            break;
        case PUBACK:
            break;
        case PUBREC:
            break;
        case PUBREL:
            break;
        case PUBCOMP:
            break;
        case SUBSCRIBE:
            break;
        case SUBACK:
            break;
        case UNSUBSCRIBE:
            break;
        case UNSUBACK:
            break;
        case PINGREQ:
            break;
        case PINGRESP:
            break;
        case DISCONNECT:
            break;
        default:
            break;
        }
    }
}