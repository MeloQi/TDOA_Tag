/********************************Copyright (c)**********************************
**                			
**                          				
**-File Info--------------------------------------------------------------------
** File name:			zigbee.c
** Last Version:		V1.0
** Descriptions:		zigbee��ش���
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

boolean UpperCommander = FALSE;												//zigbee���Ϻ���
boolean WriteFactoryDateFlag = FALSE;       								//zigbeeд����ʱ���־λ
uint16_t Factory_Date;                    									//����ʱ��
volatile uint16_t SendFailTime;     										//zigbee����ʧ�ܴ���
volatile bool_t txPacketInFlight = FALSE;									//zigbee������ɱ�־
volatile bool_t packetReceived = FALSE;										//zigbee���յ�����֡��־
uint8_t txBuf[30];                  										//zigbee���ͻ�����
uint8_t rxPacket[127];              										//zigbee���ջ�����
uint8_t RF_Power = ZIGBEE_POWER;                   							//zigbee���͹���
uint8_t Send_Channel = TX_Channel;											//zigbee�����ŵ�
uint8_t	Recieve_Channel = RX_Channel;										//zigbee�����ŵ�
uint16_t RX_Time;                   										//zigbee����ʱ��

uint8_t Card_Cmd[]={0x03,0x88,0x99,0x99,0x99,0xff,0xfe,0x99,
                    0x99,0x57,0x04,0x07};									


RadioTransmitConfig radioTransmitConfig = 
{
  FALSE,  /* �ȴ�Ӧ��*/                                       
  FALSE,  /* checkCca */                                     
  0,      /* ccaAttemptMax */       
  0, /* backoffExponentMin */  
  0, /* backoffExponentMax */  
  0,      /* minimumBackoff */      
  TRUE  /* appendCrc */                                         
};

/*******************************************************************************
** ��������:  array_compare(unsigned char *buffer1,unsigned char *buffer2,
**                          unsigned char length)
** ��������:  �Ƚ����������Ƿ����
** ��ڲ���:  unsigned char *buffer1,unsigned char *buffer2,unsigned char length
** ���ڲ���:  ��
** ��    ע:  ��
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
** ��������:   Zigbee_Transmit
** ��������:   zigbee���ͺ�������������
** ��ڲ���:  
** ���ڲ���:  
** ��    ע: 
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
	  
	  Card_Cmd[10] |= (0x00|(battery.stat<<5)|(help_flag<<7));				//��ص���������״̬

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
** ��������:   Zigbee_Receive
** ��������:   zigbee���մ�����,˫��ͨѶ
** ��ڲ���:  
** ���ڲ���:  
** ��    ע: 
*******************************************************************************/
void Zigbee_Receive(void)
{
	ST_RadioSetChannel(Recieve_Channel);  
    ST_RadioInit(ST_RADIO_POWER_MODE_RX_ON);
		
	RX_Time = 0;
      while(RX_Time <=2200)                                   //����4ms (5ms 2850��ѭ��)
      {
        if(packetReceived==TRUE)
        {
          if(array_compare(&rxPacket[1],&Card_Cmd[0],9))      //�ж�֡ͷ
          {
            if((array_compare(&rxPacket[10],&Card_Cmd[9],2)|| //�ж��Ƿ������Ż�0xFFFF,���Ϻ���
                (rxPacket[10]==0xFF)&&(rxPacket[11]==0xFF)))
            {
              UpperCommander = TRUE;
              break;
            }
            else if((rxPacket[10]==0xEE)&&(rxPacket[11]==0xEE)||
                    (rxPacket[10]==Card_Cmd[9])&&(rxPacket[11]==(Card_Cmd[10]&0x80)))
            {
              UpperCommander = FALSE;                         			//ȡ�����н��뾲Ĭ
              break;
            }
            else if(0x40==(rxPacket[11]&0xE0))		      		//������������,���Ÿ��ֽڸ�2λΪ01��ʾ����������ŵ��ֽڼ����Ÿ��ֽڵ�6λ��ʾ����
            {
                Factory_Date = (((rxPacket[11]&0x3F)<<8)|rxPacket[10]);//��ó�������
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
** ��������:   zigbee_crv_init
** ��������:   zigbee���ճ�ʼ��
** ��ڲ���:  
** ���ڲ���:  
** ��    ע: 
*******************************************************************************/
void zigbee_rcv_init(void)
{
	ST_RadioSetChannel(Recieve_Channel);  
    ST_RadioInit(ST_RADIO_POWER_MODE_RX_ON);
}

/*******************************************************************************
** ��������:   zigbee_is_rcved
** ��������:   zigbee���ճɹ��ж�
** ��ڲ���:  
** ���ڲ���:  ����1��ʾ���յ����ݣ�0��ʾ������
** ��    ע: 
*******************************************************************************/
uint8_t zigbee_is_rcved(void)
{
	if(TRUE == packetReceived)	return 1;
	else						return 0;
}

/*******************************************************************************
** ��������:   zigbee_rcv_process
** ��������:   zigbee�������ݴ���
** ��ڲ���:  
** ���ڲ���: 
** ��    ע: 
*******************************************************************************/
void zigbee_rcv_process(void)
{
	if(packetReceived==TRUE){
		 packetReceived = FALSE;
		if(array_compare(&rxPacket[1],&Card_Cmd[0],9)){      //�ж�֡ͷ
			if((array_compare(&rxPacket[10],&Card_Cmd[9],2)|| //�ж��Ƿ������Ż�0xFFFF,���Ϻ���
                (rxPacket[10]==0xFF)&&(rxPacket[11]==0xFF))){
				UpperCommander = TRUE;
              	return;
            }
            else if((rxPacket[10]==0xEE)&&(rxPacket[11]==0xEE)||
                    (rxPacket[10]==Card_Cmd[9])&&(rxPacket[11]==(Card_Cmd[10]&0x80))){
              	UpperCommander = FALSE;                         			//ȡ�����н��뾲Ĭ
              	return;
            }
            else if(0x40==(rxPacket[11]&0xE0)){		      		//������������,���Ÿ��ֽڸ�3λΪ010��ʾ����������ŵ��ֽڼ����Ÿ��ֽڵ�6λ��ʾ����
                Factory_Date = (((rxPacket[11]&0x3F)<<8)|rxPacket[10]);//��ó�������
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
** ��������:   do_zigbee
** ��������:   zigbeeģʽ���ʹ���
** ��ڲ���:  
** ���ڲ���:  
** ��    ע: 
*******************************************************************************/
void do_zigbee(void)
{
	if((!know_addr)&&(!(UpperCommander||call_card.stat||help_flag))){												//�����ھ�ȷ��λ����zigbee����
		disp.disable();	
		Zigbee_Transmit();
		disp.enable();
	}	
}


/*******************************************************************************
** ��������:   ST_RadioTransmitCompleteIsrCallback
** ��������:   zigbee������ɵĻص�������(phy-library�к�������,��stmRadioTransmitIsr�ж�����)
** ��ڲ���:  
** ���ڲ���:  
** ��    ע:  ���飬�ο�phy-library.h�ļ��Դ˺�������
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
** ��������:   ST_RadioReceiveIsrCallback
** ��������:   zigbee���յ�����֡�����жϵĻص���stmRadioReceiveIsr�ж�
** ��ڲ���:  
** ���ڲ���:  
** ��    ע:  ���飬�ο�phy-library.h�ļ��Դ˺�������
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


