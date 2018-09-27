/**
 ****************************************************************************
 * @Warning :Without permission from the author,Not for commercial use
 * @File    :disguise_time.c
 * @Author  :Seblee
 * @date    :2018-09-26 11:37:56
 * @version :V1.0.0
 *************************************************
 * @brief   :
 ****************************************************************************
 * @Last Modified by: Seblee
 * @Last Modified time: 2018-09-26 14:07:41
 ****************************************************************************
**/
/* Private include -----------------------------------------------------------*/
#include "disguise_time.h"
#include <stdio.h>
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

time_t current_timestamp;
rt_mutex_t time_mutex;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

/**
 ****************************************************************************
 * @Function :   void current_timestamp_tick(void)
 * @File     : rtc_bsp.c
 * @Program  : none
 * @Created  : 2018-09-25 by seblee
 * @Brief    : current_timestamp += 1
 * @Version  : V1.0
**/
void current_timestamp_tick(void)
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
static void current_timestamp_set(time_t value)
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
/**
 ****************************************************************************
 * @Function : time_t current_timestamp_get(void)
 * @File     : rtc_bsp.c
 * @Program  : none
 * @Created  : 2018-09-26 by seblee
 * @Brief    : get current timestamp
 * @Version  : V1.0
**/
static time_t current_timestamp_get(void)
{
    time_t rc;
    if (time_mutex == RT_NULL)
    {
        time_mutex = rt_mutex_create("timestamp_mutex", RT_IPC_FLAG_FIFO);
    }

    rt_mutex_take(time_mutex, RT_WAITING_FOREVER);
    rc = current_timestamp;
    rt_mutex_release(time_mutex);
    return rc;
}
/**
 ****************************************************************************
 * @Function : void current_systime_get(void)
 * @File     : rtc_bsp.c
 * @Program  : none
 * @Created  : 2018-09-26 by seblee
 * @Brief    : 
 * @Version  : V1.0
**/
struct tm *gmtime_con(const time_t *timep, struct tm *r);
void current_systime_get(struct tm *ti)
{
    time_t now = current_timestamp_get();

    gmtime_con(&now, ti);
}
/**
 ****************************************************************************
 * @Function : void current_systime_set(void)
 * @File     : rtc_bsp.c
 * @Program  : none
 * @Created  : 2018-09-26 by seblee
 * @Brief    : 
 * @Version  : V1.0
**/
void current_systime_set(struct tm *ti)
{
    time_t now;
    now = mktime(ti);
    current_timestamp_set(now);
}
const short __spm[13] = {
    0,
    (31),
    (31 + 28),
    (31 + 28 + 31),
    (31 + 28 + 31 + 30),
    (31 + 28 + 31 + 30 + 31),
    (31 + 28 + 31 + 30 + 31 + 30),
    (31 + 28 + 31 + 30 + 31 + 30 + 31),
    (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31),
    (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30),
    (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31),
    (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30),
    (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31),
};
int __isleap(int year)
{
    /* every fourth year is a leap year except for century years that are
	 * not divisible by 400. */
    /*  return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)); */
    return (!(year % 4) && ((year % 100) || !(year % 400)));
}
/**
 ****************************************************************************
 * @Function : struct tm *gmtime_con(const time_t *timep, struct tm *r)
 * @File     : bkg_thread.c
 * @Program  : none
 * @Created  : 2018-09-26 by seblee
 * @Brief    : 
 * @Version  : V1.0
**/
struct tm *gmtime_con(const time_t *timep, struct tm *r)
{
    time_t i;
    register time_t work = *timep % (86400);
    r->tm_sec = work % 60;
    work /= 60;
    r->tm_min = work % 60;
    r->tm_hour = work / 60;
    work = *timep / (86400);
    r->tm_wday = (4 + work) % 7;
    for (i = 1970;; ++i)
    {
        register time_t k = __isleap(i) ? 366 : 365;
        if (work >= k)
            work -= k;
        else
            break;
    }
    r->tm_year = i - 1900;
    r->tm_yday = work;

    r->tm_mday = 1;
    if (__isleap(i) && (work > 58))
    {
        if (work == 59)
            r->tm_mday = 2; /* 29.2. */
        work -= 1;
    }

    for (i = 11; i && (__spm[i] > work); --i)
        ;
    r->tm_mon = i;
    r->tm_mday += work - __spm[i];
    return r;
}
const unsigned char MonthStr[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
/**
 ****************************************************************************
 * @Function : void get_bulid_date_time(struct tm *r)
 * @File     : disguise_time.c
 * @Program  : none
 * @Created  : 2018-09-26 by seblee
 * @Brief    : 
 * @Version  : V1.0
**/
void get_bulid_date_time(struct tm *r)
{
    unsigned char temp_str[4] = {0, 0, 0, 0}, i = 0;

    sscanf(__DATE__, "%s %d %d", temp_str, &(r->tm_mday), &(r->tm_year));
    sscanf(__TIME__, "%d:%d:%d", &(r->tm_hour), &(r->tm_min), &(r->tm_sec));
    for (i = 0; i < 12; i++)
    {
        if (temp_str[0] == MonthStr[i][0] && temp_str[1] == MonthStr[i][1] && temp_str[2] == MonthStr[i][2])
        {
            r->tm_mon = i + 1;
            break;
        }
    }
    r->tm_year -= 1900;
}
