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
uint8_t Sleep_Flag;                 //唤醒标志位               
uint8_t SelfDetect_Flag;            //自检标志位
uint8_t TransmitStream_Flag;        //发送载波标志位
uint8_t RFID_Buf[60];               //RFID 接收缓冲区
uint16_t Card_ID;                   //人员标识卡号
volatile bool_t EEPROM_Status;		//状态标志		

extern unsigned short card_id;
extern uint8_t Card_Cmd[];          //协议数据
extern uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr);
extern void AT24CXX_WriteData(uint16_t StartAddr,uint8_t length,uint8_t *write_data);
 uint8_t array_compare(uint8_t *buffer1,uint8_t *buffer2,uint8_t length);
/*******************************************************************************
** 函数名称:  void Read_RFID_Data(uint16_t reg, const void *data, size_t count)
** 函数功能:  读取EEPROM信息
** 入口参数:  读取地址，存放地址，数量
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void Read_RFID_Data(uint16_t reg, void *data, uint8_t count)
{
  I2cInit(I2C_SPEED_FAST);
  I2cRead(User_Memory,reg,data,count);
}

/*******************************************************************************
** 函数名称:  void Read_Control_Reg(uint16_t reg, void *data, size_t count)
** 函数功能:  读取EEPROM信息
** 入口参数:  读取地址，存放地址，数量
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void Read_Control_Reg(uint16_t reg, void *data, uint8_t count)
{
  I2cInit(I2C_SPEED_FAST);
  I2cRead(Sys_Memory,reg,data,count);
}

/*******************************************************************************
** 函数名称:  void Write_Control_Reg(uint16_t reg, const void *data, size_t count)
** 函数功能:  写EEPROM寄存器信息
** 入口参数:  写地址，存放地址，数量
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void Write_Control_Reg(uint16_t reg, const void *data, uint8_t count)
{
  I2cInit(I2C_SPEED_STANDARD);
  I2cWrite(Sys_Memory,reg,data,count);
  halCommonDelayMilliseconds(6);                //写操作至少需要5ms
}
/*******************************************************************************
** 函数名称:  void Write_RFID_Data(uint16_t reg, const void *data, size_t count)
** 函数功能:  写EEPROM信息
** 入口参数:  读取地址，存放地址，数量
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void Write_RFID_Data(uint16_t reg, const void *data, uint8_t count)
{
  I2cInit(I2C_SPEED_STANDARD);
  I2cWrite(User_Memory,reg,data,count);
  halCommonDelayMilliseconds(6);                //写操作至少需要5ms
}
/*******************************************************************************
** 函数名称:  void Read_CardID_Config()
** 函数功能:  卡号设置
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void Read_CardID_Config()
{  
	uint8_t rfid[4];
	if(EEPROM_Status){
		//EEPROMOn();                                						//电源打开
    	Read_RFID_Data(CARD_NUM_ADDR,rfid,BLOCK_NUM);       	//读取EEPROM配置信息
    	//    EEPROMOff();                              						//电源关闭 
    	Card_ID=(rfid[2]<<8)|(rfid[3]);
     	if((rfid[0]==0x11)&&(rfid[1]==0x86)&&(Card_ID>=1)&&(Card_ID<=8000)){//卡号1~8000之内         		
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
  		Card_Cmd[9] = (card_id&0xff);     									//低位前，高位后
    	Card_Cmd[10]= (card_id>>8)&0xff;
  	}
}
/*******************************************************************************
** 函数名称:  void Read_WakeSleep_Config()
** 函数功能:  休眠/唤醒设置
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void Read_WakeSleep_Config()
{
  uint8_t rfid[4];
//  EEPROMOn();                                //电源打开
//  halCommonDelayMicroseconds(1000);
  Read_RFID_Data(CARD_SLEEP_ADDR,&rfid,BLOCK_NUM);       //读取EEPROM配置信息
//  EEPROMOff();                               //电源关闭 
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
** 函数名称:  void Read_SelfDetect_Config()
** 函数功能:  卡自检设置
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void Read_SelfDetect_Config()
{
  uint8_t rfid[4];
//  EEPROMOn();                                //电源打开
//  halCommonDelayMicroseconds(1000);
  Read_RFID_Data(CARD_DETECT_ADDR,&rfid,BLOCK_NUM);       //读取EEPROM配置信息
//  EEPROMOff();                               //电源关闭 
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
** 函数名称:  uint8_t RF_Power_Config()
** 函数功能:  发射功率设置
** 入口参数:  无
** 出口参数:  发射功率值
** 备    注:  无
*******************************************************************************/
uint8_t RF_Power_Config()
{
  uint8_t rf_power;
  uint8_t rfid[4];
//  EEPROMOn();                                //电源打开
  Read_RFID_Data(CARD_SEND_POWER_ADDR,&rfid,BLOCK_NUM);       //读取EEPROM配置信息
//  EEPROMOff();                               //电源关闭 
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
** 函数名称:  uint8_t RF_Send_Channel()
** 函数功能:  发射信道设置
** 入口参数:  无
** 出口参数:  发射功率值
** 备    注:  无
*******************************************************************************/
uint8_t RF_Send_Channel()
{
  uint8_t send_channel;
  uint8_t rfid[4];
//  EEPROMOn();                                //电源打开
  Read_RFID_Data(CARD_SEND_CHANNEL_ADDR,&rfid,BLOCK_NUM);       //读取EEPROM配置信息
//  EEPROMOff();                               //电源关闭 
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
** 函数名称:  uint8_t RF_Recieve_Channel()
** 函数功能:  接收信道设置
** 入口参数:  无
** 出口参数:  发射功率值
** 备    注:  无
*******************************************************************************/
uint8_t RF_Recieve_Channel()
{
  uint8_t recieve_channel;
  uint8_t rfid[4];
//  EEPROMOn();                                //电源打开
  Read_RFID_Data(CARD_RECV_CHANNEL_ADDR,&rfid,BLOCK_NUM);       //读取EEPROM配置信息
//  EEPROMOff();                               //电源关闭 
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
** 函数名称:  void TransmitStream_Config()
** 函数功能:  测试载波设置
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void TransmitStream_Config()
{
  uint8_t rfid[4];
//  EEPROMOn();                                //电源打开
//  halCommonDelayMicroseconds(500);
  Read_RFID_Data(CARD_CARRIER_ADDR,&rfid,BLOCK_NUM);       //读取EEPROM配置信息
//  EEPROMOff();                               //电源关闭 
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
** 函数名称:  void WriteBATValue(int16u value)
** 函数功能:  将电池电压写入到EEPROM
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void WriteBATValue(int16u value)
{
  uint8_t rfid[4];
  rfid[0] = 0x11;
  rfid[1] = 0x87;
  rfid[2] = (value>>8)&0xFF;   //写到EEPROM
  rfid[3] = (value&0xFF);
//  EEPROMOn();
//  halCommonDelayMicroseconds(1000);
  Write_RFID_Data(CARD_BAT_ADDR,&rfid,BLOCK_NUM);
//  EEPROMOff();
}

/*******************************************************************************
** 函数名称:  void WriteSendFailTime(uint16_t time)
** 函数功能:  将发送失败次数写入到EEPROM
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
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
** 函数名称:  void WriteSendFailTime(uint16_t time)
** 函数功能:  将发送失败次数写入到EEPROM
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void WriteSendFailTime(uint16_t time)
{
  uint8_t rfid[4];
  rfid[0] = 0x11;
  rfid[1] = 0x89;
  rfid[2] = (time>>8)&0xFF;   //写到EEPROM
  rfid[3] = (time&0xFF);
//  EEPROMOn();
  Write_RFID_Data(CARD_SEND_FAIL_ADDR,&rfid,BLOCK_NUM);
//  EEPROMOff();
}

/*******************************************************************************
** 函数名称:  uint16_t ReadRecvSuccessTime(void)
** 函数功能:  读取双向通信次数
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
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
** 函数名称:  void WriteRecvSuccessTime(uint16_t time)
** 函数功能:  将双向通信次数写入到EEPROM
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void WriteRecvSuccessTime(uint16_t time)
{
  uint8_t rfid[4];
  rfid[0] = 0x11;
  rfid[1] = 0x8A;
  rfid[2] = (time>>8)&0xFF;   //写到EEPROM
  rfid[3] = (time&0xFF);
  EEPROMOn();
  Write_RFID_Data(CARD_BIDIRECT_ADDR,&rfid,BLOCK_NUM);
  EEPROMOff();
}

/*******************************************************************************
** 函数名称:  uint16_t ReadAbnormalWakeTime(void)
** 函数功能:  读取非正常唤醒的次数
** 入口参数:  无
** 出口参数:  次数
** 备    注:  无
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
** 函数名称:  void WriteAbnormalWakeTime(uint16_t time)
** 函数功能:  将非正常唤醒的次数写入到EEPROM
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void WriteAbnormalWakeTime(uint16_t time)
{
  uint8_t rfid[4];
  rfid[0] = 0x11;
  rfid[1] = 0x8F;
  rfid[2] = (time>>8)&0xFF;   //写到EEPROM
  rfid[3] = (time&0xFF);
  EEPROMOn();
  Write_RFID_Data(CARD_ABNORMAL_WAKE_ADDR,&rfid,BLOCK_NUM);
  EEPROMOff();
}

uint8_t Nano_Carrier_Flag;
/*******************************************************************************
** 函数名称:  void Nano_TransmitCarrier_Config()
** 函数功能:  测试NANO载波设置
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void Nano_TransmitCarrier_Config()
{
  uint8_t rfid[4];
//  EEPROMOn();                                //电源打开
//  halCommonDelayMicroseconds(500);
  Read_RFID_Data(CARD_NANO_CARRIER_ADDR,&rfid,BLOCK_NUM);       //读取EEPROM配置信息
//  EEPROMOff();                               //电源关闭 
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
** 函数名称:  uint8_t EEPROM_SelfDetect()
** 函数功能:  先写入一组数据，然后读出，判断是否相同
** 入口参数:  无
** 出口参数:  成功标志位
** 备    注:  无
*******************************************************************************/
 uint8_t EEPROM_SelfDetect()
 {
   uint8_t selfdetect_flag;
   uint8_t detect_time=0;
   uint16_t RandomNum[2];               // 16位随机数
   uint8_t randomNum[4],checknum[4];    // 8位随机数和读出的数据
   do
   {
     ST_RadioGetRandomNumbers(&RandomNum[0],2);          //获取1个随机数
     randomNum[0] = RandomNum[0]&0x00FF;
     randomNum[1] = (RandomNum[0]>>8)&0xFF;              //先低后高
     randomNum[2] = RandomNum[1]&0x00FF;
     randomNum[3] = (RandomNum[1]>>8)&0xFF;              //先低后高
     Write_RFID_Data(0x007C,&randomNum[0],4);            //先写入数据
     Read_RFID_Data(0x007C,&checknum[0],4);              //读出数据，比较是否相同      
     if(array_compare(&randomNum[0],&checknum[0],4)!=TRUE)
     {
       selfdetect_flag = FALSE;
     }
     else
     {
       selfdetect_flag = TRUE; 
     }
     detect_time++;
   }while((selfdetect_flag==FALSE)&&(detect_time<=3));//如果失败，连续自检3次
   return selfdetect_flag;
 }

/*******************************************************************************
** 函数名称:  uint8_t EEPROM_Fist_Boot()
** 函数功能:  判断是否为第一次上电
** 入口参数:  无
** 出口参数:  1表示第一次上电，0表示非第一次上电
** 备    注:  无
*******************************************************************************/
uint8_t EEPROM_Fist_Boot()
{
	uint8_t rfid[4];
  	Read_RFID_Data(CARD_IS_FIST_BOOT_ADDR,&rfid,BLOCK_NUM);       //读取EEPROM配置信息
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
** 函数名称:   EEPROM_Process()
** 函数功能:  eeprom主处理函数
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void EEPROM_Process(void)
{
	if((EEPROM_Status)&&(EEPROM_Idle))
    {   
        EEPROMOn();                                	//电源打开
        halCommonDelayMicroseconds(1000);
        RF_Power = RF_Power_Config();              	//发射功率设置        
        Read_SelfDetect_Config();                  	//自检功能      
        TransmitStream_Config();                   	//发送载波测试
        Read_WakeSleep_Config();                   	//休眠/唤醒
    }
    Read_CardID_Config();                        	//读取卡号信息
    EEPROMOff();                               		//电源关闭 

	HandleEEPROM();									//根据EEPROM信息处理函数
}
