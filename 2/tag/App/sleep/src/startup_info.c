/********************************Copyright (c)**********************************
**                			
**                          				
**-File Info--------------------------------------------------------------------
** File name:			startup_info.c
** Last Version:		V1.0
** Descriptions:		��������������¼(flash)
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
uint32_t addr = 0x0800FC00;                                 //����flash���׵�ַ,����63(��������С1K����muc��64��������������Ŵ�0��ʼ)
StartUp_Info value_set;                                     //����Ҫ����������Ϣ�ļ���
extern long int RAMFlag1;                                   //��������������ʱ����ʼ���ı���
extern long int RAMFlag2;                                   //�����ж����ϵ��������Ǹ�λ����

/*******************************************************************************
** ��������:   set_startup_info
** ��������:  ��������Ϣ����flash
** ��ڲ���:  StartUp_Info������������Ϣ�ṹ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
static void set_startup_info(StartUp_Info *value)
{
  uint32_t len;                                               //д��flash�еĳ���(���ֳ�)
  len = (sizeof(StartUp_Info)+1)/2;                           //���ֳ���Ϊ��λ
  
  #define POWERON_FLASH (*((volatile int32u *)addr))
  halInternalFlashErase(MFB_PAGE_ERASE,addr);                 //��StartUp_Info�ṹд��flash
  while(halFlashEraseIsActive());
  halInternalFlashWrite(addr,(unsigned short*)value,len);
}

/*******************************************************************************
** ��������:   IsFist_PowerOn
** ��������:  �ж��Ƿ�Ϊ��һ���ϵ�
** ��ڲ���:  ��
** ���ڲ���:  ����Ϊ1 ��ʾ�ǵ�һ���ϵ磬�����ǵ�һ���ϵ�
** ��    ע:  ��
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
** ��������:   startup_info_process
** ��������:  ������������������Ҫ���ϵ���������λ��������
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void startup_info_process(void)
{
                                                              //�������յĴ�����
  StartUp_Info *read_value;                                   //���ڶ�ȡflash�еĽṹ����Ϣ
  
  read_value = (StartUp_Info *)addr;
  
  if(IsFist_PowerOn()){                                        //��һ���ϵ��  
    value_set.poweron_cnt = 1;                                 //ʼ��flash�и����ֶ�ֵ
    value_set.powerup_cnt = 1;
    value_set.isfist_on   = 0x12801280;
    RAMFlag1 = 0x128A128A;                                      //��ram�ǳ�ʼ���������ض�ֵ�������ж��Ƿ�Ϊ��λ����                               
    RAMFlag2 = 0x128B128B;
  }else{                                                        //�ǵ�һ���ϵ磬���ж����������͵�����
    if((RAMFlag1 == 0x128A128A)&&(RAMFlag2 == 0x128B128B)){     //��λ����
     value_set.powerup_cnt = (read_value->powerup_cnt + 1);     //���¸�λ��������
     value_set.poweron_cnt = read_value->poweron_cnt;           //�����ֶβ���
     value_set.isfist_on   = read_value->isfist_on;          
    }else{
     RAMFlag1 = 0x128A128A;                                                        
     RAMFlag2 = 0x128B128B;
     value_set.powerup_cnt = read_value->powerup_cnt;     
     value_set.poweron_cnt = (read_value->poweron_cnt+1);       //�����ϵ���������
     value_set.isfist_on   = read_value->isfist_on;          
    }//end if((RAMFlag1 == ..)
  }//end if(IsFist_PowerOn)
  set_startup_info(&value_set);                                    
}

/*******************************************************************************
** ��������:   get_CurStartInfo
** ��������:  ��õ�ǰ������Ϣ
** ��ڲ���:  ��
** ���ڲ���:  ����StartUp_Info�ṹ��ָ�룬�ڰ��������ĸ�����Ϣ
** ��    ע:  ��
*******************************************************************************/
StartUp_Info *get_CurStartInfo(void)
{
  StartUp_Info *read_value;
  read_value = (StartUp_Info *)addr;
  return read_value;
}

/*******************************************************************************
** ��������:   Clearn_StartInfo
** ��������:  ���flash���й���������Ϣ�ֶ�
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void Clearn_StartInfo(void)
{
  StartUp_Info value;
  value.powerup_cnt = 0;
  value.poweron_cnt = 0;
  value.isfist_on = 0;
  set_startup_info(&value);
}