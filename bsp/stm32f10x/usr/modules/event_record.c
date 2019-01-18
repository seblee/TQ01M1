#include "event_record.h"
#include "I2c_bsp.h"
#include "rtc_bsp.h"
#include "fifo.h"
#include <string.h>
#include "daq.h"
#include "sys_status.h"
#include "password.h"

#define INVALID_HUM_TEMP 0xf001;
// alarm status chian
static alram_node head_node;

//chain
static struct rt_mempool alarm_status_mp_inst;

static uint8_t alarm_mempool[(ACL_TOTAL_NUM) * (sizeof(alram_node) + 4)];

//operationall event record
static fifo8_cb_td eventlog_fifo_inst;
static cyc_buff_st event_cyc_buff_inst;

//Alarm record

static fifo8_cb_td alarmlog_fifo;
static alarm_table_st alarm_status_table[ACL_TOTAL_NUM];
static cyc_buff_st alarm_cyc_buff_inst;

//tem_hum
typedef struct
{
    uint8_t base;
    uint8_t c1;
    uint8_t c2;
} cnt_st;

//计算地址。
//static void cacl_event_pt(record_pt_st *pt_record,uint16_t max)
//{
//
//	  pt_record->cnt++;
//	 if(pt_record->cnt > max)
//	 {
//	 	pt_record->cnt = max;//
//	 	pt_record->startpiont++;
//		 if(pt_record->startpiont > max)
//		 {
//				pt_record->startpiont = 0;
//		 }
//	 }

//	  pt_record->endpiont++;
//	 if( pt_record->endpiont > max)
//	 {
//	 	 pt_record->endpiont = 0;
//
//	 }

//
//}

//event record

/*********************************************************
	Init Event_Record FiFo

	
***********************************************************/

static void init_eventlog_fifo(void)
{
    uint16_t block_size;
    block_size = sizeof(event_log_st);

    fifo8_init(&eventlog_fifo_inst, block_size, EVENT_FIFO_DEPTH);
}

void init_evnet_log(void)
{
    extern sys_reg_st g_sys;
    init_eventlog_fifo();
    init_cycbuff(&event_cyc_buff_inst, sizeof(event_log_st), EVE_MAX_CNT, EVENT_REC_PT_ADDR);
    add_eventlog_fifo(0xffff, (USER_CPAD << 8) | g_sys.status.general.permission_level, 0, 1);
}

void user_eventlog_add(void)
{
    // event fifo

    uint8_t len, index, i;
    uint8_t data_buff[ALARM_RECORD_LEN];
    event_log_st eventlog_inst;

    len = get_fifo8_length(&eventlog_fifo_inst);
    for (index = 0; index < len; index++)
    {
        i = 0;
        if (fifo8_pop(&eventlog_fifo_inst, (uint8_t *)&eventlog_inst) == 1)
        {
            data_buff[i++] = eventlog_inst.event_id >> 8;
            data_buff[i++] = eventlog_inst.event_id;
            data_buff[i++] = eventlog_inst.time >> 24;
            data_buff[i++] = eventlog_inst.time >> 16;
            data_buff[i++] = eventlog_inst.time >> 8;
            data_buff[i++] = eventlog_inst.time;
            data_buff[i++] = eventlog_inst.user_id >> 8;
            data_buff[i++] = eventlog_inst.user_id;
            data_buff[i++] = eventlog_inst.former_data >> 8;
            data_buff[i++] = eventlog_inst.former_data;
            data_buff[i++] = eventlog_inst.new_data >> 8;
            data_buff[i++] = eventlog_inst.new_data;

            add_cycbuff(&event_cyc_buff_inst, (uint8_t *)&data_buff);
        }
    }
}

/*********************************************************
	add Event_Record FiFo

	
***********************************************************/

void add_eventlog_fifo(uint16 event_id, uint16 user_id, uint16 former_data, uint16 new_data)
{
    event_log_st eventlog_inst;
    //获取系统时间
    time_t now;

    get_local_time(&now);
    eventlog_inst.time = now;
    eventlog_inst.event_id = event_id;
    eventlog_inst.user_id = user_id;
    eventlog_inst.former_data = former_data;
    eventlog_inst.new_data = new_data;
    //压入FIFO;
    fifo8_push(&eventlog_fifo_inst, (uint8_t *)&eventlog_inst);
}

/***************************************************************
Init_ALarmlog_FIFO

*****************************************************************/

static void init_aLarmlog_fifo(void)
{
    uint16_t block_size, i;
    uint32_t time;
    uint8_t data_buff[sizeof(alarm_log_st)], index;

    block_size = sizeof(alarm_log_st);
    fifo8_init(&alarmlog_fifo, block_size, ALARM_FIFO_DEPTH);

    //初始化cycbuffer
    init_cycbuff(&alarm_cyc_buff_inst, sizeof(alarm_log_st), ALARM_MAX_CNT, ALARM_REC_PT_ADDR);

    //获取EEPROM中的数据表。
    I2C_EE_BufRead((uint8_t *)&alarm_status_table[0], AlARM_TABLE_ADDR, sizeof(alarm_table_st) * ACL_TOTAL_NUM);

    //获取系统时间
    get_local_time(&time);

    //遍历Alarm_Table
    for (i = 0; i < ACL_TOTAL_NUM; i++)
    {
        if ((alarm_status_table[i].trigger_time != 0xFFFFFFFF))
        {
            index = 0;
            data_buff[index++] = alarm_status_table[i].alarm_id >> 8;
            data_buff[index++] = alarm_status_table[i].alarm_id;
            data_buff[index++] = alarm_status_table[i].trigger_time >> 24;
            data_buff[index++] = alarm_status_table[i].trigger_time >> 16;
            data_buff[index++] = alarm_status_table[i].trigger_time >> 8;
            data_buff[index++] = alarm_status_table[i].trigger_time;
            data_buff[index++] = time >> 24;
            data_buff[index++] = time >> 16;
            data_buff[index++] = time >> 8;
            data_buff[index++] = time;
            data_buff[index++] = alarm_status_table[i].alarm_value >> 8;
            data_buff[index++] = alarm_status_table[i].alarm_value;
            //保存到 EEPROM中
            add_cycbuff(&alarm_cyc_buff_inst, (uint8_t *)&data_buff[0]);
        }
        alarm_status_table[i].trigger_time = 0xFFFFFFFF;
        alarm_status_table[i].alarm_value = 0xFFFF;
    }
    I2C_EE_BufWrite((uint8_t *)&alarm_status_table[0], AlARM_TABLE_ADDR, sizeof(alarm_table_st) * ACL_TOTAL_NUM);
}

void init_alarm_log(void)
{

    init_aLarmlog_fifo();
}

void user_alarmlog_add(void)
{
    uint8_t index, len, i, table_index;
    uint8_t data_buff[sizeof(alarm_log_st)];
    alarm_log_st alarmlog_inst;

    len = get_fifo8_length(&alarmlog_fifo);

    for (index = 0; index < len; index++)
    {

        if (fifo8_pop(&alarmlog_fifo, (uint8_t *)&alarmlog_inst) == 1)
        {
            //第字节代表报警序号
            table_index = alarmlog_inst.alarm_id & 0x00FF;

            if (alarmlog_inst.end_time == 0xFFFFFFFF) //报警触发
            {
                //触发存放数据

                alarm_status_table[table_index].trigger_time = alarmlog_inst.trigger_time;
                alarm_status_table[table_index].alarm_value = alarmlog_inst.rev;
                alarm_status_table[table_index].alarm_id = alarmlog_inst.alarm_id;
                //save to EEPROM
                I2C_EE_BufWrite((uint8_t *)&alarm_status_table[table_index], AlARM_TABLE_ADDR + sizeof(alarm_table_st) * table_index, sizeof(alarm_table_st));
                //
            }
            else //报警结束
            {
                if (alarm_status_table[table_index].trigger_time != 0xFFFFFFFF)
                {
                    i = 0;

                    data_buff[i++] = alarm_status_table[table_index].alarm_id >> 8;
                    data_buff[i++] = alarm_status_table[table_index].alarm_id;

                    data_buff[i++] = alarm_status_table[table_index].trigger_time >> 24;
                    data_buff[i++] = alarm_status_table[table_index].trigger_time >> 16;
                    data_buff[i++] = alarm_status_table[table_index].trigger_time >> 8;
                    data_buff[i++] = alarm_status_table[table_index].trigger_time;

                    data_buff[i++] = alarmlog_inst.end_time >> 24;
                    data_buff[i++] = alarmlog_inst.end_time >> 16;
                    data_buff[i++] = alarmlog_inst.end_time >> 8;
                    data_buff[i++] = alarmlog_inst.end_time;

                    data_buff[i++] = alarm_status_table[table_index].alarm_value >> 8;
                    data_buff[i++] = alarm_status_table[table_index].alarm_value;
                    //保存到 EEPROM中
                    add_cycbuff(&alarm_cyc_buff_inst, (uint8_t *)&data_buff[0]);
                }
                else
                {
                    rt_kprintf("user_alarmlog_add ERRO= %d \n", table_index);
                }
                //save table
                alarm_status_table[table_index].alarm_value = 0xFFFF;
                alarm_status_table[table_index].trigger_time = 0xFFFFFFFF;
                I2C_EE_BufWrite((uint8_t *)&alarm_status_table[table_index], AlARM_TABLE_ADDR + sizeof(alarm_table_st) * table_index, sizeof(alarm_table_st));
            }
        }
    }
}

/*********************************************************
	add Event_Record FiFo

	FLG =0 ;ALARM Trigger;

	FLG =1 ALARM_END;
	
***********************************************************/

void add_alarmlog_fifo(uint16_t alarm_id, alarm_enum flg, uint16 alarm_value)
{
    // fifo
    alarm_log_st alarmLog_inst;
    //获取系统时间
    time_t now;
    get_local_time(&now);
    if (flg == ALARM_END)
    {
        alarmLog_inst.end_time = now;
        alarmLog_inst.trigger_time = 0;
    }
    else
    {
        alarmLog_inst.trigger_time = now;
        alarmLog_inst.end_time = 0xFFFFFFFF;
    }

    alarmLog_inst.alarm_id = alarm_id;
    alarmLog_inst.rev = alarm_value;

    //压入FIFO;
    if (fifo8_push(&alarmlog_fifo, (uint8_t *)&alarmLog_inst) == 0)
    {
        rt_kprintf("\nfifo16_push ERRO\n");
    }
}

//清除一切事件记录
//Flag=0//清除事件记录
//flag=1 清除报警记录
uint8_t clear_log(uint8_t flag)
{
    uint8_t ret;

    ret = 0;
    //事件记录归零

    if (flag == EVENT_TYPE)
    {
        if (clear_cycbuff_log(&event_cyc_buff_inst) == EEPROM_NOERRO)
        {
            return (1);
        }
        else
        {
            return (0);
        }
    }
    else
    {
        ret += clear_cycbuff_log(&alarm_cyc_buff_inst);
        memset((uint8_t *)&alarm_status_table[0], 0xff, sizeof(alarm_table_st) * ACL_TOTAL_NUM);
        ret += I2C_EE_BufWrite((uint8_t *)&alarm_status_table[0], AlARM_TABLE_ADDR, sizeof(alarm_table_st) * ACL_TOTAL_NUM);
        if (ret == 2 * EEPROM_NOERRO)
        {
            return (1);
        }
        else
        {
            return (0);
        }
    }
}

//获取事件记录
// flag=1;表示获取报警记录
//flag=0；表示获取事件记录
//NO表示距离当前第多少条。NO =0.表示目前条，
//CNT 表示尧都区的数据条数

//函数返回值，表示读出来的事件记录的条数

uint16_t get_log(uint8_t *log_data, uint16_t start_num, uint16_t cont, uint16_t *total_cnt, uint8_t log_type)
{
    if (log_type == ALARM_TYPE)
    {
        //不包含报警状态在内的报警
        I2C_EE_BufRead((uint8_t *)&alarm_cyc_buff_inst.pt_inst, alarm_cyc_buff_inst.eeprom_buff_base_addr, sizeof(record_pt_st));
        //不包含报警未解除的内容
        *(total_cnt) = alarm_cyc_buff_inst.pt_inst.cnt;
        return (quiry_cycbuff(&alarm_cyc_buff_inst, start_num, cont, log_data));
    }
    else
    {
        I2C_EE_BufRead((uint8_t *)&event_cyc_buff_inst.pt_inst, event_cyc_buff_inst.eeprom_buff_base_addr, sizeof(record_pt_st));
        *(total_cnt) = event_cyc_buff_inst.pt_inst.cnt;
        return (quiry_cycbuff(&event_cyc_buff_inst, start_num, cont, log_data));
    }
}

void chain_init(void)
{
    extern sys_reg_st g_sys;
    //ACL_TOTAL_NUM*sizeof(alram_node)
    //sizeof(alram_node)
    //初始化内块
    if (rt_mp_init(&alarm_status_mp_inst, "alarm_satus", &alarm_mempool[0], sizeof(alarm_mempool), sizeof(alram_node)) != RT_EOK)
    {
        rt_kprintf("ALRAM_CAcl :rt_mp_init erro \n");
        return;
    }
    head_node.next = NULL;
    head_node.alarm_value = 0; //报警计数
    g_sys.status.alarm_status_cnt.total_cnt = 0;
    g_sys.status.alarm_status_cnt.major_cnt = 0;
    g_sys.status.alarm_status_cnt.critical_cnt = 0;
    g_sys.status.alarm_status_cnt.mioor_cnt = 0;
}

static uint8_t calc_pwr_off(void)
{

    return 0;
    /*
		const uint8_t power_alarm_table[14]={ACL_POWER_HI_FD,ACL_POWER_LO_FD,ACL_POWER_EP,ACL_POWER_A_HIGH,ACL_POWER_B_HIGH ,ACL_POWER_C_HIGH,
								 ACL_POWER_A_LOW,ACL_POWER_B_LOW,ACL_POWER_C_LOW,ACL_POWER_A_OP,ACL_POWER_B_OP,ACL_POWER_C_OP,ACL_AIR_LOSS,ACL_REMOTE_SHUT};
		const uint8_t compessor1_alarm_table[7]={ACL_HI_PRESS1,ACL_HI_LOCK1,ACL_LO_PRESS1,ACL_LO_LOCK1,ACL_EXTMP1,ACL_EXTMP_LOCK1,ACL_SHORT_TERM1};
		const uint8_t compessor2_alarm_table[7]={ACL_HI_PRESS2,ACL_HI_LOCK2,ACL_LO_PRESS2,ACL_LO_LOCK2,ACL_EXTMP2,ACL_EXTMP_LOCK2,ACL_SHORT_TERM2};		
		const uint8_t fan_alarm_table[8]={ACL_FAN_OVERLOAD1,ACL_FAN_OVERLOAD2,ACL_FAN_OVERLOAD3,ACL_FAN_OVERLOAD4,ACL_FAN_OVERLOAD5,ACL_FAN_OVERLOAD6,ACL_FAN_OVERLOAD7,ACL_FAN_OVERLOAD8};
		const uint8_t wavle_alarm_table[1]={ACL_COIL_BLOCKING};
		uint8_t i,req =0,compressor1_alram =0 ,compressor2_alram =0,fan_alarm_cnt=0;
		
		
		for(i=0;i<sizeof(power_alarm_table);i++)
		{
				req |= get_alarm_bitmap(power_alarm_table[i]);
				if(req ==1)
				{
					return(req);
				}				
		}
		if((get_alarm_bitmap(ACL_WATER_OVERFLOW) == 1)&&(g_sys.config.alarm[ACL_WATER_OVERFLOW].alarm_param == 0))
		{
				req |= 1;
				return(req);
		}
		//compress_alarm 
		if((req ==0)&&((g_sys.config.general.cool_type == COOL_TYPE_MODULE_WIND)||(g_sys.config.general.cool_type == COOL_TYPE_COLUMN_WIND)))
		{
					for(i=0;i<sizeof(compessor1_alarm_table);i++)
					{
								compressor1_alram |=get_alarm_bitmap(compessor1_alarm_table[i]);
								
					}
					req|= compressor1_alram;
					// double compressor
					if(devinfo_get_compressor_cnt() == 2)
					{
							for(i=0;i<sizeof(compessor2_alarm_table);i++)
							{
									compressor2_alram |=get_alarm_bitmap(compessor2_alarm_table[i]);
							}
							req &= compressor2_alram;
					}
					
		}
		// water valve alarm
		else if((req ==0)&&((g_sys.config.general.cool_type == COOL_TYPE_MODULE_WATER)||(g_sys.config.general.cool_type == COOL_TYPE_COLUMN_WATER)))
		{
				for(i=0;i<sizeof(wavle_alarm_table);i++)
				{
						req |=get_alarm_bitmap(wavle_alarm_table[i]);
				}
		}
		else
		{
				;
		}
		// fan_alram
		if(req ==0)
		{
					for(i=0;i<sizeof(fan_alarm_table);i++)
					{
							fan_alarm_cnt+=get_alarm_bitmap(fan_alarm_table[i]);
					}
					if(fan_alarm_cnt !=0)
					{
							if((g_sys.config.general.cool_type == COOL_TYPE_MODULE_WIND)||(g_sys.config.general.cool_type == COOL_TYPE_MODULE_WATER))
							{
									if(fan_alarm_cnt!=0)
									{
										req =1;
									}
							}
							else if((g_sys.config.general.cool_type == COOL_TYPE_COLUMN_WIND)||(g_sys.config.general.cool_type == COOL_TYPE_COLUMN_WATER))
							{
										if((g_sys.config.general.cool_type == COOL_TYPE_COLUMN_WIND)&&((fan_alarm_cnt >=g_sys.config.fan.num)||(fan_alarm_cnt >1)))
										{
												req =1;
										}
										if((g_sys.config.general.cool_type == COOL_TYPE_COLUMN_WATER)&&(fan_alarm_cnt >=g_sys.config.fan.num))
										{
												req =1;
										}
							}
							else 
							{
							}
					}
					
		}
		
		return(req);
*/
}

////计算二进制1个数
//static uint8_t Calc_Bit1(uint16_t Num)
//{
//			uint8 Count=0;
//
//			while(Num!= 0)
//			{
//					Count ++;
//					Num&= (Num - 1);
//			}
//			return Count;
//}
static void calc_alrarm_cnt(void)
{
    alram_node *p_node2;
    uint8_t critical_cnt, major_cnt, mioor_cnt;
    uint16_t alarm_id;
    extern sys_reg_st g_sys;

    p_node2 = head_node.next;
    critical_cnt = 0;
    major_cnt = 0;
    mioor_cnt = 0;

    if (head_node.next == NULL)
    {
        ;
    }
    else
    {
        while (p_node2 != NULL)
        {

            alarm_id = p_node2->alarm_id;
            alarm_id = alarm_id >> 8;
            if ((alarm_id & 0x03) == CRITICAL_ALARM_lEVEL)
            {
                //								//Alair,20161227
                //								if(g_sys.status.Alarm_COM_NTC_BIT[ALARM_COM])
                //								{
                //										critical_cnt+=(Calc_Bit1(g_sys.status.Alarm_COM_NTC_BIT[ALARM_COM])-1);	//后面会加一次，这儿减1
                //								}
                critical_cnt++;
            }
            else if (((alarm_id & 0x03) == MAJOR_ALARM_LEVEL))
            {
                major_cnt++;
            }
            else //3
            {
                mioor_cnt++;
            }
            p_node2 = p_node2->next;
        }
    }
    if ((g_sys.status.alarm_bitmap[5] & WORK_LIMIT_CATION)) //开机口令维护提示告警
    {
        critical_cnt++;
    }
    //	if((g_sys.status.ControlPassword.Run_State&0x3F))//开机管控提示告警
    //	{
    //			mioor_cnt++;
    //	}
    g_sys.status.alarm_status_cnt.total_cnt = critical_cnt + major_cnt + mioor_cnt;
    g_sys.status.alarm_status_cnt.mioor_cnt = mioor_cnt;
    g_sys.status.alarm_status_cnt.pwr_off_alarm = calc_pwr_off();
    if (g_sys.status.alarm_status_cnt.total_cnt > 0)
    {
        sys_set_remap_status(WORK_MODE_STS_REG_NO, ALARM_STUSE_BPOS, 1);
    }
    else
    {
        sys_set_remap_status(WORK_MODE_STS_REG_NO, ALARM_STUSE_BPOS, 0);
    }

    if ((g_sys.status.alarm_status_cnt.major_cnt < major_cnt) || (g_sys.status.alarm_status_cnt.critical_cnt < critical_cnt))
    {
        sys_set_remap_status(WORK_MODE_STS_REG_NO, ALARM_BEEP_BPOS, 1); //严重告警
    }
    g_sys.status.alarm_status_cnt.major_cnt = major_cnt;
    g_sys.status.alarm_status_cnt.critical_cnt = critical_cnt;

    if ((g_sys.status.alarm_status_cnt.major_cnt == 0) && (g_sys.status.alarm_status_cnt.critical_cnt == 0))
    {
        sys_set_remap_status(WORK_MODE_STS_REG_NO, ALARM_BEEP_BPOS, 0);
    }
}
//增加节点
uint8_t node_append(uint16_t alarm_id, uint16_t alarm_value)
{
    extern sys_reg_st g_sys;
    alram_node *p_node1;
    time_t now;

    p_node1 = (alram_node *)rt_mp_alloc(&alarm_status_mp_inst, 0);
    if (p_node1 == RT_NULL)
    {
        return 0;
    }
    get_local_time(&now);
    p_node1->next = head_node.next;
    //节点数量计数

    p_node1->alarm_id = alarm_id;
    p_node1->alarm_value = alarm_value;
    p_node1->trigger_time = now;
    head_node.next = p_node1;
    //计算报警状态总数和报警总类
    calc_alrarm_cnt();
    return 1;
}

//删除节点

uint8_t node_delete(uint16_t alarm_id)
{
    alram_node *p_node1, *p_node2, *p_node3;
    uint8_t cnt;
    extern sys_reg_st g_sys;

    p_node1 = NULL;
    p_node2 = head_node.next;
    p_node3 = &head_node;

    if (head_node.next == NULL)
    {
        return (0);
    }
    //节点数量
    cnt = g_sys.status.alarm_status_cnt.total_cnt;
    while (cnt)
    {
        cnt--;
        if (p_node2->alarm_id == alarm_id)
        {
            p_node1 = p_node2;
            break;
        }
        else
        {
            if (p_node2->next != NULL)
            {
                p_node3 = p_node2;
                p_node2 = p_node2->next;
            }
            else
            {
                break;
            }
        }
    }

    if (p_node1 != NULL)
    {
        p_node3->next = p_node1->next;
        rt_mp_free(p_node1);
        calc_alrarm_cnt();

        return (1);
    }
    return (0);
}

uint8_t get_alarm_status(uint8_t *status_data, uint16_t start_num, uint8_t len)
{
    uint8_t read_len;
    uint8_t index;
    alram_node *start_node_pt;
    extern sys_reg_st g_sys;

    read_len = 0;
    index = 0;
    if (g_sys.status.alarm_status_cnt.total_cnt <= start_num)
    {
        return (0);
    }
    start_node_pt = head_node.next;

    while (start_num--)
    {
        if (start_node_pt == NULL)
        {
            return (0);
        }
        if (start_node_pt->next == NULL)
        {
            return (0);
        }
        start_node_pt = start_node_pt->next;
        rt_kprintf("2start_num = %d,len = %d\n", start_num, len);
    }
    read_len = 0;
    for (index = 0; index < len; index++)
    {
        read_len++;
        *(status_data++) = start_node_pt->alarm_id >> 8;
        *(status_data++) = start_node_pt->alarm_id;
        *(status_data++) = (start_node_pt->trigger_time >> 24);
        *(status_data++) = (start_node_pt->trigger_time >> 16);
        *(status_data++) = (start_node_pt->trigger_time >> 8);
        *(status_data++) = (start_node_pt->trigger_time);
        *(status_data++) = (start_node_pt->alarm_value >> 8);
        *(status_data++) = start_node_pt->alarm_value;

        //	rt_kprintf("\n start_node->alarm_id = %d ,start_node->alarm_value= %d\n",start_node->alarm_id,start_node->alarm_value);

        if (start_node_pt->next == NULL)
        {
            return (read_len);
        }
        start_node_pt = start_node_pt->next;
    }

    return (read_len);
}

/****************************************************************************************************************************************
      温湿度点记录48小时温湿度曲线 每分钟一个点
*********************************************************************************************************************************************/

typedef struct
{
    int16_t temp;
    uint16_t hum;

} hum_tem_st;

typedef struct
{
    uint8_t timer;
    hum_tem_st tem_hum_buff[60];
    uint8_t index_buff;
    hum_tem_st tem_hum_point[60];
    int8_t index_point;
    cyc_buff_st tem_hum_cyc;
} hum_tem_log_st;

static hum_tem_log_st hum_temp_log_inst;

void init_tem_hum_record(void)
{
    uint8_t i;
    hum_temp_log_inst.index_point = 0;
    hum_temp_log_inst.timer = 0;
    hum_temp_log_inst.index_buff = 0;
    //初始化cycbuffer
    //memset((uint8_t*)&hum_temp_log_inst.tem_hum_point[0].temp,0xff00,60*sizeof(hum_tem_st));
    for (i = 0; i < 60; i++)
    {
        hum_temp_log_inst.tem_hum_point[i].hum = INVALID_HUM_TEMP;
        hum_temp_log_inst.tem_hum_point[i].temp = INVALID_HUM_TEMP;
    }
    init_cycbuff(&hum_temp_log_inst.tem_hum_cyc, sizeof(hum_tem_st) * 60, HUM_TEMP_MAX_CNT, TEM_HUM_PT_ADDR);
}

static void calc_hum_temp(void)
{
    uint8_t i, hum_flag, temp_flag;
    uint32_t hum;
    int32_t temp;

    hum_flag = 0;
    temp_flag = 0;
    hum = 0;
    temp = 0;

    for (i = 0; i < 60; i++)
    {
        if (hum_temp_log_inst.tem_hum_buff[i].hum == 0x7fff)
        {
            hum_flag = 1;
            ;
        }
        else
        {
            hum += hum_temp_log_inst.tem_hum_buff[i].hum;
        }

        if (hum_temp_log_inst.tem_hum_buff[i].temp == 0x7fff)
        {
            temp_flag = 1;
        }
        else
        {
            temp += hum_temp_log_inst.tem_hum_buff[i].temp;
        }
    }

    hum = hum / 60;
    temp = temp / 60;
    if (temp_flag)
    {
        temp = INVALID_HUM_TEMP;
    }
    if (hum_flag)
    {
        hum = INVALID_HUM_TEMP;
    }
    hum_temp_log_inst.tem_hum_point[hum_temp_log_inst.index_point].temp = temp;
    hum_temp_log_inst.tem_hum_point[hum_temp_log_inst.index_point].hum = hum;

    hum_temp_log_inst.index_point++;
}

static void update_hum_temp_to_EE(void)
{
    uint8_t i;
    static uint8_t tem_hum_buff[240];

    for (i = 0; i < 60; i++)
    {
        tem_hum_buff[i * 2] = hum_temp_log_inst.tem_hum_point[i].temp >> 8;
        tem_hum_buff[i * 2 + 1] = hum_temp_log_inst.tem_hum_point[i].temp;
    }
    for (i = 0; i < 60; i++)
    {
        tem_hum_buff[i * 2 + 120] = hum_temp_log_inst.tem_hum_point[i].hum >> 8;
        tem_hum_buff[i * 2 + 1 + 120] = hum_temp_log_inst.tem_hum_point[i].hum;
    }

    add_cycbuff(&hum_temp_log_inst.tem_hum_cyc, tem_hum_buff);

    hum_temp_log_inst.index_point = 0;
    for (i = 0; i < 60; i++)
    {
        hum_temp_log_inst.tem_hum_point[i].hum = INVALID_HUM_TEMP;
        ;
        hum_temp_log_inst.tem_hum_point[i].temp = INVALID_HUM_TEMP;
    }
}

// 1s
void add_hum_temp_log(void)
{
    extern sys_reg_st g_sys;

    hum_temp_log_inst.timer++;

    if (g_sys.config.algorithm.ctrl_target_mode == TARGET_MODE_RETURN)
    {
        hum_temp_log_inst.tem_hum_buff[hum_temp_log_inst.index_buff].temp = g_sys.status.sys_tem_hum.return_air_temp;
        hum_temp_log_inst.tem_hum_buff[hum_temp_log_inst.index_buff].hum = g_sys.status.sys_tem_hum.return_air_hum;
    }
    else if (g_sys.config.algorithm.ctrl_target_mode == TARGET_MODE_SUPPLY)
    {
        hum_temp_log_inst.tem_hum_buff[hum_temp_log_inst.index_buff].temp = g_sys.status.sys_tem_hum.supply_air_temp;
        hum_temp_log_inst.tem_hum_buff[hum_temp_log_inst.index_buff].hum = g_sys.status.sys_tem_hum.supply_air_hum;
    }
    else
    {
        hum_temp_log_inst.tem_hum_buff[hum_temp_log_inst.index_buff].temp = g_sys.status.sys_tem_hum.remote_air_temp;
        hum_temp_log_inst.tem_hum_buff[hum_temp_log_inst.index_buff].hum = g_sys.status.sys_tem_hum.remote_air_hum;
    }

    hum_temp_log_inst.index_buff++;
    hum_temp_log_inst.index_buff = hum_temp_log_inst.index_buff % 60;

    if (hum_temp_log_inst.timer >= 60)
    {
        hum_temp_log_inst.timer = 0;
        calc_hum_temp();
    }

    if (hum_temp_log_inst.index_point >= 59)
    {
        update_hum_temp_to_EE();
    }
}

uint8_t get_tem_hum_log(uint8_t *log_data, uint16_t block)
{

    uint8_t req, i;

    req = 0;

    //读取内存数据
    I2C_EE_BufRead((uint8_t *)&hum_temp_log_inst.tem_hum_cyc.pt_inst, hum_temp_log_inst.tem_hum_cyc.eeprom_buff_base_addr, sizeof(record_pt_st));
    *(log_data++) = hum_temp_log_inst.tem_hum_cyc.pt_inst.cnt + 1;

    if (block == 0)
    {
        *(log_data++) = 0;
        *(log_data++) = hum_temp_log_inst.index_point;
        req = 1;
        for (i = 0; i < 60; i++)
        {
            *(log_data + i * 2) = hum_temp_log_inst.tem_hum_point[i].temp >> 8;
            *(log_data + i * 2 + 1) = hum_temp_log_inst.tem_hum_point[i].temp;
        }
        for (i = 0; i < 60; i++)
        {
            *(log_data + i * 2 + 120) = hum_temp_log_inst.tem_hum_point[i].hum >> 8;
            *(log_data + i * 2 + 1 + 120) = hum_temp_log_inst.tem_hum_point[i].hum;
        }
    }
    else
    {

        *(log_data++) = block;
        *(log_data++) = 60;
        req += quiry_cycbuff(&hum_temp_log_inst.tem_hum_cyc, block, 1, log_data);
    }
    return (req);
}

uint8_t clear_tem_hum_log(void)
{
    uint8_t req, i;
    record_pt_st log_pt_inst;

    memset((uint8_t *)&log_pt_inst.startpiont, 0, sizeof(record_pt_st));
    hum_temp_log_inst.index_point = 0;
    hum_temp_log_inst.timer = 0;
    hum_temp_log_inst.index_buff = 0;
    for (i = 0; i < 60; i++)
    {
        hum_temp_log_inst.tem_hum_point[i].hum = INVALID_HUM_TEMP;
        hum_temp_log_inst.tem_hum_point[i].temp = INVALID_HUM_TEMP;
    }

    req = I2C_EE_BufWrite((uint8_t *)&log_pt_inst, TEM_HUM_PT_ADDR, sizeof(record_pt_st));
    if (req == EEPROM_NOERRO)
    {
        req = 1;
    }
    else
    {
        req = 0;
    }
    return (req);
}
