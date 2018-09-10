/**
 ****************************************************************************
 * @Warning :Without permission from the author,Not for commercial use
 * @File    :
 * @Author  :Seblee
 * @date    :2018-09-10 10:16:41
 * @version :V1.0.0
 *************************************************
 * @brief   :
 ****************************************************************************
 * @Last Modified by: Seblee
 * @Last Modified time: 2018-09-10 17:02:45
 ****************************************************************************
**/
#ifndef __SIM7600_H_
#define __SIM7600_H_
/* Private include -----------------------------------------------------------*/
#include "sys_conf.h"
#include <rtthread.h>
#include "board.h"
/* Private typedef -----------------------------------------------------------*/
/* UART接收消息结构*/

/* Private define ------------------------------------------------------------*/
#define SIM7600_DIR_PORT UART3_DIR_GPIO
#define SIM7600_DIR_PIN UART3_DIR_GPIO_PIN

#define SIM7600_DIR_WIFI GPIO_SetBits(SIM7600_DIR_PORT, SIM7600_DIR_PIN)
#define SIM7600_DIR_4G GPIO_ResetBits(SIM7600_DIR_PORT, SIM7600_DIR_PIN)

#define aliyun_domain "%s.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define aliyun_iot_port 1883
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
extern rt_mq_t rx_mq;
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
rt_uint32_t sim7600_send_message(rt_device_t dev, const char *senddata, rt_uint8_t **data);
/*----------------------------------------------------------------------------*/

#endif
