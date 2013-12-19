/********************************Copyright (c)**********************************
**                			
**                          				
**-File Info--------------------------------------------------------------------
** File name:			startup_info.c
** Last Version:		V1.0
** Descriptions:		处理冷热启动记录(flash)
**------------------------------------------------------------------------------
** creat by:	 		QI
** creat date:     		2013-08-12
** Version:	       	V1.0
** Descriptions:    	
*******************************************************************************/

#include "iar.h"
#include "hal.h"
#include "hal/micro/cortexm3/flash.h"
#include "startup_info.h"

uint32_t addr1 = 0x0000FC00;
uint32_t addr = 0x0800FC00;                                 //存入flash的首地址,扇区63(此扇区大小1K，此muc共64个扇区，扇区编号从0开始)
StartUp_Info value_set;                                     //保存要设置重启信息的集合
extern long int RAMFlag1;                                   //定义两个在启动时不初始化的变量
extern long int RAMFlag2;                                   //用于判断是上电重启还是复位重启

/*******************************************************************************
** 函数名称:   set_startup_info
** 函数功能:  将重启信息出入flash
** 入口参数:  StartUp_Info：描述重启信息结构体
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
static void set_startup_info(StartUp_Info *value)
{
  uint32_t len;                                               //写入flash中的长度(半字长)
  len = (sizeof(StartUp_Info)+1)/2;                           //半字长度为单位
  
  #define POWERON_FLASH (*((volatile int32u *)addr))
  halInternalFlashErase(MFB_PAGE_ERASE,addr);                 //将StartUp_Info结构写入flash
  while(halFlashEraseIsActive());
  halInternalFlashWrite(addr,(unsigned short*)value,len);
}

/*******************************************************************************
** 函数名称:   IsFist_PowerOn
** 函数功能:  判断是否为第一次上电
** 入口参数:  无
** 出口参数:  返回为1 表示是第一次上电，否则不是第一次上电
** 备    注:  无
*******************************************************************************/
static uint8_t IsFist_PowerOn()
{
  StartUp_Info *sui;
  sui = (StartUp_Info *)addr;
  if(sui->isfist_on != 0x12801280)
    return 1;
  else
    return 0;
}

/*******************************************************************************
** 函数名称:   startup_info_process
** 函数功能:  重启处理主函数，主要做上电重启，复位重启计数
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void startup_info_process(void)
{
                                                              //保存最终的处理结果
  StartUp_Info *read_value;                                   //用于读取flash中的结构体信息
  
  read_value = (StartUp_Info *)addr;
  
  if(IsFist_PowerOn()){                                        //第一次上电初  
    value_set.poweron_cnt = 1;                                 //始化flash中各个字段值
    value_set.powerup_cnt = 1;
    value_set.isfist_on   = 0x12801280;
    RAMFlag1 = 0x128A128A;                                      //在ram非初始化区存入特定值，用于判断是否为复位重启                               
    RAMFlag2 = 0x128B128B;
  }else{                                                        //非第一次上电，则判断是那种类型的启动
    if((RAMFlag1 == 0x128A128A)&&(RAMFlag2 == 0x128B128B)){     //复位重启
     value_set.powerup_cnt = (read_value->powerup_cnt + 1);     //更新复位重启计数
     value_set.poweron_cnt = read_value->poweron_cnt;           //其他字段不变
     value_set.isfist_on   = read_value->isfist_on;          
    }else{
     RAMFlag1 = 0x128A128A;                                                        
     RAMFlag2 = 0x128B128B;
     value_set.powerup_cnt = read_value->powerup_cnt;     
     value_set.poweron_cnt = (read_value->poweron_cnt+1);       //更新上电重启计数
     value_set.isfist_on   = read_value->isfist_on;          
    }//end if((RAMFlag1 == ..)
  }//end if(IsFist_PowerOn)
  set_startup_info(&value_set);                                    
}

/*******************************************************************************
** 函数名称:   get_CurStartInfo
** 函数功能:  获得当前重启信息
** 入口参数:  无
** 出口参数:  返回StartUp_Info结构体指针，内包含重启的各个信息
** 备    注:  无
*******************************************************************************/
StartUp_Info *get_CurStartInfo(void)
{
  StartUp_Info *read_value;
  read_value = (StartUp_Info *)addr;
  return read_value;
}

/*******************************************************************************
** 函数名称:   Clearn_StartInfo
** 函数功能:  清除flash中有关重启的信息字段
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void Clearn_StartInfo(void)
{
  StartUp_Info value;
  value.powerup_cnt = 0;
  value.poweron_cnt = 0;
  value.isfist_on = 0;
  set_startup_info(&value);
}