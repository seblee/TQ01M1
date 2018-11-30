#include <rtthread.h>
#include <components.h>
#include "sys_conf.h"
#include "calc.h"
#include "sys_def.h"
#include "i2c_bsp.h"
#include "authentication.h"
#include "local_status.h"
#include "global_var.h"
#include "event_record.h"
#include "password.h"
#include "team.h"
#include "reg_map_check.h"
#include "mb_event_cpad.h"
#include "dio_bsp.h"
//configuration parameters

#ifndef var_log
#define var_log(N, ...) rt_kprintf("####[var %s:%4d] " N "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif /* at_log(...) */

typedef enum
{
    INIT_LOAD_USR = 0,
    INIT_LOAD_FACT,
    INIT_LOAD_DEBUT,
    INIT_LOAD_DEFAULT,
} init_state_em;

#define EE_FLAG_LOAD_USR 0xdf
#define EE_FLAG_LOAD_FACT 0x1b
#define EE_FLAG_LOAD_DFT 0x9b
#define EE_FLAG_LOAD_DEBUT 0xff

#define EE_FLAG_EMPTY 0x00
#define EE_FLAG_OK 0x01
#define EE_FLAG_ERROR 0x02

alarm_acl_conf_st g_alarm_acl_inst[MAX_ALARM_ACL_NUM]; //alarm check list declairation
sys_reg_st g_sys;                                      //global parameter declairation
local_reg_st l_sys;                                    //local status declairation
/**
 * 	 序号	指针	最小值	 最大值   默认值  权限		
 * 	{0, 	NULL,	0, 		3600,	0, 		0, 	 1, NULL}, 
 *
 ***/
const conf_reg_map_st conf_reg_map_inst[CONF_REG_MAP_NUM] = {
    //id,mapped registers,min,max,dft,permission,r/w,chk_prt
    {0, NULL, 0, 3600, 0, 0, 1, NULL},
    {1, NULL, 0, 3600, 0, 0, 1, NULL},
    {2, &g_sys.config.general.surv_baudrate, 0, 5, 1, 1, 1, NULL},
    {3, &g_sys.config.general.surv_addr, 1, 128, 1, 1, 1, NULL},
    {4, &g_sys.config.general.diagnose_mode_en, 0, 1, 0, 2, 1, NULL},
    {5, &g_sys.config.general.alarm_bypass_en, 0, 1, 0, 3, 1, NULL},
    {6, &g_sys.config.general.testing_mode_en, 0, 1, 0, 4, 1, NULL},
    {7, &g_sys.config.general.power_mode_mb_en, 0, 1, 1, 3, 1, NULL},
    {8, &g_sys.config.dev_mask.din_bitmap_polarity[0], 0, 0xFFFF, 0x3E5B, 3, 1, NULL}, // DI极性
    {9, &g_sys.config.dev_mask.din_bitmap_polarity[1], 0, 0xffff, 0x00, 3, 1, NULL},
    {10, &g_sys.config.dev_mask.ain, 0, 0xffff, 0x001F, 3, 1, NULL},
    {11, &g_sys.config.dev_mask.din[0], 0, 0xFFFF, 0x3E7F, 3, 1, NULL}, // DI屏蔽位
    {12, &g_sys.config.dev_mask.din[1], 0, 0xffff, 0x0000, 3, 1, NULL},
    {13, &g_sys.config.dev_mask.aout, 0, 0x003f, 0x0001, 3, 1, NULL},
    {14, &g_sys.config.dev_mask.mb_comp, 0, 0xFFFF, 0x01, 3, 1, NULL},
    {15, &g_sys.config.dev_mask.dout[0], 0, 0xffff, DO_MASK1, 3, 1, NULL},
    {16, &g_sys.config.dev_mask.dout[1], 0, 0xffff, DO_MASK2, 3, 1, NULL},
    {17, NULL, 0, 0, 0, 0, 1, NULL},
    {18, &g_sys.config.fan.mode, 0, 8, 0, 1, 1, NULL},
    {19, NULL, 0, 3600, 0, 0, 1, NULL},
    {20, NULL, 0, 3600, 0, 0, 1, NULL},
    {21, NULL, 0, 3600, 0, 0, 1, NULL},
    {22, &g_sys.config.compressor.startup_delay, 0, 600, 0, 2, 1, NULL},
    {23, &g_sys.config.compressor.stop_delay, 0, 90, 5, 2, 1, NULL},
    {24, &g_sys.config.compressor.min_runtime, 60, 600, 180, 2, 1, NULL},
    {25, &g_sys.config.compressor.min_stoptime, 60, 600, 180, 2, 1, NULL},
    {26, &g_sys.config.compressor.startup_lowpress_shield, 60, 600, 120, 2, 1, NULL},
    {27, &g_sys.config.ComPara.u16SN_Code[0], 0, 0xFFFF, 0, 2, 1, NULL},
    {28, &g_sys.config.ComPara.u16SN_Code[1], 0, 0xFFFF, 0, 2, 1, NULL},
    {29, &g_sys.config.ComPara.u16SN_Code[2], 0, 0xFFFF, 0, 2, 1, NULL},
    {30, &g_sys.config.ComPara.u16SN_Code[3], 0, 0xFFFF, 0, 2, 1, NULL},
    {31, NULL, 0, 3600, 0, 0, 1, NULL},
    {32, NULL, 0, 3600, 0, 0, 1, NULL},
    {33, NULL, 0, 3600, 0, 0, 1, NULL},
    {34, NULL, 0, 3600, 0, 0, 1, NULL},
    {35, &g_sys.config.ComPara.u16M_Type, 0, 100, 0, 2, 1, NULL},
    {36, &g_sys.config.ComPara.u16Power_Mode, 0, 1, 1, 2, 1, NULL},
    {37, &g_sys.config.ComPara.u16Start_Temp, 0, 800, 150, 2, 1, NULL},
    {38, &g_sys.config.ComPara.u16Start_Humidity, 0, 999, 350, 2, 1, NULL},
    {39, &g_sys.config.ComPara.u16Stop_Temp, 0, 800, 100, 2, 1, NULL},
    {40, &g_sys.config.ComPara.u16Stop_Humidity, 0, 999, 300, 2, 1, NULL},
    {41, &g_sys.config.ComPara.u16Start_Defrost_Temp, 0, 0xFFFF, (uint16_t)-30, 2, 1, NULL},
    {42, &g_sys.config.ComPara.u16Stop_Defrost_Temp, 0, 0xFFFF, 60, 2, 1, NULL},
    {43, &g_sys.config.ComPara.u16Sterilize_Time[0], 1, 600, 5, 2, 1, NULL},
    {44, &g_sys.config.ComPara.u16Sterilize_Interval[0], 1, 10000, 480, 2, 1, NULL},
    {45, &g_sys.config.ComPara.u16Water_Ctrl, 0, 0xFFFF, 0x0000, 2, 1, NULL},
    {EE_WATER_MODE, &g_sys.config.ComPara.u16Water_Mode, 0, 3, 0, 2, 1, NULL},
    {EE_WATER_FLOW, &g_sys.config.ComPara.u16Water_Flow, 0, 65500, 1000, 2, 1, NULL},
    {48, &g_sys.config.ComPara.u16NormalWater_Temp, 0, 400, 230, 2, 1, NULL},
    {49, &g_sys.config.ComPara.u16HotWater_Temp, 0, 1000, 500, 2, 1, NULL},
    {50, &g_sys.config.ComPara.u16WaterSource_Mode, 0, 1, 0, 2, 1, NULL},
    {51, &g_sys.config.ComPara.u16Change_WaterTank, 0, 1, 0, 2, 1, NULL},
    {52, &g_sys.config.ComPara.u16Sterilize_Mode, 0x01, 0x03, 0x03, 2, 1, NULL},
    {53, &g_sys.config.ComPara.u16Sterilize_Time[1], 1, 600, 5, 2, 1, NULL},
    {54, &g_sys.config.ComPara.u16Sterilize_Interval[1], 1, 10000, 480, 2, 1, NULL},
    {55, NULL, 0, 3600, 0, 0, 1, NULL},
    {56, &g_sys.config.ComPara.u16Reset, 0, 0xFF, 0, 2, 1, NULL},
    {57, &g_sys.config.ComPara.u16Test_Mode_Type, 0, 0xFF, 0, 2, 1, NULL}, //121
    {58, &g_sys.config.ComPara.u16Manual_Test_En, 0, 2, 0, 2, 1, NULL},    //122
    {59, &l_sys.bitmap[0][BITMAP_MANUAL], 0, 0xffff, 0, 1, 1, NULL},
    {60, &g_sys.config.ComPara.u16TPower_En, 0, 1, 0, 2, 1, NULL},
    {61, &g_sys.config.ComPara.u16TPower_On, 0, 0xFFFF, 0x0600, 2, 1, NULL},
    {62, &g_sys.config.ComPara.u16TPower_Off, 0, 0xFFFF, 0x1700, 2, 1, NULL},
    {63, &g_sys.config.alarm[ACL_FILTER_OT].alarm_param, 100, 65000, 4320, 2, 1, NULL},
    {64, NULL, 0, 3600, 0, 0, 1, NULL},
    {65, &g_sys.config.alarm[ACL_FILTER_ELEMENT_0_OT].alarm_param, 100, 65535, 22000, 2, 1, NULL},
    {66, &g_sys.config.ComPara.u16Clear_RT, 0, 0xFF, 0, 2, 1, NULL},
    {67, &g_sys.config.ComPara.u16Clear_ALARM, 0, 0xFF, 0, 2, 1, NULL},
    {68, &g_sys.config.ComPara.u16Set_Time[0], 0, 0xFFFF, 0, 2, 1, NULL},
    {69, &g_sys.config.ComPara.u16Set_Time[1], 0, 0xFFFF, 0, 2, 1, NULL},
    {70, &g_sys.config.ComPara.u16Fan_Start_Delay, 1, 600, 180, 1, 1, NULL},
    {71, &g_sys.config.ComPara.u16Fan_Stop_Delay, 1, 300, 60, 1, 1, NULL},
    {72, &g_sys.config.ComPara.u16Comp_Interval, 10, 600, 30, 2, 1, NULL},
    {73, NULL, 0, 3600, 0, 0, 1, NULL},
    {74, NULL, 0, 3600, 0, 0, 1, NULL},
    {75, &g_sys.config.general.ai_cali[0], 0, 0xffff, 0, 2, 1, NULL},
    {76, &g_sys.config.general.ntc_cali[0], 0, 0xffff, 0, 2, 1, NULL},
    {77, &g_sys.config.general.ntc_cali[1], 0, 0xffff, 0, 2, 1, NULL},
    {78, &g_sys.config.general.ntc_cali[2], 0, 0xffff, 0, 2, 1, NULL},
    {79, &g_sys.config.general.temp_sensor_cali[0].temp, 0, 0xffff, 0, 2, 1, NULL},
    {80, &g_sys.config.general.temp_sensor_cali[0].hum, 0, 0xffff, 0, 2, 1, NULL},
    {81, &g_sys.config.general.ntc_cali[3], 0, 0xffff, 0, 2, 1, NULL},
    {82, NULL, 0, 3600, 0, 0, 1, NULL},
    {83, (uint16_t *)&l_sys.ao_list[AO_EC_FAN][BITMAP_MANUAL], 0, 100, 0, 0, 1, NULL}, //147
    {84, &g_sys.config.fan.set_speed, 0, 100, 70, 2, 1, NULL},
    {85, NULL, 0, 3600, 0, 2, 1, NULL},
    {86, &g_sys.config.ComPara.u16ColdWater_Mode, 0, 1, 0, 2, 1, NULL},
    {87, &g_sys.config.ComPara.u16ColdWater_StartTemp, 50, 400, 150, 2, 1, NULL},
    {88, &g_sys.config.ComPara.u16ColdWater_StopTemp, 10, 300, 70, 2, 1, NULL},
    {89, &g_sys.config.ComPara.u16HeatFan_StartTemp, 0, 1000, 600, 2, 1, NULL},
    {90, &g_sys.config.ComPara.u16HeatFan_StopTemp, 0, 1000, 400, 2, 1, NULL},
    {91, &g_sys.config.ComPara.u16FILTER_ELEMENT_Type, 0, 1, 0, 2, 1, NULL},
    {92, &g_sys.config.alarm[ACL_FILTER_ELEMENT_1_OT].alarm_param, 100, 65535, 22000, 2, 1, NULL},
    {93, &g_sys.config.alarm[ACL_FILTER_ELEMENT_2_OT].alarm_param, 100, 65535, 22000, 2, 1, NULL},
    {94, &g_sys.config.alarm[ACL_FILTER_ELEMENT_3_OT].alarm_param, 100, 65535, 22000, 2, 1, NULL},
    {95, &g_sys.config.alarm[ACL_FILTER_ELEMENT_4_OT].alarm_param, 100, 65535, 22000, 2, 1, NULL},
    {96, &g_sys.config.alarm[ACL_UV1_OT].alarm_param, 100, 65535, 2000, 2, 1, NULL},
    {97, &g_sys.config.alarm[ACL_UV2_OT].alarm_param, 100, 65535, 2000, 2, 1, NULL},
    {98, NULL, 0, 3600, 0, 2, 1, NULL},
    {99, NULL, 0, 3600, 0, 2, 1, NULL},
    {100, NULL, 0, 3600, 0, 2, 1, NULL},
    {101, &g_sys.config.Platform.Fixed_Report, 10, 1000, 30, 2, 1, NULL},
    {102, &g_sys.config.Platform.Real_Report, 5, 1000, 10, 2, 1, NULL},
    {103, NULL, 0, 3600, 0, 2, 1, NULL},
    {104, NULL, 0, 3600, 0, 2, 1, NULL},
    {105, NULL, 0, 3600, 0, 2, 1, NULL},
#ifdef SYS_HMI_VJL

#else
    {106, NULL, 0, 3600, 0, 0, 1, NULL},
    {107, NULL, 0, 3600, 0, 0, 1, NULL},
    {108, NULL, 0, 3600, 0, 2, 1, NULL},
    {109, NULL, 0, 3600, 0, 2, 1, NULL},
    {110, NULL, 0, 3600, 0, 2, 1, NULL},
    {111, NULL, 0, 3600, 0, 0, 1, NULL},
    {112, NULL, 0, 3600, 0, 0, 1, NULL},
    {113, NULL, 0, 3600, 0, 2, 1, NULL},
    {114, NULL, 0, 3600, 0, 2, 1, NULL},
    {115, NULL, 0, 3600, 0, 2, 1, NULL},
    {116, NULL, 0, 3600, 0, 2, 1, NULL},
    {117, NULL, 0, 3600, 0, 0, 1, NULL},
    {118, NULL, 0, 3600, 0, 2, 1, NULL},
    {119, NULL, 0, 3600, 0, 2, 1, NULL},
    {120, NULL, 0, 3600, 0, 2, 1, NULL},
    {121, NULL, 0, 3600, 0, 0, 1, NULL},
    {122, NULL, 0, 3600, 0, 0, 1, NULL},
    {123, NULL, 0, 3600, 0, 2, 1, NULL},
    {124, NULL, 0, 3600, 0, 2, 1, NULL},
    {125, NULL, 0, 3600, 0, 2, 1, NULL},
    {126, NULL, 0, 3600, 0, 2, 1, NULL},
    {127, NULL, 0, 3600, 0, 0, 1, NULL},
    {128, NULL, 0, 3600, 0, 2, 1, NULL},
    {129, NULL, 0, 3600, 0, 2, 1, NULL},
    {130, NULL, 0, 3600, 0, 2, 1, NULL},
    {131, NULL, 0, 3600, 0, 0, 1, NULL},
    {132, NULL, 0, 3600, 0, 0, 1, NULL},
    {133, NULL, 0, 3600, 0, 2, 1, NULL},
    {134, NULL, 0, 3600, 0, 2, 1, NULL},
    {135, NULL, 0, 3600, 0, 2, 1, NULL},
    {136, NULL, 0, 3600, 0, 2, 1, NULL},
    {137, NULL, 0, 3600, 0, 0, 1, NULL},
    {138, NULL, 0, 3600, 0, 2, 1, NULL},
    {139, NULL, 0, 3600, 0, 2, 1, NULL},
    {140, NULL, 0, 3600, 0, 2, 1, NULL},
    {141, NULL, 0, 3600, 0, 0, 1, NULL},
    {142, NULL, 0, 3600, 0, 0, 1, NULL},
    {143, NULL, 0, 3600, 0, 2, 1, NULL},
    {144, NULL, 0, 3600, 0, 2, 1, NULL},
    {145, NULL, 0, 3600, 0, 2, 1, NULL},
    {146, NULL, 0, 3600, 0, 2, 1, NULL},
    {147, NULL, 0, 3600, 0, 0, 1, NULL},
    {148, NULL, 0, 3600, 0, 2, 1, NULL},
    {149, NULL, 0, 3600, 0, 2, 1, NULL},
    {150, NULL, 0, 3600, 0, 2, 1, NULL},
    {151, NULL, 0, 3600, 0, 0, 1, NULL},
    {152, NULL, 0, 3600, 0, 0, 1, NULL},
    {153, NULL, 0, 3600, 0, 2, 1, NULL},
    {154, NULL, 0, 3600, 0, 2, 1, NULL},
    {155, NULL, 0, 3600, 0, 2, 1, NULL},
    {156, NULL, 0, 3600, 0, 2, 1, NULL},
    {157, NULL, 0, 3600, 0, 0, 1, NULL},
    {158, NULL, 0, 3600, 0, 2, 1, NULL},
    {159, NULL, 0, 3600, 0, 2, 1, NULL},
    {160, NULL, 0, 3600, 0, 2, 1, NULL},
    {161, NULL, 0, 3600, 0, 0, 1, NULL},
    {162, NULL, 0, 3600, 0, 0, 1, NULL},
    {163, NULL, 0, 3600, 0, 2, 1, NULL},
    {164, NULL, 0, 3600, 0, 2, 1, NULL},
    {165, NULL, 0, 3600, 0, 2, 1, NULL},
    {166, NULL, 0, 3600, 0, 2, 1, NULL},
    {167, NULL, 0, 3600, 0, 0, 1, NULL},
    {168, NULL, 0, 3600, 0, 2, 1, NULL},
    {169, NULL, 0, 3600, 0, 2, 1, NULL},
    {170, NULL, 0, 3600, 0, 2, 1, NULL},
    {171, NULL, 0, 3600, 0, 0, 1, NULL},
    {172, NULL, 0, 3600, 0, 0, 1, NULL},
    {173, NULL, 0, 3600, 0, 2, 1, NULL},
    {174, NULL, 0, 3600, 0, 2, 1, NULL},
    {175, NULL, 0, 3600, 0, 2, 1, NULL},
    {176, NULL, 0, 3600, 0, 2, 1, NULL},
    {177, NULL, 0, 3600, 0, 0, 1, NULL},
    {178, &g_sys.config.ComPara.Net_Conf.u16Net_WifiSet, 0, 0xFFFF, 0, 2, 1, NULL},
    {179, &g_sys.config.ComPara.Net_Conf.u16Net_Sel, 0, 0xFFFF, 0, 2, 1, NULL},
    {180, &g_sys.config.ComPara.Net_Conf.u16Wifi_Name[0], 0, 0xFFFF, 0, 2, 1, NULL},
    {181, &g_sys.config.ComPara.Net_Conf.u16Wifi_Name[1], 0, 0xFFFF, 0, 2, 1, NULL},
    {182, &g_sys.config.ComPara.Net_Conf.u16Wifi_Name[2], 0, 0xFFFF, 0, 2, 1, NULL},
    {183, &g_sys.config.ComPara.Net_Conf.u16Wifi_Name[3], 0, 0xFFFF, 0, 2, 1, NULL},
    {184, &g_sys.config.ComPara.Net_Conf.u16Wifi_Name[4], 0, 0xFFFF, 0, 2, 1, NULL},
    {185, &g_sys.config.ComPara.Net_Conf.u16Wifi_Name[5], 0, 0xFFFF, 0, 2, 1, NULL},
    {186, &g_sys.config.ComPara.Net_Conf.u16Wifi_Name[6], 0, 0xFFFF, 0, 2, 1, NULL},
    {187, &g_sys.config.ComPara.Net_Conf.u16Wifi_Name[7], 0, 0xFFFF, 0, 2, 1, NULL},
    {188, &g_sys.config.ComPara.Net_Conf.u16Wifi_Name[8], 0, 0xFFFF, 0, 2, 1, NULL},
    {189, &g_sys.config.ComPara.Net_Conf.u16Wifi_Name[9], 0, 0xFFFF, 0, 2, 1, NULL},
    {EE_WIFI_PASSWORD, &g_sys.config.ComPara.Net_Conf.u16Wifi_Password[0], 0, 0xFFFF, 0, 2, 1, NULL},
    {191, &g_sys.config.ComPara.Net_Conf.u16Wifi_Password[1], 0, 0xFFFF, 0, 2, 1, NULL},
    {192, &g_sys.config.ComPara.Net_Conf.u16Wifi_Password[2], 0, 0xFFFF, 0, 2, 1, NULL},
    {193, &g_sys.config.ComPara.Net_Conf.u16Wifi_Password[3], 0, 0xFFFF, 0, 2, 1, NULL},
    {194, &g_sys.config.ComPara.Net_Conf.u16Wifi_Password[4], 0, 0xFFFF, 0, 2, 1, NULL},
    {195, &g_sys.config.ComPara.Net_Conf.u16Wifi_Password[5], 0, 0xFFFF, 0, 2, 1, NULL},
    {196, &g_sys.config.ComPara.Net_Conf.u16Wifi_Password[6], 0, 0xFFFF, 0, 2, 1, NULL},
    {197, &g_sys.config.ComPara.Net_Conf.u16Wifi_Password[7], 0, 0xFFFF, 0, 2, 1, NULL},
    {198, &g_sys.config.ComPara.Net_Conf.u16Wifi_Password[8], 0, 0xFFFF, 0, 2, 1, NULL},
    {199, &g_sys.config.ComPara.Net_Conf.u16Wifi_Password[9], 0, 0xFFFF, 0, 2, 1, NULL},
    /***********************************************************************************/
    {200, &g_sys.config.ComPara.device_info[0], 0, 0xffff, 0, 2, 1, NULL},
    {201, &g_sys.config.ComPara.device_info[1], 0, 0xffff, 0, 2, 1, NULL},
    {202, &g_sys.config.ComPara.device_info[2], 0, 0xffff, 0, 2, 1, NULL},
    {203, &g_sys.config.ComPara.device_info[3], 0, 0xffff, 0, 2, 1, NULL},
    {204, &g_sys.config.ComPara.device_info[4], 0, 0xffff, 0, 2, 1, NULL},
    {205, &g_sys.config.ComPara.device_info[5], 0, 0xffff, 0, 2, 1, NULL},
    {206, &g_sys.config.ComPara.device_info[6], 0, 0xffff, 0, 2, 1, NULL},
    {207, &g_sys.config.ComPara.device_info[7], 0, 0xffff, 0, 2, 1, NULL},
    {208, &g_sys.config.ComPara.device_info[8], 0, 0xffff, 0, 2, 1, NULL},
    {209, &g_sys.config.ComPara.device_info[9], 0, 0xffff, 0, 2, 1, NULL},
    {210, &g_sys.config.ComPara.device_info[10], 0, 0xffff, 0, 2, 1, NULL},
    {211, &g_sys.config.ComPara.device_info[11], 0, 0xffff, 0, 2, 1, NULL},
    {212, &g_sys.config.ComPara.device_info[12], 0, 0xffff, 0, 2, 1, NULL},
    {213, &g_sys.config.ComPara.device_info[13], 0, 0xffff, 0, 2, 1, NULL},
    {214, &g_sys.config.ComPara.device_info[14], 0, 0xffff, 0, 2, 1, NULL},
    {215, &g_sys.config.ComPara.device_info[15], 0, 0xffff, 0, 2, 1, NULL},
    {216, &g_sys.config.ComPara.device_info[16], 0, 0xffff, 0, 2, 1, NULL},
    {217, &g_sys.config.ComPara.device_info[17], 0, 0xffff, 0, 2, 1, NULL},
    {218, &g_sys.config.ComPara.device_info[18], 0, 0xffff, 0, 2, 1, NULL},
    {219, &g_sys.config.ComPara.device_info[19], 0, 0xffff, 0, 2, 1, NULL},
    {220, &g_sys.config.ComPara.device_info[20], 0, 0xffff, 0, 2, 1, NULL},
    {221, &g_sys.config.ComPara.device_info[21], 0, 0xffff, 0, 2, 1, NULL},
    {222, &g_sys.config.ComPara.device_info[22], 0, 0xffff, 0, 2, 1, NULL},
    {223, &g_sys.config.ComPara.device_info[23], 0, 0xffff, 0, 2, 1, NULL},
    {224, &g_sys.config.ComPara.device_info[24], 0, 0xffff, 0, 2, 1, NULL},
    {225, &g_sys.config.ComPara.device_info[25], 0, 0xffff, 0, 2, 1, NULL},
    {226, &g_sys.config.ComPara.device_info[26], 0, 0xffff, 0, 2, 1, NULL},
    {227, &g_sys.config.ComPara.device_info[27], 0, 0xffff, 0, 2, 1, NULL},
    {228, &g_sys.config.ComPara.device_info[28], 0, 0xffff, 0, 2, 1, NULL},
    {229, &g_sys.config.ComPara.device_info[29], 0, 0xffff, 0, 2, 1, NULL},
    {230, &g_sys.config.ComPara.device_info[30], 0, 0xffff, 0, 2, 1, NULL},
    {231, &g_sys.config.ComPara.device_info[31], 0, 0xffff, 0, 2, 1, NULL},
    {232, &g_sys.config.ComPara.device_info[32], 0, 0xffff, 0, 2, 1, NULL},
    {233, &g_sys.config.ComPara.device_info[33], 0, 0xffff, 0, 2, 1, NULL},
    {234, &g_sys.config.ComPara.device_info[34], 0, 0xffff, 0, 2, 1, NULL},
    {235, &g_sys.config.ComPara.device_info[35], 0, 0xffff, 0, 2, 1, NULL},
    {236, &g_sys.config.ComPara.device_info[36], 0, 0xffff, 0, 2, 1, NULL},
    {237, &g_sys.config.ComPara.device_info[37], 0, 0xffff, 0, 2, 1, NULL},
    {238, &g_sys.config.ComPara.device_info[38], 0, 0xffff, 0, 2, 1, NULL},
    {239, &g_sys.config.ComPara.device_info[39], 0, 0xffff, 0, 2, 1, NULL},
    {240, &g_sys.config.ComPara.device_info[40], 0, 0xffff, 0, 2, 1, NULL},
    {241, &g_sys.config.ComPara.device_info[41], 0, 0xffff, 0, 2, 1, NULL},
    {242, &g_sys.config.ComPara.device_info[42], 0, 0xffff, 0, 2, 1, NULL},
    {243, &g_sys.config.ComPara.device_info[43], 0, 0xffff, 0, 2, 1, NULL},
    {244, &g_sys.config.ComPara.device_info[44], 0, 0xffff, 0, 2, 1, NULL},
    {245, &g_sys.config.ComPara.device_info[45], 0, 0xffff, 0, 2, 1, NULL},
    {246, &g_sys.config.ComPara.device_info[46], 0, 0xffff, 0, 2, 1, NULL},
    {247, &g_sys.config.ComPara.device_info[47], 0, 0xffff, 0, 2, 1, NULL},
    {248, &g_sys.config.ComPara.device_info[48], 0, 0xffff, 0, 2, 1, NULL},
    {249, &g_sys.config.ComPara.device_info[49], 0, 0xffff, 0, 2, 1, NULL},
    {250, &g_sys.config.ComPara.device_info[50], 0, 0xffff, 0, 2, 1, NULL},
    {251, &g_sys.config.ComPara.device_info[51], 0, 0xffff, 0, 2, 1, NULL},
    {252, &g_sys.config.ComPara.device_info[52], 0, 0xffff, 0, 2, 1, NULL},
    {253, &g_sys.config.ComPara.device_info[53], 0, 0xffff, 0, 2, 1, NULL},
    {254, &g_sys.config.ComPara.device_info[54], 0, 0xffff, 0, 2, 1, NULL},
    {255, &g_sys.config.ComPara.device_info[55], 0, 0xffff, 0, 2, 1, NULL},
    {256, &g_sys.config.ComPara.device_info[56], 0, 0xffff, 0, 2, 1, NULL},
    {257, &g_sys.config.ComPara.device_info[57], 0, 0xffff, 0, 2, 1, NULL},
    {258, &g_sys.config.ComPara.device_info[58], 0, 0xffff, 0, 2, 1, NULL},
    {259, &g_sys.config.ComPara.device_info[59], 0, 0xffff, 0, 2, 1, NULL},
    {260, &g_sys.config.ComPara.device_info[60], 0, 0xffff, 0, 2, 1, NULL},
    {261, &g_sys.config.ComPara.device_info[61], 0, 0xffff, 0, 2, 1, NULL},
    {262, &g_sys.config.ComPara.device_info[62], 0, 0xffff, 0, 2, 1, NULL},
    {263, &g_sys.config.ComPara.device_info[63], 0, 0xffff, 0, 2, 1, NULL},
    {264, &g_sys.config.ComPara.device_info[64], 0, 0xffff, 0, 2, 1, NULL},
    {265, &g_sys.config.ComPara.device_info[65], 0, 0xffff, 0, 2, 1, NULL},
    {266, &g_sys.config.ComPara.device_info[66], 0, 0xffff, 0, 2, 1, NULL},
    {267, &g_sys.config.ComPara.device_info[67], 0, 0xffff, 0, 2, 1, NULL},
    {268, &g_sys.config.ComPara.device_info[68], 0, 0xffff, 0, 2, 1, NULL},
    {269, &g_sys.config.ComPara.device_info[69], 0, 0xffff, 0, 2, 1, NULL},
    {270, &g_sys.config.ComPara.device_info[70], 0, 0xffff, 0, 2, 1, NULL},
    {271, &g_sys.config.ComPara.device_info[71], 0, 0xffff, 0, 2, 1, NULL},
    {272, &g_sys.config.ComPara.device_info[72], 0, 0xffff, 0, 2, 1, NULL},
    {273, &g_sys.config.ComPara.device_info[73], 0, 0xffff, 0, 2, 1, NULL},
    {274, &g_sys.config.ComPara.device_info[74], 0, 0xffff, 0, 2, 1, NULL},
    {275, &g_sys.config.ComPara.device_info[75], 0, 0xffff, 0, 2, 1, NULL},
    {276, &g_sys.config.ComPara.device_info[76], 0, 0xffff, 0, 2, 1, NULL},
    {277, &g_sys.config.ComPara.device_info[77], 0, 0xffff, 0, 2, 1, NULL},
    {278, &g_sys.config.ComPara.device_info[78], 0, 0xffff, 0, 2, 1, NULL},
    {279, &g_sys.config.ComPara.device_info[79], 0, 0xffff, 0, 2, 1, NULL},
    {280, &g_sys.config.ComPara.device_info[80], 0, 0xffff, 0, 2, 1, NULL},
    {281, &g_sys.config.ComPara.device_info[81], 0, 0xffff, 0, 2, 1, NULL},
    {282, &g_sys.config.ComPara.device_info[82], 0, 0xffff, 0, 2, 1, NULL},
    {283, &g_sys.config.ComPara.device_info[83], 0, 0xffff, 0, 2, 1, NULL},
    {284, &g_sys.config.ComPara.device_info[84], 0, 0xffff, 0, 2, 1, NULL},
    {285, &g_sys.config.ComPara.device_info[85], 0, 0xffff, 0, 2, 1, NULL},
    {286, &g_sys.config.ComPara.device_info[86], 0, 0xffff, 0, 2, 1, NULL},
    {287, &g_sys.config.ComPara.device_info[87], 0, 0xffff, 0, 2, 1, NULL},
    {288, &g_sys.config.ComPara.device_info[88], 0, 0xffff, 0, 2, 1, NULL},
    {289, &g_sys.config.ComPara.device_info[89], 0, 0xffff, 0, 2, 1, NULL},
    {290, &g_sys.config.ComPara.device_info[90], 0, 0xffff, 0, 2, 1, NULL},
    {291, &g_sys.config.ComPara.device_info[91], 0, 0xffff, 0, 2, 1, NULL},
    {292, &g_sys.config.ComPara.device_info[92], 0, 0xffff, 0, 2, 1, NULL},
    {293, &g_sys.config.ComPara.device_info[93], 0, 0xffff, 0, 2, 1, NULL},
    {294, &g_sys.config.ComPara.device_info[94], 0, 0xffff, 0, 2, 1, NULL},
    {295, &g_sys.config.ComPara.device_info[95], 0, 0xffff, 0, 2, 1, NULL},
    {296, &g_sys.config.ComPara.device_info[96], 0, 0xffff, 0, 2, 1, NULL},
    {297, &g_sys.config.ComPara.device_info[97], 0, 0xffff, 0, 2, 1, NULL},
    {298, &g_sys.config.ComPara.device_info[98], 0, 0xffff, 0, 2, 1, NULL},
    {299, &g_sys.config.ComPara.device_info[99], 0, 0xffff, 0, 2, 1, NULL},

#endif
};

//status register map declairation
const sts_reg_map_st status_reg_map_inst[STATUS_REG_MAP_NUM] = {
    //id,mapped registers,                    default
    {0, &g_sys.status.ComSta.u16Hardware_ver, HARDWARE_VER},
    {1, &g_sys.status.ComSta.u16Software_ver, SOFTWARE_VER},
    {2, &g_sys.status.ComSta.u16Status_remap[0], 0},
    {3, &g_sys.status.ComSta.u16Din_bitmap[0], 0},
    {4, &g_sys.status.ComSta.u16Dout_bitmap[0], 0},
    {5, &g_sys.status.ComSta.u16Alarm_bitmap[0], 0},
    {6, &g_sys.status.ComSta.u16Alarm_bitmap[1], 0},
    {7, &g_sys.status.ComSta.u16Ain[0], 0},
    {8, &g_sys.status.ComSta.u16Ain[1], 0},
    {9, &g_sys.status.ComSta.u16Ain[2], 0},
    {10, &g_sys.status.ComSta.u16Ain[3], 0},
    {11, &g_sys.status.ComSta.u16TH[0].Temp, 0},
    {12, &g_sys.status.ComSta.u16TH[0].Hum, 0},
    {13, &g_sys.status.ComSta.u16Last_Water, 0},
    {14, &g_sys.status.ComSta.u16Cumulative_Water[0], 0},
    {15, &g_sys.status.ComSta.u16Cumulative_Water[1], 0},
    {16, &g_sys.status.ComSta.u16PM25, 0},
    {17, &g_sys.status.ComSta.u16Dout_bitmap[1], 0},
    {18, &g_sys.status.ComSta.u16Ain[AI_NTC4], 0},
    // {18, &g_sys.status.ComSta.REQ_TEST[0], 0},
    {19, &g_sys.status.ComSta.u16AO[0], 0},
    {20, &g_sys.status.ComSta.u16Runtime[1][DO_FILLTER_DUMMY_BPOS], 0},
    // {20, &g_sys.status.ComSta.REQ_TEST[1], 0},
    {21, &g_sys.status.ComSta.u16Runtime[1][DO_FILLTER_ELEMENT_DUMMY_BPOS_0], 0},
    {22, &g_sys.status.ComSta.u16Runtime[1][DO_FAN_BPOS], 0},
    {23, &g_sys.status.ComSta.u16Runtime[1][DO_COMP1_BPOS], 0},
    {24, &g_sys.status.ComSta.u16Runtime[1][DO_COMP2_BPOS], 0},
    {25, &g_sys.status.ComSta.u16Runtime[1][DO_FILLTER_ELEMENT_DUMMY_BPOS_1], 0},
    {26, &g_sys.status.ComSta.u16Runtime[1][DO_FILLTER_ELEMENT_DUMMY_BPOS_2], 0},
    {27, &g_sys.status.ComSta.u16Runtime[1][DO_FILLTER_ELEMENT_DUMMY_BPOS_3], 0},
    {28, &g_sys.status.ComSta.u16Runtime[1][DO_FILLTER_ELEMENT_DUMMY_BPOS_4], 0},
    {29, &g_sys.status.ComSta.u16Runtime[1][DO_UV1_BPOS], 0},
    {30, &g_sys.status.ComSta.u16Runtime[1][DO_UV2_BPOS], 0},
    {31, &g_sys.status.ComSta.REQ_TEST[0], 0},
    {32, &g_sys.status.ComSta.REQ_TEST[1], 0},
    {33, NULL, 0},
    {34, NULL, 0},
    {35, &g_sys.status.ComSta.u16WL, 0},
    {36, &g_sys.status.ComSta.u16Status_remap[1], 0},
    {37, &g_sys.status.ComSta.u16Cur_Water, 0},
    {38, NULL, 0},
    {39, NULL, 0},
};

/**
  * @brief  get eeprom program status
  * @param  None
  * @retval 
		`EE_FLAG_OK:		configuration data valid in eeprom
		`EE_FLAG_EMPTY:	eeprom empty
  */

static init_state_em get_ee_status(void)
{
    init_state_em em_init_state;
    uint8_t ee_pflag;
    rt_thread_delay(10);                       //wait for eeprom power on
    I2C_EE_BufRead(&ee_pflag, STS_EE_ADDR, 1); //启动区

    l_sys.SEL_Jump = GetSEL();
    //TEST
    if (l_sys.SEL_Jump & Start_Init) //上电初始化
    {
        reset_runtime(0xFF); //清零所有运行时间
        ee_pflag = EE_FLAG_LOAD_DEBUT;
    }
    //		//TEST
    // reset_runtime(0xFF); //清零所有运行时间
    // ee_pflag = EE_FLAG_LOAD_DEBUT;
    switch (ee_pflag)
    {
    case (EE_FLAG_LOAD_USR):
    {
        em_init_state = INIT_LOAD_USR;
        break;
    }

    case (EE_FLAG_LOAD_FACT):
    {
        em_init_state = INIT_LOAD_FACT;
        break;
    }
    case (EE_FLAG_LOAD_DFT):
    {
        em_init_state = INIT_LOAD_DEFAULT;
        break;
    }
    default:
    {
        em_init_state = INIT_LOAD_DEBUT;
        break;
    }
    }
    return em_init_state;
}

/**
  * @brief 	save system configurable variables initialization
	* @param  0:load usr1 eeprom
						1:load usr2 eeprom
						2:load facotry eeprom
						3:load default eeprom
	* @retval err_cnt: mismatch read/write data count
  */

uint16_t set_load_flag(uint8_t ee_load_flag)
{
    uint8_t ee_flag;
    switch (ee_load_flag)
    {
    case (0):
    {
        ee_flag = EE_FLAG_LOAD_USR; //用户区
        break;
    }
    case (1):
    {
        ee_flag = EE_FLAG_LOAD_FACT;
        break;
    }
    case (2):
    {
        ee_flag = EE_FLAG_LOAD_DEBUT;
        break;
    }
    default:
    {
        ee_flag = EE_FLAG_LOAD_DFT;
        break;
    }
    }
    I2C_EE_BufWrite(&ee_flag, STS_EE_ADDR, 1);
    return 1;
}

/**
  * @brief 	save system configurable variables initialization
	* @param  addr_sel:
						`0: save current configuration to usr1 eeprom address
						`1:	save current configuration to usr2 eeprom address
						`2:	save current configuration to facotry eeprom address
	* @retval err_cnt: mismatch read/write data count
  */
uint16_t save_conf_reg(uint8_t addr_sel)
{
    uint16_t conf_reg[CONF_REG_MAP_NUM];
    uint16_t test_reg[CONF_REG_MAP_NUM];
    uint16_t i, j, err_cnt, chk_res;
    uint16_t ee_save_addr;
    uint8_t ee_flag, req;

    ee_save_addr = 0;
    err_cnt = 0;

    switch (addr_sel)
    {
    case (0):
    {
        ee_flag = EE_FLAG_LOAD_USR;
        break;
    }
    case (1): //保存工厂参数
    {
        ee_save_addr = CONF_REG_FACT_ADDR;
        ee_flag = EE_FLAG_LOAD_FACT;
        break;
    }
    default:
    {
        return 0xff;
    }
    }

    for (i = 0; i < CONF_REG_MAP_NUM; i++) //set configration reg with default value
    {
        conf_reg[i] = *(conf_reg_map_inst[i].reg_ptr);
    }
    if (ee_flag == EE_FLAG_LOAD_USR)
    {
        req = 0;
        for (j = 0; j < 3; j++)
        {
            if (j == 0)
            {
                ee_save_addr = CONF_REG_EE1_ADDR;
            }
            else if (j == 1)
            {
                ee_save_addr = CONF_REG_EE2_ADDR;
            }
            else
            {
                ee_save_addr = CONF_REG_EE3_ADDR;
            }

            I2C_EE_BufWrite((uint8_t *)conf_reg, ee_save_addr, (CONF_REG_MAP_NUM)*2); //save configuration data to eeprom
            for (i = 0; i < 10; i++)
                ;
            I2C_EE_BufRead((uint8_t *)test_reg, ee_save_addr, (CONF_REG_MAP_NUM)*2);
            for (i = 0; i < CONF_REG_MAP_NUM; i++)
            {
                if (conf_reg[i] != test_reg[i])
                {
                    err_cnt++;
                    rt_kprintf("\ni = %d,conf_reg=%d,test_reg=%d,err_cnt=%d\n", i, conf_reg[i], test_reg[i], err_cnt);
                }
            }
            if (err_cnt == 0)
            {
                chk_res = checksum_u16(conf_reg, CONF_REG_MAP_NUM); //set parameter checksum
                I2C_EE_BufWrite((uint8_t *)&chk_res, ee_save_addr + (CONF_REG_MAP_NUM * 2), 2);
                rt_kprintf("\nchk_res_addr = %d,,nchk_res=%d\n", ee_save_addr + (CONF_REG_MAP_NUM * 2), chk_res);
                I2C_EE_BufWrite(&ee_flag, STS_EE_ADDR, 1); //set eeprom program flag
            }
            else
            {
                req++;
            }
        }
        if (req < 2)
        {
            err_cnt = 0;
        }
        else
        {
            err_cnt = req;
        }
    }
    else
    {
        I2C_EE_BufWrite((uint8_t *)conf_reg, ee_save_addr, (CONF_REG_MAP_NUM)*2); //save configuration data to eeprom
        for (i = 0; i < 10; i++)
            ;
        I2C_EE_BufRead((uint8_t *)test_reg, ee_save_addr, (CONF_REG_MAP_NUM)*2);
        for (i = 0; i < CONF_REG_MAP_NUM; i++)
        {
            if (conf_reg[i] != test_reg[i])
            {
                err_cnt++;
            }
        }
        if (err_cnt == 0)
        {
            chk_res = checksum_u16(conf_reg, CONF_REG_MAP_NUM); //set parameter checksum
            I2C_EE_BufWrite((uint8_t *)&chk_res, ee_save_addr + (CONF_REG_MAP_NUM * 2), 2);
            I2C_EE_BufWrite(&ee_flag, STS_EE_ADDR, 1); //set eeprom program flag
        }
    }

    return err_cnt;
}
static uint16_t conf_reg_read_ee(uint16_t addr);
static uint16_t init_load_default(void)
{
    uint16_t i, ret;
    ret = 1;
    for (i = 0; i < CONF_REG_MAP_NUM; i++) //initialize global variable with default values
    {
#ifdef SYS_HMI_VJL
        if (conf_reg_map_inst[i].reg_ptr != NULL)
        {
            *(conf_reg_map_inst[i].reg_ptr) = conf_reg_map_inst[i].dft;
        }
#else
        if (i < CONF_REG_SID_START)
        {
            if (conf_reg_map_inst[i].reg_ptr != NULL)
            {
                *(conf_reg_map_inst[i].reg_ptr) = conf_reg_map_inst[i].dft;
            }
        }
        else //三元组信息恢复到内存变量
        {
            uint16_t sum = 0, sum_reg;
            sum_reg = sum;
            sum += conf_reg_read_ee(i);
            if (sum != sum_reg)
            {
                rt_kprintf("Err addr:%d\n", i);
            }
        }
#endif
    }
    authen_init();
    ret = 1;
    g_sys.status.status_remap[0] |= 0x0001;

    return ret;
}

/**
  * @brief  load system configuration data from eeprom
  * @param  None
  * @retval None
  */

static uint16_t conf_reg_read_ee(uint16_t addr)
{
    uint16_t reg;
    uint16_t ee_err, ret;
    reg = 0;
    ee_err = 0;
    ret = 0;
    ee_err = eeprom_compare_read(addr, &reg);

    if ((conf_reg_map_inst[addr].reg_ptr != NULL) && ((reg < conf_reg_map_inst[addr].min) || (reg > conf_reg_map_inst[addr].max)))
    {
        *(conf_reg_map_inst[addr].reg_ptr) = (conf_reg_map_inst[addr].dft);
        ret = 0;
    }
    else
    {
        *(conf_reg_map_inst[addr].reg_ptr) = reg;
        ret = 1;
    }

    if ((ee_err != 0) || (ret == 0))
    {
        ret = 1;
    }
    else
    {
        ret = 0;
    }
    return ret;
}

static uint16_t init_load_user_conf(void)
{
    uint16_t i, sum, sum_reg;
    sum = 0;

    for (i = 0; i < CONF_REG_MAP_NUM; i++)
    {
        sum_reg = sum;
        sum += conf_reg_read_ee(i);
        if (sum != sum_reg)
        {
            rt_kprintf("Err addr:%d\n", i);
        }
    }

    if (sum == 0)
    {
        return (1);
    }
    else
    {
        return (0);
    }
}

static uint16_t init_load_factory_conf(void)
{
    uint16_t buf_reg[CONF_REG_MAP_NUM + 1];
    uint16_t i, sum = 0, sum_reg;
    uint16_t chk_res;
    uint16_t ee_load_addr;
    ee_load_addr = CONF_REG_FACT_ADDR;

    I2C_EE_BufRead((uint8_t *)buf_reg, ee_load_addr, (CONF_REG_MAP_NUM + 1) * 2); //read eeprom data & checksum to data buffer
    rt_thread_delay(1);                                                           //wait for i2c opeartion comletion
    chk_res = checksum_u16(buf_reg, (CONF_REG_MAP_NUM + 1));
    if (chk_res != 0) //eeprom configuration data checksum fail
    {
        for (i = 0; i < CONF_REG_MAP_NUM; i++) //initialize global variable with default values
        {
#ifdef SYS_HMI_VJL
            if (conf_reg_map_inst[i].reg_ptr != NULL)
            {
                *(conf_reg_map_inst[i].reg_ptr) = conf_reg_map_inst[i].dft;
            }
#else
            if (i < CONF_REG_SID_START)
            {
                if (conf_reg_map_inst[i].reg_ptr != NULL)
                {
                    *(conf_reg_map_inst[i].reg_ptr) = conf_reg_map_inst[i].dft;
                }
            }
            else //三元组信息恢复到内存变量
            {
                sum_reg = sum;
                sum += conf_reg_read_ee(i);
                if (sum != sum_reg)
                {
                    rt_kprintf("Err addr:%d\n", i);
                }
            }
#endif
        }
        return 0;
    }
    else
    {
        for (i = 0; i < CONF_REG_MAP_NUM; i++)
        {
            *(conf_reg_map_inst[i].reg_ptr) = buf_reg[i];
        }

        return 1;
    }
}

/**
  * @brief  initialize system status reg data
  * @param  None
  * @retval None
  */
static void init_load_status(void)
{
    uint16_t i;
    for (i = 0; i < STATUS_REG_MAP_NUM; i++)
    {
        if (status_reg_map_inst[i].reg_ptr != NULL)
        {
            *(status_reg_map_inst[i].reg_ptr) = status_reg_map_inst[i].dft;
        }
    }

    //		I2C_EE_BufRead((uint8_t *)&g_sys.status.run_time,STS_REG_EE1_ADDR,sizeof(g_sys.status.run_time));	//read legacy checksum data
    I2C_EE_BufRead((uint8_t *)&g_sys.status.ComSta.u16Runtime, STS_REG_EE1_ADDR, sizeof(g_sys.status.ComSta.u16Runtime));
    //累计流量
    I2C_EE_BufRead((uint8_t *)&g_sys.status.ComSta.u16Cumulative_Water[0], STS_REG_EE1_ADDR + sizeof(g_sys.status.ComSta.u16Runtime), 4); //u16Cumulative_Water,
}

/**
  * @brief  system configurable variables initialization
  * @param  None
  * @retval 
			1:	load default data
			2:	load eeprom data
  */
uint16_t sys_global_var_init(void)
{
    uint16_t ret;
    init_state_em em_init_state;
    em_init_state = get_ee_status(); //get eeprom init status
    init_load_status();
    switch (em_init_state)
    {
    case (INIT_LOAD_USR): //load usr1 data
    {
        ret = init_load_user_conf();
        if (ret == 1)
        {
            g_sys.status.status_remap[0] |= 0x01;
            rt_kprintf("Usr conf file loaded successfully.\n");
        }
        else
        {
            g_sys.status.status_remap[0] &= 0xFFFE;
            rt_kprintf("Usr conf file loaded failed.\n");
        }
        break;
    }
    case (INIT_LOAD_FACT): //load factory data
    {
        ret = init_load_factory_conf();
        if (ret == 1)
        {
            save_conf_reg(0);
            set_load_flag(0);
            g_sys.status.status_remap[0] |= 0x01;
            rt_kprintf("Factory conf file loaded successfully.\n");
        }
        else
        {
            g_sys.status.status_remap[0] &= 0xFFFE;
            rt_kprintf("Factory conf file loaded failed.\n");
        }
        break;
    }
    case (INIT_LOAD_DEBUT): //resotre default configuration data, include reset password to default values
    {
        ret = init_load_default();
        if (ret == 1)
        {
            g_sys.status.status_remap[0] |= 0x01;
            save_conf_reg(0);
            save_conf_reg(1);
            set_load_flag(0);
            // reset dev run time
            reset_runtime(0xff);
            rt_kprintf("INIT_LOAD_DEBUT loaded successfully.\n");
        }
        else
        {
            g_sys.status.status_remap[0] &= 0xFFFE;
            rt_kprintf("INIT_LOAD_DEBUT loaded failed.\n");
        }
        break;
    }
    default: //resotre default configuration data, include reset password to default values
    {
        ret = init_load_default();
        if (ret == 1)
        {
            g_sys.status.status_remap[0] |= 0x01;
            rt_kprintf("Default conf data load successfully.\n");
        }
        else
        {
            g_sys.status.status_remap[0] &= 0xFFFE;
            rt_kprintf("Default conf file loaded failed.\n");
        }
        break;
    }
    }
    //测试模式和诊断模式复位。
    g_sys.config.ComPara.u16Manual_Test_En = 0;
    g_sys.config.general.alarm_bypass_en = 0;
    g_sys.config.ComPara.u16Test_Mode_Type = 0;
    //更换水箱
    g_sys.config.ComPara.u16Change_WaterTank = 0;
    //出水
    g_sys.config.ComPara.u16Water_Mode = 0;
    g_sys.config.ComPara.u16Water_Flow = 0;
    return ret;
}

uint16_t sys_local_var_init(void)
{
    uint16_t i, j;

    for (i = 0; i < REQ_MAX_CNT; i++)
    {
        for (j = 0; j < REQ_MAX_LEVEL; j++)
        {
            l_sys.require[i][j] = 0;
        }
    }
    //MAX REQ initialization
    l_sys.require[MAX_REQ][T_REQ] = 100;
    l_sys.require[MAX_REQ][H_REQ] = 0;

    l_sys.t_fsm_state = 0;

    for (i = 0; i < DO_MAX_CNT; i++)
    {
        l_sys.bitmap[0][i] = 0;
        l_sys.bitmap[1][i] = 0;
    }

    for (i = 0; i < DO_MAX_CNT; i++)
    {
        l_sys.comp_timeout[i] = 0;
    }

    for (i = 0; i < AO_MAX_CNT; i++)
    {
        for (j = 0; j < BITMAP_MAX_CNT; j++)
        {
            l_sys.ao_list[i][j] = 0;
        }
    }

    for (i = 0; i < L_FSM_STATE_MAX_NUM; i++)
    {
        l_sys.l_fsm_state[i] = 0;
    }
    l_sys.debug_flag = 0;
    l_sys.debug_tiemout = DEBUG_TIMEOUT_MAX;
    //		l_sys.debug_flag = 1;
    //		l_sys.debug_tiemout = DEBUG_TIMEOUT_MAX;
    l_sys.t_fsm_signal = 0;
    l_sys.ec_fan_diff_reg = 0;
    l_sys.ec_fan_suc_temp = 0;

    l_sys.TH_Check_Delay = 5; //上电后延迟5秒
    return 1;
}

/**
  * @brief  get current system permission
  * @param  None
  * @retval current system permission level
  */
uint16_t get_sys_permission(void)
{
    return g_sys.status.general.permission_level;
}

static int16_t eeprom_singel_write(uint16 base_addr, uint16 reg_offset_addr, uint16 wr_data, uint16_t rd_data)
{
    int16_t err_no;
    uint16_t wr_data_buf;
    uint16_t cs_data, ee_rd_cheksum;
    wr_data_buf = wr_data;

    err_no = I2C_EE_BufRead((uint8_t *)&cs_data, base_addr + (CONF_REG_MAP_NUM << 1), 2);
    //计算check_sum
    ee_rd_cheksum = cs_data ^ rd_data ^ wr_data;
    // 写寄存器
    err_no += I2C_EE_BufWrite((uint8_t *)&wr_data_buf, base_addr + (reg_offset_addr << 1), 2);
    // 写校验
    err_no += I2C_EE_BufWrite((uint8_t *)&ee_rd_cheksum, base_addr + (CONF_REG_MAP_NUM << 1), 2);

    return err_no;
}

int16_t eeprom_compare_read(uint16 reg_offset_addr, uint16_t *rd_data)
{
    uint16_t rd_buf0;
    uint16_t rd_buf1;
    uint16_t rd_buf2;
    int16_t ret;

    I2C_EE_BufRead((uint8_t *)&rd_buf0, CONF_REG_EE1_ADDR + (reg_offset_addr << 1), 2);
    I2C_EE_BufRead((uint8_t *)&rd_buf1, CONF_REG_EE2_ADDR + (reg_offset_addr << 1), 2);
    I2C_EE_BufRead((uint8_t *)&rd_buf2, CONF_REG_EE3_ADDR + (reg_offset_addr << 1), 2);

    //normal situation
    if ((rd_buf0 == rd_buf1) && (rd_buf2 == rd_buf1))
    {
        if (rd_buf0)
            *rd_data = rd_buf0;
        ret = 0;
    }
    else if ((rd_buf0 == rd_buf1) || (rd_buf0 == rd_buf2) || (rd_buf1 == rd_buf2))
    {
        *rd_data = rd_buf0;
        if (rd_buf0 == rd_buf1) //buf2!= buf1
        {
            *rd_data = rd_buf0;
            eeprom_singel_write(CONF_REG_EE3_ADDR, reg_offset_addr, rd_buf0, rd_buf2);
        }
        else if (rd_buf0 == rd_buf2) //buf2 = buf0, buf1错
        {
            *rd_data = rd_buf2;
            eeprom_singel_write(CONF_REG_EE2_ADDR, reg_offset_addr, rd_buf2, rd_buf1);
        }
        else //(rd_buf1 == rd_buf2)
        {
            *rd_data = rd_buf1;
            eeprom_singel_write(CONF_REG_EE1_ADDR, reg_offset_addr, rd_buf1, rd_buf0);
        }
        var_log("eeprom_compare_read :reg_offset_addr_ERRO= %d \n", reg_offset_addr);
        ret = 0;
    }
    else //三个都错误
    {
        *rd_data = 0x7fff;
        ret = 1;
        var_log("eeprom_compare_read :reg_offset_addr_ALL_ERRO= %d \n", reg_offset_addr);
    }
    return (ret);
}

int16_t eeprom_tripple_write(uint16 reg_offset_addr, uint16 wr_data, uint16_t rd_data)
{
    int16_t err_no;
    uint16_t wr_data_buf;
    uint16_t cs_data, ee_rd_cheksum;
    wr_data_buf = wr_data;

    err_no = eeprom_compare_read(CONF_REG_MAP_NUM, &cs_data);

    if (err_no == 0)
    {
        ee_rd_cheksum = cs_data ^ rd_data ^ wr_data;
    }
    else
    {
        rt_kprintf("eeprom_tripple_write : ERRO \n");
        return -1;
    }
    err_no = 0;

    //write data to eeprom
    err_no += I2C_EE_BufWrite((uint8_t *)&wr_data_buf, CONF_REG_EE1_ADDR + (reg_offset_addr << 1), 2);
    err_no += I2C_EE_BufWrite((uint8_t *)&wr_data_buf, CONF_REG_EE2_ADDR + (reg_offset_addr << 1), 2);
    err_no += I2C_EE_BufWrite((uint8_t *)&wr_data_buf, CONF_REG_EE3_ADDR + (reg_offset_addr << 1), 2);

    //write checksum data to eeprom
    err_no += I2C_EE_BufWrite((uint8_t *)&ee_rd_cheksum, CONF_REG_EE1_ADDR + (CONF_REG_MAP_NUM * 2), 2);
    err_no += I2C_EE_BufWrite((uint8_t *)&ee_rd_cheksum, CONF_REG_EE2_ADDR + (CONF_REG_MAP_NUM * 2), 2);
    err_no += I2C_EE_BufWrite((uint8_t *)&ee_rd_cheksum, CONF_REG_EE3_ADDR + (CONF_REG_MAP_NUM * 2), 2);

    return err_no;
}

/**
  * @brief  write register map with constraints.
  * @param  reg_addr: reg map address.
  * @param  wr_data: write data. 
	* @param  permission_flag:  
  *   This parameter can be one of the following values:
  *     @arg PERM_PRIVILEGED: write opertion can be performed dispite permission level
  *     @arg PERM_INSPECTION: write operation could only be performed when pass permission check
  * @retval 
  *   This parameter can be one of the following values:
  *     @arg 1: write operation success
  *     @arg 0: write operation fail
  */
uint16 reg_map_write(uint16 reg_addr, uint16 *wr_data, uint8_t wr_cnt, uint16 User_ID)
{
    uint16_t i;
    uint16_t err_code;
    uint16_t sys_permission;
    uint16_t ee_wr_data, ee_rd_data;
    err_code = CPAD_ERR_NOERR;
    //			g_sys.status.general.TEST=0x01;
    sys_permission = get_sys_permission();
    //modebus_slave permission_high_lev
    sys_permission = 4;
    if (User_ID == USER_MODEBUS_SLAVE)
    {
        sys_permission = 3;
    }
    if ((reg_addr + wr_cnt) > CONF_REG_MAP_NUM) //address range check
    {
        err_code = CPAD_ERR_ADDR_OR;
        //			rt_kprintf("MB_SLAVE CPAD_ERR_ADDR_OR1 failed\n");
        var_log("err_code=%d,User_ID=%d,reg_addr=%d,wr_data=%d\n", err_code, User_ID, reg_addr, *wr_data);
        return err_code;
    }
    //			g_sys.status.general.TEST|=0x02;

    for (i = 0; i < wr_cnt; i++) //permission check
    {
        if (conf_reg_map_inst[reg_addr + i].permission > sys_permission)
        {
            err_code = CPAD_ERR_PERM_OR;
            //					rt_kprintf("CPAD_ERR_PERM_OR1 failed\n");
            var_log("err_code=%d,User_ID=%d,reg_addr=%d,wr_data=%d\n", err_code, User_ID, reg_addr, *wr_data);
            return err_code;
        }
    }

    for (i = 0; i < wr_cnt; i++) //writablility check
    {
        if (conf_reg_map_inst[reg_addr + i].rw != 1)
        {
            err_code = CPAD_ERR_WR_OR;
            //					rt_kprintf("CPAD_ERR_WR_OR02 failed\n");
            var_log("err_code=%d,User_ID=%d,reg_addr=%d,wr_data=%d\n", err_code, User_ID, reg_addr, *wr_data);
            return err_code;
        }
    }
    //			g_sys.status.general.TEST|=0x04;
    for (i = 0; i < wr_cnt; i++) //min_max limit check
    {
        if ((*(wr_data + i) > conf_reg_map_inst[reg_addr + i].max) || (*(wr_data + i) < conf_reg_map_inst[reg_addr + i].min)) //min_max limit check
        {
            err_code = CPAD_ERR_DATA_OR;
            //						rt_kprintf("CPAD_ERR_WR_OR03 failed\n");
            var_log("err_code=%d,User_ID=%d,reg_addr=%d,wr_data=%d\n", err_code, User_ID, reg_addr, *wr_data);
            return err_code;
        }

        if (conf_reg_map_inst[reg_addr + i].chk_ptr != NULL)
        {
            if (conf_reg_map_inst[reg_addr + i].chk_ptr(*(wr_data + i)) == 0)
            {
                err_code = CPAD_ERR_CONFLICT_OR;
                //								rt_kprintf("CHK_PTR:CPAD_ERR_WR_OR failed\n");
                var_log("err_code=%d,User_ID=%d,reg_addr=%d,wr_data=%d\n", err_code, User_ID, reg_addr, *wr_data);
                return err_code;
            }
        }
    }
    //				g_sys.status.general.TEST|=0x08;
    for (i = 0; i < wr_cnt; i++) //data write
    {
        ee_rd_data = *(conf_reg_map_inst[reg_addr + i].reg_ptr);     //buffer legacy reg data
        ee_wr_data = *(wr_data + i);                                 //buffer current write data
        *(conf_reg_map_inst[reg_addr + i].reg_ptr) = *(wr_data + i); //write data to designated register
        //						g_sys.status.general.TEST|=0x10;
        //		var_log("ee_rd_data=%d,ee_wr_data=%X\n,", ee_rd_data, ee_wr_data);
        //写EEPROM参数
        if ((reg_addr == EE_WATER_MODE) || (reg_addr == EE_WATER_FLOW))
        {
            return err_code;
        }

        eeprom_tripple_write(i + reg_addr, ee_wr_data, ee_rd_data); //数据写入EEPROM
        //event_Recrd
        add_eventlog_fifo(i + reg_addr, (User_ID << 8) | g_sys.status.general.permission_level, ee_rd_data, ee_wr_data);
        //	rt_kprintf("reg_map_write I2C_EE_BufRead ADDR \n");
    }
    return err_code;
}
/**
  * @brief  read register map.
  * @param  reg_addr: reg map address.
	* @param  *rd_data: read data buffer ptr.
  * @retval 
  *   This parameter can be one of the following values:
  *     @arg 1: write operation success
  *     @arg 0: read operation fail
  */
uint16 reg_map_read(uint16 reg_addr, uint16 *reg_data, uint8_t read_cnt)
{
    uint16_t i;
    uint16_t err_code;
    extern USHORT cpad_usSRegHoldBuf[CMD_REG_SIZE];
    err_code = CPAD_ERR_NOERR;
    if ((reg_addr & 0x8000) != 0)
    {
        reg_addr &= 0x7fff;
        if (reg_addr > STATUS_REG_MAP_NUM) //address out of range
        {
            err_code = CPAD_ERR_ADDR_OR;
        }
        else
        {
            for (i = 0; i < read_cnt; i++)
            {
                *(reg_data + i) = *(status_reg_map_inst[reg_addr + i].reg_ptr); //read data from designated register
            }
        }
    }
    else if ((reg_addr & 0x4000) != 0)
    {
        reg_addr &= 0x3fff;
        if (reg_addr > CMD_REG_SIZE) //address out of range
        {
            err_code = CPAD_ERR_ADDR_OR;
        }
        else
        {
            for (i = 0; i < read_cnt; i++)
            {
                *(reg_data + i) = cpad_usSRegHoldBuf[reg_addr + i]; //read data from designated register
            }
        }
    }
    else
    {
        reg_addr = reg_addr;
        if (reg_addr > CONF_REG_MAP_NUM) //address out of range
        {
            err_code = CPAD_ERR_ADDR_OR;
        }
        else
        {
            for (i = 0; i < read_cnt; i++)
            {
                *(reg_data + i) = *(conf_reg_map_inst[reg_addr + i].reg_ptr); //read data from designated register
            }
        }
    }
    return err_code;
}

/**
  * @brief  show register map content.
  * @param  reg_addr: reg map address.
	* @param  *rd_data: register read count.
  * @retval none
  */
static void show_reg_map(uint16_t reg_addr, uint16_t reg_cnt)
{
    uint16_t reg_buf[32];
    uint16_t i;
    reg_map_read(reg_addr, reg_buf, reg_cnt);
    rt_kprintf("Reg map info:\n");
    for (i = 0; i < reg_cnt; i++)
    {
        rt_kprintf("addr:%d;data:%x\n", (reg_addr + i) & 0x3fff, reg_buf[i]);
    }
}

uint16_t write_reg_map(uint16_t reg_addr, uint16_t data)
{
    uint16_t ret;
    ret = reg_map_write(reg_addr, &data, 1, USER_DEFAULT);
    return ret;
}

//内存修改E2
uint16_t RAM_Write_Reg(uint16_t reg_addr, uint16_t data, uint8_t u8Num)
{
    uint16_t ret;
    ret = reg_map_write(reg_addr, &data, u8Num, USER_DEFAULT);
    return ret;
}

static void read_eeprom(uint16_t offset, uint16_t rd_cnt)
{
    uint8_t rd_reg[32];
    uint16_t i;
    I2C_EE_BufRead((uint8_t *)rd_reg, offset, rd_cnt);
    for (i = 0; i < rd_cnt; i++)
    {
        rt_kprintf("addr:%d,data:%x\n", i + offset, rd_reg[i]);
    }
}

//清零运行时间
uint8_t reset_runtime(uint16_t param)
{
    uint8_t i, req;

    req = 1;
    if (param == 0xff)
    {
        for (i = 0; i < DO_MAX_CNT; i++)
        {
            g_sys.status.ComSta.u16Runtime[0][i] = 0;
            g_sys.status.ComSta.u16Runtime[1][i] = 0;
        }
        g_sys.status.ComSta.u16Cumulative_Water[0] = 0;
        memset((uint8_t *)&g_sys.status.ComSta.u16Cumulative_Water[0], 0x00, 4);
        I2C_EE_BufWrite((uint8_t *)&g_sys.status.ComSta.u16Cumulative_Water[0], STS_REG_EE1_ADDR + sizeof(g_sys.status.ComSta.u16Runtime), 4); //u16Cumulative_Water,
    }
    else
    {
        if ((param > 0) && (param <= DO_MAX_CNT))
        {
            g_sys.status.ComSta.u16Runtime[0][param - 1] = 0;
            g_sys.status.ComSta.u16Runtime[1][param - 1] = 0;
        }
        else
        {
            req = 0;
        }
    }
    if (req == 1)
    {
        I2C_EE_BufWrite((uint8_t *)&g_sys.status.ComSta.u16Runtime, STS_REG_EE1_ADDR, sizeof(g_sys.status.ComSta.u16Runtime));
    }

    return (req);
}

uint8_t load_factory_pram(void)
{
    uint8_t req;
    req = 0;
    req = init_load_factory_conf();
    authen_init();
    save_conf_reg(0);
    set_load_flag(0);
    rt_thread_delay(1000);
    NVIC_SystemReset();
    return (req);
}

//void write_passward1 (uint16_t work_mode)
//{
//
//		uint8_t pass[4]={0x12,0x34,0x56,0x78};
//
//		write_passward(pass,work_mode,2);
//
//		I2C_EE_BufRead((uint8_t*)&g_sys.status.sys_work_mode.work_mode,WORK_MODE_EE_ADDR,sizeof(work_mode_st));
//
//		rt_kprintf("g_sys.status.sys_work_mode.work_mode = %d\n",g_sys.status.sys_work_mode.work_mode);
//		rt_kprintf("g_sys.status.sys_work_mode.limit_time = %d\n",g_sys.status.sys_work_mode.limit_day);
//		rt_kprintf("g_sys.status.sys_work_mode.runing_time = %d\n",g_sys.status.sys_work_mode.runing_day);
//		rt_kprintf("g_sys.status.sys_work_mode.runing_hour = %d\n",g_sys.status.sys_work_mode.runing_hour);
//		rt_kprintf("g_sys.status.sys_work_mode.runing_sec = %d\n",g_sys.status.sys_work_mode.runing_sec);
//		rt_kprintf("g_sys.status.sys_work_mode.pass_word[0] = %d\n",g_sys.status.sys_work_mode.pass_word[0]);
//		rt_kprintf("g_sys.status.sys_work_mode.pass_word[1] = %d\n",g_sys.status.sys_work_mode.pass_word[1]);
//		rt_kprintf("g_sys.status.sys_work_mode.pass_word[2] = %d\n",g_sys.status.sys_work_mode.pass_word[2]);
//		rt_kprintf("g_sys.status.sys_work_mode.pass_word[3] = %d\n",g_sys.status.sys_work_mode.pass_word[3]);
//
//}

//void cpad_work_mode1(uint16_t work_mode)
//{
//
//		//cpad_work_mode(work_mode,2);
//		rt_kprintf("g_sys.status.sys_work_mode.work_mode = %d\n",g_sys.status.sys_work_mode.work_mode);
//		rt_kprintf("g_sys.status.sys_work_mode.limit_time = %d\n",g_sys.status.sys_work_mode.limit_day);
//		rt_kprintf("g_sys.status.sys_work_mode.runing_time = %d\n",g_sys.status.sys_work_mode.runing_day);
//		rt_kprintf("g_sys.status.sys_work_mode.runing_hour = %d\n",g_sys.status.sys_work_mode.runing_hour);
//		rt_kprintf("g_sys.status.sys_work_mode.runing_sec = %d\n",g_sys.status.sys_work_mode.runing_sec);
//		rt_kprintf("g_sys.status.sys_work_mode.pass_word[0] = %d\n",g_sys.status.sys_work_mode.pass_word[0]);
//		rt_kprintf("g_sys.status.sys_work_mode.pass_word[1] = %d\n",g_sys.status.sys_work_mode.pass_word[1]);
//		rt_kprintf("g_sys.status.sys_work_mode.pass_word[2] = %d\n",g_sys.status.sys_work_mode.pass_word[2]);
//				rt_kprintf("g_sys.status.sys_work_mode.pass_word[3] = %d\n",g_sys.status.sys_work_mode.pass_word[3]);
//}

//void test_fsm(void)
//{
////		I2C_EE_BufRead((uint8_t*)&g_sys.status.sys_work_mode.work_mode,WORK_MODE_EE_ADDR,sizeof(work_mode_st));
//		rt_kprintf("g_sys.status.sys_work_mode.work_mode = %d\n",g_sys.status.sys_work_mode.work_mode);
//		rt_kprintf("g_sys.status.sys_work_mode.limit_time = %d\n",g_sys.status.sys_work_mode.limit_day);
//		rt_kprintf("g_sys.status.sys_work_mode.runing_time = %d\n",g_sys.status.sys_work_mode.runing_day);
//		rt_kprintf("g_sys.status.sys_work_mode.runing_hour = %d\n",g_sys.status.sys_work_mode.runing_hour);
//		rt_kprintf("g_sys.status.sys_work_mode.runing_sec = %d\n",g_sys.status.sys_work_mode.runing_sec);
//		rt_kprintf("g_sys.status.sys_work_mode.pass_word[0] = %d\n",g_sys.status.sys_work_mode.pass_word[0]);
//		rt_kprintf("g_sys.status.sys_work_mode.pass_word[1] = %d\n",g_sys.status.sys_work_mode.pass_word[1]);
//		rt_kprintf("g_sys.status.sys_work_mode.pass_word[2] = %d\n",g_sys.status.sys_work_mode.pass_word[2]);
//			rt_kprintf("g_sys.status.sys_work_mode.pass_word[3] = %d\n",g_sys.status.sys_work_mode.pass_word[3]);
//}

static void sys_dbg(uint16_t flag)
{
    if (flag == 1)
    {
        l_sys.debug_flag = DEBUG_ON_FLAG;
        l_sys.debug_tiemout = DEBUG_TIMEOUT_MAX;
    }
    else if (flag == 223)
    {
        l_sys.debug_flag = DEBUG_ON_FLAG;
        l_sys.debug_tiemout = DEBUG_TIMEOUT_NA;
    }
    else
    {
        l_sys.debug_flag = DEBUG_OFF_FLAG;
        l_sys.debug_tiemout = 0;
    }
}

void eeprom_addr(void)
{
    rt_kprintf("user1 conf  start =%d ,end =%d ,size = %d\n", CONF_REG_EE1_ADDR, CONF_REG_EE2_ADDR, (CONF_REG_EE2_ADDR - CONF_REG_EE1_ADDR));
    rt_kprintf("user2 conf  start =%d ,end =%d ,size = %d\n", CONF_REG_EE2_ADDR, CONF_REG_EE3_ADDR, (CONF_REG_EE3_ADDR - CONF_REG_EE2_ADDR));
    rt_kprintf("user3 conf  start =%d ,end =%d ,size = %d\n", CONF_REG_EE3_ADDR, CONF_REG_FACT_ADDR, (CONF_REG_FACT_ADDR - CONF_REG_EE3_ADDR));
    rt_kprintf("fact  conf  start =%d ,end =%d ,size = %d\n", CONF_REG_FACT_ADDR, STS_REG_EE1_ADDR, (STS_REG_EE1_ADDR - CONF_REG_FACT_ADDR));
    rt_kprintf("status reg1 start =%d ,end =%d ,size = %d\n", STS_REG_EE1_ADDR, STS_REG_EE2_ADDR, (STS_REG_EE2_ADDR - STS_REG_EE1_ADDR));
    rt_kprintf("status reg2 start =%d ,end =%d ,size = %d\n", STS_REG_EE2_ADDR, WORK_MODE_EE_ADDR, (WORK_MODE_EE_ADDR - STS_REG_EE2_ADDR));
    rt_kprintf("work_mode   start =%d ,end =%d ,size = %d\n", WORK_MODE_EE_ADDR, AlARM_TABLE_ADDR, (AlARM_TABLE_ADDR - WORK_MODE_EE_ADDR));
    rt_kprintf("alarm_table start =%d ,end =%d ,size = %d\n", AlARM_TABLE_ADDR, TEM_HUM_PT_ADDR, (TEM_HUM_PT_ADDR - AlARM_TABLE_ADDR));
    rt_kprintf("TEM_HUM_REC start =%d ,end =%d ,size = %d\n", TEM_HUM_PT_ADDR, ALARM_REC_PT_ADDR, (ALARM_REC_PT_ADDR - TEM_HUM_PT_ADDR));
    rt_kprintf("ALARM_REC   start =%d ,end =%d ,size = %d\n", ALARM_REC_PT_ADDR, EVENT_REC_PT_ADDR, (EVENT_REC_PT_ADDR - ALARM_REC_PT_ADDR));
    rt_kprintf("EVENT_REC   start =%d ,end =%d ,size = %d\n", EVENT_REC_PT_ADDR, EE_REC_END, (EE_REC_END - EVENT_REC_PT_ADDR));
}
FINSH_FUNCTION_EXPORT(eeprom_addr, show_ee_addr_table);
FINSH_FUNCTION_EXPORT(sys_dbg, system debug switchs.);
//FINSH_FUNCTION_EXPORT(test_fsm, test_test_fasm.);
//FINSH_FUNCTION_EXPORT(write_passward1, test_test_cpad_work_mode.);
//FINSH_FUNCTION_EXPORT(cpad_work_mode1, test_test_write_passward.);
FINSH_FUNCTION_EXPORT(reset_runtime, reset run_time eeprom &regs.);
FINSH_FUNCTION_EXPORT(show_reg_map, show registers map.);
FINSH_FUNCTION_EXPORT(write_reg_map, write data into conf registers.);
FINSH_FUNCTION_EXPORT(set_load_flag, set sys init load option.);
FINSH_FUNCTION_EXPORT(save_conf_reg, save current conf reg data.);
FINSH_FUNCTION_EXPORT(read_eeprom, read eeprom content eeprom flag.);
