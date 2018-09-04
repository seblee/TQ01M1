#include <rtthread.h>
/* UART接收消息结构*/
struct rx_msg
{
	rt_device_t dev;
	rt_size_t size;
}

/* 用于接收消息的消息队列*/
static rt_mq_t rx_mq;
/* 接收线程的接收缓冲区*/
static char uart_rx_buffer[64];

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

//rt_device_t sim7600_device;
char out_test[] = "test uart3 device out string";

void sim7600_thread_entry(void *parameter)
{
	struct rx_msg msg;
	int count = 0;
	rt_device_t device, write_device;
	rt_err_t result = RT_EOK;
	/* 查找系统中的串口1设备 */
	device = rt_device_find("uart1");
	if (device != RT_NULL)
	{
		/* 设置回调函数及打开设备*/
		rt_device_set_rx_indicate(device, uart_input);
		rt_device_open(device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
	}
	/* 设置写设备为uart1设备 */
	write_device = device;
	/* 查找系统中的串口2设备 */
	device = rt_device_find("uart2");
	if (device_ != RT_NULL)
	{
		/* 设置回调函数及打开设备*/
		rt_device_set_rx_indicate(device, uart_input);
		rt_device_open(device, RT_DEVICE_OFLAG_RDWR);
	}
	while (1)
	{
		/* 从消息队列中读取消息*/
		result = rt_mq_recv(rx_mq, &msg, sizeof(struct rx_msg), 50);
		if (result == -RT_ETIMEOUT)
		{
			/* 接收超时*/
			rt_kprintf("timeout count:%d\n", ++count);
		}
		/* 成功收到消息*/
		if (result == RT_EOK)
		{
			rt_uint32_t rx_length;
			rx_length = (sizeof(uart_rx_buffer) - 1) > msg.size ? msg.size : sizeof(uart_rx_buffer) - 1;
			/* 读取消息*/
			rx_length = rt_device_read(msg.dev, 0, &uart_rx_buffer[0],
									   rx_length);
			uart_rx_buffer[rx_length] = '\0';
			/* 写到写设备中*/
			if (write_device != RT_NULL)
				rt_device_write(write_device, 0, &uart_rx_buffer[0],
								rx_length);
		}
	}
}
