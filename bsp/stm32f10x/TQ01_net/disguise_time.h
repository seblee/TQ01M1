/**
 ****************************************************************************
 * @Warning :Without permission from the author,Not for commercial use
 * @File    :disguise_time.h
 * @Author  :Seblee
 * @date    :2018-09-26 11:38:01
 * @version :V1.0.0
 *************************************************
 * @brief   :
 ****************************************************************************
 * @Last Modified by: Seblee
 * @Last Modified time: 2018-09-26 14:06:58
 ****************************************************************************
**/
#ifndef __DISGUISE_TIME_H_
#define __DISGUISE_TIME_H_
/* Private include -----------------------------------------------------------*/
#include <rtthread.h >
#include "sys_conf.h"
#include "time.h"
/* Private typedef -----------------------------------------------------------*/
struct timeval
{
    long tv_sec;  /* seconds */
    long tv_usec; /* and microseconds */
};

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void current_timestamp_tick(void);

void current_systime_get(struct tm *ti);

void get_bulid_date_time(struct tm *r);

void current_systime_set(struct tm *ti);

int gettimeofday(struct timeval *tp, void *ignore);
/*----------------------------------------------------------------------------*/

#endif /* DISGUISE_TIME*/
