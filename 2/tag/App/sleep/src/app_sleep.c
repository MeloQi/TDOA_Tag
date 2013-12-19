/********************************Copyright (c)**********************************
**                			
**                         				
**-File Info--------------------------------------------------------------------
** File name:			app_sleep.c
** Last Version:		V1.0
** Descriptions:		休眠管理
**------------------------------------------------------------------------------
** Created by:	 	QI
** Created date:     	2013-08-12
** Version:	       	V1.0
** Descriptions:    	
*******************************************************************************/

#include "iar.h"
#include "hal.h"
#include "nano/ntrxutil.h"
#include "conf.h"
#include "sys_time.h"
#include "App.h"
#include "commu.h"
#include "disp.h"

static int32u rang_false_cnt=0,diff_d_cnt=0;        		//测距失败次数和差值在范围内次数
extern unsigned	char rang_ok;
extern Disp disp;
extern uint8_t WriteFactoryDateFlag;
extern uint8_t factory_date_succed;
extern uint8_t help_flag;
extern Call_Card call_card;
extern uint8_t UpperCommander;
/*******************************************************************************
** 函数名称:  void PeripheralInit(void)
** 函数功能:  引脚配置
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void powerdown_gpio(void)//未用
{  
  //GPIO_DBGCFG &=  (~GPIO_DEBUGDIS);
  
  GPIO_PACFGH_REG |= PA4_CFG; 								//配置PA4为输出RF
  GPIO_PACFGH_REG &= ((~PA4_CFG)|(GPIOCFG_OUT<<PA4_CFG_BIT));
  GPIO_PAOUT |= PA4;
    
  GPIO_PBCFGH_REG |= PB6_CFG;			                	//FMQ
  GPIO_PBCFGH_REG &= ((~PB6_CFG)|(GPIOCFG_OUT<<PB6_CFG_BIT)); 
  GPIO_PBOUT &= ~PB6;  
  
  GPIO_PCCFGL_REG |= PC0_CFG;			                	//中断
  GPIO_PCCFGL_REG &= ((~PC0_CFG)|(GPIOCFG_OUT<<PC0_CFG_BIT)); 
  GPIO_PCOUT &= ~PC0;

  GPIO_PCCFGL_REG |= PC1_CFG;			                	//马达
  GPIO_PCCFGL_REG &= ((~PC1_CFG)|(GPIOCFG_OUT<<PC1_CFG_BIT)); 
  GPIO_PCOUT &= ~PC1;
  
  GPIO_PCCFGL_REG |= PC2_CFG;			                	//灯
  GPIO_PCCFGL_REG &= ((~PC2_CFG)|(GPIOCFG_OUT<<PC2_CFG_BIT)); 
  GPIO_PCOUT &= ~PC2;
  
  GPIO_PCCFGL_REG |= PC3_CFG;			                	//灯
  GPIO_PCCFGL_REG &= ((~PC3_CFG)|(GPIOCFG_OUT<<PC3_CFG_BIT)); 
  GPIO_PCOUT &= ~PC3;
  
  GPIO_PCCFGH_REG |= PC7_CFG;			                	//配置PC7为输出模式  按钮
  GPIO_PCCFGH_REG &= ((~PC7_CFG)|(GPIOCFG_OUT<<PC7_CFG_BIT));
  GPIO_PCOUT &= ~PC7;
  
  disp.operation(0);

  if(WriteFactoryDateFlag == TRUE){							//收到出厂命令后灯蜂鸣器一直开(休眠后也一直开)
  	if(factory_date_succed){
		disp.operation((GREEN_LED|FM|MOTOR));         		
	}else{
		disp.operation((RED_LED|FM|MOTOR));            	
	}
  }
  if(help_flag){
      disp.operation(RED_LED|FM|MOTOR);      		
  }
  if(call_card.stat || UpperCommander){
  	disp.operation(RED_LED|FM|MOTOR);
  }
  
}

void powerup_gpio(void)//未用
{
	GPIO_PCCFGH_REG |= PC7_CFG; 							//配置PC7为上拉输入模式 按钮
	GPIO_PCCFGH_REG &= ((~PC7_CFG)|(GPIOCFG_IN_PUD<<PC7_CFG_BIT));
	GPIO_PCOUT_REG |= (GPIOOUT_PULLUP<<PC7_BIT);    
    
}

void PeripheralInit(void)
{
    GPIO_DBGCFG |=  GPIO_DEBUGDIS;
    GPIO_PCCFGL_REG |= PC2_CFG;			        			//配置PC2为开漏输出模式LED_R
    GPIO_PCCFGL_REG &= ((~PC2_CFG)|(GPIOCFG_OUT<<PC2_CFG_BIT));

	GPIO_PCCFGL_REG |= PC3_CFG; 							//配置PC3为开漏输出模式LED_D
	GPIO_PCCFGL_REG &= ((~PC3_CFG)|(GPIOCFG_OUT<<PC3_CFG_BIT));

	GPIO_PCCFGL_REG |= PC1_CFG; 							//配置PC1为开漏出模式MOTOR
	GPIO_PCCFGL_REG &= ((~PC1_CFG)|(GPIOCFG_OUT<<PC1_CFG_BIT));

    GPIO_PBCFGH_REG |= PB6_CFG;			        			//配置PB6为开漏出模式FMQ
    GPIO_PBCFGH_REG &= ((~PB6_CFG)|(GPIOCFG_OUT<<PB6_CFG_BIT));
    GPIO_PBOUT &=~PB6;
   
    GPIO_PCOUT &= ~(PC1|PC2|PC3);							//以上都输出为低
   																			
    
	GPIO_PACFGH_REG |= PA6_CFG;			        			//配置PA6为开漏输出模式DIIO0
    GPIO_PACFGH_REG &= ((~PA6_CFG)|(GPIOCFG_OUT<<PA6_CFG_BIT));
	GPIO_PAOUT &= ~PA6;

    GPIO_PCCFGH_REG |= PC7_CFG;			        			//配置PC7为拉输入模式 按钮
    GPIO_PCCFGH_REG &= ((~PC7_CFG)|(GPIOCFG_IN_PUD<<PC7_CFG_BIT));
    GPIO_PCOUT |= PC7;

	GPIO_PACFGH_REG |= PA4_CFG; 							//配置PA4为输入RF
	GPIO_PACFGH_REG &= ((~PA4_CFG)|(GPIOCFG_IN<<PA4_CFG_BIT));

	GPIO_PBCFGH_REG |= PB5_CFG;			        			//配置PB7为开漏出模式RFID
    GPIO_PBCFGH_REG &= ((~PB5_CFG)|(GPIOCFG_OUT<<PB5_CFG_BIT));
	GPIO_PBOUT &=~PB7;
	

}

void nano_sleep()
{
	uint8_t value;
    //............. nano休眠操作
    //**Dio端口配置 
    NTRXSetIndexReg (0x00);
    NTRXSetRegister(NA_RxCmdStop,TRUE);								//关闭接收
    NTRXSetRegister(NA_DioDirection,FALSE);               	        //配置Dio端口方向
    NTRXSetRegister(NA_DioOutValueAlarmEnable,TRUE);      	        //端口功能配置
    NTRXSetRegister(NA_DioAlarmStart,FALSE);              	        //
    NTRXSetRegister(NA_DioAlarmPolarity,FALSE);           	        //下降沿
      
    NTRXSetRegister(NA_EnableWakeUpRtc,FALSE);            	        //rtc作为 nano唤醒中断源
    NTRXSetRegister(NA_EnableWakeUpDio,TRUE);             	        //Dio作为nano唤醒中断源   
    NTRXSetRegister(NA_PowerUpTime,0);
    NTRXSetRegister(NA_PowerDownMode,0);                  	        //配置功耗模式
    NTRXSetRegister(NA_PowerDown,TRUE);                   	        //nano进入低功耗
    /////

/*
	NTRXSetIndexReg (0x00);
	NTRXSetRegister(NA_RxCmdStop,TRUE);								//停止接收
    																	//配置Dio_0作为唤醒引脚(上升沿触发)
	//value = ((1<<NA_DioDirection_B)|(1<<NA_DioOutValueAlarmEnable_B)|(1<<NA_DioAlarmPolarity_B));
	value = ((1<<NA_DioDirection_B)|(1<<NA_DioOutValueAlarmEnable_B)|(1<<NA_DioAlarmPolarity_B)|(1<<NA_DioAlarmStart_B));
	NTRXWriteSingleSPI(NA_DioInValueAlarmStatus_O,value);
	//NTRXSetRegister(NA_DioAlarmStart,TRUE);
	NTRXWriteSingleSPI(NA_DioPortWe_O,1<<0);

	value = ((1<<NA_EnableWakeUpDio_B)|(1<<NA_PowerUpTime_LSB));		//休眠模式配置
	NTRXWriteSingleSPI(NA_PowerDown_O, (1<<NA_ResetBbClockGate_B)); 	//
	NTRXWriteSingleSPI(NA_EnableBbCrystal_O, 0x00);						//NA_系统时钟相关
	NTRXWriteSingleSPI(NA_EnableWakeUpRtc_O, value );
	NTRXWriteSingleSPI(NA_PowerDown_O, (1<<NA_ResetBbClockGate_B)|(1<<NA_PowerDown_B));
*/
}


/*
void nano_ExitPowerdown( void )
{

	NTRXWriteSingleSPI( NA_PowerDown_O, 0 ) ;
	NTRXWriteSingleSPI( NA_EnableBbCrystal_O, 0x03 ) ;
	NTRXWriteSingleSPI( NA_EnableWakeUpRtc_O, 0 ) ;
	NTRXWriteSingleSPI( NA_RxCmdStart, True ) ;
}
*/


/*******************************************************************************
** 函数名称:  void sleep()
** 函数功能:  休眠
** 入口参数: 休眠时间 单位毫秒
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void sleep(int32u sleepms)
{	
	int32u startcont;

	
        
	//**mcu休眠
	startcont = halCommonGetInt32uMillisecondTick();		//获得当前系统计数
	SLEEPTMR_CMPAL = (startcont+sleepms)&0xffff;			//配置比较寄存器
	SLEEPTMR_CMPAH = ((startcont+sleepms)>>16)&0xffff;
	INT_SLEEPTMRFLAG = INT_SLEEPTMRCMPA;					//清除中断标志
	INT_SLEEPTMRCFG = INT_SLEEPTMRCMPA;						//设置休眠定时器比较A匹配中断
	halPowerDown();
	halSleepWithOptions(SLEEPMODE_WAKETIMER,0<<7); 			//深层睡眠，管脚PA7或定时器唤醒
	halPowerUp();
	PeripheralInit();
	INT_SLEEPTMRCFG = INT_SLEEPTMRCFG_RESET;				//禁用睡眠定时器中断
        ////
        
        /**mcu休眠测试
	startcont = halCommonGetInt32uMillisecondTick();				//获得当前系统计数
	SLEEPTMR_CMPAL = (startcont+sleepms)&0xffff;				//配置比较寄存器
	SLEEPTMR_CMPAH = ((startcont+sleepms)>>16)&0xffff;
	INT_SLEEPTMRFLAG = INT_SLEEPTMRCMPA;					//清除中断标志
	INT_SLEEPTMRCFG = INT_SLEEPTMRCMPA;						//设置休眠定时器比较A匹配中断
	//halPowerDown();
    	halSleepWithOptions(SLEEPMODE_WAKETIMER,0); 		        	//深层睡眠，管脚PC1或定时器唤醒
	//halPowerUp();
	INT_SLEEPTMRCFG = INT_SLEEPTMRCFG_RESET;				//禁用睡眠定时器中断
        *////
}



void hwclock_init(void);
extern Rang_Dist old_dist,cur_dist,pre_dist;
extern uint8_t fist_PowerOn;
extern Call_Card call_card;
extern uint8_t help_flag;
extern struct button 	button1;
extern uint8_t UpperCommander;
extern uint8_t WriteFactoryDateFlag;
extern uint8_t	oldD_update_flag;
int32u sleep_time = STANDARD_T;								//休眠时间
uint8_t no_sleep = 0;										//判断是否进入休眠
/*******************************************************************************
** 函数名称:  Sleep_Process()
** 函数功能:  休眠策略，三种休眠时间，
              1.连续MAXFALSE_CNT次测距不成功，休眠LONG_T毫秒
              2.连续测距不成功次数在STANDARD_CNT之内，休眠STANDARD_T毫秒
              2.连续测距不成功次数在STANDARD_CNT与MAXFALSE_CNT间或连续DIFF_D_CNT次测距值与上次测距值差值在DIFF_D内，则休眠MID_T毫秒
** 入口参数:  无  
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void Sleep_Process(void)        //
{
	unsigned short temp;
	nano_sleep();
	if(rang_ok){                                                                                                        //测距成功
		rang_false_cnt = 0;
		rang_ok = 0;
																					//测距成功,比较上次测距差值决定休眠时间/////
		if(old_dist.dist>cur_dist.dist){temp = old_dist.dist-cur_dist.dist;}else{temp = cur_dist.dist-old_dist.dist;}     //计算当前距离与历史距离差值 
		if((temp<=DIFF_D)&&(cur_dist.dest[5]==old_dist.dest[5])&&(cur_dist.dest[4]==old_dist.dest[4])){                   //判断差值决定是否进行中等长度休眠还是标准休眠
			diff_d_cnt++;
			if(diff_d_cnt>=DIFF_D_CNT){                                                                                     
				sleep_time = MID_T;
				goto app_sleep_end;
			}else{}// end if(diff_d_cnt
    	}else{diff_d_cnt = 0;}//end if((temp<=DIFF_D)&&....
    
  	} else{ rang_false_cnt++;}//end if(rang_ok)
  
  	if(rang_false_cnt <= STANDARD_CNT)       										//根据测距失败次数决定休眠时间///                                                                             //根据测距失败次数决定休眠时间                                                                             
    	sleep_time = STANDARD_T;
  	else if(rang_false_cnt >= MAXFALSE_CNT)
    	sleep_time = LONG_T;
  	else
    	sleep_time = MID_T;
  
app_sleep_end:
	if(oldD_update_flag){															//更新历史测距信息
		old_dist.dist = cur_dist.dist;													
		old_dist.dest[4]= cur_dist.dest[4];
  		old_dist.dest[5]= cur_dist.dest[5];
		old_dist.time = cur_dist.time;

		pre_dist.dist = cur_dist.dist;													
		pre_dist.dest[4]= cur_dist.dest[4];
  		pre_dist.dest[5]= cur_dist.dest[5];
		pre_dist.time = cur_dist.time;
	}
  	sleep_time = 1000;																//固定休眠1s(仅用于测试)
  	set_systime_ms((sleep_time + get_systime_ms()));								//更新系统时间      
  	powerdown_gpio();
  	halSleepForMillisecondsWithOptions(&sleep_time,0);
  	halPowerUp();
  	PeripheralInit();
/*
  	if(sleep_time!=0){               														//异常唤醒
		AbnormalWakeTime++;
		if(EEPROM_Status&&EEPROM_Idle){
			EEPROMOn();
			halCommonDelayMicroseconds(1000);
           	WriteAbnormalWakeTime(AbnormalWakeTime);										//写未正常唤醒的次数
           	EEPROMOff();
		}      
	}
 */
}
