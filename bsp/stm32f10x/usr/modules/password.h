#ifndef _PASSWORD_H
#define _PASSWORD_H

#include "sys_conf.h"
enum
{
	WORK_MODE_FSM_OPEN = 0xE1,//完全授权
	WORK_MODE_FSM_MANAGE = 0xD2,//限时管控
	WORK_MODE_FSM_LOCK = 0xC3,//开机管控
	WORK_MODE_FSM_LIMIT = 0xB4,//限时运行
};

enum//剩余时间
{
	WORK_REMAIN_0=0x00,//0天
	WORK_REMAIN_1=0x01,//1天
	WORK_REMAIN_3=0x03,//3天
	WORK_REMAIN_7=0x07,//7天
	WORK_REMAIN_15=0x0F,//15天
	WORK_REMAIN_30=0x1E,//30天
};

enum//限时剩余时间
{
	WORK_LIMIT_ZERO=0x01,//0天
	WORK_LIMIT_DAY_1=0x02,//1天
	WORK_LIMIT_DAY_3=0x04,//3天
	WORK_LIMIT_DAY_7=0x08,//7天
	WORK_LIMIT_DAY_15=0x10,//15天
	WORK_LIMIT_DAY_30=0x20,//30天
	WORK_LIMIT_LOCK=0x100,//锁屏
	WORK_LIMIT_CATION=0x400,//告警提醒,90号告警状态字
	WORK_PASS_CLEAR=0x3F,//清除告警
	WORK_PASS_ALL=0x13F,//清除告警
	WORK_PASS_SET=0x1000,//
};
#define LIMIT_DAY_MAX 1000
//5级加密管控
enum
{
	GRADE_POWERON = 0xA1,//阶段0，开机
	GRADE_1 = 0xB2,//阶段1
	GRADE_2 = 0xC3,//阶段2
	GRADE_3 = 0xD4,//阶段3
	GRADE_4 = 0xE5,//阶段4
	GRADE_LOCK = 0xF6,//锁定关机
	GRADE_OPEN = 0x6F,//完全授权
};

void init_work_mode(void);

void work_mode_manage(void);

uint8_t passward_compare(uint8_t *st1,uint8_t *st2, uint8_t len);
uint8_t cpad_work_mode(uint8_t work_mode,uint16_t day_limit);

uint8_t  write_passward (uint8_t*password, uint8_t work_mode,uint16 limit_time);
uint8_t get_work_mode_power_state(void);

uint8_t  Write_Controlpassward (uint16_t*MBBuffer);
uint8_t  Passward_Verify(uint16_t MBBuff);
#endif

