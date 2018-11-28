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
 * @Last Modified time: 2018-11-21 16:01:28
 ****************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "mqtt_client.h"
#include "utils_hmac.h"
#include "paho_mqtt.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#ifndef mqtt_log
#define mqtt_log(N, ...) rt_kprintf("####[MQTT %s:%4d] " N "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif /* mqtt_log(...) */

#define DBG_ENABLE
#define DBG_SECTION_NAME "mqtt"
#ifdef MQTT_DEBUG
#define DBG_LEVEL DBG_LOG
#else
#define DBG_LEVEL DBG_INFO
#endif /* MQTT_DEBUG */
#define DBG_COLOR
#include <rtdbg.h>

#ifndef LOG_D
#error "Please update the 'rtdbg.h' file to GitHub latest version (https://github.com/RT-Thread/rt-thread/blob/master/include/rtdbg.h)"
#endif

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

static void mqtt_WATER_NOTICE_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub callback: %.*s %.*s",
          msg_data->topicName->lenstring.len,
          msg_data->topicName->lenstring.data,
          msg_data->message->payloadlen,
          (char *)msg_data->message->payload);
    if (network_water_notice_parse((const char *)msg_data->message->payload) == RT_EOK)
        c->isQRcodegeted = 1;
    return;
}
static void mqtt_PARAMETER_GET_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub callback: %.*s %.*s",
          msg_data->topicName->lenstring.len,
          msg_data->topicName->lenstring.data,
          msg_data->message->payloadlen,
          (char *)msg_data->message->payload);
    if (network_water_notice_parse((const char *)msg_data->message->payload) == RT_EOK)
        c->isparameterPutted = 0;
    return;
}
static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub callback: %.*s %.*s",
          msg_data->topicName->lenstring.len,
          msg_data->topicName->lenstring.data,
          msg_data->message->payloadlen,
          (char *)msg_data->message->payload);

    return;
}

static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub default callback: %.*s %.*s",
          msg_data->topicName->lenstring.len,
          msg_data->topicName->lenstring.data,
          msg_data->message->payloadlen,
          (char *)msg_data->message->payload);
    return;
}

static void mqtt_connect_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_connect_callback!");
}

static void mqtt_online_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_online_callback!");
}

static void mqtt_offline_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_offline_callback!");
}

int mqtt_client_init(MQTTClient *client)
{
    iotx_device_info_t device_info;
    iotx_conn_info_t device_connect;

    MQTTPacket_connectData client_con = MQTTPacket_connectData_initializer;
    client->isconnected = 0;
    client->isQRcodegeted = 0;
    client->isparameterPutted = 0;
    /*******init client parameter*********/
    mqtt_log("mqtt_client_init");
    rt_memset(&device_info, 0, sizeof(iotx_device_info_t));
    rt_strncpy(device_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
    rt_strncpy(device_info.device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    rt_strncpy(device_info.device_secret, DEVICE_SECRET, strlen(DEVICE_SECRET));
    rt_sprintf(device_info.device_id, DEVICE_ID, strlen(DEVICE_ID));
    rt_snprintf(device_info.device_id, sizeof(device_connect.client_id), "rtthread%d", rt_tick_get());
    // LOG_D("product_key:%s", device_info.product_key);
    // LOG_D("device_name:%s", device_info.device_name);
    // LOG_D("device_secret:%s", device_info.device_secret);
    // LOG_D("device_id:%s", device_info.device_id);
    // mqtt_setup_connect_info(&device_connect, device_info_p);

    /* generate the random client ID */
    mqtt_setup_connect_info(&device_connect, &device_info);

    client_con.keepAliveInterval = 60;
    client_con.clientID.cstring = (char *)device_connect.client_id;
    client_con.username.cstring = (char *)device_connect.username;
    client_con.password.cstring = (char *)device_connect.password;
    // LOG_D("clientID:%s", client_con.clientID.cstring);
    // LOG_D("username:%s", client_con.username.cstring);
    // LOG_D("password:%s", client_con.password.cstring);

    {
        char mqtt_uri[100] = {0};
        rt_snprintf(mqtt_uri, sizeof(mqtt_uri),
                    "tcp://%s:%d",
                    device_connect.host_name,
                    device_connect.port);
        char *uri_p = rt_malloc(strlen(mqtt_uri) + 1);
        if (!uri_p)
        {
            LOG_D("no memory for MQTT client buffer!");
            goto _exit;
        }
        rt_memset(uri_p, 0, strlen(mqtt_uri) + 1);
        rt_strncpy(uri_p, mqtt_uri, strlen(mqtt_uri));
        client->uri = uri_p;
    }

    LOG_D("client->uri:%s", client->uri);
    /* config connect param */
    memcpy(&client->condata, &client_con, sizeof(client_con));
    // LOG_D("client->clientID:%s", client->condata.clientID);
    // LOG_D("client->username:%s", client->condata.username);
    // LOG_D("client->password:%s", client->condata.password);

    // LOG_D("clientID:%s", client_con.clientID.cstring);
    // LOG_D("username:%s", client_con.username.cstring);
    // LOG_D("password:%s", client_con.password.cstring);
    /* config MQTT will param. */
    // client->condata.willFlag = 1;
    // client->condata.will.qos = 1;
    // client->condata.will.retained = 0;
    // client->condata.will.topicName.cstring = MQTT_PUBTOPIC;
    // client->condata.will.message.cstring = MQTT_WILLMSG;

    /* malloc buffer. */
    client->buf_size = client->readbuf_size = 1024;
    client->buf = rt_malloc(client->buf_size);
    client->readbuf = rt_malloc(client->readbuf_size);
    if (!(client->buf && client->readbuf))
    {
        LOG_D("no memory for MQTT client buffer!");
        goto _exit;
    }
    /* set event callback function */
    client->connect_callback = mqtt_connect_callback;
    client->online_callback = mqtt_online_callback;
    client->offline_callback = mqtt_offline_callback;

    /* set subscribe table and event callback */
    {
        client->messageHandlers[WATER_NOTICE].topicFilter =
            (char *)iot_sub_topics[WATER_NOTICE].topic_str;
        client->messageHandlers[WATER_NOTICE].callback = mqtt_WATER_NOTICE_callback;
        client->messageHandlers[WATER_NOTICE].qos = iot_sub_topics[WATER_NOTICE].qos;

        client->messageHandlers[PARAMETER_SET].topicFilter =
            (char *)iot_sub_topics[PARAMETER_SET].topic_str;
        client->messageHandlers[PARAMETER_SET].callback = mqtt_sub_callback;
        client->messageHandlers[PARAMETER_SET].qos = iot_sub_topics[PARAMETER_SET].qos;

        client->messageHandlers[PARAMETER_GET].topicFilter =
            (char *)iot_sub_topics[PARAMETER_GET].topic_str;
        client->messageHandlers[PARAMETER_GET].callback = mqtt_PARAMETER_GET_callback;
        client->messageHandlers[PARAMETER_GET].qos = iot_sub_topics[PARAMETER_GET].qos;
    }

    /* set default subscribe event callback */
    client->defaultMessageHandler = mqtt_sub_default_callback;

_exit:
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
    // LOG_D("host_name:%s", conn->host_name);
    // LOG_D("username:%s", conn->username);
    // LOG_D("hmac_source:%s", hmac_source);
    utils_hmac_md5(hmac_source, strlen(hmac_source),
                   guider_sign,
                   device_info->device_secret,
                   strlen(device_info->device_secret));
    rt_snprintf(conn->password, sizeof(conn->password),
                "%s",
                guider_sign);
    LOG_D("password:%s", conn->password);
    // if (conn->style == IOT_WIFI_MODE)
    //     rt_snprintf(conn->client_id, sizeof(conn->client_id),
    //                 "%s"
    //                 "|securemode=%d"
    //                 ",signmethod=%s"
    //                 "|",
    //                 device_info->device_id, SECURE_TCP, MD5_METHOD);
    // else if (conn->style == IOT_4G_MODE)
    rt_snprintf(conn->client_id, sizeof(conn->client_id),
                "%s"
                "|securemode=%d"
                ",signmethod=%s"
                "|",
                device_info->device_id, SECURE_TCP, MD5_METHOD);
    LOG_D("client_id:%s", conn->client_id);
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
    // rc = MQTTPacket_read(read_buffer, MSG_LEN_MAX, transport_getdata);
    if (rc > 0)
    {
        switch (rc)
        {
        // case CONNECT:
        //     break;
        case CONNACK:
            mqtt_log("packet type:CONNACK");
            break;
        case PUBLISH:
            mqtt_log("packet type:PUBLISH");
            // mqtt_client_receive_publish((const char *)read_buffer, MSG_LEN_MAX);
            break;
        case PUBACK:
        {
            //            unsigned short mypacketid;
            //            unsigned char dup, type;
            // if (MQTTDeserialize_ack(&type, &dup, &mypacketid, read_buffer, MSG_LEN_MAX) == 1)
            //     mqtt_log("PUBACK,type:%d,dup:%d,packetid:%d", type, dup, mypacketid);
            // else
            //     mqtt_log("PUBACK Deserialize err");
        }
        break;
        case PUBREC:
            mqtt_log("packet type:PUBREC");
            break;
        case PUBREL:
            mqtt_log("packet type:PUBREL");
            break;
        case PUBCOMP:
            mqtt_log("packet type:PUBCOMP");
            break;
        // case SUBSCRIBE:
        //     break;
        case SUBACK:
            mqtt_log("packet type:SUBACK");
            break;
        // case UNSUBSCRIBE:
        //     break;
        case UNSUBACK:
            mqtt_log("packet type:UNSUBACK");
            break;
        // case PINGREQ:
        //     break;
        case PINGRESP:
            mqtt_log("packet type:PINGRESP");
            break;
        case DISCONNECT:
            mqtt_log("packet type:DISCONNECT");
            break;
        default:
            break;
        }
    }
    return rc;
}
/**
 ****************************************************************************
 * @Function : int mqtt_client_packet_id(void)
 * @File     : mqtt_client.c
 * @Program  : none
 * @Created  : 2018-09-18 by seblee
 * @Brief    : get packet_id
 * @Version  : V1.0
**/
unsigned short mqtt_client_packet_id(void)
{
    static unsigned short id = 1;
    if (id < 1)
        id = 1;
    return id++;
}

/**
 ****************************************************************************
 * @Function : rt_err_t mqtt_client_receive_publish(const char *c, rt_uint16_t len)
 * @File     : mqtt_client.c
 * @Program  : *c:data buffer
 * @Created  : 2018-09-28 by seblee
 * @Brief    : operation received publish data
 * @Version  : V1.0
**/
rt_err_t mqtt_client_receive_publish(const char *c, rt_uint16_t len)
{
    rt_err_t rc = -1, result;
    unsigned char *p = (unsigned char *)c;
    rt_uint16_t length = len;
    unsigned char dup;
    int qos;
    unsigned char retained;
    unsigned short msgid;
    int payloadlen_in;
    unsigned char *payload_in;
    MQTTString receivedTopic;

    rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
                                 &payload_in, &payloadlen_in, p, length);
    mqtt_log("mqtt received msgid:%d,dup:%d,qos:%d", msgid, dup, qos);
    //    if (qos == MQTT_QOS0)
    //    {
    //    }
    //    else if (qos == MQTT_QOS1)
    //        result = mqtt_client_MQTTPuback(write_buffer, MSG_LEN_MAX, msgid, PUBACK);
    //    else if (qos == MQTT_QOS2)
    //        result = mqtt_client_MQTTPuback(write_buffer, MSG_LEN_MAX, msgid, PUBREC);

    mqtt_log("receivedTopic:%.*s", receivedTopic.lenstring.len, receivedTopic.lenstring.data);
    mqtt_log("message arrived:%.*s", payloadlen_in, payload_in);
    //  rc = mqtt_client_find_topic(receivedTopic.lenstring.data);
    if (rc > 0)
    {
        mqtt_log("find_topic rc:%d", rc);
        switch (rc)
        {
        case WATER_NOTICE:
            network_water_notice_parse((const char *)payload_in);
            break;
        case PARAMETER_SET:
            network_parameter_set_parse((const char *)payload_in);
            break;
        case PARAMETER_GET:
            network_parameter_get_parse((const char *)payload_in);
            break;
        default:
            break;
        }
    }
    return result;
}

/**
 ****************************************************************************
 * @Function : rt_err_t mqtt_client_MQTTPuback(rt_uint8_t *c, rt_uint16_t len, enum msgTypes type)
 * @File     : mqtt_client.c
 * @Program  : none
 * @Created  : 2018-09-28 by seblee
 * @Brief    : 
 * @Version  : V1.0
**/
rt_err_t mqtt_client_MQTTPuback(rt_uint8_t *c, rt_uint16_t len, unsigned int msgId, enum msgTypes type)
{
    rt_err_t rc, lenth = len;
    if (type == PUBACK)
        len = MQTTSerialize_ack((rt_uint8_t *)c, lenth, PUBACK, 0, msgId);
    else if (type == PUBREC)
        len = MQTTSerialize_ack((rt_uint8_t *)c, lenth, PUBREC, 0, msgId);
    else if (type == PUBREL)
        len = MQTTSerialize_ack((rt_uint8_t *)c, lenth, PUBREL, 0, msgId);
    else
        return -RT_EIO;

    if (len <= 0)
        return -RT_ERROR;
    // rc = transport_sendPacketBuffer(0, c, len);
    mqtt_log("Puback packet:%d,send:%d,msgId:%d", len, rc, msgId);
    if (rc == len)
        return RT_EOK;
    else
        return -RT_ERROR;
}

/**
 ****************************************************************************
 * @Function : rt_err_t network_get_register(iotx_device_info_pt device_info_p)
 * @File     : network.c
 * @Program  : none
 * @Created  : 2018-10-17 by seblee
 * @Brief    : 
 * @Version  : V1.0
**/
// extern sys_reg_st g_sys;
rt_err_t network_get_register(iotx_device_info_pt device_info_p)
{
    rt_err_t err;
     char guider_sign[256] = {0};
    char request[512] = {0};
    char body[512] = {0};
    // iotx_device_info_pt device_info_p;
    // device_info_p = g_sys.config.ComPara.device_info;
    if (device_info_p->flag == IOT_SN_FLAG)
    {
    }
    else if (device_info_p->flag == IOT_SID_FLAG)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }

    rt_snprintf(request, sizeof(request),
                "deviceName%sproductKey%srandom567345",
                REGISTER_DEVICE_NAME, REGISTER_PRODUCT_KEY);
    mqtt_log("scr:%s", request);
    utils_hmac_md5(request, strlen(request),
                   guider_sign,
                   REGISTER_PRODUCT_SECRET,
                   strlen(REGISTER_PRODUCT_SECRET));
    mqtt_log("sign:%s", guider_sign);
    rt_snprintf((char *)body, sizeof(body),
                "productKey=%s&deviceName=%s&random=567345&sign=%s&signMethod=HmacMD5",
                REGISTER_PRODUCT_KEY, REGISTER_DEVICE_NAME, guider_sign);
    mqtt_log("body:%s", body);

    rt_snprintf(request, sizeof(request),
                "POST %s HTTP/1.1\r\n"
                "Host: %s\r\n"
                "Content-Type: application/x-www-form-urlencoded\r\n"
                "Content-Length: %d\r\n"
                "\r\n"
                "%s",
                REGISTER_PATH, REGISTER_HOST, strlen((char *)body), body);
    mqtt_log("request:%s", request);

    return err;
}
