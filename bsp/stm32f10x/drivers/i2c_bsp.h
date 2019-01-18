#ifndef _IIC_
#define _IIC_

#ifdef _IIC_H
#define _IIC_Def_
#else
#define _IIC_Def_ extern
#endif

#include "stdint.h"
#include "sys_conf.h"
#include "sys_def.h"

extern sys_reg_st g_sys;
#define ALARM_MAX_CNT 500 //报警类记录总条数
#define HUM_TEMP_MAX_CNT 48

#define STS_EE_ADDR 0x0000
#define USR_PW_EE_BASE 0x0100

#define CONF_REG_EE1_ADDR 0x0200

#define CONF_REG_EE2_ADDR (CONF_REG_EE1_ADDR + 2 * (CONF_REG_MAP_NUM + 1))  // CONF_REG_MAP_NUM+1 //Reg+check
#define CONF_REG_EE3_ADDR (CONF_REG_EE2_ADDR + 2 * (CONF_REG_MAP_NUM + 1))  // CONF_REG_MAP_NUM+1 //Reg+check
#define CONF_REG_FACT_ADDR (CONF_REG_EE3_ADDR + 2 * (CONF_REG_MAP_NUM + 1)) // CONF_REG_MAP_NUM+1 //Reg+check
#define STS_REG_EE1_ADDR (CONF_REG_FACT_ADDR + 2 * (CONF_REG_MAP_NUM + 1))  // CONF_REG_MAP_NUM+1 //Reg+check

#define STS_REG_EE2_ADDR (STS_REG_EE1_ADDR + 100)

#define WORK_MODE_EE_ADDR (STS_REG_EE2_ADDR + 4) //team_bitmap[TEAM_BITMAP_FINAL_OUT]

#define CONTROLPASSWORD_EE_ADDR (WORK_MODE_EE_ADDR + sizeof(work_mode_st))

#define AlARM_TABLE_ADDR (CONTROLPASSWORD_EE_ADDR + sizeof(ControlPassword_st))

#define TEM_HUM_PT_ADDR (AlARM_TABLE_ADDR + sizeof(alarm_table_st) * ACL_TOTAL_NUM)

#define ALARM_REC_PT_ADDR (TEM_HUM_PT_ADDR + (4 * 60 * HUM_TEMP_MAX_CNT) + sizeof(record_pt_st))

//操作类事件记录 500条
#define EVENT_REC_PT_ADDR (ALARM_REC_PT_ADDR + 12 * ALARM_MAX_CNT + sizeof(record_pt_st))
#define EE_REC_END (EVENT_REC_PT_ADDR + sizeof(event_log_st) * EVE_MAX_CNT + sizeof(record_pt_st))

//驱动底层
#define SLAVE_ADDR 0xae
#define I2C2_EE_PageSize 64

#define II_SCL_Pin GPIO_Pin_10
#define II_SCL_GPIO GPIOB

#define II_SDA_Pin GPIO_Pin_11
#define II_SDA_GPIO GPIOB

#define II_WP_Pin GPIO_Pin_15
#define II_WP_GPIO GPIOE

#define READ_SDA() GPIO_ReadInputDataBit(II_SDA_GPIO, II_SDA_Pin)

#define WP_Enable() GPIO_ResetBits(II_WP_GPIO, II_WP_Pin)

#define WP_Diable() GPIO_SetBits(II_WP_GPIO, II_WP_Pin)

//IIC?ùóD2ù×÷oˉêy

enum
{
    EEPROM_NOERRO = 0,
    EEPROM_BUSSERRO,
    EEPROM_MUXERRO,
};

void drv_i2c_init(void);
int8_t I2C_EE_BufWrite(uint8_t *write_buffer, uint16_t write_addr, uint16_t num_byte_write);
//int8_t I2C_EE_PageWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
int8_t I2C_EE_BufRead(uint8_t *read_buffer, uint16_t read_addr, uint16_t num_byte_read);
//void I2C_EE_WaitEepromStandbyState(void);

#endif
