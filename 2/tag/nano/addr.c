/********************************Copyright (c)**********************************
**                
**                         
**-File Info--------------------------------------------------------------------
** File name:			Addr.c
** Last Version:		V1.0
** Descriptions:		NANO 2.4G CSS��ඨλ
**------------------------------------------------------------------------------
** Created by:	 	QI
** Created date:     	2013-08-12
** Version:	       	V1.0
** Descriptions:    	
*******************************************************************************/
#include    "hal/micro/cortexm3/compiler/iar.h"
#include	"ntrxdil.h"
#include	"hwclock.h"
#include	"ntrxutil.h"
#include	"nnspi.h"
#include	"App/sleep/src/App.h"
#include	"App/sleep/src/conf.h"
#include	"App/sleep/src/app_adc.h"
#include	"App/sleep/src/sys_time.h"
#include	"App/sleep/src/startup_info.h"
#include	"App/sleep/src/commu.h"
#include	"App/sleep/src/disp.h"
#include	"App/sleep/src/zigbee.h"

//#define NEED_LOCATION									//��λ����
//#define  DIRECTION										//��������
#define  DIST_POLICY_THREE								//�������˿���
#define  DIST_POLICY_LARGE_VALUE						//��ֵ���˿���
#define  DIST_POLICY_LITTLE_VALUE						//Сֵ���˿���
#define  DIST_POLICY_HALF_VALUE							//�۰���������

extern StartUp_Info *read_startinfo;
extern  MyWord16T card_id;                              //����

appMemT appM = {                                          
  {                                                     //ԴMAC��ַ
    0xB2,                                               //ȷ��app->src[0]�����λΪ������10
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
  },
  {	                                                	//Ŀ��MAC��ַ
    0xA2,                                               //ȷ��app->dest[0]�����λΪ������10
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
  }
};
appMemT *app;                                           						//��¼Դ��ַ Ŀ�ĵ�ַ
Rang_Dist old_dist = {															//��¼��ʷ����
	.dest = {0x00,0x00,0x00,0x00,0x00,0x00},
	.dist = 0,
	.time = 0
};
Rang_Dist pre_dist = {															//��¼��һ������
	.dest = {0x00,0x00,0x00,0x00,0x00,0x00},
	.dist = 0,
	.time = 0
};

Rang_Dist cur_dist;                            									//��¼��ǰ����ʷ���Ŀ�ĵ�ַ������
MyByte8T rang_ok = 0;                                   						//���ɹ���־
unsigned char data_buff[CALL_LEN]={0x88,0x99,0x99,0x99,0xff,0x00,0x00,0x00,0x00}; //����֡�е����ݲ���
extern Bat       battery;                               						//��������Ϣ
unsigned short distan,distan1,distan2;                          				//distanΪ����ֵ*100��distan1Ϊ������վ����    , distan2 Ϊ���ӷ�վ����                
Locat_Info Location = {
	.time = 0,
	.flag = 1,
	.state = 0,
	.direction = 0,
};
extern uint8_t help_flag;

/*******************************************************************************
** ��������:   sort
** ��������:  ð������
** ��ڲ���:  sort_data  	Ҫ���������
              		   lenght        ���鳤��
** ���ڲ���:  
** ��    ע:  ��
*******************************************************************************/
void sort(unsigned short *sort_data,int8u lenght)
{
    int8u  len = (lenght-1);
    int16u temp,i,j;
    
    for(j=0;j<=len;j++){ 
      for (i=0;i<len-j;i++)
        if(sort_data[i]>sort_data[i+1]){ 
          temp=sort_data[i];
          sort_data[i]=sort_data[i+1];
          sort_data[i+1]=temp;
        }
    } 
}


#define	DIST_THREE_DT			9												//����������Чʱ��λs
#define	DIST_ARRY_SIZE			3	
static 	unsigned short 	dist_arry[DIST_ARRY_SIZE] = {0,0,0};					//������������ֵ
static 	unsigned short 	dist_arry1[DIST_ARRY_SIZE] = {0,0,0};					//��������������ֵ
static	uint8_t			three_filter_index;										//���������ѱ���������
#define	DIST_BIG_FILTER			5												//��ֵ����ϵ��
#define	DIST_LITTLE_FILTER		2												//Сֵ���˷�ֵ
uint8_t	oldD_update_flag;														//�Ƿ������ʷ������Ϣ��־
static uint8_t dist_policy_restart;												//

/*******************************************************************************
** ��������:  void dist_policy(void)
** ��������: �Բ����봦��Ĳ���
** ��ڲ���:  Dist ���ش����ľ���ֵ
** ���ڲ���:  Dist ���ش����ľ���ֵ
** ����ֵ	:  ����1��ʾ����ֵ���ã�0��ʾ����ֵ������
** ��    ע:  ��
*******************************************************************************/
static MyByte8T dist_policy(unsigned short * Dist)
{	
	unsigned short 	Dd,PDd;														//��������ֵ
	unsigned short	D;															//��ǰ����
	uint32_t 		Dt,PDt;
	uint8_t 		dir;														//��¼����(��������ֵ�����ӻ��Ǽ�С)
	uint8_t 		i;
	#define	DIST_SUB	0														
	#define	DIST_ADD	1
	
	if(old_dist.time > cur_dist.time)	goto dist_policy_end;
	if((old_dist.time == 0)||(pre_dist.time == 0))	goto dist_policy_end;		//��һ�β��ֵ��������;���β�����ϴβ���վ��ͬ��������
	if((old_dist.dest[5] != cur_dist.dest[5])||(old_dist.dest[4] != cur_dist.dest[4])){
		three_filter_index = 0;
		goto dist_policy_end;	
	}
	
	if((pre_dist.dest[5] != cur_dist.dest[5])||(pre_dist.dest[4] != cur_dist.dest[4])){
		three_filter_index = 0;
		goto dist_policy_end;	
	}
	
	if(dist_policy_restart){													//�������봦��									
		dist_policy_restart = 0;
		goto dist_policy_end;
	}

	Dt = cur_dist.time - old_dist.time; 										//��ʷ����һ���뵱ǰʱ���
	PDt = cur_dist.time - pre_dist.time;
	D = cur_dist.dist;
	
#ifdef 	DIST_POLICY_THREE
	if(PDt > DIST_THREE_DT)	three_filter_index = 0;								//��������
	if(three_filter_index < DIST_ARRY_SIZE){									
		dist_arry[three_filter_index] = cur_dist.dist;
		three_filter_index++;
	}else if(three_filter_index == DIST_ARRY_SIZE){								//���յ���������ֵ
		for(i=0;i<(DIST_ARRY_SIZE-1);i++){										//ɾ������ֵ����ֵ����
			dist_arry[i] = 	dist_arry[(i+1)];
		}
		dist_arry[(DIST_ARRY_SIZE-1)] = cur_dist.dist;
		for(i = 0;i < DIST_ARRY_SIZE;i++){										//�ҳ��������ֵ�е��м�ֵ	
			dist_arry1[i] = dist_arry[i];
		}
		sort(dist_arry1,DIST_ARRY_SIZE);
		D = dist_arry1[(DIST_ARRY_SIZE/2)];
	}else{three_filter_index = 0;}
#endif
																				//��ʷ�͵�ǰ�����
	if(old_dist.dist > D) 				{Dd = old_dist.dist - D; dir = DIST_SUB;}
	else							  	{Dd = D - old_dist.dist; dir = DIST_ADD;}
/*
	if(pre_dist.dist > D) 				{PDd = pre_dist.dist - D; } 						//��ֵ����
	else							  	{PDd = D - pre_dist.dist; }
	if(PDd > PDt*DIST_BIG_FILTER)		{oldD_update_flag = 0; return 0;}			
	pre_dist.dist = cur_dist.dist;														//������һ�ξ������ֵ											
	pre_dist.dest[4]= cur_dist.dest[4];
	pre_dist.dest[5]= cur_dist.dest[5];
	pre_dist.time = get_systime();
*/

#ifdef 	DIST_POLICY_LARGE_VALUE
	if(Dd > Dt*DIST_BIG_FILTER)		{oldD_update_flag = 0; return 0;}			//��ֵ����
#endif

#ifdef 	DIST_POLICY_LITTLE_VALUE
	if(Dd <= DIST_LITTLE_FILTER)	{oldD_update_flag = 0; return 0;}			//Сֵ����
#endif

#ifdef 	DIST_POLICY_HALF_VALUE
	if(dir == DIST_ADD)				{* Dist = (old_dist.dist + Dd/2);}			//�۰�����
	else if(dir == DIST_SUB)		{* Dist = (old_dist.dist - Dd/2);}
#endif

dist_policy_end:	
	cur_dist.dist = (* Dist);													//���µ�ǰ����Ϊ�������ľ���
	oldD_update_flag = 1;
	return 1;
}

#define RANG_FALSE_MAX 0                                						//��ͬһ��վ��������������ʧ�ܴ���
/*******************************************************************************
** ��������:  void rang_poll(void)
** ��������:  ���
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
static MyByte8T rang_poll(void)
{
   	MyDouble32T dist1;
	unsigned short distan_value;
	MyByte8T false_cnt = 0;                     								//���ʧ�ܴ�����¼
	while(1){
		dist1 = NTRXRange(app->dest);
		if(dist1 >= 0){															//���ɹ�
          	distan = (unsigned short)(dist1 * 100);
          	distan1 = (distan/100);
			distan_value = distan1;
      		cur_dist.dist = distan1;                        					//���µ�ǰ�����Ϣ
      		cur_dist.dest[4]= app->dest[4];
      		cur_dist.dest[5]= app->dest[5];
			cur_dist.time = get_systime();		

#ifdef NEED_LOCATION																	
		  	if(!Location.state) goto rang_ok_end;								//��δ��λ�򲻷��Ͳ����
#endif

#ifdef DIRECTION
			if((dist_policy(&distan_value) == 0))	goto rang_ok_end;				//������˼�����								
			if(Location.direction == 1){										//����1������Ϊ����
			  	distan_value = ((distan_value&0xFFFE) + 1);		
		  	}else if(Location.direction == 0){									//����0������Ϊż��
		  		distan_value = (distan_value&0xFFFE);
		  	}else{goto rang_ok_end;}
#endif
			if(help_flag) data_buff[CALL_LEN-4] = 0x11;			   				//��������
		  	else data_buff[CALL_LEN-4] = 0x00;                     				//��ͨ��������
          	data_buff[CALL_LEN-3] = (MyByte8T)distan_value;               		//����
          	data_buff[CALL_LEN-2] = (MyByte8T)(distan_value>>8);    
          	data_buff[CALL_LEN-1] = (0x00|(battery.stat<<5)|(help_flag<<7)|(Location.direction<<4));   //��ʾ��״̬    0000 0000���� 0010 0000���� 1100 0000����0001 0000����,����λΪ1��ʾ������վԶ���ӷ�վһ��
          	//NTRXSetRegister( NA_TxArq, FALSE);
          	NTRXSendMessage (PacketTypeData,app->dest, data_buff, CALL_LEN);
rang_ok_end:
          	return 1;                                       			
		}else{																	//���ʧ��
			false_cnt++;
			if(false_cnt > RANG_FALSE_MAX){
       			false_cnt = 0;
				return 0;                                     			
			}
		}//end if(dist1 >= 0)
	} //end while(1)
}

uint8_t sub_child_addr[6];
#define SUB_CHILD_FALSE	3														//�������������
/*******************************************************************************
** ��������:  void rang_childsub(void)
** ��������:  ���ӷ�վ���
** ��ڲ���:  ��
** ���ڲ���:  1Ϊ���ɹ���0Ϊʧ��
** ��    ע:  ��
*******************************************************************************/
static MyByte8T rang_childsub(void){
	MyDouble32T dist1;
	uint8_t i;
	MyByte8T false_cnt = 0;                     								//���ʧ�ܴ�����¼
	for(i=0;i<5;i++){
		sub_child_addr[i] = app->dest[i];
	}
	sub_child_addr[5] = (0x80|app->dest[5]);
	
	while(1){
		dist1 = NTRXRange(sub_child_addr);
		if(dist1 >= 0){															//���ɹ�
			distan = (unsigned short)(dist1 * 100);
			distan2 = (distan/100);
			return 1;
		}else{																	//���ʧ��
			false_cnt++;
          	if(false_cnt > SUB_CHILD_FALSE){
            	false_cnt = 0;
            	return 0;                                     			
          	}
		}
	}
}

static uint8_t rang_poll_falsecnt = 0;
extern uint8_t fist_PowerOn;
uint8_t know_addr = 0;															//�Ƿ���������������վ��־
uint8_t broadcast_addr[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};					//�㲥��ַ
#define MAX_POLL_FALSE		5													//���ʧ��MAX_POLL_FALSE�κ����¹㲥

/*******************************************************************************
** ��������:  void PollApplication(void)
** ��������:  ��༰��λ,��վMACΨһ��ͨ���㲥Ѱ�Ҹ�����վ
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void PollApplication(void)
{
	unsigned short distan_value;
	if((get_systime()>=Location.time)||(fist_PowerOn)||(!Location.state)){   	//�ж��Ƿ���Ҫ��λ
      	Location.time = (get_systime() + LOCATION_TIME);
      	Location.flag = 1;
    }else{
      	Location.flag = 0;
	}

	if(!know_addr){																//δ֪������վ
		dist_policy_restart = 1;												
		data_buff[CALL_LEN-4] = 0xFF;                       					//�㲥����
		data_buff[CALL_LEN-3] = (MyByte8T)distan1;               	
		data_buff[CALL_LEN-2] = (MyByte8T)(distan1>>8);    
		data_buff[CALL_LEN-1] = (0x00|(battery.stat<<5)|(help_flag<<7));    	//��ʾ��״̬    0000 0000���� 0010 0000���� 1100 0000����
		NTRXSendMessage (PacketTypeBrdcast,broadcast_addr, data_buff, CALL_LEN);
		return;
	}else{																		//��֪������վ
		if(rang_poll()){														//������վ���
			rang_ok += 1;                                   		 			//���ɹ���־��λ
        	
			if(Location.flag){
#ifdef NEED_LOCATION
				if(rang_childsub()){											//���ӷ�վ���
#else
				if(0){		
#endif
					/*�����ж�*/
					if((distan1+distan2) < (2*SUB_D)){
						Location.state = 1;
						Location.direction = 0;									//����0��ʾ������վ�����ӷ�վһ��
					}else if((distan1 > distan2)&&((distan1-distan2) >= (SUB_D/2))){
						Location.state = 1;
						Location.direction = 0;
					}else if((distan2 > distan1)&&((distan2-distan1) >= (SUB_D/2))){
						Location.state = 1;
						Location.direction = 1;									//����1��ʾ������վԶ���ӷ�վһ��
					}else{														//��λʧ��
						Location.state = 0;
					}
					
					/*���ͷ�������*/
					if(Location.state){
						distan_value = distan1;
#ifdef DIRECTION
						if((dist_policy(&distan_value) == 0))	goto location_end;
						if(Location.direction == 1){							//����1������Ϊ����
			  				distan_value = ((distan_value&0xFFFE) + 1);		
		  				}else if(Location.direction == 0){						//����0������Ϊż��
		  					distan_value = (distan_value&0xFFFE);
		  				}else{}
#endif                                        
						data_buff[CALL_LEN-4] = 0xDD;                       	//��������
						data_buff[CALL_LEN-3] = (MyByte8T)distan_value;               	
						data_buff[CALL_LEN-2] = (MyByte8T)(distan_value>>8);    
						data_buff[CALL_LEN-1] = (0x00|(battery.stat<<5)|(help_flag<<7)|(Location.direction<<4));    	//��ʾ��״̬    0000 0000���� 0010 0000���� 1100 0000����
						//NTRXSetRegister( NA_TxArq, FALSE);
						NTRXSendMessage (PacketTypeData,app->dest, data_buff, CALL_LEN);
location_end:
						;
					}
					
				}else{//���ӷ�վ���ʧ��
					Location.state = 0;
				}
			}else{}	//����Ҫ��λ
				
		}else{//�����ʧ��      
			rang_poll_falsecnt++;
			if(rang_poll_falsecnt >= MAX_POLL_FALSE){
				know_addr = 0;
				Location.state = 0;
			}
		}
	}
	
}

/*******************************************************************************
** ��������:  void PollApplication1(void)
** ��������:  ��༰��λ����վ��ַΪ�̶��ļ���,ͨ��Ѳ�췽ʽѰ�Ҹ�����վ
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void PollApplication1(void)
{
    int i;
	if((get_systime()>=Location.time)||(fist_PowerOn)){   								//�ж��Ƿ���Ҫ��λ
		Location.time = (get_systime() + LOCATION_TIME);
		Location.flag = 1;
    }else{
		Location.flag = 0;
    }
        
	for(i=0;i<POLL_NUM;i++){
		app->dest[5]++;                                     							//��ѯ��վ
		if(app->dest[5]>POLL_NUM)
			app->dest[5] = 1;
     
		if(rang_poll()){                                    							//�����ɹ�����������Ϣ���˳���ѯ
			rang_ok += 1;                                    							//���ɹ���־��λ
			if(rang_ok){                                 								//������ѯ�е�һ�β��ɹ�
				cur_dist.dist = distan1;                        						//���µ�ǰ�����Ϣ
				cur_dist.dest[4]= app->dest[4];
				cur_dist.dest[5]= app->dest[5];
			}
			if(Location.flag){                                							//��λ����
				Location.state++;
				if(Location.state == 2){
					break;
				}else{continue;}//end if(Location.state == 2)
          
			}else{break;}//end if(Location.flag)             										 //�Ƕ�λ��ֱ���˳�
       
		}else{}//end if(rang_poll()) 
      
	}//end for()
    
	if(rang_ok){                                          								//����Ŀ�Ļ�վ��Ŀ�����´�Ѳ����ϴβ��ɹ���վ��ʼ
		if(cur_dist.dest[5] == 1)                             
			app->dest[5] = POLL_NUM;
        else
            app->dest[5] = (cur_dist.dest[5]-1); 
    } 
}

static uint8_t area_time_flag;															//�Ƿ��ѳ�ʼ�������ж�ʱ���־
static uint32_t area_time;																//�����ʼʱ��
#define	INTERVAL_3s_MODE	0															//3sģʽ
#define	INTERVAL_6s_MODE	1															//6sģʽ
static uint8_t rang_search_mode = INTERVAL_3s_MODE;										//�������ģʽ(3s/6s���)
#define	SWITCH_TIME			60															//������SWITCH_TIMEʱ��δ�յ�����վ��6s����һ��

/*******************************************************************************
** ��������:  void rang_mode_process(void)
** ��������:  �������ģʽ����(��ȷ��λ��3s���һ��,��ͨ��6s����һ��)
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void rang_mode_process(void)
{
	if(!know_addr){
		if(!area_time_flag){
			area_time = (get_systime() + SWITCH_TIME);
			area_time_flag = 1;
		}
		if(get_systime() >= area_time){													//����SWITCH_TIMEʱ��δ�ѵ�����վ������INTERVAL_6s_MODEģʽ
			rang_search_mode = INTERVAL_6s_MODE;
		}
	}else{
		area_time_flag = 0;
		rang_search_mode = INTERVAL_3s_MODE;
	}
	
	if(INTERVAL_6s_MODE == rang_search_mode){
		
	}
}

/*******************************************************************/
/**
 * @brief��ʼ������Դ��ַ.
 *
 *  .
******************************************************************/
void	InitApplication(void)
{
	app = &appM;

/***************** ����Դ��ַ���� *****************/
	app->src[0] = 0xB2;                               									//ȷ��app->src[0]�����λΪ������10
	app->src[1] = 0x00;
	app->src[2] = 0x00;
	app->src[3] = 0x00;
	app->src[4] = (0xFF&card_id);
	app->src[5] = (card_id >> 8);
	NTRXSetStaAddress (app->src);
}

/**
 * nano_init.
 *
 * ����ʼ��.
*/
MyBoolT nano_init()
{
  if(NtrxInit() == FALSE)
    return FALSE;
  InitApplication();
    return TRUE;
}

CREATE_DELY_OBJ(comm_rang_dely,0,HELP_RANG_DELY)								//ʵ������˫��ͨѶ״̬�£�ͨѶʱ����
extern MyByte8T Run_Step;
extern Disp disp;
extern Call_Card call_card;
extern uint8_t UpperCommander;
/**
 * start_rang.
 *
 * ���.
*/
void start_rang()
{
	uint16_t temp;														
	if(UpperCommander||call_card.stat||help_flag){										//˫��ͨѶ����²������
		if(mydely_come(&comm_rang_dely)>=0){
			if(help_flag) temp = HELP_RANG_DELY; else temp = STANDARD_T;
			set_mydely(&comm_rang_dely,0,temp);     
           	disp.disable();
			if(nano_init())                                 								//nano����
 				PollApplication();   
      		//NTRXAllCalibration ();
			//PollApplication();
			if(!know_addr){Zigbee_Transmit();}
   			disp.enable();
		}
	}else{																				//�����������
	  	disp.disable();	
		if(nano_init())                                 								//nano����
 			PollApplication();    		          										//��� 
		//NTRXAllCalibration ();
		disp.disable();
	}
}
