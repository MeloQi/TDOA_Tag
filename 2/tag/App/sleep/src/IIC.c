/********************************Copyright (c)**********************************
**                       
**                        
**-File Info--------------------------------------------------------------------
** File name:			App.c
** Last Version:		V1.0
** Descriptions:		RFID&&2.4GHz 矿用人员定位标识卡读写器
**------------------------------------------------------------------------------
** Created by:	            QI
** Created date:               2012-12-12
** Version:	               V1.0
** Descriptions:    新型标识卡读写器，使用ISO15693标准的RFID芯片CR95HF,可使标识卡进入
**                  休眠或唤醒，更改卡号，自检，载波，设置发送功率等功能。
*******************************************************************************/
//#include PLATFORM_HEADER
//#include "board.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include "error.h"
//#include "hal.h"
//#include "phy-library.h"
//#include <stdint.h>
//#include "regs.h"
//#include "stm32w108xx_conf.h"
//#include "System.h"
#include "regs.h"
#include "iar.h"
#include "i2c.h"
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
#include "App.h"

//SCL-PB2   SDA-PB1
//#define SCL_L()     GPIO_ResetBits(GPIOB,GPIO_Pin_2)
//#define SCL_H()     GPIO_SetBits(GPIOB,GPIO_Pin_2)
//#define SDA_L()     GPIO_ResetBits(GPIOB,GPIO_Pin_1)
//#define SDA_H()     GPIO_SetBits(GPIOB,GPIO_Pin_1)
#define SCL_L()     (GPIO_PBOUT &=~PB2)
#define SCL_H()     (GPIO_PBOUT |= PB2)
#define SDA_L()     (GPIO_PBOUT &=~PB1)
#define SDA_H()     (GPIO_PBOUT |= PB1)

//IO方向设置
#define SDA_IN()     GPIO_PBCFGL = (GPIOCFG_IN<<PB1_CFG_BIT)
#define SDA_OUT()    GPIO_PBCFGL = (GPIOCFG_OUT<<PB1_CFG_BIT)
#define READ_SDA     GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)

/*******************************************************************************
** 函数名称:  void IIC_IO_Init()
** 函数功能:  IIC所用的GPIO管脚初始化
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void IIC_IO_Init(void)
{
  GPIO_InitTypeDef    gpioInitType;  
  gpioInitType.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_4;         //PB3-SDA PB4-SCL
  gpioInitType.GPIO_Mode=GPIO_Mode_OUT_PP;
  GPIO_Init(GPIOB,&gpioInitType);
}

/*******************************************************************************
** 函数名称:  void IIC_Start()
** 函数功能:  IIC起始信号
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void IIC_Start(void)
{
  SDA_OUT();            //SDA线输出
  SDA_H();	  	  
  SCL_H();
  halCommonDelayMicroseconds(4);
  SDA_L();
  halCommonDelayMicroseconds(4);
  SCL_L();              //钳住I2C总线，准备发送或接收数据 
}

/*******************************************************************************
** 函数名称:  void IIC_Stop()
** 函数功能:  IIC停止信号
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void IIC_Stop(void)
{
  SDA_OUT();            //SDA线输出
  SCL_L();
  SDA_L();
  halCommonDelayMicroseconds(4);
  SCL_H(); 
  SDA_H();              //发送I2C总线结束信号
  halCommonDelayMicroseconds(4);							   	
}
/*******************************************************************************
** 函数名称:  void IIC_Wait_Ack()
** 函数功能:  等待应答信号到来
** 入口参数:  无
** 出口参数:  1，接收应答失败
**            0，接收应答成功
** 备    注:  无
*******************************************************************************/
uint8_t IIC_Wait_Ack(void)
{
  uint8_t ucErrTime=0;
  SDA_IN();             //SDA设置为输入  
  SDA_H();
  halCommonDelayMicroseconds(1);	   
  SCL_H();
  halCommonDelayMicroseconds(1);	 
  while(READ_SDA==Bit_SET)
  {
    ucErrTime++;
    if(ucErrTime>250)
    {
      IIC_Stop();
      return 1;
    }
  }
  SCL_L();              //时钟输出0 	   
  return 0;  
} 
/*******************************************************************************
** 函数名称:  void IIC_Ack()
** 函数功能:  ACK应答
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void IIC_Ack(void)
{
  SCL_L();
  SDA_OUT();
  SDA_L();
  halCommonDelayMicroseconds(2);
  SDA_H();
  halCommonDelayMicroseconds(2);
  SCL_L();
}
/*******************************************************************************
** 函数名称:  void IIC_NAck()
** 函数功能:  不产生ACK应答	
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void IIC_NAck(void)
{
  SCL_L();
  SDA_OUT();
  SDA_H();
  halCommonDelayMicroseconds(2);
  SCL_H();
  halCommonDelayMicroseconds(2);
  SCL_L();
}					 				     

/*******************************************************************************
** 函数名称:  void IIC_Send_Byte()
** 函数功能:  IIC发送一个字节	
** 入口参数:  无
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void IIC_Send_Byte(uint8_t txd)
{                        
  uint8_t t;   
  SDA_OUT(); 	    
  SCL_L();              //拉低时钟开始数据传输
  for(t=0;t<8;t++)
  {              
    if((txd&0x80)>>7)
      SDA_H();
    else
      SDA_L();
    txd<<=1; 	  
    halCommonDelayMicroseconds(2);
    SCL_H();
    halCommonDelayMicroseconds(2);
    SCL_L();	
    halCommonDelayMicroseconds(2);
  }	 
} 

/*******************************************************************************
** 函数名称:  void IIC_Read_Byte()
** 函数功能:  IIC读一个字节	
** 入口参数:  无
** 出口参数:  无
** 备    注:  ack=1时，发送ACK，ack=0，发送nACK 
*******************************************************************************/
uint8_t IIC_Read_Byte(uint8_t ack)
{
  uint8_t i,receive=0;
  SDA_IN();                     //SDA设置为输入
  for(i=0;i<8;i++ )
  {
    SCL_L();
    halCommonDelayMicroseconds(2);
    SCL_H();
    receive<<=1;
    if(READ_SDA==Bit_SET)
      receive++;   
    halCommonDelayMicroseconds(1); 
  }					 
  if (!ack)
    IIC_NAck();//发送nACK
  else
    IIC_Ack(); //发送ACK   
  return receive;
}

/*******************************************************************************
** 函数名称:  void AT24CXX_ReadOneByte()
** 函数功能:  在AT24CXX指定地址读出一个数据
** 入口参数:  ReadAddr:开始读数的地址
** 出口参数:  读到的数据
** 备    注:  无
*******************************************************************************/
uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr)
{				  
  uint8_t temp=0;		  	    																 
  IIC_Start();  
  IIC_Send_Byte((User_Memory<<1)|0x00);	            //发送写命令
  IIC_Wait_Ack();
  IIC_Send_Byte(ReadAddr>>8);       //发送高地址	       
  IIC_Wait_Ack(); 
  IIC_Send_Byte(ReadAddr&0x00FF);   //发送低地址
  IIC_Wait_Ack();	    
  IIC_Start();  	 	   
  IIC_Send_Byte((User_Memory<<1)|0x01);             //进入接收模式			   
  IIC_Wait_Ack();	 
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();                      //产生一个停止条件	    
  return temp;
}

/*******************************************************************************
** 函数名称:  uint8_t AT24CXX_ReadData(uint16_t StartAddr,uint8_t length,uint8_t *read_data)
** 函数功能:  在AT24CXX指定地址读出长度为length的数据
** 入口参数:  StartAddr:开始读数的地址
** 出口参数:  读到的数据
** 备    注:  无
*******************************************************************************/
void AT24CXX_ReadData(uint16_t StartAddr,uint8_t length,uint8_t *read_data)
{				  
  for(uint8_t i=0;i<length;i++)
  {
    read_data[i] = AT24CXX_ReadOneByte(StartAddr+i);
  }
}
/*******************************************************************************
** 函数名称:  void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
** 函数功能:  在AT24CXX指定地址写入一个数据	
** 入口参数:  WriteAddr  :写入数据的目的地址   DataToWrite:要写入的数据
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
  IIC_Start();   
  IIC_Send_Byte((User_Memory<<1)|0x00);	                //发送写命令
  IIC_Wait_Ack();
  IIC_Send_Byte(WriteAddr>>8);          //发送高地址	  
  IIC_Wait_Ack();	   
  IIC_Send_Byte(WriteAddr&0x00FF);      //发送低地址
  IIC_Wait_Ack(); 	 										  		   
  IIC_Send_Byte(DataToWrite);           //发送字节							   
  IIC_Wait_Ack();  		    	   
  IIC_Stop();                           //产生一个停止条件 
  halCommonDelayMilliseconds(10);       //加延时
}

/*******************************************************************************
** 函数名称:  void AT24CXX_WriteData(uint16_t StartAddr,uint8_t length,uint8_t *write_data)
** 函数功能:  在AT24CXX指定地址写长度为length的数据
** 入口参数:  StartAddr:开始读数的地址
** 出口参数:  无
** 备    注:  无
*******************************************************************************/
void AT24CXX_WriteData(uint16_t StartAddr,uint8_t length,uint8_t *write_data)
{				  
  for(uint8_t i=0;i<length;i++)
  {
    AT24CXX_WriteOneByte(StartAddr+i,write_data[i]);
  }
}





/********************************Copyright (c)**********************************
**                         江苏三恒科技集团有限公司
**                          http://www.3shine.com
**-File Info--------------------------------------------------------------------
** File name:			i2c.c
** Last Version:		
** Descriptions:		
**------------------------------------------------------------------------------
** Created by:	               张国良
** Created date:               2012-10-19
** Version:	               V1.0
** Descriptions:    
**                  
*******************************************************************************/
#include "regs.h"
#include "iar.h"
#include "i2c.h"

/********************************************
* Defines *
********************************************/
#define User_Memory    0x53     //用户存储区地址
#define Sys_Memory     0x57     //系统存储区地址

#define Page_Write     4        //逐页写操作
#define Byte_Write     1        //逐字节写操作

/********************************************
* Function defines *
********************************************/
/* *///modified by zgl,using SC1--PB1&PB2
void I2cInit(uint32_t speed)
{
//    SC1_MODE =  SC1_MODE_I2C;
    
    GPIO_PBCFGL_REG &= ~(0x0F<<(1*4));
    GPIO_PBCFGL_REG |= (GPIOCFG_OUT_ALT<<(1*4));
    
    GPIO_PBCFGL_REG &= ~(0x0F<<(2*4));
    GPIO_PBCFGL_REG |= (GPIOCFG_OUT_ALT<<(2*4));
    
    SC1_RATELIN =  14;   // generates standard 100kbps or 400kbps
    
    if (speed == I2C_SPEED_STANDARD)
    {
        SC1_RATEEXP =  3;    // 3 yields 100kbps; 1 yields 400kbps   
    }
    else if (speed == I2C_SPEED_FAST)
    {
        SC1_RATEEXP =  1;    // 3 yields 100kbps; 1 yields 400kbps
    }
}

/*******************************************************************************
* Function Name  : I2cFrameSend
* Description    : It sends I2C frame 
* Input          : DeviceAddress is the destination device address
*                  pBUffer is the buffer data
*                  NoOfBytes is the number of bytes
* Output         : None
* Return         : status
*******************************************************************************/
static uint8_t I2cFrameSend (uint8_t dev, uint16_t reg, uint8_t *pBuffer, uint8_t NoOfBytes)
{
  IIC_Start();   
  IIC_Send_Byte((dev<<1)|0x00);	                //发送写命令
  IIC_Wait_Ack();
  IIC_Send_Byte((reg>>16)&0x00FF);          //发送高地址	  
  IIC_Wait_Ack();	   
  IIC_Send_Byte(reg&0x00FF);      //发送低地址
  IIC_Wait_Ack(); 	
  for (i=0; i<NoOfBytes; i++) 
  {
    IIC_Send_Byte(*pBuffer ++);           //发送字节							   
    IIC_Wait_Ack();  
  }
  IIC_Stop();                           //产生一个停止条件 
  halCommonDelayMilliseconds(10);       //加延时 
  return SUCCESS;
}/* end I2cFrameSend() */

/*******************************************************************************
* Function Name  : I2cFrameRecv
* Description    : It receives an I2C frame and stores it in pBUffer parameter
* Input          : dev is the slave address
*                  reg is the register address
*                  NoOfBytes is the numenr of bytes to read starting from reg_addr
* Output         : pBuffer
* Return         : status
*******************************************************************************/
static uint8_t I2cFrameRecv (uint8_t dev, uint16_t reg, uint8_t *pBuffer, uint16_t NoOfBytes)
{
  uint8_t temp=0;		  	    																 
  IIC_Start();  
  IIC_Send_Byte((dev<<1)|0x00);	            //发送写命令
  IIC_Wait_Ack();
  IIC_Send_Byte((addr>>16)&0x00FF);       //发送高地址	       
  IIC_Wait_Ack(); 
  IIC_Send_Byte(addr&0x00FF);   //发送低地址
  IIC_Wait_Ack();	    
  IIC_Start();  	 	   
  IIC_Send_Byte((dev<<1)|0x01);             //读操作		   
  IIC_Wait_Ack();
//  for (i=0;i<NoOfBytes;i++)
//  {  
//    temp=IIC_Read_Byte(0);		   
    for (i=0;i<NoOfBytes;i++)
    {        
        if (i < (NoOfBytes - 1))
            SC1_TWICTRL2 |= SC_TWIACK;   // ack on receipt of data
        else
            SC1_TWICTRL2 &= ~SC_TWIACK;  // don't ack if last one
        
        SC1_TWICTRL1 |= SC_TWIRECV;    // set to receive
        WAIT_RX_FIN();
        *(pBuffer+i) = SC1_DATA;       // receive data                   
    }
  IIC_Stop();                      //产生一个停止条件	    
  return temp;
  
  
  
    uint8_t i;
    uint16_t addr = reg;
    
    SC1_TWICTRL1 |= SC_TWISTART;   // send start
    WAIT_CMD_FIN();
    SEND_BYTE((dev<<1)|0x00);     // 写操作
    WAIT_TX_FIN();
    SEND_BYTE((addr>>16)&0x00FF); // 地址高字节  先高后低 
    WAIT_TX_FIN();
    SEND_BYTE(addr&0x00FF);       // 地址低字节         
    WAIT_TX_FIN();   
    SC1_TWICTRL1 |= SC_TWISTART;  // send start
    WAIT_CMD_FIN();
    SEND_BYTE((dev<<1)|0x01);     // 读操作
    WAIT_TX_FIN();
    // loop receiving the data
    
    for (i=0;i<NoOfBytes;i++)
    {        
        if (i < (NoOfBytes - 1))
            SC1_TWICTRL2 |= SC_TWIACK;   // ack on receipt of data
        else
            SC1_TWICTRL2 &= ~SC_TWIACK;  // don't ack if last one
        
        SC1_TWICTRL1 |= SC_TWIRECV;    // set to receive
        WAIT_RX_FIN();
        *(pBuffer+i) = SC1_DATA;       // receive data                   
    }
    SC1_TWICTRL1 |= SC_TWISTOP;       //added by zgl 1-7
    WAIT_CMD_FIN();
    return SUCCESS;
}/* end I2cRecvFrame() */

/* */
status_t I2cWrite(uint8_t dev, uint16_t reg, const void *data, size_t count)
{
    I2cFrameSend(dev,reg,(uint8_t *)data,count);
    
    return STATUS_OK;   
}

status_t I2cRead(uint8_t dev, uint16_t reg, void *data, size_t count)
{
    I2cFrameRecv(dev,reg,(uint8_t *)data,count);
    
    return STATUS_OK;   
}
