#ifndef _REG_MAP_CHECK_H
#define _REG_MAP_CHECK_H

#include "sys_conf.h"
uint8_t comp_uper_spd_chk(uint16_t pram);
uint8_t comp_low_spd_chk(uint16_t pram);
uint8_t water_valve_set_chk(uint16_t param);
uint8_t water_valve_min_chk(uint16_t param);
uint8_t water_valve_max_chk(uint16_t param);
uint8_t fan_set_spd_chk(uint16_t pram);
uint8_t fan_set_spd_chk(uint16_t pram);
uint8_t fan_low_spd_chk(uint16_t pram);
uint8_t fan_dehum_min_spd_chk(uint16_t pram);
uint8_t fan_uper_spd_chk(uint16_t pram);
uint8_t team_total_num_chk(uint16_t pram);
uint8_t team_back_num_chk(uint16_t pram);
uint8_t return_temp_hiacl_chk(uint16_t pram);
uint8_t return_temp_lowacl_chk(uint16_t pram);
uint8_t return_hum_hiacl_chk(uint16_t pram);
uint8_t return_hum_lowacl_chk(uint16_t pram);
uint8_t supply_temp_hiacl_chk(uint16_t pram);
uint8_t supply_temp_lowacl_chk(uint16_t pram);
uint8_t supply_hum_hiacl_chk(uint16_t pram);
uint8_t supply_hum_lowacl_chk(uint16_t pram);
uint8_t water_elec_hi_chk(uint16_t pram);
uint8_t water_elec_lo_chk(uint16_t pram);
uint8_t power_frq_hi_chk(uint16_t pram);
uint8_t power_frq_lo_chk(uint16_t pram);
uint8_t power_a_hi_chk(uint16_t pram);
uint8_t power_a_lo_chk(uint16_t pram);
uint8_t power_b_hi_chk(uint16_t pram);
uint8_t power_b_lo_chk(uint16_t pram);
uint8_t power_c_hi_chk(uint16_t pram);
uint8_t power_c_lo_chk(uint16_t pram);
uint8_t cool0_temp_hi_chk(uint16_t pram);
uint8_t cool0_temp_lo_chk(uint16_t pram);
uint8_t cool1_temp_hi_chk(uint16_t pram);
uint8_t cool1_temp_lo_chk(uint16_t pram);

#endif
