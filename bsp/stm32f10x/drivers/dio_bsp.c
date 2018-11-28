#ifndef __DIO
#define __DIO
#include <components.h>
#include <rtthread.h>
#include "dio_bsp.h"
#include "led_bsp.h"
#include "local_status.h"

#define DI_MAX_CNT 16
//#define DIN_MASK_MASK         0x003f
//#define DIN_MASK_MASK1        0xffff //屏蔽
//#define DIN_POLARITY_MASK     0xffc0
//#define DI_BUF_DEPTH 					600
//#define	DI_UPDATE_PERIOD			1000
#define DI_BUF_DEPTH 50
#define SAMPLE_INTERVAL 10

//RT_TIMER_TICK_PER_SECOND
typedef struct
{
    uint16_t bitmap[2];
    uint16_t reg_array[2][DI_BUF_DEPTH];
} di_dev_st;

typedef struct
{
    uint16_t bitmap;
} do_dev_st;

typedef struct
{
    di_dev_st din;
    do_dev_st dout;
} dio_dev_st;

typedef struct
{
    uint16_t pin_id;
    void *pin_base;
} pin_map_st;

//static rt_timer_t pwm_slow_timer;

static uint16_t do_set(int16_t pin_id, BitAction value);
#define Pin_Map_In DI_MAX_CNT
const pin_map_st in_pin_map_inst[Pin_Map_In] = //数字输入Pin_Map
    {
        {GPIO_Pin_7, GPIOC},  //DI1
        {GPIO_Pin_8, GPIOC},  //DI2
        {GPIO_Pin_8, GPIOA},  //DI3
                              //		{GPIO_Pin_8, 		GPIOA},		//DI2
                              //		{GPIO_Pin_8, 	  GPIOC},		//DI3
        {GPIO_Pin_6, GPIOC},  //DI4
        {GPIO_Pin_12, GPIOD}, //DI6
        {GPIO_Pin_11, GPIOD}, //DI7
        {GPIO_Pin_13, GPIOD}, //DI5//中下水位
        {GPIO_Pin_14, GPIOD}, //DI8
        {GPIO_Pin_15, GPIOD}, //DI9
        {GPIO_Pin_10, GPIOD}, //DI10
        {GPIO_Pin_15, GPIOB}, //DI11
        {GPIO_Pin_14, GPIOB}, //DI12
        {GPIO_Pin_13, GPIOB}, //DI13
        {GPIO_Pin_12, GPIOB}, //DI14
        {GPIO_Pin_0, GPIOE},  //DI15
        {GPIO_Pin_0, GPIOD},  //DI16
};
#define Pin_Map_Out 21
const pin_map_st out_pin_map_inst[Pin_Map_Out] = //数字输出Pin_Map
    {
#ifdef SYS_HMI_TQ_T10
        {GPIO_Pin_4, GPIOC},  //DO1
        {GPIO_Pin_7, GPIOA},  //DO2
        {GPIO_Pin_6, GPIOA},  //DO3
        {GPIO_Pin_5, GPIOA},  //DO4
        {GPIO_Pin_0, GPIOB},  //DO17//T10,UV_24V,DO5与DO17互换
        {GPIO_Pin_2, GPIOA},  //DO6
        {GPIO_Pin_1, GPIOA},  //DO7
        {GPIO_Pin_0, GPIOA},  //DO8
        {GPIO_Pin_8, GPIOE},  //DO9
        {GPIO_Pin_10, GPIOE}, //DO10
        {GPIO_Pin_11, GPIOE}, //DO11
        {GPIO_Pin_12, GPIOE}, //DO12
        {GPIO_Pin_13, GPIOE}, //DO13
        {GPIO_Pin_14, GPIOE}, //DO14
        {GPIO_Pin_7, GPIOE},  //DO15
        {GPIO_Pin_1, GPIOB},  //DO16

        {GPIO_Pin_4, GPIOA},  //DO5
        {GPIO_Pin_5, GPIOC},  //DO18
        {GPIO_Pin_9, GPIOE},  //DO19-LOCKLED
        {GPIO_Pin_11, GPIOA}, //DO20-PWRCTRL
        {GPIO_Pin_13, GPIOC}, //LED,RUN
#else
        {GPIO_Pin_4, GPIOC},  //DO1
        {GPIO_Pin_7, GPIOA},  //DO2
        {GPIO_Pin_6, GPIOA},  //DO3
        {GPIO_Pin_5, GPIOA},  //DO4
        {GPIO_Pin_4, GPIOA},  //DO5
        {GPIO_Pin_2, GPIOA},  //DO6
        {GPIO_Pin_1, GPIOA},  //DO7
        {GPIO_Pin_0, GPIOA},  //DO8
        {GPIO_Pin_8, GPIOE},  //DO9
        {GPIO_Pin_10, GPIOE}, //DO10
        {GPIO_Pin_11, GPIOE}, //DO11
        {GPIO_Pin_12, GPIOE}, //DO12
        {GPIO_Pin_13, GPIOE}, //DO13
        {GPIO_Pin_14, GPIOE}, //DO14
        {GPIO_Pin_7, GPIOE},  //DO15
        {GPIO_Pin_1, GPIOB},  //DO16

        {GPIO_Pin_0, GPIOB},  //DO17
        {GPIO_Pin_5, GPIOC},  //DO18
        {GPIO_Pin_9, GPIOE},  //DO19-LOCKLED
        {GPIO_Pin_11, GPIOA}, //DO20-PWRCTRL
        {GPIO_Pin_13, GPIOC}, //LED,RUN
#endif

};

//local variable definition
static dio_dev_st dio_dev_inst;

static uint16_t drv_di_timer_init(void);
static void dio_reg_init(void);
static void di_reg_update(void);
//static void pwm_slow_timer_init(void);
static void drv_dio_bsp_init(void);

//digital input sampling thread
void di_thread_entry(void *parameter)
{
    rt_thread_delay(DI_THREAD_DELAY);
    drv_dio_bsp_init();
    dio_reg_init();
    drv_di_timer_init();
    //    pwm_slow_timer_init();
    rt_thread_delay(300);
    while (1)
    {
        di_reg_update();
        rt_thread_delay(500);
    }
}

/**
  * @brief  digital IOs GPIO initialization
  * @param  none
  * @retval none
  */
//数字输入输出初始化函数
static void drv_dio_bsp_init(void)
{
    extern local_reg_st l_sys;
    GPIO_InitTypeDef GPIO_InitStructure;
    uint16_t i;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    //数字输入PIN初始化
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    for (i = 0; i < Pin_Map_In; i++)
    {
        GPIO_InitStructure.GPIO_Pin = in_pin_map_inst[i].pin_id;
        GPIO_Init(in_pin_map_inst[i].pin_base, &GPIO_InitStructure);
    }

    //数字输出PIN初始化
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    for (i = 0; i < Pin_Map_Out; i++)
    {
        GPIO_InitStructure.GPIO_Pin = out_pin_map_inst[i].pin_id;
        GPIO_Init(out_pin_map_inst[i].pin_base, &GPIO_InitStructure);
    }
    //			Led_Gpio_Init();
    //复位
    for (i = 1; i < Pin_Map_Out; i++)
    {
        do_set(i, Bit_RESET);
    }

    //跳线选择初始化
    GPIO_InitStructure.GPIO_Pin = SLE1_PIN | SLE2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M
    GPIO_Init(SLE_GPIO, &GPIO_InitStructure);

    return;
}

/**
  * @brief  digital io stucture initialization
  * @param  none
  * @retval none
  */
//数字输入初始化函数�
static void dio_reg_init(void)
{
    uint16_t i;
    //		dio_dev_inst.din.bitmap[0] = 0;
    memset(dio_dev_inst.din.bitmap, 0, sizeof(dio_dev_inst.din.bitmap));
    for (i = 0; i < DI_BUF_DEPTH; i++)
    {
        dio_dev_inst.din.reg_array[0][i] = 0;
        dio_dev_inst.din.reg_array[1][i] = 0;
    }
    dio_dev_inst.dout.bitmap = 0;
}

/**
  * @brief  digital input result caculation
  * @param  none
  * @retval none
  */
//数字输入OOK解调
static void di_reg_update(void)
{
    uint16_t di_data[2], i, j, pusl_mask;
    uint16_t di_reg[DI_MAX_CNT];
    extern sys_reg_st g_sys;

    pusl_mask = g_sys.config.dev_mask.din_pusl;
    //		di_data = 0;
    memset(di_data, 0, sizeof(di_data));

    for (i = 0; i < DI_MAX_CNT; i++)
    {
        di_reg[i] = 0;
    }

    for (i = 0; i < DI_MAX_CNT; i++) //outer loop caculate each channels di data
    {
        for (j = 0; j < DI_BUF_DEPTH; j++) //inner loop caculate sum of one channel di data
        {
            if (i < DIN_WORD2)
            {
                di_reg[i] += (dio_dev_inst.din.reg_array[0][j] >> i) & (0x0001);
            }
            else
            {
                di_reg[i] += (dio_dev_inst.din.reg_array[1][j] >> (i - DIN_WORD2)) & (0x0001);
            }
        }
    }
    for (i = 0; i < DI_MAX_CNT; i++)
    {
        if (i < DIN_WORD2)
        {
            if (pusl_mask & (0x01 << i)) //脉冲采集
            {
                //								if(di_reg[i] > (DI_BUF_DEPTH - 15))
                //								{
                //										di_data[0] &= ~(0x0001<<i);
                //								}
                //								else
                //								{
                //										di_data[0] |= (0x0001<<i);
                //								}
                //20170223,台达风机修改
                //								if((di_reg[i] > (DI_BUF_DEPTH - 15))||(di_reg[i] < g_sys.config.alarm[ACL_FAN_OVERLOAD2].alarm_param))
                //								{
                //										di_data[0] &= ~(0x0001<<i);
                //								}
                //								else
                //								{
                //										di_data[0] |= (0x0001<<i);
                //								}
            }
            else
            {
                if (di_reg[i] < (DI_BUF_DEPTH - (DI_BUF_DEPTH >> 2))) //[0~75%] duty cycle is consider set state, otherwise is considered reset state
                {
                    di_data[0] |= (0x0001 << i);
                }
                else
                {
                    di_data[0] &= ~(0x0001 << i);
                }
            }
        }
        else
        {
            if (di_reg[i] < (DI_BUF_DEPTH - (DI_BUF_DEPTH >> 2))) //[0~75%] duty cycle is consider set state, otherwise is considered reset state
            {
                di_data[1] |= (0x0001 << (i - DIN_WORD2));
            }
            else
            {
                di_data[1] &= ~(0x0001 << (i - DIN_WORD2));
            }
        }
    }
    //		dio_dev_inst.din.bitmap[0] = di_data[0];
    //		dio_dev_inst.din.bitmap[1] = di_data[1];
    memcpy(dio_dev_inst.din.bitmap, di_data, sizeof(di_data));
    //		rt_kprintf("dio_dev_inst.din.bitmap[0] = %d\n",dio_dev_inst.din.bitmap[0]);
    //		rt_kprintf("dio_dev_inst.din.bitmap[1] = %d\n",dio_dev_inst.din.bitmap[1]);
}

/**
  * @brief  raw digital input data read
  * @param  none
  * @retval 18 channels data, each bit stands for one channel
  */
//数字输入函数，对所有数字输入状态进行更新
static uint32_t di_read(void)
{
    uint32_t read_bitmap;
    uint16_t i;
    read_bitmap = 0;
    for (i = 0; i <= DI_MAX_CNT - 1; i++)
    {
        read_bitmap |= GPIO_ReadInputDataBit(in_pin_map_inst[DI_MAX_CNT - 1 - i].pin_base, in_pin_map_inst[DI_MAX_CNT - 1 - i].pin_id);
        if (i < DI_MAX_CNT - 1)
        {
            read_bitmap = read_bitmap << 1;
        }
    }
    return read_bitmap;
}

//数字输出控制函数；
static uint16_t do_set(int16_t pin_id, BitAction value)
{
    if ((pin_id <= Pin_Map_Out) && (pin_id > 0))
    {
        GPIO_WriteBit(out_pin_map_inst[pin_id - 1].pin_base, out_pin_map_inst[pin_id - 1].pin_id, value);
        return 1;
    }
    else
    {
        return 0;
    }
}

//置位所有数字输出
static void do_set_all(void)
{
    uint16_t i;
    for (i = 1; i <= Pin_Map_Out; i++)
    {
        do_set(i, Bit_SET);
    }
}

//复位所有数字输出
static void do_reset_all(void)
{
    uint16_t i;
    for (i = 1; i <= Pin_Map_Out; i++)
    {
        do_set(i, Bit_RESET);
    }
}

/**
  * @brief  digital input sample interval timeout callback function, calls di_read() each time to update di buffer queue
  * @param  none
  * @retval none
  */
//数字输入定时器回调函数，对数字输入电平进行采样后放入缓冲队列；
static void stimer_di_timeout(void *parameter)
{

    static uint16_t count;
    uint32_t pBuf;

    if (count >= DI_BUF_DEPTH)
    {
        count = count % DI_BUF_DEPTH;
    }
    pBuf = di_read();
    dio_dev_inst.din.reg_array[0][count] = pBuf;
    dio_dev_inst.din.reg_array[1][count] = pBuf >> 16;
    count++;
}

/**
  * @brief  digital input sample interval timer initialization, expires in 6 miliseconds pieriod
  * @param  none
  * @retval none
  */
//数字输入定时器，每10ms周期对数字输入进行采样
static uint16_t drv_di_timer_init(void)
{
    rt_timer_t stimer_dio;
    stimer_dio = rt_timer_create("stimer_di",
                                 stimer_di_timeout,
                                 RT_NULL,
                                 SAMPLE_INTERVAL,
                                 RT_TIMER_FLAG_PERIODIC);
    rt_timer_start(stimer_dio);
    return 1;
}
/**
  * @brief  digital IO initialization function
  * @param  none
  * @retval none
 */
void drv_dio_init(void)
{
    drv_dio_bsp_init();
    drv_di_timer_init();
}

/**
  * @brief  update global variable g_din_inst and g_ain_inst according to di and ai inputs
  * @param  none
  * @retval none
**/
void di_sts_update(sys_reg_st *gds_sys_ptr)
{
    //			uint16_t din_mask_bitmap;
    //		uint16_t din_bitmap_polarity;
    //
    //		din_mask_bitmap = (gds_sys_ptr->config.dev_mask.din[0]|(DIN_MASK_MASK))&(DIN_MASK_MASK1);
    //				rt_kprintf("din_mask_bitmap = %X\n",din_mask_bitmap);
    //	  din_bitmap_polarity = gds_sys_ptr->config.dev_mask.din_bitmap_polarity[0]&(DIN_POLARITY_MASK);
    //					rt_kprintf("din_bitmap_polarity = %X\n",din_bitmap_polarity);
    //	  //mask报警掩码
    //		dio_dev_inst.din.bitmap[0] = (~(dio_dev_inst.din.bitmap[0]^din_bitmap_polarity));
    //				rt_kprintf("dio_dev_inst.din.bitmap[0]1 = %X\n",dio_dev_inst.din.bitmap[0]);
    //		// 数字输入掩码
    //		gds_sys_ptr->status.din_bitmap[0] = din_mask_bitmap & dio_dev_inst.din.bitmap[0];
    //					rt_kprintf("gds_sys_ptr->status.din_bitmap[0] = %X\n",gds_sys_ptr->status.din_bitmap[0]);

    uint16_t din_mask_bitmap[2];
    uint16_t din_bitmap_polarity[2];

    memcpy(din_mask_bitmap, gds_sys_ptr->config.dev_mask.din, sizeof(din_mask_bitmap));
    memcpy(din_bitmap_polarity, gds_sys_ptr->config.dev_mask.din_bitmap_polarity, sizeof(din_bitmap_polarity));

    din_bitmap_polarity[0] = gds_sys_ptr->config.dev_mask.din_bitmap_polarity[0];
    //		din_bitmap_polarity[0]=0x5B;
    //mask报警掩码
    dio_dev_inst.din.bitmap[0] = (~(dio_dev_inst.din.bitmap[0] ^ din_bitmap_polarity[0]));
    dio_dev_inst.din.bitmap[1] = (~(dio_dev_inst.din.bitmap[1] ^ din_bitmap_polarity[1]));
    // 数字输入掩码
    gds_sys_ptr->status.din_bitmap[0] = din_mask_bitmap[0] & dio_dev_inst.din.bitmap[0];
    gds_sys_ptr->status.din_bitmap[1] &= 0xFF;
    gds_sys_ptr->status.din_bitmap[1] |= ((din_mask_bitmap[1] & dio_dev_inst.din.bitmap[1]) << 8); //放到高16位

    //		rt_kprintf("bitmap[0] = %X,din_bitmap[0] = %X\n",dio_dev_inst.din.bitmap[0],gds_sys_ptr->status.din_bitmap[0]);

    // 数字输入掩码
    gds_sys_ptr->status.ComSta.u16Din_bitmap[0] = din_mask_bitmap[0] & dio_dev_inst.din.bitmap[0];
    //		rt_kprintf("bitmap[0] = %X,din_bitmap[0] = %X\n",dio_dev_inst.din.bitmap[0],gds_sys_ptr->status.ComSta.u16Din_bitmap[0]);
}

void dio_set_do(uint16_t channel_id, BitAction data)
{
    do_set(channel_id, data);
}

//LED闪烁
void led_toggle(void)
{
    extern sys_reg_st g_sys;
    static uint8_t led_flag = 0;

    if (led_flag == 0)
    {
        do_set(Pin_Map_Out, Bit_RESET);
        led_flag = 1;
    }
    else
    {
        do_set(Pin_Map_Out, Bit_SET);
        led_flag = 0;
    }
}

//获取跳线值
uint8_t GetSEL(void)
{
    extern local_reg_st l_sys;
    uint8_t u8Ret;

    u8Ret = 0x00;

    if (!SLE2_READ)
    {
        u8Ret |= 0x02;
    }
    else
    {
        u8Ret &= ~0x02;
    }

    if (!SLE1_READ)
    {
        u8Ret |= 0x01;
    }
    else
    {
        u8Ret &= ~0x01;
    }
    l_sys.SEL_Jump = u8Ret;
    return u8Ret;
}

//FINSH_FUNCTION_EXPORT(slow_pwm_set, slow pwm set.);
FINSH_FUNCTION_EXPORT(do_set, set data out bit);
FINSH_FUNCTION_EXPORT(do_set_all, set all data bit 1);
FINSH_FUNCTION_EXPORT(do_reset_all, set all data out bit 0);
#endif //__DIO
