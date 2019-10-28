#ifndef __SYS_DEF
#define __SYS_DEF
// Define   NULL   pointer
#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif //   NULL

#include <stdint.h>

typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned long uint32;
typedef long int32;
typedef uint32_t time_t;

#define SYS_DEBUG 1

#define DEBUG_TIMEOUT_MAX 5000 //上电DEBUG_TIMEOUT_MAX时间后关闭调试串口
#define DEBUG_TIMEOUT_NA 0xffff
#define DEBUG_ON_FLAG 0
#define DEBUG_OFF_FLAG 123

#define SYS_HMI_TQ 1 //天泉
//#define	SYS_HMI_TQ_T10		1			//T10小机
//#define	SYS_HMI_VJL		1		//威金利

#ifdef SYS_HMI_TQ
#define CONF_REG_MAP_NUM 435
#define CONF_REG_SID_NUM 100
#define CONF_REG_SID_START CONF_REG_MAP_NUM - CONF_REG_SID_NUM
#define VER_0 (uint16)3 //V2100 0xf000
#define VER_1 (uint16)0 //V2100 0x0f80
#define VER_2 (uint16)2 //V2100 0x007f
#define SOFTWARE_VER (((VER_0 & 0x0f) << 12) | ((VER_1 & 0x001f) << 7) | (VER_2 & 0x007f))
#define SOFTWARE_VER_NAME "TQ01M1"

#elif SYS_HMI_TQ_T10
#define CONF_REG_MAP_NUM 300
#define CONF_REG_SID_START 200
#define CONF_REG_SID_NUM 100
#define SOFTWARE_VER 0x1104

#elif SYS_HMI_VJL
#define CONF_REG_MAP_NUM 106
#define SOFTWARE_VER 0x110C
#endif

#define STATUS_REG_MAP_NUM 40
#define HARDWARE_VER 0x1000
#define SERIAL_NO_3 0
#define SERIAL_NO_2 0
#define SERIAL_NO_1 0
#define SERIAL_NO_0 0
#define MAN_DATA_1 0
#define MAN_DATA_0 0
#define PERM_PRIVILEGED 1
#define PERM_INSPECT 0

//base year
#define BASE_YEAR 2000
//main components
#define MAX_COMPRESSOR_NUM 2
#define MAX_FAN_NUM 3
#define MAX_HEATER_NUM 2
//accesories
#define TEMP_HUM_SENSOR_NUM 8
#define MBM_DEV_P_NUM 2
#define NTC_NUM 4
#define CURRENT_SENSE_CHAN 3
#define VOLTAGE_SENSE_CHAN 3
//alarm defs
#define MAX_ALARM_ACL_NUM 50
#define MAX_ALARM_HISTORY_NUM 200
#define MAX_ALARM_STATUS_NUM 50
//sys log defs
#define MAX_SYS_STATUS_LOG_NUM 100
#define MAX_SYS_RUMTIME_LOG_NUM 100

//cpad err code
#define CPAD_ERR_NOERR 0
#define CPAD_ERR_ADDR_OR 1
#define CPAD_ERR_DATA_OR 2
#define CPAD_ERR_PERM_OR 3
#define CPAD_ERR_WR_OR 4
#define CPAD_ERR_CONFLICT_OR 5

#define CPAD_ERR_UNKNOWN 0x1f

#endif //__SYS_DEF
