/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbport.h,v 1.17 2006/12/07 22:10:34 wolti Exp $
 *            mbport.h,v 1.60 2013/08/17 11:42:56 Armink Add Master Functions  $
 */

#ifndef MNT_MB_PORT_H
#define MNT_MB_PORT_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif
#include "port.h"
#define MNT_RX_LEN 512
#define MNT_RX_MIN 4
#define MNT_CMD_LEN 8
typedef enum
{
    MB_PAR_NONE, /*!< No parity. */
    MB_PAR_ODD,  /*!< Odd parity. */
    MB_PAR_EVEN  /*!< Even parity. */
} eMBParity;

//enum
//{
//		REC_ADDR_STATE =0,
//		REC_DATA_STATE
//};

enum
{
    REC_ADDR_STATE = 0,
    REC_HEAD_STATE,
    REC_LEN_STATE,
    REC_DATA_STATE
};

typedef struct
{
    uint8_t rec_state;
    uint8_t rxbuf[MNT_RX_LEN];
    uint8_t rx_flag;
    uint8_t rx_ok;
    uint16_t rec_cnt;
    uint16_t addr;
    uint8_t rx_timeout;
    uint8_t update_timer_flag;
    uint8_t rx_DataCnt;
} cpad_slave_st;

/* ----------------------- Serial port functions ----------------------------*/

void cpad_MBPortSerialInit(UCHAR ucPort, ULONG ulBaudRate,
                           UCHAR ucDataBits, eMBParity eParity);

void MonitorvMBPortClose(void);

void MonitorxMBPortSerialClose(void);

void MonitorvMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable);
void cpad_xMBPortSerialPutByte(uint8_t *ucbyte, uint16_t len);

/* ----------------------- Timers functions ---------------------------------*/
void cpad_xMBPortTimersInit(USHORT usTimeOut50us);

void cpad_xMBPortTimersClose(void);

void cpad_vMBPortTimersEnable(void);
void cpad_vMBPortTimersDisable(void);

/* ----------------------- Callback for the protocol stack ------------------*/


#ifdef __cplusplus
PR_END_EXTERN_C
#endif

#endif
