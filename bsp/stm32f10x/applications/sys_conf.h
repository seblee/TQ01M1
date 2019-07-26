#ifndef __SYS_CONF
#define __SYS_CONF

#include "sys_def.h"
#include "alarms.h"
#include "string.h"
//#include "user_mb_app.h"

#define TRUE 1
#define FALSE 0

//数字输出映射
enum
{
    DO_COMP1_BPOS = 0, //压机1启动
    DO_COMP2_BPOS,     //压机2
    DO_RH1_BPOS,       //电加热1
    DO_FAN_BPOS,       //内风机
    DO_LAMP_BPOS,      //紫外灯1,定时杀菌
    DO_WV_BPOS,        //冷煤制水阀(压机制冷)
    DO_CV_BPOS,        //冷煤制冰水阀
    DO_RSV1_BPOS,      //保留
    DO_WP_BPOS,        //出水泵//泵2
    DO_HWP_BPOS,       //热水出水泵
    DO_PWP_BPOS,       //净化泵//泵1
    DO_DWP_BPOS,       //杀菌泵
    DO_DV_BPOS,        //出水阀//阀2
    DO_FV_BPOS,        //外接进水阀
    DO_RSV2_BPOS,      //保留
    DO_EL1_BPOS,       //电子锁1

    DO_EL2_BPOS,      //电子锁2
    DO_RSV3_BPOS,     //预留
    DO_LED_LOCK_BPOS, //童锁LED DC5V
    DO_PWR_CTRL_BPOS, //12V电源控制,低电平有效
    DO_RSV_BPOS_0,    //预留
    DO_RSV_BPOS_1,    //预留
    DO_RSV_BPOS_2,    //预留
    DO_RSV_BPOS_3,    //预留
    DO_RSV_BPOS_4,    //预留
    DO_RSV_BPOS_5,    //预留
    DO_RSV_BPOS_6,    //预留
    DO_RSV_BPOS_7,    //预留
    DO_RSV_BPOS_8,    //预留
    DO_RSV_BPOS_9,    //预留
    DO_RSV_BPOS_10,   //预留
    DO_RSV_BPOS_11,   //预留

    DO_FILLTER_DUMMY_BPOS,           //滤网
    DO_FILLTER_ELEMENT_DUMMY_BPOS_0, //滤芯  0
    DO_FILLTER_ELEMENT_DUMMY_BPOS_1, //滤芯  1
    DO_FILLTER_ELEMENT_DUMMY_BPOS_2, //滤芯  2
    DO_FILLTER_ELEMENT_DUMMY_BPOS_3, //滤芯  3
    DO_FILLTER_ELEMENT_DUMMY_BPOS_4, //滤芯  4
    DO_FILLTER_ELEMENT_DUMMY_BPOS_5, //滤芯  5
    DO_MAX_CNT,
};
#define DO_FAN_LOW_BPOS DO_FAN_BPOS //风机低档
#define DO_UV1_BPOS DO_RSV1_BPOS    //紫外灯2,过流
//L  双按键出水
#define DO_WP2_BPOS DO_RSV2_BPOS //出水泵2
#define DO_DV2_BPOS DO_RSV3_BPOS //出水阀2

#define DO_HEAT_FAN_BPOS DO_RSV2_BPOS //扇热风机
//TEST
#define DO_EV1_BPOS DO_DWP_BPOS  //循环阀//阀1
#define DO_EV2_BPOS DO_DV_BPOS   //阀2
#define DO_EV3_BPOS DO_RSV2_BPOS //阀3
#define DO_EV4_BPOS DO_RSV3_BPOS //阀4

#define DO_P1_BPOS DO_PWP_BPOS //泵1
#define DO_P2_BPOS DO_WP_BPOS  //泵2

#define DO_BD_BPOS DO_EL1_BPOS     //冰胆
#define DO_BD_FAN_BPOS DO_EL2_BPOS //冰胆风扇

#define DO_UV24_BPOS DO_RSV2_BPOS //24V紫外灯
#define DO_F24_BPOS DO_EL1_BPOS   //24V风机,T8机组

//application delay
#define MODBUS_MASTER_THREAD_DELAY 500
#define MODBUS_MASTER_THREAD_DELAY_01 800
#define MODBUS_SLAVE_THREAD_DELAY 2000
#define NET_THREAD_DELAY 2000
//#define		TCOM_THREAD_DELAY	          1150
//#define		TEAM_THREAD_DELAY           1200
#define MBM_FSM_THREAD_DELAY 1150
#define MBM_FSM_THREAD_DELAY_01 1250
#define DI_THREAD_DELAY 1300
#define DAQ_THREAD_DELAY 1350
#define CORE_THREAD_DELAY 3000
#define SURV_THREAD_DELAY 1400
#define CPAD_THREAD_DELAY 1600
#define BKG_THREAD_DELAY 2200
#define TESTCASE_THREAD_DELAY 1650

///////////////////////////////////////////////////////////////
//AI configuration
///////////////////////////////////////////////////////////////
enum
{
    AI_SENSOR1 = 0,
    AI_NTC1,
    AI_NTC2,
    AI_NTC3,
    AI_NTC4,
    AI_MAX_CNT
};
#define AI_SENSOR_ERR AI_MAX_CNT + 1 //传感器故障
#define AI_SENSOR_NUM 1              //传感器数量
#define AI_NTC_NUM 4                 //NTC数量

enum
{
    AO_EC_FAN = 0, //EC风机
    AO_PREV_1,
    AO_WATER_VALVE,
    AO_EC_COMPRESSOR, //变频压机
    AO_PREV_2,
    AO_MAX_CNT,
    AO_INV_FAN, //虚拟输出 变频
};

#define AO_REAL_CNT 1
#define ABNORMAL_VALUE 0x7FFF //异常值
#define OVER_VALUE 1000       //温湿度超限值

//测试模式
enum
{
    TEST_UNABLE = 0,               //退出测试模式
    TEST_PRPDUCE_WATER = 0x01,     //制水
    TEST_PURIFICATION = 0x02,      //净化
    TEST_NORMAL_WATER = 0x03,      //出常温水
    TEST_HEAT_WATER = 0x04,        //出热水
    TEST_PRPDUCE_COLDWATER = 0x05, //制冰水
    TEST_TANK = 0x06,              //抽空源水箱与饮水箱
    TEST_OUTWATER = 0x07,          //出水阀测试
    TEST_UV = 0x08,                //UV测试
    TEST_Relay = 0x3C,             //继电器测试
    TEST_ALL_OUT = 0x5A,           //全开
};

#define TEST_TIME 1             //测试时间1s
#define TEST_CICLE 15           //开关周期
#define TEST_UV2_OPEN 30 / 2    //开关周期
#define TEST_UV2_CLOSE 90 / 2   //开关周期
#define TEST_UV1_OPEN 600 / 2   //开关周期
#define TEST_UV1_CLOSE 6600 / 2 //开关周期

#define TEST_UR2_OPEN 5   //开关周期
#define TEST_UR2_CLOSE 5  //开关周期
#define TEST_UR1_OPEN 10  //开关周期
#define TEST_UR1_CLOSE 10 //开关周期

//手动测试模式
enum
{
    MANUAL_TEST_UNABLE = 0,    //退出测试模式
    MANUAL_MODE_ENABLE = 0x01, //手动模式
    TEST_MODE_ENABLE = 0x02,   //测试模式
};

///////////////////////////////////////////////////////////////
//system configuration
///////////////////////////////////////////////////////////////

typedef struct
{
    uint8_t Err_Master0[10];    //异常计数
    uint8_t Err_Master1[2];     //异常计数
    uint8_t Err_Master2[4];     //异常计数
    uint16_t Err_Master_Cnt[3]; //通信异常
    uint8_t Err_Enable;         //通信告警使能
} mbm_Error_St;

typedef struct
{
    uint16_t id;
    uint16_t *reg_ptr;
    int16_t min;
    uint16_t max;
    uint16_t dft;
    uint8_t permission;
    uint8_t rw;
    uint8_t (*chk_ptr)(uint16_t pram);
} conf_reg_map_st;

typedef struct
{
    uint16_t id;
    uint16_t *reg_ptr;
    uint16_t dft;
    //uint8_t		rw;
} sts_reg_map_st;

//system component mask, if set 1 measn exist, otherwise absent
typedef struct
{
    uint16_t power_switch;
    uint16_t set_voltage;
    uint16_t set_current;
    //uint8_t		rw;
} pwr_dev_set_st;

typedef struct
{
    uint16_t ain;
    uint16_t din[2];
    uint16_t din_pusl;
    uint16_t aout;
    uint16_t dout[2];
    uint16_t mb_comp;
    uint16_t mb_discrete_mask;
    uint16_t return_temp_mask;
    uint16_t supply_temp_mask;
    uint16_t cab_temp_mask;
    uint16_t din_bitmap_polarity[2];
    //    uint16_t  pwm_out;
    //		pwr_dev_set_st mbm_pwr_cfg[2];

} dev_mask_st;

#define ALARM_TOTAL_WORD 2
typedef struct
{
    uint16_t temp;
    uint16_t hum;
} temp_sensor_cali_st;
typedef struct
{
    uint16_t power_mode;                            //power-off or power-on
    uint16_t standalone_timer;                      //automatic, manual
    uint16_t cool_type;                             //cooling type
    uint16_t cpad_baudrate;                         //control pad communication baudrate
    uint16_t surv_baudrate;                         //surveillance communication baudrate
    uint16_t surv_addr;                             //surveillance communication address
    uint16_t diagnose_mode_en;                      //diagnose mode enalbe
    uint16_t alarm_bypass_en;                       //diagnose mode enalbe
    uint16_t testing_mode_en;                       //test mode enalbe
    uint16_t power_mode_mb_en;                      // modbuss power mode control enable
    uint16_t cancel_alarm_mb_en;                    // cancel all alarm enable
    uint16_t alarm_remove_bitmap[ALARM_TOTAL_WORD]; //reless alarm
    uint16_t ntc_cali[AI_NTC_NUM];                  // NTC cali
    uint16_t ai_cali[AI_SENSOR_NUM];                //ai_ cali
    uint16_t LED_Num;                               //LED数量
    uint16_t Alarm_Beep;                            //严重告警，响喇叭
    temp_sensor_cali_st temp_sensor_cali[TEMP_HUM_SENSOR_NUM];
} conf_general_st;

//status_set
/*
@permission_level: control pad accesssible user permission level
	0:	lowest						
	1:	above lowest
	2:	below highest
	3:	highest
@running_mode: control pad accesssible user permission level
	0:	standalone_power-off						
	1:	standalone_on
	2:	team_poweroff
	3:	team_power_on

@running_mode: control pad accesssible user permission level
	bit0:	fatal error						
	bit1:	internal modbus bus communication error
	bit2:	survallance modbus bus communication error
	bit3:	can bus communication error
*/
enum
{
    SYS_ERR_INIT = 0,
    SYS_ERR_TEAM,
    SYS_ERR_MBM,
    SYS_ERR_MBS,
    SYS_ERR_CAN,
    SYS_ERR_CPAD,
};

enum
{
    WORK_MODE_STS_REG_NO = 0, //机组工作状态
    GEN_STS_REG_NO,
    MBM_COM_STS_REG_NO,
    SENSOR_STS_REG_NO
};

enum
{
    ALARM_COM = 0,
    ALARM_NTC,
};

enum
{
    PWR_STS_BPOS = 0,   //开机
    COOLING_STS_BPOS,   //制水
    OUTWATER_STS_BPOS,  //出水
    STERILIZE_STS_BPOS, //杀菌
    DEFROST1_STS_BPOS,  //除霜1
    DEFROST2_STS_BPOS,  //除霜2
    FAN_STS_BPOS,       //风机
    HEATING_STS_BPOS,   //加热
    EXITWATER_STS_BPOS, //外接水源
    NET_STS_BPOS,
    ALARM_STUSE_BPOS = 14,
    ALARM_BEEP_BPOS = 15,
};

typedef struct
{
    uint16_t permission_level; //user authentication level
    uint16_t running_mode;     //automatic, manual or testing
    uint16_t sys_error_bitmap; //system error status
    uint16_t Alarm_AC_CNT[3];  //AC告警数量

} status_general_st;

enum
{
    P_ALOGORITHM = 0,
    PID_ALOGORITHM,
    FUZZY_ALOGORITHM
};

enum
{
    HUM_RELATIVE = 0,
    HUM_ABSOLUTE
};
enum
{
    WATER_LEVEL_NO = 0,        //水少
    WATER_LEVEL_LOW,           //低水位
    WATER_LEVEL_REPLENISHMENT, //补水水位
    WATER_LEVEL_HIGH,          //高水位
    WATER_LEVEL_OVERFLOW,      //溢水
    WATER_LEVEL_OTHER,         //异常
};
// meter tem_hum
typedef struct
{
    uint16_t supply_air_temp;
    uint16_t return_air_temp;
    uint16_t remote_air_temp;
    uint16_t supply_air_max_temp;
    uint16_t return_air_max_temp;
    uint16_t remote_air_max_temp;
    uint16_t supply_air_hum;
    uint16_t return_air_hum;
    uint16_t remote_air_hum;
    uint16_t supply_air_min_temp;
} sys_tem_hum_st;
//algorithm
typedef struct
{
    uint16_t temp_calc_mode;
    uint16_t temp_ctrl_mode;
    uint16_t hum_ctrl_mode;
    uint16_t ctrl_target_mode;
    uint16_t supply_air_temp;
    uint16_t return_air_temp;
    uint16_t remote_air_temp;
    uint16_t supply_air_hum;
    uint16_t return_air_hum;
    uint16_t remote_air_hum;
    uint16_t temp_precision;
    uint16_t hum_precision;
    uint16_t temp_deadband;
    uint16_t hum_deadband;
    uint16_t sample_interval;
    uint16_t temp_integ;
    uint16_t temp_diff;
    uint16_t pid_action_max;
    uint16_t temp_req_out_max;
} algorithm_st;

//compressor
typedef struct
{
    uint16_t startup_delay;
    uint16_t stop_delay;
    uint16_t min_runtime;
    uint16_t min_stoptime;
    uint16_t startup_lowpress_shield;
    uint16_t alter_mode;
    uint16_t alter_time;
    uint16_t start_interval;
    uint16_t ev_ahead_start_time;
    uint16_t ev_ahead_shut_time;
    uint16_t speed_upper_lim;
    uint16_t speed_lower_lim;
    uint16_t ec_comp_start_req;
    uint16_t startup_freq;
    uint16_t high_press_threshold;
    uint16_t high_press_hyst;
    uint16_t ret_oil_period;
    uint16_t ret_oil_freq;
    uint16_t low_freq_switch_period;
    uint16_t low_freq_threshold;
    uint16_t step;
    uint16_t step_period;
} compressor_st;

//fan
typedef struct
{
    uint16_t type;
    uint16_t mode;
    uint16_t num;
    uint16_t adjust_step;
    uint16_t startup_delay;
    uint16_t cold_start_delay;
    uint16_t stop_delay;
    uint16_t set_speed;
    uint16_t min_speed;
    uint16_t max_speed;
    uint16_t dehum_ratio;
    uint16_t hum_min_speed;
    uint16_t set_flow_diff;
    uint16_t flow_diff_deadzone;
    uint16_t flow_diff_step;
    uint16_t flow_diff_delay;
    uint16_t target_suc_temp;
    uint16_t suc_temp_deadzone;
    uint16_t suc_temp_step;
    uint16_t suc_temp_delay;
    uint16_t noload_down; //无负载风机降速使能
    uint16_t target_temp;
    uint16_t temp_dead_band;
    uint16_t temp_precision;
    uint16_t temp_add_fan_en;
    uint16_t tem_add_fan_delay;
    uint16_t fan_k;
    uint16_t CFM_Enable; //风量显示使能
    uint16_t CFM_Para_A; //风量参数A
    uint16_t CFM_Para_B;
    uint16_t CFM_Para_C;
} fan_st;

//team set
typedef struct
{
    uint16_t team_en;       //team enable
    uint16_t mode;          //team mode 0,1,2,3
    uint16_t addr;          //team id
    uint16_t baudrate;      //team communication baudrate
    uint16_t total_num;     //units number in the team
    uint16_t backup_num;    //backup units
    uint16_t rotate_period; //upper byte:0:no rotate;1:daily;2:weekly;lower byte:week day(0:sunday,1:monday...)
    uint16_t rotate_time;   //upper byte:hour;lower byte:minite;
    uint16_t rotate_num;
    uint16_t rotate_manual;
    uint16_t cascade_enable;
    uint16_t team_fan_mode;
    uint16_t fault_power_en;
} team_st;

//analog_in
typedef struct
{
    uint16_t ai_data[AI_MAX_CNT]; //
    uint64_t ai_mask;
} ain_st;

#define DI_COMP_1_HI_TEMP_POS ((uint32_t)0x00000001 << 0)
#define DI_COMP_2_HI_TEMP_POS ((uint32_t)0x00000001 << 1)
#define DI_COMP_1_LOW_TEMP_POS ((uint32_t)0x00000001 << 2)
#define DI_COMP_2_LOW_TEMP_POS ((uint32_t)0x00000001 << 3)
#define DI_COMP_1_DISC_TEMP_POS ((uint32_t)0x00000001 << 4)
#define DI_COMP_2_DISC_TEMP_POS ((uint32_t)0x00000001 << 5)
#define DI_FAN_1_OVF_POS ((uint32_t)0x00000001 << 6)
#define DI_FAN_2_OVF_POS ((uint32_t)0x00000001 << 7)
#define DI_FAN_3_OVF_POS ((uint32_t)0x00000001 << 8)
#define DI_AIR_LOSS_POS ((uint32_t)0x00000001 << 9)
#define DI_FILTER_CLOG_POS ((uint32_t)0x00000001 << 10)
#define DI_WATER_OVER_FLOW_POS ((uint32_t)0x00000001 << 11)
#define DI_RMT_SHUT_POS ((uint32_t)0x00000001 << 12)
#define DI_HUM_WATER_LV ((uint32_t)0x00000001 << 13)
#define DI_RESERVE_2_POS ((uint32_t)0x00000001 << 14)
#define DI_RESERVE_3_POS ((uint32_t)0x00000001 << 15)

#define ST_PWR_PA_AB_POS ((uint32_t)0x00000001 << 16)
#define ST_PWR_PB_AB_POS ((uint32_t)0x00000001 << 17)
#define ST_PWR_PC_AB_POS ((uint32_t)0x00000001 << 18)
#define ST_HUM_WL_H_POS ((uint32_t)0x00000001 << 19)
#define ST_HUM_HC_H_POS ((uint32_t)0x00000001 << 20)
#define ST_HUM_WQ_L_POS ((uint32_t)0x00000001 << 21)

//Digtal input status
/*
    bit map:
    bit0:  compressor 1 hi temp valve
    bit1:  compressor 2 hi temp valve
    bit2:  compressor 1 low temp valve
    bit3:  compressor 2 low temp valve
    bit4:  compressor 1 discharge temp valve
    bit5:  compressor 2 discharge temp valve
    bit6:  fan 1 overload valve
    bit7:  fan 2 overload valve
    bit8:  fan 3 overload valve
    bit9:  air lost valve
    bit10: filter clog valve
    bit11: water overflow valve
    bit12: remote shut valve
    bit13: reserve1
    bit14: reserve2
    bit15: reserve3

    bit16: power phase A error 
    bit17: power phase B error 
    bit18: power phase C error 
    bit19: humidifier water level high
    bit20: humidifier heating current high
    bit21: humidifier conductivity low
*/

typedef struct
{
    uint32_t din_data;
    uint32_t din_mask;
} din_st;

///////////////////////////////////////////////////////////////
//system output status
///////////////////////////////////////////////////////////////

//analog_out
//this feature is not yet determined, reserve interface for future application
typedef struct
{
    int16_t ec_fan[3];
    int16_t vf_compressor[2];
    int16_t reserve_aout[2];
} aout_st;

///////////////////////////////////////////////////////////////
//system log
///////////////////////////////////////////////////////////////
//alarm status
typedef struct
{
    int16_t alarm_id;
    time_t trigger_time;
} alarm_status_st;

//alarm history
typedef struct
{
    int16_t alarm_id;
    time_t trigger_time;
    time_t clear_time;
} alarm_history_st;

//alarm system runtime log, record components accumulative running time
/*
@comp_id:
    0: compressor 1
    1: compressor 2
    2: fan 1
    3: fan 2
    4: fan 3
    5: heater 1
    6: heater 2
    7: humidifier
@action:
    0: deactivated
    1: activated
@trigger_time:
    sys_time
*/

typedef struct
{
    int16_t comp1_runtime_day;
    int16_t comp1_runtime_min;
    int16_t comp2_runtime_day;
    int16_t comp2_runtime_min;
    int16_t fan1_runtime_day;
    int16_t fan1_runtime_min;
    int16_t fan2_runtime_day;
    int16_t fan2_runtime_min;
    int16_t fan3_runtime_day;
    int16_t fan3_runtime_min;
    int16_t heater1_runtime_day;
    int16_t heater1_runtime_min;
    int16_t heater2_runtime_day;
    int16_t heater2_runtime_min;
    int16_t humidifier_runtime_day;
    int16_t humidifier_runtime_min;
} sys_runtime_log_st;

//alarm system runtime log, record components change of output states
/*
@comp_id:
    0: compressor 1
    1: compressor 2
    2: fan 1
    3: fan 2
    4: fan 3
    5: heater 1
    6: heater 2
    7: humidifier
@action:
    0: deactivated
    1: activated
@trigger_time:
 sys_time
*/
typedef struct
{
    uint16_t comp_id;
    uint16_t action;
    time_t trigger_time;
    time_t clear_time;
} sys_status_log_st;

///////////////////////////////////////////////////////////////
//alarms definition
///////////////////////////////////////////////////////////////

//alarms: acl definition
/*
    @id:   alarm id
    @delay:  trigger&clear delay 
    @timeout: delay timeout count down
    @trigger_time: alarm trigger time
    @enable mode: alarm enable mode
        `0x00:  enable
        `0x01:  suspend
        `0x02:  forbid
    @enable mask: alarm enable mask
        '0x03: all mode enable
        '0x02: enable or forbid 
        '0x01: enable or suspend
        '0x00: only enable
    @alarm_param: related paramter(eg. threshold)
    @void (*alarm_proc): designated alarm routine check function
*/

typedef struct
{
    uint16_t id;
    uint16_t delay;
    uint16_t enable_mode;
    uint16_t alarm_param;
} alarm_acl_conf_st;

//system memory configuration map
typedef struct sys_conf_map
{
    int16_t id;
    void *str_ptr;
    int16_t length;
} sys_conf_map_st;

typedef struct sys_status_map
{
    int16_t id;
    int16_t *addr;
    int16_t length;
} sys_status_map_st;

typedef struct
{
    uint16_t u16Net_Sel;           //wifi-4G选择
    uint16_t u16Net_WifiSet;       //wifi已经设置
    uint16_t u16Wifi_Name[10];     //wifi名称/ASIIC
    uint16_t u16Wifi_Password[10]; //wifi密码/ASIIC
} Net_Conf_st;
#define WIFI_SET 0x5AA5

//空气制水机参数
typedef struct
{
    Net_Conf_st Net_Conf;              //网络配置
    uint16_t u16SN_Code[4];            //SN码
    uint16_t u16M_Type;                //设备类型
    uint16_t u16Power_Mode;            //开关机
    uint16_t u16Start_Temp[2];         //制水启动温度
    uint16_t u16Start_Humidity;        //制水启动湿度
    uint16_t u16Stop_Temp[2];          //制水停止温度
    uint16_t u16Stop_Humidity;         //制水停止湿度
    uint16_t u16Start_Defrost_Temp;    //除霜启动温度
    uint16_t u16Stop_Defrost_Temp;     //除霜停止温度
    uint16_t u16Sterilize_Mode;        //杀菌模式:BIT0-220V,BIT1-24V
    uint16_t u16Sterilize_Time[2];     //杀菌时间
    uint16_t u16Sterilize_Interval[2]; //杀菌间隔
    uint16_t u16UV_Delay;              //UV关闭延时,MIN
    uint16_t u16Water_Ctrl;            //水路控制方案
    uint16_t u16Water_Mode;            //出水模式
    uint16_t u16Water_Flow;            //出水流量
    uint16_t u16NormalWater_Temp;      //常温水温度
    uint16_t u16HotWater_Temp;         //热水温度
    uint16_t u16ExitWater_Mode;        //外接水源模式
    uint16_t u16Disinfection_Mode;     //消毒模式
    uint16_t u16Rsv0[4];               //
    uint16_t u16Reset;                 //恢复出厂
    uint16_t u16Test_Mode_Type;        //测试模式选择
    uint16_t u16Manual_Test_En;        //手动测试模式使能
    uint16_t u16BITMAP_MANUAL;         //手动输出
    uint16_t u16TPower_En;             //定时开关机
    uint16_t u16TPower_On;             //定时开机时间
    uint16_t u16TPower_Off;            //定时关机时间
    uint16_t u16Rsv1[3];               //
    uint16_t u16FILTER_ELEMENT_Type;   //滤芯告警类型:0-流量L;1-时间h
    uint16_t u16Clear_RT;              //清除部件时间
    uint16_t u16Clear_ALARM;           //清除告警
    uint16_t u16Set_Time[2];           //设置系统时间
    uint16_t u16Start_Delay;           //风机开启延时
    uint16_t u16Fan_Stop_Delay;        //风机关闭延时
    uint16_t u16Comp_Interval;         //压机间隔
    uint16_t u16ColdWater_Mode;        //冰水模式
    uint16_t u16ColdWater_StartTemp;   //制冰水温度
    uint16_t u16ColdWater_StopTemp;    //制冰水停止温度
    uint16_t u16HeatFan_StartTemp;     //热风机启动温度
    uint16_t u16HeatFan_StopTemp;      //热风机停止温度
    uint16_t u16WaterFlow;             //出水流量后开始制水
    uint16_t u16Storage;               //贮存
    uint16_t u16StorageDealy[2];       //延时
    uint16_t u16CloseFrist;            //null
    uint16_t u16CloseDelay;            //null
    uint16_t u16TestEV[2];             //null
    uint16_t device_info[100];         //三元组信息
} ComPara_Conf_st;

typedef struct
{
    uint16_t eevproType; //电子膨胀阀类型
    uint16_t excSpeed;   //励磁速度 pps

    uint16_t excAllOpenSteps;       //全开脉冲
    uint16_t excOpenValveMinSteps;  //阀开脉冲--最小步数      //@2017-08-21
    uint16_t excOpenValveMinDegree; //关阀时阀开度的最小极限值
    uint16_t eevHoldTime;           //unit 100ms
    uint16_t restore_factory_setting;
} Moter_st;

//平台通信参数
typedef struct
{
    uint16_t Fixed_Report; //定时上报间隔
    uint16_t Real_Report;  //实时上报间隔
} Platform_st;

typedef struct
{
    conf_general_st general;
    dev_mask_st dev_mask;
    //	power_supply_st       ac_power_supply;
    algorithm_st algorithm;
    alarm_acl_conf_st alarm[ACL_TOTAL_NUM];
    //	mbm_Conf_st           mbm_Conf;
    ComPara_Conf_st ComPara;
    fan_st fan;
    compressor_st compressor;
    Moter_st Moter;
    Platform_st Platform;
} config_st;

typedef struct
{
    uint16_t dev_sts;
    uint16_t conductivity;
    uint16_t hum_current;
    uint16_t water_level;
} mbm_hum_st;

//pwr
typedef struct
{
    uint16_t status;
    uint16_t output_voltage;
    uint16_t output_current;
    uint16_t pwr_on;
    uint16_t set_voltage;
    uint16_t set_current;
} pwr_imf_st;

typedef struct
{
    uint16_t dev_sts;
    uint16_t temp;
    uint16_t hum;
} mbm_tnh_st;

typedef struct
{
    uint16_t Temp;
    uint16_t Hum;
} Com_tnh_st;

//system information
typedef struct
{
    uint16_t status_reg_num;
    uint16_t config_reg_num;
    uint16_t software_ver;
    uint16_t hardware_ver;
    uint16_t serial_no[4];
    uint16_t man_date[2];
} sys_info_st;

//modbus master data structure
typedef struct
{
    mbm_tnh_st tnh[TEMP_HUM_SENSOR_NUM]; //温湿度
    // mbm_IPM_St IPM;                      //IPM电表
    // mbm_PDU_St PDU[4];                   //PDU
    // mbm_AC_St AC;                        //空调
    // // mbm_UPS_APC_St   UPS;
    // //#if (UPS_TYPE==UPS_EMERSON_ITA2)
    // // mbm_UPS_ITA2_St   UPS;
    // //#elif (UPS_TYPE==UPS_JYD_SIN)
    // // mbm_UPS_JYD_SIN_St UPS;
    // //#endif
    // mbm_UPS_HW_St UPS;
    mbm_Error_St Err_M;
} mbm_sts_st;

typedef struct
{
    uint16_t low;
    uint16_t high;
} run_time_st;

typedef struct
{
    uint16_t pwr_off_alarm;
    uint16_t critical_cnt;
    uint16_t major_cnt;
    uint16_t mioor_cnt;
    uint16_t total_cnt;
} alarm_state_cnt_st;

typedef struct
{
    uint16_t work_mode;
    uint16_t limit_day;
    uint16_t runing_day;
    uint16_t runing_hour;
    uint16_t runing_sec;
    uint16_t runing_State;
    uint8_t pass_word[4];
} work_mode_st;

typedef struct
{
    uint16_t Grade_Manage;         //管控阶段
    uint16_t Password_Poweron;     //开机管控密码
    uint16_t Password_Grade[4][2]; //1-4级管控天数及密码
    uint16_t Remain_day;           //当前阶段剩余天数
    uint16_t Run_day;              //当前阶段运行天数
    uint16_t Run_hour;             //当前阶段运行小时
    uint16_t Run_second;           //当前阶段运行秒
    uint16_t Run_State;            //运行状态
} ControlPassword_st;

typedef enum
{
    RETURN_AIR_PLUSS_MODE = 0,
    SET_FAN_SPEED_MODE,
} return_air_mode_st;
typedef struct
{
    uint16_t timer;
    return_air_mode_st return_air_work_mode;
} return_air_sta_st;

typedef struct
{
    uint16_t Sec;
    uint16_t Min;
    uint16_t Hour;
    uint16_t Day;
    uint16_t Mon;
    uint16_t Year;
    uint16_t Weekday;
    time_t u32Systime;
} System_Time_st;

typedef struct
{
    uint16_t u16Hardware_ver;
    uint16_t u16Software_ver;
    uint16_t u16Status_remap[4];
    uint16_t u16Din_bitmap[2];
    uint16_t u16Dout_bitmap[2];
    uint16_t u16Alarm_bitmap[ALARM_TOTAL_WORD];
    uint16_t u16Ain[AI_MAX_CNT]; //
    uint16_t u16AO[AO_MAX_CNT];
    Com_tnh_st u16TH[2]; //温湿度
    uint16_t u16Pluse_CNT;
    uint16_t u16Cur_Water;
    uint16_t u16Last_Water;
    uint16_t u16Cumulative_Water[2]; //累计取水
    uint16_t u16PM25;
    uint16_t u16Rev1[3];
    uint16_t u16Runtime[2][DO_MAX_CNT]; //使用时间
    uint16_t u16WL;                     //水位
    System_Time_st Sys_Time;            //系统时间
    uint16_t TEST;
    uint16_t TEST2;
    uint16_t TEST3;
    uint16_t TEST4;
    uint16_t TEST5;
    uint16_t TEST6;
    uint16_t REQ_TEST[3];
    uint16_t net_status;
} ComSta_st;

typedef struct
{
    sys_info_st sys_info;
    status_general_st general; //3
    mbm_sts_st mbm;            //25
    uint16_t ain[AI_MAX_CNT];  //
    uint16_t aout[AO_MAX_CNT]; //6
    // uint16_t CFM;                  //总风量
    // uint16_t pwmout[PWM_MAX_CNT];  //2
    uint16_t din_bitmap[2];        //2
    uint16_t dout_bitmap[2];       //2
    uint16_t status_remap[4];      //4
    uint16_t alarm_bitmap[7];      //6
    uint16_t Alarm_COM_NTC_BIT[2]; //2
    // uint16_t flash_program_flag;   //1
    // run_time_st run_time[DO_MAX_CNT];
    alarm_state_cnt_st alarm_status_cnt;
    sys_tem_hum_st sys_tem_hum;
    // work_mode_st sys_work_mode;
    // uint16_t flow_diff_timer;
    // return_air_sta_st return_air_status;
    // uint16_t Hum_Water_Level;           //1
    // ControlPassword_st ControlPassword; //5级密码管控
    ComSta_st ComSta;
} status_st;

typedef struct
{
    config_st config;
    status_st status;
} sys_reg_st;

typedef enum
{
    TEM_HUM_SENSOR1_BPOS = 0,
    TEM_HUM_SENSOR2_BPOS,
    TEM_HUM_SENSOR3_BPOS,
    TEM_HUM_SENSOR4_BPOS,
    TEM_HUM_SENSOR5_BPOS,
    TEM_HUM_SENSOR6_BPOS,
    TEM_HUM_SENSOR7_BPOS,
    TEM_HUM_SENSOR8_BPOS,
    //
    POWER_MODULE_BPOS,
    AC_MODULE_BPOS,
    UPS_MODULE_BPOS,
    RESERVE_GSM_BPOS,
    PDU_MODULE_BPOS,
    PDU_01_MODULE_BPOS,
    PDU_02_MODULE_BPOS,
    PDU_03_MODULE_BPOS,
} DEV_MASK_MB_BPOS;

#endif //	__SYS_CONF
