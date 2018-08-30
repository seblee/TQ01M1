#ifndef __I2C_EE_H
#define	__I2C_EE_H

#include "stm32f10x.h"
#include <rtthread.h>
/* 
 * AT24C256 264kb = 262,144bit = 262,144/8 B = 32768 B
 * 512 pages of 64 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */

 /* STM32 I2C 快速模式 */
#define I2C2_Speed              100000

/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2C2_EE_ADDRESS	  	   0xa0   

/* AT24C01/02每页有8个字节 */
#define I2C2_EE_PageSize       64

#define I2C2_EE_TIMEOUT				 0x10000
 
 
#define  EEP_Firstpage      0x00
/* EEPROM Addresses defines */
#define EEPROM_Block0_ADDRESS 0xae   /* E2 = 0 */

// #define EEPROM_Block1_ADDRESS 0xA2 /* E2 = 0 */
// #define EEPROM_Block2_ADDRESS 0xA4 /* E2 = 0 */
// #define EEPROM_Block3_ADDRESS 0xA6 /* E2 = 0 */

void drv_i2c_init(void);
int8_t I2C_EE_BufWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite);
int8_t I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr);
int8_t I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite);
int8_t I2C_EE_BufRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);
void I2C_EE_WaitEepromStandbyState(void);

#endif /* __I2C_EE_H */
