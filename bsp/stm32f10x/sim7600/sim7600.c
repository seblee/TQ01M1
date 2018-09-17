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
 * @Last Modified time: 2018-09-11 18:18:29
 ****************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "sim7600.h"
#include "transport.h"
#include "mqtt_client.h"
#include "at_transfer.h"
/* Private typedef -----------------------------------------------------------*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};

/* Private define ------------------------------------------------------------*/
#ifndef sim7600_log
#define sim7600_log(N, ...) rt_kprintf("####[sim7600 %s:%4d] " N "\r\n", __FILE__, __LINE__, ##__VA_ARGS__);
#endif /* at_log(...) */
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
rt_device_t write_device;
/* 用于接收消息的消息队列*/
rt_mq_t rx_mq;
/* 接收线程的接收缓冲区*/
static char uart_rx_buffer[64];
rt_uint8_t write_buffer[MSG_LEN_MAX];

const char iot_deviceid[] = {DEVICE_ID};
const char iot_devicename[] = {DEVICE_NAME};
const char iot_productKey[] = {PRODUCT_KEY};
const char iot_secret[] = {DEVICE_SECRET};
/* Private function prototypes -----------------------------------------------*/
iotx_device_info_t device_info;
iotx_conn_info_t device_connect;
MQTTPacket_connectData client_con = MQTTPacket_connectData_initializer;
/* Private functions ---------------------------------------------------------*/

/* 数据到达回调函数*/
void SIM7600_DIR_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = SIM7600_DIR_PIN;
    GPIO_Init(SIM7600_DIR_PORT, &GPIO_InitStructure);
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
    struct rx_msg msg;

    rt_err_t result = RT_EOK;

    rx_mq = rt_mq_create("7600_rx_mq",
                         sizeof(struct rx_msg), /* 每个消息的大小是 128 - void* */
                         5,                     /* 消息队列的最大容量 */
                         RT_IPC_FLAG_FIFO);     /* 如果有多个线程等待，按照FIFO的方法分配消息 */
    SIM7600_DIR_WIFI;
    SIM7600_DIR_Init();
    rt_thread_delay(SIM7600_THREAD_DELAY);
    write_device = rt_device_find("uart3");
    if (write_device != RT_NULL)
    {
        rt_device_set_rx_indicate(write_device, uart_input);
        rt_device_open(write_device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
    result = at_wifi_init(write_device);
    mqtt_client_init(write_device);
    sim7600_log("mqtt_client_init done");

    transport_open(write_device, device_connect.host_name, device_connect.port);
    mqtt_client_connect(write_device, &client_con);
    result = mqtt_client_subscribe_topics();
    // transport_close(write_device);
    while (1)
    {

        /* 从消息队列中读取消息*/
        result = rt_mq_recv(rx_mq, &msg, sizeof(struct rx_msg), 50);
        if (result == -RT_ETIMEOUT)
        {
            /* 接收超时*/
            // rt_kprintf("timeout count:%d\n", ++count);
            // rt_device_write(write_device, 0, "time out",
            //                 strlen("time out"));
        }
        /* 成功收到消息*/
        if (result == RT_EOK)
        {
            rt_uint32_t rx_length;
            rx_length = (sizeof(uart_rx_buffer) - 1) > msg.size ? msg.size : sizeof(uart_rx_buffer) - 1;
            /* 读取消息*/
            rx_length = rt_device_read(msg.dev, 0, &uart_rx_buffer[0],
                                       rx_length);

            rt_memset(uart_rx_buffer, 0, sizeof(uart_rx_buffer));
            if (rx_length >= 23)
            {
                rt_sprintf(uart_rx_buffer, "rx_length:%ld\r\n", rx_length);
                // if (write_device != RT_NULL)
                //     rt_device_write(write_device, 0, &uart_rx_buffer[0],
                //                     strlen(uart_rx_buffer));
                // rx_length = rt_device_read(msg.dev, 0, &uart_rx_buffer[0],
                //                            rx_length);
                // rt_sprintf(&uart_rx_buffer[rx_length], "\r\n");
                // /* 写到写设备中*/
                // if (write_device != RT_NULL)
                //     rt_device_write(write_device, 0, &uart_rx_buffer[0],
                //                     strlen(uart_rx_buffer));
            }
        }
    }
}
rt_uint32_t sim7600_send_message(rt_device_t dev, const char *senddata, rt_uint8_t **data)
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
            rx_length = (sizeof(uart_rx_buffer) - 1) > msg.size ? msg.size : sizeof(uart_rx_buffer) - 1;
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
 * @Function : rt_int32_t sim7600_read_message(rt_device_t dev, rt_uint8_t *data, rt_int16_t len, rt_int32_t timeout)
 * @File     : sim7600.c
 * @Program  : dev:serial device
 *             *data:the buff read
 *             len:the length read
 * @Created  : 2017/12/12 by seblee
 * @Brief    : 
 * @Version  : V1.0
**/
rt_int32_t sim7600_read_message(rt_device_t dev, rt_uint8_t *data, rt_int16_t len, rt_int32_t timeout)
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
            timer = 100;
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
