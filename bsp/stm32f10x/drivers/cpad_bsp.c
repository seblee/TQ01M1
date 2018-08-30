#include <rtthread.h>
#include <components.h>
#include "stm32f10x.h"
#include "kits/fifo.h"
#include "global_var.h"
#include "sys_def.h"
#include "rtc_bsp.h"
#include "authentication.h"
#include <time.h>
#include "event_record.h"
#include "password.h"
#include "mb_event_cpad.h"
#include "can_bsp.h"
#include "sys_status.h"
#define CPAD_RS485_SND_MODE  GPIO_SetBits(GPIOA,GPIO_Pin_1)
#define CPAD_RS485_RCV_MODE  GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define RECORD_CMD_FLAG			10000
#define CPAD_TX_BUF_DEPTH 					256
#define CPAD_RX_BUF_DEPTH 					256
#define CPAD_FSM_TIMEOUT	 					2

#define FRAME_CMD_POS								2
#define FRAME_LEN_POS								3

#define FRAME_DATA_0                                4
#define FRAME_DATA_1                                5
#define FRAME_DATA_2                                6
#define FRAME_DATA_3                                7
#define FRAME_DATA_4                                8
#define FRAME_DATA_5                                9

#define RECORD_START_POS_H                          4

#define RECORD_START_POS_L                          5

#define READ_COUNT_POS                              6

#define TEM_HUM_TYPE_POS                            7

#define CPAD_FRAME_TAG_RX_SYNC1			0x1b
#define CPAD_FRAME_TAG_RX_SYNC2			0xdf

#define CPAD_FRAME_TAG_TX_SYNC1			0x9b
#define CPAD_FRAME_TAG_TX_SYNC2			0xdf

#define CPAD_FRAME_FSM_SYNC1				0x01
#define CPAD_FRAME_FSM_SYNC2				0x02
#define CPAD_FRAME_FSM_CMD					0x04
#define CPAD_FRAME_FSM_LEN					0x08
#define CPAD_FRAME_FSM_DATA					0x10

#define CPAD_CMD_RD_REG							0x01
#define CPAD_CMD_WR_REG							0x02

#define CPAD_CMD_CONF_SAVE_OPTION		0x10
#define CPAD_CMD_CONF_LOAD_OPTION		0x11
#define CPAD_CMD_SET_TIME				0x12
#define CPAD_CMD_REQ_TIME				0x13
#define CPAD_CMD_REQ_PERM				0x14
#define CPAD_CMD_CHANGE_PWD				0x15
#define CPAD_CMD_READ_CURRENT_ALARAM    0x16
#define CPAD_CMD_REDA_ALARM_RECORD      0x17
#define CPAD_CMD_EVENT_RECORD           0x18
#define CPAD_CMD_CLEAR_RECORD           0x19
#define CPAD_CMD_READ_TEM_HUM           0x1A
#define CPAD_CMD_CLEAR_RUN_TIME         0x1B
#define CPAD_CMD_CLEAR_ALARM            0x1C
#define CPAD_CMD_DEFAULT_PRAM           0x1D
#define CPAD_CMD_PASSWORD_SET           0x1E
#define CPAD_CMD_PASSWORD_PRAM_SET      0x1f
#define CPAD_CMD_CLEAR_ALARM_BEEP	      0x21

#define CPAD_CMD_CONTROLPASSWORD_SET    0x22
#define CPAD_CMD_CONTROLPASSWORD_VERIFY	0x23

//static fifo8_cb_td cpad_rx_fifo;
//static fifo8_cb_td cpad_tx_fifo;

typedef struct
{
		volatile uint8_t tx_buf[CPAD_TX_BUF_DEPTH];
		volatile uint8_t tx_cnt;
		volatile uint8_t tx_cmd;
		volatile uint8_t rx_buf[CPAD_RX_BUF_DEPTH];
		volatile uint8_t rx_cnt;
		volatile uint8_t rx_tag;
		volatile uint16_t rtx_timeout;
		volatile uint8_t cpad_fsm_cstate;
}cpad_reg_st;


static cpad_reg_st cpad_reg_inst;

/**
  * @brief  cpad rtx buffer initialization 
	* @param  none
  * @retval none
  */
//static void cpad_buf_init(void)
//{
//		uint16_t i;

//		//tx buffer initialization
//		for(i=0;i<CPAD_TX_BUF_DEPTH;i++)
//		{
//				cpad_reg_inst.tx_buf[i] = 0;
//		}
//		for(i=0;i<CPAD_RX_BUF_DEPTH;i++)
//		{
//				cpad_reg_inst.rx_buf[i] = 0;
//		}		
//		cpad_reg_inst.tx_buf[0] = CPAD_FRAME_TAG_TX_SYNC1;
//		cpad_reg_inst.tx_buf[1] = CPAD_FRAME_TAG_TX_SYNC2;
//		cpad_reg_inst.tx_cnt = 0;
//		cpad_reg_inst.tx_cmd = 0;		
//		
//		cpad_reg_inst.rx_cnt = 0;
//		cpad_reg_inst.rx_tag = 0;
//		cpad_reg_inst.rtx_timeout = 0;
//		cpad_reg_inst.cpad_fsm_cstate = CPAD_FRAME_FSM_SYNC1;
//		
//		//rx fifo initialization
//		fifo8_init(&cpad_rx_fifo,1,CPAD_RX_BUF_DEPTH);
//		fifo8_init(&cpad_tx_fifo,1,CPAD_TX_BUF_DEPTH);
//		
//}


/**
  * @brief  cpad module uart device initialilzation
	* @param  baudrate: uart baudrate
  * @retval none
  */
//void cpad_uart_init(uint16_t baudrate)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	USART_DeInit(UART5);
//	//======================时钟初始化=======================================
//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD ,	ENABLE);
//	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_UART5,ENABLE);
//	
//	//======================IO初始化=========================================	
//	//UART5_TX
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	//UART5_RX
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
//	//配置485发送和接收模式
//	//TODO   暂时先写B13 等之后组网测试时再修改
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
//	//======================串口初始化=======================================
//	USART_InitStructure.USART_BaudRate = baudrate;
//	USART_InitStructure.USART_Parity = USART_Parity_No;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;

//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_HardwareFlowControl =	USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;


//	USART_Init(UART5, &USART_InitStructure);
//	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
//	USART_Cmd(UART5, ENABLE);

//	//=====================中断初始化======================================
//	//设置NVIC优先级分组为Group2：0-3抢占式优先级，0-3的响应式优先级
////	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//}


/**
  * @brief  cpad uart send interface
	* @param  tx_buf_ptr: tx src buffer pointer
						tx_cnt: tx src buffer transmitt count
  * @retval none
  */
//void cpad_dev_init(uint16_t baudrate)
//{
//		cpad_buf_init();
//		cpad_uart_init(baudrate);
//}

///**
//  * @brief  cpad uart send interface
//	* @param  tx_buf_ptr: tx src buffer pointer
//						tx_cnt: tx src buffer transmitt count
//  * @retval none
//  */
//void cpad_uart_send(uint8_t* tx_buf_ptr, uint8_t tx_cnt)
//{
//		rt_memcpy((uint8_t *)(cpad_reg_inst.tx_buf), tx_buf_ptr, tx_cnt);
//		cpad_reg_inst.tx_num = tx_cnt;
//		cpad_reg_inst.tx_cnt = 0;
//		USART_ClearFlag(UART5, USART_FLAG_TC);
//		CONSOLE_RS485_SND_MODE;		
//		USART_SendData(UART5, cpad_reg_inst.tx_buf[cpad_reg_inst.tx_cnt]);
//		cpad_reg_inst.tx_cnt++;	
//		USART_ITConfig(UART5, USART_IT_TC, ENABLE);
//}

/**
  * @brief  cpad uart send interface
	* @param  tx_buf_ptr: tx src buffer pointer
						tx_cnt: tx src buffer transmitt count
  * @retval none
  */
//void cpad_uart_send_fifo(void)
//{
//		uint8_t tx_data;
//		USART_ClearFlag(USART2, USART_FLAG_TC);
//		CPAD_RS485_SND_MODE;		
//		if(is_fifo8_empty(&cpad_tx_fifo) == 0)
//		{
//				fifo8_pop(&cpad_tx_fifo,&tx_data);
//				USART_SendData(USART2, tx_data);
//				USART_ITConfig(USART2, USART_IT_TC, ENABLE);
//		}
//}

/**
  * @brief  cpad recieve frame checksum
	* @param  none
  * @retval 
			`0: checksum ok
			`1:	checksum fail
  */
//static uint16_t frame_checksum(void)
//{
//		uint16_t res,i;
//		res = 0;
//		for(i=0;i<(cpad_reg_inst.rx_cnt-1);i++)
//		{
//				res += cpad_reg_inst.rx_buf[i];
//		}
//		res &= 0x00ff;
//		if(res == cpad_reg_inst.rx_buf[i])
//		{
//			return 1;
//		}
//		else
//		{
//			return 0;
//		}
//}

/**
  * @brief  cpad transmmite frame checksum
	* @param  data_ptr: data buffer pointer whose checksum is to be caculated
						data_num: number of data to be caculated
  * @retval caculated checksum
  */
//static uint16_t frame_checksum_gen(uint8_t* data_ptr, uint8_t data_num)
//{
//		uint16_t res,i;
//		res = 0;
//		for(i=0;i<data_num;i++)
//		{
//				res += *(data_ptr+i);
//		}
//		res &= 0x00ff;
//		return res;
//}

/**
  * @brief  cpad recieve frame finite state machine
	* @param  none
  * @retval 
			`0: cpad frame recieve ok
			`1:	cpad frame recieve ng
  */
uint16_t cpad_get_comm_sts(void)
{
		return cpad_reg_inst.rtx_timeout;
}


uint8_t cpad_get_rx_fsm(void)
{
		return cpad_reg_inst.cpad_fsm_cstate;
}

/**
  * @brief  cpad recieve frame finite state machine
	* @param  none
  * @retval 
			`0: cpad frame recieve ok
			`1:	cpad frame recieve ng
  */
uint16_t cpad_frame_recv(void)
{
		if(cpad_reg_inst.rx_tag == 1)	//if there is already an unprocessed frame in the rx buffer, quit new frame recieving
		{
				return 1;
		}
		else
		{
				return 0;
		}
}

/**
  * @brief  cpad send response frame
	* @param  none
  * @retval none
  */
//static void cmd_response(uint16_t ack_type)
//{
//		uint16_t err_code;
//		uint16_t i,tx_cnt,check_sum;
//		err_code = ack_type;

//		cpad_reg_inst.tx_buf[0] = CPAD_FRAME_TAG_TX_SYNC1;
//		cpad_reg_inst.tx_buf[1] = CPAD_FRAME_TAG_TX_SYNC2;		
//	
//		tx_cnt = cpad_reg_inst.tx_cnt;																											//response tx data count set
//		if(err_code == CPAD_ERR_NOERR)																											//response command set
//		{	
//				cpad_reg_inst.tx_buf[2] = cpad_reg_inst.tx_cmd;		
//		}
//		else
//		{
//				cpad_reg_inst.tx_buf[2] = cpad_reg_inst.tx_cmd|0x80;	
//		}
//		cpad_reg_inst.tx_buf[3] = tx_cnt;																										//response frame length
//		check_sum = frame_checksum_gen((uint8_t *)(&cpad_reg_inst.tx_buf[0]),(tx_cnt+4));		//response frame checksum caculate
//		cpad_reg_inst.tx_buf[tx_cnt+4] = check_sum;																					
//		for(i=0;i<tx_cnt+5;i++)																															//fifo test
//		{
//				fifo8_push(&cpad_tx_fifo,(uint8_t *)(&cpad_reg_inst.tx_buf[i]));
//		}
//		if(cpad_reg_inst.tx_cmd != CPAD_CMD_RD_REG)
//		{
//				authen_expire_set();
//		}
//		cpad_uart_send_fifo();																															//send response frame						
//} 

/**
  * @brief  cpad reset configration data to default value 
	* @param  none
  * @retval 
		`CPAD_ERR_NOERR			 : operation OK
		`CPAD_ERR_ADDR_OR	   : requested address out of range
    `CPAD_ERR_DATA_OR	   : requested data out of range
    `CPAD_ERR_PERM_OR	   : request permission denied
    `CPAD_ERR_WR_OR		   : write operation prohibited
    `CPAD_ERR_UNKNOWN	   : unknown error
  */
//static uint16_t cmd_conf_load_option(void)
//{
//		uint8_t err_code;
//		uint16_t ret;
//		uint8_t load_option;
//		extern sys_reg_st	 g_sys; 
//	
//		cpad_reg_inst.tx_cmd	= cpad_reg_inst.rx_buf[FRAME_CMD_POS];		
//		if(g_sys.status.general.permission_level >= 1)
//		{ 
//				err_code = CPAD_ERR_NOERR;	
//		/*add usr code start*/
//			
//				load_option = cpad_reg_inst.rx_buf[4];
//		/*add usr code fin*/		
//				
//				cpad_reg_inst.rx_cnt = 0;																					//clear rx_buffer
//				cpad_reg_inst.rx_tag = 0;						

//		/*add usr code start*/
//				
//				ret = set_load_flag(load_option);
//				if(ret == 0)
//				{
//						err_code =  CPAD_ERR_UNKNOWN;
//				}
//				
//		/*add usr code fin*/
//				cpad_reg_inst.tx_cnt = 1;
//				add_eventlog_fifo((CPAD_CMD_CONF_LOAD_OPTION+RECORD_CMD_FLAG),(USER_CPAD<<8)|g_sys.status.general.permission_level ,0xffff,load_option);
//	}
//	else
//	{
//			err_code = CPAD_ERR_PERM_OR; 
//			cpad_reg_inst.tx_buf[4] = err_code;
//			cpad_reg_inst.tx_cnt = 1;
//	}
//	
//	cmd_response(err_code);
//	return err_code;		
//}

/**
  * @brief  cpad reset configration data to default value 
	* @param  none
  * @retval 
		`CPAD_ERR_NOERR			 : operation OK
		`CPAD_ERR_ADDR_OR	   : requested address out of range
    `CPAD_ERR_DATA_OR	   : requested data out of range
    `CPAD_ERR_PERM_OR	   : request permission denied
    `CPAD_ERR_WR_OR		   : write operation prohibited
    `CPAD_ERR_UNKNOWN	   : unknown error
  */
//static uint16_t cmd_conf_save_option(void)
//{
//		uint8_t err_code;
//		uint16_t ret;
//		uint8_t save_option;
//		extern sys_reg_st	 g_sys; 

//	 cpad_reg_inst.tx_cmd	= cpad_reg_inst.rx_buf[FRAME_CMD_POS];	
//	 if(g_sys.status.general.permission_level >= 1)
//	 {
//			err_code = CPAD_ERR_NOERR;	
//	/*add usr code start*/
//		
//			save_option = cpad_reg_inst.rx_buf[4];
//	/*add usr code fin*/		
//			
//			cpad_reg_inst.rx_cnt = 0;																					//clear rx_buffer
//			cpad_reg_inst.rx_tag = 0;						

//	/*add usr code start*/
//			
//			ret = save_conf_reg(save_option);
//			if(ret != 0)
//			{
//					err_code =  CPAD_ERR_UNKNOWN;
//			}
//			
//	/*add usr code fin*/
//			cpad_reg_inst.tx_cnt = 1;
//			add_eventlog_fifo((CPAD_CMD_CONF_SAVE_OPTION+RECORD_CMD_FLAG),(USER_CPAD<<8)|g_sys.status.general.permission_level ,0xffff,save_option);
//		}
//		else
//		{
//				err_code = CPAD_ERR_PERM_OR; 
//				cpad_reg_inst.tx_buf[4] = err_code;
//				cpad_reg_inst.tx_cnt = 1;
//		}
//		cmd_response(err_code);
//		return err_code;		
//}


/**
  * @brief  cpad command quiry system permission level 
	* @param  none
  * @retval 
		`CPAD_ERR_NOERR			 : operation OK
		`CPAD_ERR_ADDR_OR	   : requested address out of range
    `CPAD_ERR_DATA_OR	   : requested data out of range
    `CPAD_ERR_PERM_OR	   : request permission denied
    `CPAD_ERR_WR_OR		   : write operation prohibited
    `CPAD_ERR_UNKNOWN	   : unknown error
  */
//static uint16_t cmd_req_perm(void)
//{
//		uint8_t err_code;
//		em_usr_name usr_name;
//		uint8_t	password_buf[16];
//		uint8_t password_len;
//		uint16_t i;
//		uint16_t ret;
//		uint16_t permission_level;
//		extern sys_reg_st	 g_sys; 
//	
//		err_code = CPAD_ERR_NOERR;	
//		permission_level = g_sys.status.general.permission_level;
//	

//		cpad_reg_inst.tx_cmd	= cpad_reg_inst.rx_buf[FRAME_CMD_POS];		
//	
//		usr_name = (em_usr_name)cpad_reg_inst.rx_buf[4];							//get user name
//		password_len = cpad_reg_inst.rx_buf[5];												//get password length
//		
//		for(i=0;i<password_len;i++)
//		{
//				password_buf[i] = cpad_reg_inst.rx_buf[6+i];
//		}

//		cpad_reg_inst.rx_cnt = 0;																			//clear rx_buffer
//		cpad_reg_inst.rx_tag = 0;				
//		

///*add usr code*/
//		
//		ret = authen_verify(usr_name,password_buf,password_len);
//		if(ret != 1)
//		{
//				err_code = CPAD_ERR_UNKNOWN;
//		}
//		else
//		{
//				err_code = CPAD_ERR_NOERR;
//		}
//		
///*add usr code fin*/
//		cpad_reg_inst.tx_cnt = 1;
//		cpad_reg_inst.tx_buf[4] = err_code;
//		
//		cmd_response(err_code);
//		
//		add_eventlog_fifo((CPAD_CMD_REQ_PERM+RECORD_CMD_FLAG),(USER_CPAD<<8)|permission_level ,permission_level,g_sys.status.general.permission_level);
//		return err_code;
//}

/**
  * @brief  cpad command change password 
	* @param  none
  * @retval 
		`CPAD_ERR_NOERR			 : operation OK
		`CPAD_ERR_ADDR_OR	   : requested address out of range
    `CPAD_ERR_DATA_OR	   : requested data out of range
    `CPAD_ERR_PERM_OR	   : request permission denied
    `CPAD_ERR_WR_OR		   : write operation prohibited
    `CPAD_ERR_UNKNOWN	   : unknown error
  */
//static uint16_t cmd_change_pwd(void)
//{
//		uint8_t err_code;
//		uint16_t permission_level;
//		em_usr_name usr_name;
//		uint8_t	password_buf_old[16];
//		uint8_t	password_buf_new[16];
//		uint8_t password_len_old,password_len_new;
//		uint16_t i;
//		uint16_t ret;
//		uint16_t new_pwd_pos;
//		extern sys_reg_st	 g_sys; 
//	
//		err_code = CPAD_ERR_NOERR;	
//		permission_level = g_sys.status.general.permission_level;
//	
//		cpad_reg_inst.tx_cmd	= cpad_reg_inst.rx_buf[FRAME_CMD_POS];		

///*add usr code start*/
//	
//		usr_name = (em_usr_name)cpad_reg_inst.rx_buf[4];									//get user name
//		password_len_old = cpad_reg_inst.rx_buf[5];												//get old password length
//		password_len_new = cpad_reg_inst.rx_buf[6];												//get new password length		
//									
//		for(i=0;i<password_len_old;i++)																		//get old password
//		{
//				password_buf_old[i] = cpad_reg_inst.rx_buf[7+i];
//		}
//		
//		new_pwd_pos = 7 + password_len_old;
//		
//		for(i=0;i<password_len_new;i++)																		//get new password
//		{
//				password_buf_new[i] = cpad_reg_inst.rx_buf[new_pwd_pos+i];
//		}		
//		
///*add usr code fin*/		
//		
//		cpad_reg_inst.rx_cnt = 0;																					//clear rx_buffer
//		cpad_reg_inst.rx_tag = 0;				
//		

///*add usr code start*/
//		
//		ret = authen_verify(usr_name,password_buf_old,password_len_old);
//		if(ret != 1)
//		{
//				err_code = CPAD_ERR_UNKNOWN;
//		}
//		else
//		{
//				ret = authen_revise(usr_name,password_buf_new,password_len_new);
//				if(ret != 1)
//				{
//						err_code = CPAD_ERR_UNKNOWN;
//				}
//		}
//		
///*add usr code fin*/
//		cpad_reg_inst.tx_buf[4] = err_code;
//		cpad_reg_inst.tx_cnt = 1;
//		cmd_response(err_code);
//		add_eventlog_fifo((CPAD_CMD_CHANGE_PWD+RECORD_CMD_FLAG),(USER_CPAD<<8)|permission_level ,permission_level,g_sys.status.general.permission_level);
//		return err_code;		
//}


/**
  * @brief  cpad command cmd_alarm_statue 
	* @param  none
  * @retval 
		`CPAD_ERR_NOERR			 : operation OK
		`CPAD_ERR_ADDR_OR	   : requested address out of range
    `CPAD_ERR_DATA_OR	   : requested data out of range
    `CPAD_ERR_PERM_OR	   : request permission denied
    `CPAD_ERR_WR_OR		   : write operation prohibited
    `CPAD_ERR_UNKNOWN	   : unknown error
  */
//static uint16_t cmd_req_alarm_statue(void)
//{
//		uint8_t err_code;
//		uint16_t start_pos=0;
//		uint16_t ret;
//		extern sys_reg_st	 g_sys; 
//	
//		cpad_reg_inst.tx_cmd = cpad_reg_inst.rx_buf[FRAME_CMD_POS];	
//		if(1)/*(g_sys.status.general.permission_level >= 0)*/
//		{
//				err_code = CPAD_ERR_NOERR;		
//				cpad_reg_inst.rx_cnt = 0;																		//clear rx_buffer
//				cpad_reg_inst.rx_tag = 0;				
//				
//				if( cpad_reg_inst.rx_buf[READ_COUNT_POS]*ALARM_STATUS_LEN > 256)
//				{
//					err_code = CPAD_ERR_NOERR;	
//				}
//				else
//				{
//					start_pos = cpad_reg_inst.rx_buf[RECORD_START_POS_H];
//					start_pos = (start_pos<<8)+  cpad_reg_inst.rx_buf[RECORD_START_POS_L];
//					
//					cpad_reg_inst.tx_buf[4] = cpad_reg_inst.rx_buf[RECORD_START_POS_H];
//					cpad_reg_inst.tx_buf[5] = cpad_reg_inst.rx_buf[RECORD_START_POS_L];
//					cpad_reg_inst.tx_buf[6] = cpad_reg_inst.rx_buf[READ_COUNT_POS];
//					cpad_reg_inst.tx_buf[7] = ALARM_STATUS_LEN;
//					ret = get_alarm_status((uint8_t*)&cpad_reg_inst.tx_buf[8],start_pos,cpad_reg_inst.rx_buf[READ_COUNT_POS]);
//					
//					cpad_reg_inst.tx_cnt = ret*ALARM_STATUS_LEN+4;

//					if(ret == 0)
//					{
//						err_code = CPAD_ERR_ADDR_OR; 
//					}
//				}
//	  }
//		else
//		{
//				err_code = CPAD_ERR_PERM_OR; 
//		}
//		
//		if(err_code != CPAD_ERR_NOERR)
//		{
//				cpad_reg_inst.tx_buf[4] = err_code;	
//				cpad_reg_inst.tx_cnt = 1;
//		}
//		cmd_response(err_code);
//		return err_code;
//}



/**
  * @brief  cpad command cmd_alarm_statue 
	* @param  none
  * @retval 
		`CPAD_ERR_NOERR			 : operation OK
		`CPAD_ERR_ADDR_OR	   : requested address out of range
    `CPAD_ERR_DATA_OR	   : requested data out of range
    `CPAD_ERR_PERM_OR	   : request permission denied
    `CPAD_ERR_WR_OR		   : write operation prohibited
    `CPAD_ERR_UNKNOWN	   : unknown error
  */
//static uint16_t cmd_req_alarm_record(void)
//{
//		uint8_t err_code;
//		uint16_t start_pos = 0;
//		uint16_t ret;
//		uint16_t total_cnt;
//		extern sys_reg_st	 g_sys; 
//	
//		cpad_reg_inst.tx_cmd = cpad_reg_inst.rx_buf[FRAME_CMD_POS];	
//		if(1)/*(g_sys.status.general.permission_level >= 0)*/
//		{
//				err_code = CPAD_ERR_NOERR;		
//				cpad_reg_inst.rx_cnt = 0;																		//clear rx_buffer
//				cpad_reg_inst.rx_tag = 0;				
//				if( cpad_reg_inst.rx_buf[READ_COUNT_POS]*ALARM_RECORD_LEN > 256)
//				{
//					err_code = CPAD_ERR_DATA_OR;	
//				}
//				else
//				{
//					start_pos = cpad_reg_inst.rx_buf[RECORD_START_POS_H];
//					start_pos = (start_pos<<8)+ cpad_reg_inst.rx_buf[RECORD_START_POS_L];
//					ret = get_log((uint8_t*)&cpad_reg_inst.tx_buf[10],start_pos,cpad_reg_inst.rx_buf[READ_COUNT_POS],&total_cnt,ALARM_TYPE);
//					cpad_reg_inst.tx_buf[4] = cpad_reg_inst.rx_buf[RECORD_START_POS_H];
//					cpad_reg_inst.tx_buf[5] = cpad_reg_inst.rx_buf[RECORD_START_POS_L];
//					cpad_reg_inst.tx_buf[6] = cpad_reg_inst.rx_buf[READ_COUNT_POS];
//					cpad_reg_inst.tx_buf[7] = ALARM_RECORD_LEN;
//					cpad_reg_inst.tx_buf[8] = total_cnt>>8;
//					cpad_reg_inst.tx_buf[9] = total_cnt;
//				
//					
//					cpad_reg_inst.tx_cnt = ret*ALARM_RECORD_LEN+6;

//					if(ret == 0)
//					{
//						err_code = CPAD_ERR_ADDR_OR; 
//					}
//				}
//		}
//		else
//		{
//					err_code = CPAD_ERR_PERM_OR; 
//		}
//		
//		if(err_code != CPAD_ERR_NOERR)
//		{
//				cpad_reg_inst.tx_buf[4] = err_code;	
//				cpad_reg_inst.tx_cnt = 1;
//		}
//		cmd_response(err_code);
//		return err_code;
//}

	/**
	  * @brief	cpad command cmd_alarm_statue 
		* @param  none
	  * @retval 
			`CPAD_ERR_NOERR 		 : operation OK
			`CPAD_ERR_ADDR_OR	   : requested address out of range
		`CPAD_ERR_DATA_OR	   : requested data out of range
		`CPAD_ERR_PERM_OR	   : request permission denied
		`CPAD_ERR_WR_OR 	   : write operation prohibited
		`CPAD_ERR_UNKNOWN	   : unknown error
	  */
//	static uint16_t cmd_req_tem_hum_record(void)
//	{
//			uint8_t err_code;
//			uint16_t start_pos = 0;
//			uint16_t ret;
//			extern sys_reg_st	 g_sys;
//			cpad_reg_inst.tx_cmd = cpad_reg_inst.rx_buf[FRAME_CMD_POS]; 
//			if(1)/*(g_sys.status.general.permission_level >= 0)*/
//		  {
//				err_code = CPAD_ERR_NOERR;	
//				cpad_reg_inst.rx_cnt = 0;																		//clear rx_buffer
//				cpad_reg_inst.rx_tag = 0;				
//			
//				start_pos = cpad_reg_inst.rx_buf[FRAME_DATA_0];
//				//start_pos = (start_pos<<8)+  cpad_reg_inst.rx_buf[RECORD_START_POS_L];
//				ret = get_tem_hum_log((uint8_t*)&cpad_reg_inst.tx_buf[4],start_pos);
//				cpad_reg_inst.tx_cnt = ret*TEMP_HUM_RECORD_LEN+3;
//	
//				if(ret == 0)
//				{
//					err_code = CPAD_ERR_ADDR_OR; 
//					cpad_reg_inst.tx_buf[4] = err_code;	
//					cpad_reg_inst.tx_cnt = 1;
//					
//				}
//				
//		  }
//			
//			else
//			{
//						err_code = CPAD_ERR_PERM_OR; 
//						cpad_reg_inst.tx_buf[4] = err_code;
//						cpad_reg_inst.tx_cnt = 1;
//			}
//			cmd_response(err_code);
//			return err_code;
//	}

	
//static uint16_t cmd_clear_dev_run_time(void)
//{
//			uint8_t err_code;
//	
//			extern sys_reg_st	 g_sys; 
//							
//			cpad_reg_inst.tx_cmd = cpad_reg_inst.rx_buf[FRAME_CMD_POS]; 
//			if(g_sys.status.general.permission_level >= 1)
//			{
//					err_code = CPAD_ERR_NOERR;	
//	
//					cpad_reg_inst.rx_cnt = 0;																		//clear rx_buffer
//					cpad_reg_inst.rx_tag = 0;				
//					if(reset_runtime(cpad_reg_inst.rx_buf[FRAME_DATA_0]) == 0)
//					{
//						 err_code=CPAD_ERR_ADDR_OR;
//					}
//					
//					cpad_reg_inst.tx_buf[4] = err_code;			
//					cpad_reg_inst.tx_cnt = 1;
//					
//					add_eventlog_fifo((CPAD_CMD_CLEAR_RUN_TIME+RECORD_CMD_FLAG),(USER_CPAD<<8)|g_sys.status.general.permission_level,
//					cpad_reg_inst.rx_buf[FRAME_DATA_0],cpad_reg_inst.rx_buf[FRAME_DATA_1]);	
//			}
//			else
//			{
//					err_code = CPAD_ERR_PERM_OR; 
//					cpad_reg_inst.tx_buf[4] = err_code;
//					cpad_reg_inst.tx_cnt = 1;
//			}
//			cmd_response(err_code);
//			return err_code;
//}


//static uint16_t cmd_clear_alarm(void)
//{
//			uint8_t err_code;
//	
//			extern sys_reg_st	 g_sys; 
//							
//			cpad_reg_inst.tx_cmd = cpad_reg_inst.rx_buf[FRAME_CMD_POS]; 	
//			if(g_sys.status.general.permission_level >= 1)
//			{
//					err_code = CPAD_ERR_NOERR;	

//					cpad_reg_inst.rx_cnt = 0;																		//clear rx_buffer
//					cpad_reg_inst.rx_tag = 0;		
//			
//					if(clear_alarm() == 0)
//					{
//						 err_code=CPAD_ERR_ADDR_OR;
//					}
//					
//					cpad_reg_inst.tx_buf[4] = err_code;			
//					cpad_reg_inst.tx_cnt = 1;
//				
//					add_eventlog_fifo((CPAD_CMD_CLEAR_ALARM+RECORD_CMD_FLAG),(USER_CPAD<<8)|g_sys.status.general.permission_level,
//					cpad_reg_inst.rx_buf[FRAME_DATA_0],cpad_reg_inst.rx_buf[FRAME_DATA_0]);
//			}
//			else
//			{
//					err_code = CPAD_ERR_PERM_OR; 
//					cpad_reg_inst.tx_buf[4] = err_code;
//					cpad_reg_inst.tx_cnt = 1;
//			}

//			cmd_response(err_code);
//			return err_code;
//}

//static uint16_t cmd_default_pram_func(void)
//{
//		  uint8_t err_code;
//			extern sys_reg_st	 g_sys; 				
//			cpad_reg_inst.tx_cmd = cpad_reg_inst.rx_buf[FRAME_CMD_POS]; 	
//			if(g_sys.status.general.permission_level >= 3)
//			{
//					err_code = CPAD_ERR_NOERR;	

//					cpad_reg_inst.rx_cnt = 0;																		//clear rx_buffer
//					cpad_reg_inst.rx_tag = 0;		
//			
//					if( load_factory_pram() == 0)
//					{
//						 err_code=CPAD_ERR_ADDR_OR;
//					
//					}
//					
//					cpad_reg_inst.tx_buf[4] = err_code;			
//					cpad_reg_inst.tx_cnt = 1;
//				
//					add_eventlog_fifo((CPAD_CMD_DEFAULT_PRAM+RECORD_CMD_FLAG),(USER_CPAD<<8)|g_sys.status.general.permission_level,
//					cpad_reg_inst.rx_buf[FRAME_DATA_0],cpad_reg_inst.rx_buf[FRAME_DATA_0]);
//			}
//			else
//			{
//					err_code = CPAD_ERR_PERM_OR; 
//					cpad_reg_inst.tx_buf[4] = err_code;
//					cpad_reg_inst.tx_cnt = 1;
//					
//			}
//			
//			cmd_response(err_code);
//			rt_thread_delay(3000);
//			if(err_code == CPAD_ERR_NOERR)
//			{
//					NVIC_SystemReset();
//			}
//			return err_code;
//}

/**
  * @brief  cpad command cmd_alarm_statue 
	* @param  none
  * @retval 
		`CPAD_ERR_NOERR			 : operation OK
		`CPAD_ERR_ADDR_OR	   : requested address out of range
    `CPAD_ERR_DATA_OR	   : requested data out of range
    `CPAD_ERR_PERM_OR	   : request permission denied
    `CPAD_ERR_WR_OR		   : write operation prohibited
    `CPAD_ERR_UNKNOWN	   : unknown error
  */
//static uint16_t cmd_req_event_record(void)
//{
//		uint8_t err_code;
//		uint16_t start_pos = 0;
//		uint16_t ret;
//		uint16_t total_cnt;
//		extern sys_reg_st	 g_sys; 
//	
//		cpad_reg_inst.tx_cmd = cpad_reg_inst.rx_buf[FRAME_CMD_POS];	
//		if(1)/*(g_sys.status.general.permission_level >= 0)*/
//		{
//				err_code = CPAD_ERR_NOERR;		
//				cpad_reg_inst.rx_cnt = 0;																		//clear rx_buffer
//				cpad_reg_inst.rx_tag = 0;				
//					
//				if( cpad_reg_inst.rx_buf[READ_COUNT_POS]*EVENT_RECORD_LEN > 256)
//				{
//					err_code = CPAD_ERR_DATA_OR;	
//					
//				}
//				else
//				{
//					start_pos = cpad_reg_inst.rx_buf[RECORD_START_POS_H];
//					start_pos = (start_pos<<8)+  cpad_reg_inst.rx_buf[RECORD_START_POS_L];
//					
//					ret = get_log((uint8_t*)&cpad_reg_inst.tx_buf[10],start_pos,cpad_reg_inst.rx_buf[READ_COUNT_POS],&total_cnt,EVENT_TYPE);
//					cpad_reg_inst.tx_buf[4] = cpad_reg_inst.rx_buf[RECORD_START_POS_H];
//					cpad_reg_inst.tx_buf[5] = cpad_reg_inst.rx_buf[RECORD_START_POS_L];
//					cpad_reg_inst.tx_buf[6] = cpad_reg_inst.rx_buf[READ_COUNT_POS];
//					cpad_reg_inst.tx_buf[7] = EVENT_RECORD_LEN;
//					
//					cpad_reg_inst.tx_buf[8] = total_cnt>>8;
//					cpad_reg_inst.tx_buf[9] = total_cnt;
//					
//					cpad_reg_inst.tx_cnt = ret*EVENT_RECORD_LEN + 6;
//					
//					if(ret == 0)
//					{
//						err_code = CPAD_ERR_ADDR_OR;
//					}
//				}
//	 }
//		else
//		{
//					err_code = CPAD_ERR_PERM_OR; 
//	
//		}
//		if(err_code != CPAD_ERR_NOERR)
//		{
//				cpad_reg_inst.tx_buf[4] = err_code;	
//				cpad_reg_inst.tx_cnt = 1;
//		}
//		cmd_response(err_code);
//		return err_code;
//}




/**
  * @brief  cpad command cmd_alarm_statue 
	* @param  none
  * @retval 
		`CPAD_ERR_NOERR			 : operation OK
		`CPAD_ERR_ADDR_OR	   : requested address out of range
    `CPAD_ERR_DATA_OR	   : requested data out of range
    `CPAD_ERR_PERM_OR	   : request permission denied
    `CPAD_ERR_WR_OR		   : write operation prohibited
    `CPAD_ERR_UNKNOWN	   : unknown error
  */
//static uint16_t cmd_req_event_clear(void)
//{
//		uint8_t err_code;
//		extern sys_reg_st	 g_sys; 
//		
//		cpad_reg_inst.tx_cmd = cpad_reg_inst.rx_buf[FRAME_CMD_POS];	
//		if(g_sys.status.general.permission_level >= 2)
//		{
//				err_code = CPAD_ERR_NOERR;		
//				cpad_reg_inst.rx_cnt = 0;																		//clear rx_buffer
//				cpad_reg_inst.rx_tag = 0;
//			
//				if( cpad_reg_inst.rx_buf[FRAME_DATA_0] == EVENT_TYPE )
//				{
//					if(clear_log(EVENT_TYPE))
//					{
//						;
//					}
//					else
//					{
//						err_code = CPAD_ERR_PERM_OR;
//					}
//				}
//				else if( cpad_reg_inst.rx_buf[FRAME_DATA_0] == ALARM_TYPE )
//				{
//					if(clear_log(ALARM_TYPE))
//					{
//						;
//					}
//					else
//					{
//						err_code = CPAD_ERR_PERM_OR;
//					}
//				}
//				else if (cpad_reg_inst.rx_buf[FRAME_DATA_0] == TEM_HUM_TYPE)
//				{
//					
//					if(clear_tem_hum_log())
//					{
//						;
//					}
//					else
//					{
//						err_code = CPAD_ERR_PERM_OR;
//					}
//				}
//				else
//				{
//					err_code = CPAD_ERR_PERM_OR;
//				}
//				
//				add_eventlog_fifo((CPAD_CMD_CLEAR_RECORD+RECORD_CMD_FLAG),(USER_CPAD<<8)|g_sys.status.general.permission_level ,cpad_reg_inst.rx_buf[FRAME_DATA_0],0xffff);
//				cpad_reg_inst.tx_cnt = 1;	
//				cpad_reg_inst.tx_buf[4] = err_code;
//		}
//		else
//		{
//					err_code = CPAD_ERR_PERM_OR; 
//					cpad_reg_inst.tx_buf[4] = err_code;
//					cpad_reg_inst.tx_cnt = 1;
//		}
//		cmd_response(err_code);
//		return err_code;
//}


/**
  * @brief  cpad command quiry system time 
	* @param  none
  * @retval 
		`CPAD_ERR_NOERR			 : operation OK
		`CPAD_ERR_ADDR_OR	   : requested address out of range
    `CPAD_ERR_DATA_OR	   : requested data out of range
    `CPAD_ERR_PERM_OR	   : request permission denied
    `CPAD_ERR_WR_OR		   : write operation prohibited
    `CPAD_ERR_UNKNOWN	   : unknown error
  */
//static uint16_t cmd_req_time(void)
//{
//    time_t now;
//		uint8_t err_code;
//		uint32_t time_buf;
//		extern sys_reg_st	 g_sys; 
//	
//		cpad_reg_inst.tx_cmd	= cpad_reg_inst.rx_buf[FRAME_CMD_POS];	
//		if(1)/*(g_sys.status.general.permission_level >= 0)*/
//		{
//			err_code = CPAD_ERR_NOERR;
//			time(&now);		
//			time_buf = now;
//			cpad_reg_inst.rx_cnt = 0;								//clear rx_buffer
//			cpad_reg_inst.rx_tag = 0;
//		
//			cpad_reg_inst.tx_buf[4] = (uint8_t)((time_buf>>24)&0x000000ff);
//			cpad_reg_inst.tx_buf[5] = (uint8_t)((time_buf>>16)&0x000000ff);
//			cpad_reg_inst.tx_buf[6] = (uint8_t)((time_buf>>8)&0x000000ff);
//			cpad_reg_inst.tx_buf[7] = (uint8_t)(time_buf&0x000000ff);

//			cpad_reg_inst.tx_cnt = 4;
//		}
//		else
//		{
//				err_code = CPAD_ERR_PERM_OR;
//				cpad_reg_inst.tx_buf[4] = err_code;
//				cpad_reg_inst.tx_cnt = 1;
//		}
//		cmd_response(err_code);
//		return err_code;
//}



/**
  * @brief  cpad command set system time 
	* @param  none
  * @retval 
		`CPAD_ERR_NOERR			 : operation OK
		`CPAD_ERR_ADDR_OR	   : requested address out of range
    `CPAD_ERR_DATA_OR	   : requested data out of range
`CPAD_ERR_WR_OR		   : write operation prohibited
    `CPAD_ERR_UNKNOWN	   : unknown error
  */


//static uint16_t cmd_set_time(void)
//{
//    uint8_t err_code;
//		extern sys_reg_st	 g_sys; 
//	
//		cpad_reg_inst.tx_cmd	= cpad_reg_inst.rx_buf[FRAME_CMD_POS];
//		if(g_sys.status.general.permission_level >= 1)
//		{
//				time_t now,lastime;
//				rt_device_t device;
//			
//				err_code = CPAD_ERR_NOERR;
//				time(&lastime);	
//				now = cpad_reg_inst.rx_buf[4];
//				now = (now<<8)|cpad_reg_inst.rx_buf[5];
//				now = (now<<8)|cpad_reg_inst.rx_buf[6];
//				now = (now<<8)|cpad_reg_inst.rx_buf[7];

//				cpad_reg_inst.rx_cnt = 0;								//clear rx_buffer
//				cpad_reg_inst.rx_tag = 0;				
//				device = rt_device_find("rtc");
//			
//				if (device != RT_NULL)
//				{
//						rt_rtc_control(device, RT_DEVICE_CTRL_RTC_SET_TIME, &now);
//				}			
//				else
//				{
//						err_code = CPAD_ERR_UNKNOWN;
//				}	
//				cpad_reg_inst.tx_buf[4] = err_code;
//				cpad_reg_inst.tx_cnt = 1;
//				add_eventlog_fifo((CPAD_CMD_SET_TIME+RECORD_CMD_FLAG),(USER_CPAD<<8)|g_sys.status.general.permission_level ,lastime,now);
//		}
//		else
//		{
//					err_code = CPAD_ERR_PERM_OR; 
//					cpad_reg_inst.tx_buf[4] = err_code;
//					cpad_reg_inst.tx_cnt = 1;
//		}
//		cmd_response(err_code);
//		return err_code;

//}



/**
  * @brief  cpad command write reg operation
	* @param  none
  * @retval 
		`CPAD_ERR_NOERR			 : operation OK
		`CPAD_ERR_ADDR_OR	   : requested address out of range
    `CPAD_ERR_DATA_OR	   : requested data out of range
    `CPAD_ERR_PERM_OR	   : request permission denied
    `CPAD_ERR_WR_OR		   : write operation prohibited
    `CPAD_ERR_UNKNOWN	   : unknown error
  */
//static uint16_t cmd_wr_reg(void)
//{
//		uint8_t err_code;
//		uint16_t tx_data[(CPAD_TX_BUF_DEPTH/2)];
//		uint16_t i,tx_addr,tx_cnt;
//		extern sys_reg_st	 g_sys; 
//	
//		cpad_reg_inst.tx_cmd	= cpad_reg_inst.rx_buf[FRAME_CMD_POS];	
////		if(1)	//modified by GP, remove power-on/off authentication--20150818
////		{
//				err_code = CPAD_ERR_NOERR;
//				tx_addr = cpad_reg_inst.rx_buf[4];
//				tx_addr = tx_addr <<8;
//				tx_addr |= cpad_reg_inst.rx_buf[5];	
//				tx_cnt = cpad_reg_inst.rx_buf[6];			

//				for(i=0;i<tx_cnt;i++)
//				{
//						tx_data[i] = cpad_reg_inst.rx_buf[7+2*i];
//						tx_data[i] = tx_data[i]<<8;
//						tx_data[i] |= cpad_reg_inst.rx_buf[7+2*i+1];
//				}
//				cpad_reg_inst.rx_cnt = 0;																																//clear rx_buffer
//				cpad_reg_inst.rx_tag = 0;		
//				err_code = reg_map_write(tx_addr,tx_data,tx_cnt,USER_CPAD);																				//write conf reg map	
//				//change  can baudrate
//				if((tx_addr<=133)&&((tx_addr+tx_cnt)>=133))
//				{	
//					 can_port_baudrate();
//				}
//				cpad_reg_inst.tx_buf[4] = err_code;
//				cpad_reg_inst.tx_cnt = 1;
////		}			//response tx data count set
////		else
////		{
////				cpad_reg_inst.tx_cmd	= cpad_reg_inst.rx_buf[FRAME_CMD_POS];		
////				err_code = CPAD_ERR_PERM_OR; 
////				cpad_reg_inst.tx_buf[4] = err_code;
////				cpad_reg_inst.tx_cnt = 1;
////		}

//		cmd_response(err_code);
//		return err_code;
//}

/**
  * @brief  cpad command write reg operation
	* @param  none
  * @retval 
		`CPAD_ERR_NOERR			 : operation OK
		`CPAD_ERR_ADDR_OR	   : requested address out of range
    `CPAD_ERR_DATA_OR	   : requested data out of range
    `CPAD_ERR_PERM_OR	   : request permission denied
    `CPAD_ERR_WR_OR		   : write operation prohibited
    `CPAD_ERR_UNKNOWN	   : unknown error
  */
//static uint16_t cmd_rd_reg(void)
//{
//		uint8_t err_code;
//		uint16_t rd_data[(CPAD_RX_BUF_DEPTH/2)];
//		uint16_t i,rd_addr,rd_cnt;
//	
//		err_code = CPAD_ERR_NOERR;
//		rd_addr = cpad_reg_inst.rx_buf[4];
//		rd_addr = rd_addr <<8;
//		rd_addr |= cpad_reg_inst.rx_buf[5];	
//		rd_cnt = cpad_reg_inst.rx_buf[6];		
//		
//		cpad_reg_inst.tx_cmd	= cpad_reg_inst.rx_buf[FRAME_CMD_POS];		
//	
//		cpad_reg_inst.rx_cnt = 0;																																//clear rx_buffer
//		cpad_reg_inst.rx_tag = 0;		

//		if(rd_cnt > (CPAD_RX_BUF_DEPTH/2))
//		{
//				err_code =  CPAD_ERR_UNKNOWN;
//		}	
//		else
//		{
//				err_code = reg_map_read(rd_addr,rd_data,rd_cnt);
//		}		

//		if(err_code == CPAD_ERR_NOERR)
//		{
//				cpad_reg_inst.tx_cnt = (rd_cnt+1)<<1;
//				cpad_reg_inst.tx_buf[4] =  cpad_reg_inst.rx_buf[FRAME_DATA_0];
//				cpad_reg_inst.tx_buf[5] =  cpad_reg_inst.rx_buf[FRAME_DATA_1];
//				for(i=0;i<rd_cnt;i++) 																															//response frame tx data add
//				{
//						cpad_reg_inst.tx_buf[6+2*i] = (rd_data[i]>>8)&0xff;
//						cpad_reg_inst.tx_buf[6+2*i+1] = (rd_data[i])&0xff;
//				}
//		}
//		else
//		{
//				cpad_reg_inst.tx_buf[4] = err_code;
//				cpad_reg_inst.tx_cnt = 1;		
//		}			
//		cmd_response(err_code);
//		return err_code;
//}
/**
  * @brief  cpad command set password
	* @param  none
  * @retval 
		`CPAD_ERR_NOERR			 : operation OK
		`CPAD_ERR_ADDR_OR	   : requested address out of range
    `CPAD_ERR_DATA_OR	   : requested data out of range
    `CPAD_ERR_PERM_OR	   : request permission denied
    `CPAD_ERR_WR_OR		   : write operation prohibited
    `CPAD_ERR_UNKNOWN	   : unknown error
  */

//static uint16_t cmd_set_password(void)
//{
//    uint8_t err_code = CPAD_ERR_NOERR;	
//		extern USHORT   cpad_usSRegHoldBuf[CMD_REG_SIZE];
//	 uint8_t pass_word[4];
//	
//		pass_word[0]= cpad_usSRegHoldBuf[1]>>8;
//		pass_word[1]= cpad_usSRegHoldBuf[1]&0xff;
//		pass_word[2]= cpad_usSRegHoldBuf[2]>>8;
//		pass_word[3]= cpad_usSRegHoldBuf[2]&0xff;
//		if(write_passward(&pass_word[0],cpad_usSRegHoldBuf[3],cpad_usSRegHoldBuf[4]) == 0)
//		{
//				err_code =	CPAD_ERR_DATA_OR;
//		}
//		return err_code;

//}

//static uint16_t Cmd_Set_Controlpassword(void)
//{
//    uint8_t err_code = CPAD_ERR_NOERR;	
//		extern USHORT   cpad_usSRegHoldBuf[CMD_REG_SIZE];
//	  uint16_t pBuff[10];
///*		
//	rt_kprintf("cpad_usSRegHoldBuf[1] = %x,cpad_usSRegHoldBuf[2] = %x,cpad_usSRegHoldBuf[3] = %x,cpad_usSRegHoldBuf[4] = %x,cpad_usSRegHoldBuf[5] = %x"
//	,cpad_usSRegHoldBuf[1],cpad_usSRegHoldBuf[2],cpad_usSRegHoldBuf[3],cpad_usSRegHoldBuf[4],cpad_usSRegHoldBuf[5]);	
//		rt_kprintf("cpad_usSRegHoldBuf[6] = %x,cpad_usSRegHoldBuf[7] = %x,cpad_usSRegHoldBuf[8] = %x,cpad_usSRegHoldBuf[9] = %x,cpad_usSRegHoldBuf[10] = %x"
//	,cpad_usSRegHoldBuf[6],cpad_usSRegHoldBuf[7],cpad_usSRegHoldBuf[8],cpad_usSRegHoldBuf[9],cpad_usSRegHoldBuf[10]);
//*/		
//	memcpy(pBuff,&cpad_usSRegHoldBuf[1],sizeof(pBuff));
//		if(Write_Controlpassward(&pBuff[0]) == 0)
//		{
//				err_code =	CPAD_ERR_DATA_OR;			
//		}
//		return err_code;

//}


/**
  * @brief  cpad command password compare
	* @param  none
  * @retval 
		`CPAD_ERR_NOERR			 : operation OK
		`CPAD_ERR_ADDR_OR	   : requested address out of range
    `CPAD_ERR_DATA_OR	   : requested data out of range
    `CPAD_ERR_PERM_OR	   : request permission denied
    `CPAD_ERR_WR_OR		   : write operation prohibited
    `CPAD_ERR_UNKNOWN	   : unknown error
  */

//static uint16_t cmd_password_compare(void)
//{
//    uint8_t err_code = CPAD_ERR_NOERR;	
//		extern USHORT   cpad_usSRegHoldBuf[CMD_REG_SIZE];
//		extern sys_reg_st	 g_sys;
//		if(passward_compare((uint8_t*)&cpad_usSRegHoldBuf[1],&g_sys.status.sys_work_mode.pass_word[0],4) == 0)
//		{
//				err_code =	CPAD_ERR_DATA_OR;
//		}
//		return err_code;

//}


///**
//  * @brief  cpad command password compare
//	* @param  none
//  * @retval 
//		`CPAD_ERR_NOERR			 : operation OK
//		`CPAD_ERR_ADDR_OR	   : requested address out of range
//    `CPAD_ERR_DATA_OR	   : requested data out of range
//    `CPAD_ERR_PERM_OR	   : request permission denied
//    `CPAD_ERR_WR_OR		   : write operation prohibited
//    `CPAD_ERR_UNKNOWN	   : unknown error
//  */

//static uint16_t cmd_password_pram_set(void)
//{
//    uint8_t err_code = CPAD_ERR_NOERR;	
//		extern sys_reg_st	 g_sys; 
//		extern USHORT   cpad_usSRegHoldBuf[CMD_REG_SIZE];
//		uint8_t pass_word[4];
//		pass_word[0]= cpad_usSRegHoldBuf[1]>>8;
//		pass_word[1]= cpad_usSRegHoldBuf[1]&0xff;
//		pass_word[2]= cpad_usSRegHoldBuf[2]>>8;
//		pass_word[3]= cpad_usSRegHoldBuf[2]&0xff;
//		if(passward_compare(&pass_word[0],&g_sys.status.sys_work_mode.pass_word[0],4) == 1)
//		{
//				err_code =	CPAD_ERR_DATA_OR;
//				if(cpad_work_mode(cpad_usSRegHoldBuf[3],cpad_usSRegHoldBuf[4]) == 0)
//				{
//						err_code =	CPAD_ERR_DATA_OR;
//				}
//		}
//		return err_code;

//}

//static uint16_t Cmd_Verify_Controlpassword(void)
//{
//    uint8_t err_code = CPAD_ERR_NOERR;	
//		extern sys_reg_st	 g_sys; 
//		extern USHORT   cpad_usSRegHoldBuf[CMD_REG_SIZE];
//		uint16_t pass_word;
//		pass_word= cpad_usSRegHoldBuf[1];

//		if(Passward_Verify(pass_word) == 1)
//		{
//				err_code =	CPAD_ERR_DATA_OR;
//		}
//		return err_code;

//}

 
/**
  * @brief  cpad command write reg operation
	* @param  none
  * @retval 
		`CPAD_ERR_NOERR			 : operation OK
		`CPAD_ERR_ADDR_OR	   : requested address out of range
    `CPAD_ERR_DATA_OR	   : requested data out of range
    `CPAD_ERR_PERM_OR	   : request permission denied
    `CPAD_ERR_WR_OR		   : write operation prohibited
    `CPAD_ERR_UNKNOWN	   : unknown error
  */
//uint16_t cpad_frame_resolve(void)
//{
//		uint8_t err_code;
//		uint8_t frame_cmd_type;
//		extern sys_reg_st	 g_sys; 
//	
//		err_code = CPAD_ERR_NOERR;
//		frame_cmd_type = cpad_reg_inst.rx_buf[FRAME_CMD_POS];

//		if(cpad_reg_inst.rx_tag == 1)
//		{
//				cpad_reg_inst.rx_cnt = 0;
//				cpad_reg_inst.rx_tag = 0;
//		}
//		else
//		{			
//		}		
//	
//		switch(frame_cmd_type)
//		{
//				case (CPAD_CMD_RD_REG):
//				{
//						err_code = cmd_rd_reg();
//						 //rt_kprintf("read reg\n");
//						break;
//						
//				}
//				
//				case (CPAD_CMD_WR_REG):
//				{	
//						err_code = cmd_wr_reg();
//						rt_kprintf("console: write reg.\n");
//						break;
//				}
//				case (CPAD_CMD_CONF_SAVE_OPTION):
//				{
//					
//						err_code = cmd_conf_save_option();
//						rt_kprintf("console: save current conf data.\n");						
//						break;
//				}		
//				case (CPAD_CMD_CONF_LOAD_OPTION):
//				{	
//					  err_code = cmd_conf_load_option();
//					  rt_kprintf("console: set load option.\n");
//						break;
//				}					
//				case (CPAD_CMD_SET_TIME):
//				{
//						err_code = cmd_set_time();
//						rt_kprintf("console: set time.\n");
//						break;
//				}
//				case (CPAD_CMD_REQ_TIME):
//				{
//						err_code = cmd_req_time();
//						rt_kprintf("console: req time.\n");
//						break;
//				}				
//				case (CPAD_CMD_REQ_PERM):
//				{
//						err_code = cmd_req_perm();
//						rt_kprintf("console: req perm.\n");
//						break;
//				}	
//				case (CPAD_CMD_CHANGE_PWD):
//				{
//						err_code = cmd_change_pwd();
//						rt_kprintf("console: pwd change.\n");
//						break;
//				}	
//				case (CPAD_CMD_READ_CURRENT_ALARAM):
//				{
//						err_code = cmd_req_alarm_statue();
//						rt_kprintf("console: CPAD_CMD_READ_CURRENT_ALARAM.\n");
//						break;
//				}	
//				case (CPAD_CMD_REDA_ALARM_RECORD):
//				{
//						err_code = cmd_req_alarm_record();
//						rt_kprintf("console: CPAD_CMD_REDA_ALARM_RECORD.\n");
//						break;
//				}	
//				case (CPAD_CMD_EVENT_RECORD):
//				{
//						err_code = cmd_req_event_record();
//						rt_kprintf("console: CPAD_CMD_EVENT_RECORD.\n");
//						break;
//				}	
//				case (CPAD_CMD_CLEAR_RECORD):
//				{
//						err_code = cmd_req_event_clear();	
//						rt_kprintf("console: CPAD_CMD_CLEAR_RECORD.\n");
//						break;
//				}	

//				case (CPAD_CMD_READ_TEM_HUM):
//				{
//						err_code = cmd_req_tem_hum_record();
//						rt_kprintf("console: CPAD_CMD_READ_TEM_HUM.\n");
//						break;
//				}	
//				
//				case (CPAD_CMD_CLEAR_RUN_TIME):
//				{
//					  err_code = cmd_clear_dev_run_time();
//						rt_kprintf("console: CPAD_CMD_CLEAR_RUN_TIME.\n");
//						break;
//				}	
//				
//				case (CPAD_CMD_CLEAR_ALARM):
//				{
//						err_code = cmd_clear_alarm();
//						rt_kprintf("console: cmd_clear_ALARM.\n");
//						break;
//				}	
//				case (CPAD_CMD_DEFAULT_PRAM):
//				{
//						err_code = cmd_default_pram_func();
//						rt_kprintf("console: CPAD_CMD_DEFAULT_PRAM.\n");
//						break;
//				}
//				case (CPAD_CMD_PASSWORD_SET):
//				{
//						err_code = cmd_set_password();
//						rt_kprintf("console: CPAD_CMD_PASSWORD_SET.\n");
//						break;
//				}
//			
//				case (CPAD_CMD_PASSWORD_PRAM_SET):
//				{
//						err_code = cmd_password_pram_set();
//						rt_kprintf("console: CPAD_CMD_PASSWORD_PRAM_SET.\n");
//						break;
//				}
//				default:
//				{
//						err_code = CPAD_ERR_UNKNOWN;
//						break;
//				}
//		}
//		return err_code;
//}

/*******************************************************************************
 * Function Name  : UART5_IRQHandler
 * Description    : This function handles UART5 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
//void UART5_IRQHandler(void)
//{
//		uint8_t rx_data,tx_data;
////		rt_interrupt_enter();
//		//溢出错误
//		if (USART_GetFlagStatus(UART5, USART_FLAG_ORE) == SET)
//		{
////				USART_ClearFlag(UART5, USART_FLAG_ORE);
//				rx_data = USART_ReceiveData(UART5);
////				if(is_fifo8_full(&cpad_rx_fifo) == 1)
////				{
////						fifo8_reset(&cpad_rx_fifo);
////				}
////				else
////				{
////						fifo8_push(&cpad_rx_fifo,&rx_data);
////				}				
//		}
//		//接收中断
//	  if (USART_GetITStatus(UART5, USART_IT_RXNE) == SET)
//		{
//				USART_ClearITPendingBit( UART5, USART_IT_RXNE);
//				rx_data = USART_ReceiveData(UART5);
//				if(cpad_reg_inst.rx_tag == 1)
//				{
//						return;
//				}
//				switch (cpad_reg_inst.cpad_fsm_cstate)
//				{
//						case (CPAD_FRAME_FSM_SYNC1):
//						{
//								cpad_reg_inst.rx_cnt = 0;
//								if(rx_data == CPAD_FRAME_TAG_RX_SYNC1)
//								{
//										cpad_reg_inst.rx_buf[cpad_reg_inst.rx_cnt] = rx_data;
//										cpad_reg_inst.rx_cnt++;
//										cpad_reg_inst.rx_tag = 0;
//										cpad_reg_inst.cpad_fsm_cstate = CPAD_FRAME_FSM_SYNC2;
//								}
//								else
//								{
//										cpad_reg_inst.rx_cnt = 0;
//										cpad_reg_inst.rx_tag = 0;
//										cpad_reg_inst.rx_buf[cpad_reg_inst.rx_cnt] = 0;										
//										cpad_reg_inst.cpad_fsm_cstate = CPAD_FRAME_FSM_SYNC1;
//								}
//								break;
//						}
//						case (CPAD_FRAME_FSM_SYNC2):
//						{
//								if(rx_data == CPAD_FRAME_TAG_RX_SYNC2)
//								{
//										cpad_reg_inst.rx_buf[cpad_reg_inst.rx_cnt] = rx_data;
//										cpad_reg_inst.rx_cnt++;
//										cpad_reg_inst.rx_tag = 0;
//										cpad_reg_inst.cpad_fsm_cstate = CPAD_FRAME_FSM_CMD;
//								}
//								else
//								{
//										cpad_reg_inst.rx_cnt = 0;
//										cpad_reg_inst.rx_tag = 0;
//										cpad_reg_inst.rx_buf[cpad_reg_inst.rx_cnt] = 0;
//										cpad_reg_inst.cpad_fsm_cstate = CPAD_FRAME_FSM_SYNC1;
//								}				
//								break;
//						}					
//						case (CPAD_FRAME_FSM_CMD):
//						{
//								if(((rx_data == CPAD_CMD_RD_REG)|| 
//									(rx_data == CPAD_CMD_WR_REG)|| 
//									(rx_data == CPAD_CMD_SET_TIME)|| 
//									(rx_data == CPAD_CMD_REQ_TIME)|| 
//									(rx_data == CPAD_CMD_REQ_PERM)|| 
//									(rx_data == CPAD_CMD_CHANGE_PWD)|| 
//									(rx_data == CPAD_CMD_CONF_LOAD_OPTION)||
//									(rx_data == CPAD_CMD_CONF_SAVE_OPTION)||

//									(rx_data == CPAD_CMD_READ_CURRENT_ALARAM)||
//									(rx_data == CPAD_CMD_REDA_ALARM_RECORD)||
//									(rx_data == CPAD_CMD_EVENT_RECORD)||
//									(rx_data == CPAD_CMD_CLEAR_RECORD)||
//									(rx_data == CPAD_CMD_READ_TEM_HUM)||
//									(rx_data == CPAD_CMD_CLEAR_ALARM) ||
//									(rx_data == CPAD_CMD_CLEAR_RUN_TIME)||
//									(rx_data == CPAD_CMD_PASSWORD_SET)||
//									(rx_data == CPAD_CMD_PASSWORD_COMP)||
//									(rx_data == CPAD_CMD_PASSWORD_PRAM_SET)||
//									(rx_data == CPAD_CMD_DEFAULT_PRAM)))
//								{
//										cpad_reg_inst.rx_buf[cpad_reg_inst.rx_cnt] = rx_data;
//										cpad_reg_inst.rx_cnt++;
//										cpad_reg_inst.rx_tag = 0;
//										cpad_reg_inst.cpad_fsm_cstate = CPAD_FRAME_FSM_LEN;
//								}
//								else
//								{
//										cpad_reg_inst.rx_cnt = 0;
//										cpad_reg_inst.rx_tag = 0;
//										cpad_reg_inst.rx_buf[cpad_reg_inst.rx_cnt] = 0;										
//										cpad_reg_inst.cpad_fsm_cstate = CPAD_FRAME_FSM_SYNC1;
//								}
//								break;
//						}		
//						case (CPAD_FRAME_FSM_LEN):
//						{								
//								if(cpad_reg_inst.rtx_timeout < CPAD_FSM_TIMEOUT)
//								{
//										cpad_reg_inst.rx_buf[cpad_reg_inst.rx_cnt] = rx_data;
//										cpad_reg_inst.rx_cnt++;
//										cpad_reg_inst.rx_tag = 0;
//										cpad_reg_inst.cpad_fsm_cstate = CPAD_FRAME_FSM_DATA;
//								}
//								else
//								{
//										cpad_reg_inst.rx_cnt = 0;
//										cpad_reg_inst.rx_tag = 0;
//										cpad_reg_inst.rx_buf[cpad_reg_inst.rx_cnt] = 0;										
//										cpad_reg_inst.cpad_fsm_cstate = CPAD_FRAME_FSM_SYNC1;
//								}				
//								break;
//						}		
//						case (CPAD_FRAME_FSM_DATA):
//						{
//								if(cpad_reg_inst.rx_buf[FRAME_LEN_POS] > (cpad_reg_inst.rx_cnt - FRAME_LEN_POS-1))
//								{
//										cpad_reg_inst.rx_buf[cpad_reg_inst.rx_cnt] = rx_data;
//										cpad_reg_inst.rx_cnt++;
//										cpad_reg_inst.rx_tag = 0;
//										cpad_reg_inst.cpad_fsm_cstate = CPAD_FRAME_FSM_DATA;
//								}
//								else
//								{
//										cpad_reg_inst.rx_buf[cpad_reg_inst.rx_cnt] = rx_data;
//										cpad_reg_inst.rx_cnt ++;										
//										if(frame_checksum() == 1)
//										{
//												cpad_reg_inst.rx_tag = 1;
//												cpad_reg_inst.rx_cnt = cpad_reg_inst.rx_cnt;
//												cpad_reg_inst.rx_buf[cpad_reg_inst.rx_cnt] = 0;
////												rt_kprintf("chk ok\n");
//										}
//										else
//										{
//												cpad_reg_inst.rx_tag = 0;
//												cpad_reg_inst.rx_cnt = 0;
//												cpad_reg_inst.rx_buf[cpad_reg_inst.rx_cnt] = 0;
////												rt_kprintf("chk error\n");
//										}
//										cpad_reg_inst.cpad_fsm_cstate = CPAD_FRAME_FSM_SYNC1;
//								}												
//								break;
//						}
//						default:
//						{
//								cpad_reg_inst.rx_cnt = 0;
//								cpad_reg_inst.rx_tag = 0;
//								cpad_reg_inst.rx_buf[cpad_reg_inst.rx_cnt] = 0;								
//								cpad_reg_inst.cpad_fsm_cstate = CPAD_FRAME_FSM_SYNC1;	
//								break;
//						}	
//				}				
//		}
//		//发送中断
//	  if (USART_GetITStatus(UART5, USART_IT_TC) == SET)
//		{
//				USART_ClearFlag(UART5, USART_FLAG_TC);
//				if(is_fifo8_empty(&cpad_tx_fifo) == 0)
//				{
//						fifo8_pop(&cpad_tx_fifo,&tx_data);
//						USART_SendData(UART5, tx_data);
//				}				
//				else
//				{
//						USART_ITConfig(UART5, USART_IT_TC, DISABLE);
//						CPAD_RS485_RCV_MODE;
//				}
//		}
//		
////		rt_interrupt_leave();
//}

static uint16_t obcmd_clear_alarm(void)
{
		extern USHORT   cpad_usSRegHoldBuf[CMD_REG_SIZE];
		
		extern sys_reg_st	 g_sys; 
		
		if(clear_alarm() != 1)
		{
				
				return 0;
		}
		else
		{
				//sys_set_remap_status(WORK_MODE_STS_REG_NO,ALARM_BEEP_BPOS,0);
				return 1;
		}
}

static uint16_t obcmd_set_time(void)
{
		extern USHORT   cpad_usSRegHoldBuf[CMD_REG_SIZE];
		time_t now;
		rt_device_t device;	

		now = cpad_usSRegHoldBuf[1];
		now = (now<<16)|cpad_usSRegHoldBuf[2];
	
		device = rt_device_find("rtc");
		
		if (device != RT_NULL)
		{
				rt_rtc_control(device, RT_DEVICE_CTRL_RTC_SET_TIME, &now);
				return 1;
		}
		else
		{
				return 0;
		}
}

static uint16_t obcmd_req_time(void)
{
		extern USHORT   cpad_usSRegHoldBuf[CMD_REG_SIZE];
		time_t now;

		time(&now);			

		cpad_usSRegHoldBuf[1] = (uint16_t)((now>>16)&0x0000ffff);
		cpad_usSRegHoldBuf[2] = (uint16_t)(now&0x0000ffff);
	
		return 1;
}

static uint16_t obcmd_clear_run_time(void)
{
			extern USHORT   cpad_usSRegHoldBuf[CMD_REG_SIZE];
	
			extern sys_reg_st	 g_sys; 

			if(reset_runtime(cpad_usSRegHoldBuf[1]) != 1)
			{
					return 0;
			}
			else
			{
					return 1;
			}
}

static uint16_t obcmd_load_fact(void)
{
			extern USHORT   cpad_usSRegHoldBuf[CMD_REG_SIZE];
	
			extern sys_reg_st	 g_sys; 

			if(load_factory_pram() != 1)
			{
					return 0;
			}
			else
			{
					return 1;
			}
}

//保存配置参数
static uint16_t obcmd_save_conf(void)
{
			extern USHORT   cpad_usSRegHoldBuf[CMD_REG_SIZE];
	
			extern sys_reg_st	 g_sys; 

			if(save_conf_reg(cpad_usSRegHoldBuf[1]) != 0)
			{
					return 0;
			}
			else
			{
					rt_thread_delay(1000);
					NVIC_SystemReset();//主板重启
					return 1;
			}
}



static uint16_t obcmd_load_conf(void)
{
		extern USHORT   cpad_usSRegHoldBuf[CMD_REG_SIZE];
		
		extern sys_reg_st	 g_sys; 
		
		if(set_load_flag(cpad_usSRegHoldBuf[1]) != 1)
		{
				return 0;
		}
		else
		{
				if(cpad_usSRegHoldBuf[1] !=0)
				{
							NVIC_SystemReset();
				}
				return 1;
		}
}

static uint16_t obcmd_clear_alarm_beep(void)
{
		 
		sys_set_remap_status(WORK_MODE_STS_REG_NO,ALARM_BEEP_BPOS,0);

		return 1;
}

//modbus cpad protocal resolve
uint16_t cpad_ob_resolve(void)
{
		extern USHORT   cpad_usSRegHoldBuf[CMD_REG_SIZE];
		uint8_t err_code;
		uint16_t cmd_type;
	
		cmd_type = cpad_usSRegHoldBuf[0];
		if(cmd_type != 0) 
		{
				rt_kprintf("cmd_type is %x\n",cmd_type);
		}
	
		err_code = 0;
	
		if(cmd_type != 0)
		{
				switch (cmd_type)
				{
						case(CPAD_CMD_CLEAR_ALARM):
						{
								err_code = obcmd_clear_alarm();
								break;
						}
						case(CPAD_CMD_SET_TIME):
						{
								err_code = obcmd_set_time();
								break;
						}
						case(CPAD_CMD_REQ_TIME):
						{
								err_code = obcmd_req_time();
								break;
						}
						case(CPAD_CMD_CLEAR_RUN_TIME):
						{
								err_code = obcmd_clear_run_time();
								break;
						}		
						case(CPAD_CMD_CLEAR_RECORD):
						{
								err_code = 1;
								break;
						}		
						case(CPAD_CMD_DEFAULT_PRAM)://恢复默认参数
						{
								err_code = obcmd_load_fact();
								break;
						}		
						case(CPAD_CMD_CONF_SAVE_OPTION):
						{
								err_code = obcmd_save_conf();
								break;
						}
						case(CPAD_CMD_CONF_LOAD_OPTION)://恢复原始参数
						{
								err_code = obcmd_load_conf();
								break;
						}				
						case(CPAD_CMD_CLEAR_ALARM_BEEP):
						{
								err_code = obcmd_clear_alarm_beep();
								break;
						}	
//						case(CPAD_CMD_PASSWORD_SET):
//						{
//								err_code = cmd_set_password();
//								break;
//						}	
//						case(CPAD_CMD_PASSWORD_PRAM_SET):
//						{
//								err_code = cmd_password_pram_set();
//								break;
//						}	
//						case(CPAD_CMD_CONTROLPASSWORD_SET)://管控密码设置
//						{
//								err_code = Cmd_Set_Controlpassword();
//								break;
//						}							
//						case(CPAD_CMD_CONTROLPASSWORD_VERIFY)://管控密码认证
//						{
//								err_code = Cmd_Verify_Controlpassword();
//								break;
//						}											
						
						default:
						{
								err_code = 1;
								break;
						}
				}
				if(err_code == 0)
				{
						cpad_usSRegHoldBuf[0] = 0;
				}
				else
				{
						cpad_usSRegHoldBuf[0] = 0xffff;
				}				
		}
		cpad_usSRegHoldBuf[0] = 0;		
		return 1;
}


static void show_cpad_info(void)
{
		rt_kprintf("Cpad rx_tag: %x, rtx_timeout: %d\n",cpad_reg_inst.rx_tag,cpad_reg_inst.rtx_timeout);
}

FINSH_FUNCTION_EXPORT(show_cpad_info, show cpad information.);
