/********************************Copyright (c)**********************************
**                			
**                          				
**-File Info--------------------------------------------------------------------
** File name:			App.c
** Last Version:		V1.0
** Descriptions:		移动测距定位标签
**------------------------------------------------------------------------------
** Created by:	 	QI
** Created date:     	2013-08-12
** Version:	       	V1.0
** Descriptions:    	3频(RFID/ZIGBEE/测距无线)
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
__no_init long int RAMFlag1;              	//定义两个在启动时不初始化的变量
__no_init long int RAMFlag2;              	//用于判断是上电重启还是复位重启

extern Locat_Info Location;               	//定位信息相关
extern  Bat       battery;                	//电池相关信息
uint8_t fist_PowerOn = 1;                 	//初始上电标志
uint8_t cycle_cnt;                        	//主循环次数
ERR_Type err_type;                        	//记录错误类型
extern volatile uint16_t SendFailTime;     	//zigbee发送失败次数
volatile uint16_t RecvSuccessTime;  		//zigbee接收成功次数
uint16_t AbnormalWakeTime;                	//非正常唤醒次数

void start_rang(void);
void nano_init(void);
void InitApplication(void);
void hwclock_init(void);

extern volatile bool_t EEPROM_Status;
extern uint8_t Send_Channel;
extern uint8_t Recieve_Channel;

//#define EE_CLEAR
/*******************************************************************************
** 函数名称:   env_init
** 函数功能:  运行环境初始
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void env_init(void)
{
  	uint16_t i;
  	//battery.stat = NLOW;                  					//电量初值为非低电量
  	//Location.time = 0;                    					//定位时间
  	//Location.state = 0;                   					//定位状态
  	//button_init();									//按键
  	//disp_init();										//LED、蜂鸣器、马达初始化
  	//call_card_init();									//井上通讯初始化
  	//help_init();										//求助
  	//battery_show_init();								//电量提醒
  	
	hwclock_init();									//定时器
  //nano_init();  										   
  
  	EEPROMOn();                          			//打开供电
  	EEPROM_Status = EEPROM_SelfDetect(); 			//eeprom自检
  	if(EEPROM_Status){ 
#ifdef EE_CLEAR										//EEPROM手动清零
		I2cSendValue(User_Memory,CARD_IS_FIST_BOOT_ADDR,0x00,BLOCK_NUM);
#endif
		if(EEPROM_Fist_Boot()){						//第一次上电,将EEPROM清零
			I2cInit(I2C_SPEED_FAST);
	 		for(i = 0;i < CARD_IS_FIST_BOOT_ADDR;i += 4){
	  			I2cSendValue(User_Memory,i,0x00,BLOCK_NUM);
				halCommonDelayMilliseconds(6);		
	 		}
		}
		WriteBootType();                         	//判断复位类型并写入到EEPROM
    	SendFailTime = ReadSendFailTime();       	//发送失败次数
    	RecvSuccessTime = ReadRecvSuccessTime(); 	//双向通信次数   
    	AbnormalWakeTime = ReadAbnormalWakeTime();	//读取非正常唤醒次数
    	
    	RingForSuccess();                  			//蜂鸣器蜂鸣表示M24LR16初始化成功
	}
	Read_CardID_Config();							//配置卡号
	Send_Channel = RF_Send_Channel();          		//发送信道
	Recieve_Channel = RF_Recieve_Channel();    		//接收信道
  	EEPROMOff();                         			//关闭供电
}


StartUp_Info *read_startinfo;           			//重启信息
uint8_t WDog_Flag;                      			//喂狗标志
uint8_t Run_Step;                       			//环环相扣
/*******************************************************************************

** 函数名称:  void main(void)
** 函数功能:  主程序
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void main(void)
{ 
  //Clearn_StartInfo();									//手动清flash启动记录
  startup_info_process();               			//重启处理
  read_startinfo = get_CurStartInfo();  			//获得重启信息 
  halCommonDelayMilliseconds(2000);                             //预留出的烧写时间
  halInit();                            			//时钟初始化等外设校准
  PeripheralInit();                     			//外设初始化
  env_init();                           			//运行环境初始化
  LEDBlink();                           			//LED灯闪烁
  //halInternalEnableWatchDog();          			//使能看门狗 2.048s timeout
  INTERRUPTS_ON();                      			//开总中断
  /*主循环*/
  while(1){
  	/*
  	while(1){											//zigbee/EEPROM测试
		EEPROM_Process();
		Zigbee_Transmit();
		halCommonDelayMilliseconds(50);
	}
	*/
    WDog_Flag = 1;                      			//喂狗标志   
    
    EEPROM_Process();
	
    Run_Step = 0xB0;                     
    if(Run_Step == 0xB0){							//电池电压采集处理策略
		Battery_V_Process();
		Run_Step++; 
    }else{}
    
    if(Run_Step == 0xB1){
		do_zigbee();                				//zigbee模式,发送
		Run_Step++;
    }else{}
	
	if(Run_Step == 0xB2){
  		start_rang();                				//测距及定位策略 	
		Run_Step++;
    }else{}
	
    if(Run_Step == 0xB3){
		communication();          					//双向通讯
	    Run_Step++;
    }else{}
    
    if(Run_Step == 0xB4){
    	err_check();               					//错误自检
    	Run_Step++;
    }else{}
    
    if(Run_Step == 0xB5){
      Sleep_Process();           					//休眠策略
    }else{}  
	
	fist_PowerOn = 0;								//上电标志

  } // end while(1)
} // end main()

