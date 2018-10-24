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
 * @Last Modified time: 2018-09-26 18:04:54
 ****************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "mqtt_client.h"
#include "utils_hmac.h"
#include "transport.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#ifndef mqtt_log
#define mqtt_log(N, ...) rt_kprintf("####[MQTT %s:%4d] " N "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
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
    rt_strncpy(device_info.product_key, PRODUCT_KEY, strlen(PRODUCT_KEY));
    rt_strncpy(device_info.device_name, DEVICE_NAME, strlen(DEVICE_NAME));
    rt_strncpy(device_info.device_secret, DEVICE_SECRET, strlen(DEVICE_SECRET));
    rt_sprintf(device_info.device_id, DEVICE_ID, strlen(DEVICE_ID));
    // mqtt_log("product_key:%s", device_info.product_key);
    // mqtt_log("device_name:%s", device_info.device_name);
    // mqtt_log("device_secret:%s", device_info.device_secret);
    // mqtt_log("device_id:%s", device_info.device_id);
    mqtt_setup_connect_info(&device_connect, device_info_p);
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
    if (conn->style == IOT_WIFI_MODE)
        rt_snprintf(conn->client_id, sizeof(conn->client_id),
                    "%s"
                    "|securemode=%d"
                    ",signmethod=%s"
                    "|",
                    device_info->device_id, SECURE_TCP, MD5_METHOD);
    else if (conn->style == IOT_4G_MODE)
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
    rt_memset(read_buffer, 0, sizeof(read_buffer));
    if (MQTTPacket_read(read_buffer, sizeof(read_buffer), transport_getdata) == CONNACK)
    {
        unsigned char sessionPresent, connack_rc;
        mqtt_log("get CONNACK packet");
        if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, read_buffer, sizeof(read_buffer)) != 1 || connack_rc != 0)
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
 * @Function : rt_err_t mqtt_client_subscribe(_topic_enmu_t subsc, iotx_device_info_pt iotx_dev_info)
 * @File     : mqtt_client.c
 * @Program  : topic:swith topic
 * @Created  : 2018-09-14 by seblee
 * @Brief    : 
 * @Version  : V1.0
**/
rt_err_t mqtt_client_subscribe(_topic_enmu_t subsc, iotx_device_info_pt iotx_dev_info)
{
    int req_qos = 0, rc;
    unsigned short msgid1;
    MQTTString topicString;
    char topic_cache[100];
    rt_strncpy(topic_cache, iot_topics[subsc].topic_str, 100);

    topicString.cstring = topic_cache;
    msgid1 = mqtt_client_packet_id();
    rc = MQTTSerialize_subscribe(write_buffer, MSG_LEN_MAX, 0, msgid1, 1, &topicString, &req_qos);

    mqtt_log("len:%d,write_buffer:%s", rc, write_buffer + 5);
    transport_sendPacketBuffer(0, write_buffer, rc);
    mqtt_log("send mqtt subscription packet done");

    rt_memset(read_buffer, 0, rc);
    if (MQTTPacket_read(read_buffer, MSG_LEN_MAX, transport_getdata) == SUBACK) /* wait for suback */
    {
        mqtt_log("get SUBACK");
        unsigned short submsgid;
        int subcount;
        int granted_qos;

        rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, read_buffer, sizeof(read_buffer));
        if (granted_qos != 0)
        {
            mqtt_log("granted qos != 0, %d", granted_qos);
            if (granted_qos == MQTT_SUBFAIL)
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
    rt_thread_delay(1000);
    /*******WATER_NOTICE********/
    rc = mqtt_client_subscribe(WATER_NOTICE, device_info_p);
    if (rc != RT_EOK)
    {
        mqtt_log("WATER_NOTICE,RT_ERROR:%d", rc);
        // goto exit;
    }
    //rt_thread_delay(1000);
    /*******PARAMETER_SET********/
    rc = mqtt_client_subscribe(PARAMETER_SET, device_info_p);
    if (rc != RT_EOK)
    {
        mqtt_log("PARAMETER_SET,RT_ERROR:%d", rc);
        // goto exit;
    }
    //rt_thread_delay(1000);
    /*******PARAMETER_GET********/
    rc = mqtt_client_subscribe(PARAMETER_GET, device_info_p);
    if (rc != RT_EOK)
    {
        mqtt_log("PARAMETER_GET,RT_ERROR:%d", rc);
        goto exit;
    }

    //rt_thread_delay(1000);
    /*******DEVICE_UPGRADE********/
    // rc = mqtt_client_subscribe(DEVICE_UPGRADE,device_info_p);
    // if (rc != RT_EOK)
    // {
    //     mqtt_log("DEVICE_UPGRADE,RT_ERROR:%d", rc);
    //     goto exit;
    // }
    // //rt_thread_delay(1000);
    /*******DEVICE_MOVE********/
    // rc = mqtt_client_subscribe(DEVICE_MOVE,device_info_p);
    // if (rc != RT_EOK)
    // {
    //     mqtt_log("DEVICE_MOVE,RT_ERROR:%d", rc);
    //     goto exit;
    // }
    // //rt_thread_delay(1000);
    // /*******DEVICE_GET********/
    // rc = mqtt_client_subscribe(DEVICE_GET,device_info_p);
    // if (rc != RT_EOK)
    // {
    //     mqtt_log("DEVICE_GET,RT_ERROR:%d", rc);
    //     goto exit;
    // }
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
    rc = MQTTPacket_read(read_buffer, MSG_LEN_MAX, transport_getdata);
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
            mqtt_client_receive_publish((const char *)read_buffer, MSG_LEN_MAX);
            break;
        case PUBACK:
        {
            unsigned short mypacketid;
            unsigned char dup, type;
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, read_buffer, MSG_LEN_MAX) == 1)
                mqtt_log("PUBACK,type:%d,dup:%d,packetid:%d", type, dup, mypacketid);
            else
                mqtt_log("PUBACK Deserialize err");
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
 * @Function : rt_err_t mqtt_client_ping(void)
 * @File     : mqtt_client.c
 * @Program  : none
 * @Created  : 2018-09-18 by seblee
 * @Brief    : ping server keep alive
 * @Version  : V1.0
**/
rt_err_t mqtt_client_ping(void)
{
    rt_err_t rc, len;
    len = MQTTSerialize_pingreq(write_buffer, MSG_LEN_MAX);
    rc = transport_sendPacketBuffer(0, write_buffer, len);
    mqtt_log("mqtt_client_ping packet:%d,send:%d", len, rc);
    if (rc == len)
        return RT_EOK;
    else
        return -RT_ERROR;
}
/**
 ****************************************************************************
 * @Function : rt_err_t mqtt_client_publish(char *topic, rt_uint8_t dup, int qos, rt_uint8_t restained ,rt_uint8_t *msg, rt_uint16_t msg_len)
 * @File     : mqtt_client.c
 * @Program  : topic:the point topic to publish 
 *             dup:Position: byte 1, bit 3. 0 first publish,1 has been published before
 *             qos:Position: byte 1, bits 2-1. 0 At most once delivery;1 At least once delivery;2 Exactly once delivery
 *             restained:Position: byte 1, bit 0.
 *             *msg:the piont of msg
 *             msg_len:the msg length
 * @Created  : 2018-09-19 by seblee
 * @Brief    : publish a topic
 * @Version  : V1.0
**/
rt_err_t mqtt_client_publish(char *topic, rt_uint8_t dup, int qos, rt_uint8_t restained, rt_uint8_t *msg, rt_uint16_t msg_len)
{
    rt_err_t rc, len;
    unsigned short msgid;
    MQTTString topicString;
    rt_uint8_t *payload;
    rt_uint16_t payload_len;

    msgid = mqtt_client_packet_id();
    topicString.cstring = topic;
    payload = msg;
    payload_len = msg_len;

    // mqtt_log("mqtt_client_publish msg:%s",msg);
    mqtt_log("mqtt_client_publish topic:%s", topicString.cstring);

    len = MQTTSerialize_publish(write_buffer, MSG_LEN_MAX, dup, qos, restained, msgid, topicString, payload, payload_len);

    rc = transport_sendPacketBuffer(0, write_buffer, len);
    mqtt_log("mqtt_client_publish msgid:%d,packet:%d,send:%d", msgid, len, rc);
    if (rc == len)
        return RT_EOK;
    else
        return -RT_ERROR;
}
/**
 ****************************************************************************
 * @Function : rt_err_t mqtt_client_publish_topics(void)
 * @File     : mqtt_client.c
 * @Program  : none
 * @Created  : 2018-09-19 by seblee
 * @Brief    : publish init topics
 * @Version  : V1.0
**/
rt_err_t mqtt_client_publish_topics(void)
{
    rt_err_t rc = -RT_ERROR;
    char *msg_playload = RT_NULL; //need free
    network_Serialize_init_json(&msg_playload);
    if (msg_playload == RT_NULL)
        goto exit;
    /*****publish TOPIC_PLATFORM_INIT************/
    rc = mqtt_client_publish(TOPIC_PLATFORM_INIT, 0, 1, 0, (rt_uint8_t *)msg_playload, strlen(msg_playload));
    rt_free(msg_playload);
    if (rc == RT_EOK)
    {
        rc = MQTTPacket_read(read_buffer, MSG_LEN_MAX, transport_getdata);
        if (rc == PUBACK)
        {
            unsigned short mypacketid;
            unsigned char dup, type;
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, read_buffer, MSG_LEN_MAX) == 1)
            {
                mqtt_log("PUBACK,type:%d,dup:%d,packetid:%d", type, dup, mypacketid);
                rc = RT_EOK;
            }
            else
            {
                mqtt_log("PUBACK Deserialize err");
                rc = -RT_ERROR;
                goto exit;
            }
        }
        else
            goto exit;
    }
    else
        goto exit;

/*************************************/
exit:
    return rc;
}
/**
 ****************************************************************************
 * @Function : rt_err_t mqtt_client_publish_parameter(void)
 * @File     : mqtt_client.c
 * @Program  : none
 * @Created  : 2018-09-21 by seblee
 * @Brief    : parameter report
 * @Version  : V1.0
**/
rt_err_t mqtt_client_publish_parameter(void)
{
    rt_err_t rc = -RT_ERROR;
    char *msg_playload = RT_NULL; //need free

    network_Serialize_para_json(&msg_playload);
    if (msg_playload == RT_NULL)
        goto exit;

    /*****publish TOPIC_PLATFORM_INIT************/
    rc = mqtt_client_publish(TOPIC_PARAMETER_PUT, 0, 1, 0, (rt_uint8_t *)msg_playload, strlen(msg_playload));
    rt_free(msg_playload);
    if (rc == RT_EOK)
    {
        rc = MQTTPacket_read(read_buffer, MSG_LEN_MAX, transport_getdata);
        if (rc == PUBACK)
        {
            unsigned short mypacketid;
            unsigned char dup, type;
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, read_buffer, MSG_LEN_MAX) == 1)
            {
                mqtt_log("PUBACK,type:%d,dup:%d,packetid:%d", type, dup, mypacketid);
                rc = RT_EOK;
            }
            else
            {
                mqtt_log("PUBACK Deserialize err");
                rc = -RT_ERROR;
                goto exit;
            }
        }
        else
            goto exit;
    }
    else
        goto exit;
exit:
    return rc;
}

/**
 ****************************************************************************
 * @Function : rt_err_t mqtt_client_find_topic(char *topic)
 * @File     : mqtt_client.c
 * @Program  : topic:the putin topic
 * @Created  : 2018-09-25 by seblee
 * @Brief    : find the putin topic
 * @Version  : V1.0
**/
rt_err_t mqtt_client_find_topic(char *topic)
{
    rt_err_t rc = -RT_ERROR;
    int i;
    for (i = PLATFORM_INIT; i < DEVICE_GET + 1; i++)
    {
        if (rt_strstr(topic, iot_topics[i].topic_str))
        {
            rc = i;
            mqtt_log("i:%d,topic:%s", i, iot_topics[i].topic_str);
            break;
        }
    }
    return rc;
}

/**
 ****************************************************************************
 * @Function : rt_err_t mqtt_client_publish_report(_topic_enmu_t topic_type)
 * @File     : mqtt_client.c
 * @Program  : topic_type:REALTIME_REPORT/TIMING_REPORT
 * @Created  : 2018-09-26 by seblee
 * @Brief    : publish report
 * @Version  : V1.0
**/
rt_err_t mqtt_client_publish_report(_topic_enmu_t topic_type)
{
    rt_err_t rc = -RT_ERROR;
    char *msg_playload = RT_NULL; //need free
    _topic_enmu_t type = topic_type;
    network_Serialize_report_json(&msg_playload, type);
    if (msg_playload == RT_NULL)
        goto exit;

    /*****publish TOPIC_PLATFORM_INIT************/
    if (type == REALTIME_REPORT)
        rc = mqtt_client_publish(TOPIC_REALTIME_REPORT, 0, 1, 0, (rt_uint8_t *)msg_playload, strlen(msg_playload));
    else
        rc = mqtt_client_publish(TOPIC_TIMING_REPORT, 0, 1, 0, (rt_uint8_t *)msg_playload, strlen(msg_playload));
    rt_free(msg_playload);
    if (rc == RT_EOK)
    {
        rc = MQTTPacket_read(read_buffer, MSG_LEN_MAX, transport_getdata);
        if (rc == PUBACK)
        {
            unsigned short mypacketid;
            unsigned char dup, type;
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, read_buffer, MSG_LEN_MAX) == 1)
            {
                mqtt_log("PUBACK,type:%d,dup:%d,packetid:%d", type, dup, mypacketid);
                rc = RT_EOK;
            }
            else
            {
                mqtt_log("PUBACK Deserialize err");
                rc = -RT_ERROR;
                goto exit;
            }
        }
        else
            goto exit;
    }
    else
        goto exit;
exit:
    return rc;
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
    if (qos == MQTT_QOS0)
    {
    }
    else if (qos == MQTT_QOS1)
        result = mqtt_client_MQTTPuback(write_buffer, MSG_LEN_MAX, msgid, PUBACK);
    else if (qos == MQTT_QOS2)
        result = mqtt_client_MQTTPuback(write_buffer, MSG_LEN_MAX, msgid, PUBREC);

    mqtt_log("receivedTopic:%.*s", receivedTopic.lenstring.len, receivedTopic.lenstring.data);
    mqtt_log("message arrived:%.*s", payloadlen_in, payload_in);
    rc = mqtt_client_find_topic(receivedTopic.lenstring.data);
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
    rc = transport_sendPacketBuffer(0, c, len);
    mqtt_log("Puback packet:%d,send:%d,msgId:%d", len, rc, msgId);
    if (rc == len)
        return RT_EOK;
    else
        return -RT_ERROR;
}
