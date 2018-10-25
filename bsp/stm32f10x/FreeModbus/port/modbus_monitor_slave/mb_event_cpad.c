
#include <rtthread.h>

#include "mbport_cpad.h"
#include "string.h"
#include "mbcrc.h"
#include "mb_event_cpad.h"
#include "global_var.h"

cpad_slave_st cpad_slave_inst;

void cpad_MBRTUInit(UCHAR ucSlaveAddress, UCHAR ucPort, ULONG ulBaudRate, eMBParity eParity)
{

    ULONG usTimerT35_50us;
    cpad_slave_inst.addr = ucSlaveAddress;
    cpad_slave_inst.rec_cnt = 0;
    cpad_slave_inst.rec_state = REC_ADDR_STATE;
    cpad_slave_inst.rx_flag = 0;
    cpad_slave_inst.rx_ok = 0;
    cpad_slave_inst.rx_timeout = 0;
    cpad_slave_inst.update_timer_flag = 0;
    /* Modbus RTU uses 8 Databits. */
    cpad_MBPortSerialInit(ucPort, ulBaudRate, 8, eParity);

    /* If baudrate > 19200 then we should use the fixed timer values
		 * t35 = 1750us. Otherwise t35 must be 3.5 times the character time.
		 */

    if (ulBaudRate > 19200)
    {
        usTimerT35_50us = 35; /* 1800us. */
    }
    else
    {
        /* The timer reload value for a character is given by:
				 *
				 * ChTimeValue = Ticks_per_1s / ( Baudrate / 11 )
				 *             = 11 * Ticks_per_1s / Baudrate
				 *             = 220000 / Baudrate
				 * The reload for t3.5 is 1.5 times this value and similary
				 * for t3.5.
				 */
        usTimerT35_50us = (7UL * 220000UL) / (2UL * ulBaudRate);
    }
    cpad_xMBPortTimersInit((USHORT)usTimerT35_50us);
}

void analysis_protocol(void)
{
    uint8_t cmd;
    uint16_t addr, nreg;
    eMBErrorCode errcode = MB_ENOERR;
    uint16_t sendlen = 0, crc;

    cmd = cpad_slave_inst.rxbuf[1];
    addr = (cpad_slave_inst.rxbuf[2] << 8) + cpad_slave_inst.rxbuf[3];
    nreg = (cpad_slave_inst.rxbuf[4] << 8) + cpad_slave_inst.rxbuf[5];
    if (cmd == CPAD_MB_REG_READ)
    {
        errcode = cpad_eMBRegHoldingCB(&cpad_slave_inst.rxbuf[3], addr, nreg, cmd);
        cpad_slave_inst.rxbuf[2] = nreg * 2;
        sendlen = cpad_slave_inst.rxbuf[2] + 5;
    }
    else if (cmd == CPAD_MB_REG_SINGLE_WRITE) //μ￥D′
    {
        errcode = cpad_eMBRegHoldingCB(&cpad_slave_inst.rxbuf[4], addr, 1, cmd);
        sendlen = 8;
    }
    else if (cmd == CPAD_MB_REG_MULTIPLE_WRITE) //?àD′
    {
        errcode = cpad_eMBRegHoldingCB(&cpad_slave_inst.rxbuf[7], addr, nreg, cmd);
        sendlen = 8;
        rt_kprintf("cmd= %X,errcode= %X\n", cmd, errcode);
    }

    if (errcode == MB_ENOERR)
    {
        ;
    }
    else //erro
    {
        cpad_slave_inst.rxbuf[1] = 0x80 | cpad_slave_inst.rxbuf[1];
        cpad_slave_inst.rxbuf[2] = errcode;
        sendlen = 5;
    }
    //crc
    crc = usMBCRC16(cpad_slave_inst.rxbuf, (sendlen - 2));
    cpad_slave_inst.rxbuf[sendlen - 1] = crc >> 8;
    cpad_slave_inst.rxbuf[sendlen - 2] = crc;
    //send data
    cpad_xMBPortSerialPutByte(cpad_slave_inst.rxbuf, sendlen);
}
void cpad_MBPoll(void)
{
    uint16_t crc;

    if (cpad_slave_inst.rx_flag)
    {
        cpad_slave_inst.rx_flag = 0;
        //update timer
        cpad_vMBPortTimersEnable();
    }

    if ((cpad_slave_inst.rx_ok) || (cpad_slave_inst.rx_timeout))
    {
        cpad_vMBPortTimersDisable();
        if (cpad_slave_inst.rx_timeout)
        {
            cpad_slave_inst.rx_ok = 1;
            cpad_slave_inst.rx_timeout = 0;
            cpad_slave_inst.rec_state = REC_ADDR_STATE;
        }

        //disable timer

        if (cpad_slave_inst.rec_cnt >= MNT_RX_MIN)
        {

            crc = usMBCRC16(cpad_slave_inst.rxbuf, cpad_slave_inst.rec_cnt);
            if (crc == 0)
            {
                analysis_protocol();
            }
            else
            {
                rt_kprintf("MNT:check erro = %d,rec_cnt=%d \n", crc, cpad_slave_inst.rec_cnt);
            }
        }
        //rx enbale
        cpad_slave_inst.rec_cnt = 0;
        cpad_slave_inst.rx_ok = 0;
    }
}

void Analysis_protocol_T5(void)
{
    extern sys_reg_st g_sys;
    uint8_t cmd;
    uint16_t addr, nreg;
    eMBErrorCode errcode = MB_ENOERR;
    uint16 cmd_value;

    cmd = cpad_slave_inst.rxbuf[3];
    addr = (cpad_slave_inst.rxbuf[4] << 8) | cpad_slave_inst.rxbuf[5];
    addr -= (T5_OFFSET - MB_N_OFFSET);
    nreg = cpad_slave_inst.rxbuf[6];
    cmd_value = (cpad_slave_inst.rxbuf[7] << 8) | cpad_slave_inst.rxbuf[8];
    rt_kprintf("cmd = %x,addr = %x,nreg = %d,cmd_value = %d\n", cmd, addr, nreg, cmd_value);
    if (cmd == T5_READ)
    {
        //				cmd_value=(cpad_slave_inst.rxbuf[7]<<8) + cpad_slave_inst.rxbuf[8];
        //				errcode = cpad_eMBRegHoldingCB(&cpad_slave_inst.rxbuf[7],addr,nreg,cmd);
        if (addr == (WATER_MODE_ADDR + CONFIG_REG_MAP_OFFSET))
        {
            g_sys.config.ComPara.u16Water_Mode = cmd_value;
            if (cmd_value != 0)
            {
                if (errcode == MB_ENOERR)
                {
                    //								RAM_Write_Reg(WATER_FLOW_ADDR,0xF000,1);
                    g_sys.config.ComPara.u16Water_Flow = 0xF000;
                }
            }
        }
    }
    else
    {
    }

    if (errcode == MB_ENOERR)
    {
        ;
    }
    else //erro
    {
    }

    return;
}

//数据更新
void Cpad_Update(void)
{
    static uint16_t u16Num[3] = {0};

    if (cpad_slave_inst.rx_flag)
    {
        cpad_slave_inst.rx_flag = 0;
        //update timer
        cpad_vMBPortTimersEnable();
    }

    if ((cpad_slave_inst.rx_ok) || (cpad_slave_inst.rx_timeout))
    {
        cpad_vMBPortTimersDisable();
        if (cpad_slave_inst.rx_timeout)
        {
            cpad_slave_inst.rx_ok = 1;
            cpad_slave_inst.rx_timeout = 0;
            cpad_slave_inst.rec_state = REC_ADDR_STATE;
        }

        //disable timer
        rt_kprintf("rec_state = %d,rx_ok = %d,rec_cnt = %d\n", cpad_slave_inst.rec_state, cpad_slave_inst.rx_ok, cpad_slave_inst.rec_cnt);
        if (cpad_slave_inst.rec_cnt >= MNT_RX_MIN)
        {
            Analysis_protocol_T5();
        }
        //
        cpad_slave_inst.rec_cnt = 0;
        cpad_slave_inst.rx_ok = 0;
    }
    //rx enbale,发送数据
    u16Num[0]++;
    if (u16Num[0] >= 0xFFFF)
    {
        u16Num[0] = 0;
    }
    if (u16Num[0] % 5 == 0)
    {
        Cpad_Send(T5_READ, WATER_MODE_ADDR + CMD_REG_SIZE, 1, T5_OFFSET - MB_N_OFFSET);
        u16Num[1]++;
    }
    else if (u16Num[1] >= 2)
    {
        Cpad_Send(T5_WRITE, ST_HARDWARE + CMD_REG_SIZE, 25, T5_OFFSET - MB_N_OFFSET);
        u16Num[1] = 0;
    }

    return;
}

void Cpad_Send(uint8_t u8RW, uint16_t u16Addr, uint8_t u8Num, uint16_t u16Offset)
{
    uint16_t u16Sendlen = 0;
    uint8_t cmd;
    uint16_t addr, nreg;
    eMBErrorCode errcode = MB_ENOERR;

    cpad_slave_inst.rxbuf[0] = (uint8_t)(T5_HEAD >> 8);
    cpad_slave_inst.rxbuf[1] = (uint8_t)(T5_HEAD);
    if (u8RW == T5_WRITE) //写命令
    {
        cpad_slave_inst.rxbuf[2] = u8Num * 2 + 3;
        cpad_slave_inst.rxbuf[3] = u8RW;
        addr = u16Addr + u16Offset;
        cpad_slave_inst.rxbuf[4] = (uint8_t)(addr >> 8);
        cpad_slave_inst.rxbuf[5] = (uint8_t)(addr);

        cmd = CPAD_MB_REG_READ;
        addr = u16Addr;
        nreg = u8Num;
        errcode = cpad_eMBRegHoldingCB(&cpad_slave_inst.rxbuf[6], addr, nreg, cmd);

        //send data
        u16Sendlen = u8Num * 2 + 3 + 3;
        cpad_xMBPortSerialPutByte(cpad_slave_inst.rxbuf, u16Sendlen);
    }
    else if (u8RW == T5_READ) //读命令
    {
        cpad_slave_inst.rxbuf[2] = u8Num + 3;
        cpad_slave_inst.rxbuf[3] = u8RW;
        addr = u16Addr + u16Offset;
        cpad_slave_inst.rxbuf[4] = (uint8_t)(addr >> 8);
        cpad_slave_inst.rxbuf[5] = (uint8_t)(addr);
        cpad_slave_inst.rxbuf[6] = u8Num;
        //send data
        u16Sendlen = u8Num + 3 + 3;
        cpad_xMBPortSerialPutByte(cpad_slave_inst.rxbuf, u16Sendlen);
    }

    return;
}
