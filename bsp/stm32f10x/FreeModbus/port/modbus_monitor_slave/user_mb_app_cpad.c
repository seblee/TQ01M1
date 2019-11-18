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
#include "mb_event_cpad.h"

#include "mbport_cpad.h"
#include "global_var.h"
#include "event_record.h"
#include "local_status.h"
#include "dio_bsp.h"
#include "req_execution.h"

/*------------------------Slave mode use these variables----------------------*/
//Slave mode:HoldingRegister variables
extern cpad_slave_st cpad_slave_inst;
static uint16_t mbs_read_reg(uint16_t read_addr);

USHORT cpad_usSRegHoldBuf[CMD_REG_SIZE];
USHORT cpad_usSRegHoldStart = CPAD_S_REG_HOLDING_START;

typedef struct
{
    uint8_t reg_type; //0=config_reg;1 =status_reg;
    uint16_t reg_addr;
    uint8_t reg_w_r; //3 =write&read,2=read_only,3=write_only
} reg_table_st;

//内存到modbus的映射表。
//元素位置对应ModeBus  协议栈中usSRegHoldBuf位置
//元素值对应conf_reg_map_inst，内存数据的位置。

void cpad_modbus_slave_thread_entry(void *parameter)
{
    extern sys_reg_st g_sys;
    extern local_reg_st l_sys;

    rt_thread_delay(MODBUS_SLAVE_THREAD_DELAY);
    cpad_MBRTUInit(1, UPORT_CPAD, 19200, MB_PAR_NONE);
    rt_kprintf("cpad_modbus_slave_thread_entry\n");
    while (1)
    {
        l_sys.u16Uart_Timeout[0]++;
        if (l_sys.u16Uart_Timeout[0] >= 500)
        {
            l_sys.u16Uart_Timeout[0] = 0;
            cpad_MBRTUInit(1, UPORT_CPAD, 19200, MB_PAR_NONE);
        }
        // if(l_sys.SEL_Jump&Com_Pad)//串口屏
        // {
        // 	Cpad_Update();
        // 	rt_thread_delay(200);
        // }
        // else
        // {
        cpad_MBPoll();
        rt_thread_delay(10);
        // }
    }
}

uint8_t COM_SINGLE_eMBRegHoldingCB(uint16_t usAddress, uint16_t usValue)
{
    extern local_reg_st l_sys;
    extern sys_reg_st g_sys;
    extern conf_reg_map_st conf_reg_map_inst[];
    eMBErrorCode eStatus = MB_ENOERR;
    uint16_t temp = 0;
    uint16_t u16RegAddr = usAddress;
    uint16_t u16Value = usValue;

    switch (u16RegAddr)
    {
    case FACTORY_RESET: //出厂设置
    {
        temp = usValue;
        if (temp == 0x3C) //恢复原始参数
        {
            reset_runtime(0xFF); //清零所有运行时间
            set_load_flag(0x02);
            rt_thread_delay(1000);
            NVIC_SystemReset();
            return MB_ENOERR;
        }
        else if (temp == 0x5A) //恢复出厂设置
        {
            set_load_flag(0x01);
            rt_thread_delay(1000);
            NVIC_SystemReset();
            return MB_ENOERR;
        }
        else if (temp == 0x69) //保存出厂设置
        {
            save_conf_reg(0x01);
            rt_thread_delay(1000);
            NVIC_SystemReset();
            return MB_ENOERR;
        }
        else if (temp == 0x2D) //重启
        {
            Close_DIS_PWR(1);
            rt_thread_delay(1000);
            NVIC_SystemReset();
            return MB_ENOERR;
        }
        else if (temp == 0x1E) //屏重启
        {
            Close_DIS_PWR(1);
            rt_thread_delay(1000);
            Close_DIS_PWR(0);
            return MB_ENOERR;
        }
        else
        {
            eStatus = MB_ENORES;
        }
    }
    break;
    case MANUAL_TSET: //测试模式
    {
        temp = usValue;
        if (temp == MANUAL_TEST_UNABLE)
        {
            rt_thread_delay(500);
            NVIC_SystemReset();
            return MB_ENOERR;
        }
        else
        {
            if (reg_map_write(conf_reg_map_inst[u16RegAddr].id, &usValue, 1, USER_CPAD) == CPAD_ERR_NOERR)
            {
                //										iRegIndex++;
                //										usNRegs--;
                eStatus = MB_ENOERR;
            }
            else
            {

                eStatus = MB_ENORES;
                //	 while( usNRegs > 0 )
            }
        }
    }
    break;
    case CLEAR_RT: //清零部件运行时间
    {
        temp = usValue;
        if (temp) //清零部件运行时间
        {
            reset_runtime(temp);
            return MB_ENOERR;
        }
        else
        {
            eStatus = MB_ENORES;
        }
    }
    break;
    case CLEAR_ALARM: //清除告警
    {
        temp = usValue;
        if (temp == 0x5A) //清零部件运行时间
        {
            clear_alarm();
            return MB_ENOERR;
        }
        else
        {
            eStatus = MB_ENORES;
        }
    }
    break;
    case SET_TL: //系统时间低位
    {
        temp = u16Value;
        if (temp != NULL) //系统时间低位
        {
            l_sys.Set_Systime_Delay = SETTIME_DELAY;
            l_sys.Set_Systime_Flag |= 0x01;
            g_sys.config.ComPara.u16Set_Time[0] = temp;
            return MB_ENOERR;
        }
        else
        {
            eStatus = MB_ENORES;
        }
    }
    break;
    case SET_TH: //系统时间高位
    {
        temp = u16Value;
        if (temp != NULL) //系统时间高位
        {
            l_sys.Set_Systime_Delay = SETTIME_DELAY;
            l_sys.Set_Systime_Flag |= 0x02;
            g_sys.config.ComPara.u16Set_Time[1] = temp;
            return MB_ENOERR;
        }
        else
        {
            eStatus = MB_ENORES;
        }
    }
    break;
    default:
    {
        if (reg_map_write(conf_reg_map_inst[u16RegAddr].id, &usValue, 1, USER_CPAD) == CPAD_ERR_NOERR)
        {
            //										iRegIndex++;
            //										usNRegs--;
            eStatus = MB_ENOERR;
        }
        else
        {

            eStatus = MB_ENORES;
        }
    }
    break;
    }
    return eStatus;
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
cpad_eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, uint8_t eMode)
{
    extern local_reg_st l_sys;
    extern sys_reg_st g_sys;
    extern conf_reg_map_st conf_reg_map_inst[];
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT iRegIndex;
    USHORT *pusRegHoldingBuf;
    USHORT REG_HOLDING_START;
    USHORT REG_HOLDING_NREGS;
    USHORT usRegHoldStart;
    USHORT i;

    uint16 cmd_value;
    //		uint16_t            temp = 0;
    uint16_t u16RegAddr = 0;

    pusRegHoldingBuf = cpad_usSRegHoldBuf;
    REG_HOLDING_START = CPAD_S_REG_HOLDING_START;
    REG_HOLDING_NREGS = CPAD_S_REG_HOLDING_NREGS;
    usRegHoldStart = cpad_usSRegHoldStart;
    //usAddress--;//FreeModbus功能函数中已经加1，为保证与缓冲区首地址一致，故减1
    if ((usAddress >= REG_HOLDING_START) &&
        (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
    {
        iRegIndex = usAddress - usRegHoldStart;
        switch (eMode)
        {
        /* Pass current register values to the protocol stack. */
        case CPAD_MB_REG_READ:
            while (usNRegs > 0)
            {
                cmd_value = mbs_read_reg(iRegIndex);
                *pucRegBuffer++ = (unsigned char)(cmd_value >> 8);
                *pucRegBuffer++ = (unsigned char)(cmd_value & 0xFF);
                iRegIndex++;
                usNRegs--;
            }
            break;

        /* Update current register values with new values from the
                         * protocol stack. */
        case CPAD_MB_REG_SINGLE_WRITE:
            while (usNRegs > 0)
            {

                //超出可写范围报错判断
                if ((usAddress + usNRegs) <= (REG_HOLDING_START + CPAD_REG_HOLDING_WRITE_NREGS))
                {
                    if ((usAddress + usNRegs) >= (REG_HOLDING_START + CONFIG_REG_MAP_OFFSET + 1))
                    {
                        cmd_value = (*pucRegBuffer) << 8;
                        cmd_value += *(pucRegBuffer + 1);
                        //写入保持寄存器中同时跟新到内存和flash保存
                        // 写入寄存器和EEPROM中。
                        //																								g_sys.status.general.TEST=0x5A;
                        u16RegAddr = iRegIndex - CONFIG_REG_MAP_OFFSET;
                        if (COM_SINGLE_eMBRegHoldingCB(u16RegAddr, cmd_value) == MB_ENOERR)
                        {
                            usNRegs--;
                        }
                        else
                        {
                            eStatus = MB_ENORES;
                            break; //	 while( usNRegs > 0 )
                        }
                    }
                    else
                    {
                        pusRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                        pusRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                        iRegIndex++;
                        usNRegs--;
                    }
                }
                else
                {

                    eStatus = MB_ENOREG;
                    break; //  while( usNRegs > 0 )
                }
            }
            break;

        case CPAD_MB_REG_MULTIPLE_WRITE:
            //手操器分帧传输，一帧最大是100个寄存器
            if ((usNRegs > 0) && (usNRegs <= 100))
            {
                //超出可写范围报错判断
                if ((usAddress + usNRegs) <= (REG_HOLDING_START + CPAD_REG_HOLDING_WRITE_NREGS))
                {
                    if ((usAddress + usNRegs) >= (REG_HOLDING_START + CONFIG_REG_MAP_OFFSET + 1))
                    {
                        for (i = 0; i < usNRegs; i++)
                        {
                            cmd_value = (*pucRegBuffer) << 8;
                            cmd_value += *(pucRegBuffer + 1);
                            *(conf_reg_map_inst[usAddress - CONFIG_REG_MAP_OFFSET + i].reg_ptr) = cmd_value;
                            pucRegBuffer += 2;
                        }
                        // if (CONF_REG_MAP_NUM == (usAddress - CONFIG_REG_MAP_OFFSET + i))
                        // {
                        rt_kprintf("modbus multiple write complete.\n");
                        save_conf_reg(0); //写入保持寄存器中同时跟新到内存和flash保存  // 写入寄存器和EEPROM中。
                        // }
                        if ((usAddress - CONFIG_REG_MAP_OFFSET) == EE_WIFI_PASSWORD) //设置WIFI密码标志
                        {
                            g_sys.config.ComPara.Net_Conf.u16Net_WifiSet = WIFI_SET;
                            RAM_Write_Reg(EE_WIFI_SET, g_sys.config.ComPara.Net_Conf.u16Net_WifiSet, 1);
                        }
                    }
                }
                else
                {
                    rt_kprintf("CPAD_MB_REG_MULTIPLE_WRITE more usNRegs failed\n"); //数量过多
                    eStatus = MB_ENOREG;
                    break;
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

static uint16_t mbs_read_reg(uint16_t read_addr)
{
    extern conf_reg_map_st conf_reg_map_inst[];
    extern sts_reg_map_st status_reg_map_inst[];
    if (read_addr < CMD_REG_SIZE)
    {
        return (cpad_usSRegHoldBuf[read_addr]);
    }
    else if ((CMD_REG_SIZE <= read_addr) && (read_addr < (CONF_REG_MAP_NUM + CMD_REG_SIZE)))
    {
        return (*(conf_reg_map_inst[read_addr - CMD_REG_SIZE].reg_ptr));
    }
    else if ((STATUS_REG_MAP_OFFSET <= read_addr) && (read_addr < (STATUS_REG_MAP_OFFSET + STATUS_REG_MAP_NUM)))
    {
        return (*(status_reg_map_inst[read_addr - STATUS_REG_MAP_OFFSET].reg_ptr));
    }
    else
    {
        return (0x7fff);
    }
}
