/********************************Copyright (c)**********************************
**                		
**                          				
**-File Info--------------------------------------------------------------------
** File name:			conf.h
** Last Version:		V1.0
** Descriptions:		常用配置
**------------------------------------------------------------------------------
** Created by:	 	QI
** Created date:     	2013-08-12
** Version:	       	V1.0
** Descriptions:    	
*******************************************************************************/


#ifndef __CONF_H__
#define __CONF_H__


//电压采集配置
#define 	ADC_DATA_NUM 	10 		  			//连续采集多少个ADC值
#define 	ADC_AVER_NUM	5		  			//取排序后中间多少个值的平均
#define 	V_MUL	        3.147		  		//电压的放大系数(值由R17 R18的电阻决定（R17+R18)/R18 )
#define 	BAT_LOW	        22000             	//电池电压低电量阀值 V*10000
#define SHOW_LOW_DELY 		3					//低电量显示时间间隔s
#define SHOW_NLOW_DELY 		30					//非低电量显示时间间隔s

//休眠配置
#define         STANDARD_CNT    5                 //测距连续失败次数小于STANDARD_CNT进入正常休眠
#define         STANDARD_T      3000              //正常休眠时间  单位ms
#define         MAXFALSE_CNT    1200              //测距连续失败MAXFALSE_CNT次进入长休眠
#define         LONG_T          3000//10000       		//长休眠时间    单位ms
#define         MID_T           3000//6000        	//中等长度休眠时间  单位ms
#define         DIFF_D          3                 //当前距离与历史距离差值的阀值
#define         DIFF_D_CNT      2                 //连续DIFF_D_CNT次测距都在DIFF_D范围内则进行中等长度休眠


#define         POLL_NUM        4                 //轮询分站数

#define         LOCATION_TIME   20                //定位时间间隔 单位秒

#define 		SUB_D			10				  //子母分站间距离
#endif