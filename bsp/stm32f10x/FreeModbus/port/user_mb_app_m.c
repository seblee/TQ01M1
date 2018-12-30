/*
 * FreeModbus Libary: user callback functions and buffer define in master mode
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
 * File: $Id: user_mb_app_m.c,v 1.60 2013/11/23 11:49:05 Armink $
 */
#include "user_mb_app.h"
#include "fifo.h"
#include "global_var.h"
#include "reg_map_check.h"
#include "calc.h"
#include "usart_bsp.h"
#define MBM_FIFO_DEPTH M_REG_HOLDING_NREGS
fifo8_cb_td mbm_data_fifo;

#define MBM_RESPONSE_DELAY 30
#define MBM_QUEST_DELAY 30

static uint16_t mbm_dev_poll(uint16_t mb_comp_mask, uint16_t des_bitmap, mbm_dev_st *mbm_dev_inst);
static uint16_t mbm_dev_init(mbm_dev_st *mbm_dev_inst);
static uint16_t mbm_reg_update(mbm_dev_st *mbm_dev_inst);
//static void mbm_fsm_init(mbm_dev_st *mbm_dev_inst);
//static void mbm_fsm_update(sys_reg_st *gds_ptr, mbm_dev_st *mbm_dev_inst);

/*-----------------------Master mode use these variables----------------------*/
//Master mode:HoldingRegister variables
static uint16_t usMRegHoldStart = M_REG_HOLDING_START;
static uint16_t usMRegHoldBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS];
//static mbm_dev_st mbm_dev_inst;

/**
  * @brief  modbus master poll thread
  * @param  none
  * @retval none
**/

//  mbm_send _data

  void mbm_fifo_init(UCHAR ucMB_Number)
{
    uint8_t block_size;
    //		uint8_t Buff2=0;

    block_size = sizeof(mbm_data_st);
    switch (ucMB_Number)
    {
    case MB_MASTER_0:
        fifo8_init(&mbm_data_fifo, block_size, MBM_FIFO_DEPTH);
        break;
    default:
        break;
    }
    //		Buff2 =fifo8_init(&mbm_data_fifo, block_size,MBM_FIFO_DEPTH);
    //			rt_kprintf("Buff2 = %d\n",Buff2);
}

void modbus_master_thread_entry(void *parameter)
{
    //		eMBErrorCode    eStatus = MB_ENOERR;
    rt_thread_delay(MODBUS_MASTER_THREAD_DELAY);
    xPort_Usart_Init(UART_HEAT); //加热器
                                 //		eStatus = eMBMasterInit(MB_RTU,UPORT_MBMASTER,9600,MB_PAR_NONE,MB_MASTER_0);
                                 //		if(eStatus != MB_ENOERR)
                                 //		{
                                 //				rt_kprintf("MBM init fail\n");
                                 //		}
                                 //		eStatus = eMBMasterEnable(MB_MASTER_0);
                                 //		if(eStatus != MB_ENOERR)
                                 //		{
                                 //				rt_kprintf("MBM enable fail\n");
                                 //		}
                                 //		//init mbm_fifo
                                 //		mbm_fifo_init(MB_MASTER_0);
    while (1)
    {
        ////				eStatus = eMBMasterPoll(MB_MASTER_0);
        //				eStatus = eMBMasterPoll();
        //				if(eStatus != MB_ENOERR)
        //				{
        //						rt_kprintf("MBM poll err!\n");
        //				}
        Comm_Service();
        rt_thread_delay(20);
    }
}

void mbm_fsm_thread_entry(void *parameter)
{
    //		extern sys_reg_st	g_sys;
    //		rt_thread_delay(MBM_FSM_THREAD_DELAY);
    //		mbm_fsm_init(&mbm_dev_inst[MB_MASTER_0]);								//initialize local modbus master register set
    while (1)
    {
        //				mbm_fsm_update(&g_sys,&mbm_dev_inst[MB_MASTER_0],MB_MASTER_0);		//update modbus slave components into local modbus master register
        rt_thread_delay(1000);
    }
}

//******************************保持寄存器回调函数**********************************
//函数定义: eMBErrorCode eMBMasterRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
//描    述：保持寄存器相关的功能（读、连续读、写、连续写）
//入口参数：pucRegBuffer : 如果需要更新用户寄存器数值，这个缓冲区必须指向新的寄存器数值。
//                         如果协议栈想知道当前的数值，回调函数必须将当前值写入这个缓冲区
//					usAddress    : 寄存器的起始地址。
//					usNRegs      : 寄存器数量
//          eMode        : 如果该参数为eMBRegisterMode::MB_REG_WRITE，用户的应用数值将从pucRegBuffer中得到更新。
//                         如果该参数为eMBRegisterMode::MB_REG_READ，用户需要将当前的应用数据存储在pucRegBuffer中
//出口参数：eMBErrorCode : 这个函数将返回的错误码
//备    注：Editor：Armink 2013-11-25    Company: BXXJS
//**********************************************************************************
eMBErrorCode
eMBMasterRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode)
{
    extern sys_reg_st g_sys;
    eMBErrorCode eStatus = MB_ENOERR;
    uint16_t iRegIndex;
    uint16_t *pusRegHoldingBuf;
    uint16_t REG_HOLDING_START;
    uint16_t REG_HOLDING_NREGS;
    uint16_t usRegHoldStart;
    //		uint8_t           mbm_dest_addr,index;
    uint8_t mbm_dest_addr;

    mbm_dest_addr = ucMBMasterGetDestAddress() - 1;

    pusRegHoldingBuf = usMRegHoldBuf[mbm_dest_addr];
    REG_HOLDING_START = M_REG_HOLDING_START;
    REG_HOLDING_NREGS = M_REG_HOLDING_NREGS;
    usRegHoldStart = usMRegHoldStart;
    //If mode is read,the master will wirte the received date to bufffer.
    if (eMode == MB_REG_WRITE)
    {
        return (MB_ENOERR);
    }

    eMode = MB_REG_WRITE;
    usAddress--; //FreeModbus功能函数中已经加1，为保证与缓冲区首地址一致，故减1
    if ((usAddress >= REG_HOLDING_START) &&
        (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
    {
        iRegIndex = usAddress - usRegHoldStart;
        switch (eMode)
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while (usNRegs > 0)
            {
                *pucRegBuffer++ = (unsigned char)(pusRegHoldingBuf[iRegIndex] >> 8);
                *pucRegBuffer++ = (unsigned char)(pusRegHoldingBuf[iRegIndex] & 0xFF);
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while (usNRegs > 0)
            {
                if (((g_sys.config.dev_mask.mb_discrete_mask >> mbm_dest_addr) & 0x0001) == 0)
                {
                    pusRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                    pusRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                    iRegIndex++;
                    usNRegs--;
                }
                else
                {
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

/**
  * @brief  poll modbus master slave device, if exist, set dev_mask bitmap accordingly
  * @param  
			@mb_comp_mask: system modbus slave device bitmap configuration.
			@mbm_dev_inst: modbus master device data struct.
  * @retval 
			@arg 1: all device online
			@arg 0: not all device online
  */
static uint16_t mbm_dev_poll(uint16_t mb_comp_mask, uint16_t des_bitmap, mbm_dev_st *mbm_dev_inst)
{
    eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;
    uint16_t i;
    uint16_t dev_poll_bitmap_reg;
    uint16_t xor_bitmap;

    dev_poll_bitmap_reg = mbm_dev_inst->bitmap.poll;

    xor_bitmap = mb_comp_mask ^ dev_poll_bitmap_reg;
    mbm_dev_inst->bitmap.discrete = des_bitmap;

    dev_poll_bitmap_reg = 0;
    if (xor_bitmap == 0) //if default bitmap equals to online bitmap, means all device are online, return ture
    {
        return 1;
    }
    else
    {
        for (i = 0; i < MB_MASTER_TOTAL_SLAVE_NUM; i++)
        {
            if (((xor_bitmap >> i) & 0x0001) == 1)
            {
                if (((des_bitmap >> i) & 0x0001) == 0)
                {
                    errorCode = eMBMasterReqReadHoldingRegister((i + 1), 0, 1, MBM_RESPONSE_DELAY);
                    if (errorCode == MB_MRE_NO_ERR)
                    {
                        dev_poll_bitmap_reg |= (0x0001 << i); //set online flag
                    }
                    rt_thread_delay(MBM_QUEST_DELAY);
                }
                else
                {

                    //												errorCode = eMBMasterReqReadHoldingRegister((i+1),mbm_read_table[i-MB_MASTER_DISCRETE_OFFSET].reg_addr[0],1,MBM_RESPONSE_DELAY);
                    //												if(errorCode == MB_MRE_NO_ERR)
                    //												{
                    //															dev_poll_bitmap_reg |= (0x0001<<i);		//set online flag
                    //												}
                    //												else
                    //												{
                    //															rt_kprintf("mbm_read_table erro= %d\n",errorCode);
                    //												}
                    //												rt_thread_delay(MBM_QUEST_DELAY);
                }
            }
        }

        mbm_dev_inst->bitmap.poll = dev_poll_bitmap_reg;
        if (dev_poll_bitmap_reg == mb_comp_mask)
        {
            mbm_dev_inst->timeout.poll = 0;
            return 1;
        }
        else
        {
            mbm_dev_inst->errcnt.poll++;
            mbm_dev_inst->timeout.poll++;
            return 0;
        }
    }
}

/**
  * @brief  initialize modbus master slave device registers
  * @param  
			@mbm_dev_inst: modbus master device data struct.
  * @retval 
			@arg 1: all device online
			@arg 0: not all device online
  */
static uint16_t mbm_dev_init(mbm_dev_st *mbm_dev_inst)
{
    eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;
    uint16_t dev_poll_bitmap_reg;
    uint16_t dev_init_bitmap_reg;
    uint16_t dev_reg_cnt;
    uint16_t xor_bitmap;
    //		uint16_t i,j,des_bitmap;
    uint16_t i, des_bitmap;

    dev_poll_bitmap_reg = mbm_dev_inst->bitmap.poll;
    dev_init_bitmap_reg = mbm_dev_inst->bitmap.init;
    des_bitmap = mbm_dev_inst->bitmap.discrete;
    xor_bitmap = dev_poll_bitmap_reg ^ dev_init_bitmap_reg;

    if (xor_bitmap == 0) //if default bitmap equals to online bitmap, means all device are online, return ture
    {
        return 1;
    }
    else
    {
        for (i = 0; i < MB_MASTER_TOTAL_SLAVE_NUM; i++)
        {
            if (((xor_bitmap >> i) & 0x0001) == 1)
            {
                if (((des_bitmap >> i) & 0x0001) == 0)
                {
                    dev_reg_cnt = usMRegHoldBuf[i][MBM_REG_ADDR_CNT_ADDR];
                    errorCode = eMBMasterReqReadHoldingRegister((i + 1), 0, dev_reg_cnt, MBM_RESPONSE_DELAY);
                    if (errorCode == MB_MRE_NO_ERR)
                    {
                        dev_init_bitmap_reg |= (0x0001 << i);
                    }
                    rt_thread_delay(MBM_QUEST_DELAY);
                }
                else
                {
                }
            }
        }
        mbm_dev_inst->bitmap.init = dev_init_bitmap_reg;
        if (dev_init_bitmap_reg == dev_poll_bitmap_reg)
        {
            mbm_dev_inst->timeout.init = 0;
            return 1;
        }
        else
        {
            mbm_dev_inst->errcnt.init++;
            mbm_dev_inst->timeout.init++;
            if (mbm_dev_inst->timeout.init >= MBM_INIT_TIMEOUT_THRESHOLD)
            {
                mbm_dev_inst->bitmap.poll = dev_init_bitmap_reg;
            }
            return 0;
        }
    }
}

/**
  * @brief  update local modbus master register map with only variable device reg values
  * @param  
			@mbm_dev_inst: modbus master device data struct.
  * @retval 
			@arg 1: all device online
			@arg 0: not all device online
  */
static uint16_t mbm_reg_update(mbm_dev_st *mbm_dev_inst)
{
    extern sys_reg_st g_sys;
    eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;
    uint16_t dev_init_bitmap_reg;
    uint16_t dev_update_bitmap_reg;
    uint16_t dev_reg_cnt;
    //		uint16_t i,j,des_bitmap;
    uint16_t i, des_bitmap;

    des_bitmap = mbm_dev_inst->bitmap.discrete;
    dev_init_bitmap_reg = mbm_dev_inst->bitmap.init;
    dev_update_bitmap_reg = 0;

    //			//test
    //		dev_init_bitmap_reg=g_sys.config.dev_mask.mb_comp;
    //		dev_update_bitmap_reg=dev_init_bitmap_reg;

    for (i = 0; i < MB_MASTER_TOTAL_SLAVE_NUM; i++)
    {
        if (((dev_init_bitmap_reg >> i) & 0x0001) == 1)
        {
            if (((des_bitmap >> i) & 0x0001) == 0)
            {
                dev_reg_cnt = usMRegHoldBuf[i][MBM_REG_ADDR_CNT_ADDR] - M_REG_HOLDING_USR_START;
                errorCode = eMBMasterReqReadHoldingRegister((i + 1), M_REG_HOLDING_USR_START, dev_reg_cnt, MBM_RESPONSE_DELAY);
                if (errorCode == MB_MRE_NO_ERR)
                {
                    dev_update_bitmap_reg |= (0x0001 << i);
                }
                rt_thread_delay(MBM_QUEST_DELAY);
            }
            else
            {
                //								 for(j=0;j<mbm_read_table[i-MB_MASTER_DISCRETE_OFFSET].reg_cnt;j++)
                //								 {
                //											errorCode = eMBMasterReqReadHoldingRegister((i+1),mbm_read_table[i-MB_MASTER_DISCRETE_OFFSET].reg_addr[j],1,MBM_RESPONSE_DELAY);
                //											if(errorCode == MB_MRE_NO_ERR)
                //											{
                //													dev_update_bitmap_reg |= (0x0001<<i);
                //											}
                //											else
                //											{
                //														rt_kprintf("mbm_read_table erro= %d\n",errorCode);
                //											}
                //											rt_thread_delay(MBM_QUEST_DELAY);
                //								 }
            }
        }
    }
    //		rt_kprintf("dev_init_bitmap_reg erro= %X,dev_update_bitmap_reg erro= %X\n",dev_init_bitmap_reg,dev_update_bitmap_reg);
    mbm_dev_inst->bitmap.update = dev_update_bitmap_reg;
    if (dev_update_bitmap_reg == mbm_dev_inst->bitmap.init)
    {
        mbm_dev_inst->timeout.update = 0;
        return 1;
    }
    else
    {
        mbm_dev_inst->timeout.update++;
        mbm_dev_inst->errcnt.update++;
        if (mbm_dev_inst->timeout.update >= MBM_UPDATE_TIMEOUT_THRESHOLD)
        {
            mbm_dev_inst->bitmap.init = dev_update_bitmap_reg;
        }
        return 0;
    }
}

static void mbm_send_fun(mbm_data_st *send_data)
{
    eMBMasterReqErrCode errorCode = MB_MRE_NO_ERR;
    //Alair,20161226
    if (send_data->mbm_WriteType == MB_WRITE_MULITE)
    {
        errorCode = eMBMasterReqWriteMultipleHoldingRegister(send_data->mbm_addr, send_data->reg_addr, send_data->mbm_NRegs, &(send_data->reg_value), MBM_RESPONSE_DELAY);
        if (errorCode != MB_MRE_NO_ERR)
        {
            rt_kprintf("mbm_send_fun erro= %d\n", errorCode);
        }
    }
    else
    {
        errorCode = eMBMasterReqWriteHoldingRegister(send_data->mbm_addr, send_data->reg_addr, send_data->reg_value, MBM_RESPONSE_DELAY);
        if (errorCode != MB_MRE_NO_ERR)
        {
            rt_kprintf("mbm_send_fun erro= %d\n", errorCode);
        }
    }

    rt_thread_delay(MBM_QUEST_DELAY);
}
/**
  * @brief  update local modbus master register map with only variable device reg values(ie. reg addr after 20)
  * @param  mbm_dev_inst: modbus master device data struct.
  * @retval none
  */
  void mbm_fsm_update(sys_reg_st *gds_ptr, mbm_dev_st *mbm_dev_inst)
{
    uint16_t mbm_fsm_cstate;
    uint8_t i, len;
    mbm_data_st send_data;

    mbm_fsm_cstate = mbm_dev_inst->mbm_fsm;

    switch (mbm_fsm_cstate)
    {
    case (MBM_FSM_IDLE):
    {
        mbm_dev_poll(gds_ptr->config.dev_mask.mb_comp, gds_ptr->config.dev_mask.mb_discrete_mask, mbm_dev_inst);
        mbm_dev_init(mbm_dev_inst);
        mbm_dev_inst->mbm_fsm = MBM_FSM_SYNC;
        break;
    }
    case (MBM_FSM_SYNC):
    {
        mbm_dev_poll(gds_ptr->config.dev_mask.mb_comp, gds_ptr->config.dev_mask.mb_discrete_mask, mbm_dev_inst);
        mbm_dev_init(mbm_dev_inst);
        mbm_reg_update(mbm_dev_inst);
        if (((mbm_dev_inst->bitmap.update) ^ (gds_ptr->config.dev_mask.mb_comp)) == 0) //if init succeeded, go into update state, otherwise remain sync state
        {
            mbm_dev_inst->mbm_fsm = MBM_FSM_UPDATE;
        }
        else if (is_fifo8_empty(&mbm_data_fifo) == 0)
        {
            mbm_dev_inst->mbm_fsm = MBM_FSM_SEND;
        }
        else
        {
            mbm_dev_inst->mbm_fsm = MBM_FSM_SYNC;
        }
        break;
    }
    case (MBM_FSM_UPDATE):
    {
        mbm_reg_update(mbm_dev_inst);
        if ((mbm_dev_inst->timeout.update >= MBM_UPDATE_TIMEOUT_THRESHOLD) || (gds_ptr->config.dev_mask.mb_comp != mbm_dev_inst->bitmap.update)) //if update err count timeout, swich to sync state
        {
            mbm_dev_inst->bitmap.poll = mbm_dev_inst->bitmap.update;
            mbm_dev_inst->mbm_fsm = MBM_FSM_SYNC;
        }
        // go to  MBM_FSM_SEND
        else if (is_fifo8_empty(&mbm_data_fifo) == 0)
        {
            mbm_dev_inst->mbm_fsm = MBM_FSM_SEND;
        }
        else
        {
            mbm_dev_inst->mbm_fsm = MBM_FSM_UPDATE;
        }
        break;
    }
    case MBM_FSM_SEND:
    {
        len = get_fifo8_length(&mbm_data_fifo);
        for (i = 0; i < len; i++)
        {
            if (fifo8_pop(&mbm_data_fifo, (uint8_t *)&send_data) == 1)
            {
                mbm_send_fun(&send_data);
            }
        }
        mbm_dev_inst->mbm_fsm = MBM_FSM_UPDATE;
        break;
    }
    default:
    {
        mbm_dev_inst->mbm_fsm = MBM_FSM_IDLE;
        break;
    }
    }
    gds_ptr->status.status_remap[MBM_COM_STS_REG_NO] = mbm_dev_inst->bitmap.update;
}

/**
  * @brief  modbus local data structure initialization
  * @param  mbm_dev_inst: modbus master device data struct.
  * @retval none
  */
   void mbm_fsm_init(mbm_dev_st *mbm_dev_inst)
{
    mbm_dev_inst->bitmap.poll = 0;
    mbm_dev_inst->bitmap.init = 0;
    mbm_dev_inst->bitmap.update = 0;
    mbm_dev_inst->timeout.poll = 0;
    mbm_dev_inst->timeout.init = 0;
    mbm_dev_inst->timeout.update = 0;
    mbm_dev_inst->errcnt.poll = 0;
    mbm_dev_inst->errcnt.init = 0;
    mbm_dev_inst->errcnt.update = 0;
    mbm_dev_inst->mbm_fsm = MBM_FSM_IDLE;
}

/**
  * @brief  modbus module interface, update global register with designated local modbus register values
  * @param  gds_ptr		global register struct pointer
  * @retval none
**/
void mbm_sts_update(sys_reg_st *gds_ptr)
{
    //		uint16_t mbm_dev_update_bitmap;
    //		uint16_t mbm_dev_init_bitmap;
    //		uint16_t i;
    ////		mbm_data_st mbm_send_data;
    //
    //
    //		mbm_dev_init_bitmap = mbm_dev_inst.bitmap.init;
    //		mbm_dev_update_bitmap = mbm_dev_inst.bitmap.update;	//get modbus update bitmap which could be used to determin which glabal regsiter to update
    //		mbm_dev_init_bitmap = mbm_dev_inst.bitmap.init;	//get modbus init bitmap which could be used to determin which glabal regsiter to update
    ///*********************************加湿检测板***************************************************/
    //		if(((mbm_dev_init_bitmap >> HUM_MODULE_BPOS)&0x0001) != 0)
    //		{
    //				if(((mbm_dev_update_bitmap >> HUM_MODULE_BPOS)&0x0001) != 0)	//copy modbus master device register to global register, humidifier daq
    //				{
    //					  //有加湿电流和注水的情况下不更新
    //						if((gds_ptr->status.dout_bitmap&(0x01<<DO_FILL_BPOS))&&(gds_ptr->status.dout_bitmap&(0x01<<DO_HUM_BPOS)))
    //						{
    //								;
    //						}
    //						else
    //						{
    //
    //								gds_ptr->status.mbm.hum[0].conductivity = usMRegHoldBuf[HUM_MODULE_BPOS][MBM_DEV_H_REG_CONDUCT_ADDR];
    //
    //						}
    //						gds_ptr->status.mbm.hum[0].dev_sts = usMRegHoldBuf[HUM_MODULE_BPOS][MBM_DEV_H_REG_STATUS_ADDR];
    //						gds_ptr->status.mbm.hum[0].hum_current = usMRegHoldBuf[HUM_MODULE_BPOS][MBM_DEV_H_REG_HUMCUR_ADDR];
    //						gds_ptr->status.mbm.hum[0].water_level = usMRegHoldBuf[HUM_MODULE_BPOS][MBM_DEV_H_REG_WT_LV_ADDR];
    //				}
    //		}
    //		else	//if device is not initialized, all date reset to 0
    //		{
    //				gds_ptr->status.mbm.hum[0].dev_sts = 0;
    //				gds_ptr->status.mbm.hum[0].conductivity = 0;
    //				gds_ptr->status.mbm.hum[0].hum_current = 0;
    //				gds_ptr->status.mbm.hum[0].water_level = 0;
    //		}
    ////		rt_kprintf("mbm_dev_init_bitmap= %X,mbm_dev_update_bitmap= %X\n",mbm_dev_init_bitmap,mbm_dev_update_bitmap);
    ////		rt_kprintf("hum_current= %d,water_level= %d\n",gds_ptr->status.mbm.hum[0].hum_current,gds_ptr->status.mbm.hum[0].water_level);
    //		//加湿板2，用于检测电流
    //		if(((mbm_dev_init_bitmap >> HUM2_MODULE_BPOS)&0x0001) != 0)
    //		{
    //				if(((mbm_dev_update_bitmap >> HUM2_MODULE_BPOS)&0x0001) != 0)	//copy modbus master device register to global register, humidifier daq
    //				{
    //						gds_ptr->status.mbm.hum[1].dev_sts = usMRegHoldBuf[HUM2_MODULE_BPOS][MBM_DEV_H_REG_STATUS_ADDR];
    ////						gds_ptr->status.mbm.hum[1].hum_current = usMRegHoldBuf[HUM2_MODULE_BPOS][MBM_DEV_H_REG_HUMCUR_ADDR]/10;
    //						gds_ptr->status.mbm.hum[1].hum_current = usMRegHoldBuf[HUM2_MODULE_BPOS][MBM_DEV_H_REG_HUMCUR_ADDR]/10+(int16_t)(gds_ptr->config.general.Hum_Current_cali);
    //						gds_ptr->status.mbm.hum[1].water_level = usMRegHoldBuf[HUM2_MODULE_BPOS][MBM_DEV_H_REG_WT_LV_ADDR];
    //				}
    ////						rt_kprintf("gds_ptr->status.mbm.hum[1].hum_current= %d\n",gds_ptr->status.mbm.hum[1].hum_current);
    //		}
    //		else	//if device is not initialized, all date reset to 0
    //		{
    //				gds_ptr->status.mbm.hum[1].dev_sts = 0;
    //				gds_ptr->status.mbm.hum[1].conductivity = 0;
    //				gds_ptr->status.mbm.hum[1].hum_current = 0;
    //				gds_ptr->status.mbm.hum[1].water_level = 0;
    //		}

    ///*********************************电源检测板***************************************************/
    //		//Alair,20170308
    //		for(i=0;i<MBM_DEV_P_NUM;i++)	//最多2个电源板
    //		{
    //				if(((mbm_dev_init_bitmap >>(POWER_MODULE_BPOS+i))&0x0001) != 0)
    //				{
    //						if(((mbm_dev_update_bitmap >> (POWER_MODULE_BPOS+i))&0x0001) != 0)
    //						{
    //							gds_ptr->status.mbm.pwr[i].dev_sts = usMRegHoldBuf[POWER_MODULE_BPOS+i][MBM_DEV_H_REG_P0WER_STATUS_ADDR];
    //							gds_ptr->status.mbm.pwr[i].pa_volt = usMRegHoldBuf[POWER_MODULE_BPOS+i][MBM_DEV_H_REG_PA_VOLT_ADDR];
    //							gds_ptr->status.mbm.pwr[i].pb_volt =	usMRegHoldBuf[POWER_MODULE_BPOS+i][MBM_DEV_H_REG_PB_VOLT_ADDR];
    //							gds_ptr->status.mbm.pwr[i].pc_volt =	usMRegHoldBuf[POWER_MODULE_BPOS+i][MBM_DEV_H_REG_PC_VOLT_ADDR];
    //							gds_ptr->status.mbm.pwr[i].freq = usMRegHoldBuf[POWER_MODULE_BPOS+i][MBM_DEV_H_REG_FREQ_ADDR];
    //							gds_ptr->status.mbm.pwr[i].pe_bitmap =	usMRegHoldBuf[POWER_MODULE_BPOS+i][MBM_DEV_H_REG_PE_ADDR];
    //						}
    //
    //				}
    //				else //if device is not initialized, all date reset to 0
    //				{
    //						gds_ptr->status.mbm.pwr[i].dev_sts = 0;
    //						gds_ptr->status.mbm.pwr[i].pa_volt = 0;
    //						gds_ptr->status.mbm.pwr[i].pb_volt =	0;
    //						gds_ptr->status.mbm.pwr[i].pc_volt =	0;
    //						gds_ptr->status.mbm.pwr[i].freq = 0;
    //						gds_ptr->status.mbm.pwr[i].pe_bitmap =	0;
    //				}
    //		}
    ///*********************************温湿度传感器***************************************************/
    //		for(i=0;i<TEMP_HUM_SENSOR_NUM;i++)	//copy modbus master device register to global register, temp and hum sensor daq
    //		{
    //				if(((mbm_dev_init_bitmap >> i)&0x0001) != 0)
    //				{
    //						if(((mbm_dev_update_bitmap >> i)&0x0001) != 0)
    //						{
    //								gds_ptr->status.mbm.tnh[i].dev_sts = usMRegHoldBuf[i][MBM_DEV_H_REG_HT_STATUS_ADDR];
    //								gds_ptr->status.mbm.tnh[i].temp = usMRegHoldBuf[i][MBM_DEV_H_REG_TEMP_ADDR]+(int16_t)(gds_ptr->config.general.temp_sensor_cali[i].temp);
    //								gds_ptr->status.mbm.tnh[i].hum = usMRegHoldBuf[i][MBM_DEV_H_REG_HUM_ADDR]+(int16_t)(gds_ptr->config.general.temp_sensor_cali[i].hum);
    //						}
    //				}
    //				else //if device is not initialized, all date reset to 0
    //				{
    //						gds_ptr->status.mbm.tnh[i].dev_sts = 0;
    //						gds_ptr->status.mbm.tnh[i].temp = 0;
    //						gds_ptr->status.mbm.tnh[i].hum = 0;
    //				}
    //		}
}
