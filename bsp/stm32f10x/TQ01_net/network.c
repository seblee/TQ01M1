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
// #define DBG_ENABLE
#define DBG_SECTION_NAME "network"
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

#define DEVICE_NAME device_info.device_name
#define PRODUCT_KEY device_info.product_key

#define NET_STACK_SIZE 3072
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/********topic dup qos restained**************/
iot_topic_param_t iot_sub_topics[MAX_MESSAGE_HANDLERS] = {
    {RT_NULL, 0, QOS1, 0}, /*{"TOPIC_WATER_NOTICE"}*/
    {RT_NULL, 0, QOS1, 0}, /*{"TOPIC_PARAMETER_SET"}*/
    {RT_NULL, 0, QOS1, 0}, /*{"TOPIC_PARAMETER_GET"}*/
    {RT_NULL, 0, QOS1, 0}, /*{"IOT_OTA_UPGRADE"}*/
};
/********topic dup qos restained**************/
iot_topic_param_t iot_pub_topics[8] = {
    {RT_NULL, 0, QOS1, 0}, /*{"TOPIC_PLATFORM_INIT"}*/
    {RT_NULL, 0, QOS1, 0}, /*{"TOPIC_WATER_STATUS"}*/
    {RT_NULL, 0, QOS1, 0}, /*{"TOPIC_PARAMETER_PUT"}*/
    {RT_NULL, 0, QOS1, 0}, /*{"TOPIC_REALTIME_REPORT"}*/
    {RT_NULL, 0, QOS1, 0}, /*{"TOPIC_TIMING_REPORT"}*/
    {RT_NULL, 0, QOS1, 0}, /*{"TOPIC_DEVICE_UPGRADE"}*/
    {RT_NULL, 0, QOS1, 0}, /*{"IOT_OTA_INFORM"}*/
    {RT_NULL, 0, QOS1, 0}, /*{"IOT_OTA_PROGRESS"}*/
};
static MQTTClient client;
extern sys_reg_st g_sys;
static iotx_device_info_t device_info;
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* define MQTT client context */
/* Private functions ---------------------------------------------------------*/

/* 数据通道控制 */
void NetWork_DIR_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(TQ01E1_PORT_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = TQ01E1_DIR_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(TQ01E1_DIR_PORT, &GPIO_InitStructure);
    DIR_8266();
}
int esp8266_at_socket_device_init(void);
int sim7600_at_socket_device_init(void);
int module_thread_start(void *parameter);

void net_thread_entry(void *parameter)
{
    rt_err_t result = RT_EOK;

    /**NetWork_DIR_Init**/
    NetWork_DIR_Init();

    rt_thread_delay(rt_tick_from_millisecond(4000));

    // result = network_Conversion_wifi_parpmeter(&g_sys.config.ComPara.Net_Conf, &temp);
    network_get_interval(&client.RealtimeInterval, &client.TimingInterval);
    LOG_D("[%d] RealtimeInterval:%d, TimingInterval:%d", rt_tick_get(), client.RealtimeInterval, client.TimingInterval);

    LOG_D("[%d] u16Net_Sel:%d", rt_tick_get(), g_sys.config.ComPara.Net_Conf.u16Net_Sel);
    LOG_D("[%d] u16Net_WifiSet:0x%04X", rt_tick_get(), g_sys.config.ComPara.Net_Conf.u16Net_WifiSet);

    static int is_started = 0;
    if (is_started)
    {
        return;
    }
    /* config MQTT context param */

    result = mqtt_client_init(&client, &device_info);
    if (result != RT_EOK)
        goto _exit;
    module_thread_start(&g_sys.config.ComPara.Net_Conf);
    paho_mqtt_start(&client);
    is_started = 1;
_exit:
    LOG_D("[%d] result:%d", rt_tick_get(), result);

    return;
}
//INIT_APP_EXPORT(net_thread_entry);
void mqtt_send_cmd(const char *send_str)
{
    MQTT_CMD(&client, send_str);

    return;
}
#ifdef RT_USING_FINSH
#include <finsh.h>
#ifdef FINSH_USING_MSH

int mq_cmd(int argc, char **argv)
{
    if (argc != 2)
    {
        rt_kprintf("More than two input parameters err!!\n");
        return 0;
    }
    mqtt_send_cmd(argv[1]);

    return 0;
}
MSH_CMD_EXPORT(mq_cmd, publish mqtt msg);
#endif /* FINSH_USING_MSH */
#endif /* RT_USING_FINSH */

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
        LOG_D("JSON add err");

    rt_snprintf(RequestNoStr, sizeof(RequestNoStr), "%d", msgid);
    result = cJSON_AddStringToObject(root, "RequestNo", RequestNoStr);
    result = cJSON_AddStringToObject(root, "ProductKey", PRODUCT_KEY);
    result = cJSON_AddStringToObject(root, "DeviceName", DEVICE_NAME);
    struct tm ti;
    char Timestamp_str[15] = {0};
    current_systime_get(&ti);
    rt_snprintf(Timestamp_str, sizeof(Timestamp_str), "%04d%02d%02d%02d%02d%02d",
                ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);

    result = cJSON_AddStringToObject(root, "Timestamp", Timestamp_str);

    rt_snprintf(sign_hex, sizeof(sign_hex), "DeviceName=%s&MCode=001&ProductKey=%s&RequestNo=%s&Timestamp=%s&Key=123456", DEVICE_NAME, PRODUCT_KEY, RequestNoStr, Timestamp_str);
    utils_md5((const unsigned char *)sign_hex, strlen(sign_hex), sign);
    // LOG_D("[%d] MD5(%s)", rt_tick_get(), sign_hex);
    rt_memset(sign_hex, 0, sizeof(sign_hex));
    for (i = 0; i < 16; ++i)
    {
        sign_hex[i * 2] = utils_hb2hex(sign[i] >> 4);
        sign_hex[i * 2 + 1] = utils_hb2hex(sign[i]);
    }
    // LOG_D("[%d] MD5=%s", sign_hex);
    cJSON_AddItemToObject(root, "Sign", cJSON_CreateString(sign_hex));
    *datapoint = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    // if (*datapoint)
    //     LOG_D("[%d] JSON len:%d", rt_tick_get(), strlen(*datapoint));
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
    char versiontemp[10] = {0};
    /* declare a few. */
    cJSON *root = NULL, *result, *JS_paprms;

    /* Our "Video" datatype: */
    root = cJSON_CreateObject();

    result = cJSON_AddStringToObject(root, "id", "1");
    if (result == NULL)
        LOG_D("[%d] JSON add err", rt_tick_get());

    JS_paprms = cJSON_CreateObject();
    if (!JS_paprms)
    {
        LOG_D("[%d] construct JS_paprms faild !", rt_tick_get());
        goto __exit;
    }

    rt_snprintf(versiontemp, sizeof(versiontemp), "%02d.%02d.%02d", ((SOFTWARE_VER & 0xf000) >> 12), ((SOFTWARE_VER & 0x0f80) >> 7), ((SOFTWARE_VER & 0x007f) >> 0));

    result = cJSON_AddStringToObject(JS_paprms, "version", versiontemp);
    if (result == NULL)
        LOG_D("[%d] JSON add err", rt_tick_get());
    cJSON_AddItemToObject(root, "params", JS_paprms);

    *datapoint = cJSON_PrintUnformatted(root);
__exit:
    cJSON_Delete(root);
    if (*datapoint)
        LOG_D("[%d] JSON len:%d---%s", rt_tick_get(), strlen(*datapoint), *datapoint);
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
    //汉字
    cJSON_AddStringToObject(root, "MCode", "007");
    rt_snprintf(StrCache, 10, "%d", msgid);
    cJSON_AddStringToObject(root, "RequestNo", StrCache);
    struct tm ti;
    char Timestamp_str[15] = {0};
    current_systime_get(&ti);
    rt_snprintf(Timestamp_str, sizeof(Timestamp_str), "%04d%02d%02d%02d%02d%02d",
                ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);

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
    rt_memset(sign_hex, 0, sizeof(sign_hex));
    for (i = 0; i < 16; ++i)
    {
        sign_hex[i * 2] = utils_hb2hex(sign[i] >> 4);
        sign_hex[i * 2 + 1] = utils_hb2hex(sign[i]);
    }
    // LOG_D("[%d] MD5=%s", sign_hex);
    cJSON_AddItemToObject(root, "Sign", cJSON_CreateString(sign_hex));
    *datapoint = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    //    if (*datapoint)
    //       LOG_D("[%d] JSON len:%d", rt_tick_get(), strlen(*datapoint));
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
    root = cJSON_Parse(Str);
    if (!root)
    {
        LOG_E("[%d] get root faild !", rt_tick_get());
        rc = -1;
    }
    else
    {
        cJSON *js_MCode = cJSON_GetObjectItem(root, "MCode");
        if (!js_MCode)
        {
            LOG_E("[%d] get MCode faild !", rt_tick_get());
            rc = -1;
            goto exit;
        }
        int MCode_value = 0;
        sscanf(js_MCode->valuestring, "%d", &MCode_value);
        LOG_D("[%d] MCode_value:%d !", rt_tick_get(), MCode_value);
        if (MCode_value == MCode_QRCODE_GENERATE)
        {
            LOG_D("[%d] get QRCode !!!", rt_tick_get());
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
                ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);

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
    // LOG_D("[%d] MD5(%s)", rt_tick_get(), sign_Cache);
    rt_memset(sign_hex, 0, sizeof(sign_hex));
    for (i = 0; i < 16; ++i)
    {
        sign_hex[i * 2] = utils_hb2hex(sign[i] >> 4);
        sign_hex[i * 2 + 1] = utils_hb2hex(sign[i]);
    }
    cJSON_AddItemToObject(root, "Sign", cJSON_CreateString(sign_hex));
    *datapoint = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    // if (*datapoint)
    //     LOG_D("[%d] JSON len:%d", rt_tick_get(), strlen(*datapoint));
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
    // *real = REALTIME_INTERVAL_DEFAULT;
    // *timing = TIMING_INTERVAL_DEFAULT;

    rt_uint32_t interval_temp = g_sys.config.Platform.Fixed_Report * 60;
    if (interval_temp > TIMING_INTERVAL_MAX)
        interval_temp = TIMING_INTERVAL_MAX;
    if (interval_temp < TIMING_INTERVAL_MIN)
        interval_temp = TIMING_INTERVAL_MIN;
    *timing = interval_temp;

    interval_temp = g_sys.config.Platform.Real_Report;
    if (interval_temp > REALTIME_INTERVAL_MAX)
        interval_temp = REALTIME_INTERVAL_MAX;
    if (interval_temp < REALTIME_INTERVAL_MIN)
        interval_temp = REALTIME_INTERVAL_MIN;
    *real = interval_temp;
    // LOG_D("[%d] real:%d timing:%d", *real, *timing);
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
    LOG_D("[%d] Str:%s", rt_tick_get(), Str);
    root = cJSON_Parse(Str);
    if (!root)
    {
        LOG_E("[%d] get root faild !");
        rc = -1;
    }
    else
    {
        cJSON *js_MCode = cJSON_GetObjectItem(root, "MCode");
        sscanf(js_MCode->valuestring, "%d", &MCode_value);
        LOG_D("[%d] MCode_value:%d !", rt_tick_get(), MCode_value);
        if (MCode_value == MCode_PARAMETER_GET)
        {
            LOG_D("[%d] get PARAMETER_GET !!!", rt_tick_get());
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
    LOG_D("[%d] Str:%s", rt_tick_get(), Str);
    root = cJSON_Parse(Str);
    if (!root)
    {
        LOG_E("[%d] get root faild !", rt_tick_get());
        rc = -1;
    }
    else
    {
        cJSON *js_MCode = cJSON_GetObjectItem(root, "MCode");
        sscanf(js_MCode->valuestring, "%d", &MCode_value);
        LOG_D("[%d] MCode_value:%d !", rt_tick_get(), MCode_value);
        if (MCode_value == MCode_PARAMETER_SET)
        {
            LOG_D("[%d] get PARAMETER_SET !!!", rt_tick_get());
            int Setaddrstart;
            cJSON *js_Setaddrstart = cJSON_GetObjectItem(root, "Setaddrstart");
            if (js_Setaddrstart == RT_NULL)
            {
                rc = -RT_ERROR;
                LOG_E("[%d] get js_Setaddrstart err !!!", rt_tick_get());
            }
            sscanf(js_Setaddrstart->valuestring, "%d", &Setaddrstart);
            LOG_D("[%d] get Setaddrstart:%d", rt_tick_get(), Setaddrstart);
            int Settingleng;
            cJSON *js_Settingleng = cJSON_GetObjectItem(root, "Settingleng");
            if (js_Settingleng == RT_NULL)
            {
                rc = -RT_ERROR;
                LOG_E("[%d] get js_Settingleng err !!!", rt_tick_get());
            }
            sscanf(js_Settingleng->valuestring, "%d", &Settingleng);
            LOG_D("[%d] get Settingleng:%d", rt_tick_get(), Settingleng);
            cJSON *js_Settingmsg = cJSON_GetObjectItem(root, "Settingmsg");
            if (js_Settingmsg == RT_NULL)
            {
                rc = -RT_ERROR;
                LOG_E("[%d] get js_Settingmsg err !!!", rt_tick_get());
            }
            char *Settingmsg_str = rt_calloc(Settingleng * 4 + 1, sizeof(rt_uint8_t));
            if (Settingmsg_str == RT_NULL)
            {
                rc = -RT_ERROR;
                LOG_E("[%d] get Settingmsg_str err !!!", rt_tick_get());
            }
            int Settingmsg_data;
            if ((js_Settingmsg != RT_NULL) && (Settingmsg_str != RT_NULL))
            {
                rt_strncpy(Settingmsg_str, js_Settingmsg->valuestring, Settingleng * 4);
                LOG_E("[%d] get Settingmsg_str:%s", rt_tick_get(), Settingmsg_str);
                int i;
                unsigned char *Settingmsg_data_buf = rt_calloc(Settingleng * 2, sizeof(rt_uint8_t));

                for (i = 0; i < Settingleng * 2; i++)
                {
                    sscanf(Settingmsg_str + 2 * i, "%02X", &Settingmsg_data);
                    Settingmsg_data_buf[i] = (uint8_t)(Settingmsg_data & (int)0xFF);
                    // rt_kprintf("%02x", Settingmsg_data_buf[i]);
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
    LOG_D("[%d] Str:%s", rt_tick_get(), Str);
    root = cJSON_Parse(Str);
    if (!root)
    {
        LOG_E("[%d] get root faild !", rt_tick_get());
        rc = -RT_ERROR;
    }
    else
    {
        cJSON *js_Code = cJSON_GetObjectItem(root, "code");
        if (js_Code == RT_NULL)
        {
            rc = -RT_ERROR;
            LOG_E("[%d] get js_Code err !!!", rt_tick_get());
            goto exit;
        }
        LOG_D("[%d] code:%d !", js_Code->valueint);
        cJSON *js_Message = cJSON_GetObjectItem(root, "message");
        if (js_Message == RT_NULL)
        {
            rc = -RT_ERROR;
            LOG_E("[%d] get js_Message err !!!", rt_tick_get());
            goto exit;
        }
        LOG_D("[%d] message:%s !", rt_tick_get(), js_Message->valuestring);
        if (js_Code->valueint == 200)
        {
            cJSON *js_Data = cJSON_GetObjectItem(root, "data");
            if (js_Data == RT_NULL)
            {
                rc = -RT_ERROR;
                LOG_E("[%d] get js_Data err !!!", rt_tick_get());
                goto exit;
            }
            cJSON *js_deviceName = cJSON_GetObjectItem(js_Data, "deviceName");
            if (js_deviceName == RT_NULL)
            {
                rc = -RT_ERROR;
                LOG_E("[%d] get js_deviceName err !!!", rt_tick_get());
                goto exit;
            }
            cJSON *js_deviceSecret = cJSON_GetObjectItem(js_Data, "deviceSecret");
            if (js_deviceSecret == RT_NULL)
            {
                rc = -RT_ERROR;
                LOG_E("[%d] get js_deviceSecret err !!!", rt_tick_get());
                goto exit;
            }
            cJSON *js_productKey = cJSON_GetObjectItem(js_Data, "productKey");
            if (js_productKey == RT_NULL)
            {
                rc = -RT_ERROR;
                LOG_E("[%d] get js_productKey err !!!", rt_tick_get());
                goto exit;
            }
            rt_memset(dev_info, 0, sizeof(iotx_device_info_t));
            rt_snprintf(dev_info->device_name, sizeof(dev_info->device_name), "%s", js_deviceName->valuestring);
            rt_snprintf(dev_info->product_key, sizeof(dev_info->product_key), "%s", js_productKey->valuestring);
            rt_snprintf(dev_info->device_secret, sizeof(dev_info->device_secret), "%s", js_deviceSecret->valuestring);
            rt_snprintf(dev_info->device_id, sizeof(dev_info->device_id), "%s", DEVICE_ID);

            LOG_D("[%d] device_id:%s", rt_tick_get(), dev_info->device_id);
            LOG_D("[%d] device_name:%s", rt_tick_get(), dev_info->device_name);
            LOG_D("[%d] product_key:%s", rt_tick_get(), dev_info->product_key);
            LOG_D("[%d] device_secret:%s", rt_tick_get(), dev_info->device_secret);
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
    rt_memcpy(dst, src, sizeof(Net_Conf_st));

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
rt_err_t Conversion_modbus_2_ram(rt_uint8_t *dst, rt_uint8_t *src, rt_uint16_t len)
{
    int i;

    for (i = 0; i < (len / 2); i++)
    {
        *(dst + 2 * i) = *(src + 2 * i + 1);
        *(dst + 2 * i + 1) = *(src + 2 * i);
    }
    return 0;
}
