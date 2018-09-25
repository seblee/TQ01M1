/*
 * File      : rtc.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version.
 * 2011-11-26     aozima       implementation time.
 */

#include <rtthread.h>
#include <stm32f10x.h>
#include "rtc_bsp.h"

static time_t current_timestamp = 0;
static rt_mutex_t time_mutex = RT_NULL;
static void current_timestamp_tick(void);

static struct rt_device rtc;
static rt_err_t rt_rtc_open(rt_device_t dev, rt_uint16_t oflag)
{
    if (dev->rx_indicate != RT_NULL)
    {
        /* Open Interrupt */
    }

    return RT_EOK;
}

static rt_size_t rt_rtc_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    return 0;
}

rt_err_t rt_rtc_control(rt_device_t dev, rt_uint8_t cmd, void *args) //modified by GP
{
    rt_time_t *time;
    RT_ASSERT(dev != RT_NULL);

    switch (cmd)
    {
    case RT_DEVICE_CTRL_RTC_GET_TIME:
        time = (rt_time_t *)args;
        /* read device */
        *time = RTC_GetCounter();
        break;

    case RT_DEVICE_CTRL_RTC_SET_TIME:
    {
        time = (rt_time_t *)args;

        /* Enable PWR and BKP clocks */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);

        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        /* Change the current time */
        RTC_SetCounter(*time);

        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();

        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
    }
    break;
    }

    return RT_EOK;
}

/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : Configures the RTC.
* Input          : None
* Output         : None
* Return         : 0 reday,-1 error.
*******************************************************************************/
int RTC_Configuration(void)
{
    u32 count = 0x200000;

    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    /* Reset Backup Domain */
    BKP_DeInit();

    /* Enable LSE */
    RCC_LSEConfig(RCC_LSE_ON);
    /* Wait till LSE is ready */
    while ((RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) && (--count))
        ;
    if (count == 0)
    {
        return -1;
    }

    /* Select LSE as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Set RTC prescaler: set RTC period to 1sec */
    RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    return 0;
}

void drv_rtc_init(void)
{
    rtc.type = RT_Device_Class_RTC;

    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
        rt_kprintf("rtc is not configured\n");
        rt_kprintf("please configure with set_date and set_time\n");
        if (RTC_Configuration() != 0)
        {
            rt_kprintf("rtc configure fail...\r\n");
            return;
        }
    }
    else
    {
        /* Wait for RTC registers synchronization */
        RTC_WaitForSynchro();
    }

    /* register rtc device */
    rtc.init = RT_NULL;
    rtc.open = rt_rtc_open;
    rtc.close = RT_NULL;
    rtc.read = rt_rtc_read;
    rtc.write = RT_NULL;
    rtc.control = rt_rtc_control;

    /* no private */
    rtc.user_data = RT_NULL;

    rt_device_register(&rtc, "rtc", RT_DEVICE_FLAG_RDWR);

    return;
}

#include <time.h>
#if defined(__IAR_SYSTEMS_ICC__) && (__VER__) >= 6020000 /* for IAR 6.2 later Compiler */
#pragma module_name = "?time"
time_t(__time32)(time_t *t) /* Only supports 32-bit timestamp */
#else
time_t time(time_t *t)
#endif
{
    rt_device_t device;
    time_t time = 0;

    device = rt_device_find("rtc");
    if (device != RT_NULL)
    {
        rt_device_control(device, RT_DEVICE_CTRL_RTC_GET_TIME, &time);
        if (t != RT_NULL)
            *t = time;
    }

    return time;
}
void get_local_time(time_t *t)
{
    time(t);
}

#ifdef RT_USING_FINSH
#include <finsh.h>

void set_date(uint32_t year, uint32_t month, uint32_t day)
{
    time_t now;
    struct tm *ti;
    rt_device_t device;

    ti = RT_NULL;
    /* get current time */
    time(&now);

    ti = localtime(&now);
    if (ti != RT_NULL)
    {
        ti->tm_year = year - 1900;
        ti->tm_mon = month - 1; /* ti->tm_mon 	= month; 0~11 */
        ti->tm_mday = day;
    }

    now = mktime(ti);

    device = rt_device_find("rtc");
    if (device != RT_NULL)
    {
        rt_rtc_control(device, RT_DEVICE_CTRL_RTC_SET_TIME, &now);
    }
}
FINSH_FUNCTION_EXPORT(set_date, set date.e.g
                      : set_date(2010, 2, 28))

void set_time(uint32_t hour, uint32_t minute, uint32_t second)
{
    time_t now;
    struct tm *ti;
    rt_device_t device;

    ti = RT_NULL;
    /* get current time */
    time(&now);

    ti = localtime(&now);
    if (ti != RT_NULL)
    {
        ti->tm_hour = hour;
        ti->tm_min = minute;
        ti->tm_sec = second;
    }

    now = mktime(ti);
    now -= 28800;
    device = rt_device_find("rtc");
    if (device != RT_NULL)
    {
        rt_rtc_control(device, RT_DEVICE_CTRL_RTC_SET_TIME, &now);
    }
}
FINSH_FUNCTION_EXPORT(set_time, set time.e.g
                      : set_time(23, 59, 59))

void list_date(void)
{
    time_t now;

    time(&now);
    now += 28800;

    rt_kprintf("%s\n", ctime(&now));
}
FINSH_FUNCTION_EXPORT(list_date, show date and time.)
#endif

void Rtc_sts_update(sys_reg_st *gds_sys_ptr)
{
    time_t now;
    struct tm *ti;

    time(&now);
    ti = localtime(&now);

    gds_sys_ptr->status.ComSta.Sys_Time.Year = ti->tm_year + 1900;
    gds_sys_ptr->status.ComSta.Sys_Time.Mon = ti->tm_mon + 1;
    gds_sys_ptr->status.ComSta.Sys_Time.Day = ti->tm_mday;
    gds_sys_ptr->status.ComSta.Sys_Time.Hour = ti->tm_hour;
    gds_sys_ptr->status.ComSta.Sys_Time.Min = ti->tm_min;
    gds_sys_ptr->status.ComSta.Sys_Time.Sec = ti->tm_sec;
    gds_sys_ptr->status.ComSta.Sys_Time.Weekday = ti->tm_wday;

    gds_sys_ptr->status.ComSta.Sys_Time.u32Systime = now;
    //		  rt_kprintf("now= %d\n",now);
    //	  rt_kprintf("Year= %d,Mon= %d,Day= %d\n",ti->tm_year+1900,ti->tm_mon,ti->tm_mday);
    //    rt_kprintf("Hour= %d,Min= %d,Sec= %d\n",ti->tm_hour,ti->tm_min,ti->tm_sec);

    return;
}

/**
 ****************************************************************************
 * @Function : static void current_timestamp_tick(void)
 * @File     : rtc_bsp.c
 * @Program  : none
 * @Created  : 2018-09-25 by seblee
 * @Brief    : current_timestamp += 1
 * @Version  : V1.0
**/
static void current_timestamp_tick(void)
{
    rt_err_t rc;
    if (time_mutex == RT_NULL)
        time_mutex = rt_mutex_create("timestamp_mutex", RT_IPC_FLAG_FIFO);

    rc = rt_mutex_take(time_mutex, RT_WAITING_FOREVER);
    if (rc != RT_EOK)
    {
        return;
    }
    current_timestamp += 1;
    rc = rt_mutex_release(time_mutex);
}
/**
 ****************************************************************************
 * @Function : void current_timestamp_set(time_t value)
 * @File     : rtc_bsp.c
 * @Program  : valut current time
 * @Created  : 2018-09-25 by seblee
 * @Brief    : set current time
 * @Version  : V1.0
**/
void current_timestamp_set(time_t value)
{
    rt_err_t rc;
    if (time_mutex == RT_NULL)
        time_mutex = rt_mutex_create("timestamp_mutex", RT_IPC_FLAG_FIFO);

    rc = rt_mutex_take(time_mutex, RT_WAITING_FOREVER);
    if (rc != RT_EOK)
    {
        return;
    }
    current_timestamp = value;
    rc = rt_mutex_release(time_mutex);
}
