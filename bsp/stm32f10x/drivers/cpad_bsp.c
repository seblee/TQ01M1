#include <rtthread.h>
#include <components.h>
#include "stm32f10x.h"
#include "kits/fifo.h"
#include "global_var.h"
#include "sys_def.h"
#include "rtc_bsp.h"
#include "authentication.h"
#include <time.h>
#include "event_record.h"
#include "password.h"
#include "mb_event_cpad.h"
#include "can_bsp.h"
#include "sys_status.h"
#define CPAD_RS485_SND_MODE GPIO_SetBits(GPIOA, GPIO_Pin_1)
#define CPAD_RS485_RCV_MODE GPIO_ResetBits(GPIOA, GPIO_Pin_1)
#define RECORD_CMD_FLAG 10000
#define CPAD_TX_BUF_DEPTH 256
#define CPAD_RX_BUF_DEPTH 256
#define CPAD_FSM_TIMEOUT 2

#define FRAME_CMD_POS 2
#define FRAME_LEN_POS 3

#define FRAME_DATA_0 4
#define FRAME_DATA_1 5
#define FRAME_DATA_2 6
#define FRAME_DATA_3 7
#define FRAME_DATA_4 8
#define FRAME_DATA_5 9

#define RECORD_START_POS_H 4

#define RECORD_START_POS_L 5

#define READ_COUNT_POS 6

#define TEM_HUM_TYPE_POS 7

#define CPAD_FRAME_TAG_RX_SYNC1 0x1b
#define CPAD_FRAME_TAG_RX_SYNC2 0xdf

#define CPAD_FRAME_TAG_TX_SYNC1 0x9b
#define CPAD_FRAME_TAG_TX_SYNC2 0xdf

#define CPAD_FRAME_FSM_SYNC1 0x01
#define CPAD_FRAME_FSM_SYNC2 0x02
#define CPAD_FRAME_FSM_CMD 0x04
#define CPAD_FRAME_FSM_LEN 0x08
#define CPAD_FRAME_FSM_DATA 0x10

#define CPAD_CMD_RD_REG 0x01
#define CPAD_CMD_WR_REG 0x02

#define CPAD_CMD_CONF_SAVE_OPTION 0x10
#define CPAD_CMD_CONF_LOAD_OPTION 0x11
#define CPAD_CMD_SET_TIME 0x12
#define CPAD_CMD_REQ_TIME 0x13
#define CPAD_CMD_REQ_PERM 0x14
#define CPAD_CMD_CHANGE_PWD 0x15
#define CPAD_CMD_READ_CURRENT_ALARAM 0x16
#define CPAD_CMD_REDA_ALARM_RECORD 0x17
#define CPAD_CMD_EVENT_RECORD 0x18
#define CPAD_CMD_CLEAR_RECORD 0x19
#define CPAD_CMD_READ_TEM_HUM 0x1A
#define CPAD_CMD_CLEAR_RUN_TIME 0x1B
#define CPAD_CMD_CLEAR_ALARM 0x1C
#define CPAD_CMD_DEFAULT_PRAM 0x1D
#define CPAD_CMD_PASSWORD_SET 0x1E
#define CPAD_CMD_PASSWORD_PRAM_SET 0x1f
#define CPAD_CMD_CLEAR_ALARM_BEEP 0x21

#define CPAD_CMD_CONTROLPASSWORD_SET 0x22
#define CPAD_CMD_CONTROLPASSWORD_VERIFY 0x23

//static fifo8_cb_td cpad_rx_fifo;
//static fifo8_cb_td cpad_tx_fifo;

typedef struct
{
    volatile uint8_t tx_buf[CPAD_TX_BUF_DEPTH];
    volatile uint8_t tx_cnt;
    volatile uint8_t tx_cmd;
    volatile uint8_t rx_buf[CPAD_RX_BUF_DEPTH];
    volatile uint8_t rx_cnt;
    volatile uint8_t rx_tag;
    volatile uint16_t rtx_timeout;
    volatile uint8_t cpad_fsm_cstate;
} cpad_reg_st;

static cpad_reg_st cpad_reg_inst;

/**
  * @brief  cpad recieve frame finite state machine
	* @param  none
  * @retval 
			`0: cpad frame recieve ok
			`1:	cpad frame recieve ng
  */
uint16_t cpad_get_comm_sts(void)
{
    return cpad_reg_inst.rtx_timeout;
}

uint8_t cpad_get_rx_fsm(void)
{
    return cpad_reg_inst.cpad_fsm_cstate;
}

/**
  * @brief  cpad recieve frame finite state machine
	* @param  none
  * @retval 
			`0: cpad frame recieve ok
			`1:	cpad frame recieve ng
  */
uint16_t cpad_frame_recv(void)
{
    if (cpad_reg_inst.rx_tag == 1) //if there is already an unprocessed frame in the rx buffer, quit new frame recieving
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static uint16_t obcmd_clear_alarm(void)
{
    extern USHORT cpad_usSRegHoldBuf[CMD_REG_SIZE];

    extern sys_reg_st g_sys;

    if (clear_alarm() != 1)
    {

        return 0;
    }
    else
    {
        //sys_set_remap_status(WORK_MODE_STS_REG_NO,ALARM_BEEP_BPOS,0);
        return 1;
    }
}

static uint16_t obcmd_set_time(void)
{
    extern USHORT cpad_usSRegHoldBuf[CMD_REG_SIZE];
    time_t now;
    rt_device_t device;

    now = cpad_usSRegHoldBuf[1];
    now = (now << 16) | cpad_usSRegHoldBuf[2];

    device = rt_device_find("rtc");

    if (device != RT_NULL)
    {
        rt_rtc_control(device, RT_DEVICE_CTRL_RTC_SET_TIME, &now);
        return 1;
    }
    else
    {
        return 0;
    }
}

static uint16_t obcmd_req_time(void)
{
    extern USHORT cpad_usSRegHoldBuf[CMD_REG_SIZE];
    time_t now;

    time(&now);

    cpad_usSRegHoldBuf[1] = (uint16_t)((now >> 16) & 0x0000ffff);
    cpad_usSRegHoldBuf[2] = (uint16_t)(now & 0x0000ffff);

    return 1;
}

static uint16_t obcmd_clear_run_time(void)
{
    extern USHORT cpad_usSRegHoldBuf[CMD_REG_SIZE];

    extern sys_reg_st g_sys;

    if (reset_runtime(cpad_usSRegHoldBuf[1]) != 1)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

static uint16_t obcmd_load_fact(void)
{
    extern USHORT cpad_usSRegHoldBuf[CMD_REG_SIZE];

    extern sys_reg_st g_sys;

    if (load_factory_pram() != 1)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

//保存配置参数
static uint16_t obcmd_save_conf(void)
{
    extern USHORT cpad_usSRegHoldBuf[CMD_REG_SIZE];

    extern sys_reg_st g_sys;

    if (save_conf_reg(cpad_usSRegHoldBuf[1]) != 0)
    {
        return 0;
    }
    else
    {
        rt_thread_delay(1000);
        NVIC_SystemReset(); //主板重启
        return 1;
    }
}

static uint16_t obcmd_load_conf(void)
{
    extern USHORT cpad_usSRegHoldBuf[CMD_REG_SIZE];

    extern sys_reg_st g_sys;

    if (set_load_flag(cpad_usSRegHoldBuf[1]) != 1)
    {
        return 0;
    }
    else
    {
        if (cpad_usSRegHoldBuf[1] != 0)
        {
            NVIC_SystemReset();
        }
        return 1;
    }
}

static uint16_t obcmd_clear_alarm_beep(void)
{

    sys_set_remap_status(WORK_MODE_STS_REG_NO, ALARM_BEEP_BPOS, 0);

    return 1;
}

//modbus cpad protocal resolve
uint16_t cpad_ob_resolve(void)
{
    extern USHORT cpad_usSRegHoldBuf[CMD_REG_SIZE];
    uint8_t err_code;
    uint16_t cmd_type;

    cmd_type = cpad_usSRegHoldBuf[0];
    if (cmd_type != 0)
    {
        rt_kprintf("cmd_type is %x\n", cmd_type);
    }

    err_code = 0;

    if (cmd_type != 0)
    {
        switch (cmd_type)
        {
        case (CPAD_CMD_CLEAR_ALARM):
        {
            err_code = obcmd_clear_alarm();
            break;
        }
        case (CPAD_CMD_SET_TIME):
        {
            err_code = obcmd_set_time();
            break;
        }
        case (CPAD_CMD_REQ_TIME):
        {
            err_code = obcmd_req_time();
            break;
        }
        case (CPAD_CMD_CLEAR_RUN_TIME):
        {
            err_code = obcmd_clear_run_time();
            break;
        }
        case (CPAD_CMD_CLEAR_RECORD):
        {
            err_code = 1;
            break;
        }
        case (CPAD_CMD_DEFAULT_PRAM): //恢复默认参数
        {
            err_code = obcmd_load_fact();
            break;
        }
        case (CPAD_CMD_CONF_SAVE_OPTION):
        {
            err_code = obcmd_save_conf();
            break;
        }
        case (CPAD_CMD_CONF_LOAD_OPTION): //恢复原始参数
        {
            err_code = obcmd_load_conf();
            break;
        }
        case (CPAD_CMD_CLEAR_ALARM_BEEP):
        {
            err_code = obcmd_clear_alarm_beep();
            break;
        }

        default:
        {
            err_code = 1;
            break;
        }
        }
        if (err_code == 0)
        {
            cpad_usSRegHoldBuf[0] = 0;
        }
        else
        {
            cpad_usSRegHoldBuf[0] = 0xffff;
        }
    }
    cpad_usSRegHoldBuf[0] = 0;
    return 1;
}

static void show_cpad_info(void)
{
    rt_kprintf("Cpad rx_tag: %x, rtx_timeout: %d\n", cpad_reg_inst.rx_tag, cpad_reg_inst.rtx_timeout);
}

FINSH_FUNCTION_EXPORT(show_cpad_info, show cpad information.);
