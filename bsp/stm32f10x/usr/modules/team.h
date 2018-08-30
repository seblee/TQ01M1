#ifndef __TEAM
#define	__TEAM

#include "sys_conf.h"
#define 	TEAM_MAX_SLAVE_NUM					32
#define 	TEAM_BROADCAST_ADDR					0
#define 	TEAM_TAB_TIMEOUT_CNT				10

#define		TEAM_CONF_RS_BPOS						0x0f
#define		TEAM_CONF_TMODE_BPOS				0x0e
#define		TEAM_CONF_HMODE_BPOS				0x0d
#define		TEAM_CONF_TCMODE_BPOS				0x0c
#define		TEAM_CONF_TMMOD_BPOS				0x0a
#define		TEAM_CONF_TDIRECT_BPOS			0x08
#define		TEAM_CONF_TEAM_FAN_MODE_BPOS			0x06
#define		TEAM_CONF_ROTATE_BPOS				0x05
#define		TEAM_CONF_CASCADE_BPOS			0x04
#define		TEAM_CONF_TARGET_BPOS				0x02
#define		TEAM_CONF_TEAM_POWER_OFF_EN	0x01

//#define		TEAM_MASTER_PARAM_SET			0x02
//#define		TEAM_MASTER_RUN_STS_SET		0x03
//#define		TEAM_MASTER_STS_QEURY			0x04
//#define		TEAM_SLAVE_LOCAL_STS			0x81
#define		TEAM_STS_CONFD_BPOS					0x05
#define		TEAM_STS_ALARM_BPOS					0x04
#define		TEAM_STS_FSM_BPOS						0x01
#define		TEAM_STS_RUN_BPOS						0x00
#define   TEAM_STS_HEATING_STS_BPOS   0x06
#define   TEAM_STS_COOLING_STS_BPOS   0x07
#define   TEAM_STS_HUMING_STS_BPOS    0x08
#define   TEAM_STS_DEMHUM_STS_BPOS    0x09
#define   TEAM_STS_TEAM_MOD_BPOS      0x0A

#define   ROTATE_TIMEOUT_CNT                  70
#define   CHECK_TEAM_CONFIG_TIMEOUT           10
#define   SLAVE_TO_MASTER_PARAM_PROTECT_CNT   3
#define   SYNC_PARAM_PROTECT_CNT              3
#define   MASTER_SEND_PARAM_PROTECT_CNT       45
#define		SYNC_POWERON_TIMEOUT								60

#define		TEAM_HUM_REQ_HUM										0
#define		TEAM_TEMP_REQ_TEMP									1
#define		TEAM_FAN_REQ_TEMP										2

typedef enum
{
		TEAM_MODE_STANDALONE=0,
		TEAM_MODE_EVENDISTRIB,
		TEAM_MODE_UINIDIRECTION,
		TEAM_MODE_REQDISTRIB		
}team_mode_em;


typedef enum
{
		TEAM_CMD_MASTER_REQ_DISTB=1,
		TEAM_CMD_MASTER_PARAM_SET,	
		TEAM_CMD_MASTER_RUN_STS_SET,	
		TEAM_CMD_MASTER_STS_QEURY,	
		TEAM_CMD_SLAVE_QUERY_RESP,		
		TEAM_CMD_LOCAL_STS,
    TEAM_CMD_MASTER_EX_TEMP_PARAM_SET,
    TEAM_CMD_MASTER_EX_HUM_PARAM_SET,
		TEAM_CMD_MASTER_RUN_FINAL_STS_SET,
    TEAM_CMD_MASTER_CLEAR_CONFD_STS_SET
}team_cmd_em;


typedef enum
{
		TEAM_FSM_IDLE = 0,
		TEAM_FSM_SYNC,
		TEAM_FSM_SLAVE,
		TEAM_FSM_MASTER,
}team_fsm_em;

typedef enum
{
		TEAM_SIG_STOP = 0,
		TEAM_SIG_SYNC,
		TEAM_SIG_SLAVE_EN,
		TEAM_SIG_MASTER_EN,
}team_signal_em;


typedef enum
{
		TEAM_TAB_STATUS = 0,
		TEAM_TAB_TEMP,
		TEAM_TAB_HUM,
		TEAM_TAB_RESERVE,
		TEAM_TAB_RUN_TIME,
		TEAM_TAB_STOP_TIME,
		TEAM_TAB_TIMEOUT,
    TEAM_TAB_TEMP_REQ,
    TEAM_TAB_HUM_REQ,
		TEAM_TAB_MAX
}team_pkt_em;

#define TEAM_MASTER_CONF					TEAM_TAB_STATUS
#define TEAM_MASTER_TEMP_REQ			TEAM_TAB_TEMP
#define TEAM_MASTER_HUM_REQ				TEAM_TAB_HUM
#define TEAM_MASTER_TIMEOUT				TEAM_TAB_TIMEOUT
	
	
typedef enum
{
		TEAM_ROTATE_PERIOD_NONE=0,
		TEAM_ROTATE_PERIOD_DAY,
		TEAM_ROTATE_PERIOD_WEEK,
		TEAM_ROTATE_PERIOD_HOUR,
}team_rotate_period_em;

typedef enum
{
		TEAM_CONF_MODE = 0,
		TEAM_CONF_TEMP_PRECISION,
		TEAM_CONF_TEMP_DEADBAND,
		TEAM_CONF_TEMP_SETVAL,
		TEAM_CONF_HUM_PRECISION,
		TEAM_CONF_HUM_DEADBAND,
		TEAM_CONF_HUM_SETVAL,
		TEAM_CONF_ALARM_HI_TEMP_PARAM,
		TEAM_CONF_ALARM_LO_TEMP_PARAM,
		TEAM_CONF_ALARM_HI_HUM_PARAM,
		TEAM_CONF_ALARM_LO_HUM_PARAM,	
		TEAM_CONF_CNT,
		TEAM_CONF_ROTATE_PERIOD,
		TEAM_CONF_ROTATE_TIME,
		TEAM_CONF_FAN_MODE,
		TEAM_CONF_FAN_TARGET_TEMP,
		TEAM_CONF_FAN_TEMP_DEADBAND,
		TEAM_CONF_FAN_TEMP_PRECISION,
		TEAM_CONF_FAN_SET_FLOW_DIFF,
		TEAM_CONF_FAN_FLOW_DIFF_DEADZONE,
		TEAM_CONF_FAN_FLOW_DIFF_STEP,
		TEAM_CONF_FAN_FLOW_DIFF_DELAY,
    TEAM_CONF_RESERVE,
		TEAM_CONF_MAX
}team_conf_em;

typedef enum
{
		TEAM_PARAM_MODE = 0,
		TEAM_PARAM_TEMP_REQ,
		TEAM_PARAM_HUM_REQ,
		TEAM_PARAM_FAN_REQ,
		TEAM_PARAM_RESERVE,
    TEAM_PARAM_TOTAL_TEMP_REQ,
    TEAM_PARAM_TOTAL_HUM_REQ,
    TEAM_PARAM_AVER_TEMP_REQ,
    TEAM_PARAM_AVER_HUM_REQ,
    TEAM_PARAM_DIFF_TEMP_REQ,
    TEAM_PARAM_DIFF_HUM_REQ,
		TEAM_PARAM_MAX,
}team_param_em;


#define 	TEAM_CMD_LEN_MASTER_REQ_DISTB			8
#define 	TEAM_CMD_LEN_MASTER_PARAM_SET			7
#define 	TEAM_CMD_LEN_MASTER_RUN_STS_SET		4
#define 	TEAM_CMD_LEN_MASTER_STS_QEURY			2
#define 	TEAM_CMD_LEN_SLAVE_LOCAL_STS			8
#define 	TEAM_CMD_LEN_SLAVE_CONF_STS				7
#define   TEAM_CMD_LEN_MASTER_EX_TEMP_PARAM_SET   6
#define   TEAM_CMD_LEN_MASTER_EX_HUM_PARAM_SET    6
#define   TEAM_CMD_LEN_MASTER_RUN_FINAL_STS_SET   8
#define   TEAM_CMD_LEN_MASTER_CLEAR_CONFD_STS_SET 0

typedef enum
{
		TEAM_BITMAP_RS_SET=0,
		TEAM_BITMAP_RS_STS,
		TEAM_BITMAP_MALFUN_DEV,
    TEAM_BITMAP_ONLINE,
    TEAM_BITMAP_CONFD,
    TEAM_BITMAP_OUTPUT,
    TEAM_BITMAP_FINAL_OUT,
    TEAM_BITMAP_TEMP_FULL_REQ_OUT,
    TEAM_BITMAP_HUM_FULL_REQ_OUT,
		TEAM_BITMAP_MAX
}team_bitmap_em;

typedef enum
{
		TEAM_WORK_STATUS_0_3=0,
		TEAM_WORK_STATUS_4_7,
		TEAM_WORK_STATUS_8_11,
    TEAM_WORK_STATUS_12_15,
    TEAM_WORK_STATUS_16_19,
    TEAM_WORK_STATUS_20_23,
    TEAM_WORK_STATUS_24_27,
    TEAM_WORK_STATUS_28_31,
    TEAM_WORK_STATUS_MAX
}team_work_status_em;

enum
{
		TEAM_FAN_MODE_AVR_RETURN = 0,
		TEAM_FAN_MODE_AVR_SUPPLY,
		TEAM_FAN_MODE_MAX_RETURN,
		TEAM_FAN_MODE_MAX_SUPPLY,
};

typedef struct
{
		uint16_t team_table[TEAM_MAX_SLAVE_NUM][TEAM_TAB_MAX];
		int16_t  team_param[TEAM_PARAM_MAX];
		uint16_t team_config[TEAM_CONF_MAX];
		uint8_t  team_fsm;
		uint32_t team_bitmap[TEAM_BITMAP_MAX];
    uint16_t team_work_status[TEAM_WORK_STATUS_MAX];
		team_signal_em team_signal;
    uint8_t team_sort_temp[TEAM_MAX_SLAVE_NUM];
    uint8_t team_sort_hum[TEAM_MAX_SLAVE_NUM];
    uint8_t team_config_set_flag;
    uint8_t team_config_set_flag_1;
    uint8_t slave_to_master_param_protect;  //从机变为主机下发参数延时
    uint8_t sync_param_protect;   //SYNC本地需求延时
    //主机下发参数缓存
    uint8_t master_req_distb_buf[8];
    uint8_t master_ex_temp_param_buf[8];
    uint8_t master_ex_hum_param_buf[8];
    uint8_t master_run_final_sts_buf[8];
    uint8_t master_send_param_protect;
    //if rotate_timeout < 60, can't rotate
    uint8_t rotate_timeout;
		uint8_t run_sts_cd;
		uint8_t	run_sts_flag;//运行状态参数是否有修改标志位
		uint8_t	sync_poweron_timeout;//处于sync时，到达设定的timeout时间后开启处于sync的设备
		time_t rotate_time_t;
}team_local_st;


//uint8_t get_team_ms_sts(void);
//uint8_t get_team_con_sts(void);
team_signal_em team_signal_gen(void);
void team_fsm_trans(void);
uint16_t team_get_pwr_sts(void);
void  team_clear_confd(void);
uint8_t check_target_temp_hum(void);
uint16_t team_get_th(uint16_t type, uint32_t th_temp);
void team_local_var_init(void);

#endif //	__TEAM
