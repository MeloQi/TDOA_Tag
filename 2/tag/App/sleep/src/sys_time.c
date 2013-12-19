/********************************Copyright (c)**********************************
**                			
**                          				
**-File Info--------------------------------------------------------------------
** File name:			sys_time.c
** Last Version:		V1.0
** Descriptions:		ϵͳʱ�䴦��
**------------------------------------------------------------------------------
** creat by:	 		qi
** creat date:     		2013-08-12
** Version:	       	V1.0
** Descriptions:    	
*******************************************************************************/
#include "iar.h"
#include "hal.h"
#include  "sys_time.h"

uint32_t  sys_time = 0;                       //ϵͳ��ʼ���е����ڵ�ʱ��
uint16_t  sys_ms = 0;
uint32_t get_systime()
{
  return sys_time;
}

uint16_t get_systime_ms(void)
{
  return sys_ms;
}
void set_systime(uint32_t t)
{
  sys_time = t;
}

void set_systime_ms(uint16_t t)
{
  sys_ms = (t%1000);
  sys_time += (t/1000);
}

/*******************************************************************************
** ��������:  set_mydelymain
** ��������:  ����ĳ��ģ�����ʱ
** ��ڲ���:  ev_tģ���Ӧ���¼�����ʱ�����,dely_sģ��Ҫ��ʱ����,dely_msģ��Ҫ��ʱ��ms
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void set_mydely(MyEvent_Time *ev_t,uint32_t dely_s,uint16_t dely_ms)
{
	ev_t->s = get_systime() + dely_s + ((dely_ms + get_systime_ms())/1000);
	ev_t->ms = ((dely_ms + get_systime_ms())%1000);
}

/*******************************************************************************
** ��������:  mydely_come
** ��������:  �ж��Ƿ���ʱʱ�䵽
** ��ڲ���:  Ҫ�жϵ�ʱ���¼�����
** ���ڲ���:  �����ȴ�ʱ����ʱ�䣬��λms
** ��    ע:  ��
*******************************************************************************/
int32_t mydely_come(MyEvent_Time *ev_t)
{	
	uint32_t td = 0;
	uint32_t ts = get_systime();
	uint16_t tms = get_systime_ms();
	
	if(ts >= ev_t->s){
		td = ts - ev_t->s;
		if(td >= 0xFFFF) 
			return (0);
		if((tms >= ev_t->ms)||(td>0)){
			td = (td*1000) + tms - ev_t->ms;
			return ((int32_t)td);
		}
	}

	return (-1);
}