#include "sys_def.h"
#include "reg_map_check.h"
#include "user_mb_app.h"
extern sys_reg_st g_sys;

////compressor0_uper_speed check
//uint8_t comp_uper_spd_chk(uint16_t pram)
//{
//	if(pram > g_sys.config.compressor.speed_lower_lim)
//	{
//			return(1);
//	}
//	return(0);
//}
////compressor0 lower speed check
//uint8_t comp_low_spd_chk(uint16_t pram)
//{
//	if(pram < g_sys.config.compressor.speed_upper_lim)
//	{
//			return(1);
//	}
//	return(0);
//}

//uint8_t water_valve_set_chk(uint16_t param)
//{
//    if((param <= g_sys.config.water_valve.max_opening) &&
//       (param >= g_sys.config.water_valve.min_opening))
//    {
//        return (1);
//    }
//    return (0);
//}

//uint8_t water_valve_min_chk(uint16_t param)
//{
//    if(param < g_sys.config.water_valve.max_opening)
//    {
//        return(1);
//    }
//    return (0);
//}

//uint8_t water_valve_max_chk(uint16_t param)
//{
//    if(param > g_sys.config.water_valve.min_opening)
//    {
//        return (1);
//    }
//    return (0);
//}

//uint8_t fan_set_spd_chk(uint16_t pram)
//{
//	if((pram <= g_sys.config.fan.max_speed)&&(pram >= g_sys.config.fan.min_speed))
//	{
//			return(1);
//	}
//	return(0);
//}
//uint8_t fan_low_spd_chk(uint16_t pram)
//{
//	if((pram <= g_sys.config.fan.max_speed)&&(pram <= g_sys.config.fan.set_speed))
//	{
//			return(1);
//	}
//	return(0);
//}
//uint8_t fan_uper_spd_chk(uint16_t pram)
//{
//	if((pram >= g_sys.config.fan.min_speed)&&(pram >= g_sys.config.fan.set_speed))
//	{
//			return(1);
//	}
//	return(0);
//}
//uint8_t fan_dehum_min_spd_chk(uint16_t pram)
//{
//	if((pram <= g_sys.config.fan.max_speed)&&(pram >= g_sys.config.fan.min_speed)&&(pram <= g_sys.config.fan.set_speed))
//	{
//			return(1);
//	}
//	return(0);
//}

//uint8_t team_total_num_chk(uint16_t pram)
//{
//	if(pram > g_sys.config.team.backup_num)
//	{
//			return(1);
//	}
//	return(0);
//}
//uint8_t team_back_num_chk(uint16_t pram)
//{
//	if(pram <= (g_sys.config.team.total_num>>1))
//	{
//			return(1);
//	}
//	return(0);
//}

//uint8_t AC_Conf_Write(uint16_t pram,uint8_t Type)
//{
//	extern	mbm_read_st mbm_read_table_01[];
//	uint8_t k;
//
//	switch(Type)
//	{
//		case 0x00:
//		{
//			for(k=0;k < mbm_read_table_01[MBM_DEV_AC_ADDR].reg_w_cnt;k++)
//			{
//					*(mbm_read_table_01[MBM_DEV_AC_ADDR].w_pt[k].conf_Flag)=FALSE;
//			}
//			break;
//		}
//		case 0x01:
//		{
//			for(k=0;k < mbm_read_table_01[MBM_DEV_AC_ADDR].reg_w_cnt;k++)
//			{
//					*(mbm_read_table_01[MBM_DEV_AC_ADDR].w_pt[k].conf_Flag)=TRUE;
//			}
//			break;
//		}
//		case 0x02:
//		{
//			if(mbm_read_table_01[MBM_DEV_AC_ADDR].reg_w_cnt ==0)
//			{
//				return(0);
//			}
//			else
//			{
////							g_sys.status.general.TEST|=0x40;
//				for(k=0;k < mbm_read_table_01[MBM_DEV_AC_ADDR].reg_w_cnt;k++)
//				{
//					if(pram == mbm_read_table_01[MBM_DEV_AC_ADDR].w_pt[k].Conf_addr)
//					{
////									g_sys.status.general.TEST|=0x80;
//						*(mbm_read_table_01[MBM_DEV_AC_ADDR].w_pt[k].conf_Flag)=TRUE;
//					}
//				}
//			}
//			break;
//		}
//		default:
//			break;
//	}
//	return(1);
//}
