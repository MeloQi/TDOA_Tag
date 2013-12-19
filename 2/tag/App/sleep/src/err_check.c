/********************************Copyright (c)**********************************
**                			
**                          				
**-File Info--------------------------------------------------------------------
** File name:			err.c
** Last Version:		V1.0
** Descriptions:		自检处理
**------------------------------------------------------------------------------
** Created by:	 	QI
** Created date:     	2013-08-12
** Version:	       	V1.0
** Descriptions:    	
*******************************************************************************/

#include "iar.h"
#include "hal.h"
#include "nano/ntrxutil.h"
#include "sys_time.h"
#include "App.h"
#include "disp.h"

static  uint32_t  ERRCheck_Time;
extern uint8_t Run_Step;
extern Disp disp;
/*******************************************************************************
** 函数名称:  err_check
** 函数功能:  运行错误检查
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void err_check(void)
{
  if(get_systime()>=ERRCheck_Time){
    ERRCheck_Time = (get_systime() + 60);         //1分钟自检一次
    if(NTRXCheckVerRev()){                        //nano自检正确
    }else{                                        //nano自检错误，红灯提醒
      disp.operation(RED_LED);
      halCommonDelayMilliseconds(50);
      disp.operation(0);
    }
  }
}