#include "Comm_TDS.h"
#include "local_status.h"
#define SAMPLE_UART_NAME "uart4"
/* 串口接收消息结构*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};
/* 串口设备句柄 */
static rt_device_t serial;
/* 消息队列控制块 */
static struct rt_messagequeue rx_mq;

unsigned char recOK = 0;
#define COM_LEN   14
static unsigned char tx_buffer[20] = {0xFF,0x01,0x00,0x00,0x02,0x00,0x00,0x03,0x00,0x00,0x04,0x00,0x00,0xEE};
static unsigned char rx_buffer[20] = {0xFF,0x01,0x00,0x00,0x02,0x00,0x00,0x03,0x00,0x00,0x04,0x00,0x00,0xEE};


/*************************function******************************************************/
/* 接收数据回调函数 */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    rt_err_t result;
    msg.dev = dev;
    msg.size = size;

    result = rt_mq_send(&rx_mq, &msg, sizeof(msg));
    if (result == -RT_EFULL)
    {
        /* 消息队列满 */
        rt_kprintf("message queue full！\n");
    }
    return result;
}

/*******************************************************************************
 * Function Name  : TDS_thread_Init
 * Description    : TDS线程串口通信初始化.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
int TDS_thread_Init(void)
{
    rt_err_t ret = RT_EOK;
    char uart_name[RT_NAME_MAX];
    static char msg_pool[256];
    char str[] = "hello RT-Thread!\r\n";

    rt_strncpy(uart_name, SAMPLE_UART_NAME, RT_NAME_MAX);

    /* 查找串口设备 */
    serial = rt_device_find(uart_name);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", uart_name);
        return RT_ERROR;
    }

    /* 初始化消息队列 */
    rt_mq_init(&rx_mq, "rx_mq",
               msg_pool,              /* 存放消息的缓冲区 */
               sizeof(struct rx_msg), /* 一条消息的最大长度 */
               sizeof(msg_pool),      /* 存放消息的缓冲区大小 */
               RT_IPC_FLAG_FIFO);     /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

    /* 以 DMA 接收及轮询发送方式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_DMA_RX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_input);
    /* 发送字符串 */
    rt_device_write(serial, 0, str, (sizeof(str) - 1));

    return ret;
}

/*******************************************************************************
 * Function Name  : receiveData
 * Description    : 串口接收数据校验.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
static    uint16_t	u16Buff = 0;
static uint8_t receiveData(unsigned char data)
{
    static unsigned char count = 0;
		uint8_t recOK=0;
	
		switch(count)
		{
			case	0x00:
				if (data == TDS_HEAD)
				{
						rx_buffer[count] = data;
						count++;
						u16Buff|=0x01;
				}
				else
				{
						count = 0;					
				}
				break;
			case	0x01:
				if (data == TDS_01)
				{
						rx_buffer[count] = data;
						u16Buff|=0x02;
						count++;
				}
				else
				{
						count = 0;					
				}				
				break;	
			case	0x02:
			case	0x03:
				{
						rx_buffer[count] = data;
						u16Buff|=0x04;
						count++;
				}				
				break;	
			case	0x04:
				if (data == TDS_02)
				{
						rx_buffer[count] = data;
						u16Buff|=0x10;
						count++;
				}
				else
				{
						count = 0;					
				}				
				break;	
			case	0x05:
			case	0x06:
				{
						rx_buffer[count] = data;
						u16Buff|=0x20;
						count++;
				}				
				break;	
			case	0x07:
				if (data == TDS_03)
				{
						rx_buffer[count] = data;
						u16Buff|=0x100;
						count++;
				}
				else
				{
						count = 0;					
				}				
				break;	
			case	0x08:
			case	0x09:
				{
						rx_buffer[count] = data;
						u16Buff|=0x200;
						count++;
				}				
				break;			
			case	0x0A:
				if (data == TDS_04)
				{
						rx_buffer[count] = data;
						u16Buff|=0x1000;
						count++;
				}
				else
				{
						count = 0;					
				}				
				break;	
			case	0x0B:
			case	0x0C:
				{
						rx_buffer[count] = data;
						u16Buff|=0x2000;
						count++;
				}				
				break;
			case	0x0D:
				if (data == TDS_END)
				{
						rx_buffer[count] = data;
						u16Buff|=0x8000;
						count = 0;
						recOK = 1;
				}
				else
				{
						count = 0;					
				}				
				break;
			default:
						count = 0;						
				break;
		}
		return recOK;
}

/*******************************************************************************
 * Function Name  : receiveData
 * Description    : 串口接收数据校验.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
static uint8_t TDSData_Process(void)
{
    extern sys_reg_st g_sys;
    extern local_reg_st l_sys;
		uint8_t i=0;
		for(i=0;i<=3;i++)
		{
				g_sys.status.ComSta.u16TDS[i]=(rx_buffer[(i+1)*2]<<8)|rx_buffer[(i+1)*2+2];
		}
//		rt_kprintf("u16TDS[0]=%x,u16TDS[1]=%x,u16TDS[2]=%x,u16TDS[3]=%x\n",g_sys.status.ComSta.u16TDS[0],g_sys.status.ComSta.u16TDS[1],g_sys.status.ComSta.u16TDS[2],g_sys.status.ComSta.u16TDS[3]);		
		return TRUE;
	
}
/*******************************************************************************
 * Function Name  : TDS_thread_entry
 * Description    : TDS线程.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/

void TDS_thread_entry(void *parameter)
{
    struct rx_msg msg;
    rt_err_t result;
    rt_uint32_t rx_length;
    static unsigned char rcv_buffer[RT_SERIAL_RB_BUFSZ + 1];
		uint8_t recOK=0;
		static 	uint8_t u8TX_Delay=0;
    uint16_t u16Buff0 = 0;
	
    rt_thread_delay(1000);
		TDS_thread_Init();
    while (1)
    {
        rt_memset(&msg, 0, sizeof(msg));
        /* 从消息队列中读取消息*/
        result = rt_mq_recv(&rx_mq, &msg, sizeof(msg), 1000);
				u16Buff0|=0x01;
        if (result == RT_EOK)
        {
									u16Buff0|=0x02;
            /* 从串口读取数据*/
            rx_length = rt_device_read(msg.dev, 0, rcv_buffer, msg.size);
//						rt_kprintf("rcv_buffer[0]=%x,rcv_buffer[1]=%x,rcv_buffer[2]=%x,rcv_buffer[12]=%x,rcv_buffer[13]=%x\n",rcv_buffer[0],rcv_buffer[1],rcv_buffer[2],rcv_buffer[12],rcv_buffer[13]);
            u16Buff=0;
						for (char i = 0; i < rx_length; i++)
            {
                recOK=receiveData(rcv_buffer[i]);
            }
						
            if (recOK)
            {
								//数据处理
								TDSData_Process();
                recOK = 0;
            }
//						rt_kprintf("rcv_buffer[0]=%x,rcv_buffer[1]=%x,rcv_buffer[2]=%x,rcv_buffer[12]=%x,rcv_buffer[13]=%x,u16Buff=%x,recOK=%x\n",rcv_buffer[0],rcv_buffer[1],rcv_buffer[2],rcv_buffer[12],rcv_buffer[13],u16Buff,recOK);

            /* 清空数据 */
            rt_memset(rcv_buffer, 0, sizeof(rcv_buffer));
        }
				else
				{
									u16Buff0|=0x04;
						if(u8TX_Delay>=2)
						{
											u16Buff0|=0x08;
								u8TX_Delay=0;
								rt_device_write(serial, 0, tx_buffer, COM_LEN);	
//								rt_kprintf("u16Buff=%x,recOK=%x,u8TX_Delay=%x,rx_length=%d,result=%d\n",u16Buff0,recOK,u8TX_Delay,rx_length,result);
						}
						
				}
				u8TX_Delay++;
        rt_thread_delay(10);
    }
}

