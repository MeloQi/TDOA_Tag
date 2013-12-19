/********************************Copyright (c)**********************************
**                         
**                         
**-File Info--------------------------------------------------------------------
** File name:			system.c
** Last Version:		
** Descriptions:		
**------------------------------------------------------------------------------
** Created by:	               zhang
** Created date:               2013-1-9
** Version:	               V1.0
** Descriptions:               
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
#include "rfid.h"
#include "System.h"
#include "math.h"
#include "App.h"
#include "System.h"
#include "nano/ntrxutil.h"
#include "disp.h"
uint16_t SoftResetTime,WDogResetTime,PowerResetTime; //��������
extern uint8_t Send_Channel,Recieve_Channel;     //���ͽ����ŵ�
extern Disp disp;
extern uint8_t RF_Power;
/*******************************************************************************
** ��������:  uint16_t ReadResetTime(uint8_t reset_type)
** ��������:  ��EEPROM��ȡ��λ����
** ��ڲ���:  reset_type
** ���ڲ���:  ��Ӧ��λ���͵ĸ�λ����
** ��    ע:  ��
*******************************************************************************/
uint16_t ReadResetTime(uint8_t reset_type)
{
  uint8_t rfid[4];
  switch(reset_type)
  {
  case SOFT_RESET:
    Read_RFID_Data(CARD_SOFT_RESET_ADDR,&rfid,BLOCK_NUM);  //��ȡEEPROM��λ����
    if((rfid[0]==0x11)&&(rfid[1]==0x8B))
    {    
      SoftResetTime = (rfid[2]<<8)|(rfid[3]);
    }
    else
    {
      SoftResetTime = 0;
    }
    return SoftResetTime;
    break;
  case WATCHDOG_RESET:
    Read_RFID_Data(CARD_WDOG_RESET_ADDR,&rfid,BLOCK_NUM);  //��ȡEEPROM����λ����
    if((rfid[0]==0x11)&&(rfid[1]==0x8C))
    {    
      WDogResetTime = (rfid[2]<<8)|(rfid[3]);
    }
    else
    {
      WDogResetTime = 0;
    }
    return WDogResetTime;
    break;
  case POWER_RESET:
    Read_RFID_Data(CARD_POWER_RESET_ADDR,&rfid,BLOCK_NUM);  //��ȡEEPROM���縴λ����
    if((rfid[0]==0x11)&&(rfid[1]==0x8D))
    {    
      PowerResetTime = (rfid[2]<<8)|(rfid[3]);
    }
    else
    {
      PowerResetTime = 0;
    }
    return PowerResetTime;
    break;
  default:
    return ERROR;
    break;
  }  
}

/*******************************************************************************
** ��������:  void WriteResetTime(uint8_t reset_type,uint16_t reset_num)
** ��������:  ��EEPROMд��Ӧ��λ���͵ĸ�λ����
** ��ڲ���:  uint8_t reset_type,uint16_t reset_num
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void WriteResetTime(uint8_t reset_type,uint16_t reset_num)
{
  uint8_t rfid[4];
  uint16_t reset_addr;
  rfid[0] = 0x11;
  switch(reset_type)
  {
  case SOFT_RESET:
    rfid[1] = 0x8B;
    reset_addr = CARD_SOFT_RESET_ADDR;
    break;
  case WATCHDOG_RESET:
    rfid[1] = 0x8C;
    reset_addr = CARD_WDOG_RESET_ADDR;
    break;
  case POWER_RESET:
    rfid[1] = 0x8D;
    reset_addr = CARD_POWER_RESET_ADDR;
    break;
  default:
    break;
  }
  rfid[2] = (reset_num>>8)&0xFF;  
  rfid[3] = (reset_num&0xFF);
  Write_RFID_Data(reset_addr,&rfid,BLOCK_NUM);  //д��EEPROM
}

/*******************************************************************************
** ��������:  void WriteBootType(void)
** ��������:  ��EEPROMд��Ӧ��λ���͵ĸ�λ����
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void WriteBootType(void)
{
  switch(RESET_EVENT)
  {
  case RESET_SW:
    SoftResetTime = ReadResetTime(SOFT_RESET);
    SoftResetTime++;
    if(SoftResetTime > 9999)
      SoftResetTime = 0;    
    WriteResetTime(SOFT_RESET,SoftResetTime);
    break;
  case RESET_WDOG:
    WDogResetTime = ReadResetTime(WATCHDOG_RESET);
    WDogResetTime++;
    if(WDogResetTime > 9999)
      WDogResetTime = 0;
    WriteResetTime(WATCHDOG_RESET,WDogResetTime);
    break;
  case RESET_NRESET:
    break;
  case RESET_PWRLV:
    PowerResetTime = ReadResetTime(POWER_RESET);
    PowerResetTime++;
    if(PowerResetTime > 9999)
      PowerResetTime = 0;
    WriteResetTime(POWER_RESET,PowerResetTime);
    break;
  case RESET_PWRHV:
    PowerResetTime = ReadResetTime(POWER_RESET);
    PowerResetTime++;
    if(PowerResetTime > 9999)
      PowerResetTime = 0;
    WriteResetTime(POWER_RESET,PowerResetTime);
    break;
  default:
    break;
  }
}

/*******************************************************************************
** ��������:  Bubble_Sort(float sort[])  
** ��������:  ð�ݷ�������ƽ��ֵ
** ��ڲ���:  float sort[]
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
uint32_t Bubble_Sort(uint16_t sort[])                    
{
  uint8_t i,j;  
  uint16_t temp;
  uint32_t sum = 0;
  uint16_t sorted_value[30];
  for(i=0;i<30;i++)
  {
    sorted_value[i] = sort[i];
  }
  for(i=29;i>0;i--)
  {
   for(j=0;j<i;j++)
   {
     if(sorted_value[j]>sorted_value[j+1])
       {
          temp = sorted_value[j];
          sorted_value[j] = sorted_value[j+1];
          sorted_value[j+1] = temp;
       }
   }
  }
  for(i=10;i<20;i++)                    //ȡ�м�10������ ������10
  {
    sum = sum + sorted_value[i];
  }
  sum = sum/10;                         //��ƽ��ֵ
  return sum;
}
/*******************************************************************************
** ��������:  compare(float a,float b)
** ��������:  �Ƚ��������ݴ�С
** ��ڲ���:  float sort[]
** ���ڲ���:  uint8 m
** ��    ע:  
*******************************************************************************/
uint8_t compare(float a,float b)
{
   uint8_t m;
   float n;
   n = fabs(a-b);
   if(n > 3)
   {
     m = 1;
   }
   else
   {
     m = 0;
   }
   return m;
}

/*******************************************************************************
** ��������:  uint16_t AcquireVolt(void)
** ��������:  �ɼ�ADC1��ѹ30�鲢������ƽ��ֵ
** ��ڲ���:  ��
** ���ڲ���:  Voltage_EndValue
** ��    ע:  ��
*******************************************************************************/
uint16_t Voltage_EndValue;
uint16_t AcquireVolt(void)
{
  uint16_t AverageValue;
  uint16_t ADC_Data[30];
  for(uint8_t i=0;i<30;i++)
  {
    ADC_Data[i] = halMeasureVdd(7);;  
  }
  AverageValue = Bubble_Sort(ADC_Data);
  if(compare(AverageValue,Voltage_EndValue))      //���ϴ����ݶԱ��Ƿ�ͻ��
  {
    Voltage_EndValue = AverageValue;
  }
  return Voltage_EndValue;
}

uint8_t nano_carrier[]= {
							0x15,0x17,0x4f,0x41,0xAC,0xb1,0x32,0xcd,0x4f,0x41,0x04,0x05,0x06,0x5a,0x66,0xca,0x01,0x02,0x03,0x04,
							0xAC,0xca,0x54,0x5a,0x66,0x17,0xcd,0x4f,0x41,0xAC,0x4f,0x41,0x04,0x39,0xca,0x01,0x07,0x08,0x09,0x39,
							0x23,0xca,0x39,0xca,0x01,0x02,0x03,0x04,0x05,0x06,0x41,0xAC,0x4f,0x41,0x17,0x4f,0x41,0xAC,0x66,0x17,
							0x07,0x08,0x09,0x0A,0x01,0x02,0x39,0xca,0x4f,0x41,0xca,0x39,0xca,0x01,0x5a,0x66,0x17,0x17,0xcd,0x4f,
							0x04,0x05,0x06,0x5a,0x66,0x17,0x08,0x09,0xca,0x39,0x39,0xca,0x01,0x5a,0x66,0xca,0x66,0x17,0xcd,0x66
						};

extern uint8_t Nano_Carrier_Flag;
void nano_init(void);
uint32_t get_systime();
void NTRXSendMessage (uint8_t type,MyByte8T *addr, MyPtrT payload, MyWord16T len);
void Nano_TransmitCarrier_Config();
/*******************************************************************************
** ��������:  void HandleEEPROM(void)
** ��������:  ����EEPROM�Ĳ���
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void HandleEEPROM(void)
{
  uint16_t i,j;
  uint32_t t;
  while(Sleep_Flag==TRUE)                    //�ж��Ƿ�����
  {
    halPowerDown();
    int32u sleepDuration = 10000;            //ÿ10s����һ�β�ѯ�Ƿ���
    halSleepForMillisecondsWithOptions(&sleepDuration,0);
    halPowerUp();
    halInternalResetWatchDog();              //ι��
    PeripheralInit();
    EEPROMOn();                                //��Դ��
    halCommonDelayMicroseconds(1000);
    Read_WakeSleep_Config();                 //�жϻ����ֽ��Ƿ����
    EEPROMOff();                               //��Դ�ر� 
    if(Sleep_Flag==FALSE)
    {
      GreenLedOn();
      RingForSuccess();                      //������ʾ�ѻ���
      GreenLedOff();
    }
  }
  while(SelfDetect_Flag==TRUE)               //�ж��Ƿ�����Լ�ģʽ
  {
    EEPROMOn();                              //��Դ��
    halInternalDisableWatchDog(MICRO_DISABLE_WATCH_DOG_KEY); //���ÿ��Ź�
    FMQOn();
    MOTOROn();                               //���������̵�ȫ��
    GreenLedOn();
    while(!EEPROM_Idle);
    if(EEPROM_Idle)
    {
      Read_SelfDetect_Config();
      if(SelfDetect_Flag==FALSE)
      {
        FMQOff();
        MOTOROff();
        GreenLedOff();        
      }
    }
    halCommonDelayMilliseconds(3000);   
    if(SelfDetect_Flag==FALSE)
    {
      EEPROMOff();                               //��Դ�ر� 
      halInternalEnableWatchDog();               //���´򿪿��Ź�
    }
  }    
  if(TransmitStream_Flag==TRUE)            //�ж��Ƿ���뷢���ز�ģʽ
  {
    EEPROMOn();                              //��Դ��
    halInternalDisableWatchDog(MICRO_DISABLE_WATCH_DOG_KEY); //���ÿ��Ź�
    ST_RadioEnableOverflowNotification(TRUE);
    ST_RadioSetPowerMode(ST_TX_POWER_MODE_DEFAULT);    
    ST_RadioEnableAddressFiltering(FALSE);
    ST_RadioEnableAutoAck(FALSE);
    ST_RadioSetPower(RF_Power);
    ST_RadioSetChannel(Send_Channel);     
    ST_RadioInit(ST_RADIO_POWER_MODE_OFF);         
    while(TransmitStream_Flag==TRUE)
    {
      RedLedOn();
      ST_RadioStartTransmitStream();      
      if(EEPROM_Idle)
      {
        TransmitStream_Config();
        RF_Power = RF_Power_Config();         //���ڲ���ʱ�ı䷢�书��
      }
      halCommonDelayMilliseconds(3000); 
      ST_RadioSetPower(RF_Power);
      if(TransmitStream_Flag==FALSE)
      {
        RedLedOff();
        EEPROMOff();                               //��Դ�ر� 
        ST_RadioSleep();
        ST_RadioStopTransmitStream();
        halInternalEnableWatchDog();               //���´򿪿��Ź�
      }
    }      
  }
  if(Nano_Carrier_Flag == TRUE){					//����nano�ز�ģʽ
  	halInternalDisableWatchDog(MICRO_DISABLE_WATCH_DOG_KEY); //���ÿ��Ź�
	EEPROMOn();
	nano_init();
	NTRXSetRegister( NA_TxArq, FALSE);
	t = (get_systime()+1);
	GreenLedOn();
	while(1){
	  for(i=0;i<5;i++){
	  	j=(i*20);
	  	NTRXSendMessage(0,&nano_carrier[j],&nano_carrier[j],20);
	  }
	  if(get_systime()>=t){							//ÿ��1s��һ��eeprom,��У׼һ������
	  	t = (get_systime()+1);
		NTRXAllCalibration ();						//У׼nano����
		if(EEPROM_Idle)
			Nano_TransmitCarrier_Config();			//��eeprom����
	  }
	  if(Nano_Carrier_Flag == FALSE){				//�˳��ز�ģʽ
	  	GreenLedOff();
		NTRXSetRegister( NA_TxArq, TRUE);
		EEPROMOff();
		halInternalEnableWatchDog();                //���´򿪿��Ź�
		break;
	  }
	}
	EEPROMOff();
  }
}

/*******************************************************************************
** ��������:  void ValidWarning(void)
** ��������:  ������Ч�������������ʾ
** ��ڲ���:  ��
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void ValidWarning(void)
{
  FMQOn();
  MOTOROn();  
  halCommonDelayMilliseconds(200);      //������Ч�������������ʾ
  FMQOff();
  MOTOROff();
}

/*******************************************************************************
** ��������:  void WriteFactDate(uint16_t date_byte)
** ��������:  ��EEPROMд��������
** ��ڲ���:  uint16_t date_byte
** ���ڲ���:  ��
** ��    ע:  ��
*******************************************************************************/
void WriteFactDate(uint16_t date_byte)
{
  uint8_t rfid[4];
  rfid[0] = 0x11;
  rfid[1] = 0x8E;  
  rfid[2] = date_byte>>8;  
  rfid[3] = date_byte&0x00FF;
  EEPROMOn();                                //��Դ��  
  halCommonDelayMicroseconds(1000);
  Write_RFID_Data(CARD_DELIVERY_TIME_ADDR,&rfid,BLOCK_NUM);  //д��EEPROM
  EEPROMOff();  
}

/*******************************************************************************
** ��������: uint16_t ReadFactDate(void)
** ��������:  ��EEPROM��ȡ����ʱ��
** ��ڲ���:  ��
** ���ڲ���:  ����ʱ��
** ��    ע:  ��
*******************************************************************************/
uint16_t ReadFactDate(void)
{
  uint8_t rfid[4];  
  uint16_t Date_Byte;
  EEPROMOn();                                //��Դ��   
  halCommonDelayMicroseconds(1000);
  Read_RFID_Data(CARD_DELIVERY_TIME_ADDR,&rfid,BLOCK_NUM);  //��ȡEEPROM����ʱ��
  EEPROMOff();  
  if((rfid[0]==0x11)&&(rfid[1]==0x8E))
  {    
    Date_Byte = (rfid[2]<<8)|rfid[3];
    return Date_Byte;
  }
  else
    return FALSE;
}