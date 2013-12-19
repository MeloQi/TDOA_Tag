/********************************Copyright (c)**********************************
**                			
**                          				
**-File Info--------------------------------------------------------------------
** File name:			sys_time.c
** Last Version:		V1.0
** Descriptions:		系统时间处理
**------------------------------------------------------------------------------
** creat by:	 		qi
** creat date:     		2013-08-12
** Version:	       	V1.0
** Descriptions:    	
*******************************************************************************/
#include "iar.h"
#include "hal.h"
#include  "sys_time.h"

uint32_t  sys_time = 0;                       //系统开始运行到现在的时间
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
** 函数名称:  set_mydelymain
** 函数功能:  设置某个模块的延时
** 入口参数:  ev_t模块对应的事件触发时间对象,dely_s模块要延时的秒,dely_ms模块要延时的ms
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void set_mydely(MyEvent_Time *ev_t,uint32_t dely_s,uint16_t dely_ms)
{
	ev_t->s = get_systime() + dely_s + ((dely_ms + get_systime_ms())/1000);
	ev_t->ms = ((dely_ms + get_systime_ms())%1000);
}

/*******************************************************************************
** 函数名称:  mydely_come
** 函数功能:  判断是否延时时间到
** 入口参数:  要判断的时间事件对象
** 出口参数:  超过等待时间点的时间，单位ms
** 备    注:  无
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