/********************************Copyright (c)**********************************
**                		
**                          				
**-File Info--------------------------------------------------------------------
** File name:			conf.h
** Last Version:		V1.0
** Descriptions:		��������
**------------------------------------------------------------------------------
** Created by:	 	QI
** Created date:     	2013-08-12
** Version:	       	V1.0
** Descriptions:    	
*******************************************************************************/


#ifndef __CONF_H__
#define __CONF_H__


//��ѹ�ɼ�����
#define 	ADC_DATA_NUM 	10 		  			//�����ɼ����ٸ�ADCֵ
#define 	ADC_AVER_NUM	5		  			//ȡ������м���ٸ�ֵ��ƽ��
#define 	V_MUL	        3.147		  		//��ѹ�ķŴ�ϵ��(ֵ��R17 R18�ĵ��������R17+R18)/R18 )
#define 	BAT_LOW	        22000             	//��ص�ѹ�͵�����ֵ V*10000
#define SHOW_LOW_DELY 		3					//�͵�����ʾʱ����s
#define SHOW_NLOW_DELY 		30					//�ǵ͵�����ʾʱ����s

//��������
#define         STANDARD_CNT    5                 //�������ʧ�ܴ���С��STANDARD_CNT������������
#define         STANDARD_T      3000              //��������ʱ��  ��λms
#define         MAXFALSE_CNT    1200              //�������ʧ��MAXFALSE_CNT�ν��볤����
#define         LONG_T          3000//10000       		//������ʱ��    ��λms
#define         MID_T           3000//6000        	//�еȳ�������ʱ��  ��λms
#define         DIFF_D          3                 //��ǰ��������ʷ�����ֵ�ķ�ֵ
#define         DIFF_D_CNT      2                 //����DIFF_D_CNT�β�඼��DIFF_D��Χ��������еȳ�������


#define         POLL_NUM        4                 //��ѯ��վ��

#define         LOCATION_TIME   20                //��λʱ���� ��λ��

#define 		SUB_D			10				  //��ĸ��վ�����
#endif