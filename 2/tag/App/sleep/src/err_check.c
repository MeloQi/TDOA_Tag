/********************************Copyright (c)**********************************
**                			
**                          				
**-File Info--------------------------------------------------------------------
** File name:			err.c
** Last Version:		V1.0
** Descriptions:		�Լ촦��
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
** ��������:  err_check
** ��������:  ���д�����
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void err_check(void)
{
  if(get_systime()>=ERRCheck_Time){
    ERRCheck_Time = (get_systime() + 60);         //1�����Լ�һ��
    if(NTRXCheckVerRev()){                        //nano�Լ���ȷ
    }else{                                        //nano�Լ���󣬺������
      disp.operation(RED_LED);
      halCommonDelayMilliseconds(50);
      disp.operation(0);
    }
  }
}