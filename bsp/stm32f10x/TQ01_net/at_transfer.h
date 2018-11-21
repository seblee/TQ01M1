/**
 ****************************************************************************
 * @Warning :Without permission from the author,Not for commercial use
 * @File    :
 * @Author  :Seblee
 * @date    :2018-09-10 10:27:53
 * @version :V1.0.0
 *************************************************
 * @brief   :
 ****************************************************************************
 * @Last Modified by: Seblee
 * @Last Modified time: 2018-09-10 18:12:48
 ****************************************************************************
**/
#ifndef __AT_TRANSFER_H_
#define __AT_TRANSFER_H_
/* Private include -----------------------------------------------------------*/
#include "sys_conf.h"
#include <rtthread.h>
/* Private typedef -----------------------------------------------------------*/
typedef enum
{
    CIPMODE = 0,
    CWMODE_DEF,
    CWAUTOCONN,
    CIPSEND,
} __at_command_t;

typedef enum
{
    REMOTE_TCP = 0,
    REMOTE_UDP,
    REMOTE_SSL,
} __wifi_remote_t;
/* Private define ------------------------------------------------------------*/
#ifndef AT_HEADER
#define AT_HEADER "AT"
#endif /* AT_HEADER */
#define AT_WIFI_REMOTE_REC "+IPD"

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
extern const char AT_WIFI_SYNC[];
extern const char AT_WIFI_CIPSEND[];
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
rt_err_t at_wifi_init(rt_device_t dev);

rt_err_t at_4g_init(rt_device_t dev);

rt_err_t at_wifi_send_message_ack(rt_device_t dev, const char *AT_command, const char *pKeyword);

rt_err_t at_wifi_get_cipstatus(rt_device_t dev);

rt_err_t at_wifi_CIPSTART(rt_device_t dev, __wifi_remote_t type, char *host, int port);

rt_err_t at_wifi_set_CIPMODE_mode(rt_device_t dev, rt_uint8_t value);

rt_err_t at_wifi_https(rt_device_t dev, char *host, int port, char *request, char **response);

rt_err_t at_4g_https(rt_device_t dev, char *host, int port, char *request, char **response);

/*----------------------------------------------------------------------------*/

#endif
