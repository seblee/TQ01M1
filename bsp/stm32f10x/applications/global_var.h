#ifndef __GLOBAL_VAR
#define __GLOBAL_VAR
#include "sys_conf.h"

#define MB_N_OFFSET 100  //Modbus偏移
#define T5_OFFSET 0x1100 //T5屏寄存器偏移

//特殊地址
#define POWER_ON_ADDR 36u
#define WATER_MODE_ADDR 46u
#define WATER_FLOW_ADDR 47u
#define WATERTANK_ADDR 51u

#define FACTORY_RESET 56u
#define MANUAL_TSET 58u
#define CLEAR_RT 66u
#define CLEAR_ALARM 67u
#define SET_TL 68u //系统时间低位
#define SET_TH 69u

#define EE_WATER_MODE 46u
#define EE_WATER_FLOW 47u
#define EE_WIFI_SET 313u
#define EE_WIFI_PASSWORD 325u

#define ST_HARDWARE 0 + 106

#define SETTIME_DELAY 10

uint16 reg_map_write(uint16 reg_addr, uint16 *wr_data, uint8_t wr_cnt, uint16 User_ID);
uint16 reg_map_read(uint16 reg_addr, uint16 *reg_data, uint8_t read_cnt);
uint16_t save_conf_reg(uint8_t addr_sel);
uint16_t set_load_flag(uint8_t ee_load_flag);
uint16_t sys_global_var_init(void);
uint16_t sys_local_var_init(void);
int16_t eeprom_tripple_write(uint16 reg_offset_addr, uint16 wr_data, uint16_t rd_data);

int16_t eeprom_compare_read(uint16 reg_offset_addr, uint16_t *rd_data);

uint8_t reset_runtime(uint16_t param);
uint8_t load_factory_pram(void);
uint16_t write_reg_map(uint16_t reg_addr, uint16_t data);
uint16_t RAM_Write_Reg(uint16_t reg_addr, uint16_t data, uint8_t u8Num);

#endif //__GLOBAL_VAR
