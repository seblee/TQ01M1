/**
 ****************************************************************************
 * @Warning Without permission from the author,Not for commercial use
 * @File    ble_key.c
 * @Author  xiaowine@cee0.com
 * @date    
 * @version V1.0
 *************************************************
 * @brief   标注系统信息
 ****************************************************************************
 * @attention 
 * Powered By Xiaowine
 * <h2><center>&copy;  Copyright(C) cee0.com 2015-2019</center></h2>
 * All rights reserved
 * 
**/

#include <rtthread.h>
#include <rtdevice.h>
#include "ble_key.h"
#include "i2c_utils.h"

#ifdef I2C_TOOLS_USE_SW_I2C
#define SDA_PORT_NUM 79
#define SCL_PORT_NUM 78

#else
#define I2C_DEVICE_NAME "i2c1"
static struct rt_i2c_bus_device *i2c_bus = RT_NULL; /* I2C总线设备句柄 */
#endif

static void i2c_thread_entry(void *para)
{
    rt_uint8_t buf[100] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    rt_kprintf("i2c_thread_entry start \n");
    while (1)
    {
        /* 调用I2C设备接口传输数据 */
        if (i2c_write(12, buf, 10) == 1)
        {
            rt_kprintf("RT_I2C_WR OK \n");
        }
        else
        {
            rt_kprintf("RT_I2C_WR err \n");
        }
        rt_thread_delay(rt_tick_from_millisecond(1000));
    }
}

int i2cBleThreadInit(void)
{
#ifdef I2C_TOOLS_USE_SW_I2C
    if (i2c_init(SDA_PORT_NUM, SCL_PORT_NUM))
    {
        rt_kprintf("[i2c] failed to find bus with sda=%d scl=%d\n", SDA_PORT_NUM, SCL_PORT_NUM);
        return RT_ERROR;
    }
#else
    char name[RT_NAME_MAX];
    rt_strncpy(name, I2C_DEVICE_NAME, RT_NAME_MAX);
    if (i2c_init(name))
    {
        rt_kprintf("[i2c] failed to find bus %s\n", name);
        return;
    }

#endif
    /* 创建 i2c 线程 */
    rt_thread_t thread = rt_thread_create("i2c", i2c_thread_entry, RT_NULL, 1024, 25, 10);
    /* 创建成功则启动线程 */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        rt_kprintf("thread_create i2c err \n");
        return RT_ERROR;
    }
    return RT_EOK;
}
INIT_APP_EXPORT(i2cBleThreadInit);
