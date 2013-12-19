/********************************Copyright (c)**********************************
**                		
**                          			
**-File Info--------------------------------------------------------------------
** File name:			disp.c
** Last Version:		V1.0
** Descriptions:		按键以及显示相关
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


/*按键相关*/
#define BUTTON_STAT ((GPIO_PCIN&PC7)!=PC7)			//按键状态,0为非按下，非0为按下
//#define BUTTON_STAT ((GPIO_PBIN&PB6)!=PB6)

#define BUTTON_DELY	50			        			//50ms 检测一次按键状态
#define BUTTON_HOLD_TIME 3000						//按钮持续按3s则为长按键
#define BUTTON_CLICK_TIME 200						//按钮持续按200ms则为短按键
CREATE_DELY_OBJ(button_check_time,0,BUTTON_DELY)	//按键检测时间间隔对象实例


struct button 	button1 = 	{
								.stat = NO,
								.hold_time = 0,
							};						//按键对象实例
Disp disp = {
				.stat = 0,
				.operation = disp_operation,
				.disable = disable_display,
				.enable = enable_display,
			};
void halCommonDelayMilliseconds(int16u ms);
/*******************************************************************************
** 函数名称:  void button_init (void)
** 函数功能:  按键相关初始化
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void button_init(void)//未用
{	
	//......端口引脚配置
	GPIO_PCCFGH_REG |= PC7_CFG;			        //配置PA7为上拉输入模式 按钮
    GPIO_PCCFGH_REG &= ((~PC7_CFG)|(GPIOCFG_IN_PUD<<PC7_CFG_BIT));
    GPIO_PCOUT_REG |= (GPIOOUT_PULLUP<<PC7_BIT);
	set_mydely(&button_check_time,0,BUTTON_DELY);	//按键检测时间间隔设置
	button1.stat = NO;								//按键实例初始化
	button1.hold_time = 0;							
}

int32u button_test;
/*******************************************************************************
** 函数名称:  void button_poll (void)
** 函数功能:  按键检测
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void button_poll(void)
{
    
	/*按键检测*/
	int32_t dt = mydely_come(&button_check_time);
	if((dt>=0)){
		set_mydely(&button_check_time,0,BUTTON_DELY);										//BUTTON_DELY时间检测一次按键状态
		button_test = (GPIO_PCIN&PC7);
                if(BUTTON_STAT){
				button1.hold_time += (BUTTON_DELY+(uint32_t)dt);
		}else{
			button1.hold_time = 0;
		}
	}

	/*按键状态判断*/
	if((button1.hold_time >= BUTTON_CLICK_TIME)&&(button1.hold_time < BUTTON_HOLD_TIME)){ 	//短按键按住事件
		button1.stat = CLICK_PRESS;
	}
	else if((button1.stat != LONG_PRESS_LISTEN)&&(button1.hold_time >= BUTTON_HOLD_TIME)){	//长按键按住事件										//长按键按下
		button1.stat = LONG_PRESS;												
	}
	else if(button1.hold_time == 0){
		if(button1.stat == CLICK_PRESS)														//点击事件(短按键松开)
			button1.stat = CLICK;
		else if((button1.stat == LONG_PRESS)||(button1.stat == LONG_PRESS_LISTEN))			//长按释放事件(长按键松开)										//长按键松开
			button1.stat = LONG_PRESS_RELEASE;
		else if(button1.stat == NO){}else{}													//无按键
	}else{}
	
}

/*******************************************************************************
** 函数名称:  listen_button
** 函数功能:  监听指定按键的指定状态
** 入口参数:  button为监听按键对象，stat为要监听的状态
** 出口参数:  无
** 备    注:  无
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
** 函数名称:  disp_operation
** 函数功能:  操作外围显示
** 入口参数:  RED_LED/GREEN_LED/FM/MOTOR，中的组合
** 出口参数:  无
** 备    注:  无
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
** 函数名称:  void disable_display()
** 函数功能:  屏蔽所有显示，与enable_display成对使用
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
static uint8_t display_stat;
static void disable_display(void)
{
	display_stat = disp.stat;
	disp.operation(0);						//关闭所有显示
}

/*******************************************************************************
** 函数名称:  void enable_display()
** 函数功能:  恢复显示,与disable_display成对使用
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
static void enable_display(void)
{
	disp.operation(display_stat);			//恢复显示
}

/*******************************************************************************
** 函数名称:  disp_init
** 函数功能:  显示初始化
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/

void disp_init(void)
{
	disp.stat = 0;
	disp.operation = disp_operation;
	disp.enable = enable_display;
	disp.disable = disable_display;
}


/*******************************************************************************
** 函数名称:  void LEDBlink()
** 函数功能:  双色灯红、绿顺序闪烁
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
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
** 函数名称:  void RingForSuccess()
** 函数功能:  蜂鸣器蜂鸣表示初始化成功
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
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

