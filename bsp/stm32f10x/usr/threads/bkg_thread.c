#include <rtthread.h>
#include "sys_conf.h"
#include "calc.h"
#include "local_status.h"
#include "team.h"
#include "i2c_bsp.h"
#include "rtc_bsp.h"
#include "event_record.h"
#include "daq.h"
#include "sys_status.h"
#include "watchdog_bsp.h"
#include "password.h"
#include "req_execution.h"
#include "dio_bsp.h"
#include "led_bsp.h"
#include "usart_bsp.h"

static void sys_comp_cooldown(void);
static void run_time_process(void);
// static void team_tab_cooldown(void);
// static void team_timer(void);
// static void check_team_config(void);
static void sys_debug_timeout(void);
// static void analog_dummy_out(void);
// static void ec_fan_diff_req(void);
// static void ec_fan_suc_temp(void);
// extern void work_mode_manage(void);
static void temp_hum_calc(void);
static void test_mode_init_data(void);
static uint16_t Set_Systime(void);

//掉电提示
void power_loss_delay(void)
{
    static uint16_t delay = 0;

    if (delay++ > 30)
    {
        delay = 11;
        sys_option_di_sts(DI_POWER_LOSS_BPOS, 0);
    }
}

/**
  * @brief 	output control module components cooldown 
	* @param  none
	* @retval none
  */
void bkg_thread_entry(void *parameter)
{
    //初始化温湿度曲线记录
    extern sys_reg_st g_sys;
    rt_thread_delay(BKG_THREAD_DELAY);
    init_tem_hum_record();
    watchdog_init();

    while (1)
    {
        led_toggle();
        // team_tab_cooldown();
        // team_timer();
        sys_comp_cooldown();
        run_time_process();
        // check_team_config();
        // analog_dummy_out();
        // ec_fan_diff_req();
        // ec_fan_suc_temp();
        // user_eventlog_add();
        // user_alarmlog_add();
        temp_hum_calc();

        // work_mode_manage();
        // add_hum_temp_log();
        sys_running_mode_update();
        sys_debug_timeout();
        Set_Systime();
        Rtc_sts_update(&g_sys);
        // test mode
        test_mode_init_data();
        // power_loss_delay();

        dog();
        rt_thread_delay(1000);
    }
}

static void temp_hum_calc(void)
{
    extern sys_reg_st g_sys;
    g_sys.status.sys_tem_hum.return_air_hum = get_current_hum();
    g_sys.status.sys_tem_hum.return_air_temp = get_current_temp();

    // rt_kprintf("g_sys.status.sys_tem_hum.return_air_hum = %d\n", g_sys.status.sys_tem_hum.return_air_hum);
    // rt_kprintf("g_sys.status.sys_tem_hum.return_air_temp = %d\n", g_sys.status.sys_tem_hum.return_air_temp);
}

//测试模式
static void test_mode_init_data(void)
{
    extern sys_reg_st g_sys;
    extern local_reg_st l_sys;
    uint8_t u8Temp = 0;
    uint16_t u16W_Flow = 1000;
    static uint8_t u8Test[2] = {0};
    static uint16_t u16Test_UV[3] = {0};

    static uint32_t u32Sterilize_Interval = 0;
    static uint16_t u16Sterilize_Time = 0;

    // TEST mode
    // g_sys.config.ComPara.u16Manual_Test_En = 2;
    // g_sys.config.ComPara.u16Test_Mode_Type = TEST_UR;
    if (g_sys.config.ComPara.u16Manual_Test_En != TEST_MODE_ENABLE)
    {
        g_sys.config.ComPara.u16Test_Mode_Type = TEST_UNABLE;
        return;
    }
    switch (g_sys.config.ComPara.u16Test_Mode_Type)
    {
    case TEST_ALL_OUT: //全开
        g_sys.config.dev_mask.ain = 0xffff;
        g_sys.config.dev_mask.din_bitmap_polarity[0] = 0xffff;
        g_sys.config.dev_mask.din_bitmap_polarity[1] = 0xffff;
        g_sys.config.dev_mask.din[0] = 0xffff;
        g_sys.config.dev_mask.din[1] = 0xffff;
        g_sys.config.dev_mask.aout = 0xffff;
        g_sys.config.dev_mask.mb_comp = 0x01;
        g_sys.config.dev_mask.dout[0] = DO_MASK1;
        g_sys.config.dev_mask.dout[1] = DO_MASK2;

        l_sys.bitmap[0][BITMAP_MANUAL] = DO_MASK1;
        l_sys.bitmap[1][BITMAP_MANUAL] = DO_MASK2;
        l_sys.ao_list[AO_EC_FAN][BITMAP_MANUAL] = 50;
        l_sys.ao_list[AO_EC_COMPRESSOR][BITMAP_MANUAL] = 50;
        l_sys.ao_list[AO_WATER_VALVE][BITMAP_MANUAL] = 50;
        l_sys.ao_list[AO_PREV_1][BITMAP_MANUAL] = 50;
        l_sys.ao_list[AO_PREV_2][BITMAP_MANUAL] = 50;
        break;
    case TEST_PRPDUCE_WATER: //制水
        g_sys.config.dev_mask.ain = 0xffff;
        g_sys.config.dev_mask.din[0] = 0xffff;
        g_sys.config.dev_mask.din[1] = 0xffff;
        g_sys.config.dev_mask.aout = 0xffff;
        g_sys.config.dev_mask.mb_comp = 0x01;
        g_sys.config.dev_mask.dout[0] = DO_MASK1;
        g_sys.config.dev_mask.dout[1] = DO_MASK2;
        l_sys.bitmap[1][BITMAP_MANUAL] = 0x00;
        l_sys.bitmap[1][BITMAP_MANUAL] = DO_POWER_CTR_ONLY;

        l_sys.bitmap[0][BITMAP_MANUAL] = (0x0001 << DO_COMP1_BPOS) | (0x0001 << DO_COMP2_BPOS) | (0x0001 << DO_FAN_BPOS);
        l_sys.ao_list[AO_EC_FAN][BITMAP_MANUAL] = 50;
        break;
    case TEST_PURIFICATION: //净化
        g_sys.config.dev_mask.ain = 0xffff;
        g_sys.config.dev_mask.din[0] = 0xffff;
        g_sys.config.dev_mask.din[1] = 0xffff;
        g_sys.config.dev_mask.aout = 0xffff;
        g_sys.config.dev_mask.mb_comp = 0x01;
        g_sys.config.dev_mask.dout[0] = DO_MASK1;
        g_sys.config.dev_mask.dout[1] = DO_MASK2;
        l_sys.bitmap[1][BITMAP_MANUAL] = DO_POWER_CTR_ONLY;

        u32Sterilize_Interval = (g_sys.config.ComPara.u16Sterilize_Interval[0] * 60);
        if (u32Sterilize_Interval >= (g_sys.config.ComPara.u16Sterilize_Interval[0] * 60)) //开始杀菌
        {
            u16Sterilize_Time++;
            l_sys.bitmap[0][BITMAP_MANUAL] = (0x0001 << DO_WP_BPOS) | (0x0001 << DO_DWP_BPOS) | (0x0001 << DO_UV1_BPOS);
        }
        if (u16Sterilize_Time >= g_sys.config.ComPara.u16Sterilize_Time[0] * 60) //退出杀菌
        {
            u32Sterilize_Interval = 0;
            u16Sterilize_Time = 0;
            l_sys.bitmap[0][BITMAP_MANUAL] = 0;
        }
        break;
    case TEST_NORMAL_WATER: //出常温水
        g_sys.config.dev_mask.ain = 0xffff;
        g_sys.config.dev_mask.din[0] = 0xffff;
        g_sys.config.dev_mask.din[1] = 0xffff;
        g_sys.config.dev_mask.aout = 0xffff;
        g_sys.config.dev_mask.mb_comp = 0x01;
        g_sys.config.dev_mask.dout[0] = DO_MASK1;
        g_sys.config.dev_mask.dout[1] = DO_MASK2;
        l_sys.bitmap[1][BITMAP_MANUAL] = DO_POWER_CTR_ONLY;

        l_sys.bitmap[0][BITMAP_MANUAL] = (0x0001 << DO_UV1_BPOS) | (0x0001 << DO_WP_BPOS) | (0x0001 << DO_EV2_BPOS) | (0x0001 << DO_PWP_BPOS);
        //				l_sys.bitmap[1][BITMAP_MANUAL] = (0x0001<<(DO_DV2_BPOS>>16));
        break;
    case TEST_TANK: //抽空源水箱与饮水箱
        g_sys.config.dev_mask.ain = 0xffff;
        g_sys.config.dev_mask.din[0] = 0xffff;
        g_sys.config.dev_mask.din[1] = 0xffff;
        g_sys.config.dev_mask.aout = 0xffff;
        g_sys.config.dev_mask.mb_comp = 0x01;
        g_sys.config.dev_mask.dout[0] = DO_MASK1;
        g_sys.config.dev_mask.dout[1] = DO_MASK2;
        l_sys.bitmap[1][BITMAP_MANUAL] = DO_POWER_CTR_ONLY;

        l_sys.bitmap[0][BITMAP_MANUAL] = (0x0001 << DO_WP_BPOS) | (0x0001 << DO_EV2_BPOS) | (0x0001 << DO_PWP_BPOS);
        //				l_sys.bitmap[1][BITMAP_MANUAL] = (0x0001<<(DO_DV2_BPOS>>16));
        break;
    case TEST_HEAT_WATER: //出热水
        g_sys.config.dev_mask.ain = 0xffff;
        g_sys.config.dev_mask.din[0] = 0xffff;
        g_sys.config.dev_mask.din[1] = 0xffff;
        g_sys.config.dev_mask.aout = 0xffff;
        g_sys.config.dev_mask.mb_comp = 0x01;
        g_sys.config.dev_mask.dout[0] = DO_MASK1;
        g_sys.config.dev_mask.dout[1] = DO_MASK2;
        l_sys.bitmap[1][BITMAP_MANUAL] = DO_POWER_CTR_ONLY;

        l_sys.bitmap[0][BITMAP_MANUAL] = (0x0001 << DO_RH1_BPOS) | (0x0001 << DO_HWP_BPOS); //加热器电源

        u8Temp = g_sys.config.ComPara.u16HotWater_Temp / 10;
        if (Heat_Send(HEAT_WRITEPARA, OPEN_HEAT, u8Temp, u16W_Flow)) //发送热水命令
        {
        }
        break;
    case TEST_PRPDUCE_COLDWATER: //制冰水
        g_sys.config.dev_mask.ain = 0xffff;
        g_sys.config.dev_mask.din[0] = 0xffff;
        g_sys.config.dev_mask.din[1] = 0xffff;
        g_sys.config.dev_mask.aout = 0xffff;
        g_sys.config.dev_mask.mb_comp = 0x01;
        g_sys.config.dev_mask.dout[0] = DO_MASK1;
        g_sys.config.dev_mask.dout[1] = DO_MASK2;
        l_sys.bitmap[1][BITMAP_MANUAL] = DO_POWER_CTR_ONLY;
#ifdef WV_TEST
        l_sys.bitmap[0][BITMAP_MANUAL] = (0x0001 << DO_COMP1_BPOS) | (0x0001 << DO_COMP2_BPOS) | (0x0001 << DO_FAN_BPOS) | (0x0001 << DO_CV_BPOS);
#else
        l_sys.bitmap[0][BITMAP_MANUAL] = (0x0001 << DO_COMP1_BPOS) | (0x0001 << DO_COMP2_BPOS) | (0x0001 << DO_FAN_BPOS) | (0x0001 << DO_WV_BPOS) | (0x0001 << DO_CV_BPOS);
#endif
        l_sys.ao_list[AO_EC_FAN][BITMAP_MANUAL] = 80;
        break;
    case TEST_Relay: //测试继电器
        u8Test[0]++;
        if (u8Test[0] > TEST_TIME)
        {
            if (u8Test[0] > TEST_TIME * 2)
            {
                u8Test[0] = 0;
            }
            g_sys.config.dev_mask.ain = 0xffff;
            g_sys.config.dev_mask.din[0] = 0xffff;
            g_sys.config.dev_mask.din[1] = 0xffff;
            g_sys.config.dev_mask.aout = 0xffff;
            g_sys.config.dev_mask.mb_comp = 0x01;
            g_sys.config.dev_mask.dout[0] = DO_MASK1;
            g_sys.config.dev_mask.dout[1] = DO_MASK2;

            l_sys.bitmap[0][BITMAP_MANUAL] = DO_MASK1;
            u8Test[1]++;
            if (u8Test[1] > TEST_CICLE * 2)
            {
                u8Test[1] = 0;
            }
            if (u8Test[1] > TEST_CICLE)
            {
                l_sys.bitmap[0][BITMAP_MANUAL] &= ~(0x0001 << DO_FAN_BPOS);
            }
            l_sys.bitmap[1][BITMAP_MANUAL] = DO_MASK2;
            l_sys.ao_list[AO_EC_FAN][BITMAP_MANUAL] = 50;
            l_sys.ao_list[AO_EC_COMPRESSOR][BITMAP_MANUAL] = 50;
            l_sys.ao_list[AO_WATER_VALVE][BITMAP_MANUAL] = 50;
            l_sys.ao_list[AO_PREV_1][BITMAP_MANUAL] = 50;
            l_sys.ao_list[AO_PREV_2][BITMAP_MANUAL] = 50;
        }
        else
        {
            g_sys.config.dev_mask.ain = 0x00;
            g_sys.config.dev_mask.din[0] = 0x00;
            g_sys.config.dev_mask.din[1] = 0x00;
            g_sys.config.dev_mask.aout = 0x00;
            g_sys.config.dev_mask.mb_comp = 0x00;
            g_sys.config.dev_mask.dout[0] = DO_MASK1;
            g_sys.config.dev_mask.dout[1] = DO_MASK2;
            l_sys.bitmap[1][BITMAP_MANUAL] = 0x00;

            l_sys.bitmap[0][BITMAP_MANUAL] = 0x00;
            if (u8Test[1] <= TEST_CICLE)
            {
                l_sys.bitmap[0][BITMAP_MANUAL] |= (0x0001 << DO_FAN_BPOS);
            }
            l_sys.ao_list[AO_EC_FAN][BITMAP_MANUAL] = 0x00;
            l_sys.ao_list[AO_EC_COMPRESSOR][BITMAP_MANUAL] = 0x00;
            l_sys.ao_list[AO_WATER_VALVE][BITMAP_MANUAL] = 0x00;
            l_sys.ao_list[AO_PREV_1][BITMAP_MANUAL] = 0x00;
            l_sys.ao_list[AO_PREV_2][BITMAP_MANUAL] = 0x00;

            u8Temp = g_sys.config.ComPara.u16NormalWater_Temp / 10;
            if (Heat_Send(HEAT_WRITEPARA, CLOSE_HEAT, u8Temp, g_sys.config.ComPara.u16Water_Flow)) //退出
            {
            }
            u32Sterilize_Interval = 0;
            u16Sterilize_Time = 0;
        }

        break;
    case TEST_UV: //UV测试
        u16Test_UV[2]++;
        if (u16Test_UV[2] > (TEST_UV1_OPEN + TEST_UV1_CLOSE))
        {
            u16Test_UV[2] = 0;
        }
        g_sys.config.dev_mask.dout[0] = DO_MASK1;
        g_sys.config.dev_mask.dout[1] = DO_MASK2;

        l_sys.bitmap[0][BITMAP_MANUAL] = (0x0001 << DO_EV2_BPOS);
        l_sys.bitmap[1][BITMAP_MANUAL] = DO_POWER_CTR_ONLY;

        if (u16Test_UV[2] > TEST_UV1_OPEN)
        {
            l_sys.bitmap[0][BITMAP_MANUAL] = 0x00;
            l_sys.bitmap[0][BITMAP_MANUAL] |= (0x0001 << DO_EV2_BPOS);
        }
        else
        {
            l_sys.bitmap[0][BITMAP_MANUAL] &= ~(0x0001 << DO_EV2_BPOS);
        }
        break;
    case TEST_OUTWATER: //出水阀测试
        u16Test_UV[2]++;
        if (u16Test_UV[2] > (g_sys.config.ComPara.u16TestEV[0] + g_sys.config.ComPara.u16TestEV[1]))
        {
            u16Test_UV[2] = 0;
        }
        g_sys.config.dev_mask.dout[0] = DO_MASK1;
        g_sys.config.dev_mask.dout[1] = DO_MASK2;

        l_sys.bitmap[0][BITMAP_MANUAL] = (0x0001 << DO_EV2_BPOS);
        l_sys.bitmap[1][BITMAP_MANUAL] = DO_POWER_CTR_ONLY;

        if (u16Test_UV[2] > g_sys.config.ComPara.u16TestEV[0])
        {
            l_sys.bitmap[0][BITMAP_MANUAL] = 0x00;
            l_sys.bitmap[0][BITMAP_MANUAL] |= (0x0001 << DO_EV2_BPOS);
        }
        else
        {
            l_sys.bitmap[0][BITMAP_MANUAL] &= ~(0x0001 << DO_EV2_BPOS);
        }

        // if (u16Test_UV[0] > TEST_TIME)
        // {
        //     u16Test_UV[0] = 0;
        //     g_sys.config.dev_mask.dout[0] = DO_MASK1;
        //     g_sys.config.dev_mask.dout[1] = DO_MASK2;

        //     l_sys.bitmap[0][BITMAP_MANUAL] = (0x0001 << DO_EV2_BPOS);
        //     u16Test_UV[2]++;
        //     if (u16Test_UV[2] > (g_sys.config.ComPara.u16TestEV[0] + g_sys.config.ComPara.u16TestEV[1]))
        //     {
        //         u16Test_UV[2] = 0;
        //     }
        //     if (u16Test_UV[2] > g_sys.config.ComPara.u16TestEV[0])
        //     {
        //         l_sys.bitmap[0][BITMAP_MANUAL] &= ~(0x0001 << DO_EV2_BPOS);
        //     }
        // }
        // else
        // {
        //     g_sys.config.dev_mask.dout[0] = DO_MASK1;
        //     g_sys.config.dev_mask.dout[1] = DO_MASK2;
        //     l_sys.bitmap[1][BITMAP_MANUAL] = DO_POWER_CTR_ONLY;

        //     l_sys.bitmap[0][BITMAP_MANUAL] = 0x00;
        //     if (u16Test_UV[2] <= g_sys.config.ComPara.u16TestEV[0])
        //     {
        //         l_sys.bitmap[0][BITMAP_MANUAL] |= (0x0001 << DO_EV2_BPOS);
        //     }
        // }
        break;
    default:
        u8Test[0] = 0;
        u8Test[1] = 0;
        g_sys.config.ComPara.u16Test_Mode_Type = TEST_UNABLE;
        g_sys.config.dev_mask.ain = 0x00;
        g_sys.config.dev_mask.din[0] = 0x00;
        g_sys.config.dev_mask.din[1] = 0x00;
        g_sys.config.dev_mask.aout = 0x00;
        g_sys.config.dev_mask.mb_comp = 0x00;
        g_sys.config.dev_mask.dout[0] = DO_MASK1;
        g_sys.config.dev_mask.dout[1] = DO_MASK2;
        l_sys.bitmap[1][BITMAP_MANUAL] = DO_POWER_CTR_ONLY;

        l_sys.bitmap[0][BITMAP_MANUAL] = 0x00;
        l_sys.ao_list[AO_EC_FAN][BITMAP_MANUAL] = 0x00;
        l_sys.ao_list[AO_EC_COMPRESSOR][BITMAP_MANUAL] = 0x00;
        l_sys.ao_list[AO_WATER_VALVE][BITMAP_MANUAL] = 0x00;
        l_sys.ao_list[AO_PREV_1][BITMAP_MANUAL] = 0x00;
        l_sys.ao_list[AO_PREV_2][BITMAP_MANUAL] = 0x00;

        u8Temp = g_sys.config.ComPara.u16NormalWater_Temp / 10;
        if (Heat_Send(HEAT_WRITEPARA, CLOSE_HEAT, u8Temp, g_sys.config.ComPara.u16Water_Flow)) //退出
        {
        }
        u32Sterilize_Interval = 0;
        u16Sterilize_Time = 0;
        break;
    }
    return;
}

/**
  * @brief 	system components runtime counter 
	* @param  none
	* @retval none
  */
//运行时间计算
static void time_calc(uint16_t *sec, uint16_t *h)
{
    uint16_t second;
    uint16_t hour;

    second = *sec;
    hour = *h;
    if ((second & 0x0fff) >= 3600)
    {
        second = 0;
        if (second == 0)
        {
            hour++;
            *h = hour;
        }
        *sec = second;
    }
}

//流量计算
static void Flow_calc(uint16_t *u16ML, uint16_t *u16L)
{
    uint16_t ML;
    uint16_t L;
    uint16_t nL;

    ML = *u16ML;
    L = *u16L;
    if (ML >= 1000)
    {
        nL = ML / 1000;
        L += nL;
        *u16L = L;
        ML %= 1000;
        *u16ML = ML;
    }

    return;
}

//运行时间
static void run_time_process(void)
{
    extern sys_reg_st g_sys;
    extern local_reg_st l_sys;
    //		time_t now;
    uint16_t i;
    static uint16_t u16Sec = 0;

    for (i = 0; i < DO_FILLTER_DUMMY_BPOS; i++)
    {
        if (i >= 16) //大于16
        {
            if ((g_sys.status.dout_bitmap[1] & (0x0001 << (i - 16))) != 0)
            {
                g_sys.status.ComSta.u16Runtime[0][i]++;
                time_calc(&g_sys.status.ComSta.u16Runtime[0][i], &g_sys.status.ComSta.u16Runtime[1][i]);
            }
        }
        else
        {
            if ((g_sys.status.dout_bitmap[0] & (0x0001 << i)) != 0)
            {
                g_sys.status.ComSta.u16Runtime[0][i]++;
                time_calc(&g_sys.status.ComSta.u16Runtime[0][i], &g_sys.status.ComSta.u16Runtime[1][i]);
            }
        }
    }
    //过滤网运行时间累计
    if ((g_sys.status.dout_bitmap[0] & (0x0001 << DO_FAN_BPOS)) != 0)
    {
        g_sys.status.ComSta.u16Runtime[0][DO_FILLTER_DUMMY_BPOS]++;
        time_calc(&g_sys.status.ComSta.u16Runtime[0][DO_FILLTER_DUMMY_BPOS], &g_sys.status.ComSta.u16Runtime[1][DO_FILLTER_DUMMY_BPOS]);
    }
    //滤芯运行时间累计
    if (g_sys.config.ComPara.u16FILTER_ELEMENT_Type != 0) //时间计算
    {
        if ((sys_get_remap_status(WORK_MODE_STS_REG_NO, OUTWATER_STS_BPOS) == TRUE)) //Water out
        {
            for (i = DO_FILLTER_ELEMENT_DUMMY_BPOS_0; i <= DO_FILLTER_ELEMENT_DUMMY_BPOS_5; i++)
            {
                g_sys.status.ComSta.u16Runtime[0][i]++;
                time_calc(&g_sys.status.ComSta.u16Runtime[0][i], &g_sys.status.ComSta.u16Runtime[1][i]);
            }
        }
    }
    else //流量计算
    {
        if (l_sys.OutWater_OK == WATER_OUT)
        {
            l_sys.OutWater_OK = HEATER_IDLE;
            for (i = DO_FILLTER_ELEMENT_DUMMY_BPOS_0; i <= DO_FILLTER_ELEMENT_DUMMY_BPOS_5; i++)
            {
                //								g_sys.status.ComSta.u16Runtime[0][i]+=g_sys.status.ComSta.u16Cumulative_Water[0];
                g_sys.status.ComSta.u16Runtime[0][i] += g_sys.status.ComSta.u16Last_Water;
                Flow_calc(&g_sys.status.ComSta.u16Runtime[0][i], &g_sys.status.ComSta.u16Runtime[1][i]);
            }
        }
    }

    // get_local_time(&now);
    // if ((now % FIXED_SAVETIME) == 0) //每15分钟保存一次
    u16Sec++;
    // rt_kprintf("adc_value=%d\n", u16Sec);
    if ((u16Sec % FIXED_SAVETIME) == 0) //每15分钟保存一次
    {
        u16Sec = 0;
        I2C_EE_BufWrite((uint8_t *)&g_sys.status.ComSta.u16Runtime, STS_REG_EE1_ADDR, sizeof(g_sys.status.ComSta.u16Runtime)); //when, fan is working update eeprom every minite
        //累计流量
        I2C_EE_BufWrite((uint8_t *)&g_sys.status.ComSta.u16Cumulative_Water[0], STS_REG_EE1_ADDR + sizeof(g_sys.status.ComSta.u16Runtime), 4); //u16Cumulative_Water,
    }
    return;
}

static void sys_comp_cooldown(void)
{
    extern local_reg_st l_sys;
    uint16_t i;

    for (i = 0; i < DO_MAX_CNT; i++)
    {
        if (l_sys.comp_timeout[i] > 0)
        {
            l_sys.comp_timeout[i]--;
        }
        else
        {
            l_sys.comp_timeout[i] = 0;
        }
    }

    if (l_sys.comp_startup_interval > 0)
    {
        l_sys.comp_startup_interval--;
    }
    else
    {
        l_sys.comp_startup_interval = 0;
    }

    if (l_sys.comp_stop_interval > 0)
    {
        l_sys.comp_stop_interval--;
    }
    else
    {
        l_sys.comp_stop_interval = 0;
    }

    if (l_sys.TH_Check_Delay > 0)
    {
        l_sys.TH_Check_Delay--;
        l_sys.TH_Check_Interval = 0;
    }
    else
    {
        l_sys.TH_Check_Delay = 0;
    }

    if (l_sys.TH_Check_Interval > 0)
    {
        l_sys.TH_Check_Interval--;
    }
    else
    {
        l_sys.TH_Check_Interval = 0;
    }

    if (l_sys.Set_Systime_Delay > 0)
    {
        l_sys.Set_Systime_Delay--;
    }
    else
    {
        l_sys.Set_Systime_Delay = 0;
    }

    //童锁
    if ((sys_get_di_sts(DI_Child_BPOS) == 1))
    {
        l_sys.ChildLock_Cnt[0]++;
    }
    else
    {
        l_sys.ChildLock_Cnt[0] = 0;
    }
    //童锁指示
    if (l_sys.ChildLock_Cnt[1])
    {
        l_sys.ChildLock_Cnt[1]--;
    }
    else
    {
        l_sys.ChildLock_Cnt[1] = 0;
    }

    //出水延时
    if (l_sys.OutWater_Delay[0])
    {
        l_sys.OutWater_Delay[0]--;
    }
    else
    {
        l_sys.OutWater_Delay[0] = 0;
    }

    //出水延时
    if (l_sys.OutWater_Delay[1])
    {
        l_sys.OutWater_Delay[1]--;
    }
    else
    {
        l_sys.OutWater_Delay[1] = 0;
    }

    if (l_sys.u16BD_Time)
    {
        l_sys.u16BD_Time--;
    }
    if (l_sys.u16BD_FAN_Delay)
    {
        l_sys.u16BD_FAN_Delay--;
    }

    //净化泵打开时间
    if (sys_get_do_sts(DO_PWP_BPOS) == 0)
    {
        l_sys.Pwp_Open_Time = 0;
    }
    else
    {
        l_sys.Pwp_Open_Time++;
    }

    //UV延迟
    if (l_sys.u16UV_Delay)
    {
        l_sys.u16UV_Delay--;
    }

    return;
}

//通信设置系统时间
static uint16_t Set_Systime(void)
{
    extern local_reg_st l_sys;
    extern sys_reg_st g_sys;
    time_t now;
    rt_device_t device;

    if (l_sys.Set_Systime_Delay == 0)
    {
        l_sys.Set_Systime_Flag = 0;
    }
    if (l_sys.Set_Systime_Flag == 0x03)
    {
        now = g_sys.config.ComPara.u16Set_Time[1];
        now = (now << 16) | g_sys.config.ComPara.u16Set_Time[0];

        device = rt_device_find("rtc");

        if (device != RT_NULL)
        {
            rt_rtc_control(device, RT_DEVICE_CTRL_RTC_SET_TIME, &now);
            return 1;
        }
    }
    return 0;
}

static void sys_debug_timeout(void)
{
    extern local_reg_st l_sys;
    if (l_sys.debug_tiemout == DEBUG_TIMEOUT_NA)
    {
        return;
    }
    else if (l_sys.debug_tiemout > 0)
    {
        l_sys.debug_tiemout--;
    }
    else
    {
        l_sys.debug_flag = DEBUG_OFF_FLAG;
        l_sys.debug_tiemout = 0;
    }
}
