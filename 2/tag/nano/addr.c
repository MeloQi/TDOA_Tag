/********************************Copyright (c)**********************************
**                
**                         
**-File Info--------------------------------------------------------------------
** File name:			Addr.c
** Last Version:		V1.0
** Descriptions:		NANO 2.4G CSS测距定位
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

//#define NEED_LOCATION									//定位开关
//#define  DIRECTION										//方向处理开关
#define  DIST_POLICY_THREE								//三三过滤开关
#define  DIST_POLICY_LARGE_VALUE						//大值过滤开关
#define  DIST_POLICY_LITTLE_VALUE						//小值过滤开关
#define  DIST_POLICY_HALF_VALUE							//折半趋近开关

extern StartUp_Info *read_startinfo;
extern  MyWord16T card_id;                              //卡号

appMemT appM = {                                          
  {                                                     //源MAC地址
    0xB2,                                               //确保app->src[0]最后两位为二进制10
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
  },
  {	                                                	//目的MAC地址
    0xA2,                                               //确保app->dest[0]最后两位为二进制10
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
  }
};
appMemT *app;                                           						//记录源地址 目的地址
Rang_Dist old_dist = {															//记录历史距离
	.dest = {0x00,0x00,0x00,0x00,0x00,0x00},
	.dist = 0,
	.time = 0
};
Rang_Dist pre_dist = {															//记录上一个距离
	.dest = {0x00,0x00,0x00,0x00,0x00,0x00},
	.dist = 0,
	.time = 0
};

Rang_Dist cur_dist;                            									//记录当前及历史测距目的地址及距离
MyByte8T rang_ok = 0;                                   						//测距成功标志
unsigned char data_buff[CALL_LEN]={0x88,0x99,0x99,0x99,0xff,0x00,0x00,0x00,0x00}; //发送帧中的数据部分
extern Bat       battery;                               						//电池相关信息
unsigned short distan,distan1,distan2;                          				//distan为距离值*100，distan1为距主分站距离    , distan2 为距子分站距离                
Locat_Info Location = {
	.time = 0,
	.flag = 1,
	.state = 0,
	.direction = 0,
};
extern uint8_t help_flag;

/*******************************************************************************
** 函数名称:   sort
** 函数功能:  冒泡排序
** 入口参数:  sort_data  	要排序的数组
              		   lenght        数组长度
** 出口参数:  
** 备    注:  无
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


#define	DIST_THREE_DT			9												//三三过滤有效时间差单位s
#define	DIST_ARRY_SIZE			3	
static 	unsigned short 	dist_arry[DIST_ARRY_SIZE] = {0,0,0};					//保存三三过滤值
static 	unsigned short 	dist_arry1[DIST_ARRY_SIZE] = {0,0,0};					//保存三三过排序值
static	uint8_t			three_filter_index;										//三三过滤已保存距离个数
#define	DIST_BIG_FILTER			5												//大值过滤系数
#define	DIST_LITTLE_FILTER		2												//小值过滤阀值
uint8_t	oldD_update_flag;														//是否更新历史距离信息标志
static uint8_t dist_policy_restart;												//

/*******************************************************************************
** 函数名称:  void dist_policy(void)
** 函数功能: 对测距距离处理的策略
** 入口参数:  Dist 返回处理后的距离值
** 出口参数:  Dist 返回处理后的距离值
** 返回值	:  返回1表示距离值可用，0表示距离值不可用
** 备    注:  无
*******************************************************************************/
static MyByte8T dist_policy(unsigned short * Dist)
{	
	unsigned short 	Dd,PDd;														//保存距离差值
	unsigned short	D;															//当前距离
	uint32_t 		Dt,PDt;
	uint8_t 		dir;														//记录方向(即距离数值是增加还是减小)
	uint8_t 		i;
	#define	DIST_SUB	0														
	#define	DIST_ADD	1
	
	if(old_dist.time > cur_dist.time)	goto dist_policy_end;
	if((old_dist.time == 0)||(pre_dist.time == 0))	goto dist_policy_end;		//第一次测距值不做处理;本次测距与上次测距分站不同不做处理
	if((old_dist.dest[5] != cur_dist.dest[5])||(old_dist.dest[4] != cur_dist.dest[4])){
		three_filter_index = 0;
		goto dist_policy_end;	
	}
	
	if((pre_dist.dest[5] != cur_dist.dest[5])||(pre_dist.dest[4] != cur_dist.dest[4])){
		three_filter_index = 0;
		goto dist_policy_end;	
	}
	
	if(dist_policy_restart){													//重启距离处理									
		dist_policy_restart = 0;
		goto dist_policy_end;
	}

	Dt = cur_dist.time - old_dist.time; 										//历史、上一组与当前时间差
	PDt = cur_dist.time - pre_dist.time;
	D = cur_dist.dist;
	
#ifdef 	DIST_POLICY_THREE
	if(PDt > DIST_THREE_DT)	three_filter_index = 0;								//三三过滤
	if(three_filter_index < DIST_ARRY_SIZE){									
		dist_arry[three_filter_index] = cur_dist.dist;
		three_filter_index++;
	}else if(three_filter_index == DIST_ARRY_SIZE){								//已收到三个距离值
		for(i=0;i<(DIST_ARRY_SIZE-1);i++){										//删除最老值将新值插入
			dist_arry[i] = 	dist_arry[(i+1)];
		}
		dist_arry[(DIST_ARRY_SIZE-1)] = cur_dist.dist;
		for(i = 0;i < DIST_ARRY_SIZE;i++){										//找出三组距离值中的中间值	
			dist_arry1[i] = dist_arry[i];
		}
		sort(dist_arry1,DIST_ARRY_SIZE);
		D = dist_arry1[(DIST_ARRY_SIZE/2)];
	}else{three_filter_index = 0;}
#endif
																				//历史和当前距离差
	if(old_dist.dist > D) 				{Dd = old_dist.dist - D; dir = DIST_SUB;}
	else							  	{Dd = D - old_dist.dist; dir = DIST_ADD;}
/*
	if(pre_dist.dist > D) 				{PDd = pre_dist.dist - D; } 						//大值过滤
	else							  	{PDd = D - pre_dist.dist; }
	if(PDd > PDt*DIST_BIG_FILTER)		{oldD_update_flag = 0; return 0;}			
	pre_dist.dist = cur_dist.dist;														//更新上一次距离测量值											
	pre_dist.dest[4]= cur_dist.dest[4];
	pre_dist.dest[5]= cur_dist.dest[5];
	pre_dist.time = get_systime();
*/

#ifdef 	DIST_POLICY_LARGE_VALUE
	if(Dd > Dt*DIST_BIG_FILTER)		{oldD_update_flag = 0; return 0;}			//大值过滤
#endif

#ifdef 	DIST_POLICY_LITTLE_VALUE
	if(Dd <= DIST_LITTLE_FILTER)	{oldD_update_flag = 0; return 0;}			//小值过滤
#endif

#ifdef 	DIST_POLICY_HALF_VALUE
	if(dir == DIST_ADD)				{* Dist = (old_dist.dist + Dd/2);}			//折半趋近
	else if(dir == DIST_SUB)		{* Dist = (old_dist.dist - Dd/2);}
#endif

dist_policy_end:	
	cur_dist.dist = (* Dist);													//更新当前距离为处理过后的距离
	oldD_update_flag = 1;
	return 1;
}

#define RANG_FALSE_MAX 0                                						//对同一基站允许最大连续测距失败次数
/*******************************************************************************
** 函数名称:  void rang_poll(void)
** 函数功能:  测距
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
static MyByte8T rang_poll(void)
{
   	MyDouble32T dist1;
	unsigned short distan_value;
	MyByte8T false_cnt = 0;                     								//测距失败次数记录
	while(1){
		dist1 = NTRXRange(app->dest);
		if(dist1 >= 0){															//测距成功
          	distan = (unsigned short)(dist1 * 100);
          	distan1 = (distan/100);
			distan_value = distan1;
      		cur_dist.dist = distan1;                        					//更新当前测距信息
      		cur_dist.dest[4]= app->dest[4];
      		cur_dist.dest[5]= app->dest[5];
			cur_dist.time = get_systime();		

#ifdef NEED_LOCATION																	
		  	if(!Location.state) goto rang_ok_end;								//若未定位则不发送测距结果
#endif

#ifdef DIRECTION
			if((dist_policy(&distan_value) == 0))	goto rang_ok_end;				//距离过滤及修正								
			if(Location.direction == 1){										//方向1，距离为奇数
			  	distan_value = ((distan_value&0xFFFE) + 1);		
		  	}else if(Location.direction == 0){									//方向0，距离为偶数
		  		distan_value = (distan_value&0xFFFE);
		  	}else{goto rang_ok_end;}
#endif
			if(help_flag) data_buff[CALL_LEN-4] = 0x11;			   				//求助命令
		  	else data_buff[CALL_LEN-4] = 0x00;                     				//普通测距包命令
          	data_buff[CALL_LEN-3] = (MyByte8T)distan_value;               		//距离
          	data_buff[CALL_LEN-2] = (MyByte8T)(distan_value>>8);    
          	data_buff[CALL_LEN-1] = (0x00|(battery.stat<<5)|(help_flag<<7)|(Location.direction<<4));   //标示卡状态    0000 0000正常 0010 0000电量 1100 0000求助0001 0000方向,第五位为1表示在主分站远离子分站一侧
          	//NTRXSetRegister( NA_TxArq, FALSE);
          	NTRXSendMessage (PacketTypeData,app->dest, data_buff, CALL_LEN);
rang_ok_end:
          	return 1;                                       			
		}else{																	//测距失败
			false_cnt++;
			if(false_cnt > RANG_FALSE_MAX){
       			false_cnt = 0;
				return 0;                                     			
			}
		}//end if(dist1 >= 0)
	} //end while(1)
}

uint8_t sub_child_addr[6];
#define SUB_CHILD_FALSE	3														//最大连续测距次数
/*******************************************************************************
** 函数名称:  void rang_childsub(void)
** 函数功能:  与子分站测距
** 入口参数:  无
** 出口参数:  1为测距成功，0为失败
** 备    注:  无
*******************************************************************************/
static MyByte8T rang_childsub(void){
	MyDouble32T dist1;
	uint8_t i;
	MyByte8T false_cnt = 0;                     								//测距失败次数记录
	for(i=0;i<5;i++){
		sub_child_addr[i] = app->dest[i];
	}
	sub_child_addr[5] = (0x80|app->dest[5]);
	
	while(1){
		dist1 = NTRXRange(sub_child_addr);
		if(dist1 >= 0){															//测距成功
			distan = (unsigned short)(dist1 * 100);
			distan2 = (distan/100);
			return 1;
		}else{																	//测距失败
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
uint8_t know_addr = 0;															//是否已搜索到附近分站标志
uint8_t broadcast_addr[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};					//广播地址
#define MAX_POLL_FALSE		5													//测距失败MAX_POLL_FALSE次后重新广播

/*******************************************************************************
** 函数名称:  void PollApplication(void)
** 函数功能:  测距及定位,分站MAC唯一，通过广播寻找附近分站
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void PollApplication(void)
{
	unsigned short distan_value;
	if((get_systime()>=Location.time)||(fist_PowerOn)||(!Location.state)){   	//判断是否需要定位
      	Location.time = (get_systime() + LOCATION_TIME);
      	Location.flag = 1;
    }else{
      	Location.flag = 0;
	}

	if(!know_addr){																//未知附近分站
		dist_policy_restart = 1;												
		data_buff[CALL_LEN-4] = 0xFF;                       					//广播命令
		data_buff[CALL_LEN-3] = (MyByte8T)distan1;               	
		data_buff[CALL_LEN-2] = (MyByte8T)(distan1>>8);    
		data_buff[CALL_LEN-1] = (0x00|(battery.stat<<5)|(help_flag<<7));    	//标示卡状态    0000 0000正常 0010 0000电量 1100 0000求助
		NTRXSendMessage (PacketTypeBrdcast,broadcast_addr, data_buff, CALL_LEN);
		return;
	}else{																		//已知附近分站
		if(rang_poll()){														//与主分站测距
			rang_ok += 1;                                   		 			//测距成功标志置位
        	
			if(Location.flag){
#ifdef NEED_LOCATION
				if(rang_childsub()){											//与子分站测距
#else
				if(0){		
#endif
					/*方向判断*/
					if((distan1+distan2) < (2*SUB_D)){
						Location.state = 1;
						Location.direction = 0;									//方向0表示在主分站靠近子分站一侧
					}else if((distan1 > distan2)&&((distan1-distan2) >= (SUB_D/2))){
						Location.state = 1;
						Location.direction = 0;
					}else if((distan2 > distan1)&&((distan2-distan1) >= (SUB_D/2))){
						Location.state = 1;
						Location.direction = 1;									//方向1表示在主分站远离子分站一侧
					}else{														//定位失败
						Location.state = 0;
					}
					
					/*发送方向命令*/
					if(Location.state){
						distan_value = distan1;
#ifdef DIRECTION
						if((dist_policy(&distan_value) == 0))	goto location_end;
						if(Location.direction == 1){							//方向1，距离为奇数
			  				distan_value = ((distan_value&0xFFFE) + 1);		
		  				}else if(Location.direction == 0){						//方向0，距离为偶数
		  					distan_value = (distan_value&0xFFFE);
		  				}else{}
#endif                                        
						data_buff[CALL_LEN-4] = 0xDD;                       	//方向命令
						data_buff[CALL_LEN-3] = (MyByte8T)distan_value;               	
						data_buff[CALL_LEN-2] = (MyByte8T)(distan_value>>8);    
						data_buff[CALL_LEN-1] = (0x00|(battery.stat<<5)|(help_flag<<7)|(Location.direction<<4));    	//标示卡状态    0000 0000正常 0010 0000电量 1100 0000求助
						//NTRXSetRegister( NA_TxArq, FALSE);
						NTRXSendMessage (PacketTypeData,app->dest, data_buff, CALL_LEN);
location_end:
						;
					}
					
				}else{//与子分站测距失败
					Location.state = 0;
				}
			}else{}	//不需要定位
				
		}else{//若测距失败      
			rang_poll_falsecnt++;
			if(rang_poll_falsecnt >= MAX_POLL_FALSE){
				know_addr = 0;
				Location.state = 0;
			}
		}
	}
	
}

/*******************************************************************************
** 函数名称:  void PollApplication1(void)
** 函数功能:  测距及定位，分站地址为固定的几个,通过巡检方式寻找附近分站
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void PollApplication1(void)
{
    int i;
	if((get_systime()>=Location.time)||(fist_PowerOn)){   								//判断是否需要定位
		Location.time = (get_systime() + LOCATION_TIME);
		Location.flag = 1;
    }else{
		Location.flag = 0;
    }
        
	for(i=0;i<POLL_NUM;i++){
		app->dest[5]++;                                     							//轮询分站
		if(app->dest[5]>POLL_NUM)
			app->dest[5] = 1;
     
		if(rang_poll()){                                    							//若测距成功则更新相关信息，退出轮询
			rang_ok += 1;                                    							//测距成功标志置位
			if(rang_ok){                                 								//本次轮询中第一次测距成功
				cur_dist.dist = distan1;                        						//更新当前测距信息
				cur_dist.dest[4]= app->dest[4];
				cur_dist.dest[5]= app->dest[5];
			}
			if(Location.flag){                                							//定位处理
				Location.state++;
				if(Location.state == 2){
					break;
				}else{continue;}//end if(Location.state == 2)
          
			}else{break;}//end if(Location.flag)             										 //非定位，直接退出
       
		}else{}//end if(rang_poll()) 
      
	}//end for()
    
	if(rang_ok){                                          								//调整目的基站，目的是下次巡检从上次测距成功基站开始
		if(cur_dist.dest[5] == 1)                             
			app->dest[5] = POLL_NUM;
        else
            app->dest[5] = (cur_dist.dest[5]-1); 
    } 
}

static uint8_t area_time_flag;															//是否已初始化区域判断时间标志
static uint32_t area_time;																//区域初始时间
#define	INTERVAL_3s_MODE	0															//3s模式
#define	INTERVAL_6s_MODE	1															//6s模式
static uint8_t rang_search_mode = INTERVAL_3s_MODE;										//测距搜索模式(3s/6s间隔)
#define	SWITCH_TIME			60															//若连续SWITCH_TIME时间未收到测距分站则6s搜索一次

/*******************************************************************************
** 函数名称:  void rang_mode_process(void)
** 函数功能:  测距搜索模式处理(精确定位区3s测距一次,普通区6s搜索一次)
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void rang_mode_process(void)
{
	if(!know_addr){
		if(!area_time_flag){
			area_time = (get_systime() + SWITCH_TIME);
			area_time_flag = 1;
		}
		if(get_systime() >= area_time){													//连续SWITCH_TIME时间未搜到测距分站，进入INTERVAL_6s_MODE模式
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
 * @brief初始化本机源地址.
 *
 *  .
******************************************************************/
void	InitApplication(void)
{
	app = &appM;

/***************** 本机源地址配置 *****************/
	app->src[0] = 0xB2;                               									//确保app->src[0]最后两位为二进制10
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
 * 测距初始化.
*/
MyBoolT nano_init()
{
  if(NtrxInit() == FALSE)
    return FALSE;
  InitApplication();
    return TRUE;
}

CREATE_DELY_OBJ(comm_rang_dely,0,HELP_RANG_DELY)								//实例化在双向通讯状态下，通讯时间间隔
extern MyByte8T Run_Step;
extern Disp disp;
extern Call_Card call_card;
extern uint8_t UpperCommander;
/**
 * start_rang.
 *
 * 测距.
*/
void start_rang()
{
	uint16_t temp;														
	if(UpperCommander||call_card.stat||help_flag){										//双向通讯情况下测距流程
		if(mydely_come(&comm_rang_dely)>=0){
			if(help_flag) temp = HELP_RANG_DELY; else temp = STANDARD_T;
			set_mydely(&comm_rang_dely,0,temp);     
           	disp.disable();
			if(nano_init())                                 								//nano配置
 				PollApplication();   
      		//NTRXAllCalibration ();
			//PollApplication();
			if(!know_addr){Zigbee_Transmit();}
   			disp.enable();
		}
	}else{																				//正常测距流程
	  	disp.disable();	
		if(nano_init())                                 								//nano配置
 			PollApplication();    		          										//测距 
		//NTRXAllCalibration ();
		disp.disable();
	}
}
