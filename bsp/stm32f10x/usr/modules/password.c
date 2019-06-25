//#include "password.h"
//#include "i2C_bsp.h"
//#include "global_var.h"
//#include "sys_status.h"
//#include <rtthread.h>
//#include "mbport_cpad.h"
//#include "rtc_bsp.h"
//extern sys_reg_st					g_sys; 	


//uint8_t passward_compare(uint8_t *st1,uint8_t *st2, uint8_t len)
//{
//		uint8_t i,req;
//	
//		req = 1;
//		for(i=0;i<len;i++)
//		{
//				if(*(st1+i) != *(st2+i))
//				{
//						req = 0;
//						break;
//				}
//		}
//		return(req);
//}

//uint8_t cpad_work_mode(uint8_t work_mode,uint16_t day_limit)
//{
//	uint8_t req;
//	req =1;
//	
//	if(day_limit < 1000)
//	{
//			g_sys.status.sys_work_mode.work_mode  = work_mode;
//			g_sys.status.sys_work_mode.limit_day = day_limit;
//			g_sys.status.sys_work_mode.runing_day =0; 
//			g_sys.status.sys_work_mode.runing_hour = 0;
//	}
//	else
//	{
//		req = 0;
//	}

//	//write to EEPROM 
//	if(req == 1)
//	{
//			I2C_EE_BufWrite((uint8_t*)&g_sys.status.sys_work_mode.work_mode,WORK_MODE_EE_ADDR,sizeof(work_mode_st));
//	}
//	return(req);
//}

//uint8_t write_passward (uint8_t*password, uint8_t work_mode,uint16 limit_time)
//{
//	  uint8_t req ,i;
//		req = 1;
//		if(limit_time < 1000)
//		{
//				g_sys.status.sys_work_mode.work_mode  = work_mode;
//				for(i=0;i<4;i++)
//				{
//						g_sys.status.sys_work_mode.pass_word[i] = *(password + i);
//				}
//				g_sys.status.sys_work_mode.limit_day = limit_time;
//				g_sys.status.sys_work_mode.runing_day =0;
//				g_sys.status.sys_work_mode.runing_hour =0;
//			  g_sys.status.sys_work_mode.runing_State = WORK_PASS_SET;//Alair,20170104
////		 rt_kprintf("g_sys.status.sys_work_mode.runing_State:%x\n",g_sys.status.sys_work_mode.runing_State);
//		}
//		else
//		{
//			req = 0;
//		}
//		if(req == 1)
//		{
//				I2C_EE_BufWrite((uint8_t*)&g_sys.status.sys_work_mode.work_mode,WORK_MODE_EE_ADDR,sizeof(work_mode_st));
//		}
//		return(req);
//}



//void init_work_mode(void)
//{
//		//读取5级密码状态参数等
//		I2C_EE_BufRead((uint8_t*)&g_sys.status.ControlPassword.Grade_Manage,CONTROLPASSWORD_EE_ADDR,sizeof(ControlPassword_st));
//		if((g_sys.status.ControlPassword.Grade_Manage!=GRADE_POWERON)&&(g_sys.status.ControlPassword.Grade_Manage!=GRADE_1)&&
//				(g_sys.status.ControlPassword.Grade_Manage!=GRADE_2)&&(g_sys.status.ControlPassword.Grade_Manage!=GRADE_3)&&
//				(g_sys.status.ControlPassword.Grade_Manage!=GRADE_4)&&(g_sys.status.ControlPassword.Grade_Manage!=GRADE_LOCK))
//		{
//			   g_sys.status.ControlPassword.Grade_Manage=GRADE_OPEN;
//			  
//		}
//		
//}
////运行时间计算
//void work_mode_runtime(time_t Starttime)
//{
//		uint32_t Second = 0;
//		static uint16_t u16Second = 0;
////	
////	if( sys_get_pwr_sts() ==1)//开机状态
////	{
////	    if(g_sys.status.ControlPassword.Remain_day != 0)
////	    {
////				g_sys.status.ControlPassword.Run_second = second;
////				if(second++ >3600)
////				{
////						second = 0;
////						g_sys.status.ControlPassword.Run_hour++;
////						//save time to eeprom
////						I2C_EE_BufWrite((uint8_t*)&g_sys.status.ControlPassword.Grade_Manage,CONTROLPASSWORD_EE_ADDR,sizeof(ControlPassword_st));
////				}
////				if(g_sys.status.ControlPassword.Run_hour >= 24)
////				{
////						g_sys.status.ControlPassword.Run_hour = 0 ;
////						g_sys.status.ControlPassword.Run_day++;
////						//save time to eeprom  
////						I2C_EE_BufWrite((uint8_t*)&g_sys.status.ControlPassword.Grade_Manage,CONTROLPASSWORD_EE_ADDR,sizeof(ControlPassword_st));
////				}
////			}	
////	}
//		
//		if(g_sys.status.ControlPassword.Remain_day != 0)
//		{
//			if(Starttime!=0)
//			{
//				Second =g_sys.status.sys_info.Sys_Time.u32Systime-Starttime;
//			
//				g_sys.status.ControlPassword.Run_day=Second/86400;
//				g_sys.status.ControlPassword.Run_hour=(Second%86400)/3600;			
//				g_sys.status.ControlPassword.Run_second = Second%3600;
//				if(u16Second++ >= 15*60)//15分钟存一次,EE寿命
//				{
//						u16Second=0;
//						//save time to eeprom  
//						I2C_EE_BufWrite((uint8_t*)&g_sys.status.ControlPassword.Grade_Manage,CONTROLPASSWORD_EE_ADDR,sizeof(ControlPassword_st));
//				}				
//			}
//		}		
////		rt_kprintf("u32Systime = %x,Starttime = %x,Second= %x,Remain_day=%d\n",g_sys.status.sys_info.Sys_Time.u32Systime,Starttime,Second,g_sys.status.ControlPassword.Remain_day);
//		return ;
//}

//void work_mode_manage(void)
//{	
//		static uint16_t work_mode = GRADE_OPEN;
//	
//			switch(work_mode)
//			{
//				case GRADE_POWERON://开机管控
//								g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_LOCK;//锁屏
//								g_sys.status.ControlPassword.Run_day = 0;
//								g_sys.status.ControlPassword.Run_hour = 0;		
//				break;
//				case GRADE_1:
//					  if(g_sys.status.ControlPassword.Password_Grade[0].Day == 0)
//						{					      
//								g_sys.status.ControlPassword.Grade_Manage = GRADE_2;	
//							  I2C_EE_BufWrite((uint8_t*)&g_sys.status.ControlPassword.Grade_Manage,CONTROLPASSWORD_EE_ADDR,sizeof(ControlPassword_st));
//							  break;							  
//						}
//						work_mode_runtime(g_sys.status.ControlPassword.Password_Grade[0].Starttime);//运行时间计算			
//						
//						if(g_sys.status.ControlPassword.Password_Grade[0].Day > g_sys.status.ControlPassword.Run_day)
//						{
//						g_sys.status.ControlPassword.Remain_day =g_sys.status.ControlPassword.Password_Grade[0].Day-g_sys.status.ControlPassword.Run_day;
//						}
//						else
//						{
//							g_sys.status.ControlPassword.Remain_day = 0;
//						}
//						g_sys.status.ControlPassword.Run_State&=0xFE00;//状态标识		
//						if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_0)
//						{ 
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_ZERO;
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_LOCK;//锁屏
////											if(!(g_sys.status.alarm_bitmap[5]&WORK_LIMIT_CATION))//设置告警状态位
////											{
////													g_sys.status.alarm_bitmap[5]|=WORK_LIMIT_CATION;										
////											}
//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_1)
//						{

//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_1;//状态标识		

//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_3)
//						{
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_3;//状态标识		

//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_7)
//						{
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_7;//状态标识		
//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_15)
//						{
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_15;//状态标识		
//						}
//				break;
//				case GRADE_2:
//					  if(g_sys.status.ControlPassword.Password_Grade[1].Day == 0)
//						{					      
//								g_sys.status.ControlPassword.Grade_Manage = GRADE_3;
//							  I2C_EE_BufWrite((uint8_t*)&g_sys.status.ControlPassword.Grade_Manage,CONTROLPASSWORD_EE_ADDR,sizeof(ControlPassword_st));
//							  break;							  
//						}
//						work_mode_runtime(g_sys.status.ControlPassword.Password_Grade[1].Starttime);//运行时间计算	
//						
//						if(g_sys.status.ControlPassword.Password_Grade[1].Day > g_sys.status.ControlPassword.Run_day)
//						{
//						g_sys.status.ControlPassword.Remain_day =g_sys.status.ControlPassword.Password_Grade[1].Day-g_sys.status.ControlPassword.Run_day;
//						}
//						else
//						{
//							g_sys.status.ControlPassword.Remain_day = 0;
//						}		
//						g_sys.status.ControlPassword.Run_State&=0xFE00;//状态标识		
//						if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_0)
//						{ 
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_ZERO;
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_LOCK;//锁屏
////											if(!(g_sys.status.alarm_bitmap[5]&WORK_LIMIT_CATION))//设置告警状态位
////											{
////													g_sys.status.alarm_bitmap[5]|=WORK_LIMIT_CATION;										
////											}
//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_1)
//						{

//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_1;//状态标识		


//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_3)
//						{
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_3;//状态标识		

//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_7)
//						{
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_7;//状态标识		
//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_15)
//						{
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_15;//状态标识		
//						}
//				break;
//				case GRADE_3:
//					  if(g_sys.status.ControlPassword.Password_Grade[2].Day == 0)
//						{					      
//								g_sys.status.ControlPassword.Grade_Manage = GRADE_4;
//							  I2C_EE_BufWrite((uint8_t*)&g_sys.status.ControlPassword.Grade_Manage,CONTROLPASSWORD_EE_ADDR,sizeof(ControlPassword_st));
//							  break;							  
//						}
//						work_mode_runtime(g_sys.status.ControlPassword.Password_Grade[2].Starttime);//运行时间计算
//						if(g_sys.status.ControlPassword.Password_Grade[2].Day > g_sys.status.ControlPassword.Run_day)
//						{
//						g_sys.status.ControlPassword.Remain_day =g_sys.status.ControlPassword.Password_Grade[2].Day-g_sys.status.ControlPassword.Run_day;
//						}
//						else
//						{
//							g_sys.status.ControlPassword.Remain_day = 0;
//						}		
//						g_sys.status.ControlPassword.Run_State&=0xFE00;//状态标识		
//						if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_0)
//						{ 
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_ZERO;
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_LOCK;//锁屏
////											if(!(g_sys.status.alarm_bitmap[5]&WORK_LIMIT_CATION))//设置告警状态位
////											{
////													g_sys.status.alarm_bitmap[5]|=WORK_LIMIT_CATION;										
////											}
//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_1)
//						{

//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_1;//状态标识		


//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_3)
//						{
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_3;//状态标识		

//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_7)
//						{
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_7;//状态标识		
//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_15)
//						{
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_15;//状态标识		
//						}
//				break;
//				case GRADE_4:
//					  if(g_sys.status.ControlPassword.Password_Grade[3].Day == 0)
//						{					      
//								g_sys.status.ControlPassword.Grade_Manage = GRADE_OPEN;
//							  I2C_EE_BufWrite((uint8_t*)&g_sys.status.ControlPassword.Grade_Manage,CONTROLPASSWORD_EE_ADDR,sizeof(ControlPassword_st));
//							  break;							  
//						}
//						work_mode_runtime(g_sys.status.ControlPassword.Password_Grade[3].Starttime);//运行时间计算
//						if(g_sys.status.ControlPassword.Password_Grade[3].Day > g_sys.status.ControlPassword.Run_day)
//						{
//						g_sys.status.ControlPassword.Remain_day =g_sys.status.ControlPassword.Password_Grade[3].Day-g_sys.status.ControlPassword.Run_day;
//						}
//						else
//						{
//							g_sys.status.ControlPassword.Remain_day = 0;
//						}						
//						g_sys.status.ControlPassword.Run_State&=0xFE00;//状态标识			
//						if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_0)
//						{ 
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_ZERO;
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_LOCK;//锁屏
////											if(!(g_sys.status.alarm_bitmap[5]&WORK_LIMIT_CATION))//设置告警状态位
////											{
////													g_sys.status.alarm_bitmap[5]|=WORK_LIMIT_CATION;										
////											}
//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_1)
//						{

//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_1;//状态标识		


//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_3)
//						{
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_3;//状态标识		

//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_7)
//						{
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_7;//状态标识		
//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_15)
//						{
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_15;//状态标识		
//						}
//						else if(g_sys.status.ControlPassword.Remain_day <=WORK_REMAIN_30)
//						{
//									g_sys.status.ControlPassword.Run_State|=WORK_LIMIT_DAY_30;//状态标识		
//						}
//				break;
//				case GRADE_OPEN:
//						g_sys.status.ControlPassword.Run_State=	0x00;				
//				break;	
//				default:
//					   ;
//				break;
//		}
//		work_mode = g_sys.status.ControlPassword.Grade_Manage;
//		
////		rt_kprintf("Grade_Manage = %x,Run_State= %x\n",g_sys.status.ControlPassword.Grade_Manage,g_sys.status.ControlPassword.Run_State);
///*		
//		rt_kprintf("g_sys.status.ControlPassword.Password_Poweron = %x\n",g_sys.status.ControlPassword.Password_Poweron);
//		rt_kprintf("g_sys.status.ControlPassword.Password_Grade[0][0] = %x\n",g_sys.status.ControlPassword.Password_Grade[0][0]);
//		rt_kprintf("g_sys.status.ControlPassword.Password_Grade[0][1] = %x\n",g_sys.status.ControlPassword.Password_Grade[0][1]);
//		rt_kprintf("g_sys.status.ControlPassword.Password_Grade[1][0] = %x\n",g_sys.status.ControlPassword.Password_Grade[1][0]);
//		rt_kprintf("g_sys.status.ControlPassword.Password_Grade[1][1] = %x\n",g_sys.status.ControlPassword.Password_Grade[1][1]);
//		rt_kprintf("g_sys.status.ControlPassword.Password_Grade[2][0] = %x\n",g_sys.status.ControlPassword.Password_Grade[2][0]);
//		rt_kprintf("g_sys.status.ControlPassword.Password_Grade[2][1] = %x\n",g_sys.status.ControlPassword.Password_Grade[2][1]);
//		rt_kprintf("g_sys.status.ControlPassword.Password_Grade[3][0] = %x\n",g_sys.status.ControlPassword.Password_Grade[3][0]);
//		rt_kprintf("g_sys.status.ControlPassword.Password_Grade[3][1] = %x\n",g_sys.status.ControlPassword.Password_Grade[3][1]);
//		rt_kprintf("g_sys.status.ControlPassword.Remain_day = %x\n",g_sys.status.ControlPassword.Remain_day);
//		rt_kprintf("g_sys.status.ControlPassword.Run_day = %x\n",g_sys.status.ControlPassword.Run_day);
//		rt_kprintf("g_sys.status.ControlPassword.Run_hour = %x\n",g_sys.status.ControlPassword.Run_hour);
//		rt_kprintf("g_sys.status.ControlPassword.Run_second = %x\n",g_sys.status.ControlPassword.Run_second);
//		rt_kprintf("g_sys.status.ControlPassword.Run_State = %x\n",g_sys.status.ControlPassword.Run_State);

//*/
//}

//uint8_t Passward_Verify(uint16_t MBBuff)
//{
//		uint8_t req;
//	  uint8_t i = 5;
//	  uint16_t grade_temp[5] = {GRADE_1,GRADE_2,GRADE_3,GRADE_4,GRADE_OPEN};
//		
//		extern  USHORT   cpad_usSRegHoldBuf[];
//		req=0;
//	  //rt_kprintf("MBBuff = %.5d,cpad_usSRegHoldBuf[1] = %.5d\n",MBBuff,cpad_usSRegHoldBuf[1]);	
////				g_sys.status.ControlPassword.Grade_Manage=GRADE_OPEN;					
//		switch(g_sys.status.ControlPassword.Grade_Manage)//管控等级
//		{
//			case GRADE_POWERON:
//				i = 0;
//				break;
//			case GRADE_1:
//				i = 1;
//				break;			
//			case GRADE_2:
//				i = 2;
//			break;
//			case GRADE_3:
//				i = 3;
//				break;
//			case GRADE_4:
//				i = 4;
//				break;
//			default:
//				i = 5;
//				break;	
//		}
//		
//		for(;i < 5 ;i++)
//		{
//				if(0 == i)
//				{
//						if(MBBuff==g_sys.status.ControlPassword.Password_Poweron)//开机管控
//						{
//							  g_sys.status.ControlPassword.Grade_Manage=GRADE_1;
//							  g_sys.status.ControlPassword.Password_Grade[0].Starttime=g_sys.status.sys_info.Sys_Time.u32Systime;		
//								g_sys.status.ControlPassword.Password_Grade[1].Starttime=g_sys.status.sys_info.Sys_Time.u32Systime;
//								g_sys.status.ControlPassword.Password_Grade[2].Starttime=g_sys.status.sys_info.Sys_Time.u32Systime;
//								g_sys.status.ControlPassword.Password_Grade[3].Starttime=g_sys.status.sys_info.Sys_Time.u32Systime;								
//							  break;
//						}
//				}
//				else
//				{
//						if(MBBuff==g_sys.status.ControlPassword.Password_Grade[i - 1].Password)
//						{
//							  g_sys.status.ControlPassword.Grade_Manage=grade_temp[i];
//							  g_sys.status.ControlPassword.Password_Grade[i].Starttime=g_sys.status.sys_info.Sys_Time.u32Systime;				
//							  break;
//						}
//				}
//		}
//		if(i >= 5)			//密码认证都不相等
//		{
//				req=1;					
//		}
//		if(!req)
//		{
//				g_sys.status.ControlPassword.Run_State&=~WORK_PASS_ALL;//解除锁屏
//				g_sys.status.ControlPassword.Run_day = 0;
//				g_sys.status.ControlPassword.Run_hour = 0;			
//				g_sys.status.ControlPassword.Run_second = 0;	
//			
//				g_sys.status.alarm_bitmap[5]&=~WORK_LIMIT_CATION;
//				//save time to eeprom
//				I2C_EE_BufWrite((uint8_t*)&g_sys.status.ControlPassword.Grade_Manage,CONTROLPASSWORD_EE_ADDR,sizeof(ControlPassword_st));
//		}
//		return(req);
//}
//uint8_t Write_Controlpassward (uint16_t*MBBuffer)
//{
//	  uint8_t req;
//		req = 1;
//	
//		if((MBBuffer[2] < LIMIT_DAY_MAX)&&(MBBuffer[4] < LIMIT_DAY_MAX)&&
//			(MBBuffer[6] < LIMIT_DAY_MAX)&&(MBBuffer[8] < LIMIT_DAY_MAX))
//		{
//				memcpy(&g_sys.status.ControlPassword.Grade_Manage,&MBBuffer[0],4);
//				memcpy(&g_sys.status.ControlPassword.Password_Grade[0],&MBBuffer[2],4);
//				memcpy(&g_sys.status.ControlPassword.Password_Grade[1],&MBBuffer[4],4);
//				memcpy(&g_sys.status.ControlPassword.Password_Grade[2],&MBBuffer[6],4);
//				memcpy(&g_sys.status.ControlPassword.Password_Grade[3],&MBBuffer[8],4);
//				g_sys.status.ControlPassword.Run_day=0;
//				g_sys.status.ControlPassword.Run_hour=0;
//				g_sys.status.ControlPassword.Run_second=0;
//				g_sys.status.ControlPassword.Run_State = WORK_PASS_SET;;
//				I2C_EE_BufWrite((uint8_t*)&g_sys.status.ControlPassword.Grade_Manage,CONTROLPASSWORD_EE_ADDR,sizeof(ControlPassword_st));
//		}
//		else
//		{
//			req = 0;
//		}
//		return(req);
//}

//uint8_t get_work_mode_power_state(void)
//{
//		if((g_sys.status.ControlPassword.Grade_Manage == GRADE_POWERON)||(g_sys.status.ControlPassword.Grade_Manage==GRADE_LOCK))
//		{
//				return(0);
//		}
//		else
//		{
//			 return(1);
//		}
//}


