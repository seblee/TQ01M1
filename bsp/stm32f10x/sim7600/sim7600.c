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
 * @Last Modified time: 2018-09-10 17:54:26
 ****************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "sim7600.h"
#include "transport.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};
/* 用于接收消息的消息队列*/
rt_mq_t rx_mq;
/* 接收线程的接收缓冲区*/
static char uart_rx_buffer[64];
const char out_test[] = "test uart3 device out start*******\r\n";

const char iot_devicename[] = {"cdtest004"};
const char iot_productKey[] = {"a1JOOi3mNEf"};
const char iot_secret[] = {"WjzDAlsux7gBMfF31M9CSZ9LKmutISPe"};
/* Private function prototypes -----------------------------------------------*/

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
    rt_device_t write_device;
    rt_err_t result = RT_EOK;
    char IOT_domain_str[100] = {0};

    rx_mq = rt_mq_create("7600_rx_mq",
                         sizeof(struct rx_msg), /* 每个消息的大小是 128 - void* */
                         5,                     /* 消息队列的最大容量 */
                         RT_IPC_FLAG_FIFO);     /* 如果有多个线程等待，按照FIFO的方法分配消息 */
    SIM7600_DIR_WIFI;
    SIM7600_DIR_Init();
    /* 查找系统中的串口1设备 */
    rt_thread_delay(SIM7600_THREAD_DELAY);
    write_device = rt_device_find("uart3");
    if (write_device != RT_NULL)
    {
        /* 设置回调函数及打开设备*/
        rt_device_set_rx_indicate(write_device, uart_input);
        rt_device_open(write_device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
    rt_device_write(write_device, 0, out_test, strlen(out_test));
    rt_sprintf(IOT_domain_str, aliyun_domain, iot_productKey);
    transport_open(write_device, IOT_domain_str, aliyun_iot_port);
    while (1)
    {
        /* 从消息队列中读取消息*/
        result = rt_mq_recv(rx_mq, &msg, sizeof(struct rx_msg), 50);
        // if (result == -RT_ETIMEOUT)
        // {
        //     if (receive_flag == 1)
        //     {
        //         /**received something**/
        //     }
        //     /* 接收超时*/
        //     // rt_kprintf("timeout count:%d\n", ++count);
        //     // rt_device_write(write_device, 0, "time out",
        //     //                 strlen("time out"));
        // }
        // if (result == RT_EOK)
        // {
        //     receive_flag = 1;
        // }

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
            // rx_length = rt_device_read(msg.dev, 0, &uart_rx_buffer[0],
            //                            rx_length);

            rt_memset(uart_rx_buffer, 0, sizeof(uart_rx_buffer));
            if (rx_length >= 23)
            {
                rt_sprintf(uart_rx_buffer, "rx_length:%ld\r\n", rx_length);
                if (write_device != RT_NULL)
                    rt_device_write(write_device, 0, &uart_rx_buffer[0],
                                    strlen(uart_rx_buffer));
                rx_length = rt_device_read(msg.dev, 0, &uart_rx_buffer[0],
                                           rx_length);
                rt_sprintf(&uart_rx_buffer[rx_length], "\r\n");
                /* 写到写设备中*/
                if (write_device != RT_NULL)
                    rt_device_write(write_device, 0, &uart_rx_buffer[0],
                                    strlen(uart_rx_buffer));
            }
        }
    }
}
rt_uint32_t sim7600_send_message(rt_device_t dev, const char *senddata, rt_uint8_t **data)
{
    rt_uint16_t timeout = 500;
    rt_uint32_t count = 0;
    struct rx_msg msg;
    rt_err_t result = RT_EOK;
    rt_uint32_t rx_length;

    rt_device_write(dev, 0, senddata, strlen(senddata));

    if (*data != RT_NULL)
    {
        rt_free(*data);
        *data = RT_NULL;
    }
    while (1)
    {
        result = rt_mq_recv(rx_mq, &msg, sizeof(struct rx_msg), timeout);
        if (result == RT_EOK)
        {
            timeout = 50;
            rx_length = (sizeof(uart_rx_buffer) - 1) > msg.size ? msg.size : sizeof(uart_rx_buffer) - 1;
            if (*data == RT_NULL)
                *data = (rt_uint8_t *)rt_malloc(rx_length);
            else
                *data = (rt_uint8_t *)rt_malloc(rx_length + count);
            if (*data == RT_NULL)
                goto exit;
            rx_length = rt_device_read(msg.dev, 0, &uart_rx_buffer[count], rx_length);
            count += rx_length;
        }
        if (result == -RT_ETIMEOUT)
            break;
    }
    return count;
exit:
    if (*data != RT_NULL)
    {
        rt_free(*data);
        *data = RT_NULL;
    }
    return 0;
}
