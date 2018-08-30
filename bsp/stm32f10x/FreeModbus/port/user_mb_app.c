/*
 * FreeModbus Libary: user callback functions and buffer define in slave mode
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: user_mb_app.c,v 1.60 2013/11/23 11:49:05 Armink $
 */
#include "user_mb_app.h"

#include "global_var.h"
#include "event_record.h"
/*------------------------Slave mode use these variables----------------------*/
//Slave mode:HoldingRegister variables



	


USHORT   usSRegHoldStart                              = S_REG_HOLDING_START;
USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS]           ;
typedef struct 
{
	uint8_t reg_type; //0=config_reg;1 =status_reg;
	uint16_t reg_addr;//地址
	uint8_t reg_w_r; //3 =write&read,2=read_only,3=write_only
}reg_table_st;

typedef struct
{
	uint16_t *reg_ptr;
}mbm_reg_table_st;
static uint16_t mbm_cmd_reg =0;
const mbm_reg_table_st mbm_reg_table[]=
{
	{&mbm_cmd_reg}
};	
//内存到modbus的映射表。
//元素位置对应ModeBus  协议栈中usSRegHoldBuf位置
//元素值对应conf_reg_map_inst，内存数据的位置。

//监控通信寄存器S_REG_HOLDING_NREGS=70
const reg_table_st modbus_slave_reg_table[]={
{0,36},//开关机0
{0,37},//  1
{0,38},//  2
{0,39},//
{0,40},//
{0,41},//
{0,42},//
{0,43},//
{0,44},//
{0,45},//

{0,46},//
{0,47},//
{0,48},//
{0,49},//
{0,50},//
{0,51},//
{0,52},//
{0,53},//
{0,54},//
{0,55},//

{0,56},//
{0,57},//
{0,58},//
{0,59},//
{0,60},//
{0,61},//
{0,62},//
{0,63},//
{0,64},//
{0,65},//

{0,66},//
{0,67},//
{0,68},//
{0,69},//
{0,70},//
{0,71},//
{0,72},//
{0,73},//
{0,74},//
{0,75},//

{0,76},//
{0,77},//
{0,78},//
{0,79},//
{0,80},//
{0,81},//
{0,82},//
{0,83},//
{0,84},//
{0,85},//

{0,86},//
{0,87},//
{0,88},//
{0,89},//
{0,90},//
{0,91},//
{0,92},//
{0,93},//
{0,94},//
{0,95},//

{0,96},//
{0,97},//
{0,98},//
{0,99},//
{0,100},//
{0,101},//
{0,102},//
{0,103},//
{0,104},//
{0,105},//

{1,0},//
{1,1},//
{1,2},//
{1,3},//
{1,4},//
{1,5},//
{1,6},//
{1,7},//
{1,8},//
{1,9},//

{1,10},//
{1,11},//
{1,12},//
{1,13},//
{1,14},//
{1,15},//
{1,16},//
{1,17},//
{1,18},//
{1,19},//

{1,20},//
{1,21},//
{1,22},//
{1,23},//
{1,24},//
{1,25},//
{1,26},//
{1,27},//
{1,28},//
{1,29},//

{1,30},//
{1,31},//
{1,32},//
{1,33},//
{1,34},//
{1,35},//
{1,36},//
{1,37},//
{1,38},//
{1,39},//
};

enum
{
  BAUD_4800 = 0,
	BAUD_9600,
	BAUD_19200,
	BAUD_38400,
	BAUD_57600,
	BAUD_115200,
};


//typedef struct
//{
//		uint16_t baudrate;
//		uint16_t com_addr;
//}communication_change_st;
//static communication_change_st com_change_inst;

//static ULONG Get_Baudrate(uint16_t 	baudrate)
//{
//		ULONG ulBaudRate;	
//		switch(com_change_inst.baudrate )
//		{
//			
//			case BAUD_4800:
//					ulBaudRate = 4800;
//					break;
//			case BAUD_9600:
//					ulBaudRate = 9600;
//					break;
//			case BAUD_19200:
//					ulBaudRate = 19200;
//					break;
//			case BAUD_38400:
//					ulBaudRate = 38400;
//					break;
//			case BAUD_57600:
//					ulBaudRate = 57600;
//					break;
//			case BAUD_115200:
//					ulBaudRate = 115200;
//					break;
//			default:
//					ulBaudRate = 9600;
//				break;
//		}	
//		return ulBaudRate;
//}
//static void change_surv_baudrate(void)
//{
//	extern sys_reg_st					g_sys; 

//	if((com_change_inst.baudrate != g_sys.config.general.surv_baudrate)||(g_sys.config.general.surv_addr != com_change_inst.com_addr ))
//	{
//			com_change_inst.baudrate  =  g_sys.config.general.surv_baudrate;
//			com_change_inst.com_addr = g_sys.config.general.surv_addr;
//			eMBInit(MB_RTU,(UCHAR)g_sys.config.general.surv_addr, UPORT_MONITOR, Get_Baudrate(com_change_inst.baudrate),  MB_PAR_NONE);
//			eMBEnable();
//	}
//}
void modbus_slave_thread_entry(void* parameter)
{
		extern sys_reg_st		g_sys; 
		eMBErrorCode    eStatus = MB_ENOERR;
	
		rt_thread_delay(MODBUS_SLAVE_THREAD_DELAY);
//		com_change_inst.baudrate  =  g_sys.config.general.surv_baudrate;
//		com_change_inst.com_addr = g_sys.config.general.surv_addr;
//		eStatus = eMBInit(MB_RTU,(UCHAR)g_sys.config.general.surv_addr, UPORT_MONITOR, Get_Baudrate(com_change_inst.baudrate),  MB_PAR_NONE);
		eStatus = eMBInit(MB_RTU,1, UPORT_SLAVE, 9600,  MB_PAR_NONE);
		if(eStatus != MB_ENOERR)
		{
				rt_kprintf("MB_SLAVE init failed\n");
		}
		eStatus = eMBEnable();			
		if(eStatus != MB_ENOERR)
		{
				rt_kprintf("MB_SLAVE enable failed\n");	
		}
		while(1)
		{
				eStatus = eMBPoll();
				if(eStatus != MB_ENOERR)
				{
						rt_kprintf("MB_SLAVE enable failed\n");	
				}	
				mbs_sts_update();	
//				change_surv_baudrate();
				rt_thread_delay(10);
		}
}

//******************************保持寄存器回调函数**********************************
//函数定义: eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
//描    述：保持寄存器相关的功能（读、连续读、写、连续写）
//入口参数：pucRegBuffer : 如果需要更新用户寄存器数值，这个缓冲区必须指向新的寄存器数值。
//                         如果协议栈想知道当前的数值，回调函数必须将当前值写入这个缓冲区
//			usAddress    : 寄存器的起始地址。
//			usNRegs      : 寄存器数量
//          eMode        : 如果该参数为eMBRegisterMode::MB_REG_WRITE，用户的应用数值将从pucRegBuffer中得到更新。
//                         如果该参数为eMBRegisterMode::MB_REG_READ，用户需要将当前的应用数据存储在pucRegBuffer中
//出口参数：eMBErrorCode : 这个函数将返回的错误码
//备    注：Editor：Armink 2010-10-31    Company: BXXJS
//**********************************************************************************
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
	extern sys_reg_st  g_sys; 
	eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex;
    USHORT *        pusRegHoldingBuf;
    USHORT          REG_HOLDING_START;
    USHORT          REG_HOLDING_NREGS;
    USHORT          usRegHoldStart;

	extern  conf_reg_map_st conf_reg_map_inst[];
	uint16              Writ_Value;
	const reg_table_st *           pt= modbus_slave_reg_table;
  const mbm_reg_table_st*        mem_pt = mbm_reg_table;
	


	pusRegHoldingBuf = usSRegHoldBuf;
	REG_HOLDING_START = S_REG_HOLDING_START;
	REG_HOLDING_NREGS = S_REG_HOLDING_NREGS;
	usRegHoldStart = usSRegHoldStart;
	usAddress--;//FreeModbus功能函数中已经加1，为保证与缓冲区首地址一致，故减1
    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = usAddress - usRegHoldStart;
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
								*pucRegBuffer++ = ( unsigned char )( pusRegHoldingBuf[iRegIndex] >> 8 );
							  *pucRegBuffer++ = ( unsigned char )( pusRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
					//forbid modbuss option power switch
						if((iRegIndex == 0)&&(g_sys.config.general.power_mode_mb_en ==0))
						{
								eStatus = MB_ENOREG;
								break;//	case MB_REG_WRITE:
						}
            while( usNRegs > 0 )
            {
				
						//超出可写范围报错判断
								if ((pt[iRegIndex].reg_type == 0)||(pt[iRegIndex].reg_type == 3))
								{
									//写入植边界判断
										Writ_Value=0;
										Writ_Value+=(*pucRegBuffer) << 8;
										Writ_Value+=*(pucRegBuffer+1);
										//max min
										
										if((Writ_Value<=conf_reg_map_inst[pt[iRegIndex].reg_addr].max)&&(Writ_Value>=conf_reg_map_inst[pt[iRegIndex].reg_addr].min)&& 
											(pt[iRegIndex].reg_type == 0))
										{
										
												//写入保持寄存器中同时跟新到内存和flash保存
												// 写入寄存器和EEPROM中。
												
												if(reg_map_write(conf_reg_map_inst[pt[iRegIndex].reg_addr].id,&Writ_Value,1,USER_MODEBUS_SLAVE)
													==CPAD_ERR_NOERR)
													{
																pusRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
																pusRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
																iRegIndex++;
																usNRegs--;	

													}
													else
													{
														
														eStatus = MB_ENORES;
														break;//	 while( usNRegs > 0 )
													}
											}
											else if((pt[iRegIndex].reg_type == 3)&&(g_sys.config.general.cancel_alarm_mb_en == 1))
											{
														*mem_pt[pt[iRegIndex].reg_addr].reg_ptr = Writ_Value;
														pusRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
														pusRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
														iRegIndex++;
														usNRegs--;
												
											}
											else
											{
												
												eStatus = MB_EINVAL;
												break;// while( usNRegs > 0 )
											}
									
									
								}
								else
								{
										
									eStatus = MB_ENOREG;
										break;//  while( usNRegs > 0 )
								}
					
						
						
				}
			break;
        }
    }
    else
    {
    
        eStatus = MB_ENOREG;
    }
    return eStatus;
}



void mbs_sts_update(void)// 更新本地变量到协议栈寄存器中
{
		char i=0;
		const reg_table_st *           pt = modbus_slave_reg_table;
		extern	conf_reg_map_st conf_reg_map_inst[];
	  extern  sts_reg_map_st status_reg_map_inst[];
		extern sys_reg_st  g_sys; 
	
		for(i=0;i<S_REG_HOLDING_NREGS;i++)
		{
				if(pt[i].reg_type == 0)
				{
						usSRegHoldBuf[i]=*(conf_reg_map_inst[pt[i].reg_addr].reg_ptr);
				}
				else
				{
						usSRegHoldBuf[i]=*(status_reg_map_inst[pt[i].reg_addr].reg_ptr);
				}
		} 
		
		if(mbm_cmd_reg == 0x01)
		{
				clear_alarm();
				mbm_cmd_reg =0;
		}
	
}
