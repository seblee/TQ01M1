#include <rtthread.h>
#include "sys_conf.h"
#include "local_status.h"
#include "authentication.h"
#include "dio_bsp.h"
#include "pwm_bsp.h"

//手动控制模式比特位操作函数
static void manual_ao_op(uint8_t component_bpos, int16_t value)
{
    extern local_reg_st l_sys;

    l_sys.ao_list[component_bpos][BITMAP_MANUAL] = value;
}

//最终数字输出比特位操作函数
static void final_ao_op(uint8_t component_bpos, int16_t value)
{
    extern local_reg_st l_sys;

    l_sys.ao_list[component_bpos][BITMAP_FINAL] = value;
}

/**
  * @brief 	output control module dout and system status update 
	* @param  none
	* @retval none
  */
//数字输出执行函数
static void oc_do_update(uint32_t new_bitmap)
{
    extern sys_reg_st g_sys;
    extern local_reg_st l_sys;

    uint32_t xor_bitmap, old_bitmap;
    uint16_t i;
    uint32_t u32DO[2];

    old_bitmap = g_sys.status.dout_bitmap[0] | ((uint32_t)g_sys.status.dout_bitmap[1] << 16);

    u32DO[0] = old_bitmap;
    u32DO[1] = new_bitmap;
    old_bitmap = Sts_Remap(u32DO[0], Rep_DO, 0);
    new_bitmap = Sts_Remap(u32DO[1], Rep_DO, 0);
    // rt_kprintf("new_bitmap= %x,old_bitmap= %x,u32DO[0]=%x,u32DO[1]=%x\n", new_bitmap, old_bitmap, u32DO[0], u32DO[1]);
    xor_bitmap = new_bitmap ^ old_bitmap;

    if (xor_bitmap != 0) //if output bitmap changed
    {
        for (i = 0; i < 32; i++)
        {
            if (((xor_bitmap >> i) & 0x00000001) != 0) //do status change
            {
                if (((new_bitmap >> i) & 0x00000001) != 0)
                {
                    dio_set_do(i + 1, Bit_SET);
                }
                else
                {
                    dio_set_do(i + 1, Bit_RESET);
                }
            }
            else //do status no change, continue for loop
            {
                continue;
            }
        }
        g_sys.status.dout_bitmap[0] = new_bitmap;                            //update system dout bitmap
        g_sys.status.dout_bitmap[1] = new_bitmap >> 16;                      //update system dout bitmap
        g_sys.status.ComSta.u16Dout_bitmap[0] = g_sys.status.dout_bitmap[0]; //update system dout bitmap
        g_sys.status.ComSta.u16Dout_bitmap[1] = g_sys.status.dout_bitmap[1]; //update system dout bitmap
    }
    else //output bitmap unchange
    {
        ;
    }
    authen_expire_cd();

    //		uint16_t xor_bitmap,old_bitmap;
    //		uint16_t i;
    //		old_bitmap = g_sys.status.dout_bitmap[0];
    //		xor_bitmap = new_bitmap ^ old_bitmap;
    ////	  rt_kprintf("g_sys.status.dout_bitmap= %x,old_bitmap= %x,xor_bitmap=%x,new_bitmap=%x\n",g_sys.status.dout_bitmap,old_bitmap,xor_bitmap,new_bitmap);
    //		if(xor_bitmap != 0)																					//if output bitmap changed
    //		{
    //				for(i=0;i<16;i++)
    //				{
    //						if(((xor_bitmap>>i)&0x0001) != 0)										//do status change
    //						{
    //								if(((new_bitmap>>i)&0x0001) != 0)
    //								{
    //										dio_set_do(i+1,Bit_SET);
    //								}
    //								else
    //								{
    //										dio_set_do(i+1,Bit_RESET);
    //								}
    //						}
    //						else																											//do status no change, continue for loop
    //						{
    //								continue;
    //						}
    //				}
    //				g_sys.status.dout_bitmap = new_bitmap;		//update system dout bitmap
    //				g_sys.status.ComSta.u16Dout_bitmap = new_bitmap;		//update system dout bitmap
    //		}
    //		else																															//output bitmap unchange
    //		{
    //				;
    //		}
    //		authen_expire_cd();
}

/**
  * @brief 	system output arbitration
	* @param  none
	* @retval final output bitmap
  */
//数字输出仲裁函数
static uint32_t oc_arbitration(void)
{
    extern sys_reg_st g_sys;
    extern local_reg_st l_sys;
    uint16_t cat_bitmap;
    uint16_t sys_bitmap;
    uint16_t alarm_bitmap, bitmap_mask, bitmap_mask_reg, bitmap_mask_reset;
    uint16_t target_bitmap;
    uint16_t final_bitmap;
    uint8_t u8i;
    uint32_t u32Bit_Final;

    for (u8i = 0; u8i <= 1; u8i++)
    {
        bitmap_mask_reset = 0;
        alarm_bitmap = l_sys.bitmap[u8i][BITMAP_ALARM];
        bitmap_mask = l_sys.bitmap[u8i][BITMAP_MASK];

        cat_bitmap = l_sys.bitmap[u8i][BITMAP_REQ];

        if ((g_sys.config.ComPara.u16Manual_Test_En == 0) && (g_sys.config.ComPara.u16Test_Mode_Type == 0)) //if diagnose enable, output manual, else out put concatenated bitmap
        {
            target_bitmap = cat_bitmap;
            l_sys.bitmap[u8i][BITMAP_MANUAL] = l_sys.bitmap[u8i][BITMAP_FINAL];
        }
        else
        {
            target_bitmap = l_sys.bitmap[u8i][BITMAP_MANUAL];
        }

        bitmap_mask_reg = (g_sys.config.general.alarm_bypass_en == 0) ? bitmap_mask : bitmap_mask_reset; //bitmap mask selection, if alarm_bypass_en set, output reset bitmap
                                                                                                         //		rt_kprintf("alarm_bitmap = %X,bitmap_mask = %X,bitmap_mask_reg = %X\n",alarm_bitmap,bitmap_mask,bitmap_mask_reg);

        sys_bitmap = (target_bitmap & ~bitmap_mask_reg) | (alarm_bitmap & bitmap_mask_reg); //sys_out_bitmap output

        final_bitmap = (g_sys.config.ComPara.u16Test_Mode_Type == 0) ? sys_bitmap : l_sys.bitmap[u8i][BITMAP_MANUAL]; //final bitmap selection, if test mode enable, output manual, otherwise sys_bitmap

        l_sys.bitmap[u8i][BITMAP_FINAL] = final_bitmap & g_sys.config.dev_mask.dout[u8i];
        //		rt_kprintf("target_bitmap = %X,bitmap_mask_reg = %X,sys_bitmap = %X,final_bitmap = %X,l_sys = %X\n",target_bitmap,bitmap_mask_reg,sys_bitmap,final_bitmap,l_sys.bitmap[BITMAP_FINAL]);
        //				return l_sys.bitmap[0][BITMAP_FINAL];
    }

    u32Bit_Final = l_sys.bitmap[0][BITMAP_FINAL] | ((uint32_t)l_sys.bitmap[1][BITMAP_FINAL] << 16); //输出
    return u32Bit_Final;
}

//模拟输出仲裁函数
static void oc_ao_arbitration(void)
{
    extern sys_reg_st g_sys;
    extern local_reg_st l_sys;
    uint16_t i;

    if ((g_sys.config.ComPara.u16Manual_Test_En == 0) && (g_sys.config.ComPara.u16Test_Mode_Type == 0)) //if diagnose enable, output manual, else out put concatenated bitmap
    {
        for (i = 0; i < AO_REAL_CNT; i++)
        {
            manual_ao_op(i, g_sys.status.aout[i]);
            final_ao_op(i, l_sys.ao_list[i][BITMAP_REQ]);
        }
    }
    else
    {
        for (i = 0; i < AO_REAL_CNT; i++)
        {
            final_ao_op(i, l_sys.ao_list[i][BITMAP_MANUAL]);
        }
    }
}
//模拟输出执行函数
static void oc_ao_update(void)
{
    //		extern sys_reg_st			g_sys;
    //		extern local_reg_st 	l_sys;
    //		uint16_t i;
    ////		//TEST
    ////		pwm_set_ao(1,50);
    //		for(i=0;i<AO_MAX_CNT;i++)
    //		{
    //				if(g_sys.config.dev_mask.aout&(0x0001<<i))
    //				{
    //					if(g_sys.status.aout[i] != l_sys.ao_list[i][BITMAP_FINAL])
    //					{
    //							g_sys.status.aout[i] = l_sys.ao_list[i][BITMAP_FINAL];
    ////							if(AO_EC_FAN==i)
    ////							{
    ////									pwm_set_ao(i+1,((g_sys.status.aout[i]*g_sys.config.fan.fan_k)/100));
    ////							}
    ////							else
    //							{
    //									pwm_set_ao(i+1,g_sys.status.aout[i]);
    //							}
    //					}
    //				}
    //				else
    //				{
    //					if(g_sys.status.aout[i] != 0)
    //					{
    //							g_sys.status.aout[i] = 0;
    //							pwm_set_ao(i+1,0);
    //					}
    //				}
    //		}
    extern sys_reg_st g_sys;
    extern local_reg_st l_sys;
    uint16_t i;
    //		//TEST
    //		pwm_set_ao(1,50);
    for (i = 0; i < AO_REAL_CNT; i++)
    {
        if (g_sys.config.dev_mask.aout & (0x0001 << i))
        {
            if (g_sys.status.ComSta.u16AO[i] != l_sys.ao_list[i][BITMAP_FINAL])
            {
                g_sys.status.ComSta.u16AO[i] = l_sys.ao_list[i][BITMAP_FINAL];

                pwm_set_ao(i + 1, g_sys.status.ComSta.u16AO[i]);
            }
        }
        else
        {
            if (g_sys.status.ComSta.u16AO[i] != 0)
            {
                g_sys.status.ComSta.u16AO[i] = 0;
                pwm_set_ao(i + 1, 0);
            }
        }
    }
}

/**
  * @brief 	update system output reffering to local bitmaps
	* @param  none
	* @retval none
  */
void oc_update(void)
{
    uint32_t final_bitmap;
    //数字输出仲裁判决
    final_bitmap = oc_arbitration();
    //数字输出执行
    oc_do_update(final_bitmap);
    //模拟输出仲裁
    oc_ao_arbitration();
    //模拟输出执行
    oc_ao_update();
    //    //PWM输出仲裁
    //    oc_pwm_arbitration();
    //    //PWM输出执行
    //    oc_pwm_update();
}
