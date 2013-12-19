/********************************Copyright (c)**********************************
**                			
**                          				
**-File Info--------------------------------------------------------------------
** File name:			app_adc.c
** Last Version:		V1.0
** Descriptions:		电池管理
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
static  int16u    ADCData[ADC_DATA_NUM];            //保存采集到的ADC数据
        Bat       battery = {.stat = NLOW,
							 .v = 0,
							};                          //电池信息
static  uint32_t  ADC_Time;                         //ADC采样时间
/*******************************************************************************
** 函数名称:  int16u sort_average(void)
** 函数功能:  冒泡排序并求平均值
** 入口参数:  adcData_sort  要排序的数组
              lenght        数组长度
              num           排序后中间多少值的平均
** 出口参数:  int16u 排序后的平均值
** 备    注:  无
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
** 函数名称:  double get_Battery_V(void)
** 函数功能:  获取电池电压
** 入口参数:  无
** 出口参数:  double 电池电压值 单位 V
** 备    注:  无
*******************************************************************************/
static int32u get_Battery_V()
{
    //int16s  adc_v;
    int16u adc_aver;
  
    halGpioConfig(PORTB_PIN(7), GPIOCFG_ANALOG);                                        //配置PB7为模拟输入
    for(int i=0;i<ADC_DATA_NUM;i++){                                                    //连续采集10次
      halStartAdcConversion(1,ADC_REF_INT,(0x9|(0x2<<4)),5);                            
      halReadAdcBlocking(1,&ADCData[i]);                                                
    }
    
    //adc_v = halConvertValueToVolts(sort_average(ADCData,ADC_DATA_NUM,ADC_AVER_NUM));     	//将10个ADC值排序，取中间5个的平均值，并转换为电压值
    //adc_v = halConvertValueToVolts(ADCData[0]);
    
    adc_aver = sort_average(ADCData,ADC_DATA_NUM,ADC_AVER_NUM);
    return adc_aver;
}

/*
*  电池特性曲线配置
*/
static Bat_V_T Battery_V_Conf[]={
        //电压值*10000  //对应采样间隔时间
        {30000,       3600},              //大于3.0V 每隔1小时采样一次电压
        {27000,       1800},              //大于2.7V 每隔30分钟采样一次电压
        {23000,       600},               //大于2.3V 每隔10分站采样一次电压
        {22000,        0},                //大于2.2V 每次都进行采样
        {KEY_VALUE_END,KEY_VALUE_END}
       };

static MyEvent_Time battery_show_dely = {.s = SHOW_LOW_DELY,
										 .ms = 0			
										 };										//电池电量显示时间间隔

extern Disp disp;
extern uint8_t help_flag;
extern Call_Card call_card;	
extern uint8_t EEPROM_Status;
extern int16_t SendFailTime;

/*******************************************************************************
** 函数名称:  Battery_V_Process(void)
** 函数功能:  电池电压处理线程
** 入口参数:  无
** 出口参数:  无
** 备    注:  此函数更新全局变量bat_stat用于判断电池电量
              此函数动态更新采样周期
*******************************************************************************/
void Battery_V_Process()
{
	if((get_systime()>=ADC_Time)||(fist_PowerOn)){              
		battery.v = get_Battery_V();                                 		//获得电压值
    
		if(battery.v <= BAT_LOW)                                   			//根据电压值更新电池状态
			battery.stat = LOW;
		if(battery.v >= (BAT_LOW+2000))
			battery.stat = NLOW;

		if((EEPROM_Status)&&(EEPROM_Idle))
        {
          	EEPROMOn();
          	halCommonDelayMicroseconds(1000);
          	WriteBATValue(battery.v);             							//将电池电压写到EEPROM
          	WriteSendFailTime(SendFailTime);      							//写zigbee发送失败次数
        }
		
		for(int i=0;Battery_V_Conf[i].key!=KEY_VALUE_END;i++){            	//根据当前电压值查找Battery_V_Conf表，设置下次采样时间
			if(battery.v>=Battery_V_Conf[i].key){
				ADC_Time = (get_systime() + Battery_V_Conf[i].value);
				break;
			}//end if
		}//end for
	}//end if((get_systime 

	/*电量显示*/
	if((!help_flag)&&(call_card.stat == 0)&&(mydely_come(&battery_show_dely) >= 0)){
		if(battery.stat == LOW){											//低电量红灯SHOW_LOW_DELY闪一次
			set_mydely(&battery_show_dely,SHOW_LOW_DELY,0);
			disp.operation(RED_LED);
			halCommonDelayMilliseconds(20);
			disp.operation(0);
		}else{	
			set_mydely(&battery_show_dely,SHOW_NLOW_DELY,0);				//非低电量绿灯SHOW_NLOW_DELY闪一次
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
