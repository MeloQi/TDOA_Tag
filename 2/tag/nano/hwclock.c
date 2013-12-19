/********************************Copyright (c)**********************************
**                			
**                          				
**-File Info--------------------------------------------------------------------
** File name:			hwclock.c
** Last Version:		V1.0
** Descriptions:		硬件时间相关
**------------------------------------------------------------------------------
** Created by:	 	QI
** Created date:     	2013-08-12
** Version:	       	V1.0
** Descriptions:    	
*******************************************************************************/

#include "iar.h"
#include "hal.h"
//#include "phy-library.h"
//#include "hal/micro/cortexm3/micro-common.h"

#include "config.h"
#include "ntrxtypes.h"
#include "hwclock.h"
#include "nnspi.h"
#include "ntrxutil.h"
#include "ntrxutil.h"

#ifndef __OPTIMIZE__
#define __OPTIMIZE__ 0
#endif

#define	MILLISECONDS_PER_TICK	10 /* 10ms定时器 */
#define	STATE_HIGH	5
#define	STATE_LOW	-5

MyBoolT	key_flags[NKEYS] = {
	FALSE, FALSE, FALSE, FALSE
};

static	MyDword32T	jiffies = 0;
extern  MyDword32T tiRecal;
/**
 * hwdelay:
 *
 * 短延时.
 *
 * .
 */
void	hwdelay(MyDword32T t)
{
  unsigned long lc;
  unsigned char timercount;

  for(lc = 0;lc < t;lc++)
  {
    for(timercount = 0;timercount < 5;timercount++);
  }
}


/**
 * hwclockRestart:
 *
 * 时钟初始化.
 *
 * .
 */
void hwclockRestart (MyDword32T start)
{
	
	TIM2_CR1 = 0;
	TIM2_PSC = 5;						//12MHz/2^5 = 12MHz/32 = 375kHz
	TIM2_ARR = 3750;					//10ms
	TIM2_CNT = 0;						//清除计数器
	TIM2_EGR = 1;						//产生软件更新事件，以装载新的分频值 
	ATOMIC(
	INT_TIM2CFG = INT_TIMUIF;			        //使能timer2 的更新事件中断
	INT_CFGSET = INT_TIM2;				        //使能NVIC 中的timer2 的全局中断 
	TIM2_CR1 |= TIM_CEN;				        //开始计数
	)
	jiffies = start;
}

/**
 * hwclock_init:
 *
 * clock 时钟初始化接口.
 *
 *.
 */
void hwclock_init(void)
{
	hwclockRestart (0);
}

/**
 * hwclock:
 *
 * 时钟节拍.
 *
 * .
 */
MyDword32T	hwclock(void)
{
	return	jiffies;
}

MyWord16T WDog_Cnt = 0;
extern MyByte8T WDog_Flag;
extern uint32_t  sys_time; 										//系统时间
extern uint16_t  sys_ms;								
void halInternalResetWatchDog(void);
/**
 * halTimer2Isr
 *
 * TIMER2 定时器中断:
 *
 *
 */
void halTimer2Isr(void)
{     
    jiffies += MILLISECONDS_PER_TICK; 	                        //用于校准
    if( jiffies > 0xFFFF0000 ){
      jiffies = 0;
      tiRecal = 0;
    }
	
	sys_ms += MILLISECONDS_PER_TICK;							//系统时间
	if(sys_ms >= 1000){
		sys_ms = 0;
		sys_time++;
	}
    WDog_Cnt++;
    if((WDog_Cnt >= 120)&&WDog_Flag){                           //1.2s喂狗一次
      WDog_Flag = 0;                                            //喂狗标志
      WDog_Cnt = 0;
      halInternalResetWatchDog();                               //喂狗
    }
    
    INT_TIM2FLAG = 0xFFFFFFFF;                                  //清除中断
   
}



