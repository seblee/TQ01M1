#include "zph01_opt.h"
#include <rtthread.h>
#include "sys_conf.h"
static unsigned char rx_buffer[10];
extern sys_reg_st g_sys;
/**********************************************************************
* 函数名: ucharFucCheckSum(uchar *i,ucharln)
* 功能描述:求和校验（取发送、接收协议的1\2\3\4\5\6\7的和取反+1）
* 函数说明:将数组的元素1-倒数第二个元素相加后取反+1（元素个数必须大于2）
**********************************************************************/
unsigned char FucCheckSum(unsigned char *i, unsigned char ln)
{
    unsigned char j, tempq = 0;
    i += 1;
    for (j = 0; j < (ln - 2); j++)
    {
        tempq += *i;
        i++;
    }
    tempq = (~tempq) + 1;
    return (tempq);
}

/**********************************************************************
* 函数名: void zph01ReceiveData(unsigned char data)
* 功能描述:接收数据
* 函数说明:校验处理数据
**********************************************************************/
void zph01ReceiveData(unsigned char data)
{
    static unsigned char count = 0;
    if ((count == 0) && (data == 0xff))
    {
        rx_buffer[count] = data;
        count++;
    }
    else if ((count == 1) && (data == 0x18))
    {
        rx_buffer[count] = data;
        count++;
    }
    else if ((count > 1) && (count < 9))
    {
        rx_buffer[count] = data;
        count++;
    }
    else
    {
        count = 0;
        rt_memset(rx_buffer, 0, 10);
    }

    if (count == 9)
    {
        if (rx_buffer[8] == FucCheckSum(rx_buffer, 9))
        {
            g_sys.status.ComSta.u16PM25 = rx_buffer[3] * 100 + rx_buffer[4];
        }

        count = 0;
        rt_memset(rx_buffer, 0, 10);
    }
}
