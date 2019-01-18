/*
 * File      : rtc.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

#ifndef __RTC_H__
#define __RTC_H__
#include <rtthread.h>
#include "sys_conf.h"
void drv_rtc_init(void);
rt_err_t rt_rtc_control(rt_device_t dev, rt_uint8_t cmd, void *args);
//void rtc_sts_update(sys_reg_st*	gds_sys_ptr);
void set_date(uint32_t year, uint32_t month, uint32_t day);
void set_time(uint32_t hour, uint32_t minute, uint32_t second);

void get_local_time(time_t *t);
void list_date(void);
void Rtc_sts_update(sys_reg_st *gds_sys_ptr);
#endif //__RTC_H__
