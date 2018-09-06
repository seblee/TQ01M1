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
 * @Last Modified time: 2018-09-06 17:15:18
 ****************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "sys_conf.h"
#include <rtthread.h>
#include "transport.h"
/* Private typedef -----------------------------------------------------------*/
/* UART接收消息结构*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* 用于接收消息的消息队列*/
static rt_mq_t rx_mq;
/* 接收线程的接收缓冲区*/
static char uart_rx_buffer[64];
char out_test[] = "test uart3 device out start*******\r\n";
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* 数据到达回调函数*/
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
    rt_uint8_t receive_flag = 1;
    struct rx_msg msg;
    int count = 0;
    rt_device_t device, write_device;
    rt_err_t result = RT_EOK;

    rx_mq = rt_mq_create("7600_rx_mq",
                         sizeof(struct rx_msg), /* 每个消息的大小是 128 - void* */
                         5,                     /* 消息队列的最大容量 */
                         RT_IPC_FLAG_FIFO);     /* 如果有多个线程等待，按照FIFO的方法分配消息 */

    /* 查找系统中的串口1设备 */
    rt_thread_delay(SIM7600_THREAD_DELAY);
    device = rt_device_find("uart3");
    if (device != RT_NULL)
    {
        /* 设置回调函数及打开设备*/
        rt_device_set_rx_indicate(device, uart_input);
        rt_device_open(device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
    /* 设置写设备为uart1设备 */
    write_device = device;
    // /* 查找系统中的串口2设备 */
    // device = rt_device_find("uart2");
    // if (device != RT_NULL)
    // {
    // 	/* 设置回调函数及打开设备*/
    // 	rt_device_set_rx_indicate(device, uart_input);
    // 	rt_device_open(device, RT_DEVICE_OFLAG_RDWR);
    // }
    rt_device_write(write_device, 0, out_test,
                    strlen(out_test));
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
