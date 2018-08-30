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
 * File: $Id: mbrtu.h,v 1.9  2006/12/07 22:10:34 wolti Exp $
 * File: $Id: mbrtu.h,v 1.60 2013/08/17 13:11:42 Armink Add Master Functions $
 */
#include "mbconfig.h"

#ifndef _MB_RTU_H
#define _MB_RTU_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

eMBErrorCode    MonitoreMBRTUInit( UCHAR slaveAddress, UCHAR ucPort, ULONG ulBaudRate,
						    eMBParity eParity );
void            MonitoreMBRTUStart( void );
void            MonitoreMBRTUStop( void );
eMBErrorCode    MonitoreMBRTUReceive( UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength );
eMBErrorCode    MonitoreMBRTUSend( UCHAR slaveAddress, const UCHAR * pucFrame, USHORT usLength );
BOOL            MonitorxMBRTUReceiveFSM( void );
BOOL            MonitorxMBRTUTransmitFSM( void );
BOOL            MonitorxMBRTUTimerT15Expired( void );
BOOL            MonitorxMBRTUTimerT35Expired( void );

#if MB_MASTER_RTU_ENABLED > 0
eMBErrorCode    MonitoreMBMasterRTUInit( UCHAR ucPort, ULONG ulBaudRate,eMBParity eParity );
void            MonitoreMBMasterRTUStart( void );
void            MonitoreMBMasterRTUStop( void );
eMBErrorCode    MonitoreMBMasterRTUReceive( UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength );
eMBErrorCode    MonitoreMBMasterRTUSend( UCHAR slaveAddress, const UCHAR * pucFrame, USHORT usLength );
BOOL            MonitorxMBMasterRTUReceiveFSM( void );
BOOL            MonitorxMBMasterRTUTransmitFSM( void );
BOOL            MonitorxMBMasterRTUTimerExpired( void );
#endif

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
