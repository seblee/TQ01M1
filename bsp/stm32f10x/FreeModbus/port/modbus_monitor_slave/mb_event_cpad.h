

#ifndef MNT_MB_EVENT_H
#define MNT_MB_EVENT_H

#include "port.h"

#include "mbport_cpad.h"

#include "sys_def.h"
typedef enum
{
    MB_ENOERR,    /*!< no error. */
    MB_ENOREG,    /*!< illegal register address. */
    MB_EINVAL,    /*!< illegal argument. */
    MB_EPORTERR,  /*!< porting layer error. */
    MB_ENORES,    /*!< insufficient resources. */
    MB_EIO,       /*!< I/O error. */
    MB_EILLSTATE, /*!< protocol stack in illegal state. */
    MB_ETIMEDOUT  /*!< timeout error occurred. */
} eMBErrorCode;
/* -----------------------Slave Defines -------------------------------------*/

#define MCPAD_REG_HOLDING_WRITE_NREGS 5 //????+

#define CPAD_MB_REG_READ 0x03
#define CPAD_MB_REG_SINGLE_WRITE 0x06
#define CPAD_MB_REG_MULTIPLE_WRITE 0x10

#define RESPONSE_REG_SIZE 300
#define CMD_REG_SIZE 64

#define CMD_REG_MAP_OFFSET 0
#define CONFIG_REG_MAP_OFFSET 64

#ifdef SYS_HMI_VJL
#define STATUS_REG_MAP_OFFSET 170
#else
#define STATUS_REG_MAP_OFFSET 500
#endif
////#define STATUS_REG_MAP_OFFSET  170
//#define STATUS_REG_MAP_OFFSET  500

/* -----------------------Slave Defines -------------------------------------*/
#define CPAD_S_REG_HOLDING_START 0
//#define 				 CPAD_S_REG_HOLDING_NREGS           (64+106+40)//CMD
#define CPAD_S_REG_HOLDING_NREGS (STATUS_REG_MAP_OFFSET + 300) //CMD
#define CPAD_REG_HOLDING_WRITE_NREGS STATUS_REG_MAP_OFFSET + 1 //可写范围

#define T5_HEAD 0x5AA5
#define T5_WRITE 0x82
#define T5_READ 0x83
#define T5_HEAD_LEN 3

void cpad_MBRTUInit(UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity);

void cpad_MBPoll(void);
eMBErrorCode cpad_eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, uint8_t eMode);

void Cpad_Update(void);
void Cpad_Send(uint8_t u8RW, uint16_t u16Addr, uint8_t u8Num, uint16_t u16Offset);
#endif
