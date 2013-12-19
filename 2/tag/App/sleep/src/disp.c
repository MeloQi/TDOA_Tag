/********************************Copyright (c)**********************************
**                		
**                          			
**-File Info--------------------------------------------------------------------
** File name:			disp.c
** Last Version:		V1.0
** Descriptions:		�����Լ���ʾ���
**------------------------------------------------------------------------------
** Created by:	 	QI
** Created date:     	2013-08-12
** Version:	       	V1.0
** Descriptions:    	
*******************************************************************************/


#include        "iar.h"
#include		"nano/ntrxdil.h"
#include        "disp.h"
#include        "sys_time.h"

void disp_operation(uint8_t opt);
void disable_display(void);
void enable_display(void);


/*�������*/
#define BUTTON_STAT ((GPIO_PCIN&PC7)!=PC7)			//����״̬,0Ϊ�ǰ��£���0Ϊ����
//#define BUTTON_STAT ((GPIO_PBIN&PB6)!=PB6)

#define BUTTON_DELY	50			        			//50ms ���һ�ΰ���״̬
#define BUTTON_HOLD_TIME 3000						//��ť������3s��Ϊ������
#define BUTTON_CLICK_TIME 200						//��ť������200ms��Ϊ�̰���
CREATE_DELY_OBJ(button_check_time,0,BUTTON_DELY)	//�������ʱ��������ʵ��


struct button 	button1 = 	{
								.stat = NO,
								.hold_time = 0,
							};						//��������ʵ��
Disp disp = {
				.stat = 0,
				.operation = disp_operation,
				.disable = disable_display,
				.enable = enable_display,
			};
void halCommonDelayMilliseconds(int16u ms);
/*******************************************************************************
** ��������:  void button_init (void)
** ��������:  ������س�ʼ��
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void button_init(void)//δ��
{	
	//......�˿���������
	GPIO_PCCFGH_REG |= PC7_CFG;			        //����PA7Ϊ��������ģʽ ��ť
    GPIO_PCCFGH_REG &= ((~PC7_CFG)|(GPIOCFG_IN_PUD<<PC7_CFG_BIT));
    GPIO_PCOUT_REG |= (GPIOOUT_PULLUP<<PC7_BIT);
	set_mydely(&button_check_time,0,BUTTON_DELY);	//�������ʱ��������
	button1.stat = NO;								//����ʵ����ʼ��
	button1.hold_time = 0;							
}

int32u button_test;
/*******************************************************************************
** ��������:  void button_poll (void)
** ��������:  �������
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void button_poll(void)
{
    
	/*�������*/
	int32_t dt = mydely_come(&button_check_time);
	if((dt>=0)){
		set_mydely(&button_check_time,0,BUTTON_DELY);										//BUTTON_DELYʱ����һ�ΰ���״̬
		button_test = (GPIO_PCIN&PC7);
                if(BUTTON_STAT){
				button1.hold_time += (BUTTON_DELY+(uint32_t)dt);
		}else{
			button1.hold_time = 0;
		}
	}

	/*����״̬�ж�*/
	if((button1.hold_time >= BUTTON_CLICK_TIME)&&(button1.hold_time < BUTTON_HOLD_TIME)){ 	//�̰�����ס�¼�
		button1.stat = CLICK_PRESS;
	}
	else if((button1.stat != LONG_PRESS_LISTEN)&&(button1.hold_time >= BUTTON_HOLD_TIME)){	//��������ס�¼�										//����������
		button1.stat = LONG_PRESS;												
	}
	else if(button1.hold_time == 0){
		if(button1.stat == CLICK_PRESS)														//����¼�(�̰����ɿ�)
			button1.stat = CLICK;
		else if((button1.stat == LONG_PRESS)||(button1.stat == LONG_PRESS_LISTEN))			//�����ͷ��¼�(�������ɿ�)										//�������ɿ�
			button1.stat = LONG_PRESS_RELEASE;
		else if(button1.stat == NO){}else{}													//�ް���
	}else{}
	
}

/*******************************************************************************
** ��������:  listen_button
** ��������:  ����ָ��������ָ��״̬
** ��ڲ���:  buttonΪ������������statΪҪ������״̬
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
uint8_t listen_button(Button *b,Button_Stat stat)
{
	if(b->stat == stat){
		if(stat == LONG_PRESS){
			b->stat = LONG_PRESS_LISTEN;
		}else{
			//button.hold_time = 0;
			//button.stat = NO;
		}
		
		return TRUE;
	}
	
	return FALSE;
}





/*******************************************************************************
** ��������:  disp_operation
** ��������:  ������Χ��ʾ
** ��ڲ���:  RED_LED/GREEN_LED/FM/MOTOR���е����
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
static void disp_operation(uint8_t opt)
{	
	if((opt&RED_LED) != (disp.stat&RED_LED)){
		if(opt&RED_LED) RedLedOn(); else RedLedOff();
	}
	if((opt&GREEN_LED) != (disp.stat&GREEN_LED)){
		if(opt&GREEN_LED) GreenLedOn(); else GreenLedOff();
	}
	if((opt&FM) != (disp.stat&FM)){
		if(opt&FM) FMQOn(); else FMQOff();
	}
	if((opt&MOTOR) != (disp.stat&MOTOR)){
		if(opt&MOTOR) MOTOROn(); else MOTOROff();
	}
	
}

/*******************************************************************************
** ��������:  void disable_display()
** ��������:  ����������ʾ����enable_display�ɶ�ʹ��
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
static uint8_t display_stat;
static void disable_display(void)
{
	display_stat = disp.stat;
	disp.operation(0);						//�ر�������ʾ
}

/*******************************************************************************
** ��������:  void enable_display()
** ��������:  �ָ���ʾ,��disable_display�ɶ�ʹ��
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
static void enable_display(void)
{
	disp.operation(display_stat);			//�ָ���ʾ
}

/*******************************************************************************
** ��������:  disp_init
** ��������:  ��ʾ��ʼ��
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/

void disp_init(void)
{
	disp.stat = 0;
	disp.operation = disp_operation;
	disp.enable = enable_display;
	disp.disable = disable_display;
}


/*******************************************************************************
** ��������:  void LEDBlink()
** ��������:  ˫ɫ�ƺ졢��˳����˸
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void LEDBlink()
{
  for(int i=0;i<3;i++){
  	disp.operation(RED_LED);
  	halCommonDelayMilliseconds(50);
	disp.operation(0);
        disp.operation(MOTOR);
	halCommonDelayMilliseconds(50);
	disp.operation(GREEN_LED);
	halCommonDelayMilliseconds(50);
	disp.operation(0);
	halCommonDelayMilliseconds(50);
		/*
	    RedLedOn();
	    halCommonDelayMilliseconds(50);
	    RedLedOff();
	    halCommonDelayMilliseconds(50);  
	    GreenLedOn();
	    halCommonDelayMilliseconds(50);
	    GreenLedOff();
	    halCommonDelayMilliseconds(50);
	    */
  }
  
}

/*******************************************************************************
** ��������:  void RingForSuccess()
** ��������:  ������������ʾ��ʼ���ɹ�
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void RingForSuccess()
{
	disp.operation(FM);
	halCommonDelayMilliseconds(50);
	disp.operation(0);
	/*
  FMQOn();
  halCommonDelayMilliseconds(50);
  FMQOff();
  */
}

