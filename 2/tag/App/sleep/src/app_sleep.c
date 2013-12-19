/********************************Copyright (c)**********************************
**                			
**                         				
**-File Info--------------------------------------------------------------------
** File name:			app_sleep.c
** Last Version:		V1.0
** Descriptions:		���߹���
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

static int32u rang_false_cnt=0,diff_d_cnt=0;        		//���ʧ�ܴ����Ͳ�ֵ�ڷ�Χ�ڴ���
extern unsigned	char rang_ok;
extern Disp disp;
extern uint8_t WriteFactoryDateFlag;
extern uint8_t factory_date_succed;
extern uint8_t help_flag;
extern Call_Card call_card;
extern uint8_t UpperCommander;
/*******************************************************************************
** ��������:  void PeripheralInit(void)
** ��������:  ��������
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void powerdown_gpio(void)//δ��
{  
  //GPIO_DBGCFG &=  (~GPIO_DEBUGDIS);
  
  GPIO_PACFGH_REG |= PA4_CFG; 								//����PA4Ϊ���RF
  GPIO_PACFGH_REG &= ((~PA4_CFG)|(GPIOCFG_OUT<<PA4_CFG_BIT));
  GPIO_PAOUT |= PA4;
    
  GPIO_PBCFGH_REG |= PB6_CFG;			                	//FMQ
  GPIO_PBCFGH_REG &= ((~PB6_CFG)|(GPIOCFG_OUT<<PB6_CFG_BIT)); 
  GPIO_PBOUT &= ~PB6;  
  
  GPIO_PCCFGL_REG |= PC0_CFG;			                	//�ж�
  GPIO_PCCFGL_REG &= ((~PC0_CFG)|(GPIOCFG_OUT<<PC0_CFG_BIT)); 
  GPIO_PCOUT &= ~PC0;

  GPIO_PCCFGL_REG |= PC1_CFG;			                	//���
  GPIO_PCCFGL_REG &= ((~PC1_CFG)|(GPIOCFG_OUT<<PC1_CFG_BIT)); 
  GPIO_PCOUT &= ~PC1;
  
  GPIO_PCCFGL_REG |= PC2_CFG;			                	//��
  GPIO_PCCFGL_REG &= ((~PC2_CFG)|(GPIOCFG_OUT<<PC2_CFG_BIT)); 
  GPIO_PCOUT &= ~PC2;
  
  GPIO_PCCFGL_REG |= PC3_CFG;			                	//��
  GPIO_PCCFGL_REG &= ((~PC3_CFG)|(GPIOCFG_OUT<<PC3_CFG_BIT)); 
  GPIO_PCOUT &= ~PC3;
  
  GPIO_PCCFGH_REG |= PC7_CFG;			                	//����PC7Ϊ���ģʽ  ��ť
  GPIO_PCCFGH_REG &= ((~PC7_CFG)|(GPIOCFG_OUT<<PC7_CFG_BIT));
  GPIO_PCOUT &= ~PC7;
  
  disp.operation(0);

  if(WriteFactoryDateFlag == TRUE){							//�յ����������Ʒ�����һֱ��(���ߺ�Ҳһֱ��)
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

void powerup_gpio(void)//δ��
{
	GPIO_PCCFGH_REG |= PC7_CFG; 							//����PC7Ϊ��������ģʽ ��ť
	GPIO_PCCFGH_REG &= ((~PC7_CFG)|(GPIOCFG_IN_PUD<<PC7_CFG_BIT));
	GPIO_PCOUT_REG |= (GPIOOUT_PULLUP<<PC7_BIT);    
    
}

void PeripheralInit(void)
{
    GPIO_DBGCFG |=  GPIO_DEBUGDIS;
    GPIO_PCCFGL_REG |= PC2_CFG;			        			//����PC2Ϊ��©���ģʽLED_R
    GPIO_PCCFGL_REG &= ((~PC2_CFG)|(GPIOCFG_OUT<<PC2_CFG_BIT));

	GPIO_PCCFGL_REG |= PC3_CFG; 							//����PC3Ϊ��©���ģʽLED_D
	GPIO_PCCFGL_REG &= ((~PC3_CFG)|(GPIOCFG_OUT<<PC3_CFG_BIT));

	GPIO_PCCFGL_REG |= PC1_CFG; 							//����PC1Ϊ��©��ģʽMOTOR
	GPIO_PCCFGL_REG &= ((~PC1_CFG)|(GPIOCFG_OUT<<PC1_CFG_BIT));

    GPIO_PBCFGH_REG |= PB6_CFG;			        			//����PB6Ϊ��©��ģʽFMQ
    GPIO_PBCFGH_REG &= ((~PB6_CFG)|(GPIOCFG_OUT<<PB6_CFG_BIT));
    GPIO_PBOUT &=~PB6;
   
    GPIO_PCOUT &= ~(PC1|PC2|PC3);							//���϶����Ϊ��
   																			
    
	GPIO_PACFGH_REG |= PA6_CFG;			        			//����PA6Ϊ��©���ģʽDIIO0
    GPIO_PACFGH_REG &= ((~PA6_CFG)|(GPIOCFG_OUT<<PA6_CFG_BIT));
	GPIO_PAOUT &= ~PA6;

    GPIO_PCCFGH_REG |= PC7_CFG;			        			//����PC7Ϊ������ģʽ ��ť
    GPIO_PCCFGH_REG &= ((~PC7_CFG)|(GPIOCFG_IN_PUD<<PC7_CFG_BIT));
    GPIO_PCOUT |= PC7;

	GPIO_PACFGH_REG |= PA4_CFG; 							//����PA4Ϊ����RF
	GPIO_PACFGH_REG &= ((~PA4_CFG)|(GPIOCFG_IN<<PA4_CFG_BIT));

	GPIO_PBCFGH_REG |= PB5_CFG;			        			//����PB7Ϊ��©��ģʽRFID
    GPIO_PBCFGH_REG &= ((~PB5_CFG)|(GPIOCFG_OUT<<PB5_CFG_BIT));
	GPIO_PBOUT &=~PB7;
	

}

void nano_sleep()
{
	uint8_t value;
    //............. nano���߲���
    //**Dio�˿����� 
    NTRXSetIndexReg (0x00);
    NTRXSetRegister(NA_RxCmdStop,TRUE);								//�رս���
    NTRXSetRegister(NA_DioDirection,FALSE);               	        //����Dio�˿ڷ���
    NTRXSetRegister(NA_DioOutValueAlarmEnable,TRUE);      	        //�˿ڹ�������
    NTRXSetRegister(NA_DioAlarmStart,FALSE);              	        //
    NTRXSetRegister(NA_DioAlarmPolarity,FALSE);           	        //�½���
      
    NTRXSetRegister(NA_EnableWakeUpRtc,FALSE);            	        //rtc��Ϊ nano�����ж�Դ
    NTRXSetRegister(NA_EnableWakeUpDio,TRUE);             	        //Dio��Ϊnano�����ж�Դ   
    NTRXSetRegister(NA_PowerUpTime,0);
    NTRXSetRegister(NA_PowerDownMode,0);                  	        //���ù���ģʽ
    NTRXSetRegister(NA_PowerDown,TRUE);                   	        //nano����͹���
    /////

/*
	NTRXSetIndexReg (0x00);
	NTRXSetRegister(NA_RxCmdStop,TRUE);								//ֹͣ����
    																	//����Dio_0��Ϊ��������(�����ش���)
	//value = ((1<<NA_DioDirection_B)|(1<<NA_DioOutValueAlarmEnable_B)|(1<<NA_DioAlarmPolarity_B));
	value = ((1<<NA_DioDirection_B)|(1<<NA_DioOutValueAlarmEnable_B)|(1<<NA_DioAlarmPolarity_B)|(1<<NA_DioAlarmStart_B));
	NTRXWriteSingleSPI(NA_DioInValueAlarmStatus_O,value);
	//NTRXSetRegister(NA_DioAlarmStart,TRUE);
	NTRXWriteSingleSPI(NA_DioPortWe_O,1<<0);

	value = ((1<<NA_EnableWakeUpDio_B)|(1<<NA_PowerUpTime_LSB));		//����ģʽ����
	NTRXWriteSingleSPI(NA_PowerDown_O, (1<<NA_ResetBbClockGate_B)); 	//
	NTRXWriteSingleSPI(NA_EnableBbCrystal_O, 0x00);						//NA_ϵͳʱ�����
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
** ��������:  void sleep()
** ��������:  ����
** ��ڲ���: ����ʱ�� ��λ����
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void sleep(int32u sleepms)
{	
	int32u startcont;

	
        
	//**mcu����
	startcont = halCommonGetInt32uMillisecondTick();		//��õ�ǰϵͳ����
	SLEEPTMR_CMPAL = (startcont+sleepms)&0xffff;			//���ñȽϼĴ���
	SLEEPTMR_CMPAH = ((startcont+sleepms)>>16)&0xffff;
	INT_SLEEPTMRFLAG = INT_SLEEPTMRCMPA;					//����жϱ�־
	INT_SLEEPTMRCFG = INT_SLEEPTMRCMPA;						//�������߶�ʱ���Ƚ�Aƥ���ж�
	halPowerDown();
	halSleepWithOptions(SLEEPMODE_WAKETIMER,0<<7); 			//���˯�ߣ��ܽ�PA7��ʱ������
	halPowerUp();
	PeripheralInit();
	INT_SLEEPTMRCFG = INT_SLEEPTMRCFG_RESET;				//����˯�߶�ʱ���ж�
        ////
        
        /**mcu���߲���
	startcont = halCommonGetInt32uMillisecondTick();				//��õ�ǰϵͳ����
	SLEEPTMR_CMPAL = (startcont+sleepms)&0xffff;				//���ñȽϼĴ���
	SLEEPTMR_CMPAH = ((startcont+sleepms)>>16)&0xffff;
	INT_SLEEPTMRFLAG = INT_SLEEPTMRCMPA;					//����жϱ�־
	INT_SLEEPTMRCFG = INT_SLEEPTMRCMPA;						//�������߶�ʱ���Ƚ�Aƥ���ж�
	//halPowerDown();
    	halSleepWithOptions(SLEEPMODE_WAKETIMER,0); 		        	//���˯�ߣ��ܽ�PC1��ʱ������
	//halPowerUp();
	INT_SLEEPTMRCFG = INT_SLEEPTMRCFG_RESET;				//����˯�߶�ʱ���ж�
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
int32u sleep_time = STANDARD_T;								//����ʱ��
uint8_t no_sleep = 0;										//�ж��Ƿ��������
/*******************************************************************************
** ��������:  Sleep_Process()
** ��������:  ���߲��ԣ���������ʱ�䣬
              1.����MAXFALSE_CNT�β�಻�ɹ�������LONG_T����
              2.������಻�ɹ�������STANDARD_CNT֮�ڣ�����STANDARD_T����
              2.������಻�ɹ�������STANDARD_CNT��MAXFALSE_CNT�������DIFF_D_CNT�β��ֵ���ϴβ��ֵ��ֵ��DIFF_D�ڣ�������MID_T����
** ��ڲ���:  ��  
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void Sleep_Process(void)        //
{
	unsigned short temp;
	nano_sleep();
	if(rang_ok){                                                                                                        //���ɹ�
		rang_false_cnt = 0;
		rang_ok = 0;
																					//���ɹ�,�Ƚ��ϴβ���ֵ��������ʱ��/////
		if(old_dist.dist>cur_dist.dist){temp = old_dist.dist-cur_dist.dist;}else{temp = cur_dist.dist-old_dist.dist;}     //���㵱ǰ��������ʷ�����ֵ 
		if((temp<=DIFF_D)&&(cur_dist.dest[5]==old_dist.dest[5])&&(cur_dist.dest[4]==old_dist.dest[4])){                   //�жϲ�ֵ�����Ƿ�����еȳ������߻��Ǳ�׼����
			diff_d_cnt++;
			if(diff_d_cnt>=DIFF_D_CNT){                                                                                     
				sleep_time = MID_T;
				goto app_sleep_end;
			}else{}// end if(diff_d_cnt
    	}else{diff_d_cnt = 0;}//end if((temp<=DIFF_D)&&....
    
  	} else{ rang_false_cnt++;}//end if(rang_ok)
  
  	if(rang_false_cnt <= STANDARD_CNT)       										//���ݲ��ʧ�ܴ�����������ʱ��///                                                                             //���ݲ��ʧ�ܴ�����������ʱ��                                                                             
    	sleep_time = STANDARD_T;
  	else if(rang_false_cnt >= MAXFALSE_CNT)
    	sleep_time = LONG_T;
  	else
    	sleep_time = MID_T;
  
app_sleep_end:
	if(oldD_update_flag){															//������ʷ�����Ϣ
		old_dist.dist = cur_dist.dist;													
		old_dist.dest[4]= cur_dist.dest[4];
  		old_dist.dest[5]= cur_dist.dest[5];
		old_dist.time = cur_dist.time;

		pre_dist.dist = cur_dist.dist;													
		pre_dist.dest[4]= cur_dist.dest[4];
  		pre_dist.dest[5]= cur_dist.dest[5];
		pre_dist.time = cur_dist.time;
	}
  	sleep_time = 1000;																//�̶�����1s(�����ڲ���)
  	set_systime_ms((sleep_time + get_systime_ms()));								//����ϵͳʱ��      
  	powerdown_gpio();
  	halSleepForMillisecondsWithOptions(&sleep_time,0);
  	halPowerUp();
  	PeripheralInit();
/*
  	if(sleep_time!=0){               														//�쳣����
		AbnormalWakeTime++;
		if(EEPROM_Status&&EEPROM_Idle){
			EEPROMOn();
			halCommonDelayMicroseconds(1000);
           	WriteAbnormalWakeTime(AbnormalWakeTime);										//дδ�������ѵĴ���
           	EEPROMOff();
		}      
	}
 */
}
