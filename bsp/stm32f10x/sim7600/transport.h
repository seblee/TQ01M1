/**
 ****************************************************************************
 * @Warning :Without permission from the author,Not for commercial use
 * @File    :
 * @Author  :Seblee
 * @date    :2018-09-06 17:22:33
 * @version :V1.0.0
 *************************************************
 * @brief   :
 ****************************************************************************
 * @Last Modified by: Seblee
 * @Last Modified time: 2018-09-06 17:23:51
 ****************************************************************************
**/
#ifndef __TRANSPORT_H_
#define __TRANSPORT_H_
/* Private include -----------------------------------------------------------*/
#include <rtthread.h>

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
rt_size_t transport_sendPacketBuffer(rt_device_t dev, unsigned char *buf, int buflen);

int transport_getdata(unsigned char *buf, int count);

int transport_open(char *addr, int port);

int transport_close(void);

/*----------------------------------------------------------------------------*/

#endif