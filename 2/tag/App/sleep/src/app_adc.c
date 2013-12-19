/********************************Copyright (c)**********************************
**                			
**                          				
**-File Info--------------------------------------------------------------------
** File name:			app_adc.c
** Last Version:		V1.0
** Descriptions:		��ع���
**------------------------------------------------------------------------------
** Created by:	 	Qi
** Created date:     	2013-08-12
** Version:	       	V1.0
** Descriptions:    	
*******************************************************************************/
#include "iar.h"
#include "hal.h"
#include "hal/micro/adc.h"
#include "nano/ntrxtypes.h"
#include "conf.h"
#include "sys_time.h"
#include "app_adc.h"
#include "disp.h"
#include "commu.h"
#include "rfid.h"

extern  uint8_t   fist_PowerOn;
static  int16u    ADCData[ADC_DATA_NUM];            //����ɼ�����ADC����
        Bat       battery = {.stat = NLOW,
							 .v = 0,
							};                          //�����Ϣ
static  uint32_t  ADC_Time;                         //ADC����ʱ��
/*******************************************************************************
** ��������:  int16u sort_average(void)
** ��������:  ð��������ƽ��ֵ
** ��ڲ���:  adcData_sort  Ҫ���������
              lenght        ���鳤��
              num           ������м����ֵ��ƽ��
** ���ڲ���:  int16u ������ƽ��ֵ
** ��    ע:  ��
*******************************************************************************/
static int16u sort_average(int16u *adcData_sort,int8u lenght,int8u num)
{
    int8u  len = (lenght-1);
    int16u temp,i,j;
    int32u  adc_sum = 0;
    
    for(j=0;j<=len;j++){ 
      for (i=0;i<len-j;i++)
        if(adcData_sort[i]>adcData_sort[i+1]){ 
          temp=adcData_sort[i];
          adcData_sort[i]=adcData_sort[i+1];
          adcData_sort[i+1]=temp;
        }
    } 
    
    temp = ((len-num)/2);
    for(i=0;i<num;i++){
      adc_sum += adcData_sort[i+temp];
    }
    
    return ((int16u)(adc_sum/num));
}

/*******************************************************************************
** ��������:  double get_Battery_V(void)
** ��������:  ��ȡ��ص�ѹ
** ��ڲ���:  ��
** ���ڲ���:  double ��ص�ѹֵ ��λ V
** ��    ע:  ��
*******************************************************************************/
static int32u get_Battery_V()
{
    //int16s  adc_v;
    int16u adc_aver;
  
    halGpioConfig(PORTB_PIN(7), GPIOCFG_ANALOG);                                        //����PB7Ϊģ������
    for(int i=0;i<ADC_DATA_NUM;i++){                                                    //�����ɼ�10��
      halStartAdcConversion(1,ADC_REF_INT,(0x9|(0x2<<4)),5);                            
      halReadAdcBlocking(1,&ADCData[i]);                                                
    }
    
    //adc_v = halConvertValueToVolts(sort_average(ADCData,ADC_DATA_NUM,ADC_AVER_NUM));     	//��10��ADCֵ����ȡ�м�5����ƽ��ֵ����ת��Ϊ��ѹֵ
    //adc_v = halConvertValueToVolts(ADCData[0]);
    
    adc_aver = sort_average(ADCData,ADC_DATA_NUM,ADC_AVER_NUM);
    return adc_aver;
}

/*
*  ���������������
*/
static Bat_V_T Battery_V_Conf[]={
        //��ѹֵ*10000  //��Ӧ�������ʱ��
        {30000,       3600},              //����3.0V ÿ��1Сʱ����һ�ε�ѹ
        {27000,       1800},              //����2.7V ÿ��30���Ӳ���һ�ε�ѹ
        {23000,       600},               //����2.3V ÿ��10��վ����һ�ε�ѹ
        {22000,        0},                //����2.2V ÿ�ζ����в���
        {KEY_VALUE_END,KEY_VALUE_END}
       };

static MyEvent_Time battery_show_dely = {.s = SHOW_LOW_DELY,
										 .ms = 0			
										 };										//��ص�����ʾʱ����

extern Disp disp;
extern uint8_t help_flag;
extern Call_Card call_card;	
extern uint8_t EEPROM_Status;
extern int16_t SendFailTime;

/*******************************************************************************
** ��������:  Battery_V_Process(void)
** ��������:  ��ص�ѹ�����߳�
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  �˺�������ȫ�ֱ���bat_stat�����жϵ�ص���
              �˺�����̬���²�������
*******************************************************************************/
void Battery_V_Process()
{
	if((get_systime()>=ADC_Time)||(fist_PowerOn)){              
		battery.v = get_Battery_V();                                 		//��õ�ѹֵ
    
		if(battery.v <= BAT_LOW)                                   			//���ݵ�ѹֵ���µ��״̬
			battery.stat = LOW;
		if(battery.v >= (BAT_LOW+2000))
			battery.stat = NLOW;

		if((EEPROM_Status)&&(EEPROM_Idle))
        {
          	EEPROMOn();
          	halCommonDelayMicroseconds(1000);
          	WriteBATValue(battery.v);             							//����ص�ѹд��EEPROM
          	WriteSendFailTime(SendFailTime);      							//дzigbee����ʧ�ܴ���
        }
		
		for(int i=0;Battery_V_Conf[i].key!=KEY_VALUE_END;i++){            	//���ݵ�ǰ��ѹֵ����Battery_V_Conf�������´β���ʱ��
			if(battery.v>=Battery_V_Conf[i].key){
				ADC_Time = (get_systime() + Battery_V_Conf[i].value);
				break;
			}//end if
		}//end for
	}//end if((get_systime 

	/*������ʾ*/
	if((!help_flag)&&(call_card.stat == 0)&&(mydely_come(&battery_show_dely) >= 0)){
		if(battery.stat == LOW){											//�͵������SHOW_LOW_DELY��һ��
			set_mydely(&battery_show_dely,SHOW_LOW_DELY,0);
			disp.operation(RED_LED);
			halCommonDelayMilliseconds(20);
			disp.operation(0);
		}else{	
			set_mydely(&battery_show_dely,SHOW_NLOW_DELY,0);				//�ǵ͵����̵�SHOW_NLOW_DELY��һ��
			disp.operation(GREEN_LED);
			halCommonDelayMilliseconds(20);
			disp.operation(0);
		}	
	}
}

void battery_show_init(void)
{
	set_mydely(&battery_show_dely,SHOW_LOW_DELY,0);
}
