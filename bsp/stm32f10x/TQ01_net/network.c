/**
 ****************************************************************************
 * @Warning :Without permission from the author,Not for commercial use
 * @File    :
 * @Author  :Seblee
 * @date    :2018-11-21 10:40:27
 * @version : V1.0.0
 *************************************************
 * @brief :
 ****************************************************************************
 * @Last Modified by: Seblee
 * @Last Modified time: 2018-11-21 16:05:07
 ****************************************************************************
**/

/* Private include -----------------------------------------------------------*/
#include "network.h"
#include "mqtt_client.h"
#include "disguise_time.h"

#include "cJSON.h"
#include "utils_md5.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

//#define CONFIG_DEBUG
#ifdef CONFIG_DEBUG
#ifndef network_log
#define network_log(N, ...) rt_kprintf("####[network %s:%4d] " N "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif /* at_log(...) */
#else
#define network_log(...)
#endif /* ! CONFIG_DEBUG */

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/********topic dup qos restained**************/
iot_topic_param_t iot_sub_topics[MAX_MESSAGE_HANDLERS] = {
    {TOPIC_WATER_NOTICE, 0, QOS1, 0},  /*{"TOPIC_WATER_NOTICE"}*/
    {TOPIC_PARAMETER_SET, 0, QOS1, 0}, /*{"TOPIC_PARAMETER_SET"}*/
    {TOPIC_PARAMETER_GET, 0, QOS1, 0}, /*{"TOPIC_PARAMETER_GET"}*/
    {IOT_OTA_UPGRADE, 0, QOS1, 0},     /*{"IOT_OTA_UPGRADE"}*/
};
/********topic dup qos restained**************/
iot_topic_param_t iot_pub_topics[8] = {
    {TOPIC_PLATFORM_INIT, 0, QOS1, 0},   /*{"TOPIC_PLATFORM_INIT"}*/
    {TOPIC_WATER_STATUS, 0, QOS1, 0},    /*{"TOPIC_WATER_STATUS"}*/
    {TOPIC_PARAMETER_PUT, 0, QOS1, 0},   /*{"TOPIC_PARAMETER_PUT"}*/
    {TOPIC_REALTIME_REPORT, 0, QOS1, 0}, /*{"TOPIC_REALTIME_REPORT"}*/
    {TOPIC_TIMING_REPORT, 0, QOS1, 0},   /*{"TOPIC_TIMING_REPORT"}*/
    {TOPIC_DEVICE_UPGRADE, 0, QOS1, 0},  /*{"TOPIC_DEVICE_UPGRADE"}*/
    {IOT_OTA_INFORM, 0, QOS1, 0},        /*{"IOT_OTA_INFORM"}*/
    {IOT_OTA_PROGRESS, 0, QOS1, 0},      /*{"IOT_OTA_PROGRESS"}*/
};
static MQTTClient client;
extern sys_reg_st g_sys;
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
rt_err_t network_Conversion_wifi_parpmeter(Net_Conf_st *src, Net_Conf_st *dst);
/*----------------------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* define MQTT client context */
/* Private functions ---------------------------------------------------------*/

/* 数据通道控制 */
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
int esp8266_at_socket_device_init(void);
int sim7600_at_socket_device_init(void);
void net_thread_entry(void *parameter)
{
    /**NetWork_DIR_Init**/
    NetWork_DIR_Init();
    struct tm ti;
    get_bulid_date_time(&ti);
    current_systime_set(&ti);
    rt_thread_delay(rt_tick_from_millisecond(3000));

    // if (g_sys.config.ComPara.Net_Conf.u16Net_Sel)
    // {
    //     SIM7600_DIR_4G;
    //     sim7600_at_socket_device_init();
    // }
    // else
    // {
    //     SIM7600_DIR_WIFI;
    //     esp8266_at_socket_device_init();
    // }
    SIM7600_DIR_4G;
    sim7600_at_socket_device_init();

    // Net_Conf_st temp;
    // network_Conversion_wifi_parpmeter(&g_sys.config.ComPara.Net_Conf, &temp);
    // worknet_log("u16Net_Sel:%d", g_sys.config.ComPara.Net_Conf.u16Net_Sel);
    // worknet_log("u16Net_WifiSet:0x%04X", g_sys.config.ComPara.Net_Conf.u16Net_WifiSet);

    // worknet_log("u16Wifi_Name len:%d ssid:%s",
    //             strlen((const char *)temp.u16Wifi_Name),
    //             temp.u16Wifi_Name);
    // network_log("u16Wifi_Key len:%d key:%s",
    //             strlen((const char *)temp.u16Wifi_Password),
    //             temp.u16Wifi_Password);

    network_get_interval(&client.RealtimeInterval, &client.TimingInterval);
    network_log("RealtimeInterval:%d, TimingInterval:%d", client.RealtimeInterval, client.TimingInterval);
    static int is_started = 0;
    if (is_started)
    {
        return;
    }
    /* config MQTT context param */
    network_get_register(NULL);
    mqtt_client_init(&client);
    rt_thread_delay(rt_tick_from_millisecond(7000));
    rt_thread_delay(rt_tick_from_millisecond(7000));
    rt_thread_delay(rt_tick_from_millisecond(7000));
    rt_thread_delay(rt_tick_from_millisecond(7000));
    rt_thread_delay(rt_tick_from_millisecond(7000));
    rt_thread_delay(rt_tick_from_millisecond(7000));
    paho_mqtt_start(&client);
    is_started = 1;
    return;
}
//INIT_APP_EXPORT(net_thread_entry);
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
 * @Function : void network_Serialize_inform_json(char**datapoint)
 * @File     : network.c
 * @Program  : **datapoint:buff of json serialized
 * @Created  : 2018-12-05 by seblee
 * @Brief    : serialize json
 * @Version  : V1.0
**/
void network_Serialize_inform_json(char **datapoint)
{

    /* declare a few. */
    cJSON *root = NULL, *result, *JS_paprms;

    /* Our "Video" datatype: */
    root = cJSON_CreateObject();
    if (!root)
    {
        network_log("get root faild !\n");
        goto __exit;
    }

    result = cJSON_AddStringToObject(root, "id", "1");
    if (result == NULL)
        network_log("JSON add err");

    JS_paprms = cJSON_CreateObject();
    if (!JS_paprms)
    {
        network_log("construct JS_paprms faild !\n");
        goto __exit;
    }
    result = cJSON_AddStringToObject(JS_paprms, "version", "0.0.2");
    if (result == NULL)
        network_log("JSON add err");
    cJSON_AddItemToObject(root, "params", JS_paprms);

    *datapoint = cJSON_PrintUnformatted(root);
__exit:
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
            //          iot_state = IOT_PARAM_REPORT;
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
void network_get_interval(unsigned int *real, unsigned int *timing)
{
    *real = REALTIME_INTERVAL_DEFAULT;
    *timing = TIMING_INTERVAL_DEFAULT;
    rt_uint8_t temp_buf[6];
    cpad_eMBRegHoldingCB((unsigned char *)temp_buf, 165, 2, CPAD_MB_REG_READ);
    rt_uint32_t interval_temp = (temp_buf[0] << 8) | temp_buf[1];
    interval_temp *= 60;
    if (interval_temp > TIMING_INTERVAL_MAX)
        interval_temp = TIMING_INTERVAL_MAX;
    if (interval_temp < TIMING_INTERVAL_MIN)
        interval_temp = TIMING_INTERVAL_MIN;
    *timing = interval_temp;
    network_log("timing:%d", *timing);

    interval_temp = (temp_buf[2] << 8) | temp_buf[3];
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
            //      _iot_state_t pub_msg = IOT_PARAM_REPORT;
            //    rt_mq_send(publish_mq, &pub_msg, sizeof(_iot_state_t));
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

/**
 **************************************************************** ************
 * @Function : rt_err_t network_register_parse(const char *Str, iotx_device_info_t *dev_info)
 * @File     : network.c
 * @Program  : Str:in put json
 * @Created  : 2018-10-18 by seblee
 * @Brief    : parse register_json data
 * @Version  : V1.0
**/
rt_err_t network_register_parse(const char *Str, iotx_device_info_t *dev_info)
{
    rt_err_t rc;
    cJSON *root = RT_NULL;
    network_log("Str:%s", Str);
    root = cJSON_Parse(Str);
    if (!root)
    {
        network_log("get root faild !\n");
        rc = -RT_ERROR;
    }
    else
    {
        cJSON *js_Code = cJSON_GetObjectItem(root, "code");
        if (js_Code == RT_NULL)
        {
            rc = -RT_ERROR;
            network_log("get js_Code err !!!");
            goto exit;
        }
        network_log("code:%d !", js_Code->valueint);
        cJSON *js_Message = cJSON_GetObjectItem(root, "message");
        if (js_Message == RT_NULL)
        {
            rc = -RT_ERROR;
            network_log("get js_Message err !!!");
            goto exit;
        }
        network_log("message:%s !", js_Message->valuestring);
        if (js_Code->valueint == 200)
        {
            cJSON *js_Data = cJSON_GetObjectItem(root, "data");
            if (js_Data == RT_NULL)
            {
                rc = -RT_ERROR;
                network_log("get js_Data err !!!");
                goto exit;
            }
            cJSON *js_deviceName = cJSON_GetObjectItem(js_Data, "deviceName");
            if (js_deviceName == RT_NULL)
            {
                rc = -RT_ERROR;
                network_log("get js_deviceName err !!!");
                goto exit;
            }
            cJSON *js_deviceSecret = cJSON_GetObjectItem(js_Data, "deviceSecret");
            if (js_deviceSecret == RT_NULL)
            {
                rc = -RT_ERROR;
                network_log("get js_deviceSecret err !!!");
                goto exit;
            }
            cJSON *js_productKey = cJSON_GetObjectItem(js_Data, "productKey");
            if (js_productKey == RT_NULL)
            {
                rc = -RT_ERROR;
                network_log("get js_productKey err !!!");
                goto exit;
            }
            rt_memset(dev_info, 0, sizeof(iotx_device_info_t));
            rt_snprintf(dev_info->device_name, sizeof(dev_info->device_name), "%s", js_deviceName->valuestring);
            rt_snprintf(dev_info->product_key, sizeof(dev_info->product_key), "%s", js_productKey->valuestring);
            rt_snprintf(dev_info->device_secret, sizeof(dev_info->device_secret), "%s", js_deviceSecret->valuestring);
            rt_snprintf(dev_info->device_id, sizeof(dev_info->device_id), "%s", DEVICE_ID);

            network_log("device_id:%s", dev_info->device_id);
            network_log("device_name:%s", dev_info->device_name);
            network_log("product_key:%s", dev_info->product_key);
            network_log("device_secret:%s", dev_info->device_secret);
            dev_info->flag = DEVICE_INFO_FLAG;
        }
        else
            rc = -RT_ERROR;
    }
exit:
    if (root)
        cJSON_Delete(root);
    return rc;
}

rt_err_t network_Conversion_wifi_parpmeter(Net_Conf_st *src, Net_Conf_st *dst)
{
    int i;
    for (i = 0; i < sizeof(src->u16Wifi_Name); i++)
    {
        dst->u16Wifi_Name[i] = src->u16Wifi_Name[i] << 8;
        dst->u16Wifi_Name[i] |= src->u16Wifi_Name[i] >> 8;
    }
    for (i = 0; i < sizeof(src->u16Wifi_Password); i++)
    {
        dst->u16Wifi_Password[i] = src->u16Wifi_Password[i] << 8;
        dst->u16Wifi_Password[i] |= src->u16Wifi_Password[i] >> 8;
    }
    return 0;
}
