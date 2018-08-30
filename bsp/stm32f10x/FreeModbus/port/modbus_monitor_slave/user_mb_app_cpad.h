#ifndef	USER_APP
#define USER_APP
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb_monitor.h"
#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbutils.h"
#include "sys_conf.h"
/* -----------------------Slave Defines -------------------------------------*/
#define 				 MONITOR_REG_HOLDING_START           0
#define 				 MONITOR_REG_HOLDING_NREGS           37
#define 				 REG_HOLDING_WRITE_NREGS       5//可写范围+
//从机模式：在保持寄存器中，各个地址对应的功能定义
#define          S_HD_RESERVE                     0		  //保留
#define          S_HD_CPU_USAGE_MAJOR             1         //当前CPU利用率的整数位
#define          S_HD_CPU_USAGE_MINOR             2         //当前CPU利用率的小数位

//从机模式：在输入寄存器中，各个地址对应的功能定义
#define          S_IN_RESERVE                     0		  //保留




void mbm_sts_update(sys_reg_st* gds_ptr);
void mbs_sts_update(void);// 更新本地变量到协议栈寄存器中
#endif
