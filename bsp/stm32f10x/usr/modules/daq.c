#include <rtthread.h>
#include <components.h>
#include "daq.h"
#include "sys_conf.h"
#include "sys_status.h"
#include "calc.h"
#include "user_mb_app.h"
#include "adc_bsp.h"
#include "calc.h"

#define NTC_TEMP_SCALE 191
#define NTC_TEMP_OFFSET 39
#define NTC_TEMP_DT 15
#define K_FACTOR_HI_PRESS 174
#define K_FACTOR_LO_PRESS 232

extern sys_reg_st g_sys;

const uint16_t ntc_lookup_tab[NTC_TEMP_SCALE] = {
	193, 204, 215, 227, 239, 252, 265, 279, 294, 309, 324, 340, 357, 375, 393, 411, 431, 451, 471,
	492, 514, 537, 560, 584, 609, 635, 661, 687, 715, 743, 772, 801, 831, 862, 893, 925, 957, 991,
	1024, 1058, 1093, 1128, 1164, 1200, 1236, 1273, 1310, 1348, 1386, 1424, 1463, 1501, 1540,
	1579, 1618, 1657, 1697, 1736, 1775, 1815, 1854, 1893, 1932, 1971, 2010, 2048, 2087, 2125,
	2163, 2200, 2238, 2274, 2311, 2347, 2383, 2418, 2453, 2488, 2522, 2556, 2589, 2622, 2654,
	2685, 2717, 2747, 2777, 2807, 2836, 2865, 2893, 2921, 2948, 2974, 3000, 3026, 3051, 3075,
	3099, 3123, 3146, 3168, 3190, 3212, 3233, 3253, 3273, 3293, 3312, 3331, 3349, 3367, 3384,
	3401, 3418, 3434, 3450, 3465, 3481, 3495, 3510, 3524, 3537, 3551, 3564, 3576, 3588, 3600,
	3612, 3624, 3635, 3646, 3656, 3667, 3677, 3686, 3696, 3705, 3714, 3723, 3732, 3740, 3748,
	3756, 3764, 3772, 3779, 3786, 3793, 3800, 3807, 3813, 3820, 3826, 3832, 3838, 3844, 3849,
	3855, 3860, 3865, 3870, 3875, 3880, 3884, 3889, 3893, 3898, 3902, 3906, 3910, 3914, 3918,
	3922, 3925, 3929, 3932, 3936, 3939, 3942, 3945, 3949, 3952, 3954, 3957, 3960, 3963, 3966,
	3968, 3971, 3973};

static int16_t calc_ntc(uint16_t adc_value, int16_t adjust)
{
	int16_t ntc_temp;
	int16_t index;
	uint16_t offset;
	adc_value = 4096 - adc_value;
	index = bin_search((uint16_t *)ntc_lookup_tab, (NTC_TEMP_SCALE - 1), adc_value);
	if (index < 0)
	{
		return ABNORMAL_VALUE;
	}
	else
	{
		offset = (adc_value - ntc_lookup_tab[index - 1]) * 10 / (ntc_lookup_tab[index] - ntc_lookup_tab[index - 1]);
		ntc_temp = (index - NTC_TEMP_OFFSET) * 10 + offset + adjust - NTC_TEMP_DT;
		return ntc_temp;
	}
}

int16_t get_current_temp()
{
	int16_t temp = 0;

	return (temp);
}

uint16_t get_current_hum()
{
	uint16_t sum = 0;
	uint8_t index = 0, i;

	for (i = MB_DEV_TH_RETURN_START; i <= MB_DEV_TH_RETURN_END; i++)
	{
		if (((g_sys.config.dev_mask.mb_comp) & (0X01 << i)) &&
			(sys_get_mbm_online(i) == 1)) //配置位 online
		{
			if (sys_get_remap_status(SENSOR_STS_REG_NO, i) == 0) //报警位
			{
				sum += (int16_t)(g_sys.status.mbm.tnh[i].hum);
				index++;
			}
		}
	}

	if (index != 0)
	{
		return (sum / index);
	}
	else
	{
		return (0x7fff);
	}
}

//#define Rs  200
//static int16_t calc_ap_ai(uint16_t adc_value,int16_t cali)
//{
//	int16_t relvaule;
//	relvaule = ((825*adc_value)/(40*Rs)) - 25 + (int16_t)(cali);//(adc_value*1330-100)/(1024*Rs);
//	if(relvaule<0)
//	{
//			relvaule = ABNORMAL_VALUE;
//	}
//	if(relvaule>200)
//	{
//			relvaule = 0x7fff;
//	}
//	return(relvaule);
//}

//k_factor has 3-valid-digitals integer
static int16_t calc_hi_press_ai(uint16_t adc_value, uint16_t k_factor, int16_t cali)
{
	int32_t ret_val = 0;
	//As Vo/Vcc*100 = K*P+10, Vadc*4/Vcc*100 = K*P+10. Vadc = 3.3/4096 * N.
	//(((3.3/4096)*N)*4)/Vcc*100 = K*P + 10;
	ret_val = ((3.3 * adc_value * 100 * 4) / (4096 * 5) - 10) * 1000 / k_factor + (int16_t)(cali); //unit is BAR(aka. 0.1MPaG)
	if (ret_val <= 0)
	{
		ret_val = ABNORMAL_VALUE;
	}
	return (int16_t)ret_val;
}
//测量UV光强
static int16_t Calc_UV_ai(uint16_t adc_value, uint16_t k_factor, int16_t cali)
{
	int32_t ret_val = 0;
	//As Vo/Vcc*100 = K*P+10, Vadc*4/Vcc*100 = K*P+10. Vadc = 3.3/4096 * N.
	//(((3.3/4096)*N)*4)/Vcc*100 = K*P ;
	ret_val = ((3.3 * adc_value * 100 * 4) / (4096 * 5) - 10) * 1000 / k_factor + (int16_t)(cali); //unit is BAR(aka. 0.1MPaG)
	if (ret_val <= 0)
	{
		ret_val = ABNORMAL_VALUE;
	}
	return (int16_t)ret_val;
}
////water flow supper voice wave
//static int16_t calc_water_flow_ai(uint16_t adc_value,int16_t cali)
//{
//		uint16_t q_max=450;//45.0m3/h
//		int32_t ret_val=0;
//		//AS Q = (Qmax/16)*(Ima-4)
//		//Ima= 1000*V200/R;
//		// V200= 4*Vadc
//		// Vadc =3.3*adc/4096
//		// Q =Qmax/16(16.5*adc/1024 - 4)
//		ret_val = (q_max/16)*((16.5*adc_value/1024)-4) + (int16_t)cali;
//
//		 if(ret_val <=0)
//		{
//				ret_val =0x7fff;
//		}
//		if(ret_val >=q_max + cali+5)
//		{
//				ret_val =0x7fff;
//		}
//    return (int16_t)ret_val;
//}

//static int16_t calc_water_press_ai(uint16_t adc_value,int16_t cali)
//{
//	  int32_t ret_val = 0;
//		uint16_t k_factor =7735;
//		uint16_t b_factor = 1784;
//		//AS vo/(vcc*100) = 77.35*P + 17.84
//		// VO/VCC = 7735*P + 1784;
//		//VO = 4*Vadc*3.3V/4096;
//		//VCC =5V
//		// P = ((4*Vadc*3.3/4096)/5+1784)/7735;
//    //As Vo/Vcc*100 = K*P+10, Vadc*4/Vcc*100 = K*P+10. Vadc = 3.3/4096 * N.
//    //(((3.3/4096)*N)*4)/Vcc*100 = K*P + 10;
//    //ret_val = ((3.3*adc_value*100*4)/(4096*5) - 10)*1000/k_factor; //unit is BAR(aka. 0.1MPaG)
//		ret_val = ((4*adc_value*3.3/4096)/5+ b_factor)/k_factor + (int16_t) cali; //unit is MPAG
//	  if(ret_val <=0)
//		{
//				ret_val =0x7fff;
//		}
//    return (int16_t)ret_val;
//}

void ADCValProcess(uint16_t *ptrADCval, uint16_t *ptrADCbuf, uint8_t index)
{
	uint8_t i = 0;
	volatile uint16_t ADC_VOL_ave = 0;
	uint16_t ADC_Tmp[MAX_ADBUFEVERY];
	for (i = 0; i < MAX_ADBUFEVERY; i++)
	{
		ADC_Tmp[i] = 0x0000;
		ADC_Tmp[i] = ptrADCbuf[i * AI_MAX_CNT + index];
	}
	quick(ADC_Tmp, 0, 19); //0~35标号,共36个
	for (i = 2; i < 18; i++)
	{
		ADC_VOL_ave += ADC_Tmp[i];
	}
	ADC_VOL_ave >>= 4;
	ptrADCval[index] = ADC_VOL_ave;
}

void ai_sts_update(sys_reg_st *gds_sys_ptr)
{
	extern volatile uint16_t ADC1ConvertedValue[AI_MAX_CNT];
	extern volatile uint16_t ADC1Buff[AI_MAX_CNT * MAX_ADBUFEVERY];
	uint16_t ain_mask_bitmap;
	uint16_t i;
	uint16_t u16ADCRemapValue[AI_MAX_CNT]; //

	ain_mask_bitmap = gds_sys_ptr->config.dev_mask.ain;
	//		ain_mask_bitmap =0x001F;
	//rt_kprintf("ADC1ConvertedValue = %X,V[1] = %X,V[2] = %X,V[3] = %X,V[4] = %X\n",ADC1ConvertedValue[0],ADC1ConvertedValue[1],ADC1ConvertedValue[2],ADC1ConvertedValue[3],ADC1ConvertedValue[4]);
	//	  for(i = 0;i < AI_MAX_CNT;i++)
	//		{
	//				ADCValProcess(ADC1ConvertedValue,ADC1Buff,i);
	//		}
	for (i = 0; i < AI_MAX_CNT; i++)
	{
		u16ADCRemapValue[i] = ADC1ConvertedValue[i];
	}
	//rt_kprintf("ADC1ConvertedValue = %X,V[1] = %X,V[2] = %X,V[3] = %X,V[4] = %X\n",ADC1ConvertedValue[0],ADC1ConvertedValue[1],ADC1ConvertedValue[2],ADC1ConvertedValue[3],ADC1ConvertedValue[4]);
	//rt_kprintf("u16ADCRemapValue = %X,R[1] = %X,R[2] = %X,R[3] = %X,R[4] = %X\n",u16ADCRemapValue[0],u16ADCRemapValue[1],u16ADCRemapValue[2],u16ADCRemapValue[3],u16ADCRemapValue[4]);

	for (i = AI_NTC1; i < AI_MAX_CNT; i++)
	{
		gds_sys_ptr->status.ComSta.u16Ain[i] = (((ain_mask_bitmap >> i) & 0x0001) != 0) ? calc_ntc(u16ADCRemapValue[i], gds_sys_ptr->config.general.ntc_cali[i - AI_NTC1]) : 0;
	}

	if ((ain_mask_bitmap & (0x0001 << AI_SENSOR1)) != 0)
	{
		gds_sys_ptr->status.ComSta.u16Ain[AI_SENSOR1] = calc_hi_press_ai(u16ADCRemapValue[AI_SENSOR1], K_FACTOR_HI_PRESS, gds_sys_ptr->config.general.ai_cali[AI_SENSOR1]);
	}

	//		rt_kprintf("ain[0] = %d,ain[1] = %d,ain[2] = %d,ain[3] = %d,ain[4] = %d\n",gds_sys_ptr->status.ComSta.u16Ain[0],gds_sys_ptr->status.ComSta.u16Ain[1],gds_sys_ptr->status.ComSta.u16Ain[2],gds_sys_ptr->status.ComSta.u16Ain[3],gds_sys_ptr->status.ComSta.u16Ain[4]);

	return;
}
