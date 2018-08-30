#ifndef __EVENT_RECORD_H__
#define __EVENT_RECORD_H__
#include "sys_conf.h"
#include "cyc_buff.h"

//报警记录

#define CRITICAL_ALARM_lEVEL     0x00//严重告警
#define MAJOR_ALARM_LEVEL        0x01//一般告警
#define MIOOR_ALARM_LEVEL        0x02//提示告警



#define ALARM_FIFO_DEPTH  30

#define ALARM_STATUS_LEN    (sizeof(alram_node) - 4)

#define ALARM_RECORD_LEN    sizeof(alarm_log_st)

#define EVENT_RECORD_LEN    sizeof(event_log_st)
#define TEMP_HUM_RECORD_LEN  4*60

//事件记录
#define EVE_MAX_CNT 500 //事件类记录总条数

#define EVENT_FIFO_DEPTH 64



//yxq
#pragma pack (1)

// typedef struct
// {
//	  uint16_t startpiont;
//	  uint16_t endpiont;
//	  uint16_t cnt;
// }record_pt_st;


typedef struct
{
	uint32_t trigger_time;
	uint16_t alarm_value;
	uint16_t alarm_id;
}alarm_table_st;


//事件记录结构体；
typedef struct
{
	uint16_t event_id;
	uint32_t time;
	uint16_t user_id;
	uint16_t former_data;
	uint16_t new_data;
} event_log_st;

//报警记录结构体
typedef struct
{
	uint16_t alarm_id;
	uint32_t trigger_time;
	uint32_t end_time;
	uint16_t rev;
}alarm_log_st;

typedef struct node
{
	uint32_t trigger_time;
	uint16_t  alarm_id;
	uint16_t alarm_value;
	struct node* next;
}alram_node;


#pragma pack ()


typedef enum
{
	USER_DEFAULT =0,
	USER_MODEBUS_SLAVE,
	USER_CPAD,
	USER_ADMIN,
}user_ID;


typedef enum
{
	ALARM_TRIGER=0,
	ALARM_END,
	
}alarm_enum;

enum
{
	EVENT_TYPE=0,//操作类事件记录
	ALARM_TYPE,//报警事件记录
	TEM_HUM_TYPE,
};


enum
{
	HOUR_TIMER=0,
	DATE_TIMER,
	WEEK_TIMER,
	MONTH_TIMER,
	TIMER_TOTAL_NUM,
};


uint8_t clear_log(uint8_t flag);
void add_alarmlog_fifo( uint16_t alarm_id, alarm_enum flg,uint16  alarm_value);
void user_alarmlog_add(void);
void  init_evnet_log(void);




void  init_alarm_log(void);

void user_eventlog_add(void);

void add_eventlog_fifo( uint16 event_id,uint16 user_id,uint16 former_data, uint16 new_data);


//获取事件记录
uint16_t  get_log(uint8_t*log_data,uint16_t start_num,uint16_t cont,uint16_t* total_cnt,uint8_t log_type);


//告警状态先关函数接口

void chain_init(void);

uint8_t  node_append(uint16_t alarm_id,uint16_t alarm_value);

uint8_t node_delete(uint16_t alarm_id);
//test_shell

uint8_t get_alarm_status(uint8_t*status_data,uint16_t start_num,uint8_t len);


//温湿度曲线相关函数接口
void  init_tem_hum_record(void);
void add_hum_temp_log(void);
uint8_t clear_tem_hum_log(void);
uint8_t get_tem_hum_log(uint8_t*log_data,uint16_t block);

#endif //__ALARM_ACL_H__

