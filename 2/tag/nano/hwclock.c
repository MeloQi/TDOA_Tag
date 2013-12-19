/********************************Copyright (c)**********************************
**                			
**                          				
**-File Info--------------------------------------------------------------------
** File name:			hwclock.c
** Last Version:		V1.0
** Descriptions:		Ӳ��ʱ�����
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

#define	MILLISECONDS_PER_TICK	10 /* 10ms��ʱ�� */
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
 * ����ʱ.
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
 * ʱ�ӳ�ʼ��.
 *
 * .
 */
void hwclockRestart (MyDword32T start)
{
	
	TIM2_CR1 = 0;
	TIM2_PSC = 5;						//12MHz/2^5 = 12MHz/32 = 375kHz
	TIM2_ARR = 3750;					//10ms
	TIM2_CNT = 0;						//���������
	TIM2_EGR = 1;						//������������¼�����װ���µķ�Ƶֵ 
	ATOMIC(
	INT_TIM2CFG = INT_TIMUIF;			        //ʹ��timer2 �ĸ����¼��ж�
	INT_CFGSET = INT_TIM2;				        //ʹ��NVIC �е�timer2 ��ȫ���ж� 
	TIM2_CR1 |= TIM_CEN;				        //��ʼ����
	)
	jiffies = start;
}

/**
 * hwclock_init:
 *
 * clock ʱ�ӳ�ʼ���ӿ�.
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
 * ʱ�ӽ���.
 *
 * .
 */
MyDword32T	hwclock(void)
{
	return	jiffies;
}

MyWord16T WDog_Cnt = 0;
extern MyByte8T WDog_Flag;
extern uint32_t  sys_time; 										//ϵͳʱ��
extern uint16_t  sys_ms;								
void halInternalResetWatchDog(void);
/**
 * halTimer2Isr
 *
 * TIMER2 ��ʱ���ж�:
 *
 *
 */
void halTimer2Isr(void)
{     
    jiffies += MILLISECONDS_PER_TICK; 	                        //����У׼
    if( jiffies > 0xFFFF0000 ){
      jiffies = 0;
      tiRecal = 0;
    }
	
	sys_ms += MILLISECONDS_PER_TICK;							//ϵͳʱ��
	if(sys_ms >= 1000){
		sys_ms = 0;
		sys_time++;
	}
    WDog_Cnt++;
    if((WDog_Cnt >= 120)&&WDog_Flag){                           //1.2sι��һ��
      WDog_Flag = 0;                                            //ι����־
      WDog_Cnt = 0;
      halInternalResetWatchDog();                               //ι��
    }
    
    INT_TIM2FLAG = 0xFFFFFFFF;                                  //����ж�
   
}



