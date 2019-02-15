#include <rtthread.h>
#include "adc_bsp.h"
#include "sys_conf.h"
#include "kits/fifo.h"
#include "dio_bsp.h"
#include "adc_bsp.h"
#include "rtc_bsp.h"
#include "user_mb_app.h"
#include "led_bsp.h"
#include "TH_SENSOR_BSP.h"
#include "Delay.h"

static void daq_gvar_update(void);

/**
  * @brief  get ai and di readings and update global status register periodically
  * @param  none
  * @retval none
**/
void daq_thread_entry(void *parameter)
{
    rt_thread_delay(DAQ_THREAD_DELAY);
    // delay_init();//延时初始化，系统时钟初始化
    while (1)
    {
        daq_gvar_update();
        // rt_thread_delay(1000);
        rt_thread_delay(500);
    }
}

/**
  * @brief  update global variable g_din_inst and g_ain_inst according to di and ai inputs
  * @param  none
  * @retval none
**/
static void daq_gvar_update(void)
{
    extern sys_reg_st g_sys;

    ai_sts_update(&g_sys); //update g_ain_inst
    di_sts_update(&g_sys); //update g_din_inst

    AM_Sensor_update(&g_sys);
    // mbm_sts_update(&g_sys); // update modbus master components status
}
