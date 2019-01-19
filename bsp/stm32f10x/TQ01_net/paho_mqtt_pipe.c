#include <string.h>
#include <stdint.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <dfs_posix.h>
#include <time.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>

#include "MQTTPacket.h"
#include "paho_mqtt.h"
#include <rtlibc.h>

#include "network.h"
#include "mqtt_client.h"

#include "modul_ctr.h"
// #include "disguise_time.h"

#define DBG_ENABLE
#define DBG_SECTION_NAME "paho"
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

#ifndef RT_PKG_MQTT_THREAD_STACK_SIZE
#ifdef MQTT_USING_TLS
#define RT_PKG_MQTT_THREAD_STACK_SIZE 6144
#else
#define RT_PKG_MQTT_THREAD_STACK_SIZE 4096
#endif
#endif

#ifdef MQTT_USING_TLS
#if (RT_PKG_MQTT_THREAD_STACK_SIZE < 6144)
#error "MQTT using tls, please increase MQTT thread stack size up to 6K via menuconfig tool!"
#endif
#endif

static int mq_client_publish(MQTTClient *c, _topic_pub_enmu_t pub_type);
static int MQTT_cycle(MQTTClient *c);

/*
 * resolve server address
 * @param server the server sockaddress
 * @param url the input URL address.
 * @param host_addr the buffer pointer to save server host address
 * @param request the pointer to point the request url, for example, /index.html
 *
 * @return 0 on resolve server address OK, others failed
 *
 * URL example:
 * tcp://192.168.10.151:1883
 * tls://192.168.10.151:61614
 * tcp://[fe80::20c:29ff:fe9a:a07e]:1883
 * tls://[fe80::20c:29ff:fe9a:a07e]:61614
 */
static int mqtt_resolve_uri(MQTTClient *c, struct addrinfo **res)
{
    int rc = 0;
    int uri_len = 0, host_addr_len = 0, port_len = 0;
    char *ptr;
    char port_str[6] = {0}; /* default port of mqtt(http) */

    const char *host_addr = 0;
    char *host_addr_new = RT_NULL;
    const char *uri = c->uri;
    uri_len = strlen(uri);
    // LOG_D(" c->uri:%s", c->uri);

    /* strip protocol(tcp or ssl) */
    if (strncmp(uri, "tcp://", 6) == 0)
    {
        host_addr = uri + 6;
    }
    else if (strncmp(uri, "ssl://", 6) == 0)
    {
        host_addr = uri + 6;

#ifndef MQTT_USING_TLS
        LOG_W("[%d] Warning: tls uri, please enable mqtt tls support!", rt_tick_get());
        rc = -1;
        goto _exit;
#endif
    }
    else
    {
        rc = -1;
        goto _exit;
    }
    /* ipv6 address */
    if (host_addr[0] == '[')
    {
        host_addr += 1;
        ptr = strstr(host_addr, "]");
        if (!ptr)
        {
            rc = -1;
            goto _exit;
        }
        host_addr_len = ptr - host_addr;
        if ((host_addr_len < 1) || (host_addr_len > uri_len))
        {
            rc = -1;
            goto _exit;
        }

        port_len = uri_len - 6 - host_addr_len - 3;
        if (port_len >= 6 || port_len < 1)
        {
            rc = -1;
            goto _exit;
        }

        strncpy(port_str, host_addr + host_addr_len + 2, port_len);
        port_str[port_len] = '\0';
        LOG_D("ipv6 address port: %s", port_str);
    }
    else /* ipv4 or domain. */
    {
        ptr = strstr(host_addr, ":");
        if (!ptr)
        {
            rc = -1;
            goto _exit;
        }
        host_addr_len = ptr - host_addr;
        if ((host_addr_len < 1) || (host_addr_len > uri_len))
        {
            rc = -1;
            goto _exit;
        }

        port_len = uri_len - 6 - host_addr_len - 1;
        if (port_len >= 6 || port_len < 1)
        {
            rc = -1;
            goto _exit;
        }

        strncpy(port_str, host_addr + host_addr_len + 1, port_len);
        port_str[port_len] = '\0';
        // LOG_D("ipv4 address port: %s", port_str);
    }

    /* get host addr ok. */
    {
        /* resolve the host name. */
        struct addrinfo hint;
        int ret;

        host_addr_new = rt_malloc(host_addr_len + 1);

        if (!host_addr_new)
        {
            rc = -1;
            goto _exit;
        }

        memcpy(host_addr_new, host_addr, host_addr_len);
        host_addr_new[host_addr_len] = '\0';
        // LOG_D("HOST = '%s'", host_addr_new);

#ifdef MQTT_USING_TLS
        if (c->tls_session)
        {
            c->tls_session->host = rt_strdup(host_addr_new);
            c->tls_session->port = rt_strdup(port_str);
        }
#endif

        memset(&hint, 0, sizeof(hint));

        ret = getaddrinfo(host_addr_new, port_str, &hint, res);
        if (ret != 0)
        {
            LOG_E("[%d] getaddrinfo err: %d '%s'", rt_tick_get(), ret, host_addr_new);
            rc = -1;
            goto _exit;
        }
        (*res)->ai_family = AF_INET;
    }

_exit:
    if (host_addr_new != RT_NULL)
    {
        rt_free(host_addr_new);
        host_addr_new = RT_NULL;
    }
    return rc;
}

#ifdef MQTT_USING_TLS
static int mqtt_open_tls(MQTTClient *c)
{
    int tls_ret = 0;
    const char *pers = "mqtt";

    if (!c)
        return -RT_ERROR;

    c->tls_session = (MbedTLSSession *)rt_malloc(sizeof(MbedTLSSession));
    if (c->tls_session == RT_NULL)
    {
        LOG_E("[%d] open tls failed, no memory for tls_session buffer malloc", rt_tick_get());
        return -RT_ENOMEM;
    }
    memset(c->tls_session, 0x0, sizeof(MbedTLSSession));

    c->tls_session->buffer_len = MQTT_TLS_READ_BUFFER;
    c->tls_session->buffer = rt_malloc(c->tls_session->buffer_len);
    if (c->tls_session->buffer == RT_NULL)
    {
        LOG_E("[%d] open tls failed, no memory for tls_session buffer malloc", rt_tick_get());
        rt_free(c->tls_session);
        c->tls_session = RT_NULL;
        return -RT_ENOMEM;
    }

    if ((tls_ret = mbedtls_client_init(c->tls_session, (void *)pers, strlen(pers))) < 0)
    {
        LOG_E("[%d] mbedtls_client_init err return : -0x%x", rt_tick_get(), -tls_ret);
        return -RT_ERROR;
    }

    return RT_EOK;
}
#endif

static int net_connect(MQTTClient *c)
{
    int rc = -1;
    struct addrinfo *addr_res = RT_NULL;
#ifdef MQTT_USING_TLS
    int timeout = MQTT_SOCKET_TIMEO;
#endif
    c->sock = -1;
    c->next_packetid = 0;

#ifdef MQTT_USING_TLS
    if (strncmp(c->uri, "ssl://", 6) == 0)
    {
        if (mqtt_open_tls(c) < 0)
        {
            LOG_E("[%d] mqtt_open_tls err!", rt_tick_get());
            return -RT_ERROR;
        }
    }
    else
    {
        c->tls_session = RT_NULL;
    }
#endif

    rc = mqtt_resolve_uri(c, &addr_res);
    if (rc < 0 || addr_res == RT_NULL)
    {
        LOG_E("[%d] resolve uri err", rt_tick_get());
        goto _exit;
    }

#ifdef MQTT_USING_TLS
    if (c->tls_session)
    {
        int tls_ret = 0;

        if ((tls_ret = mbedtls_client_context(c->tls_session)) < 0)
        {
            LOG_E("[%d] mbedtls_client_context err return : -0x%x", rt_tick_get(), -tls_ret);
            return -RT_ERROR;
        }

        if ((tls_ret = mbedtls_client_connect(c->tls_session)) < 0)
        {
            LOG_E("[%d] mbedtls_client_connect err return : -0x%x", rt_tick_get(), -tls_ret);
            rc = -RT_ERROR;
            goto _exit;
        }
        LOG_D("tls connect success...");

        c->sock = c->tls_session->server_fd.fd;

        /* set recv timeout option */
        setsockopt(c->sock, SOL_SOCKET, SO_RCVTIMEO, (void *)&timeout,
                   sizeof(timeout));
        setsockopt(c->sock, SOL_SOCKET, SO_SNDTIMEO, (void *)&timeout,
                   sizeof(timeout));

        rc = 0;
        goto _exit;
    }
#endif

    if ((c->sock = socket(addr_res->ai_family, SOCK_STREAM, 0)) == -1)
    {
        LOG_E("[%d] create socket error!", rt_tick_get());
        rc = -1;
        _module_state_t state = MODULE_REINIT;
        module_state(&state);
        goto _exit;
    }

    if ((rc = connect(c->sock, addr_res->ai_addr, addr_res->ai_addrlen)) == -1)
    {
        LOG_E("[%d] connect err!", rt_tick_get());
        rc = -2;
        goto _exit;
    }

_exit:
    if (addr_res)
    {
        freeaddrinfo(addr_res);
        addr_res = RT_NULL;
    }
    return rc;
}

static int net_disconnect(MQTTClient *c)
{
#ifdef MQTT_USING_TLS
    if (c->tls_session)
    {
        mbedtls_client_close(c->tls_session);
        c->sock = -1;
    }
#else
    if (c->sock >= 0)
    {
        int i = 0;
    _reclose:
        if ((closesocket(c->sock) != 0) && (i++ < 5))
            goto _reclose;
    }
    c->sock = -1;
#endif
    return 0;
}

static int sendPacket(MQTTClient *c, int length)
{
    int rc;
    struct timeval tv;

    tv.tv_sec = 2000;
    tv.tv_usec = 0;

    setsockopt(c->sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval));

#ifdef MQTT_USING_TLS
    if (c->tls_session)
    {
        rc = mbedtls_client_write(c->tls_session, c->buf, length);
        goto _continue;
    }
#endif

    rc = send(c->sock, c->buf, length, 0);
#ifdef MQTT_USING_TLS
_continue:
#endif
    if (rc == length)
    {
        rc = 0;
    }
    else
    {
        rc = -1;
    }

    return rc;
}

static int net_read(MQTTClient *c, unsigned char *buf, int len, int timeout)
{
    int bytes = 0;
    int rc;

    while (bytes < len)
    {

#ifdef MQTT_USING_TLS
        if (c->tls_session)
        {
            rc = mbedtls_client_read(c->tls_session, &buf[bytes], (size_t)(len - bytes));
            if (rc <= 0)
            {
                bytes = -1;
                break;
            }
            else
            {
                bytes += rc;
            }
            goto _continue;
        }
#endif

        rc = recv(c->sock, &buf[bytes], (size_t)(len - bytes), MSG_DONTWAIT);

        if (rc == -1)
        {
            if (errno != ENOTCONN && errno != ECONNRESET)
            {
                bytes = -1;
                break;
            }
        }
        else
            bytes += rc;
#ifdef MQTT_USING_TLS
    _continue:
#endif
        if (bytes >= len)
        {
            break;
        }

        if (timeout > 0)
        {
            fd_set readset;
            struct timeval interval;

            LOG_D("net_read %d:%d, timeout:%d", rt_tick_get(), bytes, len, timeout);
            timeout = 0;

            interval.tv_sec = 1;
            interval.tv_usec = 0;

            FD_ZERO(&readset);
            FD_SET(c->sock, &readset);

            select(c->sock + 1, &readset, RT_NULL, RT_NULL, &interval);
        }
        else
        {
            LOG_D("net_read %d:%d, break!", rt_tick_get(), bytes, len);
            break;
        }
    }

    return bytes;
}

static int decodePacket(MQTTClient *c, int *value, int timeout)
{
    unsigned char i;
    int multiplier = 1;
    int len = 0;
    const int MAX_NO_OF_REMAINING_LENGTH_BYTES = 4;

    *value = 0;
    do
    {
        int rc = MQTTPACKET_READ_ERROR;

        if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES)
        {
            rc = MQTTPACKET_READ_ERROR; /* bad data */
            goto exit;
        }
        rc = net_read(c, &i, 1, timeout);
        if (rc != 1)
            goto exit;
        *value += (i & 127) * multiplier;
        multiplier *= 128;
    } while ((i & 128) != 0);
exit:
    return len;
}

static int MQTTPacket_readPacket(MQTTClient *c)
{
    int rc = PAHO_FAILURE;
    MQTTHeader header = {0};
    int len = 0;
    int rem_len = 0;

    /* 1. read the header byte.  This has the packet type in it */
    if (net_read(c, c->readbuf, 1, 0) != 1)
        goto exit;

    len = 1;
    /* 2. read the remaining length.  This is variable in itself */
    decodePacket(c, &rem_len, 50);
    len += MQTTPacket_encode(c->readbuf + 1, rem_len); /* put the original remaining length back into the buffer */

    /* 3. read the rest of the buffer using a callback to supply the rest of the data */
    if (rem_len > 0 && (net_read(c, c->readbuf + len, rem_len, 300) != rem_len))
        goto exit;

    header.byte = c->readbuf[0];
    rc = header.bits.type;

exit:
    return rc;
}

static int getNextPacketId(MQTTClient *c)
{
    return c->next_packetid = (c->next_packetid == MAX_PACKET_ID) ? 1 : c->next_packetid + 1;
}

static int MQTTConnect(MQTTClient *c)
{
    int rc = -1, len;
    MQTTPacket_connectData *options = &c->condata;

    if (c->isconnected) /* don't send connect packet again if we are already connected */
    {
        LOG_E("[%d] %s c-> is not connected", rt_tick_get(), __FUNCTION__);
        goto _exit;
    }

    c->keepAliveInterval = options->keepAliveInterval;

    if ((len = MQTTSerialize_connect(c->buf, c->buf_size, options)) <= 0)
    {
        LOG_E("[%d] %s MQTTSerialize_connect fail", rt_tick_get(), __FUNCTION__);
        goto _exit;
    }

    if ((rc = sendPacket(c, len)) != 0) // send the connect packet
    {
        LOG_E("[%d] %s sendPacket fail", rt_tick_get(), __FUNCTION__);
        goto _exit;
    }

    {
        int res;
        fd_set readset;
        struct timeval timeout;

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        FD_ZERO(&readset);
        FD_SET(c->sock, &readset);

        res = select(c->sock + 1, &readset, RT_NULL, RT_NULL, &timeout);

        if (res <= 0)
        {
            LOG_E("[%d] %s wait resp fail, res:%d errno:%d", rt_tick_get(), __FUNCTION__, res, errno);
            rc = -1;
            goto _exit;
        }
    }

    rc = MQTTPacket_readPacket(c);
    if (rc < 0)
    {
        LOG_E("[%d] %s MQTTPacket_readPacket fail", rt_tick_get(), __FUNCTION__);
        goto _exit;
    }

    if (rc == CONNACK)
    {
        unsigned char sessionPresent, connack_rc;

        if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, c->readbuf, c->readbuf_size) == 1)
        {
            rc = connack_rc;
        }
        else
        {
            rc = -1;
        }
    }
    else
        rc = -1;

_exit:
    if (rc == 0)
        c->isconnected = 1;

    return rc;
}

static int MQTTDisconnect(MQTTClient *c)
{
    int rc = PAHO_FAILURE;
    int len = 0;

    len = MQTTSerialize_disconnect(c->buf, c->buf_size);
    if (len > 0)
        rc = sendPacket(c, len); // send the disconnect packet

    c->isconnected = 0;
    c->isQRcodegeted = 0;
    c->isparameterPutted = 0;
    c->isInformed = 0;
    // rt_thread_delay(1000);
    return rc;
}

/**
 * This function subscribe specified mqtt topic.
 *
 * @param c the pointer of MQTT context structure
 * @param topicFilter topic filter name
 * @param qos requested QoS
 *
 * @return the error code, 0 on subscribe successfully.
 */
static int MQTTSubscribe(MQTTClient *c, const char *topicFilter, enum QoS qos)
{
    int rc = PAHO_FAILURE;
    int len = 0;
    int qos_sub = qos;
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)topicFilter;

    if (!c->isconnected)
        goto _exit;
    c->next_packetid = getNextPacketId(c);
    len = MQTTSerialize_subscribe(c->buf, c->buf_size, 0, c->next_packetid, 1, &topic, &qos_sub);
    if (len <= 0)
        goto _exit;
    if ((rc = sendPacket(c, len)) != PAHO_SUCCESS) // send the subscribe packet
        goto _exit;                                // there was a problem

    {
        int res;
        fd_set readset;
        struct timeval timeout;

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        FD_ZERO(&readset);
        FD_SET(c->sock, &readset);

        res = select(c->sock + 1, &readset, RT_NULL, RT_NULL, &timeout);

        if (res <= 0)
        {
            LOG_E("[%d] %s wait resp fail, res:%d errno:%d", rt_tick_get(), __FUNCTION__, res, errno);
            rc = -1;
            goto _exit;
        }
    }
    rc = MQTT_cycle(c);
_exit:
    return rc;
}

static void NewMessageData(MessageData *md, MQTTString *aTopicName, MQTTMessage *aMessage)
{
    md->topicName = aTopicName;
    md->message = aMessage;
}

// assume topic filter and name is in correct format
// # can only be at end
// + and # can only be next to separator
static char isTopicMatched(char *topicFilter, MQTTString *topicName)
{
    char *curf = topicFilter;
    char *curn = topicName->lenstring.data;
    char *curn_end = curn + topicName->lenstring.len;

    while (*curf && curn < curn_end)
    {
        if (*curn == '/' && *curf != '/')
            break;
        if (*curf != '+' && *curf != '#' && *curf != *curn)
            break;
        if (*curf == '+')
        {
            // skip until we meet the next separator, or end of string
            char *nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/')
                nextpos = ++curn + 1;
        }
        else if (*curf == '#')
            curn = curn_end - 1; // skip until end of string
        curf++;
        curn++;
    };

    return (curn == curn_end) && (*curf == '\0');
}

static int deliverMessage(MQTTClient *c, MQTTString *topicName, MQTTMessage *message)
{
    int i;
    int rc = PAHO_FAILURE;

    // we have to find the right message handler - indexed by topic
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
    {
        if (c->messageHandlers[i].topicFilter != 0 && (MQTTPacket_equals(topicName, (char *)c->messageHandlers[i].topicFilter) ||
                                                       isTopicMatched((char *)c->messageHandlers[i].topicFilter, topicName)))
        {
            if (c->messageHandlers[i].callback != NULL)
            {
                MessageData md;
                NewMessageData(&md, topicName, message);
                c->messageHandlers[i].callback(c, &md);
                rc = PAHO_SUCCESS;
            }
        }
    }

    if (rc == PAHO_FAILURE && c->defaultMessageHandler != NULL)
    {
        MessageData md;
        NewMessageData(&md, topicName, message);
        c->defaultMessageHandler(c, &md);
        rc = PAHO_SUCCESS;
    }

    return rc;
}

static int MQTT_cycle(MQTTClient *c)
{
    // read the socket, see what work is due
    int packet_type = MQTTPacket_readPacket(c);

    int len = 0,
        rc = PAHO_SUCCESS;

    if (packet_type == -1)
    {
        rc = PAHO_ERROR;
        LOG_E("[%d] MQTTPacket_readPacket MQTTConnect fail", rt_tick_get());
        goto exit;
    }

    switch (packet_type)
    {
    case CONNACK:
    case PUBACK:
        break;
    case SUBACK:
    {
        int count = 0, grantedQoS = -1;
        unsigned short mypacketid;

        if (MQTTDeserialize_suback(&mypacketid, 1, &count, &grantedQoS, c->readbuf, c->readbuf_size) == 1)
            rc = grantedQoS; // 0, 1, 2 or 0x80

        if (rc != 0x80)
        {
            rc = 0;
        }
    }
    break;
    case PUBLISH:
    {
        MQTTString topicName;
        MQTTMessage msg;
        int intQoS;
        if (MQTTDeserialize_publish(&msg.dup, &intQoS, &msg.retained, &msg.id, &topicName,
                                    (unsigned char **)&msg.payload, (int *)&msg.payloadlen, c->readbuf, c->readbuf_size) != 1)
            goto exit;
        msg.qos = (enum QoS)intQoS;
        deliverMessage(c, &topicName, &msg);
        if (msg.qos != QOS0)
        {
            if (msg.qos == QOS1)
                len = MQTTSerialize_ack(c->buf, c->buf_size, PUBACK, 0, msg.id);
            else if (msg.qos == QOS2)
                len = MQTTSerialize_ack(c->buf, c->buf_size, PUBREC, 0, msg.id);
            if (len <= 0)
                rc = PAHO_FAILURE;
            else
                rc = sendPacket(c, len);
            if (rc == PAHO_FAILURE)
                goto exit; // there was a problem
        }
        break;
    }
    case PUBREC:
    {
        unsigned short mypacketid;
        unsigned char dup, type;
        if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1)
            rc = PAHO_FAILURE;
        else if ((len = MQTTSerialize_ack(c->buf, c->buf_size, PUBREL, 0, mypacketid)) <= 0)
            rc = PAHO_FAILURE;
        else if ((rc = sendPacket(c, len)) != PAHO_SUCCESS) // send the PUBREL packet
            rc = PAHO_FAILURE;                              // there was a problem
        if (rc == PAHO_FAILURE)
            goto exit; // there was a problem
        break;
    }
    case PUBCOMP:
        break;
    case PINGRESP:
        c->tick_ping = rt_tick_get();
        break;
    }

exit:
    return rc;
}
enum ClientSend
{
    SENDINFORM,
    SENDINIT,
    SENDEDINIT,
    SENDPARAMETER,
    SENDPING,
    SENDREALTIME,
    SENDTIMING,
} sendState = SENDINIT;

void current_systime_get(struct tm *ti);

static void paho_mqtt_thread(void *param)
{
    MQTTClient *c = (MQTTClient *)param;
    int i, rc, len = 0;
    rt_uint32_t connect_count = 0, connected_count = 0, disconnect_count = 0;
    rt_uint32_t total, used, max_used;

_mqtt_start:
    if ((module_state(RT_NULL) != MODULE_4G_READY) &&
        (module_state(RT_NULL) != MODULE_WIFI_READY))
    {
        LOG_W("[%d] module is not ready", rt_tick_get());
        rt_thread_delay(rt_tick_from_millisecond(5000));
        goto _net_disconnect;
    }
    connect_count++;
    if (c->connect_callback)
    {
        c->connect_callback(c);
    }

    rc = net_connect(c);
    if (rc != 0)
    {
        LOG_E("[%d] Net connect error(%d)", rt_tick_get(), rc);
        goto _mqtt_restart;
    }
    connected_count++;
    rc = MQTTConnect(c);
    if (rc != 0)
    {
        LOG_E("[%d] MQTT connect error(%d): %s", rt_tick_get(), rc, MQTTSerialize_connack_string(rc));
        goto _mqtt_disconnect;
    }

    // LOG_I("[%d] MQTT server connect success", rt_tick_get());

    for (i = 0; i < MAX_MESSAGE_HANDLERS; i++)
    {
        const char *topic = c->messageHandlers[i].topicFilter;
        enum QoS qos = c->messageHandlers[i].qos;

        if (topic == RT_NULL)
            continue;
        // LOG_I("Subscribe>>Qos:%d,Subscribe:%s", qos, topic);

        rc = MQTTSubscribe(c, topic, qos);
        // LOG_I("Subscribe #%d %s %s!", i, topic, (rc < 0) || (rc == 0x80) ? ("fail") : ("OK"));

        if (rc != 0)
        {
            if (rc == 0x80)
            {
                LOG_E("[%d] QoS config err!", rt_tick_get());
            }
            goto _mqtt_disconnect;
        }
    }

    if (c->online_callback)
    {
        c->online_callback(c);
    }
    c->tick_ping = c->tick_realtime = c->tick_timeing = rt_tick_get();
    while (1)
    {
        int res;
        rt_tick_t tick_now;
        fd_set readset;
        struct timeval timeout;

        if (c->isInformed)
        {
            if (c->isQRcodegeted)
            {
                if (c->isparameterPutted)
                {
                    long tv_sec_temp; /* seconds */
                    tick_now = rt_tick_get();
                    timeout.tv_sec = c->keepAliveInterval - 10 - (tick_now - c->tick_ping) / RT_TICK_PER_SECOND;
                    sendState = SENDPING;
                    /**get interval time right now**/
                    network_get_interval(&c->RealtimeInterval, &c->TimingInterval);
                    tv_sec_temp = c->TimingInterval - (tick_now - c->tick_timeing) / RT_TICK_PER_SECOND;
                    if (tv_sec_temp < timeout.tv_sec)
                    {
                        timeout.tv_sec = tv_sec_temp;
                        sendState = SENDTIMING;
                    }
                    tv_sec_temp = c->RealtimeInterval - (tick_now - c->tick_realtime) / RT_TICK_PER_SECOND;
                    if (tv_sec_temp < timeout.tv_sec)
                    {
                        timeout.tv_sec = tv_sec_temp;
                        sendState = SENDREALTIME;
                    }
                    if (timeout.tv_sec <= 5)
                        timeout.tv_sec = 1;
                }
                else
                {
                    timeout.tv_sec = 1;
                    sendState = SENDPARAMETER;
                }
            }
            else
            {
                if (sendState == SENDEDINIT)
                    timeout.tv_sec = 5;
                else
                {
                    timeout.tv_sec = 1;
                    sendState = SENDINIT;
                }
            }
        }
        else
        {
            timeout.tv_sec = 1;
            sendState = SENDINFORM;
        }

        FD_ZERO(&readset);
        FD_SET(c->sock, &readset);
        timeout.tv_usec = 0;
        // LOG_I("[%d] State:%d timeout.tv_sec: %d ", rt_tick_get(), sendState, timeout.tv_sec);

        /* int select(maxfdp1, readset, writeset, exceptset, timeout); */
        res = select(c->sock + 1, &readset, RT_NULL, RT_NULL, &timeout);
        if (res == 0)
        {
            len = 0;
            switch (sendState)
            {
            case SENDINFORM:
                len = mq_client_publish(c, OTA_INFORM);
                break;
            case SENDPING:
            {
                len = MQTTSerialize_pingreq(c->buf, c->buf_size);
                rc = sendPacket(c, len);
                if (rc != 0)
                {
                    LOG_E("[%d] send ping rc: %d ", rt_tick_get(), rc);
                    goto _mqtt_disconnect;
                }

                /* wait Ping Response. */
                timeout.tv_sec = 5;
                timeout.tv_usec = 0;

                FD_ZERO(&readset);
                FD_SET(c->sock, &readset);

                res = select(c->sock + 1, &readset, RT_NULL, RT_NULL, &timeout);
                if (res <= 0)
                {
                    LOG_E("[%d] wait Ping Response res: %d", rt_tick_get(), res);
                    goto _mqtt_disconnect;
                }
                goto __receive_;
            } /* res == 0: timeount for ping. */
            // break;
            case SENDEDINIT:
            case SENDINIT:
                len = mq_client_publish(c, PLATFORM_INIT);
                break;
            case SENDPARAMETER:
                len = mq_client_publish(c, PARAMETER_PUT);
                break;
            case SENDREALTIME:
                disconnect_count++;
                goto _mqtt_disconnect;

                len = mq_client_publish(c, REALTIME_REPORT);
                c->tick_realtime = rt_tick_get();
                break;
            case SENDTIMING:
                len = mq_client_publish(c, TIMING_REPORT);
                c->tick_timeing = rt_tick_get();
                break;
            default:
                break;
            }
            if (len <= 0)
            {
                LOG_I("[%d] MQTTSerialize_publish sendPacket rc: %d", rt_tick_get(), rc);
                goto _mqtt_disconnect;
            }

            if ((rc = sendPacket(c, len)) != PAHO_SUCCESS) // send the subscribe packet
            {
                LOG_E("[%d] MQTTSerialize_publish sendPacket rc: %d", rt_tick_get(), rc);
                goto _mqtt_disconnect;
            }

            /* wait Ping Response. */
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;

            FD_ZERO(&readset);
            FD_SET(c->sock, &readset);

            res = select(c->sock + 1, &readset, RT_NULL, RT_NULL, &timeout);
            if (res <= 0)
            {
                LOG_E("[%d] wait publish Response res: %d", rt_tick_get(), res);
                goto _mqtt_disconnect;
            }
            if (sendState == SENDINFORM)
                c->isInformed = 1;
            if (sendState == SENDPARAMETER)
                c->isparameterPutted = 1;
            if (sendState != SENDEDINIT)
                sendState++;
        }

    __receive_:
        if (res < 0)
        {
            LOG_E("[%d] select res: %d", rt_tick_get(), res);
            goto _mqtt_disconnect;
        }
        if (FD_ISSET(c->sock, &readset))
        {
            //LOG_D("sock FD_ISSET");
            rc = MQTT_cycle(c);
            //LOG_D("[%d] sock FD_ISSET rc : %d, rt_tick_get(), rc);
            if (rc < 0)
                goto _mqtt_disconnect;

            continue;
        }
        if ((module_state(RT_NULL) != MODULE_4G_READY) &&
            (module_state(RT_NULL) != MODULE_WIFI_READY))
        {
            LOG_W("[%d] module is need restart", rt_tick_get());
            goto _mqtt_disconnect;
        }
    } /* while (1) */

_mqtt_disconnect:
    MQTTDisconnect(c);
_mqtt_restart:
    if (c->offline_callback)
    {
        c->offline_callback(c);
    }
_net_disconnect:
    net_disconnect(c);
    if (module_state(RT_NULL) == MODULE_REINIT)
    {
        LOG_W("[%d] module is need restart", rt_tick_get());
        rt_sem_release(module_setup_sem);
    }
    rt_memory_info(&total, &used, &max_used);
    LOG_I("total:%d,used:%d,max_used:%d", total, used, max_used);
    struct tm ti;
    current_systime_get(&ti);
    LOG_I("%04d-%02d-%02d %02d:%02d:%02d connect:%d connected:%d disconnect:%d",
          ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec, connect_count, connected_count, disconnect_count);

    rt_thread_delay(RT_TICK_PER_SECOND * 5);
    goto _mqtt_start;

    goto _mqtt_disconnect_exit;
_mqtt_disconnect_exit:
    MQTTDisconnect(c);
    net_disconnect(c);

    goto _mqtt_exit;
_mqtt_exit:
    LOG_D("thread exit");
    return;
}

int paho_mqtt_start(MQTTClient *client)
{
    rt_err_t result;
    rt_thread_t tid;
    int stack_size = RT_PKG_MQTT_THREAD_STACK_SIZE;
    int priority = RT_THREAD_PRIORITY_MAX / 3;
    char *stack;

    static int is_started = 0;
    if (is_started)
    {
        LOG_D("paho mqtt has already started!");
        return 0;
    }

    tid = rt_malloc(RT_ALIGN(sizeof(struct rt_thread), 8) + stack_size);
    if (!tid)
    {
        LOG_E("[%d] no memory for thread: MQTT");
        return -1;
    }

    stack = (char *)tid + RT_ALIGN(sizeof(struct rt_thread), 8);
    result = rt_thread_init(tid,
                            "MQTT",
                            paho_mqtt_thread, client, // fun, parameter
                            stack, stack_size,        // stack, size
                            priority, 2               //priority, tick
    );

    if (result == RT_EOK)
    {
        rt_thread_startup(tid);
        is_started = 1;
    }

    return 0;
}

static int MQTT_local_send(MQTTClient *c, const void *data, int len)
{
    int send_len;

    send_len = write(c->pub_pipe[1], data, len);

    return send_len;
}

/*
MQTT_CMD:
"DISCONNECT"
*/
int MQTT_CMD(MQTTClient *c, const char *cmd)
{
    char *data = 0;
    int cmd_len, len;
    int rc = PAHO_FAILURE;

    if (!c->isconnected)
        goto _exit;

    cmd_len = strlen(cmd) + 1;
    if (cmd_len >= sizeof(MQTTMessage))
    {
        LOG_E("[%d] cmd too loog %d:", rt_tick_get(), cmd_len, sizeof(MQTTMessage));
        goto _exit;
    }

    data = rt_malloc(cmd_len);
    if (!data)
        goto _exit;

    strcpy(data, cmd);
    len = MQTT_local_send(c, data, cmd_len);
    if (len == cmd_len)
    {
        rc = 0;
    }

_exit:
    if (data)
        rt_free(data);

    return rc;
}

/**
 * This function publish message to specified mqtt topic.
 * [MQTTMessage] + [payload] + [topic] + '\0'
 *
 * @param c the pointer of MQTT context structure
 * @param topicFilter topic filter name
 * @param message the pointer of MQTTMessage structure
 *
 * @return the error code, 0 on subscribe successfully.
 */
int MQTTPublish(MQTTClient *c, const char *topicName, MQTTMessage *message)
{
    int rc = PAHO_FAILURE;
    int len, msg_len;
    char *data = 0;

    if (!c->isconnected)
        goto exit;

    msg_len = sizeof(MQTTMessage) + message->payloadlen + strlen(topicName) + 1;
    data = rt_malloc(msg_len);
    if (!data)
        goto exit;

    memcpy(data, message, sizeof(MQTTMessage));
    memcpy(data + sizeof(MQTTMessage), message->payload, message->payloadlen);
    strcpy(data + sizeof(MQTTMessage) + message->payloadlen, topicName);

    len = MQTT_local_send(c, data, msg_len);
    if (len == msg_len)
    {
        rc = 0;
    }
    //LOG_D("MQTTPublish sendto %d", len);

exit:
    if (data)
        rt_free(data);

    return rc;
}

static int mq_client_publish(MQTTClient *c, _topic_pub_enmu_t pub_type)
{
    int rc = PAHO_FAILURE;
    MQTTMessage message;
    char *msg_str = RT_NULL;
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)iot_pub_topics[pub_type].topic_str;
    if (!c->isconnected)
        goto exit;
    switch (pub_type)
    {
    case OTA_INFORM: /*{"IOT_OTA_INFORM"}*/
        network_Serialize_inform_json(&msg_str);
        break;
    case PLATFORM_INIT: /*{"TOPIC_PLATFORM_INIT"}*/
        network_Serialize_init_json(&msg_str);
        break;
    case WATER_STATUS: /*{"TOPIC_WATER_STATUS"}*/
        break;
    case PARAMETER_PUT: /*{"TOPIC_PARAMETER_PUT"}*/
        network_Serialize_para_json(&msg_str);
        break;
    case REALTIME_REPORT: /*{"TOPIC_REALTIME_REPORT"}*/
        network_Serialize_report_json(&msg_str, pub_type);
        break;
    case TIMING_REPORT: /*{"TOPIC_TIMING_REPORT"}*/
        network_Serialize_report_json(&msg_str, pub_type);
        break;
    case DEVICE_UPGRADE: /*{"TOPIC_DEVICE_UPGRADE"}*/
        break;
    default:
        break;
    }
    if (msg_str == RT_NULL)
        goto exit;
    message.qos = iot_pub_topics[pub_type].qos;
    message.retained = iot_pub_topics[pub_type].restained;
    message.dup = iot_pub_topics[pub_type].dup;
    message.payload = (void *)msg_str;
    message.payloadlen = strlen(message.payload);
    message.id = getNextPacketId(c);
    rc = MQTTSerialize_publish(c->buf, c->buf_size, message.dup, message.qos, message.retained, message.id,
                               topic, (unsigned char *)message.payload, message.payloadlen);
    rt_free(msg_str);
exit:
    return rc;
}
