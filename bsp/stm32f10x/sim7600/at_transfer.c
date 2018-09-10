/**
 ****************************************************************************
 * @Warning :Without permission from the author,Not for commercial use
 * @File    :
 * @Author  :Seblee
 * @date    :2018-09-10 10:28:03
 * @version :V1.0.0
 *************************************************
 * @brief   :
 ****************************************************************************
 * @Last Modified by: Seblee
 * @Last Modified time: 2018-09-10 18:16:53
 ****************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "at_transfer.h"
#include "sim7600.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/***********at command for wifi*************************/
const char AT_WIFI_SYNC[] = {"AT\r\n"};
const char AT_WIFI_STATUS[] = {"AT+CIPSTATUS\r\n"};
const char AT_WIFI_SET_SSL_BUFF_SIZE[] = {"AT+CIPSSLSIZE=4096"};
const char AT_WIFI_CONNECT_SSL[] = {"AT+CIPSTART"};
/***********at result for wifi*************************/
const char AT_WIFI_ACK_OK[] = {"OK\r\n"};
const char AT_WIFI_ACK_STATUS[] = {"STATUS"};
/***********at command for 4G*************************/
#define AT_4G_SYNC AT_WIFI_SYNC
const char AT_CBC[] = {"AT+CBC\r\n"};         //检查充电状态，以及电池电量占容量的百分比+CBC: 0,97,4164
const char AT_CREG[] = {"AT+CREG?\r\n"};      //查询网络注册状态
const char AT_CREGINIT[] = {"AT+CREG=1\r\n"}; //初始化网络注册状态
const char AT_ATE[] = {"ATE0\r\n"};           //存储当前的设置参数
const char AT_Save[] = {"AT&W\r\n"};          //存储当前的参数

//0 disable slow clock 1 enable slow clock
const char AT_CSCLK[] = {"AT+CSCLK=1\r\n"}; //enable slow clock
//0 minimum ality 1 full ality (Default) 4 disable phone both transmit and receive RF circuits
const char AT_CFUN[] = {"AT+CFUN=1\r\n"};    //使模块在重启后电源进入功能性的电平
const char AT_CGMR[] = {"AT+CGMR\r\n"};      //软件版本标识
const char AT_CCID[] = {"AT+CCID\r\n"};      //获得SIM卡标识
const char AT_CGSN[] = {"AT+CGSN\r\n"};      //IMEI
const char AT_CSQ[] = {"AT+CSQ\r\n"};        //信号强度
const char AT_CSMINS[] = {"AT+CSMINS?\r\n"}; //检测SIM卡是否插入+CSMINS: 0,1
const char AT_COPS[] = {"AT+COPS?\r\n"};
const char AT_CPOL[] = {"AT+CPOL?\r\n"};
const char AT_REG_COPS[] = {"AT+CPOL=1,2,\""};

//******************************************************************************
//SMS命令集
//------------------------------------------------------------------
const char AT_CMGF[] = {"AT+CMGF=1\r\n"};       //指定信息的输入输出格式为文本格式
const char AT_CMGFNMEA[] = {"AT+CMGF=0\r\n"};   //指定信息的输入输出格式为PDU格式
const char AT_CSCS[] = {"AT+CSCS=\"GSM\"\r\n"}; //AT+CSCS=“GSM”设置什么样的字体让模块接受
const char AT_CMGS[] = {"AT+CMGS=\""};          //发短信
const char AT_CMGR[] = {"\r\nAT+CMGR="};        //读短信

const char AT_CMGD1[] = {"AT+CMGD=1\r\n"}; //删除短信
const char AT_CMGD2[] = {"AT+CMGD=2\r\n"}; //删除短信
const char AT_CMGD3[] = {"AT+CMGD=3\r\n"}; //删除短信
const char AT_CMGD4[] = {"AT+CMGD=4\r\n"}; //删除短信
const char AT_CMGD5[] = {"AT+CMGD=5\r\n"}; //删除短信
const char AT_CMGD6[] = {"AT+CMGD=6\r\n"}; //删除短信
const char AT_CMGD7[] = {"AT+CMGD=7\r\n"}; //删除短信
const char AT_CMGD8[] = {"AT+CMGD=8\r\n"}; //删除短信
const char AT_CMGD9[] = {"AT+CMGD=9\r\n"}; //删除短信

const char AT_CNMI[] = {"AT+CNMI=2,1\r\n"};            // ？ 初始化AT指令时要用上
const char AT_CMGL[] = {"AT+CMGL=\"REC UNREAD\"\r\n"}; //接收没有读的信息
//const char AT_CSMP[] = {"AT+CSMP=17，167，0，240 \r\n"};
//const char AT_IFC[] = {"AT+IFC=2，2 \r\n"}; //建立数据呼叫，通过RTS和CTS控制
//const char AT_CBST[] = {"AT+CBST=0，0, 1 \r\n"};
//------------------------------------------------------------------
//GPRS命令集
//------------------------------------------------------------------
const char Gprs_class[] = {"AT+CGCLASS=\"B\"\r\n"}; //length = 16B 显示GPRS信息移动的类型
const char Gprs_apn[] = {"AT+CSTT=\""};             //GPRS通用的名字
const char Gprs_CSTT[] = {"AT+CIPCSGP=1,\""};       //设置GPRS为无线连接的模式
const char Gprs_cgatt[] = {"AT+CGATT=1\r\n"};       //length = 12B显示GPRS配置是附加的
const char Gprs_cgreg[] = {"AT+CGREG?\r\n"};        //查询GPRS网络注册状态
const char Gprs_cgreginit[] = {"AT+CGREG=1\r\n"};   //初始化GPRS网络注册状态

const char Gprs_T_S[] = {"AT+CIPSTART=\"TCP\",\""}; //建立TCP连接;连接成功反馈CONNECT OK
const char Gprs_U_S[] = {"AT+CIPSTART=\"UDP\",\""}; //建立UDP连接
const char Gprs_ipshut[] = {"AT+CIPSHUT\r\n"};      // //使GPRS的PDP失效
const char Gprs_send[] = {"AT+CIPSEND\r\n"};        //发送数据；
const char Gprs_dpdp[] = {"AT+CIPDPDP=1,60,3\r\n"}; //自动检测GPRS是否为附着状态；60毫秒，检测3次
const char SetBand[] = "AT+IPR=9600\r\n";           //设置与终端设备通信的波特率为9600
//------------------------------------------------------------------
//通话命令集
//------------------------------------------------------------------
const char Gsm_cmd_call_view[] = {"AT+CLIP=1\r\n"};   //使能来电显示
const char Gsm_P_ATD[] = {"\r\nATD"};                 //拨号
const char Gsm_cmd_call_ack[] = {"ATA\r\n"};          //接通
const char Gsm_call_connect[] = {"AT+CLCC\r\n"};      //查询是否接通
const char Gsm_cmd_call_noack[] = {"ATH\r\n"};        //挂断
const char Gsm_cmd_call_sel[] = {"AT+CHFA=0\r\n"};    //选主通道
const char Gsm_cmd_call_CMIC[] = {"AT+CMIC=0,9\r\n"}; //MIC增益+13.5dB,选择主麦克风
const char Gsm_cmd_call_CLVL[] = {"AT+CLVL=90\r\n"};  //受话器音量级别
const char Gsm_cmd_Voice_num1[] = {"AT+VTS=1\r\n"};   //拨号键3
const char Gsm_cmd_Voice_num2[] = {"AT+VTS=2\r\n"};   //拨号键3
const char Gsm_cmd_Voice_num3[] = {"AT+VTS=3\r\n"};   //拨号键3
const char Gsm_cmd_Voice_num4[] = {"AT+VTS=4\r\n"};   //拨号键3
const char Gsm_cmd_Voice_num5[] = {"AT+VTS=5\r\n"};   //拨号键3
const char Gsm_cmd_Voice_num6[] = {"AT+VTS=6\r\n"};   //拨号键3
const char AT_VTS[] = {"AT+VTS=\r\n"};                //Send DTMF
const char ATMicOn[] = "AT+CMUT=0\r\n";
const char ATMicOff[] = "AT+CMUT=1\r\n";
const char Gsm_cmd_Turn_Off[] = {"AT+CPOWD=1\r\n"}; //关模块命令
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/****test AT************/
rt_err_t at_wifi_send_message_ack_ok(rt_device_t dev, const char *AT_command)
{
    rt_err_t err = RT_ERROR;
    char *message = RT_NULL;

    sim7600_send_message(dev, AT_command, (rt_uint8_t **)&message);

    if (message)
    {
        char *wifi_status = rt_strstr(message, AT_WIFI_ACK_OK);
        if (wifi_status != RT_NULL)
            err = RT_EOK; // STATUS:
        rt_free(message);
    }
    return err;
}

rt_err_t at_wifi_get_cipstatus(rt_device_t dev)
{
    rt_err_t err = RT_ERROR;
    char *message = RT_NULL;

    sim7600_send_message(dev, AT_WIFI_STATUS, (rt_uint8_t **)&message);

    if (message)
    {
        char *wifi_status = rt_strstr(message, AT_WIFI_ACK_STATUS);
        if (wifi_status != RT_NULL)
            err = *(wifi_status + rt_strlen(AT_WIFI_ACK_STATUS) + 1); // STATUS:
        rt_free(message);
    }
    return err;
}
rt_err_t at_wifi_connect_ssl(rt_device_t dev, char *host, int port)
{
    rt_err_t err = RT_ERROR;
    char *message = RT_NULL;
    char send_buffer[100] = {0};
    rt_sprintf(send_buffer, "%s=\"SSL\",\"%s\",%d,1", AT_WIFI_CONNECT_SSL, host, port);

    sim7600_send_message(dev, AT_WIFI_STATUS, (rt_uint8_t **)&message);

    if (message)
    {
        char *wifi_status = rt_strstr(message, AT_WIFI_ACK_STATUS);
        if (wifi_status != RT_NULL)
            err = *(wifi_status + rt_strlen(AT_WIFI_ACK_STATUS) + 1); // STATUS:
        rt_free(message);
    }
    return err;
}
