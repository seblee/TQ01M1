#ifndef __REQ_CALC_H__
#define __REQ_CALC_H__
#include "sys_conf.h"

#define TEMP_MAX  (1000)
#define TEMP_MIN  (-400)
#define HUM_MAX   (1000)
#define HUM_MIN   (0)

#define TOTAL_REQ 1
#define AVER_REQ  2

void inc_pid_param_init(void);
void req_update(void);
int16_t team_hum_req_calc(uint8_t type);
int16_t team_hum_average_req_calc(void);
int16_t team_temp_req_calc(uint8_t type);
int16_t team_temp_average_req_calc(void);
int16_t team_fan_req_calc(void);
void team_table_req_update(void);
int16_t team_config_temp_req_calc(void);
int16_t team_config_hum_req_calc(void);
int16_t team_config_fan_req_calc(void);

#endif //__REQ_CALC_H__
