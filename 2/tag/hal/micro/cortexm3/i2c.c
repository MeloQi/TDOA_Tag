/********************************Copyright (c)**********************************
**                      
**                          
**-File Info--------------------------------------------------------------------
** File name:			i2c.c
** Last Version:		
** Descriptions:		
**------------------------------------------------------------------------------
** Created by:	             zhang
** Created date:               2012-10-19
** Version:	               V1.0
** Descriptions:    
**                  
*******************************************************************************/
#include "regs.h"
#include "iar.h"
#include "i2c.h"
//#include "hal/micro/cortexm3/micro-common.h"
//#include "micro-common.h"
#include "iar.h"
#include "hal.h"
#include "uart.h"
#include "stdio.h"
#include "phy-library.h"
#include "hal/micro/cortexm3/micro-common.h"
#include "hal/micro/system-timer.h"
#include "hal/micro/cortexm3/I2C.h"
#include "board.h"
//#include "rfid.h"
//#include "System.h"
//#include "App.h"
/********************************************
* Defines *
********************************************/
#define User_Memory    0x53     //用户存储区地址
#define Sys_Memory     0x57     //系统存储区地址

#define Page_Write     4        //逐页写操作
#define Byte_Write     1        //逐字节写操作

uint16_t iic_time;
/********************************************
* Function defines *
********************************************/
/* *///modified by zgl,using SC1--PB1&PB2
void I2cInit(uint32_t speed)
{
    SC1_MODE =  SC1_MODE_I2C;
    
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
    uint8_t i;

    SC1_TWICTRL1 |= SC_TWISTART;   // send start
    WAIT_CMD_FIN();
    SEND_BYTE((dev<<1)|0x00);     // send the address low byte
    WAIT_TX_FIN();
    SEND_BYTE((reg>>8)&0x00FF);  // send the reg address     
    WAIT_TX_FIN();
    SEND_BYTE(reg&0x00FF);    // send the reg address        
    WAIT_TX_FIN();
    // loop sending the data
    for (i=0; i<NoOfBytes; i++) 
    {   
        SEND_BYTE(*pBuffer ++);    
        WAIT_TX_FIN();
    }

    SC1_TWICTRL1 |= SC_TWISTOP;
    WAIT_CMD_FIN();
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
    uint8_t i;
    uint16_t addr = reg;
    SC1_TWICTRL1 |= SC_TWISTART;   // send start
    WAIT_CMD_FIN();
    SEND_BYTE((dev<<1)|0x00);     // 写操作
    WAIT_TX_FIN();
    SEND_BYTE((addr>>8)&0x00FF); // 地址高字节  先高后低 
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

status_t I2cRegWrite(uint8_t dev, uint16_t reg, uint8_t data)
{
    I2cFrameSend(dev,reg,&data,1);
    
    return STATUS_OK;   
}

status_t I2cRegRead(uint8_t dev, uint16_t reg, uint8_t *data)
{
    I2cFrameRecv(dev,reg,data,1);
    
    return STATUS_OK;   
}

/*******************************************************************************
* Function Name  : I2cSendValue
* Description    : 在连续区域设定固定值
* Input          : dev is the slave address
*                  reg is the register address
			value 要设置的固定值
*                  NoOfBytes is the numenr of bytes to read starting from reg_addr
* Output         : 
* Return         : status
*******************************************************************************/
uint8_t I2cSendValue (uint8_t dev, uint16_t reg, uint8_t value, uint8_t NoOfBytes)
{
    uint8_t i;

    SC1_TWICTRL1 |= SC_TWISTART;   // send start
    WAIT_CMD_FIN();
    SEND_BYTE((dev<<1)|0x00);     // send the address low byte
    WAIT_TX_FIN();
    SEND_BYTE((reg>>8)&0x00FF);  // send the reg address     
    WAIT_TX_FIN();
    SEND_BYTE(reg&0x00FF);    // send the reg address        
    WAIT_TX_FIN();
    // loop sending the data
    for (i=0; i<NoOfBytes; i++) 
    {   
        SEND_BYTE(value);    
        WAIT_TX_FIN();
    }

    SC1_TWICTRL1 |= SC_TWISTOP;
    WAIT_CMD_FIN();
    return SUCCESS;
}/* end I2cFrameSend() */















