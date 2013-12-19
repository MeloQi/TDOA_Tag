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
uint16_t SoftResetTime,WDogResetTime,PowerResetTime; //启动类型
extern uint8_t Send_Channel,Recieve_Channel;     //发送接收信道
extern Disp disp;
extern uint8_t RF_Power;
/*******************************************************************************
** 函数名称:  uint16_t ReadResetTime(uint8_t reset_type)
** 函数功能:  从EEPROM读取复位次数
** 入口参数:  reset_type
** 出口参数:  对应复位类型的复位次数
** 备    注:  无
*******************************************************************************/
uint16_t ReadResetTime(uint8_t reset_type)
{
  uint8_t rfid[4];
  switch(reset_type)
  {
  case SOFT_RESET:
    Read_RFID_Data(CARD_SOFT_RESET_ADDR,&rfid,BLOCK_NUM);  //读取EEPROM软复位次数
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
    Read_RFID_Data(CARD_WDOG_RESET_ADDR,&rfid,BLOCK_NUM);  //读取EEPROM狗复位次数
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
    Read_RFID_Data(CARD_POWER_RESET_ADDR,&rfid,BLOCK_NUM);  //读取EEPROM掉电复位次数
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
** 函数名称:  void WriteResetTime(uint8_t reset_type,uint16_t reset_num)
** 函数功能:  向EEPROM写对应复位类型的复位次数
** 入口参数:  uint8_t reset_type,uint16_t reset_num
** 出口参数:  无
** 备    注:  无
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
  Write_RFID_Data(reset_addr,&rfid,BLOCK_NUM);  //写到EEPROM
}

/*******************************************************************************
** 函数名称:  void WriteBootType(void)
** 函数功能:  向EEPROM写对应复位类型的复位次数
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
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
** 函数名称:  Bubble_Sort(float sort[])  
** 函数功能:  冒泡法排序并求平均值
** 入口参数:  float sort[]
** 出口参数:  无
** 备    注:  无
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
  for(i=10;i<20;i++)                    //取中间10组数据 并除以10
  {
    sum = sum + sorted_value[i];
  }
  sum = sum/10;                         //求平均值
  return sum;
}
/*******************************************************************************
** 函数名称:  compare(float a,float b)
** 函数功能:  比较两个数据大小
** 入口参数:  float sort[]
** 出口参数:  uint8 m
** 备    注:  
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
** 函数名称:  uint16_t AcquireVolt(void)
** 函数功能:  采集ADC1电压30组并排序求平均值
** 入口参数:  无
** 出口参数:  Voltage_EndValue
** 备    注:  无
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
  if(compare(AverageValue,Voltage_EndValue))      //与上次数据对比是否突变
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
** 函数名称:  void HandleEEPROM(void)
** 函数功能:  处理EEPROM的操作
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void HandleEEPROM(void)
{
  uint16_t i,j;
  uint32_t t;
  while(Sleep_Flag==TRUE)                    //判断是否休眠
  {
    halPowerDown();
    int32u sleepDuration = 10000;            //每10s唤醒一次查询是否唤醒
    halSleepForMillisecondsWithOptions(&sleepDuration,0);
    halPowerUp();
    halInternalResetWatchDog();              //喂狗
    PeripheralInit();
    EEPROMOn();                                //电源打开
    halCommonDelayMicroseconds(1000);
    Read_WakeSleep_Config();                 //判断唤醒字节是否更改
    EEPROMOff();                               //电源关闭 
    if(Sleep_Flag==FALSE)
    {
      GreenLedOn();
      RingForSuccess();                      //蜂鸣表示已唤醒
      GreenLedOff();
    }
  }
  while(SelfDetect_Flag==TRUE)               //判断是否进入自检模式
  {
    EEPROMOn();                              //电源打开
    halInternalDisableWatchDog(MICRO_DISABLE_WATCH_DOG_KEY); //禁用看门狗
    FMQOn();
    MOTOROn();                               //马达蜂鸣器绿灯全开
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
      EEPROMOff();                               //电源关闭 
      halInternalEnableWatchDog();               //重新打开看门狗
    }
  }    
  if(TransmitStream_Flag==TRUE)            //判断是否进入发送载波模式
  {
    EEPROMOn();                              //电源打开
    halInternalDisableWatchDog(MICRO_DISABLE_WATCH_DOG_KEY); //禁用看门狗
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
        RF_Power = RF_Power_Config();         //可在测试时改变发射功率
      }
      halCommonDelayMilliseconds(3000); 
      ST_RadioSetPower(RF_Power);
      if(TransmitStream_Flag==FALSE)
      {
        RedLedOff();
        EEPROMOff();                               //电源关闭 
        ST_RadioSleep();
        ST_RadioStopTransmitStream();
        halInternalEnableWatchDog();               //重新打开看门狗
      }
    }      
  }
  if(Nano_Carrier_Flag == TRUE){					//进入nano载波模式
  	halInternalDisableWatchDog(MICRO_DISABLE_WATCH_DOG_KEY); //禁用看门狗
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
	  if(get_systime()>=t){							//每隔1s读一次eeprom,并校准一次无线
	  	t = (get_systime()+1);
		NTRXAllCalibration ();						//校准nano无线
		if(EEPROM_Idle)
			Nano_TransmitCarrier_Config();			//读eeprom配置
	  }
	  if(Nano_Carrier_Flag == FALSE){				//退出载波模式
	  	GreenLedOff();
		NTRXSetRegister( NA_TxArq, TRUE);
		EEPROMOff();
		halInternalEnableWatchDog();                //重新打开看门狗
		break;
	  }
	}
	EEPROMOff();
  }
}

/*******************************************************************************
** 函数名称:  void ValidWarning(void)
** 函数功能:  按键有效蜂鸣器马达振动提示
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void ValidWarning(void)
{
  FMQOn();
  MOTOROn();  
  halCommonDelayMilliseconds(200);      //按键有效蜂鸣器马达振动提示
  FMQOff();
  MOTOROff();
}

/*******************************************************************************
** 函数名称:  void WriteFactDate(uint16_t date_byte)
** 函数功能:  向EEPROM写出厂日期
** 入口参数:  uint16_t date_byte
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void WriteFactDate(uint16_t date_byte)
{
  uint8_t rfid[4];
  rfid[0] = 0x11;
  rfid[1] = 0x8E;  
  rfid[2] = date_byte>>8;  
  rfid[3] = date_byte&0x00FF;
  EEPROMOn();                                //电源打开  
  halCommonDelayMicroseconds(1000);
  Write_RFID_Data(CARD_DELIVERY_TIME_ADDR,&rfid,BLOCK_NUM);  //写到EEPROM
  EEPROMOff();  
}

/*******************************************************************************
** 函数名称: uint16_t ReadFactDate(void)
** 函数功能:  从EEPROM读取出厂时间
** 入口参数:  无
** 出口参数:  出厂时间
** 备    注:  无
*******************************************************************************/
uint16_t ReadFactDate(void)
{
  uint8_t rfid[4];  
  uint16_t Date_Byte;
  EEPROMOn();                                //电源打开   
  halCommonDelayMicroseconds(1000);
  Read_RFID_Data(CARD_DELIVERY_TIME_ADDR,&rfid,BLOCK_NUM);  //读取EEPROM出厂时间
  EEPROMOff();  
  if((rfid[0]==0x11)&&(rfid[1]==0x8E))
  {    
    Date_Byte = (rfid[2]<<8)|rfid[3];
    return Date_Byte;
  }
  else
    return FALSE;
}