/********************************Copyright (c)**********************************
**                			
**                          				
**-File Info--------------------------------------------------------------------
** File name:			App.c
** Last Version:		V1.0
** Descriptions:		�ƶ���ඨλ��ǩ
**------------------------------------------------------------------------------
** Created by:	 	QI
** Created date:     	2013-08-12
** Version:	       	V1.0
** Descriptions:    	3Ƶ(RFID/ZIGBEE/�������)
*******************************************************************************/
#include "iar.h"
#include "hal.h"
#include "uart.h"
#include "stdio.h"
#include "phy-library.h"
#include "hal/micro/cortexm3/micro-common.h"
#include "hal/micro/system-timer.h"
#include "hal/micro/cortexm3/I2C.h"
#include "hal/micro/cortexm3/flash.h"
#include "board.h"
#include "rfid.h"
#include "System.h"
#include "App.h"
#include "nano/ntrxutil.h"
#include "app_adc.h"
#include "app_sleep.h"
#include "startup_info.h"
#include "err_check.h"
#include "disp.h"
#include "commu.h"
#include "Zigbee.h"

/********************************************
* Globals * 
********************************************/
__no_init long int RAMFlag1;              	//��������������ʱ����ʼ���ı���
__no_init long int RAMFlag2;              	//�����ж����ϵ��������Ǹ�λ����

extern Locat_Info Location;               	//��λ��Ϣ���
extern  Bat       battery;                	//��������Ϣ
uint8_t fist_PowerOn = 1;                 	//��ʼ�ϵ��־
uint8_t cycle_cnt;                        	//��ѭ������
ERR_Type err_type;                        	//��¼��������
extern volatile uint16_t SendFailTime;     	//zigbee����ʧ�ܴ���
volatile uint16_t RecvSuccessTime;  		//zigbee���ճɹ�����
uint16_t AbnormalWakeTime;                	//���������Ѵ���

void start_rang(void);
void nano_init(void);
void InitApplication(void);
void hwclock_init(void);

extern volatile bool_t EEPROM_Status;
extern uint8_t Send_Channel;
extern uint8_t Recieve_Channel;

//#define EE_CLEAR
/*******************************************************************************
** ��������:   env_init
** ��������:  ���л�����ʼ
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void env_init(void)
{
  	uint16_t i;
  	//battery.stat = NLOW;                  					//������ֵΪ�ǵ͵���
  	//Location.time = 0;                    					//��λʱ��
  	//Location.state = 0;                   					//��λ״̬
  	//button_init();									//����
  	//disp_init();										//LED��������������ʼ��
  	//call_card_init();									//����ͨѶ��ʼ��
  	//help_init();										//����
  	//battery_show_init();								//��������
  	
	hwclock_init();									//��ʱ��
  //nano_init();  										   
  
  	EEPROMOn();                          			//�򿪹���
  	EEPROM_Status = EEPROM_SelfDetect(); 			//eeprom�Լ�
  	if(EEPROM_Status){ 
#ifdef EE_CLEAR										//EEPROM�ֶ�����
		I2cSendValue(User_Memory,CARD_IS_FIST_BOOT_ADDR,0x00,BLOCK_NUM);
#endif
		if(EEPROM_Fist_Boot()){						//��һ���ϵ�,��EEPROM����
			I2cInit(I2C_SPEED_FAST);
	 		for(i = 0;i < CARD_IS_FIST_BOOT_ADDR;i += 4){
	  			I2cSendValue(User_Memory,i,0x00,BLOCK_NUM);
				halCommonDelayMilliseconds(6);		
	 		}
		}
		WriteBootType();                         	//�жϸ�λ���Ͳ�д�뵽EEPROM
    	SendFailTime = ReadSendFailTime();       	//����ʧ�ܴ���
    	RecvSuccessTime = ReadRecvSuccessTime(); 	//˫��ͨ�Ŵ���   
    	AbnormalWakeTime = ReadAbnormalWakeTime();	//��ȡ���������Ѵ���
    	
    	RingForSuccess();                  			//������������ʾM24LR16��ʼ���ɹ�
	}
	Read_CardID_Config();							//���ÿ���
	Send_Channel = RF_Send_Channel();          		//�����ŵ�
	Recieve_Channel = RF_Recieve_Channel();    		//�����ŵ�
  	EEPROMOff();                         			//�رչ���
}


StartUp_Info *read_startinfo;           			//������Ϣ
uint8_t WDog_Flag;                      			//ι����־
uint8_t Run_Step;                       			//�������
/*******************************************************************************

** ��������:  void main(void)
** ��������:  ������
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void main(void)
{ 
  //Clearn_StartInfo();									//�ֶ���flash������¼
  startup_info_process();               			//��������
  read_startinfo = get_CurStartInfo();  			//���������Ϣ 
  halCommonDelayMilliseconds(2000);                             //Ԥ��������дʱ��
  halInit();                            			//ʱ�ӳ�ʼ��������У׼
  PeripheralInit();                     			//�����ʼ��
  env_init();                           			//���л�����ʼ��
  LEDBlink();                           			//LED����˸
  //halInternalEnableWatchDog();          			//ʹ�ܿ��Ź� 2.048s timeout
  INTERRUPTS_ON();                      			//�����ж�
  /*��ѭ��*/
  while(1){
  	/*
  	while(1){											//zigbee/EEPROM����
		EEPROM_Process();
		Zigbee_Transmit();
		halCommonDelayMilliseconds(50);
	}
	*/
    WDog_Flag = 1;                      			//ι����־   
    
    EEPROM_Process();
	
    Run_Step = 0xB0;                     
    if(Run_Step == 0xB0){							//��ص�ѹ�ɼ��������
		Battery_V_Process();
		Run_Step++; 
    }else{}
    
    if(Run_Step == 0xB1){
		do_zigbee();                				//zigbeeģʽ,����
		Run_Step++;
    }else{}
	
	if(Run_Step == 0xB2){
  		start_rang();                				//��༰��λ���� 	
		Run_Step++;
    }else{}
	
    if(Run_Step == 0xB3){
		communication();          					//˫��ͨѶ
	    Run_Step++;
    }else{}
    
    if(Run_Step == 0xB4){
    	err_check();               					//�����Լ�
    	Run_Step++;
    }else{}
    
    if(Run_Step == 0xB5){
      Sleep_Process();           					//���߲���
    }else{}  
	
	fist_PowerOn = 0;								//�ϵ��־

  } // end while(1)
} // end main()

