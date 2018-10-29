#include <rtthread.h>
#include "Delay.h"
#include "TH_SENSOR_BSP.h"
#include "string.h"
#include "user_mb_app.h"
#include "stdlib.h"
#include "math.h"

void AM_BUS_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    //Configure BUS pins: SDA_00
    GPIO_InitStructure.GPIO_Pin = II_AM_SDA_00_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(II_AM_SDA_00_GPIO, &GPIO_InitStructure);

    GPIO_SetBits(II_AM_SDA_00_GPIO, II_AM_SDA_00_Pin);

    GPIO_InitStructure.GPIO_Pin = II_AM_SDA_01_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(II_AM_SDA_01_GPIO, &GPIO_InitStructure);

    GPIO_SetBits(II_AM_SDA_01_GPIO, II_AM_SDA_01_Pin);
}

void AM_Init(void)
{
    AM_BUS_Config();
}

static void AM_SDA_H(uint8_t u8SN)
{
    switch (u8SN)
    {
    case 0x00:
    {
        GPIO_SetBits(II_AM_SDA_00_GPIO, II_AM_SDA_00_Pin);
    }
    break;
    case 0x01:
    {
        GPIO_SetBits(II_AM_SDA_01_GPIO, II_AM_SDA_01_Pin);
    }
    break;
    default:
        break;
    }
}

static void AM_SDA_L(uint8_t u8SN)
{
    switch (u8SN)
    {
    case 0x00:
    {
        GPIO_ResetBits(II_AM_SDA_00_GPIO, II_AM_SDA_00_Pin);
    }
    break;
    case 0x01:
    {
        GPIO_ResetBits(II_AM_SDA_01_GPIO, II_AM_SDA_01_Pin);
    }
    break;
    default:
        break;
    }
}

static uint8_t AM_SDA_READ(uint8_t u8SN)
{
    uint8_t u8Read_SDA = 0;
    switch (u8SN)
    {
    case 0x00:
    {
        u8Read_SDA = GPIO_ReadInputDataBit(II_AM_SDA_00_GPIO, II_AM_SDA_00_Pin);
    }
    break;
    case 0x01:
    {
        u8Read_SDA = GPIO_ReadInputDataBit(II_AM_SDA_01_GPIO, II_AM_SDA_01_Pin);
    }
    break;
    default:
        break;
    }
    return u8Read_SDA;
}
static void AM_SDA_OUT(uint8_t u8SN)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    switch (u8SN)
    {
    case 0x00:
    {
        GPIO_InitStructure.GPIO_Pin = II_AM_SDA_00_Pin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(II_AM_SDA_00_GPIO, &GPIO_InitStructure);
    }
    break;
    case 0x01:
    {
        GPIO_InitStructure.GPIO_Pin = II_AM_SDA_01_Pin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(II_AM_SDA_01_GPIO, &GPIO_InitStructure);
    }
    break;
    default:
        break;
    }
}

static void AM_SDA_IN(uint8_t u8SN)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    switch (u8SN)
    {
    case 0x00:
    {
        GPIO_InitStructure.GPIO_Pin = II_AM_SDA_00_Pin;
        //				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(II_AM_SDA_00_GPIO, &GPIO_InitStructure);
    }
    break;
    case 0x01:
    {
        GPIO_InitStructure.GPIO_Pin = II_AM_SDA_01_Pin;
        //				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(II_AM_SDA_01_GPIO, &GPIO_InitStructure);
    }
    break;
    default:
        break;
    }
}

/********************************************\
|* 功能： 起始信号       *|
\********************************************/
void AM23XX_start(uint8_t u8SN)
{
    //主机发送起始信号
    AM_SDA_OUT(u8SN); //设为输出模式
    AM_SDA_L(u8SN);   //主机把数据总线（SDA）拉低
    Delay_us(1500);   //延时1.5Ms//拉低一段时间（至少800us），通知传感器准备数据
    AM_SDA_H(u8SN);   //释放总线
    AM_SDA_IN(u8SN);  //设为输入模式，判断传感器响应信号
    Delay_us(40);     //延时30us
}

/********************************************\
|* 功能： 读传感器发送的单个字节	        *|
\********************************************/
unsigned char Read_SensorData(uint8_t u8SN)
{
    uint8_t i;
    uint16_t j;
    uint8_t data = 0, bit = 0;

    for (i = 0; i < 8; i++)
    {
        while (!AM_SDA_READ(u8SN)) //检测上次低电平是否结束
        {
            if (++j >= 5000) //防止进入死循环
            {
                break;
            }
        }
        //延时Min=26us Max70us 跳过数据"0" 的高电平
        Delay_us(30); //延时30us

        //判断传感器发送数据位
        bit = 0;
        if (AM_SDA_READ(u8SN))
        {
            bit = 1;
        }
        j = 0;
        while (AM_SDA_READ(u8SN)) //等待高电平 结束
        {
            if (++j >= 5000) //防止进入死循环
            {
                break;
            }
        }
        data <<= 1;
        data |= bit;
    }
    return data;
}

/********************************************\
|* 功能：AM2320读取温湿度函数       *|
\********************************************/
//变量：Humi_H：湿度高位；Humi_L：湿度低位；Temp_H：温度高位；Temp_L：温度低位；Temp_CAL：校验位
//数据格式为：湿度高位（湿度整数）+湿度低位（湿度小数）+温度高位（温度整数）+温度低位（温度小数）+ 校验位
//校验：校验位=湿度高位+湿度低位+温度高位+温度低位
uint8_t Read_Sensor(uint16_t *u16TH_Buff, uint8_t u8SN)
{
    uint16_t j;
    uint8_t Humi_H, Humi_L, Temp_H, Temp_L, Temp_CAL, temp;
    //	float Temprature,Humi;//定义温湿度变量 ，此变量为全局变量
    uint8_t Sensor_AnswerFlag; //收到起始标志位
    uint8_t Sensor_ErrorFlag;  //读取传感器错误标志
    int16_t i16Temprature;     //定义温湿度变量
    uint16_t u16Humi;          //定义温湿度变量

    Sensor_ErrorFlag = Sensor_ErrorFlag;
    ENTER_CRITICAL_SECTION(); //关全局中断
    AM23XX_start(u8SN);       //从机发送起始信号

    Sensor_AnswerFlag = 0; //传感器响应标志
    //判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行
    if (AM_SDA_READ(u8SN) == 0)
    {
        Sensor_AnswerFlag = 1; //收到起始信号
        j = 0;
        while ((!AM_SDA_READ(u8SN))) //判断从机发出 80us 的低电平响应信号是否结束
        {
            if (++j >= 500) //防止进入死循环
            {
                Sensor_ErrorFlag = 1;
                break;
            }
        }
        Sensor_AnswerFlag |= 0x02;
        j = 0;
        while (AM_SDA_READ(u8SN)) //判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
        {
            if (++j >= 800) //防止进入死循环
            {
                Sensor_ErrorFlag = 1;
                break;
            }
        }
        Sensor_AnswerFlag |= 0x04;
        //接收数据
        Humi_H = Read_SensorData(u8SN);
        Humi_L = Read_SensorData(u8SN);
        Temp_H = Read_SensorData(u8SN);
        Temp_L = Read_SensorData(u8SN);
        Temp_CAL = Read_SensorData(u8SN);

        temp = (uint8_t)(Humi_H + Humi_L + Temp_H + Temp_L); //只取低8位
                                                             //			rt_kprintf("Humi_H=%d,Humi_L=%d,Temp_H=%d,Temp_L=%d,Temp_CAL=%d,temp=%d,\n",Humi_H,Humi_L,Temp_H,Temp_L,Temp_CAL,temp);
        if (Temp_CAL == temp)                                //如果校验成功，往下运行
        {
            Sensor_AnswerFlag |= 0x08;

            u16Humi = Humi_L | ((uint16_t)Humi_H << 8); //湿度

            if (Temp_H & 0X80) //为负温度
            {
                i16Temprature = 0 - ((Temp_L & 0x7F) | ((uint16_t)Temp_H << 8));
            }
            else //为正温度
            {
                i16Temprature = Temp_L | ((uint16_t)Temp_H << 8); //温度
            }
            //判断数据是否超过量程（温度：-40℃~80℃，湿度0％RH~99％RH）
            if (u16Humi > 999)
            {
                u16Humi = 999;
            }

            if (i16Temprature > 800)
            {
                i16Temprature = 800;
            }
            if (i16Temprature < -400)
            {
                i16Temprature = -400;
            }
            u16TH_Buff[0] = (uint16_t)i16Temprature; //温度
            u16TH_Buff[1] = (uint16_t)u16Humi;       //湿度
                                                     //			rt_kprintf("\r\nTemprature:  %.1f  ℃\r\n",i16Temprature); //显示温度
                                                     //			rt_kprintf("Humi:  %.1f  %%RH\r\n",u16Humi);//显示湿度
        }
        else
        {
            Sensor_AnswerFlag |= 0x10;
            //			rt_kprintf("Humi_H=%d,Humi_L=%d,Temp_H=%d,Temp_L=%d,Temp_CAL=%d,temp=%d,\n",Humi_H,Humi_L,Temp_H,Temp_L,Temp_CAL,temp);
        }
        //		rt_kprintf("Sensor_AnswerFlag =%x\r\n",Sensor_AnswerFlag);//
    }
    else
    {
        Sensor_ErrorFlag = 0; //未收到传感器响应
        rt_kprintf("Sensor Error!!\r\n");
    }
    EXIT_CRITICAL_SECTION(); //开全局中断

    return Sensor_AnswerFlag;
}

#define AM_SENSOR_NUM 2
uint16_t u16TH_Sensor[AM_SENSOR_NUM] = {0};
#define TH_AVE_NUM 5
#define JUMP_OFFSET 30
/********************************************\
|* 功能： 温湿度更新             	        *|
\********************************************/
uint8_t AM_Sensor_update(sys_reg_st *gds_ptr)
{
    extern sys_reg_st g_sys;

    static uint8_t u8CNT = 0;
    static uint8_t u8TH_CNT[AM_SENSOR_NUM] = {0};
    static uint8_t u8Err_CNT[AM_SENSOR_NUM] = {0};
    static uint16_t u16TH_Ave[AM_SENSOR_NUM][2][TH_AVE_NUM] = {0};
    static uint8_t u8Offset_CNT = 0;
    static uint16_t u16LastTH[2] = {0};

    uint8_t i = 0, j = 0, k = 0;            //收到起始标志位
    uint8_t u8SenFlag[AM_SENSOR_NUM] = {0}; //收到起始标志位
    Com_tnh_st u16TH_Buff = {0};
    uint16_t u16TH_Sum[AM_SENSOR_NUM][2] = {0};
    uint8_t u8TH_Cnt[AM_SENSOR_NUM][2] = {0};
    int16_t i16Offset_Buff[2] = {0};

    u8CNT++;
    if (u8CNT >= 0xFF)
    {
        u8CNT = 0x00;
    }
    //		i=u8CNT%AM_SENSOR_NUM;
    i = u8CNT % 6;
    //		//两次读取间隔至少2S
    //		Delay_ms(1500);//延时1500ms
    if (i != 0)
    {
        return 0;
    }
    //温湿度跳变判断延时
    u8Offset_CNT++;
    if (u8Offset_CNT >= JUMP_OFFSET)
    {
        u8Offset_CNT = JUMP_OFFSET;
    }

    memset(&u16TH_Sensor[0], 0x00, 4);

    //		//映射回风温湿度
    //		if((i==MBM_DEV_A1_ADDR)&&((gds_ptr->config.dev_mask.mb_comp&(0X01<<MBM_DEV_A1_ADDR))==0))
    //		{
    //				return u8SenFlag[i];
    //		}
    //		else if((i==MBM_DEV_A2_ADDR)&&((gds_ptr->config.dev_mask.mb_comp&(0X01<<MBM_DEV_A2_ADDR))==0))
    //		{
    //				return u8SenFlag[i];
    //		}
    u8SenFlag[i] = Read_Sensor(&u16TH_Sensor[0], i);
    if (u8SenFlag[i])
    {
        if ((u16TH_Sensor[0] == 0) && (u16TH_Sensor[0] == 0))
        {
            u8Err_CNT[i]++;
        }
        else
        {
            u8Err_CNT[i] = 0;
            u16TH_Buff.Temp = u16TH_Sensor[0];
            u16TH_Buff.Hum = u16TH_Sensor[1];
            //更新状态MBM_COM_STS_REG_NO
            gds_ptr->status.status_remap[MBM_COM_STS_REG_NO] |= (0x0001 << 0);
        }
    }
    else
    {
        u8Err_CNT[i]++;
    }

    if (u8Err_CNT[i] > ERROR_CNT_MAX)
    {
        //			g_sys.status.ComSta.u16TH[i].Temp = 0;
        //			g_sys.status.ComSta.u16TH[i].Hum = 0;
        //			//更新状态MBM_COM_STS_REG_NO
        g_sys.status.ComSta.u16TH[0].Temp = 285;
        g_sys.status.ComSta.u16TH[0].Hum = 567;
        gds_ptr->status.status_remap[MBM_COM_STS_REG_NO] &= ~(0x0001 << 0);
        AM_Init(); //AM Sensor init
        i = 0;
    }
    else if (u8Err_CNT[i] == 0)
    //		else
    {
        if ((u16TH_Buff.Temp <= 999) && ((u16TH_Buff.Temp != 0) && (u16TH_Buff.Hum != 0)))
        {
            if (u16TH_Buff.Hum <= 999)
            {
                i16Offset_Buff[0] = (int16_t)u16TH_Buff.Temp - (int16_t)u16LastTH[0];
                i16Offset_Buff[1] = (int16_t)u16TH_Buff.Hum - (int16_t)u16LastTH[1];
                if (((abs(i16Offset_Buff[0]) > TEMP_OFFSET) || (abs(i16Offset_Buff[1]) > HUM_OFFSET)) && (u8Offset_CNT >= JUMP_OFFSET))
                {
                }
                else
                {
                    u8TH_CNT[i]++;
                    if (u8TH_CNT[i] >= 0xFF)
                    {
                        u8TH_CNT[i] = 0x00;
                    }
                    j = u8TH_CNT[i] % TH_AVE_NUM;

                    u16LastTH[0] = u16TH_Buff.Temp;
                    u16LastTH[1] = u16TH_Buff.Hum;
                    u16TH_Ave[i][0][j] = u16TH_Buff.Temp;
                    u16TH_Ave[i][1][j] = u16TH_Buff.Hum;
                }
            }
        }

        //		rt_kprintf("Temp=%x,1= %x,1= %x,3 = %x,4=%x\n",u16TH_Ave[0][0][0],u16TH_Ave[0][0][1],u16TH_Ave[0][0][2],u16TH_Ave[0][0][3],u16TH_Ave[0][0][4]);

        for (k = 0; k < TH_AVE_NUM; k++)
        {
            if (u16TH_Ave[i][0][k] != 0)
            {
                u16TH_Sum[i][0] += u16TH_Ave[i][0][k];
                u8TH_Cnt[i][0]++;
            }
            if (u16TH_Ave[i][1][k] != 0)
            {
                u16TH_Sum[i][1] += u16TH_Ave[i][1][k];
                u8TH_Cnt[i][1]++;
            }
        }
        g_sys.status.ComSta.u16TH[i].Temp = u16TH_Sum[i][0] / u8TH_Cnt[i][0] + (int16_t)g_sys.config.general.temp_sensor_cali[0].temp;
        g_sys.status.ComSta.u16TH[i].Hum = u16TH_Sum[i][1] / u8TH_Cnt[i][1] + (int16_t)g_sys.config.general.temp_sensor_cali[0].hum;
    }
    //		g_sys.status.ComSta.u16TH[0].Temp=285;
    //		g_sys.status.ComSta.u16TH[0].Temp=567;
    rt_kprintf("u8CNT=%x,i=%x,u8SenFlag[0]= %x,u16TH_Sensor[0]= %x,[1] = %x,u8Err_CNT[0]=%x,Temp=%x,Hum=%x\n", u8CNT, i, u8SenFlag[0], u16TH_Sensor[0], u16TH_Sensor[1], u8Err_CNT[0], g_sys.status.ComSta.u16TH[0].Temp, g_sys.status.ComSta.u16TH[0].Hum);

    return u8SenFlag[i];
}
