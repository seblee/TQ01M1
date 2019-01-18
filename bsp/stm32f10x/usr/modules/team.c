//#include <rtthread.h>
//#include <components.h>
//#include "sys_conf.h"
//#include "local_status.h"
//#include "require_calc.h"
//#include "team.h"
//#include "time.h"
//#include "sys_status.h"
//#include "can_bsp.h"
//#include "fifo.h"
//#include "calc.h"
//#include "rtc_bsp.h"
//#include "dio_bsp.h"
//#include "i2c_bsp.h"
//#include "daq.h"

//#define CAN_FRAME_SIZE	64
//#define CAN_BUF_DEPTH	64

//#define RUN_STS_CD	15

//team_local_st team_local_inst;
//static fifo8_cb_td cf_rx_fifo;
//static fifo8_cb_td cf_tx_fifo;

//static uint8_t get_team_ms_sts(void);
//static uint8_t get_team_con_sts(void);

//void can_frame_resolve(void);
//void can_protocal_resolve(void);

//static void team_snd_pkt(void);
//static void team_lc_sts_update(void);
//static uint8_t team_update_onlinestatus(void);
//static uint32_t team_get_offlinestasus(void);
//static uint32_t team_update_confdstatus(void);
//static void team_state_update(void);
//static uint32_t team_get_totalbitmap(uint8_t total_num);
//static uint8_t is_running_dev(uint8_t num);
//static uint8_t team_update_malfun_dev(void);
////static void team_update_output(uint32_t test_dev,uint8_t test_num);
//static void team_update_output(void);
//static void team_update_Final_output(uint8_t temp_cnt, uint8_t temp_flag, uint8_t hum_cnt, uint8_t hum_flag);
//static void work_status_update(void);
//static uint8_t get_bitmap_num(uint32_t bitmap);
//static void cascade_bitmap_update(uint8_t temp_cnt, uint8_t temp_flag, uint8_t hum_cnt, uint8_t hum_flag);
//static void clear_param(void);
//static uint32_t find_bitmap_by_addr(uint8_t addr);
//static uint32_t calc_th_status(void);

//void tcom_thread_entry(void* parameter)
//{
//		extern sys_reg_st		g_sys;
//		rt_thread_delay(TCOM_THREAD_DELAY);
//		team_local_var_init();
//		while(1)
//		{
//        can_frame_resolve();
//        can_protocal_resolve();
//				team_snd_pkt();
//				rt_thread_delay(100);
//		}
//}

//static void team_local_var_reset(void)
//{
//		uint16_t i,j;

//		team_local_inst.team_fsm = TEAM_FSM_IDLE;
//
//		for(i=0;i<TEAM_MAX_SLAVE_NUM;i++)
//		{
//				for(j=0;j<TEAM_TAB_MAX;j++)
//				{
//						team_local_inst.team_table[i][j] = 0;
//				}
//		}
//
//		for(i=0;i<TEAM_CONF_MAX;i++)
//		{
//				team_local_inst.team_config[i] = 0;
//		}
//
//		for(i=0;i<TEAM_PARAM_MAX;i++)
//		{
//				team_local_inst.team_param[i] = 0;
//		}

//		for(i=0;i<TEAM_BITMAP_MAX;i++)
//		{
//				team_local_inst.team_bitmap[i] = 0;
//		}
//
//    for(i = 0; i < TEAM_WORK_STATUS_MAX; i++)
//    {
//        team_local_inst.team_work_status[i] = 0;
//    }
//
//    for(i = 0; i < TEAM_MAX_SLAVE_NUM; i++)
//    {
//        team_local_inst.team_sort_temp[i] = 0;
//        team_local_inst.team_sort_hum[i]  = 0;
//    }
//    team_local_inst.team_config_set_flag    = 0;
//    team_local_inst.team_config_set_flag_1  = 0;
//    team_local_inst.slave_to_master_param_protect = 0;
//
//    for(i = 0; i < 8; i++)
//    {
//        team_local_inst.master_req_distb_buf[i]       = 0;
//        team_local_inst.master_ex_temp_param_buf[i]   = 0;
//        team_local_inst.master_ex_hum_param_buf[i]    = 0;
//        team_local_inst.master_run_final_sts_buf[i]   = 0;
//    }
//
//    team_local_inst.master_send_param_protect = MASTER_SEND_PARAM_PROTECT_CNT;
//    team_local_inst.rotate_timeout            = ROTATE_TIMEOUT_CNT;
//		team_local_inst.run_sts_cd								= 0;
//		team_local_inst.run_sts_flag							= 0;
//		team_local_inst.sync_poweron_timeout 			= SYNC_POWERON_TIMEOUT;
//}

//void team_local_var_init(void)
//{
////		uint8_t Buffer[2];
//
//		team_local_var_reset();
//		I2C_EE_BufRead((uint8_t *)&team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT], STS_REG_EE2_ADDR, sizeof(team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT]));
//		fifo8_init(&cf_rx_fifo,CAN_FRAME_SIZE,CAN_BUF_DEPTH);
//		fifo8_init(&cf_tx_fifo,CAN_FRAME_SIZE,CAN_BUF_DEPTH);
////		Buffer[0]=fifo8_init(&cf_rx_fifo,CAN_FRAME_SIZE,CAN_BUF_DEPTH);
////		Buffer[1]=fifo8_init(&cf_tx_fifo,CAN_FRAME_SIZE,CAN_BUF_DEPTH);
////		rt_kprintf("Buffer[0] = %d,Buffer[1] = %d\n",Buffer[0],Buffer[1]);
//}

//static uint16_t team_check_local_conf_status(void)
//{
//		extern sys_reg_st		g_sys;
//
//		if((team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS]&(0x0001<<TEAM_STS_CONFD_BPOS)) != 0)
//		{
//				return 1;
//		}
//
//		return 0;
//}

//void team_master_distb_frame(void)
//{

//		extern sys_reg_st		g_sys;
//		extern local_reg_st l_sys;
//		uint8_t can_frame_tx_buf[CAN_FRAME_SIZE];
//    uint8_t data_base = 3;
//    uint8_t i;
//
//    //DA
//		can_frame_tx_buf[0]                           = TEAM_BROADCAST_ADDR;
//    //type
//    can_frame_tx_buf[1]                           = TEAM_CMD_MASTER_REQ_DISTB;
//    //Length
//		can_frame_tx_buf[2]                           = TEAM_CMD_LEN_MASTER_REQ_DISTB;
//    //data
////    *(uint16_t *)&can_frame_tx_buf[data_base + 0] = team_local_inst.team_param[TEAM_PARAM_MODE];
////		*(uint16_t *)&can_frame_tx_buf[data_base + 2] = team_local_inst.team_param[TEAM_PARAM_TEMP_REQ];
////		*(uint16_t *)&can_frame_tx_buf[data_base + 4] = team_local_inst.team_param[TEAM_PARAM_HUM_REQ];
//    for(i = 0; i < TEAM_CMD_LEN_MASTER_REQ_DISTB; i++)
//    {
//        can_frame_tx_buf[data_base + i] = team_local_inst.master_req_distb_buf[i];
//    }

//    if(team_local_inst.slave_to_master_param_protect == 0)
//    {
//        fifo8_push(&cf_tx_fifo,can_frame_tx_buf);
//    }
//}

//void team_master_conf_frame(void)
//{
//		extern sys_reg_st		g_sys;
//		extern local_reg_st l_sys;
//		uint8_t can_frame_tx_buf[CAN_FRAME_SIZE];
//    uint8_t i, data_base = 3;
//
//    //DA
//    can_frame_tx_buf[0]                               = TEAM_BROADCAST_ADDR;
//    //type
//    can_frame_tx_buf[1]                               = TEAM_CMD_MASTER_PARAM_SET;
//    //Length
//    can_frame_tx_buf[2]                               = TEAM_CMD_LEN_MASTER_PARAM_SET;
//    //data
//    for(i = 0; i < 8; i++)    //data segment:0 1 2 3 4 5 6 7
//    {
//        can_frame_tx_buf[data_base + 0]               = i;
//        *(uint16_t *)&can_frame_tx_buf[data_base + 1] = team_local_inst.team_config[0 + i * 3];
//        *(uint16_t *)&can_frame_tx_buf[data_base + 3] = team_local_inst.team_config[1 + i * 3];
//        *(uint16_t *)&can_frame_tx_buf[data_base + 5] = team_local_inst.team_config[2 + i * 3];
//
//        fifo8_push(&cf_tx_fifo,can_frame_tx_buf);
//    }
//}

//static void team_run_sts_set_frame(void)
//{
//		extern sys_reg_st		g_sys;
//		uint8_t can_frame_tx_buf[CAN_FRAME_SIZE];
//    uint8_t data_base = 3;
//
//    //DA
//    can_frame_tx_buf[0]                           = TEAM_BROADCAST_ADDR;
//    //type
//    can_frame_tx_buf[1]                           = TEAM_CMD_MASTER_RUN_STS_SET;
//    //Length
//    can_frame_tx_buf[2]                           = TEAM_CMD_LEN_MASTER_RUN_STS_SET;
//    //data
//    *(uint32_t *)&can_frame_tx_buf[data_base + 0] = team_local_inst.team_bitmap[TEAM_BITMAP_RS_SET];

//		fifo8_push(&cf_tx_fifo,can_frame_tx_buf);
//}

//static void team_query_frame(uint8_t da)
//{
//		extern sys_reg_st		g_sys;
//		uint8_t can_frame_tx_buf[CAN_FRAME_SIZE];
//    uint8_t data_base = 3;
//
//    //DA
//    can_frame_tx_buf[0]                           = da;
//    //type
//    can_frame_tx_buf[1]                           = TEAM_CMD_MASTER_STS_QEURY;
//    //Length
//    can_frame_tx_buf[2]                           = TEAM_CMD_LEN_MASTER_STS_QEURY;
//    //data
//    *(uint16_t *)&can_frame_tx_buf[data_base + 0] = team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS];

//		fifo8_push(&cf_tx_fifo,can_frame_tx_buf);
//}

//static void team_query_resp_frame(uint8_t* pt_frame)
//{
//		extern sys_reg_st		g_sys;
//		uint8_t can_frame_tx_buf[CAN_FRAME_SIZE];
//		uint8_t i, da, sa;
//    uint8_t data_base = 3;

//    sa = *(pt_frame + 0);
//    da = *(pt_frame + 1);
//
//		if(da != g_sys.config.team.addr - 1)
//		{
//				return;
//		}
//
//		if((team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS]&(0x0001<<TEAM_STS_CONFD_BPOS)) == 0)
//		{
//				return;
//		}
//
//    //DA
//    can_frame_tx_buf[0]                               = sa;
//    //type
//    can_frame_tx_buf[1]                               = TEAM_CMD_SLAVE_QUERY_RESP;
//    //Length
//    can_frame_tx_buf[2]                               = TEAM_CMD_LEN_SLAVE_CONF_STS;
//    //data
//    for(i = 0; i < 8; i++)    //data segment:0 1 2 3 4 5 6 7
//    {
//        can_frame_tx_buf[data_base + 0]               = i;
//        *(uint16_t *)&can_frame_tx_buf[data_base + 1] = team_local_inst.team_config[0 + i * 3];
//				if(i < 7)
//				{
//						*(uint16_t *)&can_frame_tx_buf[data_base + 3] = team_local_inst.team_config[1 + i * 3];
//						*(uint16_t *)&can_frame_tx_buf[data_base + 5] = team_local_inst.team_config[2 + i * 3];
//				}
//				else
//				{
//						//rotate Bitmap set as a param
////						can_frame_tx_buf[data_base + 0] = 6;
//						*(uint32_t *)&can_frame_tx_buf[data_base + 3] = team_local_inst.team_bitmap[TEAM_BITMAP_RS_SET];
//				}
//        fifo8_push(&cf_tx_fifo,can_frame_tx_buf);
//    }
//
////		fifo8_push(&cf_tx_fifo,can_frame_tx_buf);
//}

//static void team_local_status_frame(void)
//{
//		extern sys_reg_st		g_sys;
//		uint8_t can_frame_tx_buf[CAN_FRAME_SIZE];
//    uint8_t data_base = 3;
//
//    //DA
//    can_frame_tx_buf[0]                           = TEAM_BROADCAST_ADDR;
//    //type
//    can_frame_tx_buf[1]                           = TEAM_CMD_LOCAL_STS;
//    //Length
//    can_frame_tx_buf[2]                           = TEAM_CMD_LEN_SLAVE_LOCAL_STS;
//    //data
//    *(uint16_t *)&can_frame_tx_buf[data_base + 0] = team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS];
//    *(uint16_t *)&can_frame_tx_buf[data_base + 2] = team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_TEMP];
//    *(uint16_t *)&can_frame_tx_buf[data_base + 4] = team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_HUM];
//    *(uint16_t *)&can_frame_tx_buf[data_base + 6] = team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_RESERVE];

//		fifo8_push(&cf_tx_fifo,can_frame_tx_buf);
//
//		team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_MASTER_TIMEOUT] = TEAM_TAB_TIMEOUT_CNT;
//}

//static void team_master_ex_temp_set(void)
//{
//    extern sys_reg_st g_sys;
//    uint8_t can_frame_tx_buf[CAN_FRAME_SIZE];
//    uint8_t data_base = 3;
//    uint8_t i;
//
//    //DA
//    can_frame_tx_buf[0]                           = TEAM_BROADCAST_ADDR;
//    //type
//    can_frame_tx_buf[1]                           = TEAM_CMD_MASTER_EX_TEMP_PARAM_SET;
//    //Length
//    can_frame_tx_buf[2]                           = TEAM_CMD_LEN_MASTER_EX_TEMP_PARAM_SET;
//    //data
////    *(uint16_t *)&can_frame_tx_buf[data_base + 0] = team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ];
////    *(uint32_t *)&can_frame_tx_buf[data_base + 2] = team_local_inst.team_bitmap[TEAM_BITMAP_TEMP_FULL_REQ_OUT];
//
//    for(i = 0; i < TEAM_CMD_LEN_MASTER_EX_TEMP_PARAM_SET; i++)
//    {
//        can_frame_tx_buf[data_base + i] = team_local_inst.master_ex_temp_param_buf[i];
//    }
//
//    if(team_local_inst.slave_to_master_param_protect == 0)
//    {
//        fifo8_push(&cf_tx_fifo,can_frame_tx_buf);
//    }
//}

//static void team_master_ex_hum_set(void)
//{
//    extern sys_reg_st g_sys;
//    uint8_t can_frame_tx_buf[CAN_FRAME_SIZE];
//    uint8_t data_base = 3;
//    uint8_t i;
//
//    //DA
//    can_frame_tx_buf[0]                           = TEAM_BROADCAST_ADDR;
//    //type
//    can_frame_tx_buf[1]                           = TEAM_CMD_MASTER_EX_HUM_PARAM_SET;
//    //Length
//    can_frame_tx_buf[2]                           = TEAM_CMD_LEN_MASTER_EX_HUM_PARAM_SET;
//    //data
////    *(uint16_t *)&can_frame_tx_buf[data_base + 0] = team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ];
////    *(uint32_t *)&can_frame_tx_buf[data_base + 2] = team_local_inst.team_bitmap[TEAM_BITMAP_HUM_FULL_REQ_OUT];
//
//    for(i = 0; i < TEAM_CMD_LEN_MASTER_EX_HUM_PARAM_SET; i++)
//    {
//        can_frame_tx_buf[data_base + i] = team_local_inst.master_ex_hum_param_buf[i];
//    }
//
//    if(team_local_inst.slave_to_master_param_protect == 0)
//    {
//        fifo8_push(&cf_tx_fifo,can_frame_tx_buf);
//    }
//}

//static void team_run_final_sts_set_frame(void)
//{
//		extern sys_reg_st		g_sys;
//		uint8_t can_frame_tx_buf[CAN_FRAME_SIZE];
//    uint8_t data_base = 3;
//    uint8_t i;
//
//    //DA
//    can_frame_tx_buf[0]                           = TEAM_BROADCAST_ADDR;
//    //type
//    can_frame_tx_buf[1]                           = TEAM_CMD_MASTER_RUN_FINAL_STS_SET;
//    //Length
//    can_frame_tx_buf[2]                           = TEAM_CMD_LEN_MASTER_RUN_FINAL_STS_SET;
//    //data
////    *(uint32_t *)&can_frame_tx_buf[data_base + 0] = team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT];
////    *(uint32_t *)&can_frame_tx_buf[data_base + 4] = team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT];
//
//    for(i = 0; i < TEAM_CMD_LEN_MASTER_RUN_FINAL_STS_SET; i++)
//    {
//        can_frame_tx_buf[data_base + i] = team_local_inst.master_run_final_sts_buf[i];
//    }

//    if(team_local_inst.slave_to_master_param_protect == 0)
//    {
//        fifo8_push(&cf_tx_fifo, can_frame_tx_buf);
//    }
//}

//static void team_clear_confd_sts_set(void)
//{
//    extern sys_reg_st   g_sys;
//    uint8_t can_frame_tx_buf[CAN_FRAME_SIZE];
//    uint8_t data_base = 3;
//
//    //DA
//    can_frame_tx_buf[0]                           = TEAM_BROADCAST_ADDR;
//    //type
//    can_frame_tx_buf[1]                           = TEAM_CMD_MASTER_CLEAR_CONFD_STS_SET;
//    //Length
//    can_frame_tx_buf[2]                           = TEAM_CMD_LEN_MASTER_CLEAR_CONFD_STS_SET;
//    //data
//    data_base = data_base;    //add for avoid warning
//
//    fifo8_push(&cf_tx_fifo, can_frame_tx_buf);
//}

//void  team_clear_confd(void)
//{
//    team_clear_confd_sts_set();
//}

//static void team_snd_pkt(void)
//{
//		extern sys_reg_st		g_sys;
//		uint8_t can_tx_buf[CAN_FRAME_SIZE];
//		uint8_t sa, da, type, length;
//		if(is_fifo8_empty(&cf_tx_fifo) == 0)
//		{
//				fifo8_pop(&cf_tx_fifo, (uint8*)&can_tx_buf);
//				sa = g_sys.config.team.addr;
//				da = can_tx_buf[0];
//        type = can_tx_buf[1];
//        length = can_tx_buf[2];
////					rt_kprintf("can_tx_buf[0] =%x ,can_tx_buf[1] =%x ,can_tx_buf[2] = %x\n",can_tx_buf[0],can_tx_buf[1],can_tx_buf[2]);
//				can_send_singleframe(sa, da, type, &can_tx_buf[3], length, 0, 0);
//		}
//}

//void can_frame_resolve(void)
//{
//    extern fifo8_cb_td can_rx_fifo;
//    static uint8_t local_rx_buf[CAN_FRAME_SIZE];
//    uint8_t i;
//    uint16_t l_tout;
//    CanRxMsg can_rx_buf;

//    l_tout = 0;
//    while((is_fifo8_empty(&can_rx_fifo) == 0) && (l_tout < CAN_FRAME_SIZE))
//    {
//        l_tout++;
//        fifo8_pop(&can_rx_fifo, (uint8*)&can_rx_buf);
//        local_rx_buf[0] = (can_rx_buf.ExtId & CANID_SA_BITS) >> 14;
//        local_rx_buf[1] = (can_rx_buf.ExtId & CANID_DA_BITS) >> 6;
//        local_rx_buf[2] = (can_rx_buf.ExtId & CANID_SNF_BITS);
//        local_rx_buf[3] = can_rx_buf.DLC;
//        for(i = 0; i < can_rx_buf.DLC; i++)
//        {
//            local_rx_buf[4 + i] = can_rx_buf.Data[i];
//        }
//        fifo8_push(&cf_rx_fifo, local_rx_buf);
//    }
//}

//void can_protocal_resolve(void)
//{
//    extern sys_reg_st		g_sys;
//    uint8_t rx_buf[CAN_FRAME_SIZE];
//    uint8_t s_addr, d_addr, frame_len;
//    team_cmd_em cmd_type;
//    uint16_t *d16_ptr = NULL;
//    uint32_t *d32_ptr = NULL;
//    uint16_t *d16_ptr_s = NULL;
//    uint32_t *d32_ptr_s = NULL;
//    uint8_t data_segment = 0;
//
//    d16_ptr = (uint16_t *)&rx_buf[4];
//		d32_ptr = (uint32_t *)&rx_buf[4];
//
//    d16_ptr_s = (uint16_t *)&rx_buf[5];
//		d32_ptr_s = (uint32_t *)&rx_buf[5];
//
//    while(is_fifo8_empty(&cf_rx_fifo) == 0)
//    {
//        fifo8_pop(&cf_rx_fifo, rx_buf);
//        s_addr    = rx_buf[0];
//        d_addr    = rx_buf[1];
//        cmd_type  = (team_cmd_em)rx_buf[2];
//        frame_len = rx_buf[3];
//
//        //add for avoid warning
//        d_addr = d_addr;
//        frame_len = frame_len;
//        d32_ptr_s = d32_ptr_s;
////		rt_kprintf("rx_buf[0] =%x ,rx_buf[1] =%x ,rx_buf[2] = %x,rx_buf[3] = %x\n",rx_buf[0],rx_buf[1],rx_buf[2],rx_buf[3]);
//        team_local_inst.team_table[s_addr - 1][TEAM_MASTER_TIMEOUT] = TEAM_TAB_TIMEOUT_CNT;
//        switch(cmd_type)
//        {
//          case (TEAM_CMD_MASTER_REQ_DISTB):
//          {
//              if(team_local_inst.team_fsm == TEAM_FSM_SLAVE)
//              {
//                  team_local_inst.team_param[TEAM_PARAM_MODE]           = *(d16_ptr + 0);
//                  team_local_inst.team_param[TEAM_PARAM_TEMP_REQ]       = *(d16_ptr + 1);
//                  team_local_inst.team_param[TEAM_PARAM_HUM_REQ]        = *(d16_ptr + 2);
//									team_local_inst.team_param[TEAM_PARAM_FAN_REQ]				= *(d16_ptr + 3);
//              }
//              break;
//          }
//          case (TEAM_CMD_MASTER_PARAM_SET):
//          {
//              if(g_sys.config.team.addr != 1)
//              {
//                  data_segment = rx_buf[4];
//                  team_local_inst.team_config[0 + data_segment * 3]     = *(d16_ptr_s + 0);
//                  team_local_inst.team_config[1 + data_segment * 3]     = *(d16_ptr_s + 1);
//                  team_local_inst.team_config[2 + data_segment * 3]     = *(d16_ptr_s + 2);
//                  team_local_inst.team_config_set_flag = team_local_inst.team_config_set_flag | ((0x0001) << data_segment);
//                  if(team_local_inst.team_config_set_flag == 0xff)
//                  {
//                      team_local_inst.team_config_set_flag = 0;
//                      team_local_inst.team_table[g_sys.config.team.addr - 1][TEAM_TAB_STATUS] |= (0x0001 << TEAM_STS_CONFD_BPOS);
//                  }
//              }
//              break;
//          }
//          case (TEAM_CMD_MASTER_RUN_STS_SET):
//          {
//              if(team_local_inst.team_fsm == TEAM_FSM_SLAVE)
//              {
//                  team_local_inst.team_bitmap[TEAM_BITMAP_RS_SET]         = *(d32_ptr + 0);
//              }
//              break;
//          }
//          case (TEAM_CMD_MASTER_STS_QEURY):
//          {
//              team_query_resp_frame(rx_buf);
//              break;
//          }
//          case (TEAM_CMD_SLAVE_QUERY_RESP):
//          {
//              if(g_sys.config.team.addr == d_addr)
//              {
//                  data_segment = rx_buf[4];
////									if(data_segment < 6)
////									{
////											team_local_inst.team_config[0 + data_segment * 3]       = *(d16_ptr_s + 0);
////											team_local_inst.team_config[1 + data_segment * 3]       = *(d16_ptr_s + 1);
////											team_local_inst.team_config[2 + data_segment * 3]       = *(d16_ptr_s + 2);
////									}
////									else
////									{
////											team_local_inst.team_bitmap[TEAM_BITMAP_RS_SET] 				= *(d32_ptr_s + 0);
////									}
//									team_local_inst.team_config[0 + data_segment * 3]       = *(d16_ptr_s + 0);
//									if(data_segment < 7)
//									{
//											team_local_inst.team_config[1 + data_segment * 3]       = *(d16_ptr_s + 1);
//											team_local_inst.team_config[2 + data_segment * 3]       = *(d16_ptr_s + 2);
//									}
//									else
//									{
//											team_local_inst.team_bitmap[TEAM_BITMAP_RS_SET] 				= *((uint32_t *)(d16_ptr_s + 1));//*(d32_ptr_s + 0);
//									}
//                  team_local_inst.team_config_set_flag_1 = team_local_inst.team_config_set_flag_1 | ((0x0001) << data_segment);
//                  if(team_local_inst.team_config_set_flag_1 == 0xff)
//                  {
//                      team_local_inst.team_config_set_flag_1 = 0;
//                      team_local_inst.team_table[g_sys.config.team.addr - 1][TEAM_TAB_STATUS] |= (0x0001 << TEAM_STS_CONFD_BPOS);
//                  }
//              }
//              break;
//          }
//          case (TEAM_CMD_LOCAL_STS):
//          {
//              team_local_inst.team_table[s_addr - 1][TEAM_TAB_STATUS] = *(d16_ptr + 0);
//              team_local_inst.team_table[s_addr - 1][TEAM_TAB_TEMP]   = *(d16_ptr + 1);
//              team_local_inst.team_table[s_addr - 1][TEAM_TAB_HUM]    = *(d16_ptr + 2);
//              team_local_inst.team_table[s_addr - 1][TEAM_TAB_RESERVE]= *(d16_ptr + 3);
//              break;
//          }
//          case  (TEAM_CMD_MASTER_EX_TEMP_PARAM_SET):
//          {
//              if(team_local_inst.team_fsm == TEAM_FSM_SLAVE)
//              {
//                  team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ]        = *(d16_ptr + 0);
//                  team_local_inst.team_bitmap[TEAM_BITMAP_TEMP_FULL_REQ_OUT]  = *(uint32_t *)(d16_ptr + 1);
//              }
//              break;
//          }
//          case  (TEAM_CMD_MASTER_EX_HUM_PARAM_SET):
//          {
//              if(team_local_inst.team_fsm == TEAM_FSM_SLAVE)
//              {
//                  team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ]         = *(d16_ptr + 0);
//                  team_local_inst.team_bitmap[TEAM_BITMAP_HUM_FULL_REQ_OUT]   = *(uint32_t *)(d16_ptr + 1);
//              }
//              break;
//          }
//          case  (TEAM_CMD_MASTER_RUN_FINAL_STS_SET):
//          {
//              if(team_local_inst.team_fsm == TEAM_FSM_SLAVE)
//              {
//                  team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT]             = *(d32_ptr + 0);
//                  team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT]                = *(d32_ptr + 1);
//              }
//              break;
//          }
//          case  (TEAM_CMD_MASTER_CLEAR_CONFD_STS_SET):
//          {
//              if(team_local_inst.team_fsm == TEAM_FSM_SLAVE)
//              {
//                  team_local_inst.team_table[g_sys.config.team.addr - 1][TEAM_TAB_STATUS] &= ~(0x0001 << TEAM_STS_CONFD_BPOS);
//                  rt_kprintf("configured clear\n");
//              }
//              break;
//          }
//          default:
//          {
//              break;
//          }
//        }
//    }
//}

//static uint32_t team_get_peer_rs_sts(void)
//{
//		uint32_t temp;
//		uint16_t i;
//
//		temp = 0;
//		for(i=0;i<(team_local_inst.team_config[TEAM_CONF_CNT]&0x00ff);i++)
//		{
//				if(team_local_inst.team_table[i][TEAM_TAB_TIMEOUT] > 0)
//				{
//						if((team_local_inst.team_table[i][TEAM_TAB_STATUS]&(0x0001<<TEAM_STS_RUN_BPOS)) != 0)
//						{
//								temp = temp | (0x00000001 << i);
//						}
//						else
//						{
//								temp = temp & (~(0x00000001 << i));
//						}
//				}
//				else
//				{
//						temp = temp & (~(0x00000001 << i));
//				}
//		}
//		return temp;
//}

//static uint16_t calc_team_config_check(void)
//{
//    uint8_t i, ret, ret1;
//		uint16_t sum;
//    ret 	= 0;
//		ret1 	= 0;
//		//calc config
//    for(i = 0; i < TEAM_CONF_MAX; i++)
//    {
//        if(i == 0)
//        {
//            team_local_inst.team_config[i] = team_local_inst.team_config[i] & 0x7fff;
//        }
//        ret ^= team_local_inst.team_config[i] & 0xff;
//        ret ^= (team_local_inst.team_config[i] >> 8) & 0xff;
//    }
//		//calc bitmap
//		ret1 ^= team_local_inst.team_bitmap[TEAM_BITMAP_RS_SET] & 0xff;
//		ret1 ^= (team_local_inst.team_bitmap[TEAM_BITMAP_RS_SET] >> 8) & 0xff;
//		ret1 ^= team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] & 0xff;
//		ret1 ^= (team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] >> 8) & 0xff;
//
//		sum = ret | (ret1 << 8);
//    return sum;
//}

//static void team_lc_sts_update(void)
//{
//		extern sys_reg_st		g_sys;
//		extern local_reg_st l_sys;
////		uint16_t target_mode;
//
////		target_mode = (team_local_inst.team_config[TEAM_CONF_MODE] >> TEAM_CONF_TARGET_BPOS) & 0x0003;
//
//		//update team run enable mode configuration
//    if(((team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] >> (g_sys.config.team.addr - 1)) & 0x0001) != 0)
//    {
//        team_local_inst.team_config[TEAM_CONF_MODE] |= (0x0001 << TEAM_CONF_RS_BPOS);
//    }
//    else
//    {
//        team_local_inst.team_config[TEAM_CONF_MODE] &= ~(0x0001 << TEAM_CONF_RS_BPOS);
//    }
//
//		//copy team configuration to status reg
//		team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS] &= 0x03ff;
//		team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS] |= team_local_inst.team_config[TEAM_CONF_MODE] & 0xfc00;
//
//		team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS] &= ~(0x0003<<TEAM_STS_FSM_BPOS);
//		team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS] |= (team_local_inst.team_fsm<<TEAM_STS_FSM_BPOS);
//
//		if((g_sys.status.alarm_status_cnt.pwr_off_alarm > 0))
//		{
//				team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS] |= (0x0001<<TEAM_STS_ALARM_BPOS);
//		}
//		else
//		{
//				team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS] &= ~(0x0001<<TEAM_STS_ALARM_BPOS);
//		}
//
//    //风机状态位，及开关机状态
//		if(sys_get_remap_status(WORK_MODE_STS_REG_NO,FAN_STS_BPOS) != 0)
//		{
//				team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS] |= (0x0001<<TEAM_STS_RUN_BPOS);
//		}
//		else
//		{
//				team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS] &= ~(0x0001<<TEAM_STS_RUN_BPOS);
//		}
//
//    //加热状态位
//    if(sys_get_remap_status(WORK_MODE_STS_REG_NO, HEATING_STS_BPOS) != 0)
//		{
//				team_local_inst.team_table[g_sys.config.team.addr - 1][TEAM_TAB_STATUS] |= (0x0001 << TEAM_STS_HEATING_STS_BPOS);
//		}
//		else
//		{
//				team_local_inst.team_table[g_sys.config.team.addr - 1][TEAM_TAB_STATUS] &= ~(0x0001 << TEAM_STS_HEATING_STS_BPOS);
//		}
//
//    //制冷状态位
//    if(sys_get_remap_status(WORK_MODE_STS_REG_NO, COOLING_STS_BPOS) != 0)
//		{
//				team_local_inst.team_table[g_sys.config.team.addr - 1][TEAM_TAB_STATUS] |= (0x0001 << TEAM_STS_COOLING_STS_BPOS);
//		}
//		else
//		{
//				team_local_inst.team_table[g_sys.config.team.addr - 1][TEAM_TAB_STATUS] &= ~(0x0001 << TEAM_STS_COOLING_STS_BPOS);
//		}
//
//    //加湿状态位
//    if(sys_get_remap_status(WORK_MODE_STS_REG_NO, HUMING_STS_BPOS) != 0)
//		{
//				team_local_inst.team_table[g_sys.config.team.addr - 1][TEAM_TAB_STATUS] |= (0x0001 << TEAM_STS_HUMING_STS_BPOS);
//		}
//		else
//		{
//				team_local_inst.team_table[g_sys.config.team.addr - 1][TEAM_TAB_STATUS] &= ~(0x0001 << TEAM_STS_HUMING_STS_BPOS);
//		}
//
//    //除湿状态位
//    if(sys_get_remap_status(WORK_MODE_STS_REG_NO, DEMHUM_STS_BPOS) != 0)
//		{
//				team_local_inst.team_table[g_sys.config.team.addr - 1][TEAM_TAB_STATUS] |= (0x0001 << TEAM_STS_DEMHUM_STS_BPOS);
//		}
//		else
//		{
//				team_local_inst.team_table[g_sys.config.team.addr - 1][TEAM_TAB_STATUS] &= ~(0x0001 << TEAM_STS_DEMHUM_STS_BPOS);
//		}
//
//    //更新状态位到状态寄存器
//    work_status_update();
//
////		team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_TEMP] = (target_mode == 0)? g_sys.status.sys_tem_hum.return_air_temp : g_sys.status.sys_tem_hum.supply_air_temp;
////		team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_HUM] = (target_mode == 0)? g_sys.status.sys_tem_hum.return_air_hum : g_sys.status.sys_tem_hum.supply_air_hum;
//		*((uint32_t *)&team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_TEMP]) = calc_th_status();
//		team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_RESERVE] = calc_team_config_check();
//
//		team_local_inst.team_bitmap[TEAM_BITMAP_RS_STS] = team_get_peer_rs_sts();
//
//    team_update_onlinestatus();
//    team_update_malfun_dev();
//    team_local_inst.team_bitmap[TEAM_BITMAP_CONFD]  = team_update_confdstatus();
//}

////计算送回风温湿度到一个32位变量中
//static uint32_t calc_th_status(void)
//{
//	/*
//	[0]-[9]:湿度
//	[10]-[20]:温度需求温度
//		[10]:温度需求符号位
//	[21]-[31]:风机需求温度
//		[21]:风机需求符号位
//		*/
//		uint8_t target_mode, temp_calc_mode, team_fan_mode;
//		uint32_t ret = 0;
//		uint32_t ret_0_9, ret_10_20, ret_21_31;
//		target_mode 		= (team_local_inst.team_config[TEAM_CONF_MODE] >> TEAM_CONF_TARGET_BPOS) & 0x0003;
//		temp_calc_mode 	= (team_local_inst.team_config[TEAM_CONF_MODE] >> TEAM_CONF_TCMODE_BPOS) & 0x0001;
//		team_fan_mode		= team_local_inst.team_config[TEAM_CONF_FAN_MODE];
//
//		//湿度计算
//		if(target_mode == TARGET_MODE_REMOTE)
//		{
//				ret_0_9	= g_sys.status.sys_tem_hum.remote_air_hum & 0x3ff;
//		}
//		else
//		{
//				ret_0_9	= g_sys.status.sys_tem_hum.return_air_hum & 0x3ff;
//		}
//		//温度需求
//		if(target_mode == TARGET_MODE_RETURN)
//		{		//return
//				if(temp_calc_mode == MAX_TEMP_MODE)
//				{
//						ret_10_20 = (((g_sys.status.sys_tem_hum.return_air_max_temp & 0x8000) >> 5) |
//												(g_sys.status.sys_tem_hum.return_air_max_temp & 0x3ff)) & 0x7ff;
//				}
//				else
//				{
//						ret_10_20 = (((g_sys.status.sys_tem_hum.return_air_temp & 0x8000) >> 5) |
//												(g_sys.status.sys_tem_hum.return_air_temp & 0x3ff)) & 0x7ff;
//				}
//		}
//		else if(target_mode == TARGET_MODE_SUPPLY)
//		{		//supply
//				if(temp_calc_mode == MAX_TEMP_MODE)
//				{
//						ret_10_20 = (((g_sys.status.sys_tem_hum.supply_air_max_temp & 0x8000) >> 5) |
//												(g_sys.status.sys_tem_hum.supply_air_max_temp & 0x3ff)) & 0x7ff;
//				}
//				else
//				{
//						ret_10_20 = (((g_sys.status.sys_tem_hum.supply_air_temp & 0x8000) >> 5) |
//												(g_sys.status.sys_tem_hum.supply_air_temp & 0x3ff)) & 0x7ff;
//				}
//		}
//		else
//		{		//remote
//				if(temp_calc_mode == MAX_TEMP_MODE)
//				{
//						ret_10_20 = (((g_sys.status.sys_tem_hum.remote_air_max_temp & 0x8000) >> 5) |
//												(g_sys.status.sys_tem_hum.remote_air_max_temp & 0x3ff)) & 0x7ff;
//				}
//				else
//				{
//						ret_10_20 = (((g_sys.status.sys_tem_hum.remote_air_temp & 0x8000) >> 5) |
//												(g_sys.status.sys_tem_hum.remote_air_temp & 0x3ff)) & 0x7ff;
//				}
//		}
//		//风机需求
//		switch(team_fan_mode)
//		{
//				case TEAM_FAN_MODE_AVR_RETURN:
//						ret_21_31 = (((g_sys.status.sys_tem_hum.return_air_temp & 0x8000) >> 5) |
//												(g_sys.status.sys_tem_hum.return_air_temp & 0x3ff)) & 0x7ff;
//						break;
//				case TEAM_FAN_MODE_AVR_SUPPLY:
//						ret_21_31 = (((g_sys.status.sys_tem_hum.supply_air_temp & 0x8000) >> 5) |
//												(g_sys.status.sys_tem_hum.supply_air_temp & 0x3ff)) & 0x7ff;
//						break;
//				case TEAM_FAN_MODE_MAX_RETURN:
//						ret_21_31 = (((g_sys.status.sys_tem_hum.return_air_max_temp & 0x8000) >> 5) |
//												(g_sys.status.sys_tem_hum.return_air_max_temp & 0x3ff)) & 0x7ff;
//						break;
//				case TEAM_FAN_MODE_MAX_SUPPLY:
//						ret_21_31 = (((g_sys.status.sys_tem_hum.supply_air_max_temp & 0x8000) >> 5) |
//												(g_sys.status.sys_tem_hum.supply_air_max_temp & 0x3ff)) & 0x7ff;
//						break;
//				default:
//						ret_21_31 = (((g_sys.status.sys_tem_hum.return_air_temp & 0x8000) >> 5) |
//												(g_sys.status.sys_tem_hum.return_air_temp & 0x3ff)) & 0x7ff;
//						break;
//		}
//
//		ret = (ret_0_9 & 0x3ff) | ((ret_10_20 << 10) & 0x1FFC00) | ((ret_21_31 << 21) & 0xFFE00000);
//		return ret;
//}

////从32位变量中读取出对应参数
//uint16_t team_get_th(uint16_t type, uint32_t th_temp)
//{
//		uint16_t ret;
//
//		switch(type)
//		{
//				case TEAM_HUM_REQ_HUM:
//						ret = th_temp & 0x3ff;
//						break;
//				case TEAM_TEMP_REQ_TEMP:
//						ret = (th_temp >> 10) & 0x7ff;
//						break;
//				case TEAM_FAN_REQ_TEMP:
//						ret = (th_temp >> 21) & 0x7ff;
//						break;
//				default:
//						ret = 0x7fff;
//						break;
//		}
//		return ret;
//}

//static void work_status_update(void)
//{
//    uint16_t work_status;
//    uint8_t i;
//    for(i = 0; i < TEAM_WORK_STATUS_MAX; i++)
//    {
//        work_status = (((team_local_inst.team_table[0 + i * 4][TEAM_TAB_STATUS] & 0x03C0) >> 0x06) << 0) |
//                      (((team_local_inst.team_table[1 + i * 4][TEAM_TAB_STATUS] & 0x03C0) >> 0x06) << 4) |
//                      (((team_local_inst.team_table[2 + i * 4][TEAM_TAB_STATUS] & 0x03C0) >> 0x06) << 8) |
//                      (((team_local_inst.team_table[3 + i * 4][TEAM_TAB_STATUS] & 0x03C0) >> 0x06) << 12);
//        team_local_inst.team_work_status[i] = work_status;
//    }
////		rt_kprintf("((uint16_t *)&team_local_inst.team_bitmap[TEAM_BITMAP_MALFUN_DEV] + 0) = %d\n",((uint16_t *)&team_local_inst.team_bitmap[TEAM_BITMAP_MALFUN_DEV] + 0));
////		rt_kprintf("((uint16_t *)&team_local_inst.team_bitmap[TEAM_BITMAP_MALFUN_DEV] + 1) = %d\n",((uint16_t *)&team_local_inst.team_bitmap[TEAM_BITMAP_MALFUN_DEV] + 1));
////		rt_kprintf("((uint16_t *)&team_local_inst.team_bitmap[TEAM_BITMAP_ONLINE] + 0) = %d\n",((uint16_t *)&team_local_inst.team_bitmap[TEAM_BITMAP_ONLINE] + 0));
////		rt_kprintf("((uint16_t *)&team_local_inst.team_bitmap[TEAM_BITMAP_ONLINE] + 1) = %d\n",((uint16_t *)&team_local_inst.team_bitmap[TEAM_BITMAP_ONLINE] + 1));
////		rt_kprintf("((uint16_t *)&team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] + 0) = %d\n",((uint16_t *)&team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] + 0));
////		rt_kprintf("((uint16_t *)&team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] + 1) = %d\n",((uint16_t *)&team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] + 1));
////		rt_kprintf("team_local_inst.team_work_status[TEAM_WORK_STATUS_0_3] = %d\n",team_local_inst.team_work_status[TEAM_WORK_STATUS_0_3]);
//
//}

//team_signal_em team_signal_gen(void)
//{
//		extern sys_reg_st		g_sys;

//		if((get_team_con_sts() == 1)&&(g_sys.config.team.team_en == 1))
//		{
//				if(get_team_ms_sts() == 1)
//				{
//						team_local_inst.team_signal = TEAM_SIG_MASTER_EN;
//				}
//				else
//				{
//						team_local_inst.team_signal = TEAM_SIG_SLAVE_EN;
//				}
//		}
//		else if(g_sys.config.team.team_en == 1)
//		{
//				team_local_inst.team_signal = TEAM_SIG_SYNC;
//		}
//		else
//		{
//				team_local_inst.team_signal = TEAM_SIG_STOP;
//		}
//
//		return team_local_inst.team_signal;

//}

//static uint8_t get_team_con_sts(void)
//{
//		extern sys_reg_st		g_sys;
//		uint16_t i;
////    total_cnt = (team_local_inst.team_config[TEAM_CONF_CNT] & 0x00ff);

//		for(i = 0; i < TEAM_MAX_SLAVE_NUM ; i++)
//		{
//				if(team_local_inst.team_table[i][TEAM_TAB_TIMEOUT] > 0)
//				{
//						if(i == (g_sys.config.team.addr-1))
//						{
//								continue;
//						}
//						else
//						{
//								return 1;
//						}
//				}
//		}
//		return 0;
//}

//static uint8_t get_team_ms_sts(void)
//{
//		extern sys_reg_st		g_sys;
//		uint16_t i;
//
//		for(i=0;i<g_sys.config.team.addr-1;i++)
//		{
//				if(team_local_inst.team_table[i][TEAM_TAB_TIMEOUT] > 0)
//				{
//						return 0;
//				}
//		}
//		return 1;
//}

////static uint16_t team_slave_conf_chk(void)
////{
////		extern sys_reg_st		g_sys;
////		if((team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS]&(0x0001<<TEAM_STS_CONFD_BPOS)) == 1)

////		team_master_distb_frame();
////		team_local_status_frame();
////		if(get_team_con_sts() == 1)
////		{
////				return 1;
////		}
////
////		return 0;
////}

//
//static uint16_t team_idle(void)
//{
//		extern local_reg_st l_sys;
//
//		l_sys.require[TEAM_REQ][T_REQ] = l_sys.require[LOCAL_REQ][T_REQ];
//		l_sys.require[TEAM_REQ][H_REQ] = l_sys.require[LOCAL_REQ][H_REQ];
//		l_sys.require[TEAM_REQ][F_REQ] = l_sys.require[LOCAL_REQ][F_REQ];
//    clear_param();
//		team_local_inst.sync_poweron_timeout = SYNC_POWERON_TIMEOUT;
//		return 1;
//}

//static uint16_t get_conf_data(void)
//{
//		extern sys_reg_st		g_sys;
//		uint16_t conf_data;
//		conf_data = (g_sys.config.algorithm.temp_ctrl_mode << TEAM_CONF_TMODE_BPOS)|
//								(g_sys.config.algorithm.hum_ctrl_mode << TEAM_CONF_HMODE_BPOS)|
//								(g_sys.config.algorithm.ctrl_target_mode << TEAM_CONF_TARGET_BPOS)|
//								(g_sys.config.team.mode << TEAM_CONF_TMMOD_BPOS)|
//                (g_sys.config.team.cascade_enable << TEAM_CONF_CASCADE_BPOS)|
//								(g_sys.config.algorithm.temp_calc_mode << TEAM_CONF_TCMODE_BPOS)|
//								(g_sys.config.team.team_fan_mode << TEAM_CONF_TEAM_FAN_MODE_BPOS) |
//								(g_sys.config.team.fault_power_en<<TEAM_CONF_TEAM_POWER_OFF_EN);
//
//		return conf_data;
//}

//static uint16_t team_slave_init(void)
//{
//		extern sys_reg_st		g_sys;
////		rt_kprintf("team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS]=%d\n", team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS]);
//		if((team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS]&(0x0001<<TEAM_STS_CONFD_BPOS)) != 0)
//		{
//				return 1;
//		}
//		return 0;
//}

//static int16_t team_check_conf_sts(void)
//{
//		extern sys_reg_st		g_sys;
//		uint16_t i, total_cnt;
//    total_cnt = 32;//(team_local_inst.team_config[TEAM_CONF_CNT] & 0x00ff);
//		//rt_kprintf("total_cnt=%d\n", total_cnt);
//
//		for(i = 0; i < total_cnt; i++)
//		{
//				if(team_local_inst.team_table[i][TEAM_TAB_TIMEOUT] > 0)
//				{
//						if((team_local_inst.team_table[i][TEAM_TAB_STATUS]&(0x0001<<TEAM_STS_CONFD_BPOS)) != 0)
//						{
//								if(i == g_sys.config.team.addr-1)
//								{
//										continue;
//								}
//								else
//								{
//										return i;
//								}
//						}
//				}
//		}
//		return -1;
//}

//static uint32_t team_rotate_init(void)
//{
//		extern sys_reg_st		g_sys;

//		uint32_t team_rs_bitmap;
//
//		team_rs_bitmap = 0xffffffff >> (32 - (uint16_t)(team_local_inst.team_config[TEAM_CONF_CNT] & 0xff) + (uint16_t)((team_local_inst.team_config[TEAM_CONF_CNT] >> 8) & 0xff));

//		return team_rs_bitmap;
//}

//uint8_t check_target_temp_hum(void)
//{
//		extern team_local_st  team_local_inst;
//		uint16_t target_temp,target_hum;
//		uint8_t req;
//
//		req = 0;
//		if(g_sys.config.algorithm.ctrl_target_mode == TARGET_MODE_RETURN)
//		{
//				target_temp = g_sys.config.algorithm.return_air_temp;
//				target_hum = g_sys.config.algorithm.return_air_hum;
//		}
//		else if(g_sys.config.algorithm.ctrl_target_mode == TARGET_MODE_SUPPLY)
//		{
//				target_temp = g_sys.config.algorithm.supply_air_temp;
//				target_hum = g_sys.config.algorithm.return_air_hum;
//		}
//		else
//		{
//				target_temp = g_sys.config.algorithm.remote_air_temp;
//				target_hum = g_sys.config.algorithm.remote_air_hum;
//		}
//	if(team_local_inst.team_config[TEAM_CONF_TEMP_SETVAL]!= target_temp)
//	{
//			team_local_inst.team_config[TEAM_CONF_TEMP_SETVAL] = target_temp;
//			req = 1;
//	}
//	if(team_local_inst.team_config[TEAM_CONF_HUM_SETVAL] != target_hum)
//	{
//			team_local_inst.team_config[TEAM_CONF_HUM_SETVAL]= target_hum;
//			req = 1;
//	}
//	return(req);
//}
//static uint16_t team_master_init(void)
//{
//		extern sys_reg_st		g_sys;
//		int16_t dest_addr;
//		uint16_t ret;
//		if((team_check_local_conf_status() == 1)&&(g_sys.config.team.addr != 1))
//		{
//				ret = 1;
////				rt_kprintf("11111\n");
//		}
//		else
//		{
//				if(g_sys.config.team.addr == 1)
//				{
//						team_local_inst.team_config[TEAM_CONF_MODE] = get_conf_data();
//						team_local_inst.team_config[TEAM_CONF_TEMP_DEADBAND] = g_sys.config.algorithm.temp_deadband;
//						team_local_inst.team_config[TEAM_CONF_TEMP_PRECISION] = g_sys.config.algorithm.temp_precision;
//						team_local_inst.team_config[TEAM_CONF_HUM_DEADBAND] = g_sys.config.algorithm.hum_deadband;
//						team_local_inst.team_config[TEAM_CONF_HUM_PRECISION] = g_sys.config.algorithm.hum_precision;
//						team_local_inst.team_config[TEAM_CONF_ALARM_HI_TEMP_PARAM] = g_sys.config.alarm[ACL_HI_TEMP_RETURN].alarm_param;
//						team_local_inst.team_config[TEAM_CONF_ALARM_LO_TEMP_PARAM] = g_sys.config.alarm[ACL_LO_TEMP_RETURN].alarm_param;
//						team_local_inst.team_config[TEAM_CONF_ALARM_HI_HUM_PARAM] = g_sys.config.alarm[ACL_HI_HUM_RETURN].alarm_param;
//						team_local_inst.team_config[TEAM_CONF_ALARM_LO_HUM_PARAM] = g_sys.config.alarm[ACL_LO_HUM_RETURN].alarm_param;
//						team_local_inst.team_config[TEAM_CONF_CNT] = g_sys.config.team.total_num |((g_sys.config.team.backup_num << 8) & 0xff00);
//						team_local_inst.team_config[TEAM_CONF_ROTATE_PERIOD] = g_sys.config.team.rotate_period;
//            team_local_inst.team_config[TEAM_CONF_ROTATE_TIME] = g_sys.config.team.rotate_time;
//						team_local_inst.team_config[TEAM_CONF_FAN_MODE] = g_sys.config.team.team_fan_mode;
//						team_local_inst.team_config[TEAM_CONF_FAN_TARGET_TEMP] = g_sys.config.fan.target_temp;
//						team_local_inst.team_config[TEAM_CONF_FAN_TEMP_DEADBAND] = g_sys.config.fan.temp_dead_band;
//						team_local_inst.team_config[TEAM_CONF_FAN_TEMP_PRECISION] = g_sys.config.fan.temp_precision;
//						team_local_inst.team_config[TEAM_CONF_FAN_SET_FLOW_DIFF]		= g_sys.config.fan.set_flow_diff;
//						team_local_inst.team_config[TEAM_CONF_FAN_FLOW_DIFF_DEADZONE]= g_sys.config.fan.flow_diff_deadzone;
//						team_local_inst.team_config[TEAM_CONF_FAN_FLOW_DIFF_STEP]		= g_sys.config.fan.flow_diff_step;
//						team_local_inst.team_config[TEAM_CONF_FAN_FLOW_DIFF_DELAY]	= g_sys.config.fan.flow_diff_delay;
//						team_local_inst.team_bitmap[TEAM_BITMAP_RS_SET] = team_rotate_init();
//						team_local_inst.team_bitmap[TEAM_BITMAP_RS_STS] = 0;
//						check_target_temp_hum();
//						ret = 1;
////						rt_kprintf("22222\n");
//				}
//				else
//				{
//						dest_addr = team_check_conf_sts();
//						if(dest_addr > 0)
//						{
//								team_query_frame(dest_addr);
//								ret = 0;
////								rt_kprintf("33333\n");
//						}
//						else
//						{
//								team_local_inst.team_config[TEAM_CONF_MODE] = get_conf_data();
//								team_local_inst.team_config[TEAM_CONF_TEMP_DEADBAND] = g_sys.config.algorithm.temp_deadband;
//								team_local_inst.team_config[TEAM_CONF_TEMP_PRECISION] = g_sys.config.algorithm.temp_precision;
//								team_local_inst.team_config[TEAM_CONF_HUM_DEADBAND] = g_sys.config.algorithm.hum_deadband;
//								team_local_inst.team_config[TEAM_CONF_HUM_PRECISION] = g_sys.config.algorithm.hum_precision;
//								team_local_inst.team_config[TEAM_CONF_ALARM_HI_TEMP_PARAM] = g_sys.config.alarm[ACL_HI_TEMP_RETURN].alarm_param;
//								team_local_inst.team_config[TEAM_CONF_ALARM_LO_TEMP_PARAM] = g_sys.config.alarm[ACL_LO_TEMP_RETURN].alarm_param;
//								team_local_inst.team_config[TEAM_CONF_ALARM_HI_HUM_PARAM] = g_sys.config.alarm[ACL_HI_HUM_RETURN].alarm_param;
//								team_local_inst.team_config[TEAM_CONF_ALARM_LO_HUM_PARAM] = g_sys.config.alarm[ACL_LO_HUM_RETURN].alarm_param;
//								team_local_inst.team_config[TEAM_CONF_CNT] = g_sys.config.team.total_num | ((g_sys.config.team.backup_num << 8) & 0xff00);
//								team_local_inst.team_config[TEAM_CONF_ROTATE_PERIOD] = g_sys.config.team.rotate_period;
//                team_local_inst.team_config[TEAM_CONF_ROTATE_TIME] = g_sys.config.team.rotate_time;
//								team_local_inst.team_config[TEAM_CONF_FAN_MODE] = g_sys.config.team.team_fan_mode;
//								team_local_inst.team_config[TEAM_CONF_FAN_TARGET_TEMP] = g_sys.config.fan.target_temp;
//								team_local_inst.team_config[TEAM_CONF_FAN_TEMP_DEADBAND] = g_sys.config.fan.temp_dead_band;
//								team_local_inst.team_config[TEAM_CONF_FAN_TEMP_PRECISION] = g_sys.config.fan.temp_precision;
//								team_local_inst.team_config[TEAM_CONF_FAN_SET_FLOW_DIFF]		= g_sys.config.fan.set_flow_diff;
//								team_local_inst.team_config[TEAM_CONF_FAN_FLOW_DIFF_DEADZONE]= g_sys.config.fan.flow_diff_deadzone;
//								team_local_inst.team_config[TEAM_CONF_FAN_FLOW_DIFF_STEP]		= g_sys.config.fan.flow_diff_step;
//								team_local_inst.team_config[TEAM_CONF_FAN_FLOW_DIFF_DELAY]	= g_sys.config.fan.flow_diff_delay;
//								team_local_inst.team_bitmap[TEAM_BITMAP_RS_SET] = team_rotate_init();
//								team_local_inst.team_bitmap[TEAM_BITMAP_RS_STS] = 0;
//								check_target_temp_hum();
//								ret = 1;
////								rt_kprintf("44444\n");
//						}
//				}
//		}
//
//		if(ret == 1)
//		{
//				team_master_conf_frame();
//				team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS] |= (0x0001<<TEAM_STS_CONFD_BPOS);
////				rt_kprintf("55555\n");
//		}
//		else
//		{
//				team_local_inst.team_table[g_sys.config.team.addr-1][TEAM_TAB_STATUS] &= ~(0x0001<<TEAM_STS_CONFD_BPOS);
////				rt_kprintf("66666\n");
//		}
//
//		return ret;
//}

//uint16_t team_sync(void)
//{
//		uint16_t init_done;
//		extern local_reg_st l_sys;
//
//    if(team_local_inst.sync_param_protect != 0)
//    {
//        team_local_inst.sync_param_protect--;
//    }
//    if(team_local_inst.sync_param_protect == 0)
//    {
//        l_sys.require[TEAM_REQ][T_REQ] = l_sys.require[LOCAL_REQ][T_REQ];
//        l_sys.require[TEAM_REQ][H_REQ] = l_sys.require[LOCAL_REQ][H_REQ];
//				l_sys.require[TEAM_REQ][F_REQ] = l_sys.require[LOCAL_REQ][F_REQ];
//    }
//
//		if(team_local_inst.sync_poweron_timeout != 0)
//		{
//				team_local_inst.sync_poweron_timeout--;
//		}
//		if(team_local_inst.sync_poweron_timeout == 0)
//		{
//				//开启电源
//		}
//		if(team_local_inst.team_signal == TEAM_SIG_SLAVE_EN)
//		{
//				init_done = team_slave_init();
//		}
//		else if(team_local_inst.team_signal == TEAM_SIG_MASTER_EN)
//		{
//				init_done = team_master_init();
//        team_local_inst.slave_to_master_param_protect = SLAVE_TO_MASTER_PARAM_PROTECT_CNT;
//		}
//		else
//		{
//				init_done = 0;
//		}
//    team_local_status_frame();
//    clear_param();
//		return init_done;
//}

//uint16_t team_get_pwr_sts(void)
//{
//		extern sys_reg_st		g_sys;
//		if((g_sys.config.team.team_en == 1)
//      &&((((team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT]>>(g_sys.config.team.addr - 1)) & 0x0001) != 0) || //team power on
//				  (team_local_inst.sync_poweron_timeout == 0) ||	//sync mode(offline) power on
//					(g_sys.status.alarm_status_cnt.pwr_off_alarm && ((team_local_inst.team_config[TEAM_CONF_MODE] >> TEAM_CONF_TEAM_POWER_OFF_EN) & 0x0001))))	// high_temp warning power on
//		{
//				return 1;
//		}
//		else
//		{
//				return 0;
//		}
//}

////static void team_slave_mode(void)
//static void team_req_distr(void)
//{
//		uint16_t team_mode;
//		extern local_reg_st l_sys;
//    extern sys_reg_st		g_sys;
//    uint8_t cascade_enable;
//    uint8_t i, total_cnt;
//    uint32_t temp_bitmap;
//
//    cascade_enable  = (team_local_inst.team_config[TEAM_CONF_MODE] >> TEAM_CONF_CASCADE_BPOS) & 0x0001;
//    total_cnt       = (team_local_inst.team_config[TEAM_CONF_CNT] & 0x00ff);
//
//		if(((team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] >> (g_sys.config.team.addr - 1)) & 0x0001) != 0)
//		{
//				team_mode = (team_local_inst.team_config[TEAM_CONF_MODE] >> TEAM_CONF_TMMOD_BPOS) & 0x0003;
//				switch(team_mode)
//				{
//						case(TEAM_MODE_STANDALONE):
//						{
//                //独立运行模式：群控需求为本地需求
//                //支持备用和轮巡
//                //不允许层叠
//								l_sys.require[TEAM_REQ][T_REQ] = l_sys.require[LOCAL_REQ][T_REQ];
//								l_sys.require[TEAM_REQ][H_REQ] = l_sys.require[LOCAL_REQ][H_REQ];
//								l_sys.require[TEAM_REQ][F_REQ] = l_sys.require[LOCAL_REQ][F_REQ];
//								break;
//						}
//						case(TEAM_MODE_EVENDISTRIB):
//						{
//                //平均分配模式：平均分配每台的需求
//                //支持备用、轮巡和层叠
//                //是否层叠使能
//                if(cascade_enable)
//                {
//										//温度需求
//                    if(team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] & 0x0080)
//                    {   //需要层叠，前N-1台机组为100%，最后一台为X%
//                        //分配全开需求
//                        for(i = 0; i < total_cnt; i++)
//                        {
//                            if(((team_local_inst.team_bitmap[TEAM_BITMAP_TEMP_FULL_REQ_OUT] >> (g_sys.config.team.addr - 1)) & 0x0001) != 0)
//                            {
//                                if(team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] & 0x0040)
//                                {
//                                    l_sys.require[TEAM_REQ][T_REQ] = 100;
//                                }
//                                else
//                                {
//                                    l_sys.require[TEAM_REQ][T_REQ] = -100;
//                                }
//                            }
//														else
//														{
//																l_sys.require[TEAM_REQ][T_REQ] = 0;
//														}
//                        }
//												//分配最后一台需求
//                        if(g_sys.config.team.addr == ((uint8_t)((team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] >> 8) & 0xff)))
//                        {
//                            l_sys.require[TEAM_REQ][T_REQ] = team_local_inst.team_param[TEAM_PARAM_TEMP_REQ];
//                        }
//                    }
//                    else
//                    {   //不需要层叠，平均分配每台的需求
////                        if(((team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] >> (g_sys.config.team.addr - 1)) & 0x0001) != 0)
//												if(((team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT] >> (g_sys.config.team.addr - 1)) & 0x0001) != 0)
//                        {
//                            l_sys.require[TEAM_REQ][T_REQ] = team_local_inst.team_param[TEAM_PARAM_TEMP_REQ];
//                        }
//												else
//												{
//														l_sys.require[TEAM_REQ][T_REQ] = 0;
//												}
//                    }
//
//										//湿度需求
//                    if(team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] & 0x0080)
//                    {   //需要层叠，前N-1台机组为100%，最后一台为X%
//                        //分配全开需求
//                        for(i = 0; i < total_cnt; i++)
//                        {
//                            if(((team_local_inst.team_bitmap[TEAM_BITMAP_HUM_FULL_REQ_OUT] >> (g_sys.config.team.addr - 1)) & 0x0001) != 0)
//                            {
//                                if(team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] & 0x0040)
//                                {
//                                    l_sys.require[TEAM_REQ][H_REQ] = 100;
//                                }
//                                else
//                                {
//                                    l_sys.require[TEAM_REQ][H_REQ] = -100;
//                                }
//                            }
//														else
//														{
//																l_sys.require[TEAM_REQ][H_REQ] = 0;
//														}
//                        }
//												//分配最后一台需求
//                        if(g_sys.config.team.addr == ((uint8_t)((team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] >> 8) & 0xff)))
//                        {
//                            l_sys.require[TEAM_REQ][H_REQ] = team_local_inst.team_param[TEAM_PARAM_HUM_REQ];
//                        }
//                    }
//                    else
//                    {   //不需要层叠，平均分配每台的需求
////                        if(((team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] >> (g_sys.config.team.addr - 1)) & 0x0001) != 0)
//												if(((team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT] >> (g_sys.config.team.addr - 1)) & 0x0001) != 0)
//                        {
//                            l_sys.require[TEAM_REQ][H_REQ] = team_local_inst.team_param[TEAM_PARAM_HUM_REQ];
//                        }
//												else
//												{
//														l_sys.require[TEAM_REQ][H_REQ] = 0;
//												}
//                    }
//                }
//                else
//                {   //层叠不使能，平均分配需求
//                    l_sys.require[TEAM_REQ][T_REQ] = team_local_inst.team_param[TEAM_PARAM_TEMP_REQ];
//                    l_sys.require[TEAM_REQ][H_REQ] = team_local_inst.team_param[TEAM_PARAM_HUM_REQ];
//                }
//								l_sys.require[TEAM_REQ][F_REQ] = team_local_inst.team_param[TEAM_PARAM_FAN_REQ];
//								break;
//						}
//						case(TEAM_MODE_UINIDIRECTION):
//						{
//                //同向自主模式：
//                //允许备用
//                //不支持轮巡和层叠
//
//                //温度需求
//								if(((team_local_inst.team_param[TEAM_PARAM_TEMP_REQ] < 0) && (team_config_temp_req_calc() < 0))||
//										((team_local_inst.team_param[TEAM_PARAM_TEMP_REQ] > 0) && (team_config_temp_req_calc() > 0)))
//								{
////										l_sys.require[TEAM_REQ][T_REQ] = l_sys.require[LOCAL_REQ][T_REQ];
//                    l_sys.require[TEAM_REQ][T_REQ] = team_config_temp_req_calc();
//								}
//								else
//								{
//										l_sys.require[TEAM_REQ][T_REQ] = l_sys.require[RESET_REQ][T_REQ];
//								}
//								//湿度需求
//								if(((team_local_inst.team_param[TEAM_PARAM_HUM_REQ] < 0) && (team_config_hum_req_calc() < 0))||
//										((team_local_inst.team_param[TEAM_PARAM_HUM_REQ] > 0) && (team_config_hum_req_calc() > 0)))
//								{
////										l_sys.require[TEAM_REQ][H_REQ] = l_sys.require[LOCAL_REQ][H_REQ];
//                    l_sys.require[TEAM_REQ][H_REQ] = team_config_hum_req_calc();
//								}
//								else
//								{
//										l_sys.require[TEAM_REQ][H_REQ] = l_sys.require[RESET_REQ][H_REQ];
//								}
//								l_sys.require[TEAM_REQ][F_REQ] = team_config_fan_req_calc();
//								break;
//						}
//						case(TEAM_MODE_REQDISTRIB):
//						{
//                //按需分配
//                //允许备用和层叠
//                //不支持轮巡
////                if(cascade_enable)
//                {   //使能层叠
//                    //hum_req
//                    //分配最后一台需求
//                    if(g_sys.config.team.addr == ((uint8_t)((team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] >> 8) & 0xff)))
//                    {
//                        l_sys.require[TEAM_REQ][T_REQ] = team_local_inst.team_param[TEAM_PARAM_TEMP_REQ];
//                    }
//                    //分配全开需求
//                    for(i = 0; i < total_cnt; i++)
//                    {
//                        if(((team_local_inst.team_bitmap[TEAM_BITMAP_TEMP_FULL_REQ_OUT] >> (g_sys.config.team.addr - 1)) & 0x0001) != 0)
//                        {
//                            if(team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] & 0x0040)
//                            {
//                                l_sys.require[TEAM_REQ][T_REQ] = 100;
//                            }
//                            else
//                            {
//                                l_sys.require[TEAM_REQ][T_REQ] = -100;
//                            }
//                        }
//                    }
//                    //分配其余的零需求
//                    temp_bitmap = team_local_inst.team_bitmap[TEAM_BITMAP_TEMP_FULL_REQ_OUT] |
//                                  (find_bitmap_by_addr((uint8_t)((team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] >> 8) & 0xff)));
//                    temp_bitmap = team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] & (~temp_bitmap);
////                    rt_kprintf("%x %x %x \n", team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT],
////                            find_bitmap_by_addr((uint8_t)((team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] >> 8) & 0xff)),
////                            temp_bitmap);
//                    if(((temp_bitmap >> (g_sys.config.team.addr - 1)) & 0x0001) != 0)
//                    {
//                        l_sys.require[TEAM_REQ][T_REQ] = 0;
//                    }
//                    //hum_req
//                    //分配最后一台需求
//                    if(g_sys.config.team.addr == ((uint8_t)((team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] >> 8) & 0xff)))
//                    {
//                        l_sys.require[TEAM_REQ][H_REQ] = team_local_inst.team_param[TEAM_PARAM_HUM_REQ];
//                    }
//                    //分配全开需求
//                    for(i = 0; i < total_cnt; i++)
//                    {
//                        if(((team_local_inst.team_bitmap[TEAM_BITMAP_HUM_FULL_REQ_OUT] >> (g_sys.config.team.addr - 1)) & 0x0001) != 0)
//                        {
//                            if(team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] & 0x0040)
//                            {
//                                l_sys.require[TEAM_REQ][H_REQ] = 100;
//                            }
//                            else
//                            {
//                                l_sys.require[TEAM_REQ][H_REQ] = -100;
//                            }
//                        }
//                    }
//                    //分配其余的零需求
//                    temp_bitmap = team_local_inst.team_bitmap[TEAM_BITMAP_HUM_FULL_REQ_OUT] |
//                                  (find_bitmap_by_addr((uint8_t)((team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] >> 8) & 0xff)));
//                    temp_bitmap = team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] & (~temp_bitmap);
//                    if(((temp_bitmap >> (g_sys.config.team.addr - 1)) & 0x0001) != 0)
//                    {
//                        l_sys.require[TEAM_REQ][H_REQ] = 0;
//                    }
//                }
////                else
//                {   //不使能层叠
//                    ;
//                }
//								l_sys.require[TEAM_REQ][F_REQ] = team_config_fan_req_calc();
//								break;
//						}
//						default:
//						{
//								l_sys.require[TEAM_REQ][T_REQ] = l_sys.require[RESET_REQ][T_REQ];
//								l_sys.require[TEAM_REQ][H_REQ] = l_sys.require[RESET_REQ][H_REQ];
//								l_sys.require[TEAM_REQ][F_REQ] = l_sys.require[RESET_REQ][F_REQ];
//								break;
//						}
//				}
//		}
//		else
//		{
//				l_sys.require[TEAM_REQ][T_REQ] = l_sys.require[RESET_REQ][T_REQ];
//				l_sys.require[TEAM_REQ][H_REQ] = l_sys.require[RESET_REQ][H_REQ];
//				l_sys.require[TEAM_REQ][F_REQ] = l_sys.require[RESET_REQ][F_REQ];
//		}
//}

//static uint16_t team_slave(void)
//{
//		extern sys_reg_st		g_sys;
//		team_local_status_frame();
////		team_slave_mode();
//    team_req_distr();
//    clear_param();
//    team_local_inst.sync_param_protect = SYNC_PARAM_PROTECT_CNT;
//		team_local_inst.sync_poweron_timeout = SYNC_POWERON_TIMEOUT;
//		return 1;
//}

//static uint16_t check_slave_conf_sts(void)
//{
//		uint16_t i, total_cnt;
//    total_cnt = (team_local_inst.team_config[TEAM_CONF_CNT] & 0x00ff);
//		for(i = 0; i < total_cnt; i++)
//		{
//				if(team_local_inst.team_table[i][TEAM_TAB_TIMEOUT] > 0)
//				{
//						if((team_local_inst.team_table[i][TEAM_TAB_STATUS]&(0x0001<<TEAM_STS_CONFD_BPOS)) == 0)
//						{
//								return 0;
//						}
//				}
//		}
//		return 1;
//}

//static uint16_t team_rotate_timeup(void)
//{
//    uint16_t ret;
//		time_t now;
//		uint8_t wday,hour,min,period;
//		uint16_t hour_t, day_t, year_t;
//    struct tm* ti;
//		uint32_t period_rotate;
//    time(&now);
//    now += 28800;
//    ti = localtime(&now);
//
////		period = (team_local_inst.team_config[TEAM_CONF_ROTATE_PERIOD]>>8)&0x00ff;
//		wday = team_local_inst.team_config[TEAM_CONF_ROTATE_PERIOD]&0x00ff;
//    if(wday == 0)
//    {
//        period = TEAM_ROTATE_PERIOD_NONE;
//    }
//    else if(wday == 1)
//    {
//        period = TEAM_ROTATE_PERIOD_DAY;
//    }
//    else if((wday >  1) && (wday < 9))
//    {
//        period = TEAM_ROTATE_PERIOD_WEEK;
//        wday = wday - 1;
//    }
//		else if(wday == 9)
//		{
//				period = TEAM_ROTATE_PERIOD_HOUR;
//		}
//    else
//    {
//        period = 0xff;
//    }
//		min = (team_local_inst.team_config[TEAM_CONF_ROTATE_TIME]>>8)&0x00ff;
//		hour = team_local_inst.team_config[TEAM_CONF_ROTATE_TIME]&0x00ff;
//		//得出轮值小时周期
//		hour_t 	= team_local_inst.team_config[TEAM_CONF_ROTATE_TIME] % 24;
//		day_t 	= team_local_inst.team_config[TEAM_CONF_ROTATE_TIME] / 24;
//
//		if(day_t >= 365)
//		{
//				year_t	= day_t	/ 365;
//				day_t   = day_t % 365;
//		}
//		else
//		{
//				year_t  = 0;
//		}
//		hour_t = hour_t;
//		day_t  = day_t;
//		year_t = year_t;
//
//		period_rotate = team_local_inst.team_config[TEAM_CONF_ROTATE_TIME] * 3600;
////		rt_kprintf("set is %d now is %d\n", team_local_inst.rotate_time_t,now);
//		//rt_kprintf("ROTATE time is %d year %d day %d hour\n Now is %d year %d day %d hour\n", team_local_inst.time_rotate->tm_year, team_local_inst.time_rotate->tm_yday, team_local_inst.time_rotate->tm_hour,
//		//ti->tm_year, ti->tm_yday, ti->tm_hour);
////		rt_kprintf("rotate time is %d year %d day %d hour\n", year_t, day_t, hour_t);
////		rt_kprintf("%d\n", team_local_inst.rotate_timeout);
//
//		switch(period)
//		{
//				case(TEAM_ROTATE_PERIOD_NONE):
//				{
//						ret = 0;
//						break;
//				}
//				case(TEAM_ROTATE_PERIOD_DAY):
//				{
//						if((ti->tm_hour == hour)&&(ti->tm_min == min)&&(ti->tm_sec > 0))
//						{
//                if(team_local_inst.rotate_timeout == ROTATE_TIMEOUT_CNT)
//                {
//                    team_local_inst.rotate_timeout = 0;
//                    ret = 1;
//                }
//                else
//                {
//                    ret = 0;
//                }
//						}
//						else
//						{
//								ret = 0;
//						}
//						break;
//				}
//				case(TEAM_ROTATE_PERIOD_WEEK):
//				{
//						if((ti->tm_wday == wday)&&(ti->tm_hour == hour)&&(ti->tm_min == min)&&(ti->tm_sec > 0))
//						{
//								if(team_local_inst.rotate_timeout == ROTATE_TIMEOUT_CNT)
//                {
//                    team_local_inst.rotate_timeout = 0;
//                    ret = 1;
//                }
//                else
//                {
//                    ret = 0;
//                }
//						}
//						else
//						{
//								ret = 0;
//						}
//						break;
//				}
//				case(TEAM_ROTATE_PERIOD_HOUR):
//				{
////						if((ti->tm_year == (team_local_inst.time_rotate->tm_year + year_t))	&&
////							 (ti->tm_yday == (team_local_inst.time_rotate->tm_yday + day_t))	&&
////							 (ti->tm_hour == (team_local_inst.time_rotate->tm_hour + hour_t))	&&
////							 (ti->tm_min  == (team_local_inst.time_rotate->tm_min))						&&
////							 (ti->tm_sec > 0))
//						if((now) > (team_local_inst.rotate_time_t + period_rotate))
//						{
//								if(team_local_inst.rotate_timeout == ROTATE_TIMEOUT_CNT)
//								{
////										team_local_inst.time_rotate = localtime(&now);
////										rt_kprintf("set time\n");
//										team_local_inst.rotate_time_t = now;
//										team_local_inst.rotate_timeout = 0;
//										ret = 1;
//								}
//								else
//								{
//										ret = 0;
//								}
//						}
//						else
//						{
//								ret = 0;
//						}
//						break;
//				}
//				default:
//				{
//						ret = 0;
//						break;
//				}
//		}
//		return ret;
//}

//static uint32_t team_rotate(void)
//{
//		uint32_t bitmap_in;
//		uint32_t bitmap_out;
//		uint32_t bitmap_reg;
//		uint32_t mask;
//		uint16_t total_cnt,backup_cnt;

//
//		total_cnt = (team_local_inst.team_config[TEAM_CONF_CNT]&0x00ff);
//		backup_cnt = ((team_local_inst.team_config[TEAM_CONF_CNT]>>8)&0x00ff);
//		mask = ((0xffffffff<<(32-total_cnt))>>(32-total_cnt))>>(total_cnt - backup_cnt);
//
//		bitmap_in = team_local_inst.team_bitmap[TEAM_BITMAP_RS_SET];
//		bitmap_reg = (bitmap_in & mask)<<(total_cnt - backup_cnt);

//		bitmap_out = bitmap_reg|(bitmap_in>>backup_cnt);
//
//		return bitmap_out;
//}

//static uint16_t team_run_backup_switch_arbitration(void)
//{
//		uint16_t ret;
//    uint16_t team_mode;

//    team_mode         = (team_local_inst.team_config[TEAM_CONF_MODE] >> TEAM_CONF_TMMOD_BPOS) & 0x0003;

//		if((team_rotate_timeup() == 1) &&         //达到轮巡时间
//      ((team_mode == TEAM_MODE_STANDALONE) ||  //独立运行模式
//      (team_mode == TEAM_MODE_EVENDISTRIB)))   //平均需求模式
//		{
//				team_local_inst.team_bitmap[TEAM_BITMAP_RS_SET] = team_rotate();
//        rt_kprintf("run_backup_switch\n");
//				ret = 1;
//		}
//		else
//		{
//				ret = 0;
//		}
//		return ret;
//}

//static void team_calc_req(void)
//{
//		extern local_reg_st l_sys;
//    extern sys_reg_st		g_sys;
//    uint16_t team_mode;
//    uint16_t cascade_enable, total_cnt;
//    uint8_t i, temp_cnt, hum_cnt;
//    uint16_t abs_temp, abs_hum;
//    uint8_t addition_temp, addition_hum;
//    uint8_t temp_full_flag, hum_full_flag;
//
//    cascade_enable  = (team_local_inst.team_config[TEAM_CONF_MODE] >> TEAM_CONF_CASCADE_BPOS) & 0x0001;
//    total_cnt       = (team_local_inst.team_config[TEAM_CONF_CNT] & 0x00ff);
//
//    //总温度需求
//    team_local_inst.team_param[TEAM_PARAM_TOTAL_TEMP_REQ] = team_temp_req_calc(TOTAL_REQ);
//    //总湿度需求
//    team_local_inst.team_param[TEAM_PARAM_TOTAL_HUM_REQ]  = team_hum_req_calc(TOTAL_REQ);
//    //平均温度需求
//    team_local_inst.team_param[TEAM_PARAM_AVER_TEMP_REQ]  = team_temp_req_calc(AVER_REQ);
//    //平均湿度需求
//    team_local_inst.team_param[TEAM_PARAM_AVER_HUM_REQ]   = team_hum_req_calc(AVER_REQ);
//		//平均风机需求
//		team_local_inst.team_param[TEAM_PARAM_FAN_REQ]				= team_fan_req_calc();

//    addition_temp = 0;
//    addition_hum  = 0;
//    temp_full_flag= 0;
//    hum_full_flag = 0;
//
//    team_mode         = (team_local_inst.team_config[TEAM_CONF_MODE] >> TEAM_CONF_TMMOD_BPOS) & 0x0003;
//    switch(team_mode)
//    {
//        case (TEAM_MODE_STANDALONE):  //独立运行
//            //temp req
//            team_local_inst.team_param[TEAM_PARAM_TEMP_REQ] = team_local_inst.team_param[TEAM_PARAM_AVER_TEMP_REQ];
//            //hum req
//            team_local_inst.team_param[TEAM_PARAM_HUM_REQ] = team_local_inst.team_param[TEAM_PARAM_AVER_HUM_REQ];
//
//            break;
//        case (TEAM_MODE_EVENDISTRIB): //平均分配需求
//            if(cascade_enable)        //层叠使能
//            {
//                //temp req
//                if(abs(team_local_inst.team_param[TEAM_PARAM_AVER_TEMP_REQ]) > 100)
//                {   //需要层叠
//                    team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] = team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] | 0x0080;
//                    //计算需要层叠的台数
//                    if((team_local_inst.team_param[TEAM_PARAM_TOTAL_TEMP_REQ] % 100) == 0)
//                    {
//                        addition_temp   = abs(team_local_inst.team_param[TEAM_PARAM_TOTAL_TEMP_REQ]) / 100;
//                        temp_full_flag  = 0;
//                    }
//                    else
//                    {
//                        addition_temp   = abs(team_local_inst.team_param[TEAM_PARAM_TOTAL_TEMP_REQ]) / 100 + 1;
//                        temp_full_flag  = 1;
//                    }
//                    //最后一台未全开的需求
//                    team_local_inst.team_param[TEAM_PARAM_TEMP_REQ] = team_local_inst.team_param[TEAM_PARAM_TOTAL_TEMP_REQ] % 100;
//                    //全开的正负需求
//                    if(team_local_inst.team_param[TEAM_PARAM_TOTAL_TEMP_REQ] > 0)
//                    {
//                        team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] = team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] | 0x0040;
//                    }
//                    else
//                    {
//                        team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] = team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] & (~(0x0040));
//                    }
//                }
//                else
//                {   //不需要层叠,平均分配需求
//                    team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] = team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] & (~(0x0080));
//                    team_local_inst.team_param[TEAM_PARAM_TEMP_REQ] = team_local_inst.team_param[TEAM_PARAM_AVER_TEMP_REQ];
//                }
//                //hum req
//                if(abs(team_local_inst.team_param[TEAM_PARAM_AVER_HUM_REQ]) > 100)
//                {   //需要层叠
//                    team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] = team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] | 0x0080;
//                    //计算需要层叠的台数
//                    if((team_local_inst.team_param[TEAM_PARAM_TOTAL_HUM_REQ] % 100) == 0)
//                    {
//                        addition_hum    = abs(team_local_inst.team_param[TEAM_PARAM_TOTAL_HUM_REQ]) / 100;
//                        hum_full_flag   = 0;
//                    }
//                    else
//                    {
//                        addition_hum    = abs(team_local_inst.team_param[TEAM_PARAM_TOTAL_HUM_REQ]) / 100 + 1;
//                        hum_full_flag   = 1;
//                    }
//                    //最后一台未全开的需求
//                    team_local_inst.team_param[TEAM_PARAM_HUM_REQ] = team_local_inst.team_param[TEAM_PARAM_TOTAL_HUM_REQ] % 100;
//                    //全开的正负需求
//                    if(team_local_inst.team_param[TEAM_PARAM_TOTAL_HUM_REQ] > 0)
//                    {
//                        team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] = team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] | 0x0040;
//                    }
//                    else
//                    {
//                        team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] = team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] & (~(0x0040));
//                    }
//                }
//                else
//                {   //不需要层叠,平均分配需求
//                    team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] = team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] & (~(0x0080));
//                    team_local_inst.team_param[TEAM_PARAM_HUM_REQ] = team_local_inst.team_param[TEAM_PARAM_AVER_HUM_REQ];
//                }
//            }
//            else
//            {   //层叠不使能，平均分配需求
//                //temp req
//                team_local_inst.team_param[TEAM_PARAM_TEMP_REQ] = team_local_inst.team_param[TEAM_PARAM_AVER_TEMP_REQ];
//                //hum req
//                team_local_inst.team_param[TEAM_PARAM_HUM_REQ] = team_local_inst.team_param[TEAM_PARAM_AVER_HUM_REQ];
//            }
//
//            break;
//        case (TEAM_MODE_UINIDIRECTION): //同向自主
//            //temp req
//            team_local_inst.team_param[TEAM_PARAM_TEMP_REQ] = team_local_inst.team_param[TEAM_PARAM_AVER_TEMP_REQ];
//            if(team_local_inst.team_param[TEAM_PARAM_TOTAL_TEMP_REQ] == 0)   //需求为0，方向为绝对值最大的方向
//            {
//                temp_cnt = 0;
//                abs_temp = 0;
//                for(i = 0; i < total_cnt; i++)
//                {
//                    if(team_local_inst.team_table[i][TEAM_TAB_TIMEOUT] > 0)
//                    {
//                        if(abs((int16_t)team_local_inst.team_table[i][TEAM_TAB_TEMP_REQ]) > abs_temp)
//                        {
//                            abs_temp = abs((int16_t)team_local_inst.team_table[i][TEAM_TAB_TEMP_REQ]);
//                            temp_cnt = i;
//                        }
//                    }
//                }
//                team_local_inst.team_param[TEAM_PARAM_TEMP_REQ] = team_local_inst.team_table[temp_cnt][TEAM_TAB_TEMP_REQ];
//            }
//
//            //hum req
//            team_local_inst.team_param[TEAM_PARAM_HUM_REQ] = team_local_inst.team_param[TEAM_PARAM_AVER_HUM_REQ];
//            if(team_local_inst.team_param[TEAM_PARAM_TOTAL_HUM_REQ] == 0)   //需求为0，方向为绝对值最大的方向
//            {
//                hum_cnt = 0;
//                abs_hum = 0;
//                for(i = 0; i < total_cnt; i++)
//                {
//                    if(team_local_inst.team_table[i][TEAM_TAB_TIMEOUT] > 0)
//                    {
//                        if(abs((int16_t)team_local_inst.team_table[i][TEAM_TAB_HUM_REQ]) > abs_hum)
//                        {
//                            abs_hum = abs((int16_t)team_local_inst.team_table[i][TEAM_TAB_HUM_REQ]);
//                            hum_cnt = i;
//                        }
//                    }
//                }
//                team_local_inst.team_param[TEAM_PARAM_HUM_REQ] = team_local_inst.team_table[hum_cnt][TEAM_TAB_HUM_REQ];
//            }
//
//            break;
//        case (TEAM_MODE_REQDISTRIB):  //按需分配
//            //计算需要打开的台数
//            //temp_req
//            if((team_local_inst.team_param[TEAM_PARAM_TOTAL_TEMP_REQ] % 100) == 0)
//            {
//                addition_temp   = abs(team_local_inst.team_param[TEAM_PARAM_TOTAL_TEMP_REQ]) / 100;
//                temp_full_flag  = 0;
//            }
//            else
//            {
//                addition_temp   = abs(team_local_inst.team_param[TEAM_PARAM_TOTAL_TEMP_REQ]) / 100 + 1;
//                temp_full_flag  = 1;
//            }
//            //最后一台未全开的需求
//            team_local_inst.team_param[TEAM_PARAM_TEMP_REQ] = team_local_inst.team_param[TEAM_PARAM_TOTAL_TEMP_REQ] % 100;
//            //全开的正负需求
//            if(team_local_inst.team_param[TEAM_PARAM_TOTAL_TEMP_REQ] > 0)
//            {
//                team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] = team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] | 0x0040;
//            }
//            else
//            {
//                team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] = team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] & (~(0x0040));
//            }
//            //hum_req
//            if((team_local_inst.team_param[TEAM_PARAM_TOTAL_HUM_REQ] % 100) == 0)
//            {
//                addition_hum   = abs(team_local_inst.team_param[TEAM_PARAM_TOTAL_HUM_REQ]) / 100;
//                hum_full_flag  = 0;
//            }
//            else
//            {
//                addition_hum   = abs(team_local_inst.team_param[TEAM_PARAM_TOTAL_HUM_REQ]) / 100 + 1;
//                hum_full_flag  = 1;
//            }
//            //最后一台未全开的需求
//            team_local_inst.team_param[TEAM_PARAM_HUM_REQ] = team_local_inst.team_param[TEAM_PARAM_TOTAL_HUM_REQ] % 100;
//            //全开的正负需求
//            if(team_local_inst.team_param[TEAM_PARAM_TOTAL_HUM_REQ] > 0)
//            {
//                team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] = team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] | 0x0040;
//            }
//            else
//            {
//                team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] = team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] & (~(0x0040));
//            }
//
//            if(cascade_enable)        //层叠使能
//            {
//                //temp req
//
//                //hum req
//
//            }
//            else
//            {
//                //temp req
//                if(addition_temp > get_bitmap_num(team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT]))
//                {
//                    addition_temp   = get_bitmap_num(team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT]);
//                    temp_full_flag  = 0;
//                }
//                //hum req
//                if(addition_hum > get_bitmap_num(team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT]))
//                {
//                    addition_hum    = get_bitmap_num(team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT]);
//                    hum_full_flag   = 0;
//                }
//            }
//            break;
//        default:
//            team_local_inst.team_param[TEAM_PARAM_TEMP_REQ] = team_local_inst.team_param[TEAM_PARAM_AVER_TEMP_REQ];
//            team_local_inst.team_param[TEAM_PARAM_HUM_REQ] = team_local_inst.team_param[TEAM_PARAM_AVER_HUM_REQ];
//
//            break;
//    }
////    rt_kprintf("%x %x %x %x\n", addition_temp, temp_full_flag, addition_hum ,hum_full_flag);
//		team_update_Final_output(addition_temp, temp_full_flag, addition_hum, hum_full_flag);
//}

//static void team_check_config(void)
//{
//    extern sys_reg_st		g_sys;
//    uint8_t i;
//    uint16_t total_cnt;
//    total_cnt = (team_local_inst.team_config[TEAM_CONF_CNT] & 0x00ff);
//    for(i = 0; i < total_cnt; i++)
//		{
//        if(team_local_inst.team_table[i][TEAM_TAB_TIMEOUT] == 0)
//        {
//            continue;
//        }
//        if((team_local_inst.team_table[i][TEAM_TAB_RESERVE] & 0xff) !=
//           (team_local_inst.team_table[g_sys.config.team.addr - 1][TEAM_TAB_RESERVE] & 0xff))
//        {
//            team_clear_confd();
//            break;
//        }
//				if((team_local_inst.team_table[i][TEAM_TAB_RESERVE] & 0xff00) !=
//           (team_local_inst.team_table[g_sys.config.team.addr - 1][TEAM_TAB_RESERVE] & 0xff00))
//				{
//						team_local_inst.run_sts_flag							= 1;
//						break;
//				}
//    }
//}

//static uint16_t team_master(void)
//{
//		extern sys_reg_st		g_sys;
//
//    if(team_local_inst.slave_to_master_param_protect != 0)
//    {
//        team_local_inst.slave_to_master_param_protect--;
//    }
//    team_local_inst.sync_param_protect = SYNC_PARAM_PROTECT_CNT;
//		team_local_inst.sync_poweron_timeout = SYNC_POWERON_TIMEOUT;
//
//    if(team_local_inst.master_send_param_protect == 0)
//    {
//        team_local_inst.master_send_param_protect = MASTER_SEND_PARAM_PROTECT_CNT;
//
//        *(uint16_t *)&team_local_inst.master_req_distb_buf[0]     = team_local_inst.team_param[TEAM_PARAM_MODE];
//        *(uint16_t *)&team_local_inst.master_req_distb_buf[2]     = team_local_inst.team_param[TEAM_PARAM_TEMP_REQ];
//        *(uint16_t *)&team_local_inst.master_req_distb_buf[4]     = team_local_inst.team_param[TEAM_PARAM_HUM_REQ];
//				*(uint16_t *)&team_local_inst.master_req_distb_buf[6]			= team_local_inst.team_param[TEAM_PARAM_FAN_REQ];
//
//        *(uint16_t *)&team_local_inst.master_ex_temp_param_buf[0] = team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ];
//        *(uint32_t *)&team_local_inst.master_ex_temp_param_buf[2] = team_local_inst.team_bitmap[TEAM_BITMAP_TEMP_FULL_REQ_OUT];
//
//        *(uint16_t *)&team_local_inst.master_ex_hum_param_buf[0]  = team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ];
//        *(uint32_t *)&team_local_inst.master_ex_hum_param_buf[2]  = team_local_inst.team_bitmap[TEAM_BITMAP_HUM_FULL_REQ_OUT];
//
//        *(uint32_t *)&team_local_inst.master_run_final_sts_buf[0] = team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT];
//        *(uint32_t *)&team_local_inst.master_run_final_sts_buf[4] = team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT];
//    }
//
//		team_local_status_frame();
//
//    team_update_output();
//
//		team_calc_req();
//
//    team_table_req_update();
//
//		team_master_distb_frame();
//
//    team_req_distr();
//
//    team_check_config();
//
//		//check if all slaves are configured
//		if(check_slave_conf_sts() == 0)
//		{
//				team_master_conf_frame();
//		}
//
//		//check if team rotation timeup or team run state match
//		if((team_run_backup_switch_arbitration() != 0) ||
//			  (team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] != team_local_inst.team_bitmap[TEAM_BITMAP_RS_STS]) ||
//				team_local_inst.run_sts_flag)
//				//(team_local_inst.run_sts_cd == 0))
//		{
//				team_run_sts_set_frame();
//        team_run_final_sts_set_frame();
//				team_local_inst.run_sts_cd = RUN_STS_CD;
//				team_local_inst.run_sts_flag = 0;
//		}
//
//		//set local run status
//
//		return 1;
//}

//void team_fsm_trans(void)
//{
//		uint16_t init_done;
//		time_t now;
//		team_lc_sts_update();
//		team_signal_gen();
////		rt_kprintf("team_fsm = %d\n",team_local_inst.team_fsm);
//		switch(team_local_inst.team_fsm)
//		{
//				case(TEAM_FSM_IDLE):
//				{
//						team_idle();
//						if(team_local_inst.team_signal != TEAM_SIG_STOP)
//						{
//								team_local_inst.team_fsm = TEAM_FSM_SYNC;
//						}
//						else
//						{
//								team_local_inst.team_fsm = TEAM_FSM_IDLE;
//						}
//						break;
//				}
//				case(TEAM_FSM_SYNC):
//				{
//						init_done = team_sync();
//						if(team_local_inst.team_signal != TEAM_SIG_STOP)
//						{
//								if(init_done == 1)
//								{
//										if(team_local_inst.team_signal == TEAM_SIG_MASTER_EN)
//										{
//												team_local_inst.team_fsm = TEAM_FSM_MASTER;
//												//获取当前时间作为启动时间
//												time(&now);
//												now += 28800;
//												team_local_inst.rotate_time_t = now;
//										}
//										else
//										{
//												team_local_inst.team_fsm = TEAM_FSM_SLAVE;
//										}
//
//								}
//								else
//								{
//										team_local_inst.team_fsm = TEAM_FSM_SYNC;
//								}
//						}
//						else
//						{
//								team_local_var_reset();
//								team_local_inst.team_fsm = TEAM_FSM_IDLE;
//						}
//						break;
//				}
//				case(TEAM_FSM_SLAVE):
//				{
//						init_done = team_slave();
//						if(team_local_inst.team_signal == TEAM_SIG_STOP)
//						{
//								team_local_var_reset();
//								team_local_inst.team_fsm = TEAM_FSM_IDLE;
//						}
//						else if(team_local_inst.team_signal != TEAM_SIG_SLAVE_EN)
//						{
//								team_local_inst.team_fsm = TEAM_FSM_SYNC;
//						}
//						else
//						{
//								team_local_inst.team_fsm = TEAM_FSM_SLAVE;
//						}
//						break;
//				}
//				case(TEAM_FSM_MASTER):
//				{
//						init_done = team_master();
//						if(team_local_inst.team_signal == TEAM_SIG_STOP)
//						{
//								team_local_var_reset();
//								team_local_inst.team_fsm = TEAM_FSM_IDLE;
//						}
//						else if(team_local_inst.team_signal != TEAM_SIG_MASTER_EN)
//						{
//								team_local_inst.team_fsm = TEAM_FSM_SYNC;
//						}
//						else
//						{
//								team_local_inst.team_fsm = TEAM_FSM_MASTER;
//						}
//						break;
//				}
//				default:
//				{
//						team_local_inst.team_fsm = TEAM_FSM_IDLE;
//						break;
//				}
//		}
//    team_state_update();
//}

//static void clear_param(void)
//{
//    uint8_t i;
//    for(i = 0; i < TEAM_MAX_SLAVE_NUM; i++)
//    {
//        team_local_inst.team_table[i][TEAM_TAB_TEMP_REQ]  = 0;
//        team_local_inst.team_table[i][TEAM_TAB_HUM_REQ]   = 0;
//        team_local_inst.team_sort_temp[i]                 = 0;
//        team_local_inst.team_sort_hum[i]                  = 0;
//    }
//    team_local_inst.team_param[TEAM_PARAM_TOTAL_TEMP_REQ] = 0;
//    team_local_inst.team_param[TEAM_PARAM_TOTAL_HUM_REQ]  = 0;
//    team_local_inst.team_param[TEAM_PARAM_AVER_TEMP_REQ]  = 0;
//    team_local_inst.team_param[TEAM_PARAM_AVER_HUM_REQ]   = 0;
//}

//static uint8_t team_update_onlinestatus(void)
//{
//    uint32_t onlineStatus = 0;
//    uint8_t i;
//    uint8_t online_cnt = 0;
//    uint16_t total_cnt;
//
//    total_cnt = (team_local_inst.team_config[TEAM_CONF_CNT] & 0x00ff);
//
//    for(i = 0; i < total_cnt; i++)
//    {
//        if(team_local_inst.team_table[i][TEAM_TAB_TIMEOUT] > 0)
//				{
//						onlineStatus = onlineStatus | (0x00000001 << i);
//            online_cnt++;
//				}
//				else
//				{
//						onlineStatus = onlineStatus & (~(0x00000001 << i));
//				}
//    }
//    team_local_inst.team_bitmap[TEAM_BITMAP_ONLINE] = onlineStatus;
//    return online_cnt;
//}

//static uint32_t team_get_offlinestasus(void)
//{
//    extern sys_reg_st   g_sys;
//    uint32_t offlineStatus, onlineStatus;
//    onlineStatus  = team_local_inst.team_bitmap[TEAM_BITMAP_ONLINE];
//    offlineStatus = team_get_totalbitmap((uint16_t)(team_local_inst.team_config[TEAM_CONF_CNT] & 0xff)) & (~onlineStatus);
//    return offlineStatus;
//}

//static uint32_t team_update_confdstatus(void)
//{
//    uint32_t confdStatus = 0;
//    uint8_t i, total_cnt;
//    total_cnt = (team_local_inst.team_config[TEAM_CONF_CNT] & 0x00ff);
//    for(i = 0; i < total_cnt; i++)
//    {
//        if((team_local_inst.team_table[i][TEAM_TAB_STATUS] & (0x0001 << TEAM_STS_CONFD_BPOS)) != 0)
//        {
//            confdStatus = confdStatus | (0x00000001 << i);
//        }
//        else
//        {
//            confdStatus = confdStatus & (~(0x00000001 << i));
//        }
//    }
//    return confdStatus;
//}

//static uint8_t online_calc(uint32_t online_bitmap)
//{
//	uint8_t i,sum;
//	sum = 0;
//	for(i=0;i<32;i++)
//	{
//		if((online_bitmap&(0x01<<i)) != 0)
//		{
//			 sum++;
//		}
//	}
//	return(sum);
//}
//static void team_state_update(void)
//{
//		extern sys_reg_st		g_sys;
//
//		if(g_sys.config.team.team_en == 1)
//		{
//			if(team_local_inst.team_fsm == TEAM_FSM_MASTER )
//			{
//					if(g_sys.config.team.addr != 1)
//					{
//						//set status bit
//							sys_option_di_sts(DI_GROUP_DEFAULT_BPOS,1);
//
//					}
//					else if((uint16_t)(team_local_inst.team_config[TEAM_CONF_CNT] & 0xff) != online_calc( team_local_inst.team_bitmap[TEAM_BITMAP_ONLINE]))
//					{
//						//set status bit
//							sys_option_di_sts(DI_GROUP_DEFAULT_BPOS,1);
//					}
//					else
//					{
//							//reset ststus bit
//							sys_option_di_sts(DI_GROUP_DEFAULT_BPOS,0);
//					}

//			}
//			else if(team_local_inst.team_fsm == TEAM_FSM_SYNC)
//			{
//				  //set status bit
//				  sys_option_di_sts(DI_GROUP_DEFAULT_BPOS,1);
//			}
//			else
//			{
//					//reset ststus bit
//					sys_option_di_sts(DI_GROUP_DEFAULT_BPOS,0);
//			}
//
//		}
//		else
//		{
//			//reset ststus bit
//					sys_option_di_sts(DI_GROUP_DEFAULT_BPOS,0);
//		}
//}

//static uint32_t team_get_totalbitmap(uint8_t total_num)
//{
//    uint32_t totalBitmap = 0;
//    uint8_t i;
//    for(i = 0; i < total_num; i++)
//    {
//        totalBitmap |= 1 << i;
//    }
//    return totalBitmap;
//}

//static uint8_t is_running_dev(uint8_t num)
//{
//    uint8_t ret;
//    if((team_local_inst.team_bitmap[TEAM_BITMAP_RS_SET] & (0x00000001 << num)) != 0)
//    {
//        ret = 1;
//    }
//    else
//    {
//        ret = 0;
//    }
//    return ret;
//}

//static uint8_t team_update_malfun_dev(void)
//{
//    extern sys_reg_st		g_sys;
//    uint8_t i;
//    uint8_t warningDev_Num = 0;
//    uint32_t offlineStatus;
//    offlineStatus = team_get_offlinestasus();
//    for(i = 0; i < (uint16_t)(team_local_inst.team_config[TEAM_CONF_CNT] & 0xff); i++)
//    {
//        if(((team_local_inst.team_table[i][TEAM_TAB_STATUS] & (0x0001 << TEAM_STS_ALARM_BPOS)) != 0) ||
//          (offlineStatus & (0x00000001 << i)))
//        {
//            team_local_inst.team_bitmap[TEAM_BITMAP_MALFUN_DEV] |= (0x00000001 << i);
//            if(is_running_dev(i) == 1)
//            {
//                warningDev_Num++;
//            }
//        }
//        else
//        {
//            team_local_inst.team_bitmap[TEAM_BITMAP_MALFUN_DEV] &= (~(0x00000001 << i));
//        }
//    }
//    return warningDev_Num;
//}

////static void team_update_output(uint32_t test_dev,uint8_t test_num)
//static void team_update_output(void)
//{
//    extern sys_reg_st   g_sys;
//    uint32_t totalBitmap;
//    uint32_t backupBitmap;
//    uint32_t temp_bitmap;
//    uint8_t  i, temp;
//    uint8_t  warningDev_Num;
//
//    i               = 0;
//    temp            = 0;
//    temp_bitmap     = 0;
//    warningDev_Num  = team_update_malfun_dev();
////    warningDev_Num = test_num;
////    team_local_inst.team_bitmap[TEAM_BITMAP_MALFUN_DEV] = test_dev;
//
////    rt_kprintf("warningDev num: %x, bitmap: %x\n", warningDev_Num, team_local_inst.team_bitmap[TEAM_BITMAP_MALFUN_DEV]);
//    totalBitmap     = team_get_totalbitmap((uint16_t)(team_local_inst.team_config[TEAM_CONF_CNT] & 0xff));
//    backupBitmap    = totalBitmap & (~team_local_inst.team_bitmap[TEAM_BITMAP_RS_SET]);
////		rt_kprintf("total map: %x, backup map: %x\n", totalBitmap, backupBitmap);
//
//    //turn off warning device
//    team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT] = team_local_inst.team_bitmap[TEAM_BITMAP_RS_SET] &
//                                                      (~team_local_inst.team_bitmap[TEAM_BITMAP_MALFUN_DEV]);
////    rt_kprintf("turn off warning device: %x\n", team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT]);
//    //turn on backup device
//    while((i < 32) && (temp < warningDev_Num))
//    {
//        if(((backupBitmap & (0x00000001 << i)) != 0) &&
//          ((team_local_inst.team_bitmap[TEAM_BITMAP_MALFUN_DEV] & (0x00000001 << i)) == 0))
//        {
//            temp++;
//            temp_bitmap |= backupBitmap & (0x00000001 << i);
//        }
//        i++;
//    }
////    rt_kprintf("temp_bitmap: %x\n", temp_bitmap);
//    team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT] = team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT] | temp_bitmap;
////    rt_kprintf("turn on warning device: %x\n\n\n", team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT]);
////		rt_kprintf("team_local_inst.team_work_status[TEAM_BITMAP_MALFUN_DEV] = %d\n",team_local_inst.team_bitmap[TEAM_BITMAP_MALFUN_DEV]);
////		rt_kprintf("team_local_inst.team_work_status[TEAM_BITMAP_ONLINE] = %d\n",team_local_inst.team_bitmap[TEAM_BITMAP_ONLINE]);
////		rt_kprintf("team_local_inst.team_work_status[TEAM_BITMAP_OUTPUT] = %d\n",team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT]);
////		rt_kprintf("team_local_inst.team_work_status[TEAM_WORK_STATUS_0_3] = %d\n",team_local_inst.team_work_status[TEAM_WORK_STATUS_0_3]);
//}

//static void team_update_Final_output(uint8_t temp_cnt, uint8_t temp_flag, uint8_t hum_cnt, uint8_t hum_flag)
//{
//    extern sys_reg_st g_sys;
//    uint8_t cascade_enable, team_mode;
//
//    cascade_enable = (team_local_inst.team_config[TEAM_CONF_MODE] >> TEAM_CONF_CASCADE_BPOS) & 0x0001;
//    team_mode      = (team_local_inst.team_config[TEAM_CONF_MODE] >> TEAM_CONF_TMMOD_BPOS) & 0x0003;
//
//    if(cascade_enable)
//    { //层叠使能
//        switch(team_mode)
//        {
//            case TEAM_MODE_STANDALONE:
//                team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] = team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT];
//                break;
//            case TEAM_MODE_EVENDISTRIB:
//                cascade_bitmap_update(temp_cnt, temp_flag, hum_cnt, hum_flag);
//                team_master_ex_temp_set();
//                team_master_ex_hum_set();
//                break;
//            case TEAM_MODE_UINIDIRECTION:
//                team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] = team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT];
//                break;
//            case TEAM_MODE_REQDISTRIB:
////                rt_kprintf("%x %x %x %x\n", temp_cnt, temp_flag, hum_cnt, hum_flag);
//                cascade_bitmap_update(temp_cnt, temp_flag, hum_cnt, hum_flag);
//                team_master_ex_temp_set();
//                team_master_ex_hum_set();
//                break;
//            default:

//                break;
//        }
//    }
//    else
//    { //未使能层叠模式
//        switch(team_mode)
//        {
//            case TEAM_MODE_STANDALONE:
//                team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] = team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT];
//                break;
//            case TEAM_MODE_EVENDISTRIB:
//                team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] = team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT];
//                break;
//            case TEAM_MODE_UINIDIRECTION:
//                team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] = team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT];
//                break;
//            case TEAM_MODE_REQDISTRIB:
//                team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] = team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT];
////                rt_kprintf("%x %x %x %x\n", temp_cnt, temp_flag, hum_cnt, hum_flag);
//                cascade_bitmap_update(temp_cnt, temp_flag, hum_cnt, hum_flag);
//                team_master_ex_temp_set();
//                team_master_ex_hum_set();
//                break;
//            default:
//                team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT] = team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT];
//                break;
//        }
//    }
//}

//static uint8_t get_bitmap_num(uint32_t bitmap)
//{
//    uint8_t i;
//    uint8_t num;
//    num = 0;
//    for(i = 0; i < 32; i++)
//    {
//        if(bitmap & (0x0001 << i))
//        {
//            num++;
//        }
//    }
//    return num;
//}

//static uint32_t find_bitmap_in_bitmap(uint32_t bitmap, uint8_t cnt)
//{
//    uint8_t i, cnt_temp;
//    uint32_t ret;
//    cnt_temp = 0;
//    ret = 0;
//    for(i = 0; i < 32; i++)
//    {
//        if(bitmap & (0x0001 << i))
//        {
//            if(cnt_temp >= cnt)
//            {
//                break;
//            }
//            ret |= 0x0001 << i;
//            cnt_temp++;
//        }
//    }
//    return ret;
//}

//static uint32_t find_bitmap_in_bitmap_by_sort(uint32_t bitmap, uint8_t cnt, uint8_t sort[])
//{
//    uint8_t i, cnt_temp;
//    uint32_t ret;
//    cnt_temp = 0;
//    ret = 0;
//    for(i = 0; i < 32; i++)
//    {
//        if(bitmap & (0x0001 << sort[i]))
//        {
//            if(cnt_temp >= cnt)
//            {
//                break;
//            }
//            ret |= 0x0001 << sort[i];
//            cnt_temp++;
//        }
//    }
//    return ret;
//}

//static uint8_t find_addr_in_bitmap(uint32_t bitmap)
//{
//    uint8_t i;
//    uint8_t addr;
//    addr = 0xff;
//    for(i = 0; i < 32; i++)
//    {
//        if(bitmap & (0x0001 << i))
//        {
//            addr = i + 1;
//            break;
//        }
//    }
//    return addr;
//}

//static uint32_t find_bitmap_by_addr(uint8_t addr)
//{
//    uint32_t bitmap;
//    if((addr > 0) && (addr < 33)) //1~32
//    {
//        bitmap = 0x0001 << (addr - 1);
//    }
//    else
//    {
//        bitmap = 0;
//    }
//    return bitmap;
//}

//static void cascade_bitmap_update(uint8_t temp_cnt, uint8_t temp_flag, uint8_t hum_cnt, uint8_t hum_flag)
//{
//    extern sys_reg_st g_sys;
//    uint8_t output_cnt;
//    uint8_t temp_cascade_flag, hum_cascade_flag;
//    uint32_t totalBitmap, stopBitmap;
//    uint32_t usableBackupBitmap;
//    uint32_t t_usableBitmap, h_usableBitmap;
//    uint32_t temp_openBitmap, hum_openBitmap;
//    uint32_t temp_full_usableBitmap, hum_full_usableBitmap;
//
////    rt_kprintf("%x %x %x %x \n", temp_cnt, temp_flag, hum_cnt, hum_flag);
//    //当前主备切换后输出bitmap的机组数量
//    output_cnt = get_bitmap_num(team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT]);
//    if(temp_cnt > output_cnt)
//    {   //温度需要层叠
//        temp_cascade_flag = 1;
//    }
//    else
//    {   //温度不需要层叠
//        temp_cascade_flag = 0;
//    }
//    if(hum_cnt > output_cnt)
//    {   //湿度需要层叠
//        hum_cascade_flag = 1;
//    }
//    else
//    {   //湿度不需要层叠
//        hum_cascade_flag = 0;
//    }
//
//    //当前设置所有机组的Bitmap
//    totalBitmap         = team_get_totalbitmap((uint16_t)(team_local_inst.team_config[TEAM_CONF_CNT] & 0xff));
//    //未运行机组的Bitmap
//    stopBitmap          = totalBitmap & (~team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT]);
//    //可用的备机Bitmap
//    usableBackupBitmap  = stopBitmap & (~team_local_inst.team_bitmap[TEAM_BITMAP_MALFUN_DEV]);
//
//    //可用的Bitmap
//    if(temp_cascade_flag)
//    {   //温度需要层叠，可用Bitmap为可用的备机Bitmap
//        t_usableBitmap  = usableBackupBitmap;
//    }
//    else
//    {   //温度不需要层叠可用Bitmap为主备切换后的打开的Bitmap
//        t_usableBitmap  = team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT];
//    }
//    if(hum_cascade_flag)
//    {   //湿度需要层叠，可用Bitmap为可用的备机Bitmap
//        h_usableBitmap  = usableBackupBitmap;
//    }
//    else
//    {   //湿度不需要层叠可用Bitmap为主备切换后的打开的Bitmap
//        h_usableBitmap  = team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT];
//    }
//
//    //开关机的Bitmap
//    if(temp_cascade_flag)
//    {   //温度需要层叠
//        temp_openBitmap = find_bitmap_in_bitmap(t_usableBitmap, (temp_cnt - output_cnt));
//    }
//    else
//    {   //温度不需要层叠
////        temp_openBitmap = find_bitmap_in_bitmap(t_usableBitmap, temp_cnt);
//        temp_openBitmap = find_bitmap_in_bitmap_by_sort(t_usableBitmap, temp_cnt, team_local_inst.team_sort_temp);
//    }
//    if(hum_cascade_flag)
//    {   //湿度需要层叠
//        hum_openBitmap  = find_bitmap_in_bitmap(h_usableBitmap, (hum_cnt - output_cnt));
//    }
//    else
//    {   //湿度不需要层叠
////        hum_openBitmap  = find_bitmap_in_bitmap(h_usableBitmap, hum_cnt);
//        hum_openBitmap  = find_bitmap_in_bitmap_by_sort(h_usableBitmap, hum_cnt, team_local_inst.team_sort_hum);
//    }
//
//    //最终输出需要开启的Bitmap
//    team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT]  = team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT] |
//                                                          temp_openBitmap |
//                                                          hum_openBitmap;
//
//    //full open bitmap
//    if(temp_cascade_flag)
//    {   //温度需要层叠
//        temp_full_usableBitmap = find_bitmap_in_bitmap(temp_openBitmap, (temp_cnt - output_cnt - temp_flag));
//    }
//    else
//    {   //温度不需要层叠
////        temp_full_usableBitmap = find_bitmap_in_bitmap(temp_openBitmap, (temp_cnt - temp_flag));
//        temp_full_usableBitmap = find_bitmap_in_bitmap_by_sort(temp_openBitmap, (temp_cnt - temp_flag), team_local_inst.team_sort_temp);
//    }
//    if(hum_cascade_flag)
//    {   //湿度需要层叠
//        hum_full_usableBitmap  = find_bitmap_in_bitmap(hum_openBitmap, (hum_cnt - output_cnt - hum_flag));
//    }
//    else
//    {   //湿度不需要层叠
////        hum_full_usableBitmap  = find_bitmap_in_bitmap(hum_openBitmap, (hum_cnt - hum_flag));
//        hum_full_usableBitmap  = find_bitmap_in_bitmap_by_sort(hum_openBitmap, (hum_cnt - hum_flag), team_local_inst.team_sort_hum);
//    }
//
//    if(temp_cascade_flag)
//    {   //温度需要层叠
//        team_local_inst.team_bitmap[TEAM_BITMAP_TEMP_FULL_REQ_OUT] = team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT] |
//                                                                     temp_full_usableBitmap;
//    }
//    else
//    {   //温度不需要层叠
//        team_local_inst.team_bitmap[TEAM_BITMAP_TEMP_FULL_REQ_OUT] = temp_full_usableBitmap;
//    }
//    if(hum_cascade_flag)
//    {   //湿度需要层叠
//        team_local_inst.team_bitmap[TEAM_BITMAP_HUM_FULL_REQ_OUT] = team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT] |
//                                                                    hum_full_usableBitmap;
//    }
//    else
//    {   //湿度不需要层叠
//        team_local_inst.team_bitmap[TEAM_BITMAP_HUM_FULL_REQ_OUT] = hum_full_usableBitmap;
//    }
////    rt_kprintf("%x %x %x\n", team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ], temp_openBitmap, temp_full_usableBitmap);
//    //非全开的地址(1~32)
//    if(temp_cascade_flag)
//    {   //温度需要层叠
//        team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] = ((uint16_t)team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] & 0xff) |
//                                                           ((find_addr_in_bitmap(temp_openBitmap & (~temp_full_usableBitmap))) << 8);
//    }
//    else
//    {   //温度不需要层叠
//        team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] = ((uint16_t)team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ] & 0xff) |
//                                                           ((find_addr_in_bitmap(temp_openBitmap & (~temp_full_usableBitmap))) << 8);
//    }
//    if(hum_cascade_flag)
//    {   //湿度需要层叠
//        team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ]  = ((uint16_t)team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] & 0xff) |
//                                                           ((find_addr_in_bitmap(hum_openBitmap & (~hum_full_usableBitmap))) << 8);
//    }
//    else
//    {   //湿度不需要层叠
//        team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ]  = ((uint16_t)team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ] & 0xff)|
//                                                           ((find_addr_in_bitmap(hum_openBitmap & (~hum_full_usableBitmap))) << 8);
//    }
//}

////static void team_test(uint32_t test_dev,uint8_t test_num)
////{
//////    team_update_output(test_dev, test_num);
////    test_dev = test_dev;
////    test_num = test_num;
////    team_update_output();
////}

//static void show_team_info(void)
//{
//		extern sys_reg_st		g_sys;
//    extern local_reg_st l_sys;
//		uint16_t i;
//		rt_kprintf("Team---fsm   signal    addr    enmode    tmode  status      confm	  OLSta  ConfSta  MFSta  OutSta   FinalOut\n");
//		rt_kprintf("       %d      %d        %d       %x          %x      %x       %x               %x    %x      %x       %x        %x\n",
//		team_local_inst.team_fsm,
//		team_local_inst.team_signal,
//		g_sys.config.team.addr,
//		g_sys.config.team.team_en,
//		((team_local_inst.team_config[TEAM_CONF_MODE] >> TEAM_CONF_TMMOD_BPOS) & 0x0003),
//		team_local_inst.team_table[g_sys.config.team.addr - 1][TEAM_TAB_STATUS],
//		team_local_inst.team_config[TEAM_CONF_MODE],
//    team_local_inst.team_bitmap[TEAM_BITMAP_ONLINE],
//    team_local_inst.team_bitmap[TEAM_BITMAP_CONFD],
//    team_local_inst.team_bitmap[TEAM_BITMAP_MALFUN_DEV],
//    team_local_inst.team_bitmap[TEAM_BITMAP_OUTPUT],
//    team_local_inst.team_bitmap[TEAM_BITMAP_FINAL_OUT]);

//		rt_kprintf("S-ID    Stat       Temp      Hum      F_Temp      Check    TempReq    HumReq    Timeout\n");
//		for(i=0;i<TEAM_MAX_SLAVE_NUM;i++)
//		{
////				for(j=0;j<TEAM_TAB_MAX;j++)
////				{
//						if(team_local_inst.team_table[i][TEAM_TAB_TIMEOUT] == 0)
//						{
//								continue;
//						}
//						else
//						{
//								rt_kprintf(" %2d     %4x   ",(i+1),team_local_inst.team_table[i][TEAM_TAB_STATUS]);
//								rt_kprintf("   %4d   ",(int16_t)team_get_th(TEAM_TEMP_REQ_TEMP, *((uint32_t *)&team_local_inst.team_table[i][TEAM_TAB_TEMP])));
//								rt_kprintf("   %4d   ",(int16_t)team_get_th(TEAM_HUM_REQ_HUM, *((uint32_t *)&team_local_inst.team_table[i][TEAM_TAB_TEMP])));
//								rt_kprintf("   %4d   ",(int16_t)team_get_th(TEAM_FAN_REQ_TEMP, *((uint32_t *)&team_local_inst.team_table[i][TEAM_TAB_TEMP])));
//								rt_kprintf("   %4x   ",team_local_inst.team_table[i][TEAM_TAB_RESERVE]);
////								rt_kprintf("   %4x   ",team_local_inst.team_table[i][TEAM_TAB_RUN_TIME]);
////								rt_kprintf("   %4x   ",team_local_inst.team_table[i][TEAM_TAB_STOP_TIME]);
//								rt_kprintf("   %4d   ",(int16_t)team_local_inst.team_table[i][TEAM_TAB_TEMP_REQ]);
//								rt_kprintf("   %4d   ",(int16_t)team_local_inst.team_table[i][TEAM_TAB_HUM_REQ]);
//								rt_kprintf("   %4d   ",team_local_inst.team_table[i][TEAM_TAB_TIMEOUT]);
//								rt_kprintf("\n");
////						}
//						}
//		}
//    rt_kprintf("-------------------------------------------------------\n");
//    rt_kprintf("TC    Mode      TP      TD     TS     HP     HD    HS     AHT    ALT     AHH    ALH    \n");
//		rt_kprintf("     %4x       %3d     %3d    %3d    %3d    %3d    %3d    %3d    %3d    %3d    %3d    \n",
//		team_local_inst.team_config[TEAM_CONF_MODE],
//		team_local_inst.team_config[TEAM_CONF_TEMP_PRECISION],
//		team_local_inst.team_config[TEAM_CONF_TEMP_DEADBAND],
//		team_local_inst.team_config[TEAM_CONF_TEMP_SETVAL],
//		team_local_inst.team_config[TEAM_CONF_HUM_PRECISION],
//		team_local_inst.team_config[TEAM_CONF_HUM_DEADBAND],
//		team_local_inst.team_config[TEAM_CONF_HUM_SETVAL],
//		team_local_inst.team_config[TEAM_CONF_ALARM_HI_TEMP_PARAM],
//		team_local_inst.team_config[TEAM_CONF_ALARM_LO_TEMP_PARAM],
//		team_local_inst.team_config[TEAM_CONF_ALARM_HI_HUM_PARAM],
//		team_local_inst.team_config[TEAM_CONF_ALARM_LO_HUM_PARAM]);
//		rt_kprintf("TCNT     RP     RT   FAN_M	FAN_T	FAN_D	FAN_P	FLOW_S  FLOW_D   FLOW_SETP  FLOW_DELAY  BTMSET BTMSTS	\n");
//		rt_kprintf("%3x    %3x    %3x    	%3x    %3d    %3d    %3d    %3d    %3d    %3d    %3d    %3x    %3x\n",
//		team_local_inst.team_config[TEAM_CONF_CNT],
//		team_local_inst.team_config[TEAM_CONF_ROTATE_PERIOD],
//		team_local_inst.team_config[TEAM_CONF_ROTATE_TIME],
//		team_local_inst.team_config[TEAM_CONF_FAN_MODE],
//		team_local_inst.team_config[TEAM_CONF_FAN_TARGET_TEMP],
//		team_local_inst.team_config[TEAM_CONF_FAN_TEMP_DEADBAND],
//		team_local_inst.team_config[TEAM_CONF_FAN_TEMP_PRECISION],
//		team_local_inst.team_config[TEAM_CONF_FAN_SET_FLOW_DIFF],
//		team_local_inst.team_config[TEAM_CONF_FAN_FLOW_DIFF_DEADZONE],
//		team_local_inst.team_config[TEAM_CONF_FAN_FLOW_DIFF_STEP],
//		team_local_inst.team_config[TEAM_CONF_FAN_FLOW_DIFF_DELAY],
//		team_local_inst.team_bitmap[TEAM_BITMAP_RS_SET],
//		team_local_inst.team_bitmap[TEAM_BITMAP_RS_STS]);
//
//		rt_kprintf("TP     Mode        T-req       H-req       F-req       Resv\n");
//		rt_kprintf("       %4x        %3d         %3d         %3d        %4x\n",
//		team_local_inst.team_param[TEAM_PARAM_MODE],
//		team_local_inst.team_param[TEAM_PARAM_TEMP_REQ],
//		team_local_inst.team_param[TEAM_PARAM_HUM_REQ],
//		team_local_inst.team_param[TEAM_PARAM_FAN_REQ],
//		team_local_inst.team_param[TEAM_PARAM_RESERVE]);
//
//    rt_kprintf("-------------------------------------------------------\n");
//    rt_kprintf("T_T    T_H    A_T    A_H    D_T     D_H     Team_T    Team_H    Team_F      T_F_Bitmap    H_F_Bitmap\n");
//    rt_kprintf("%d    %d     %d   %d     %x     %x       %d      %d      %d    %x    %x\n",
//    team_local_inst.team_param[TEAM_PARAM_TOTAL_TEMP_REQ],
//    team_local_inst.team_param[TEAM_PARAM_TOTAL_HUM_REQ],
//    team_local_inst.team_param[TEAM_PARAM_AVER_TEMP_REQ],
//    team_local_inst.team_param[TEAM_PARAM_AVER_HUM_REQ],
//    (uint16_t)team_local_inst.team_param[TEAM_PARAM_DIFF_TEMP_REQ],
//    (uint16_t)team_local_inst.team_param[TEAM_PARAM_DIFF_HUM_REQ],
//    l_sys.require[TEAM_REQ][T_REQ],
//    l_sys.require[TEAM_REQ][H_REQ],
//		l_sys.require[TEAM_REQ][F_REQ],
//    team_local_inst.team_bitmap[TEAM_BITMAP_TEMP_FULL_REQ_OUT],
//    team_local_inst.team_bitmap[TEAM_BITMAP_HUM_FULL_REQ_OUT]);
//
//    rt_kprintf("-------------------------------------------------------\n");
//    rt_kprintf("temp sort : ");
//    for(i = 0; i < TEAM_MAX_SLAVE_NUM; i++)
//    {
//        rt_kprintf("%d ", team_local_inst.team_sort_temp[i]);
//    }
//    rt_kprintf("\n");
//    rt_kprintf("hum sort  : ");
//    for(i = 0; i < TEAM_MAX_SLAVE_NUM; i++)
//    {
//        rt_kprintf("%d ", team_local_inst.team_sort_hum[i]);
//    }
//    rt_kprintf("\n");
//    rt_kprintf("[TEAM_PARAM_TEMP_REQ]: %d [TEAM_PARAM_HUM_REQ]: %d [TEAM_PARAM_FAN_REQ]: %d\n", team_local_inst.team_param[TEAM_PARAM_TEMP_REQ], team_local_inst.team_param[TEAM_PARAM_HUM_REQ], team_local_inst.team_param[TEAM_PARAM_FAN_REQ]);
////		rt_kprintf("%x\n", *((uint32_t *)&team_local_inst.team_table[g_sys.config.team.addr - 1][TEAM_TAB_TEMP]));
//}

//FINSH_FUNCTION_EXPORT(show_team_info, show team status.);
