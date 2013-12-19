/********************************Copyright (c)**********************************
**                			
**                          			
**-File Info--------------------------------------------------------------------
** File name:			commu.c
** Last Version:		V1.0
** Descriptions:		双向通讯
**------------------------------------------------------------------------------
** Created by:	QI
** Created date:     	2013-08-12
** Version:	       	V1.0
** Descriptions:    	
*******************************************************************************/

#include    "iar.h"
#include	"nano/ntrxdil.h"
#include	"nano/ntrxutil.h"
#include	"App.h"
#include	"disp.h"
#include    "sys_time.h"
#include	"commu.h"
#include 	"app_adc.h"
#include	"disp.h"
#include	"System.h"
#include	"zigbee.h"
#include	"rfid.h"


uint8_t button_cnt = 0;
uint8_t button_flag = 0;
MyByte8T comm_successed = 0;						//命令通讯标志
uint8_t help_successed = 0;							//求助成功标志
static MyByte8T switch_flag = 0;					//灯蜂鸣器开关切换标志
uint8_t help_flag = 0;	
#define CALL_DELY	200								//井上呼叫状态下CALL_DELY ms灯蜂鸣器提醒一次
CREATE_DELY_OBJ(call_card_dely,0,CALL_DELY)			//实例化井上呼叫后(灯、蜂鸣器)提醒间隔

Call_Card call_card = {.stat = NO_CALL,};			//井上呼叫对象实例
											
uint8_t call_com[CALL_LEN]={0x88,0x99,0x99,0x99,0xff,0x00,0x00,0x00,0x00};

void halCommonDelayMilliseconds(int16u ms);
/*******************************************************************************
** 函数名称:   arry_commp
** 函数功能:  比较两个数组
** 入口参数:  a b 为要比较的数组，len为要比较的长度
** 出口参数:   
** 备    注:  无
*******************************************************************************/
MyBoolT arry_commp(uint8_t *a,uint8_t *b,uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;){
		if((*(a+i))!=(*(b+i)))
			break;
		i++;
	}
	if(i == len)
		return TRUE;
	else
		return FALSE;
}

uint8_t updata_dest_addr = 0;								//是否需要更新目的地址标志
extern appMemT appM;
extern MyByte8T buff_rx[];
extern uint8_t know_addr;

/*******************************************************************************
** 函数名称:   parse_rev_com
** 函数功能:  井上 命令解析
** 入口参数:  源MAC地址第0字节，data_len接收到数据的长度
** 出口参数:   TRUE表示发送来的为命令，FALSE表示发送来的非命令
** 备    注:  无
*******************************************************************************/
MyBoolT parse_rev_com(uint8_t src,uint8_t data_len)
{	
	
	if((src == appM.dest[0])&&(data_len == CALL_LEN)&&arry_commp(call_com,buff_rx,5)){
		comm_successed = 1;									//命令通讯标志置位
		switch(buff_rx[CALL_LEN-1]){
			case 0x21:										//井上呼叫命令
				call_card.stat = SUCCESS_CALL;
				break;
			case 0x11:										//求救确认命令
				help_successed = 1;
				break;
			case 0xFF:										//搜索附近分站应答命令
				know_addr = 1;
				updata_dest_addr = 1;
			default:
				break;
		}
		return TRUE;
	}
	
	return FALSE;
}

extern unsigned char data_buff[];
extern unsigned short distan1;
extern Bat       battery;
extern appMemT *app; 

/*******************************************************************************
** 函数名称:   call_card_ack
** 函数功能:   回送井上呼叫确认命令
** 入口参数:   无
** 出口参数:   无
** 备    注:  无
*******************************************************************************/
static void call_card_ack(void)
{
	call_card.stat = ACK_CALL;									//回送呼叫确认命令
	data_buff[CALL_LEN-4] = 0x21;                            
	data_buff[CALL_LEN-3] = (MyByte8T)distan1;              	//距离
	data_buff[CALL_LEN-2] = (MyByte8T)(distan1>>8);    
	data_buff[CALL_LEN-1] = (0x00|(battery.stat<<5));       	//标示卡状态    0000 0000正常 0010 0000电量 1100 0000求助
	//NTRXSetRegister( NA_TxArq, FALSE);
	NTRXSendMessage (PacketTypeData,app->dest, data_buff, CALL_LEN);	//发送通讯确认命令
}

/*******************************************************************************
** 函数名称:   com_ack
** 函数功能:   应答
** 入口参数:   无
** 出口参数:   无
** 备    注:  无
*******************************************************************************/
void com_ack(void)
{
	if(call_card.stat == SUCCESS_CALL){
		call_card_ack();
	}
}

uint8_t factory_date_succed = 0;
extern Disp disp;
extern Button 	button1;
extern boolean UpperCommander;
extern boolean WriteFactoryDateFlag;
extern uint16_t Factory_Date;
extern uint8_t EEPROM_Status;
extern uint16_t RecvSuccessTime;

/*******************************************************************************
** 函数名称:  void call_card_process (void)
** 函数功能:  井上呼叫/出厂日期设置处理
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
static void call_card_process(void)
{															//井上呼叫/出厂日期命令
	if(call_card.stat || UpperCommander || WriteFactoryDateFlag){					

		
		if(WriteFactoryDateFlag==TRUE){						//出厂日期命令
			if((EEPROM_Status)&&(EEPROM_Idle)){
				WriteFactDate(Factory_Date);
	          	if(Factory_Date!=ReadFactDate()){ 
					factory_date_succed = 0;
	            	disp.operation((RED_LED|FM|MOTOR)); 	//收到命令但未写入成功红灯亮
	          	}else{
	          		factory_date_succed = 1;
	            	disp.operation((GREEN_LED|FM|MOTOR)); 	//成功写入绿灯亮
	          	}
			}else{
				factory_date_succed = 0;
				disp.operation((RED_LED|FM|MOTOR));         //收到命令但未写入成功红灯亮
			}
      		
			
		}else{												//井上呼叫命令
			if(mydely_come(&call_card_dely) >= 0){			//每隔CALL_DELY时间闪烁提示一次
				set_mydely(&call_card_dely,0,CALL_DELY);    	
				if(switch_flag){
					switch_flag = ~switch_flag;
					disp.operation(0);
				}else{
					switch_flag = ~switch_flag;
					disp.operation((RED_LED|FM|MOTOR));		
				}	
			}
		}

		/*按键确认*/
		if(TRUE == listen_button(&button1,LONG_PRESS)){		//监听长按键
			call_card.stat= NO_CALL;
			UpperCommander = FALSE;
			switch_flag = 0;
			disp.operation(0);
			if(WriteFactoryDateFlag==TRUE){
				WriteFactoryDateFlag = FALSE;
				if((EEPROM_Status)&&(EEPROM_Idle)){
					EEPROMOn();
	            	WriteResetTime(SOFT_RESET,0);
	            	WriteResetTime(WATCHDOG_RESET,0);
	            	WriteResetTime(POWER_RESET,0);
	            	EEPROMOff();
				}
          	}          
          	RecvSuccessTime++;                 				//取消后记录双向通信次数
          	if((EEPROM_Status)&&(EEPROM_Idle)){
          		WriteRecvSuccessTime(RecvSuccessTime);
          	}
		}
	}
															//取消呼叫命令
	if((FALSE == UpperCommander)&&(NO_CALL == call_card.stat)&&(WriteFactoryDateFlag == FALSE)&&(!help_flag)){
		call_card.stat= NO_CALL;
		UpperCommander = FALSE;
		switch_flag = 0;
		disp.operation(0);
		
	}
}


									//求助标志

//#define HELP_BUTTON_HOLD_TIME	5000						//呼叫按键有效持续时间
//#define HELP_DELY				500							//每隔500ms发送一次求助
//static MyEvent_Time help_dely = {
//									.s = 0,
//									.ms = HELP_DELY,
//								};							//求助发送时间间隔对象
/*
void help_init(void)
{
	  set_mydely(&help_dely,0,HELP_DELY);						//求助时,发送求助的时间间隔
}
*/
//uint8_t disp_test;

static uint32_t 	help_time;								//求助持续时间,超时时间判断
static uint8_t 		help_time_init_flag;					//
#define		HELP_TIMEOUT			35						//超时HELP_TIMEOUT秒后取消求助
/*******************************************************************************
** 函数名称:  void help_up (void)
** 函数功能:  求助
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
static void help_up(void)
{
	uint8_t i;
	if(TRUE == listen_button(&button1,LONG_PRESS)){			//监听长按键
		if(help_flag) help_flag = 0;						//求助或取消求助
		else 		  help_flag = 1;
	}
	
    if(help_flag){	
		if(help_successed){									//求助成功或取消求助,绿灯蜂鸣器闪3次
            help_flag = 0;
			help_successed = 0;
			disp.operation(0);
			for(i=0;i<10;i++){
                disp.operation(GREEN_LED|FM|MOTOR);
				halCommonDelayMilliseconds(300); 
				disp.operation(0);
  				halCommonDelayMilliseconds(300);
  				
			}
		}else{
			if(!help_time_init_flag){
				help_time_init_flag = 1;
				help_time = (get_systime() + HELP_TIMEOUT);
			}
			if(get_systime() >=help_time ){					//若求助超时,取消求助
				help_time_init_flag = 0;
				help_flag = 0;
				help_successed = 0;
				disp.operation(0);
			}else{
				disp.operation(RED_LED|FM|MOTOR);      		//已求助但未收到确认或未取消求助,红灯蜂鸣器一直响
			} 
		}					                        
	}else{
		disp.operation(0);
		help_successed = 0;
		help_time_init_flag = 0;
	}								
		
	
}


/*******************************************************************************
** 函数名称:  void communication (void)
** 函数功能:  双向通讯
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
#define COMM_TIME 30										//大于30即可
void communication(void)
{	
	uint16_t i;

	zigbee_rcv_init();										//zigbee接收初始化
		
    for(i = 0; i<COMM_TIME; i++){                           //接收
      if( TRUE == NTRXUpdate()) 	break;					//nano测距无线接收
	  if(zigbee_is_rcved())			break;					//zigbee接收判断
    }
	zigbee_rcv_process();									//zigbee接收处理
	
  	button_poll();											//按键轮询
  	call_card_process();									//处理井上呼叫/出厂日期命令
  	if(!(call_card.stat||UpperCommander||WriteFactoryDateFlag)){
  		help_up();											//求助处理
  	} 
}

