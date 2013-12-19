/********************************Copyright (c)**********************************
**                			
**                          			
**-File Info--------------------------------------------------------------------
** File name:			commu.c
** Last Version:		V1.0
** Descriptions:		˫��ͨѶ
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
MyByte8T comm_successed = 0;						//����ͨѶ��־
uint8_t help_successed = 0;							//�����ɹ���־
static MyByte8T switch_flag = 0;					//�Ʒ����������л���־
uint8_t help_flag = 0;	
#define CALL_DELY	200								//���Ϻ���״̬��CALL_DELY ms�Ʒ���������һ��
CREATE_DELY_OBJ(call_card_dely,0,CALL_DELY)			//ʵ�������Ϻ��к�(�ơ�������)���Ѽ��

Call_Card call_card = {.stat = NO_CALL,};			//���Ϻ��ж���ʵ��
											
uint8_t call_com[CALL_LEN]={0x88,0x99,0x99,0x99,0xff,0x00,0x00,0x00,0x00};

void halCommonDelayMilliseconds(int16u ms);
/*******************************************************************************
** ��������:   arry_commp
** ��������:  �Ƚ���������
** ��ڲ���:  a b ΪҪ�Ƚϵ����飬lenΪҪ�Ƚϵĳ���
** ���ڲ���:   
** ��    ע:  ��
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

uint8_t updata_dest_addr = 0;								//�Ƿ���Ҫ����Ŀ�ĵ�ַ��־
extern appMemT appM;
extern MyByte8T buff_rx[];
extern uint8_t know_addr;

/*******************************************************************************
** ��������:   parse_rev_com
** ��������:  ���� �������
** ��ڲ���:  ԴMAC��ַ��0�ֽڣ�data_len���յ����ݵĳ���
** ���ڲ���:   TRUE��ʾ��������Ϊ���FALSE��ʾ�������ķ�����
** ��    ע:  ��
*******************************************************************************/
MyBoolT parse_rev_com(uint8_t src,uint8_t data_len)
{	
	
	if((src == appM.dest[0])&&(data_len == CALL_LEN)&&arry_commp(call_com,buff_rx,5)){
		comm_successed = 1;									//����ͨѶ��־��λ
		switch(buff_rx[CALL_LEN-1]){
			case 0x21:										//���Ϻ�������
				call_card.stat = SUCCESS_CALL;
				break;
			case 0x11:										//���ȷ������
				help_successed = 1;
				break;
			case 0xFF:										//����������վӦ������
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
** ��������:   call_card_ack
** ��������:   ���;��Ϻ���ȷ������
** ��ڲ���:   ��
** ���ڲ���:   ��
** ��    ע:  ��
*******************************************************************************/
static void call_card_ack(void)
{
	call_card.stat = ACK_CALL;									//���ͺ���ȷ������
	data_buff[CALL_LEN-4] = 0x21;                            
	data_buff[CALL_LEN-3] = (MyByte8T)distan1;              	//����
	data_buff[CALL_LEN-2] = (MyByte8T)(distan1>>8);    
	data_buff[CALL_LEN-1] = (0x00|(battery.stat<<5));       	//��ʾ��״̬    0000 0000���� 0010 0000���� 1100 0000����
	//NTRXSetRegister( NA_TxArq, FALSE);
	NTRXSendMessage (PacketTypeData,app->dest, data_buff, CALL_LEN);	//����ͨѶȷ������
}

/*******************************************************************************
** ��������:   com_ack
** ��������:   Ӧ��
** ��ڲ���:   ��
** ���ڲ���:   ��
** ��    ע:  ��
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
** ��������:  void call_card_process (void)
** ��������:  ���Ϻ���/�����������ô���
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
static void call_card_process(void)
{															//���Ϻ���/������������
	if(call_card.stat || UpperCommander || WriteFactoryDateFlag){					

		
		if(WriteFactoryDateFlag==TRUE){						//������������
			if((EEPROM_Status)&&(EEPROM_Idle)){
				WriteFactDate(Factory_Date);
	          	if(Factory_Date!=ReadFactDate()){ 
					factory_date_succed = 0;
	            	disp.operation((RED_LED|FM|MOTOR)); 	//�յ����δд��ɹ������
	          	}else{
	          		factory_date_succed = 1;
	            	disp.operation((GREEN_LED|FM|MOTOR)); 	//�ɹ�д���̵���
	          	}
			}else{
				factory_date_succed = 0;
				disp.operation((RED_LED|FM|MOTOR));         //�յ����δд��ɹ������
			}
      		
			
		}else{												//���Ϻ�������
			if(mydely_come(&call_card_dely) >= 0){			//ÿ��CALL_DELYʱ����˸��ʾһ��
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

		/*����ȷ��*/
		if(TRUE == listen_button(&button1,LONG_PRESS)){		//����������
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
          	RecvSuccessTime++;                 				//ȡ�����¼˫��ͨ�Ŵ���
          	if((EEPROM_Status)&&(EEPROM_Idle)){
          		WriteRecvSuccessTime(RecvSuccessTime);
          	}
		}
	}
															//ȡ����������
	if((FALSE == UpperCommander)&&(NO_CALL == call_card.stat)&&(WriteFactoryDateFlag == FALSE)&&(!help_flag)){
		call_card.stat= NO_CALL;
		UpperCommander = FALSE;
		switch_flag = 0;
		disp.operation(0);
		
	}
}


									//������־

//#define HELP_BUTTON_HOLD_TIME	5000						//���а�����Ч����ʱ��
//#define HELP_DELY				500							//ÿ��500ms����һ������
//static MyEvent_Time help_dely = {
//									.s = 0,
//									.ms = HELP_DELY,
//								};							//��������ʱ��������
/*
void help_init(void)
{
	  set_mydely(&help_dely,0,HELP_DELY);						//����ʱ,����������ʱ����
}
*/
//uint8_t disp_test;

static uint32_t 	help_time;								//��������ʱ��,��ʱʱ���ж�
static uint8_t 		help_time_init_flag;					//
#define		HELP_TIMEOUT			35						//��ʱHELP_TIMEOUT���ȡ������
/*******************************************************************************
** ��������:  void help_up (void)
** ��������:  ����
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
static void help_up(void)
{
	uint8_t i;
	if(TRUE == listen_button(&button1,LONG_PRESS)){			//����������
		if(help_flag) help_flag = 0;						//������ȡ������
		else 		  help_flag = 1;
	}
	
    if(help_flag){	
		if(help_successed){									//�����ɹ���ȡ������,�̵Ʒ�������3��
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
			if(get_systime() >=help_time ){					//��������ʱ,ȡ������
				help_time_init_flag = 0;
				help_flag = 0;
				help_successed = 0;
				disp.operation(0);
			}else{
				disp.operation(RED_LED|FM|MOTOR);      		//��������δ�յ�ȷ�ϻ�δȡ������,��Ʒ�����һֱ��
			} 
		}					                        
	}else{
		disp.operation(0);
		help_successed = 0;
		help_time_init_flag = 0;
	}								
		
	
}


/*******************************************************************************
** ��������:  void communication (void)
** ��������:  ˫��ͨѶ
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
#define COMM_TIME 30										//����30����
void communication(void)
{	
	uint16_t i;

	zigbee_rcv_init();										//zigbee���ճ�ʼ��
		
    for(i = 0; i<COMM_TIME; i++){                           //����
      if( TRUE == NTRXUpdate()) 	break;					//nano������߽���
	  if(zigbee_is_rcved())			break;					//zigbee�����ж�
    }
	zigbee_rcv_process();									//zigbee���մ���
	
  	button_poll();											//������ѯ
  	call_card_process();									//�����Ϻ���/������������
  	if(!(call_card.stat||UpperCommander||WriteFactoryDateFlag)){
  		help_up();											//��������
  	} 
}

