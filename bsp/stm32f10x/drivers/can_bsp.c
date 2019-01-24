// #include <rtthread.h>
// #include <components.h>
// #include "stm32f10x_it.h"
// #include "stm32f10x_can.h"
// #include "kits/fifo.h"
// #include "sys_def.h"
// #include "can_bsp.h"
// #include "sys_conf.h"

// enum
// {
//     //TEAM_BAUDRATE_10 = 0,
//     TEAM_BAUDRATE_20 = 0,
//     TEAM_BAUDRATE_50,
//     TEAM_BAUDRATE_100,
//     TEAM_BAUDRATE_125,
//     TEAM_BAUDRATE_250,
//     TEAM_BAUDRATE_500,
// };

// //CAN rx message buffer size definition
// #define CAN_RXBUF_SIZE 32
// //CAN user protocal maximal frame length definition
// #define MAX_CAN_FRAME_SIZE 256
// //CAN GPIO pin definiton
// #define GPIO_CAN GPIOB
// #define GPIO_Pin_CAN_RX GPIO_Pin_8
// #define GPIO_Pin_CAN_TX GPIO_Pin_9
// //CAN remapping
// #define GPIO_Remapping_CAN GPIO_Remap1_CAN1

// //can rx fifo structure;
// fifo8_cb_td can_rx_fifo;
// //can tx semaphore used in synchronization of CAN tx function and CAN tx ISR
// static rt_sem_t can_tx_sem = RT_NULL;

// static void can_nvic_config(void)
// {
//     NVIC_InitTypeDef NVIC_InitStructure;

//     //can rx isr initialization
//     NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//     NVIC_Init(&NVIC_InitStructure);

//     //can tx isr initialization
//     NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//     NVIC_Init(&NVIC_InitStructure);
// }

// // can bus struct initialization
// static void can_struct_init(uint16_t can_baudrate)
// {
//     CAN_InitTypeDef CAN_InitStructure;
//     CAN_FilterInitTypeDef CAN_FilterInitStructure;
//     GPIO_InitTypeDef GPIO_InitStructure;

//     /* GPIO clock enable */
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

//     /* Configure CAN pin: RX */
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN_RX;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//     GPIO_Init(GPIO_CAN, &GPIO_InitStructure);

//     /* Configure CAN pin: TX */
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN_TX;
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_Init(GPIO_CAN, &GPIO_InitStructure);

//     GPIO_PinRemapConfig(GPIO_Remapping_CAN, ENABLE);

//     /* CAN register init */
//     CAN_DeInit(CAN1);

//     CAN_StructInit(&CAN_InitStructure);

//     /* CAN cell init */
//     CAN_InitStructure.CAN_TTCM = DISABLE;
//     CAN_InitStructure.CAN_ABOM = ENABLE;
//     CAN_InitStructure.CAN_AWUM = DISABLE;
//     CAN_InitStructure.CAN_NART = DISABLE;
//     CAN_InitStructure.CAN_RFLM = DISABLE;
//     CAN_InitStructure.CAN_TXFP = ENABLE; //DISABLE;
//     CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;

//     /* Baudrate = 500 Kbps */
//     CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
//     CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
//     CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;

//     //Set CAN baudrate
//     switch (can_baudrate)
//     {
//     case TEAM_BAUDRATE_500:
//         CAN_InitStructure.CAN_Prescaler = 16;
//         break;
//     case TEAM_BAUDRATE_250:
//         CAN_InitStructure.CAN_Prescaler = 32;
//         break;
//     case TEAM_BAUDRATE_125:
//         CAN_InitStructure.CAN_Prescaler = 64;
//         break;
//     case TEAM_BAUDRATE_100:
//         CAN_InitStructure.CAN_Prescaler = 80;
//         break; //
//     case TEAM_BAUDRATE_50:
//         CAN_InitStructure.CAN_Prescaler = 160;
//         break;
//     case TEAM_BAUDRATE_20:
//         CAN_InitStructure.CAN_Prescaler = 400;
//         break;
//         //				case TEAM_BAUDRATE_10:		CAN_InitStructure.CAN_Prescaler=800;break;
//     default:
//         CAN_InitStructure.CAN_Prescaler = 80;
//         break;
//     }
//     //baudrate:64M/2/(2+6+8)/=0.1 ?100K
//     //		rt_kprintf("can_baudrate = %d\n",can_baudrate);
//     CAN_Init(CAN1, &CAN_InitStructure);

//     CAN_FilterInitStructure.CAN_FilterNumber = 1;
//     CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
//     CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
//     CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
//     CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
//     CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
//     CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
//     CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
//     CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
//     CAN_FilterInit(&CAN_FilterInitStructure);

//     /* CAN FIFO0 message pending interrupt enable */
//     CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
// }

// void can_port_baudrate(void)
// {
//     extern sys_reg_st g_sys;

//     rt_sem_take(can_tx_sem, 200); //wait for signal for can tx ISR
//     can_struct_init(g_sys.config.team.baudrate);
// }
// //can bus initialization general
// void drv_can_init(void)
// {
//     extern sys_reg_st g_sys;

//     can_nvic_config();
//     can_struct_init(g_sys.config.team.baudrate);
//     fifo8_init(&can_rx_fifo, sizeof(CanRxMsg), CAN_RXBUF_SIZE);
//     can_tx_sem = rt_sem_create("can_tx_sem", 0, RT_IPC_FLAG_FIFO);
// }

// // can bus send data function
// int32_t can_send(uint8_t sa, uint8_t da, const uint8_t *data, uint16_t length, uint8_t prio, uint8_t raf)
// {
//     CanTxMsg TxMessage;
//     uint8 TransmitMailbox;
//     //	uint32 temp;
//     uint32 i, res_len, res_count;
//     //validate tx data length
//     if (length > MAX_CAN_FRAME_SIZE)
//     {
//         rt_kprintf("Can frame length exceeded!\n");
//         return 0;
//     }
//     i = 0;
//     res_len = length;
//     TxMessage.ExtId = 0;
//     TxMessage.ExtId = prio << 27 | raf << 22 | sa << 14 | da << 6; //set can user protocal priority, req/ack flag, destination addr and source addr segment
//     TxMessage.StdId = 0;
//     TxMessage.IDE = CAN_ID_EXT;
//     TxMessage.RTR = CAN_RTR_DATA;

//     //if tx data length is over 8 bytes, fix DLC to 8 and repeatedly send data while residual length is over 8
//     while (res_len > 8)
//     {
//         TxMessage.DLC = 8; //when in this
//         for (i = 0; i < 8; i++)
//         {
//             TxMessage.Data[i] = *(data + (length - res_len));
//             res_len--;
//         }
//         TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);
//         TxMessage.ExtId++;           //increase slit frame number count
//         if (TransmitMailbox == 0x04) //if TransmitMailbox used up, abort transmission
//         {
//             return 0;
//         }
//         CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE); //enable can tx interrupt
//         rt_sem_take(can_tx_sem, 10);            //wait for signal for can tx ISR
//     }

//     //if tx data length is no more than 8 and not none, reset DLC to current length, and transmmit data
//     if (res_len > 0)
//     {
//         res_count = res_len;
//         TxMessage.ExtId |= CANID_EOSF_BITS; //when programe run to this point, it must be the end of frame. thus add the EOSF flag to the frame head
//         TxMessage.DLC = res_len;            // set DLC to current data length
//         for (i = 0; i < res_count; i++)
//         {
//             TxMessage.Data[i] = *(data + (length - res_len));
//             res_len--;
//         }
//         TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);
//         if (TransmitMailbox == 0x04) //if TransmitMailbox used up, abort transmission
//         {
//             return 0;
//         }
//         CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE); //enable can tx interrupt
//         rt_sem_take(can_tx_sem, 10);            //wait for signal for can tx ISR
//     }
//     return 1;
// }

// // can bus send data function
// int32_t can_send_singleframe(uint8_t sa, uint8_t da, uint8_t type, const uint8_t *data, uint16_t length, uint8_t prio, uint8_t raf)
// {
//     CanTxMsg TxMessage;
//     uint8_t TransmitMailbox;
//     uint8_t i, res_len;
//     //check data length
//     if (length > 8)
//     {
//         rt_kprintf("CAN Frame length exceeded!\n");
//         return 0;
//     }

//     i = 0;
//     res_len = length;
//     TxMessage.StdId = 0;
//     TxMessage.ExtId = 0;
//     TxMessage.ExtId = prio << 27 | raf << 22 | sa << 14 | da << 6 | type;
//     //    TxMessage.ExtId   |= CANID_EOSF_BITS; //single frame，the first frame is the end frame
//     TxMessage.IDE = CAN_ID_EXT;
//     TxMessage.RTR = CAN_RTR_DATA;
//     TxMessage.DLC = length;
//     for (i = 0; i < res_len; i++)
//     {
//         TxMessage.Data[i] = *(data + i);
//     }

//     TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);

//     if (TransmitMailbox == 0x04) //if TransmitMailbox used up, abort transmission
//     {
//         rt_kprintf("can_send_singleframe: CAN_FLAG_BOF = %d\n", CAN_GetFlagStatus(CAN1, CAN_FLAG_BOF));
//         //				rt_kprintf("TransmitMailbox used up, abort transmission  erro = %d,canflag=%d\n",CAN_GetLSBTransmitErrorCounter(CAN1),CAN_GetFlagStatus(CAN1,CAN_FLAG_BOF));
//         //when send buffer > 3; physical offline
//         return 0;
//     }
//     else
//     {
//         ;
//     }
//     CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE); //enable can tx interrupt
//     rt_sem_take(can_tx_sem, 10);            //wait for signal for can tx ISR

//     return 1;
// }

// //can bus initialization general
// void can_snd_test(uint8 da, uint8 sa, uint16 length)
// {
//     uint8_t data_buf[32];
//     uint8_t i;
//     static uint8_t j;
//     for (i = 0; i < 32; i++)
//     {
//         data_buf[i] = i + j;
//     }
//     //		can_send(da,sa,data_buf,length,0,0);
//     j++;
//     can_send_singleframe(sa, da, 0, data_buf, length, 0, 0);
// }

// //CAN RECIEVE ISR
// void USB_LP_CAN1_RX0_IRQHandler(void)
// {
//     CanRxMsg RxMessage;
//     uint8 can_fifo_len, i;
//     can_fifo_len = 0;

//     can_fifo_len = CAN_MessagePending(CAN1, CAN_FIFO0);
//     for (i = 0; i < can_fifo_len; i++)
//     {
//         CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
//         fifo8_push(&can_rx_fifo, (uint8 *)&RxMessage);
//     }
// }

// //CAN TRANSMIT ISR
// void USB_HP_CAN1_TX_IRQHandler(void)
// {
//     CAN_ClearITPendingBit(CAN1, CAN_IT_TME);
//     CAN_ITConfig(CAN1, CAN_IT_TME, DISABLE);
//     rt_sem_release(can_tx_sem);
// }

// FINSH_FUNCTION_EXPORT(drv_can_init, set can baudrate);
// FINSH_FUNCTION_EXPORT(can_snd_test, can_snd testcase);
// //
