#ifndef _MOTER_H
#define _MOTER_H

#include "stm32f10x.h"
#include "sys_conf.h"
#define MAX_EXCHOLD_CNT 3u

#define EEV1_MASKBIT 0xf69f
#define EEV2_MASKBIT 0xff87

#define RCC_EEV1 RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE
#define GPIO_PORT_EEV1_A GPIOE
#define GPIO_PIN_EEV1_A GPIO_Pin_2
#define GPIO_PORT_EEV1_B GPIOE
#define GPIO_PIN_EEV1_B GPIO_Pin_2
#define GPIO_PORT_EEV1_C GPIOB
#define GPIO_PIN_EEV1_C GPIO_Pin_9
#define GPIO_PORT_EEV1_D GPIOB
#define GPIO_PIN_EEV1_D GPIO_Pin_8

//#define RCC_EEV2        	RCC_AHBPeriph_GPIOB
//#define GPIO_PORT_EEV2  	GPIOB
//#define GPIO_PIN_EEV2_A		GPIO_Pin_6
//#define GPIO_PIN_EEV2_B		GPIO_Pin_5
//#define GPIO_PIN_EEV2_C		GPIO_Pin_4
//#define GPIO_PIN_EEV2_D		GPIO_Pin_3

#define EEV1_TIMER TIM14
#define EEV1_IRQHandler TIM14_IRQHandler
//#define EEV2_TIMER			TIM15
//#define EEV2_IRQHandler		TIM15_IRQHandler

enum
{
    DIRNULL = 0, //????
    DIROPEN,     //??
    DIRCLOSE     //??
};

enum
{
    STOP = 0,
    RUN
};

enum
{
    stepA = 0, //????
    stepAB,
    stepB,
    stepBC,
    stepC,
    stepCD,
    stepD,
    stepDA,
    stepMAX
};

#define STEP_OFFSET 2
//enum
//{
//	stepA  = 0,						//????
////	stepAB,
//	stepB ,
////	stepBC,
//	stepC ,
////	stepCD,
//	stepD ,
////	stepDA,
//	stepMAX
//};
#pragma pack(1)
struct eevHWcfg //eev????
{
    uint32_t eevRcc;
    GPIO_TypeDef *eevPort_A;
    GPIO_TypeDef *eevPort_B;
    GPIO_TypeDef *eevPort_C;
    GPIO_TypeDef *eevPort_D;
    uint16_t eevApin;
    uint16_t eevBpin;
    uint16_t eevCpin;
    uint16_t eevDpin;
    uint16_t eev_maskbit;
    uint16_t eev_table[stepMAX];
};

struct eevMovSts //???????
{
    uint8_t last_dir;    //???????
    uint8_t now_dir;     //??
    uint8_t last_status; //??????(??????????)
    uint8_t now_status;  //??
    uint8_t beatIndex;   //?????(????)
    uint16_t curPluse;   //??????
    uint16_t movPluse;   //???????
    uint16_t finalPluse; //??????,???????
    uint8_t exc_holdCnt;
};

typedef struct eev_pack
{
    struct eevHWcfg eevHardWareCfg;
    struct eevMovSts eevMoveInfo;
} EEV_Pack;
#pragma pack()

enum //控制状态
{
    STATUS_INIT = 0,   //初始化
    STATUS_CLOSE,      //关闭
    STATUS_OFF,        //待机
    STATUS_POS,        //定位
    STATUS_WAIT,       //等待
    STATUS_PRE_ADJUST, //预调节
    STATUS_ON,         //运行
    STATUS_HOLD
};

//采集取样
enum
{
    GET_ZERO = 0,
    GET_ONE,
    GET_TWO,
    GET_AVG
};

enum
{
    EEV1 = 0,
    //	EEV2,
    MAX_EEVNUM
};

enum
{
    VALVE_OFF = 0,
    VALVE_ON
};

enum
{
    NO_ACTION = 0,
    FORCE_CLOSE_VALVE,   //强制关闭阀门
    VALVE_OPEN_FIXED_POS //阀开启至固定位置
};                       //@2017-08-21

extern EEV_Pack eevPack[MAX_EEVNUM];
extern void eev_init(void);
extern void eev1_timerInit(uint8_t ppsVal);
extern void eev2_timerInit(uint8_t ppsVal);
extern uint8_t setEevMovInfo(uint16_t dir, uint16_t steps, uint16_t status, uint8_t index);

extern uint16_t getBeatIndex(uint8_t index);
extern uint16_t getFinalPluse(uint8_t index);
extern uint16_t getOpenValveDegree(uint8_t index);
extern uint16_t getPluseNumofOpenValve(uint16_t valve);
extern uint8_t GetEevMovInfo(uint8_t index);
#endif /* __MOTER_H */
