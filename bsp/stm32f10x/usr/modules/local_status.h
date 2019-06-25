#ifndef __LOCAL_REG_H__
#define __LOCAL_REG_H__

#include "stdint.h"
#include "sys_conf.h"

enum
{
    RESET_REQ = 0,
    LOCAL_REQ,
    TEAM_REQ,
    TARGET_REQ,
    MAX_REQ,
    REQ_MAX_CNT,
};

enum
{
    T_REQ = 0,
    H_REQ,
    F_REQ,
    REQ_MAX_LEVEL,
};

//global FSM states definition
enum
{
    T_FSM_TOP_ID = 0,
    T_FSM_STANALONE_ID,
    T_FSM_TEAM_ID,
    H_FSM_MAX_ID_CNT
};

//global top-level FSM states definition
enum
{
    T_FSM_STATE_IDLE = 0,
    T_FSM_STATE_STANDALONE,
    T_FSM_STATE_TEAM,
    T_FSM_MAX_CNT,
};

enum
{
    BITMAP_REQ = 0,
    BITMAP_ALARM,
    BITMAP_MANUAL,
    BITMAP_FINAL,
    BITMAP_MASK,
    BITMAP_MAX_CNT,
};

enum
{
    FAN_FSM_STATE = 0,
    COMPRESS_SIG_FSM_STATE,
    COMPRESS1_FSM_STATE,
    COMPRESS2_FSM_STATE,
    HEATER_FSM_STATE,
    DEHUMER_FSM_STATE,
    HUMIDIFIER_FSM_STATE,
    WATERVALVE_FSM_STATE,
    DP_FSM_STATE, //双电源
    L_FSM_STATE_MAX_NUM,
};

enum
{
    COMP_CONSTANT_FRE = 0, //定频
    COMP_QABP,             //变频
};

enum
{
    T_FSM_SIG_IDLE = 0,
    T_FSM_SIG_STANDALONE,
    T_FSM_SIG_TEAM,
    T_FSM_SIG_SHUT
};

/*
@signal:	system operating mode 
	1:	power on signal
	2:	power down signal
	3:	sys on signal
	4:	sys down signal
*/

typedef struct
{
    int16_t p_saved;
    int16_t i_saved;
    int16_t req_saved;
} pid_reg_st;

typedef struct
{
    int16_t set_point;  //Desired Value
    double proportion;  //Proportional Const
    double integral;    //Integral Const
    double derivative;  //Derivative Const
    int16_t last_error; //Error[-1]
    int16_t prev_error; //Error[-2]
} pid_param_st;

typedef struct
{
    pid_reg_st temp;
    pid_reg_st hum;
} pid_st;

typedef struct
{
    uint16_t Fan_Dehumer_Delay; //风机档位延时,Alair，20161113
    uint8_t Fan_Dehumer_State;  //风机档位,Alair，20161113
    uint8_t Fan_Gear;           //风机档位,Alair，20161113
    uint8_t Fan_default_cnt;    //异常风机个数
} Fan_st;

typedef struct
{
    int16_t require[REQ_MAX_CNT][REQ_MAX_LEVEL];
    uint16_t bitmap[2][BITMAP_MAX_CNT];
    int16_t ao_list[AO_MAX_CNT][BITMAP_MAX_CNT];
    uint16_t comp_timeout[DO_MAX_CNT];
    uint16_t t_fsm_state;
    uint16_t t_fsm_signal;
    int16_t ec_fan_diff_reg;
    int16_t ec_fan_suc_temp;
    uint16_t authen_cd;
    uint16_t debug_flag;
    uint16_t debug_tiemout;
    uint16_t l_fsm_state[L_FSM_STATE_MAX_NUM];
    pid_st pid;
    Fan_st Fan;
    uint8_t Fan_Close;       //风机开关信号
    uint8_t Comp_Close[2];   //压缩机开关信号
    uint8_t Water_Full;      //满水信号
    uint8_t Pwp_Open;        //净化泵开关信号
    uint16_t Pwp_Open_Time;  //净化泵打开时间
    uint8_t Sterilize;       //杀菌
    uint8_t OutWater_Flag;   //出水中
    uint8_t OutWater_OK;     //出水完成
    uint8_t HeatWater_st;    //加热器出水状态
    uint16_t HeatWater_Flow; //加热器出水流量
    uint16_t HeatWater_Time; //加热器出水时间
    uint8_t Cold_Water;      //制冰水信号
    uint16_t u16BD_Time;
    uint16_t u16BD_FAN_Delay;
    uint8_t SEL_Jump; //跳线选择
    uint16_t comp_startup_interval;
    uint16_t comp_stop_interval;
    uint16_t PowerOn_Time;      //开机计时
    uint16_t TH_Check_Interval; //温湿度检测间隔
    uint16_t TH_Check_Delay;    //温湿度检测间隔
    uint8_t Set_Systime_Flag;   //设置系统时间标识
    uint8_t Set_Systime_Delay;  //设置系统等待延时
    uint8_t OutWater_Key;       //按键出水
    uint16_t OutWater_Delay[3]; //按键出水延时
    uint8_t ChildLock_Key;      //童锁
    uint8_t ChildLock_Cnt[2];   //童锁计数器
    uint16_t u16Uart_Timeout;   //串口重启
    uint8_t u8CloseDelay;       //关闭延迟
    uint16_t u16WaterFlow;      //出水流量
} local_reg_st;

enum
{
    FAN_DEHUMER_IDLE = 0,
    FAN_DEHUMER_START,
    FAN_DEHUMER_NORMAL,
};
//风机档位,Alair，20161113
enum
{
    HUM_FLUSH = 0,
    HUM_DRAIN,
    HUM_FILL,
};
//换水状态,Alair，20161113
enum
{
    FAN_GEAR_NO = 0,
    FAN_GEAR_LOW,
    FAN_GEAR_MID,
    FAN_GEAR_HIGH,
};
#define FAN_GEAR_START FAN_GEAR_LOW
//Alair,20170304,水阀使能状态
enum
{
    WATER_OFF = 0,
    WATER_ON,
};

#endif //__LOCAL_REG_H__
