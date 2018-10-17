/**
 ****************************************************************************
 * @Warning :Without permission from the author,Not for commercial use
 * @File    :
 * @Author  :Seblee
 * @date    :2018-09-06 17:14:15
 * @version :V 1.0.0
 *************************************************
 * @brief   :
 ****************************************************************************
 * @Last Modified by: Seblee
 * @Last Modified time: 2018-10-08 18:02:28
 ****************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "network.h"
#include "transport.h"
#include "mqtt_client.h"
#include "at_transfer.h"
#include "cJSON.h"
#include "utils_md5.h"
#include "disguise_time.h"
#include "SIMCOM_AT.h"
/* Private typedef -----------------------------------------------------------*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};

/* Private define ------------------------------------------------------------*/
#ifndef network_log
#define network_log(N, ...) rt_kprintf("####[network %s:%4d] " N "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif /* at_log(...) */
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
rt_device_t write_device;
/* 用于接收消息的消息队列*/
rt_mq_t rx_mq;
/***push command ***/
rt_mq_t publish_mq = RT_NULL;
/* 接收线程的接收缓冲区*/
//static char uart_rx_buffer[64];
rt_uint8_t write_buffer[MSG_LEN_MAX];
rt_uint8_t read_buffer[MSG_LEN_MAX];

const char iot_deviceid[] = {DEVICE_ID};
const char iot_devicename[] = {DEVICE_NAME};
const char iot_productKey[] = {PRODUCT_KEY};
const char iot_secret[] = {DEVICE_SECRET};

volatile _iot_state_t iot_state = IOT_POWERON;
/********topic dup qos restained**************/
iot_topic_param_t iot_topics[] = {
    {TOPIC_PLATFORM_INIT, 0, 1, 0},   /*{"TOPIC_PLATFORM_INIT"}*/
    {TOPIC_WATER_NOTICE, 0, 1, 0},    /*{"TOPIC_WATER_NOTICE"}*/
    {TOPIC_WATER_STATUS, 0, 1, 0},    /*{"TOPIC_WATER_STATUS"}*/
    {TOPIC_PARAMETER_SET, 0, 1, 0},   /*{"TOPIC_PARAMETER_SET"}*/
    {TOPIC_PARAMETER_GET, 0, 1, 0},   /*{"TOPIC_PARAMETER_GET"}*/
    {TOPIC_PARAMETER_PUT, 0, 1, 0},   /*{"TOPIC_PARAMETER_PUT"}*/
    {TOPIC_REALTIME_REPORT, 0, 1, 0}, /*{"TOPIC_REALTIME_REPORT"}*/
    {TOPIC_TIMING_REPORT, 0, 1, 0},   /*{"TOPIC_TIMING_REPORT"}*/
    {TOPIC_DEVICE_UPGRADE, 0, 1, 0},  /*{"TOPIC_DEVICE_UPGRADE"}*/
    {TOPIC_DEVICE_MOVE, 0, 1, 0},     /*{"TOPIC_DEVICE_MOVE"}*/
    {TOPIC_DEVICE_UPDATE, 0, 1, 0},   /*{"TOPIC_DEVICE_UPDATE"}*/
    {TOPIC_DEVICE_ERR, 0, 1, 0},      /*{"TOPIC_DEVICE_ERR"}*/
    {TOPIC_DEVICE_GET, 0, 1, 0},      /*{"TOPIC_DEVICE_GET"}*/
};
/* Private function prototypes -----------------------------------------------*/
iotx_device_info_t device_info;
iotx_conn_info_t device_connect;
MQTTPacket_connectData client_con = MQTTPacket_connectData_initializer;
SIMCOM_HANDLE g_SIMCOM_Handle; //SIMCOM通信模块句柄
/* Private functions ---------------------------------------------------------*/

/* 数据到达回调函数*/
void NetWork_DIR_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = SIM7600_DIR_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(SIM7600_DIR_PORT, &GPIO_InitStructure);
    SIM7600_DIR_WIFI;
}
rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    msg.dev = dev;
    msg.size = size;
    /* 发送消息到消息队列中*/
    rt_mq_send(rx_mq, &msg, sizeof(struct rx_msg));
    return RT_EOK;
}

void sim7600_thread_entry(void *parameter)
{
    rt_err_t result = RT_EOK;
    rt_uint8_t count = 0;
    rt_uint16_t realtime_interval, timing_interval, realtime_count, timing_count;
    _iot_state_t pub_msg;

    rx_mq = rt_mq_create("7600_rx_mq", sizeof(struct rx_msg), 5, RT_IPC_FLAG_FIFO);
    publish_mq = rt_mq_create("publish_mq", sizeof(_iot_state_t), 7, RT_IPC_FLAG_FIFO);
    NetWork_DIR_Init();
    struct tm ti;
    get_bulid_date_time(&ti);
    current_systime_set(&ti);
    rt_thread_delay(SIM7600_THREAD_DELAY);
    device_connect.style = IOT_4G_MODE;

    network_log("priject build time:%04d-%02d-%d %02d:%02d:%02d", ti.tm_year + 1900, ti.tm_mon, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
    write_device = rt_device_find("uart3");
    if (write_device != RT_NULL)
    {
        rt_device_set_rx_indicate(write_device, uart_input);
        rt_device_open(write_device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
    if (device_connect.style == IOT_WIFI_MODE)
        result = at_wifi_init(write_device);
    else if (device_connect.style == IOT_4G_MODE)
        result = at_4g_init(write_device);
    mqtt_client_init(write_device);
    network_log("mqtt_client_init done");

    transport_open(write_device, &device_connect);
    mqtt_client_connect(write_device, &client_con);
    result = mqtt_client_subscribe_topics();
    // transport_close(write_device);
    network_get_interval(&realtime_interval, &timing_interval);

    iot_state = IOT_PLATFORM_INIT;
    while (1)
    {
        result = mqtt_packet_read_operation();
        if (realtime_count++ >= realtime_interval)
        {
            realtime_count = 0;
            pub_msg = IOT_REALTIME_REPORT;
            rt_mq_send(publish_mq, &pub_msg, sizeof(_iot_state_t));
        }
        if (timing_count++ >= timing_interval)
        {
            timing_count = 0;
            pub_msg = IOT_TIMING_REPORT;
            rt_mq_send(publish_mq, &pub_msg, sizeof(_iot_state_t));
        }
        if (iot_state == IOT_IDEL)
        {
            result = rt_mq_recv(publish_mq, &pub_msg, sizeof(_iot_state_t), 0);
            if (result == RT_EOK)
            {
                iot_state = pub_msg;
            }
        }
        network_log("iot_state=%d", iot_state);
        switch (iot_state)
        {
        case IOT_POWERON:
            break;
        case IOT_PLATFORM_INIT:
            result = mqtt_client_publish_topics();
            if (result == RT_EOK)
                iot_state = IOT_INIT_COMPL;
            break;
        case IOT_INIT_COMPL: /***WATI FOR QR Code topic***/
            break;
        case IOT_PARAM_REPORT:
            result = mqtt_client_publish_parameter();
            iot_state = IOT_IDEL;
            break;
        case IOT_REALTIME_REPORT:
            mqtt_client_publish_report(REALTIME_REPORT);
            iot_state = IOT_IDEL;
            break;
        case IOT_TIMING_REPORT:
            mqtt_client_publish_report(TIMING_REPORT);
            iot_state = IOT_IDEL;
            break;
        case IOT_DEVICE_UPGRADE:
            break;
        case IOT_IDEL:
            break;
        default:
            break;
        }
        if (count++ >= 10)
        {
            count = 0;
            result = mqtt_client_ping();
            continue;
        }
    }
}
rt_uint32_t network_send_message(rt_device_t dev, const char *senddata, rt_uint8_t **data)
{
    rt_uint16_t timeout = 9000;
    rt_uint32_t count = 0;
    struct rx_msg msg;
    rt_err_t result = RT_EOK;
    rt_uint32_t rx_length;
    if (senddata)
        rt_device_write(dev, 0, senddata, strlen(senddata));

    if (*data != RT_NULL)
    {
        rt_kprintf("*data != RT_NULL\r\n");
        rt_free(*data);
        *data = RT_NULL;
    }
    while (1)
    {
        result = rt_mq_recv(rx_mq, &msg, sizeof(struct rx_msg), timeout);
        if (result == RT_EOK)
        {
            timeout = 100;
            rx_length = msg.size;
            if (*data == RT_NULL)
                *data = (rt_uint8_t *)rt_calloc(rx_length + 1, sizeof(rt_uint8_t));
            else
                *data = (rt_uint8_t *)rt_realloc(*data, rx_length + count + 1);
            if (*data == RT_NULL)
                goto exit;
            rx_length = rt_device_read(msg.dev, 0, (*data + count), rx_length);
            count += rx_length;
        }
        if (result == -RT_ETIMEOUT)
            break;
    }
    return count;
exit:
    rt_kprintf("err count:%d\r\n", count);
    if (*data != RT_NULL)
    {
        rt_free(*data);
        *data = RT_NULL;
    }
    return 0;
}

/**
 ****************************************************************************
 * @Function : rt_int32_t network_read_message(rt_device_t dev, rt_uint8_t *data, rt_int16_t len, rt_int32_t timeout)
 * @File     : network.c
 * @Program  : dev:serial device
 *             *data:the buff read
 *             len:the length read
 * @Created  : 2017/12/12 by seblee
 * @Brief    : 
 * @Version  : V1.0
**/
rt_int32_t network_read_message(rt_device_t dev, rt_uint8_t *data, rt_int16_t len, rt_int32_t timeout)
{
    struct rx_msg msg;
    rt_err_t result = RT_EOK;
    rt_uint32_t rx_length, count = 0;
    rt_int32_t timer = timeout;

    if (data == RT_NULL)
        return -RT_ENOMEM;
    while (1)
    {
        result = rt_mq_recv(rx_mq, &msg, sizeof(struct rx_msg), timer);
        if (result == RT_EOK)
        {
            timer = 50;
            rx_length = msg.size;
            rx_length = rx_length > (len - count) ? (len - count) : rx_length;
            rx_length = rt_device_read(msg.dev, 0, data + count, rx_length);
            count += rx_length;
            if (count == len) //complate data count
                break;
        }
        if (result == -RT_ETIMEOUT) //超时50ms 判断完成一次 数据传输
            break;
    }
    return count;
}
/**
 ****************************************************************************
 * @Function : void network_Serialize_init_json(char **datapoint)
 * @File     : network.c
 * @Program  : databuf:point of databuffer
 *             len:sizeof databuf
 * @Created  : 2018-09-20 by seblee
 * @Brief    : 
 * @Version  : V1.0
**/
void network_Serialize_init_json(char **datapoint)
{
    char sign_hex[128] = {0};
    unsigned char sign[16];

    char RequestNoStr[10] = {0};
    unsigned short msgid;
    int i;

    /* declare a few. */
    cJSON *root = NULL, *result;
    msgid = mqtt_client_packet_id();
    /* Our "Video" datatype: */
    root = cJSON_CreateObject();

    result = cJSON_AddStringToObject(root, "MCode", "001");
    if (result == NULL)
        network_log("JSON add err");

    rt_snprintf(RequestNoStr, sizeof(RequestNoStr), "%d", msgid);
    result = cJSON_AddStringToObject(root, "RequestNo", RequestNoStr);
    result = cJSON_AddStringToObject(root, "ProductKey", PRODUCT_KEY);
    result = cJSON_AddStringToObject(root, "DeviceName", DEVICE_NAME);
    result = cJSON_AddStringToObject(root, "Timestamp", "20180720115800");

    rt_snprintf(sign_hex, sizeof(sign_hex), "DeviceName=%s&MCode=001&ProductKey=%s&RequestNo=%s&Timestamp=20180720115800&Key=123456", DEVICE_NAME, PRODUCT_KEY, RequestNoStr);
    utils_md5((const unsigned char *)sign_hex, strlen(sign_hex), sign);
    network_log("MD5(%s)", sign_hex);
    rt_memset(sign_hex, 0, sizeof(sign_hex));
    for (i = 0; i < 16; ++i)
    {
        sign_hex[i * 2] = utils_hb2hex(sign[i] >> 4);
        sign_hex[i * 2 + 1] = utils_hb2hex(sign[i]);
    }
    network_log("MD5=%s", sign_hex);
    cJSON_AddItemToObject(root, "Sign", cJSON_CreateString(sign_hex));
    *datapoint = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (*datapoint)
        network_log("JSON len:%d,string:%s", strlen(*datapoint), *datapoint);
}

/**
 ****************************************************************************
 * @Function : void network_Serialize_para_json(char **datapoint)
 * @File     : network.c
 * @Program  : the point of out data
 * @Created  : 2018-09-21 by seblee
 * @Brief    : serialize parameter json report
 * @Version  : V1.0
**/
#include "mb_event_cpad.h"
void network_Serialize_para_json(char **datapoint)
{
    char sign_hex[33] = {0};
    unsigned char sign[16];
    char sign_Cache[800] = {0};

    char StrCache[512] = {0};
    unsigned short msgid;
    int i;

    /* declare a few. */
    cJSON *root = NULL;
    msgid = mqtt_client_packet_id();
    /* Our "Video" datatype: */
    root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "MCode", "007");
    rt_snprintf(StrCache, 10, "%d", msgid);
    cJSON_AddStringToObject(root, "RequestNo", StrCache);
    struct tm ti;
    char Timestamp_str[15] = {0};
    current_systime_get(&ti);
    rt_snprintf(Timestamp_str, sizeof(Timestamp_str), "%04d%02d%02d%02d%02d%02d",
                ti.tm_year + 1900, ti.tm_mon, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);

    cJSON_AddStringToObject(root, "Timestamp", Timestamp_str);
    cJSON_AddStringToObject(root, "Settingversion", Timestamp_str);
    cJSON_AddStringToObject(root, "Setaddrstart", "100");

    cpad_eMBRegHoldingCB((unsigned char *)sign_Cache, 100, 80, CPAD_MB_REG_READ);
    for (i = 0; i < 160; ++i)
    {
        StrCache[i * 2] = utils_hb2hex(sign_Cache[i] >> 4);
        StrCache[i * 2 + 1] = utils_hb2hex(sign_Cache[i]);
    }

    cJSON_AddStringToObject(root, "Settingmsg", StrCache);
    cJSON_AddStringToObject(root, "Settingleng", "80");
    cJSON_AddStringToObject(root, "ProductKey", PRODUCT_KEY);
    cJSON_AddStringToObject(root, "DeviceName", DEVICE_NAME);

    rt_snprintf(sign_Cache, sizeof(sign_Cache), "DeviceName=%s&MCode=007&ProductKey=%s&RequestNo=%d&Setaddrstart=100&Settingleng=80&Settingmsg=%s&Settingversion=%s&Timestamp=%s&Key=123456",
                DEVICE_NAME, PRODUCT_KEY, msgid, StrCache, Timestamp_str, Timestamp_str);

    utils_md5((const unsigned char *)sign_Cache, strlen(sign_Cache), sign);
    network_log("MD5");
    rt_kprintf("MD5(%.400s", sign_Cache);
    rt_kprintf("%s)\r\n", sign_Cache + 400);
    rt_memset(sign_hex, 0, sizeof(sign_hex));
    for (i = 0; i < 16; ++i)
    {
        sign_hex[i * 2] = utils_hb2hex(sign[i] >> 4);
        sign_hex[i * 2 + 1] = utils_hb2hex(sign[i]);
    }
    network_log("MD5=%s", sign_hex);
    cJSON_AddItemToObject(root, "Sign", cJSON_CreateString(sign_hex));
    *datapoint = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (*datapoint)
    {
        network_log("JSON len:%d,string:%s", strlen(*datapoint));
        rt_kprintf("string:%.400s", *datapoint);
        rt_kprintf("%s\r\n", *datapoint + 400);
    }
}
/**
 ****************************************************************************
 * @Function : rt_err_t network_water_notice_parse(const char *Str)
 * @File     : network.c
 * @Program  : none
 * @Created  : 2018-09-25 by seblee
 * @Brief    : 
 * @Version  : V1.0
**/
rt_err_t network_water_notice_parse(const char *Str)
{
    rt_err_t rc;
    cJSON *root = RT_NULL;
    network_log("Str:%s", Str);
    root = cJSON_Parse(Str);
    if (!root)
    {
        network_log("get root faild !\n");
        rc = -1;
    }
    else
    {
        cJSON *js_MCode = cJSON_GetObjectItem(root, "MCode");
        if (!js_MCode)
        {
            network_log("get MCode faild !\n");
            rc = -1;
            goto exit;
        }
        int MCode_value = 0;
        sscanf(js_MCode->valuestring, "%d", &MCode_value);
        network_log("MCode_value:%d !\n", MCode_value);
        if (MCode_value == MCode_QRCODE_GENERATE)
        {
            network_log("get QRCode !!!");
            iot_state = IOT_PARAM_REPORT;
        }
        rc = RT_EOK;
    }
exit:
    if (root)
        cJSON_Delete(root);
    return rc;
}

/**
 ****************************************************************************
 * @Function : void network_Serialize_report_json(char **datapoint, _topic_enmu_t topic_type)
 * @File     : network.c
 * @Program  : p:the point of data point
 * @Created  : 2018-09-26 by seblee
 * @Brief    : serialize reoprt json
 * @Version  : V1.0
**/
void network_Serialize_report_json(char **datapoint, rt_uint8_t topic_type)
{
    char sign_hex[33] = {0};
    unsigned char sign[16];
    char sign_Cache[800] = {0};

    char StrCache[512] = {0};
    unsigned short msgid;
    int i;

    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "DeviceName", DEVICE_NAME);
    cJSON_AddStringToObject(root, "ProductKey", PRODUCT_KEY);
    if (topic_type == REALTIME_REPORT)
        cJSON_AddStringToObject(root, "MCode", "008");
    else
        cJSON_AddStringToObject(root, "MCode", "009");

    msgid = mqtt_client_packet_id();
    rt_snprintf(StrCache, 10, "%d", msgid);
    cJSON_AddStringToObject(root, "RequestNo", StrCache);
    struct tm ti;
    char Timestamp_str[15] = {0};
    current_systime_get(&ti);
    rt_snprintf(Timestamp_str, sizeof(Timestamp_str), "%04d%02d%02d%02d%02d%02d",
                ti.tm_year + 1900, ti.tm_mon, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);

    cJSON_AddStringToObject(root, "Timestamp", Timestamp_str);
    if (topic_type == REALTIME_REPORT)
        cJSON_AddStringToObject(root, "Statusaddrstart", "502");
    else
        cJSON_AddStringToObject(root, "Statusaddrstart", "500");

    rt_memset(StrCache, 0, sizeof(StrCache));
    if (topic_type == REALTIME_REPORT)
    {
        cpad_eMBRegHoldingCB((unsigned char *)sign_Cache, 502, 18, CPAD_MB_REG_READ);
        for (i = 0; i < 36; ++i)
        {
            StrCache[i * 2] = utils_hb2hex(sign_Cache[i] >> 4);
            StrCache[i * 2 + 1] = utils_hb2hex(sign_Cache[i]);
        }
    }
    else
    {
        cpad_eMBRegHoldingCB((unsigned char *)sign_Cache, 500, 50, CPAD_MB_REG_READ);
        for (i = 0; i < 100; ++i)
        {
            StrCache[i * 2] = utils_hb2hex(sign_Cache[i] >> 4);
            StrCache[i * 2 + 1] = utils_hb2hex(sign_Cache[i]);
        }
    }

    cJSON_AddStringToObject(root, "Statusmsg", StrCache);
    if (topic_type == REALTIME_REPORT)
        cJSON_AddStringToObject(root, "Statusleng", "18");
    else
        cJSON_AddStringToObject(root, "Statusleng", "50");

    if (topic_type == REALTIME_REPORT)
        rt_snprintf(sign_Cache, sizeof(sign_Cache), "DeviceName=%s&MCode=008&ProductKey=%s&RequestNo=%d&Statusaddrstart=502&Statusleng=18&Statusmsg=%s&Timestamp=%s&Key=123456", DEVICE_NAME, PRODUCT_KEY, msgid, StrCache, Timestamp_str);
    else
        rt_snprintf(sign_Cache, sizeof(sign_Cache), "DeviceName=%s&MCode=009&ProductKey=%s&RequestNo=%d&Statusaddrstart=500&Statusleng=50&Statusmsg=%s&Timestamp=%s&Key=123456", DEVICE_NAME, PRODUCT_KEY, msgid, StrCache, Timestamp_str);

    utils_md5((const unsigned char *)sign_Cache, strlen(sign_Cache), sign);
    // network_log("MD5(%s)", sign_Cache);
    rt_memset(sign_hex, 0, sizeof(sign_hex));
    for (i = 0; i < 16; ++i)
    {
        sign_hex[i * 2] = utils_hb2hex(sign[i] >> 4);
        sign_hex[i * 2 + 1] = utils_hb2hex(sign[i]);
    }
    network_log("MD5=%s", sign_hex);
    cJSON_AddItemToObject(root, "Sign", cJSON_CreateString(sign_hex));
    *datapoint = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (*datapoint)
        network_log("JSON len:%d,string:%s", strlen(*datapoint), *datapoint);
}
/**
 ****************************************************************************
 * @Function : void network_get_interval(rt_uint16_t *real, rt_uint16_t *timing)
 * @File     : network.c
 * @Program  : none
 * @Created  : 2018-09-27 by seblee
 * @Brief    : get interval time
 * @Version  : V1.0
**/
void network_get_interval(rt_uint16_t *real, rt_uint16_t *timing)
{
    *real = REALTIME_INTERVAL_DEFAULT;
    *timing = TIMING_INTERVAL_DEFAULT;
    cpad_eMBRegHoldingCB((unsigned char *)write_buffer, 165, 2, CPAD_MB_REG_READ);
    rt_uint32_t interval_temp = (write_buffer[0] << 8) | write_buffer[1];
    interval_temp *= 60;
    if (interval_temp > TIMING_INTERVAL_MAX)
        interval_temp = TIMING_INTERVAL_MAX;
    if (interval_temp < TIMING_INTERVAL_MIN)
        interval_temp = TIMING_INTERVAL_MIN;
    *timing = interval_temp;
    network_log("timing:%d", *timing);

    interval_temp = (write_buffer[2] << 8) | write_buffer[3];
    if (interval_temp > REALTIME_INTERVAL_MAX)
        interval_temp = REALTIME_INTERVAL_MAX;
    if (interval_temp < REALTIME_INTERVAL_MIN)
        interval_temp = REALTIME_INTERVAL_MIN;
    *real = interval_temp;
    network_log("real:%d", *real);
}

/**
 ****************************************************************************
 * @Function : rt_err_t network_parameter_check(rt_err_t)
 * @File     : network.c
 * @Program  : none
 * @Created  : 2018-09-27 by seblee
 * @Brief    : 
 * @Version  : V1.0
**/
rt_err_t network_parameter_get_parse(const char *Str)
{
    rt_err_t rc;
    int MCode_value;
    cJSON *root = RT_NULL;
    network_log("Str:%s", Str);
    root = cJSON_Parse(Str);
    if (!root)
    {
        network_log("get root faild !\n");
        rc = -1;
    }
    else
    {
        cJSON *js_MCode = cJSON_GetObjectItem(root, "MCode");
        sscanf(js_MCode->valuestring, "%d", &MCode_value);
        network_log("MCode_value:%d !\n", MCode_value);
        if (MCode_value == MCode_PARAMETER_GET)
        {
            network_log("get PARAMETER_GET !!!");
            _iot_state_t pub_msg = IOT_PARAM_REPORT;
            rt_mq_send(publish_mq, &pub_msg, sizeof(_iot_state_t));
        }
        rc = RT_EOK;
    }

    if (root)
        cJSON_Delete(root);
    return rc;
}
/**
 ****************************************************************************
 * @Function : rt_err_t network_parameter_set_parse(const char*Str)
 * @File     : network.c
 * @Program  : none
 * @Created  : 2018-09-28 by seblee
 * @Brief    : 
 * @Version  : V1.0
**/
rt_err_t network_parameter_set_parse(const char *Str)
{
    rt_err_t rc;
    int MCode_value;
    cJSON *root = RT_NULL;
    network_log("Str:%s", Str);
    root = cJSON_Parse(Str);
    if (!root)
    {
        network_log("get root faild !\n");
        rc = -1;
    }
    else
    {
        cJSON *js_MCode = cJSON_GetObjectItem(root, "MCode");
        sscanf(js_MCode->valuestring, "%d", &MCode_value);
        network_log("MCode_value:%d !\n", MCode_value);
        if (MCode_value == MCode_PARAMETER_SET)
        {
            network_log("get PARAMETER_SET !!!");
            int Setaddrstart;
            cJSON *js_Setaddrstart = cJSON_GetObjectItem(root, "Setaddrstart");
            if (js_Setaddrstart == RT_NULL)
            {
                rc = -RT_ERROR;
                network_log("get js_Setaddrstart err !!!");
            }
            sscanf(js_Setaddrstart->valuestring, "%d", &Setaddrstart);
            network_log("get Setaddrstart:%d", Setaddrstart);
            int Settingleng;
            cJSON *js_Settingleng = cJSON_GetObjectItem(root, "Settingleng");
            if (js_Settingleng == RT_NULL)
            {
                rc = -RT_ERROR;
                network_log("get js_Settingleng err !!!");
            }
            sscanf(js_Settingleng->valuestring, "%d", &Settingleng);
            network_log("get Settingleng:%d", Settingleng);
            cJSON *js_Settingmsg = cJSON_GetObjectItem(root, "Settingmsg");
            if (js_Settingmsg == RT_NULL)
            {
                rc = -RT_ERROR;
                network_log("get js_Settingmsg err !!!");
            }
            char *Settingmsg_str = rt_calloc(Settingleng * 4 + 1, sizeof(rt_uint8_t));
            if (Settingmsg_str == RT_NULL)
            {
                rc = -RT_ERROR;
                network_log("get Settingmsg_str err !!!");
            }
            int Settingmsg_data;
            if ((js_Settingmsg != RT_NULL) && (Settingmsg_str != RT_NULL))
            {
                rt_strncpy(Settingmsg_str, js_Settingmsg->valuestring, Settingleng * 4);
                network_log("get Settingmsg_str:%s", Settingmsg_str);
                int i;
                unsigned char *Settingmsg_data_buf = rt_calloc(Settingleng * 2, sizeof(rt_uint8_t));

                for (i = 0; i < Settingleng * 2; i++)
                {
                    sscanf(Settingmsg_str + 2 * i, "%02X", &Settingmsg_data);
                    Settingmsg_data_buf[i] = (uint8_t)(Settingmsg_data & (int)0xFF);
                    rt_kprintf("%02x", Settingmsg_data_buf[i]);
                }

                cpad_eMBRegHoldingCB((unsigned char *)Settingmsg_data_buf, Setaddrstart, Settingleng, CPAD_MB_REG_MULTIPLE_WRITE);
                if (Settingmsg_data_buf)
                    rt_free(Settingmsg_data_buf);
            }
            if (Settingmsg_str)
                rt_free(Settingmsg_str);
            rc = RT_EOK;
        }
        else
            rc = -RT_ERROR;
    }

    if (root)
        cJSON_Delete(root);
    return rc;
}
