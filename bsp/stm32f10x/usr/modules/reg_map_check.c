#include "sys_def.h"
#include "reg_map_check.h"
#include "user_mb_app.h"
extern sys_reg_st g_sys;

uint8_t ColdWater_StartTemp_Chek(uint16_t pram)
{
    if (pram > g_sys.config.ComPara.u16ColdWater_StopTemp)
    {
        return (1);
    }
    return (0);
}

uint8_t ColdWater_StopTemp_Chek(uint16_t pram)
{
    if (pram < g_sys.config.ComPara.u16ColdWater_StartTemp)
    {
        return (1);
    }
    return (0);
}

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
