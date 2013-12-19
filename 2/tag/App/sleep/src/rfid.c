/********************************Copyright (c)**********************************
**                        
**                         
**-File Info--------------------------------------------------------------------
** File name:			rfid.c
** Last Version:		
** Descriptions:		
**------------------------------------------------------------------------------
** Created by:	              zhang
** Created date:               2012-10-19
** Version:	               V1.0
** Descriptions:    
**                  
*******************************************************************************/
/**------------------------------------------------------------------------------
** Modify by:	               qi
** Created date:               2013-10-29
** Version:	               
** Descriptions:    
**                  
*******************************************************************************/
#include "iar.h"
#include "hal.h"
#include "uart.h"
#include "stdio.h"
#include "phy-library.h"
#include "hal/micro/cortexm3/micro-common.h"
#include "hal/micro/system-timer.h"
#include "hal/micro/cortexm3/I2C.h"
#include "board.h"
#include "App.h"
#include "rfid.h"
#include "system.h"


/******************************************************************************/








/******************************************************************************/         
uint8_t Sleep_Flag;                 //���ѱ�־λ               
uint8_t SelfDetect_Flag;            //�Լ��־λ
uint8_t TransmitStream_Flag;        //�����ز���־λ
uint8_t RFID_Buf[60];               //RFID ���ջ�����
uint16_t Card_ID;                   //��Ա��ʶ����
volatile bool_t EEPROM_Status;		//״̬��־		

extern unsigned short card_id;
extern uint8_t Card_Cmd[];          //Э������
extern uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr);
extern void AT24CXX_WriteData(uint16_t StartAddr,uint8_t length,uint8_t *write_data);
 uint8_t array_compare(uint8_t *buffer1,uint8_t *buffer2,uint8_t length);
/*******************************************************************************
** ��������:  void Read_RFID_Data(uint16_t reg, const void *data, size_t count)
** ��������:  ��ȡEEPROM��Ϣ
** ��ڲ���:  ��ȡ��ַ����ŵ�ַ������
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void Read_RFID_Data(uint16_t reg, void *data, uint8_t count)
{
  I2cInit(I2C_SPEED_FAST);
  I2cRead(User_Memory,reg,data,count);
}

/*******************************************************************************
** ��������:  void Read_Control_Reg(uint16_t reg, void *data, size_t count)
** ��������:  ��ȡEEPROM��Ϣ
** ��ڲ���:  ��ȡ��ַ����ŵ�ַ������
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void Read_Control_Reg(uint16_t reg, void *data, uint8_t count)
{
  I2cInit(I2C_SPEED_FAST);
  I2cRead(Sys_Memory,reg,data,count);
}

/*******************************************************************************
** ��������:  void Write_Control_Reg(uint16_t reg, const void *data, size_t count)
** ��������:  дEEPROM�Ĵ�����Ϣ
** ��ڲ���:  д��ַ����ŵ�ַ������
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void Write_Control_Reg(uint16_t reg, const void *data, uint8_t count)
{
  I2cInit(I2C_SPEED_STANDARD);
  I2cWrite(Sys_Memory,reg,data,count);
  halCommonDelayMilliseconds(6);                //д����������Ҫ5ms
}
/*******************************************************************************
** ��������:  void Write_RFID_Data(uint16_t reg, const void *data, size_t count)
** ��������:  дEEPROM��Ϣ
** ��ڲ���:  ��ȡ��ַ����ŵ�ַ������
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void Write_RFID_Data(uint16_t reg, const void *data, uint8_t count)
{
  I2cInit(I2C_SPEED_STANDARD);
  I2cWrite(User_Memory,reg,data,count);
  halCommonDelayMilliseconds(6);                //д����������Ҫ5ms
}
/*******************************************************************************
** ��������:  void Read_CardID_Config()
** ��������:  ��������
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void Read_CardID_Config()
{  
	uint8_t rfid[4];
	if(EEPROM_Status){
		//EEPROMOn();                                						//��Դ��
    	Read_RFID_Data(CARD_NUM_ADDR,rfid,BLOCK_NUM);       	//��ȡEEPROM������Ϣ
    	//    EEPROMOff();                              						//��Դ�ر� 
    	Card_ID=(rfid[2]<<8)|(rfid[3]);
     	if((rfid[0]==0x11)&&(rfid[1]==0x86)&&(Card_ID>=1)&&(Card_ID<=8000)){//����1~8000֮��         		
      		Card_Cmd[9] = rfid[3];
      		Card_Cmd[10]= rfid[2];
			card_id = Card_ID;
			return;
  		}else{
  			Card_Cmd[9] = (card_id&0xff);
      		Card_Cmd[10]= (card_id>>8)&0xff;
			rfid[0] = 0x11; rfid[1] = 0x86; rfid[2] = (uint8_t)(card_id>>8); rfid[3] = (uint8_t)(card_id);
			Write_RFID_Data(CARD_NUM_ADDR,rfid,BLOCK_NUM);
			return;
  		}
  	}else{
  		Card_Cmd[9] = (card_id&0xff);     									//��λǰ����λ��
    	Card_Cmd[10]= (card_id>>8)&0xff;
  	}
}
/*******************************************************************************
** ��������:  void Read_WakeSleep_Config()
** ��������:  ����/��������
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void Read_WakeSleep_Config()
{
  uint8_t rfid[4];
//  EEPROMOn();                                //��Դ��
//  halCommonDelayMicroseconds(1000);
  Read_RFID_Data(CARD_SLEEP_ADDR,&rfid,BLOCK_NUM);       //��ȡEEPROM������Ϣ
//  EEPROMOff();                               //��Դ�ر� 
  if((rfid[0]==0x11)&&(rfid[1]==0x88)&&(rfid[2]==0xE0))
  {
    if(rfid[3]==0x5A)
    {
      Sleep_Flag = FALSE;
    }
    else if(rfid[3]==0xA5)
    {
      Sleep_Flag = TRUE;
    }
  }
  else
    Sleep_Flag = FALSE;
}
/*******************************************************************************
** ��������:  void Read_SelfDetect_Config()
** ��������:  ���Լ�����
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void Read_SelfDetect_Config()
{
  uint8_t rfid[4];
//  EEPROMOn();                                //��Դ��
//  halCommonDelayMicroseconds(1000);
  Read_RFID_Data(CARD_DETECT_ADDR,&rfid,BLOCK_NUM);       //��ȡEEPROM������Ϣ
//  EEPROMOff();                               //��Դ�ر� 
  if((rfid[0]==0x11)&&(rfid[1]==0x88)&&(rfid[2]==0xE1))
  {
    if(rfid[3]==0x5A)
      SelfDetect_Flag = TRUE;
    else if(rfid[3]==0xA5)
      SelfDetect_Flag = FALSE;
  }
  else
    SelfDetect_Flag = FALSE;
}
/*******************************************************************************
** ��������:  uint8_t RF_Power_Config()
** ��������:  ���书������
** ��ڲ���:  ��
** ���ڲ���:  ���书��ֵ
** ��    ע:  ��
*******************************************************************************/
uint8_t RF_Power_Config()
{
  uint8_t rf_power;
  uint8_t rfid[4];
//  EEPROMOn();                                //��Դ��
  Read_RFID_Data(CARD_SEND_POWER_ADDR,&rfid,BLOCK_NUM);       //��ȡEEPROM������Ϣ
//  EEPROMOff();                               //��Դ�ر� 
  if((rfid[0]==0x11)&&(rfid[1]==0x88)&&(rfid[2]==0xE2))
  {
    if(rfid[3]<=8)
      rf_power = rfid[3];
    else
      rf_power = 0;
  }
  else
    rf_power = 0;
  return rf_power;
}

/*******************************************************************************
** ��������:  uint8_t RF_Send_Channel()
** ��������:  �����ŵ�����
** ��ڲ���:  ��
** ���ڲ���:  ���书��ֵ
** ��    ע:  ��
*******************************************************************************/
uint8_t RF_Send_Channel()
{
  uint8_t send_channel;
  uint8_t rfid[4];
//  EEPROMOn();                                //��Դ��
  Read_RFID_Data(CARD_SEND_CHANNEL_ADDR,&rfid,BLOCK_NUM);       //��ȡEEPROM������Ϣ
//  EEPROMOff();                               //��Դ�ر� 
  if((rfid[0]==0x11)&&(rfid[1]==0x88)&&(rfid[2]==0xE4))
  {
    if((rfid[3]<=21)&&(rfid[3]>=11))
      send_channel = rfid[3];
    else
      send_channel = TX_Channel;
  }
  else
      send_channel = TX_Channel;
  return send_channel;
}

/*******************************************************************************
** ��������:  uint8_t RF_Recieve_Channel()
** ��������:  �����ŵ�����
** ��ڲ���:  ��
** ���ڲ���:  ���书��ֵ
** ��    ע:  ��
*******************************************************************************/
uint8_t RF_Recieve_Channel()
{
  uint8_t recieve_channel;
  uint8_t rfid[4];
//  EEPROMOn();                                //��Դ��
  Read_RFID_Data(CARD_RECV_CHANNEL_ADDR,&rfid,BLOCK_NUM);       //��ȡEEPROM������Ϣ
//  EEPROMOff();                               //��Դ�ر� 
  if((rfid[0]==0x11)&&(rfid[1]==0x88)&&(rfid[2]==0xE5))
  {
    if((rfid[3]<=21)&&(rfid[3]>=11))
      recieve_channel = rfid[3];
    else
      recieve_channel = RX_Channel;
  }
  else
      recieve_channel = RX_Channel;
  return recieve_channel;
}
/*******************************************************************************
** ��������:  void TransmitStream_Config()
** ��������:  �����ز�����
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void TransmitStream_Config()
{
  uint8_t rfid[4];
//  EEPROMOn();                                //��Դ��
//  halCommonDelayMicroseconds(500);
  Read_RFID_Data(CARD_CARRIER_ADDR,&rfid,BLOCK_NUM);       //��ȡEEPROM������Ϣ
//  EEPROMOff();                               //��Դ�ر� 
  if((rfid[0]==0x11)&&(rfid[1]==0x88)&&(rfid[2]==0xE3))
  {
    if(rfid[3]==0x5A)
      TransmitStream_Flag = TRUE;
    else if(rfid[3]==0xA5)
      TransmitStream_Flag = FALSE;
  }
  else
    TransmitStream_Flag = FALSE;
}

/*******************************************************************************
** ��������:  void WriteBATValue(int16u value)
** ��������:  ����ص�ѹд�뵽EEPROM
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void WriteBATValue(int16u value)
{
  uint8_t rfid[4];
  rfid[0] = 0x11;
  rfid[1] = 0x87;
  rfid[2] = (value>>8)&0xFF;   //д��EEPROM
  rfid[3] = (value&0xFF);
//  EEPROMOn();
//  halCommonDelayMicroseconds(1000);
  Write_RFID_Data(CARD_BAT_ADDR,&rfid,BLOCK_NUM);
//  EEPROMOff();
}

/*******************************************************************************
** ��������:  void WriteSendFailTime(uint16_t time)
** ��������:  ������ʧ�ܴ���д�뵽EEPROM
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
uint16_t ReadSendFailTime(void)
{
  uint8_t rfid[4];
  uint16_t failtime;
  //    EEPROMOn();
  Read_RFID_Data(CARD_SEND_FAIL_ADDR,&rfid,BLOCK_NUM);
  //    EEPROMOff();  
  if((rfid[0]==0x11)&&(rfid[1]==0x89))
  {
    failtime = (rfid[2]<<8)|rfid[3];
  }
  else
  {
    failtime = 0; 
  }
  return failtime;
}

/*******************************************************************************
** ��������:  void WriteSendFailTime(uint16_t time)
** ��������:  ������ʧ�ܴ���д�뵽EEPROM
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void WriteSendFailTime(uint16_t time)
{
  uint8_t rfid[4];
  rfid[0] = 0x11;
  rfid[1] = 0x89;
  rfid[2] = (time>>8)&0xFF;   //д��EEPROM
  rfid[3] = (time&0xFF);
//  EEPROMOn();
  Write_RFID_Data(CARD_SEND_FAIL_ADDR,&rfid,BLOCK_NUM);
//  EEPROMOff();
}

/*******************************************************************************
** ��������:  uint16_t ReadRecvSuccessTime(void)
** ��������:  ��ȡ˫��ͨ�Ŵ���
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
uint16_t ReadRecvSuccessTime(void)
{
  uint8_t rfid[4];
  uint16_t recvtime;  
  //    EEPROMOn();
  Read_RFID_Data(CARD_BIDIRECT_ADDR,&rfid,BLOCK_NUM);
  //    EEPROMOff();  
  if((rfid[0]==0x11)&&(rfid[1]==0x8A))
  {
    recvtime = (rfid[2]<<8)|rfid[3];
  }
  else
  {
    recvtime = 0; 
  }
  return recvtime;
}

/*******************************************************************************
** ��������:  void WriteRecvSuccessTime(uint16_t time)
** ��������:  ��˫��ͨ�Ŵ���д�뵽EEPROM
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void WriteRecvSuccessTime(uint16_t time)
{
  uint8_t rfid[4];
  rfid[0] = 0x11;
  rfid[1] = 0x8A;
  rfid[2] = (time>>8)&0xFF;   //д��EEPROM
  rfid[3] = (time&0xFF);
  EEPROMOn();
  Write_RFID_Data(CARD_BIDIRECT_ADDR,&rfid,BLOCK_NUM);
  EEPROMOff();
}

/*******************************************************************************
** ��������:  uint16_t ReadAbnormalWakeTime(void)
** ��������:  ��ȡ���������ѵĴ���
** ��ڲ���:  ��
** ���ڲ���:  ����
** ��    ע:  ��
*******************************************************************************/
uint16_t ReadAbnormalWakeTime(void)
{
  uint8_t rfid[4];
  uint16_t time;  
  Read_RFID_Data(CARD_ABNORMAL_WAKE_ADDR,&rfid,BLOCK_NUM);
  if((rfid[0]==0x11)&&(rfid[1]==0x8F))
  {
    time = (rfid[2]<<8)|rfid[3];
  }
  else
  {
    time = 0; 
  }
  return time;
}

/*******************************************************************************
** ��������:  void WriteAbnormalWakeTime(uint16_t time)
** ��������:  �����������ѵĴ���д�뵽EEPROM
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void WriteAbnormalWakeTime(uint16_t time)
{
  uint8_t rfid[4];
  rfid[0] = 0x11;
  rfid[1] = 0x8F;
  rfid[2] = (time>>8)&0xFF;   //д��EEPROM
  rfid[3] = (time&0xFF);
  EEPROMOn();
  Write_RFID_Data(CARD_ABNORMAL_WAKE_ADDR,&rfid,BLOCK_NUM);
  EEPROMOff();
}

uint8_t Nano_Carrier_Flag;
/*******************************************************************************
** ��������:  void Nano_TransmitCarrier_Config()
** ��������:  ����NANO�ز�����
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void Nano_TransmitCarrier_Config()
{
  uint8_t rfid[4];
//  EEPROMOn();                                //��Դ��
//  halCommonDelayMicroseconds(500);
  Read_RFID_Data(CARD_NANO_CARRIER_ADDR,&rfid,BLOCK_NUM);       //��ȡEEPROM������Ϣ
//  EEPROMOff();                               //��Դ�ر� 
  if((rfid[0]==0x11)&&(rfid[1]==0x90)&&(rfid[2]==0xE3))
  {
    if(rfid[3]==0x5A)
      Nano_Carrier_Flag = TRUE;
    else if(rfid[3]==0xA5)
      Nano_Carrier_Flag = FALSE;
  }
  else
    Nano_Carrier_Flag = FALSE;
}

/*******************************************************************************
** ��������:  uint8_t EEPROM_SelfDetect()
** ��������:  ��д��һ�����ݣ�Ȼ��������ж��Ƿ���ͬ
** ��ڲ���:  ��
** ���ڲ���:  �ɹ���־λ
** ��    ע:  ��
*******************************************************************************/
 uint8_t EEPROM_SelfDetect()
 {
   uint8_t selfdetect_flag;
   uint8_t detect_time=0;
   uint16_t RandomNum[2];               // 16λ�����
   uint8_t randomNum[4],checknum[4];    // 8λ������Ͷ���������
   do
   {
     ST_RadioGetRandomNumbers(&RandomNum[0],2);          //��ȡ1�������
     randomNum[0] = RandomNum[0]&0x00FF;
     randomNum[1] = (RandomNum[0]>>8)&0xFF;              //�ȵͺ��
     randomNum[2] = RandomNum[1]&0x00FF;
     randomNum[3] = (RandomNum[1]>>8)&0xFF;              //�ȵͺ��
     Write_RFID_Data(0x007C,&randomNum[0],4);            //��д������
     Read_RFID_Data(0x007C,&checknum[0],4);              //�������ݣ��Ƚ��Ƿ���ͬ      
     if(array_compare(&randomNum[0],&checknum[0],4)!=TRUE)
     {
       selfdetect_flag = FALSE;
     }
     else
     {
       selfdetect_flag = TRUE; 
     }
     detect_time++;
   }while((selfdetect_flag==FALSE)&&(detect_time<=3));//���ʧ�ܣ������Լ�3��
   return selfdetect_flag;
 }

/*******************************************************************************
** ��������:  uint8_t EEPROM_Fist_Boot()
** ��������:  �ж��Ƿ�Ϊ��һ���ϵ�
** ��ڲ���:  ��
** ���ڲ���:  1��ʾ��һ���ϵ磬0��ʾ�ǵ�һ���ϵ�
** ��    ע:  ��
*******************************************************************************/
uint8_t EEPROM_Fist_Boot()
{
	uint8_t rfid[4];
  	Read_RFID_Data(CARD_IS_FIST_BOOT_ADDR,&rfid,BLOCK_NUM);       //��ȡEEPROM������Ϣ
  	if((128==rfid[0])&&(128==rfid[1])&&(128==rfid[2])&&(128==rfid[3])){
		return 0;
	}
	
	rfid[0]=128;
	rfid[1]=128;
	rfid[2]=128;
	rfid[3]=128;
	Write_RFID_Data(CARD_IS_FIST_BOOT_ADDR,&rfid,BLOCK_NUM);
	return 1;
	
}
extern uint8_t RF_Power;
/*******************************************************************************
** ��������:   EEPROM_Process()
** ��������:  eeprom��������
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void EEPROM_Process(void)
{
	if((EEPROM_Status)&&(EEPROM_Idle))
    {   
        EEPROMOn();                                	//��Դ��
        halCommonDelayMicroseconds(1000);
        RF_Power = RF_Power_Config();              	//���书������        
        Read_SelfDetect_Config();                  	//�Լ칦��      
        TransmitStream_Config();                   	//�����ز�����
        Read_WakeSleep_Config();                   	//����/����
    }
    Read_CardID_Config();                        	//��ȡ������Ϣ
    EEPROMOff();                               		//��Դ�ر� 

	HandleEEPROM();									//����EEPROM��Ϣ������
}
