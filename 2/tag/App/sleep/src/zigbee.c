/********************************Copyright (c)**********************************
**                			
**                          				
**-File Info--------------------------------------------------------------------
** File name:			zigbee.c
** Last Version:		V1.0
** Descriptions:		zigbee相关处理
**------------------------------------------------------------------------------
** creat by:	 		QI
** creat date:     		2013-08-12
** Version:	       	V1.0
** Descriptions:    	
*******************************************************************************/


#include "iar.h"
#include "hal.h"
#include "phy-library.h"
#include "hal/micro/cortexm3/micro-common.h"
#include "app_adc.h"
#include "Zigbee.h"
#include "rfid.h"
#include "disp.h"
#include "nano/ntrxutil.h"
#include "commu.h"

boolean UpperCommander = FALSE;												//zigbee井上呼叫
boolean WriteFactoryDateFlag = FALSE;       								//zigbee写出厂时间标志位
uint16_t Factory_Date;                    									//出厂时间
volatile uint16_t SendFailTime;     										//zigbee发送失败次数
volatile bool_t txPacketInFlight = FALSE;									//zigbee发送完成标志
volatile bool_t packetReceived = FALSE;										//zigbee接收到数据帧标志
uint8_t txBuf[30];                  										//zigbee发送缓冲区
uint8_t rxPacket[127];              										//zigbee接收缓冲区
uint8_t RF_Power = ZIGBEE_POWER;                   							//zigbee发送功率
uint8_t Send_Channel = TX_Channel;											//zigbee发送信道
uint8_t	Recieve_Channel = RX_Channel;										//zigbee接收信道
uint16_t RX_Time;                   										//zigbee接收时间

uint8_t Card_Cmd[]={0x03,0x88,0x99,0x99,0x99,0xff,0xfe,0x99,
                    0x99,0x57,0x04,0x07};									


RadioTransmitConfig radioTransmitConfig = 
{
  FALSE,  /* 等待应答*/                                       
  FALSE,  /* checkCca */                                     
  0,      /* ccaAttemptMax */       
  0, /* backoffExponentMin */  
  0, /* backoffExponentMax */  
  0,      /* minimumBackoff */      
  TRUE  /* appendCrc */                                         
};

/*******************************************************************************
** 函数名称:  array_compare(unsigned char *buffer1,unsigned char *buffer2,
**                          unsigned char length)
** 函数功能:  比较两组数据是否相等
** 入口参数:  unsigned char *buffer1,unsigned char *buffer2,unsigned char length
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
 uint8_t array_compare(uint8_t *buffer1,uint8_t *buffer2,uint8_t length)
 {
     while(length--)
     {
         if(*buffer1++!=*buffer2++)
         {
              return FALSE;
         }
     }
     
     return TRUE;
 }


extern Bat       battery;
extern uint8_t help_flag;
/*******************************************************************************
** 函数名称:   Zigbee_Transmit
** 函数功能:   zigbee发送函数处理主函数
** 入口参数:  
** 出口参数:  
** 备    注: 
*******************************************************************************/
void Zigbee_Transmit(void)
{
	  ST_RadioEnableOverflowNotification(FALSE);
      ST_RadioSetPowerMode(ST_TX_POWER_MODE_DEFAULT);    
      ST_RadioEnableAddressFiltering(FALSE);
      ST_RadioEnableAutoAck(FALSE);
      ST_RadioSetPower(RF_Power);
      ST_RadioSetChannel(Send_Channel);   
      ST_RadioInit(ST_RADIO_POWER_MODE_OFF); 
	  
	  Card_Cmd[10] |= (0x00|(battery.stat<<5)|(help_flag<<7));				//电池电量及求助状态

	  txBuf[0] =(int8u)(sizeof(Card_Cmd)+2);
      for(int8u ct = 0; ct < (int8u)sizeof(Card_Cmd); ct++)
        txBuf[ct + 1] = Card_Cmd[ct];      
      txPacketInFlight = TRUE;
      if(ST_RadioTransmit(txBuf)==ST_SUCCESS){
        while(txPacketInFlight);
        ST_RadioSleep();
      }
      else
        SendFailTime++;
}



/*******************************************************************************
** 函数名称:   Zigbee_Receive
** 函数功能:   zigbee接收处理函数,双向通讯
** 入口参数:  
** 出口参数:  
** 备    注: 
*******************************************************************************/
void Zigbee_Receive(void)
{
	ST_RadioSetChannel(Recieve_Channel);  
    ST_RadioInit(ST_RADIO_POWER_MODE_RX_ON);
		
	RX_Time = 0;
      while(RX_Time <=2200)                                   //接收4ms (5ms 2850个循环)
      {
        if(packetReceived==TRUE)
        {
          if(array_compare(&rxPacket[1],&Card_Cmd[0],9))      //判断帧头
          {
            if((array_compare(&rxPacket[10],&Card_Cmd[9],2)|| //判断是否自身编号或0xFFFF,井上呼叫
                (rxPacket[10]==0xFF)&&(rxPacket[11]==0xFF)))
            {
              UpperCommander = TRUE;
              break;
            }
            else if((rxPacket[10]==0xEE)&&(rxPacket[11]==0xEE)||
                    (rxPacket[10]==Card_Cmd[9])&&(rxPacket[11]==(Card_Cmd[10]&0x80)))
            {
              UpperCommander = FALSE;                         			//取消呼叫进入静默
              break;
            }
            else if(0x40==(rxPacket[11]&0xE0))		      		//日期设置命令,卡号高字节高2位为01表示日期命令，卡号低字节及卡号高字节低6位表示日期
            {
                Factory_Date = (((rxPacket[11]&0x3F)<<8)|rxPacket[10]);//获得出厂日期
                WriteFactoryDateFlag = TRUE;
                break;
            }
          }
          packetReceived = FALSE;
        }      
        RX_Time++;
      }
      packetReceived = FALSE;
}

/*******************************************************************************
** 函数名称:   zigbee_crv_init
** 函数功能:   zigbee接收初始化
** 入口参数:  
** 出口参数:  
** 备    注: 
*******************************************************************************/
void zigbee_rcv_init(void)
{
	ST_RadioSetChannel(Recieve_Channel);  
    ST_RadioInit(ST_RADIO_POWER_MODE_RX_ON);
}

/*******************************************************************************
** 函数名称:   zigbee_is_rcved
** 函数功能:   zigbee接收成功判断
** 入口参数:  
** 出口参数:  返回1表示接收到数据，0表示无数据
** 备    注: 
*******************************************************************************/
uint8_t zigbee_is_rcved(void)
{
	if(TRUE == packetReceived)	return 1;
	else						return 0;
}

/*******************************************************************************
** 函数名称:   zigbee_rcv_process
** 函数功能:   zigbee接收数据处理
** 入口参数:  
** 出口参数: 
** 备    注: 
*******************************************************************************/
void zigbee_rcv_process(void)
{
	if(packetReceived==TRUE){
		 packetReceived = FALSE;
		if(array_compare(&rxPacket[1],&Card_Cmd[0],9)){      //判断帧头
			if((array_compare(&rxPacket[10],&Card_Cmd[9],2)|| //判断是否自身编号或0xFFFF,井上呼叫
                (rxPacket[10]==0xFF)&&(rxPacket[11]==0xFF))){
				UpperCommander = TRUE;
              	return;
            }
            else if((rxPacket[10]==0xEE)&&(rxPacket[11]==0xEE)||
                    (rxPacket[10]==Card_Cmd[9])&&(rxPacket[11]==(Card_Cmd[10]&0x80))){
              	UpperCommander = FALSE;                         			//取消呼叫进入静默
              	return;
            }
            else if(0x40==(rxPacket[11]&0xE0)){		      		//日期设置命令,卡号高字节高3位为010表示日期命令，卡号低字节及卡号高字节低6位表示日期
                Factory_Date = (((rxPacket[11]&0x3F)<<8)|rxPacket[10]);//获得出厂日期
                WriteFactoryDateFlag = TRUE;
                return;
			}
		}
	}      
}

extern uint8_t know_addr;
extern Disp disp;
extern Call_Card call_card;
/*******************************************************************************
** 函数名称:   do_zigbee
** 函数功能:   zigbee模式发送处理
** 入口参数:  
** 出口参数:  
** 备    注: 
*******************************************************************************/
void do_zigbee(void)
{
	if((!know_addr)&&(!(UpperCommander||call_card.stat||help_flag))){												//若不在精确定位区则zigbee发送
		disp.disable();	
		Zigbee_Transmit();
		disp.enable();
	}	
}


/*******************************************************************************
** 函数名称:   ST_RadioTransmitCompleteIsrCallback
** 函数功能:   zigbee发送完成的回调函数，(phy-library中函数调用,由stmRadioTransmitIsr中断引发)
** 入口参数:  
** 出口参数:  
** 备    注:  详情，参考phy-library.h文件对此函数描述
*******************************************************************************/
void ST_RadioTransmitCompleteIsrCallback(StStatus status,
                                         int32u sfdSentTime,
                                         boolean framePending)
{
    switch(status){
      case ST_SUCCESS:
        txPacketInFlight = FALSE;
        break;
    }
}


/*******************************************************************************
** 函数名称:   ST_RadioReceiveIsrCallback
** 函数功能:   zigbee接收到数据帧产生中断的回调，stmRadioReceiveIsr中断
** 入口参数:  
** 出口参数:  
** 备    注:  详情，参考phy-library.h文件对此函数描述
*******************************************************************************/
void ST_RadioReceiveIsrCallback(int8u *packet,
                                boolean ackFramePendingSet,
                                int32u time,
                                int16u errors,
                                int8s rssi)
{
  uint8_t i;
  if(packetReceived == FALSE){ 
    for(i=0; i<=packet[0]; i++){ 
      rxPacket[i] = packet[i];
    }
    packetReceived = TRUE;
  }
}


