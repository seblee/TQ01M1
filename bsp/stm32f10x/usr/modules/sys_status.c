#include <rtthread.h>
#include "team.h"
#include "sys_conf.h"
#include "local_status.h"
#include "req_execution.h"
#include "dio_bsp.h"
#include "password.h"
#include "global_var.h"

void sys_set_remap_status(uint8_t reg_no, uint8_t sbit_pos, uint8_t bit_action)
{
    extern sys_reg_st g_sys;
    if (bit_action == 1)
    {
        g_sys.status.ComSta.u16Status_remap[reg_no] |= (0x0001 << sbit_pos);
    }
    else
    {
        g_sys.status.ComSta.u16Status_remap[reg_no] &= ~(0x0001 << sbit_pos);
    }
}

uint16_t sys_get_remap_status(uint8_t reg_no, uint8_t rbit_pos)
{
    extern sys_reg_st g_sys;
    //		return ((g_sys.status.status_remap[reg_no] >> rbit_pos) & 0x0001);
    return ((g_sys.status.ComSta.u16Status_remap[reg_no] >> rbit_pos) & 0x0001);
}

uint8_t sys_get_di_sts(uint8_t din_channel)
{
    uint8_t byte_offset, bit_offset;
    extern sys_reg_st g_sys;

    byte_offset = din_channel >> 4;
    bit_offset = din_channel & 0x0f;
    //		if((g_sys.status.din_bitmap[byte_offset]>>bit_offset) & 0X0001)
    if ((g_sys.status.ComSta.u16Din_bitmap[byte_offset] >> bit_offset) & 0X0001)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//数字输入状态
void sys_option_di_sts(uint8_t din_channel, uint8_t option)
{
    uint8_t byte_offset, bit_offset;
    extern sys_reg_st g_sys;

    byte_offset = din_channel >> 4;
    bit_offset = din_channel & 0x0f;

    if (option)
    {
        g_sys.status.din_bitmap[byte_offset] |= (0x0001 << bit_offset);
    }
    else
    {
        g_sys.status.din_bitmap[byte_offset] &= (~(0x0001 << bit_offset));
    }
}
uint8_t sys_get_do_sts(uint8_t dout_channel)
{
    extern sys_reg_st g_sys;
    uint8_t byte_offset, bit_offset;
    extern sys_reg_st g_sys;

    byte_offset = dout_channel >> 4;
    bit_offset = dout_channel & 0x0f;
    if ((g_sys.status.ComSta.u16Dout_bitmap[byte_offset] >> bit_offset) & 0X0001)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint16_t sys_get_pwr_signal(void)
{
    extern sys_reg_st g_sys;
    uint16_t u16TPower;
    uint16_t u16Systime;
    uint8_t ret;

    //		if((get_work_mode_power_state() ==0)||
    //				(g_sys.config.general.power_mode == 0))
    if (g_sys.config.ComPara.u16Power_Mode == 0)
    {
        //定时开机
        if ((g_sys.config.ComPara.u16TPower_En) && (g_sys.config.ComPara.u16TPower_On != g_sys.config.ComPara.u16TPower_Off))
        {
            u16TPower = g_sys.config.ComPara.u16TPower_On;
            u16Systime = g_sys.status.ComSta.Sys_Time.Hour;
            u16Systime = (u16Systime << 8) | (g_sys.status.ComSta.Sys_Time.Min);

            if (u16Systime == u16TPower)
            {
                g_sys.config.ComPara.u16Power_Mode = 1;
                write_reg_map(POWER_ON_ADDR, g_sys.config.ComPara.u16Power_Mode);
            }
        }
        ret = 0;
    }
    else
    {
        //定时关机
        if ((g_sys.config.ComPara.u16TPower_En) && (g_sys.config.ComPara.u16TPower_On != g_sys.config.ComPara.u16TPower_Off))
        {
            u16TPower = g_sys.config.ComPara.u16TPower_Off;
            u16Systime = g_sys.status.ComSta.Sys_Time.Hour;
            u16Systime = (u16Systime << 8) | (g_sys.status.ComSta.Sys_Time.Min);

            if (u16Systime == u16TPower)
            {
                g_sys.config.ComPara.u16Power_Mode = 0;
                write_reg_map(POWER_ON_ADDR, g_sys.config.ComPara.u16Power_Mode);
            }
        }
        ret = 1;
        //贮存中
        if (Sys_Get_Storage_Signal() == TRUE)
        {
            ret = 0;
        }
    }
    return ret;
}

void sys_running_mode_update(void)
{
    extern sys_reg_st g_sys;
    extern local_reg_st l_sys;
    //FAN STATUS UPDATE

    if (sys_get_pwr_signal() == 1) //开关机信号
    {
        sys_set_remap_status(WORK_MODE_STS_REG_NO, PWR_STS_BPOS, 1);
    }
    else
    {
        sys_set_remap_status(WORK_MODE_STS_REG_NO, PWR_STS_BPOS, 0);
    }

    //		g_sys.status.general.running_mode = (g_sys.config.ComPara.u16Test_Mode_En<<2)|(g_sys.config.ComPara.u16Manual_Mode_En<<1)|(g_sys.config.general.alarm_bypass_en<<0);

    if (sys_get_do_sts(DO_FAN_BPOS) == 1)
    {
        sys_set_remap_status(WORK_MODE_STS_REG_NO, FAN_STS_BPOS, 1);
    }
    else
    {
        sys_set_remap_status(WORK_MODE_STS_REG_NO, FAN_STS_BPOS, 0);
    }

    //set cooling status
    if ((sys_get_do_sts(DO_COMP1_BPOS) == 1) || (sys_get_do_sts(DO_COMP2_BPOS) == 1))
    {
        sys_set_remap_status(WORK_MODE_STS_REG_NO, COOLING_STS_BPOS, 1);
    }
    else
    {
        sys_set_remap_status(WORK_MODE_STS_REG_NO, COOLING_STS_BPOS, 0);
    }

    // rt_kprintf("DO_EV2_BPOS=%d,DO_RH1_BPOS=%d,OutWater_OK=%d,u8HeatNum=%d,u16Cur_Water=%d\n", sys_get_do_sts(DO_EV2_BPOS), l_sys.comp_timeout[DO_RH1_BPOS]);
    //set Outwater status
    // if ((sys_get_do_sts(DO_EV2_BPOS) == 1) || (l_sys.comp_timeout[DO_RH1_BPOS] > 0))
    if (l_sys.OutWater_Flag == TRUE)
    {
        sys_set_remap_status(WORK_MODE_STS_REG_NO, OUTWATER_STS_BPOS, 1);
    }
    else
    {
        sys_set_remap_status(WORK_MODE_STS_REG_NO, OUTWATER_STS_BPOS, 0);
    }
    //外接水源
    if (sys_get_do_sts(DO_FV_BPOS) == 1)
    {
        sys_set_remap_status(WORK_MODE_STS_REG_NO, EXITWATER_STS_BPOS, 1);
    }
    else
    {
        sys_set_remap_status(WORK_MODE_STS_REG_NO, EXITWATER_STS_BPOS, 0);
    }
    return;
}

uint16_t sys_get_pwr_sts(void)
{
    extern sys_reg_st g_sys;
    //		if((g_sys.status.status_remap[WORK_MODE_STS_REG_NO]>>PWR_STS_BPOS) & 0X0001)
    if ((g_sys.status.ComSta.u16Status_remap[WORK_MODE_STS_REG_NO] >> PWR_STS_BPOS) & 0X0001)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
uint8_t sys_get_mbm_online(uint8_t mbm_dev)
{
    extern sys_reg_st g_sys;
    if ((g_sys.status.status_remap[MBM_COM_STS_REG_NO]) & (0X01 << mbm_dev))
    {
        return (1);
    }
    else
    {
        return (0);
    }
}

uint16_t devinfo_get_compressor_cnt(void)
{
    extern sys_reg_st g_sys;
    uint16_t compressor_bit_map;
    uint16_t compressor_count;

    compressor_bit_map = (((g_sys.config.dev_mask.dout[0] >> DO_COMP2_BPOS) & 0x0001) << 1);
    compressor_bit_map |= ((g_sys.config.dev_mask.dout[0] >> DO_COMP1_BPOS) & 0x0001);

    switch (compressor_bit_map)
    {
    case (0):
    {
        compressor_count = 0;
        break;
    }
    case (1):
    {
        compressor_count = 1;
        break;
    }
    case (3):
    {
        compressor_count = 2;
        break;
    }
    default:
    {
        compressor_count = 0;
        break;
    }
    }
    compressor_count = 2;
    return compressor_count;
}

//浮球水位
uint16_t Get_Water_level(void)
{
    extern sys_reg_st g_sys;
    uint16_t u16Water_level = 0;

    //极性反转
    //S_L
    if (sys_get_di_sts(DI_SOURCE_DOWN_BPOS) == 0)
    {
        u16Water_level |= S_L;
    }
    else
    {
        u16Water_level &= ~S_L;
    }

    //S_M
    if (sys_get_di_sts(DI_SOURCE_MIDDLE_BPOS) == 0)
    {
        u16Water_level |= S_M;
    }
    else
    {
        u16Water_level &= ~S_M;
    }
    if (!(g_sys.config.dev_mask.din[0] & S_M))
    {
        u16Water_level &= ~S_M;
    }

    //S_U
    if (sys_get_di_sts(DI_SOURCE_UP_BPOS) == 0)
    {
        u16Water_level |= S_U;
    }
    else
    {
        u16Water_level &= ~S_U;
    }

    //D_L
    if (sys_get_di_sts(DI_DRINK_DOWN_BPOS) == 0)
    {
        u16Water_level |= D_L;
    }
    else
    {
        u16Water_level &= ~D_L;
    }

    //D_M
    if (sys_get_di_sts(DI_DRINK_MIDDLE_BPOS) == 0)
    {
        u16Water_level |= D_M;
    }
    else
    {
        u16Water_level &= ~D_M;
    }

    //D_U
    if (sys_get_di_sts(DI_DRINK_UP_BPOS) == 0)
    {
        u16Water_level |= D_U;
    }
    else
    {
        u16Water_level &= ~D_U;
    }

    //D_ML
    if (sys_get_di_sts(DI_DRINK_MD_BPOS) == 0)
    {
        u16Water_level |= D_ML;
    }
    else
    {
        u16Water_level &= ~D_ML;
    }

    g_sys.status.ComSta.u16WL = u16Water_level;
    return u16Water_level;
}
