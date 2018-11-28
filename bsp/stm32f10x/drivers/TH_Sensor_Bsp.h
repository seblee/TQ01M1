#ifndef _TH_SENSOR_BSP_H
#define _TH_SENSOR_BSP_H
#include "stm32f10x.h"
#include "sys_conf.h"
/******************************************************************************************************/
/******************************************************************************************************/
/*************************************IIC_AM2311 Temperature&Humidity**********************************/
/******************************************************************************************************/
/******************************************************************************************************/

#define ERROR_CNT_MAX 20 //异常次数
#define TEMP_OFFSET 50
#define HUM_OFFSET 100

//#define II_AM_SDA_00_Pin		GPIO_Pin_9
//#define II_AM_SDA_00_GPIO		GPIOD

//#define II_AM_SDA_01_Pin		GPIO_Pin_8
//#define II_AM_SDA_01_GPIO		GPIOD

#define II_AM_SDA_00_Pin GPIO_Pin_4
#define II_AM_SDA_00_GPIO GPIOB

#define II_AM_SDA_01_Pin GPIO_Pin_3
#define II_AM_SDA_01_GPIO GPIOB

//#define AM_SDA_READ()   GPIO_ReadInputDataBit(II_AM_SDA_GPIO,II_AM_SDA_Pin)
//#define	AM_SDA_H	   GPIO_SetBits(II_AM_SDA_GPIO,II_AM_SDA_Pin)
//#define	AM_SDA_L	   GPIO_ResetBits(II_AM_SDA_GPIO,II_AM_SDA_Pin)

extern void AM_Init(void);
extern uint8_t Read_Sensor(uint16_t *u16TH_Buff, uint8_t u8SN);
extern uint8_t AM_Sensor_update(sys_reg_st *gds_ptr);
#endif
