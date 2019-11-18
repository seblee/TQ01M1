#ifndef __REQ_EXE_H__
#define __REQ_EXE_H__
#include "stdint.h"

//#define WV_TEST 1	//制冷阀常闭，反向
//#define BD_COLD 	1	//冰胆制冷
#define PUMP_TEST 1	//制冷阀常闭，反向

#define HUM_CURRENT_UNIT 1.19
enum
{
    HUM_FSM_STATE_IDLE = 0,
    HUM_FSM_STATE_CHECK,
    HUM_FSM_STATE_WARM,
    HUM_FSM_STATE_DRAIN, //排水
    HUM_FSM_STATE_HUM,   //加湿
    HUM_FSM_STATE_FILL,  //注水
    HUM_FSM_STATE_FLUSH, //冲刷换水

};

enum
{
    HUM_TYPE_FIX,
    HUM_TYPE_P,
    HUM_TYPE_INFRARED, //红外加湿
};

enum
{
    COMPRESSOR_FSM_STATE_IDLE = 0,
    COMPRESSOR_FSM_STATE_INIT,
    COMPRESSOR_FSM_STATE_STARTUP,
    COMPRESSOR_FSM_STATE_NORMAL,
    COMPRESSOR_FSM_STATE_SHUTING,
    COMPRESSOR_FSM_STATE_STOP,
};

//风机状态机
enum
{
    FSM_FAN_IDLE = 0,
    FSM_FAN_INIT,
    FSM_FAN_START_UP,
    FSM_FAN_NORM,
    FSM_FAN_SHUT
};

/************************************************************************/
//贮存状态机
enum
{
    WATER_STROGE_IDLE = 0,
    WATER_STROGE_1,
    WATER_STROGE_2,
    WATER_STROGE_3,
    WATER_STROGE_4,
    WATER_STROGE_5,
    WATER_STROGE_6,
    WATER_STROGE_7,
    WATER_STROGE_8,
    WATER_STROGE_STOP,
};

//出水模式
enum
{
    WATER_NO = 0x00,
    WATER_NORMAL_ICE = 0x01,
    WATER_HEAT = 0x02,
    WATER_ICE = 0x04, //冰水
    WATER_HEATPOT = 0x08,//热灌
};
//冰水模式
enum
{
    ICE_NO = 0x00,
    NORMAL_ICE = 0x01, //阀冰水
    BD_ICE = 0x02,     //冰胆
};

#define COLD_FV_DELAY 	2   //1S
#define COLD_START_DELAY 	60   //1M

#define BD_TIME 180
#define BD_DELAY 60

//水路控制方案
enum
{
    HEART_POT = 0x01, //热灌
    HMI_KEY = 0x02,
    OPEN_PWP = 0x04, //开盖时，打开净化泵
    TWO_COLD = 0x08, //双路出水
};
#define ChildKey_Cnt 1
#define ChildKey_Lose 5

//水源模式
enum
{
    WATER_AIR = 0x00,
    WATER_FILL = 0x01, //注水
    WATER_EXIT = 0x02, //
};

//水位
enum
{
    S_L = 0x01,
    S_M = 0x02,
    S_U = 0x04,
    D_L = 0x08, //制水
    D_M = 0x10, //满水
    D_U = 0x20,
    D_ML = 0x40, //制冷水位,中水位
};

//流量脉冲
enum
{
    L200 = 380,
    L300 = 570,
    L500 = 1250,
    L1000 = 2750,
    L1500 = 4350,
    L2000 = 5450,
};
//流量因子
#define L300_FACTOR 0.405f
#define L500_FACTOR 0.4f
//#define L200_FACTOR   0.28f
//#define L300_FACTOR   0.33f
//#define L500_FACTOR   0.34f
#define L1000_FACTOR 0.36363636363636363636363636363636f
#define L1500_FACTOR 0.34482758620689655172413793103448f
#define L2000_FACTOR 0.36697247706422018348623853211009f

//加热器流量,500ml/MIN,60s*HEAT_FACTOR
#define HEAT_FACTOR_S 8.3333333333333333333333333333333f
#define HEAT_FACTOR_500MS 4.1666666666666666666666666666667f

//出水状态
enum
{
    HEATER_IDLE = 0,
    HEATER_SEND,
    WATER_READ,
    WATER_OUT,
};
//加热器控制
enum
{
    CLOSE_HEAT = 0,
    OPEN_HEAT,
};

//加热器出水状态
enum
{
    HEAT_NO = 0,
    HEAT_OUTWATER = 0x01,
};

#define RH_DEALY 10
#define WRITEHEAT_MAX 250
#define CLOSEHEAT_MAX 5

//杀菌
#define STERILIZE_BIT0 0x01
#define STERILIZE_BIT1 0x02
//单次出水时间限制
#define WATER_MAXTIME 300 * 2 //5分钟

//定时保存时间
#define FIXED_SAVETIME 900
enum
{
    FAN_MODE_FIX = 0,       //定速模式
    FAN_MODE_PRESS_DIFF,    //压差模式
    FAN_MODE_AVR_RETURN,    //回风平均
    FAN_MODE_AVR_SUPPLY,    //送风平均
    FAN_MODE_TEMP_DIFF,     //温差平均
    FAN_MODE_MAX_RETURN,    //回风热点
    FAN_MODE_MAX_SUPPLY,    //送风热点
    FAN_MODE_TEMP_MAX_DIFF, //温差热点
    FAM_MODE_INV_COMP,      //变频跟踪
};
//Close first
enum
{
    PUMP_FIRET = 0x00,
    VALVE_FIRST = 0x01, //
};

//时间
#define INTERAL_TIME 24*3600*2//间隔24小时
#define CLOSE_TIME 5

void hum_capacity_calc(void);
void req_execution(int16_t target_req_temp, int16_t target_req_hum);
void req_bitmap_op(uint8_t component_bpos, uint8_t action);
void Close_DIS_PWR(uint8_t u8Type);
void UV_req_exe(uint8_t u8Type,uint8_t u8Delay);
uint8_t Sys_Get_Storage_Signal(void);
#endif //__REQ_EXE_H__
