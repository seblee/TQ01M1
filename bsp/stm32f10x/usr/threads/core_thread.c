#include <rtthread.h>
#include "sys_conf.h"
#include "kits/fifo.h"
#include "local_status.h"
#include "alarms.h"
#include "team.h"
#include "output_ctrl.h"
#include "require_calc.h"
#include "req_execution.h"
#include "sys_status.h"
#include "dio_bsp.h"
//static void top_fsm(void);
static void req_process(void);

void core_thread_entry(void* parameter)
{
		rt_thread_delay(CORE_THREAD_DELAY);
		alarm_acl_init(); 
    inc_pid_param_init();
		sys_option_di_sts(DI_POWER_LOSS_BPOS,1);
		
		while(1)
		{		 
				alarm_acl_exe();
//				top_fsm();
				req_process();				
				oc_update();
//				rt_thread_delay(1000);
				rt_thread_delay(500);
		}			
}

//static void top_fsm_signal_gen(void)
//{
//		extern sys_reg_st		g_sys;
//		extern local_reg_st l_sys;
//		
//		
//		
//}

//static void top_fsm(void)
//{
//		extern sys_reg_st		g_sys;
//		extern local_reg_st l_sys;
////		extern team_local_st team_local_inst;
//		switch(l_sys.t_fsm_state)
//		{
//				case(T_FSM_STATE_IDLE):
//				{
//						if((g_sys.config.team.team_en == 0)&&												//power not enabled
//								(g_sys.config.general.power_mode == 1)&&										//power enabled
//								((g_sys.status.general.sys_error_bitmap & 0x0001) == 0))		//SYS_ERR_FATAL
//						{
//								l_sys.t_fsm_state = T_FSM_STATE_STANDALONE;
//						}
//						else if((g_sys.config.team.team_en == 1)&&
//								((g_sys.status.general.sys_error_bitmap & 0x0003) == 0))		//SYS_ERR_FATAL||SYS_ERR_TEAM
//						{
//								l_sys.t_fsm_state = T_FSM_STATE_TEAM;
//						}
//						else
//						{	
//								l_sys.t_fsm_state = T_FSM_STATE_IDLE;
//						}							
//						break;
//				}
//				case(T_FSM_STATE_STANDALONE):
//				{
//						if(	((g_sys.config.general.power_mode == 0))||
//								((g_sys.status.general.sys_error_bitmap & 0x0001) != 0))		//SYS_ERR_FATAL
//						{
//								l_sys.t_fsm_state = T_FSM_STATE_IDLE;
//						}
//						else if((g_sys.config.team.team_en == 1)&&
//								((g_sys.status.general.sys_error_bitmap & 0x0003) == 0))		//SYS_ERR_FATAL||SYS_ERR_TEAM
//						{
//								l_sys.t_fsm_state = T_FSM_STATE_TEAM;
//						}
//						else
//						{	
//								l_sys.t_fsm_state = T_FSM_STATE_STANDALONE;
//						}							
//						break;
//				}
//				case(T_FSM_STATE_TEAM):
//				{
//						if( ((g_sys.config.team.team_en == 0)&&(g_sys.config.general.power_mode == 0))||		//power down and team disable
//								((g_sys.status.general.sys_error_bitmap & 0x0001) != 0))				//SYS_ERR_FATAL
//						{
//								l_sys.t_fsm_state = T_FSM_STATE_IDLE;
//						}
//						else if(((g_sys.config.general.power_mode == 1)&&(g_sys.config.team.team_en == 0))||	//power on and team disable
//								((g_sys.status.general.sys_error_bitmap & 0x0002) == 0x0002))		//SYS_ERR_TEAM
//						{
//								l_sys.t_fsm_state = T_FSM_STATE_STANDALONE;
//						}
//						else 
//						{	
//								l_sys.t_fsm_state = T_FSM_STATE_TEAM;
//						}							
//						break;
//				}
//				default:
//				{	
//						l_sys.t_fsm_state = T_FSM_STATE_IDLE;
//				}				
//		}		
//}

//需求处理
static void req_process(void)
{
		extern sys_reg_st		g_sys;
		int16_t i16Current_Temp;
		int16_t i16Current_Hum;
		
		i16Current_Temp=(int16_t)g_sys.status.ComSta.u16TH[0].Temp;
		i16Current_Hum=(int16_t)g_sys.status.ComSta.u16TH[0].Hum;

//		rt_kprintf("i16Current_Temp=%d,i16Current_Hum= %d\n",i16Current_Temp,i16Current_Hum);			
		req_execution(i16Current_Temp,i16Current_Hum);		//execute requirement 				
}
