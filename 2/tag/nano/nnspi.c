/********************************Copyright (c)**********************************
**                			
**                          			
**-File Info--------------------------------------------------------------------
** File name:			nnspi.c
** Last Version:		V1.0
** Descriptions:		与nano测距无线,SPI驱动
**------------------------------------------------------------------------------
** Created by:	 	QI
** Created date:     	2013-08-12
** Version:	       	V1.0
** Descriptions:    	
*******************************************************************************/

#include "iar.h"
#include "hal.h"
//#include "phy-library.h"
//#include "hal/micro/cortexm3/micro-common.h"

#include "config.h"
#include "ntrxtypes.h"
#include "nnspi.h"
#include "hwclock.h"

#define CS_ON   		(GPIO_PAOUT &= (~PA3)) 
#define CS_OFF  		(GPIO_PAOUT |= PA3)

#define NANO_RST_OFF   (GPIO_PBOUT &= (~PB4)) 
#define NANO_RST_ON    (GPIO_PBOUT |= PB4)

/**
 * nanorest_init:
 *
 * nanorest_init() 配置PB4引脚为nano复位引脚
 *
 * Returns: none
 */
void nanorest_init(void)
{
	GPIO_PBCFGH_REG |= PB4_CFG;			//配置PB4为上拉输出模式PONRESET
	GPIO_PBCFGH_REG &= ((~PB4_CFG)|(GPIOCFG_OUT<<PB4_CFG_BIT));
}

/**
 * NanoReset:
 *
 * NanoReset() resets the nanoNET chip and adjusts the pin level.
 *
 * Returns: none
 */
void NanoReset(void)
{
	 NANO_RST_OFF;
	 hwdelay(500);  
	 NANO_RST_ON;
}

/**
 * transmission:
 *
 * transmission() SPI传输操作
 *
 *参数 : 要发送的值 
 *
 * Returns: 接收到的数据
 */
unsigned char transmission(unsigned value)
{ 
    uint32_t dely = 1000;
    SC2_DATA = value; 
    while (!(SC2_SPISTAT & SC_SPITXFREE)); 		        //等待发送完成
    while (!(SC2_SPISTAT & SC_SPIRXVAL)){		        //等待接收完成
	dely--;
	if(dely == 0)
	  break;
    }	
    return(SC2_DATA); 
}

/**
 * InitSPI:
 *
 * InitSPI() SPI初始化
 *
 *参数 : none
 *
 * Returns: none
 */
void InitSPI(void)
{
	GPIO_PACFGL_REG |= PA3_CFG;								//配置PA3为上拉输出模式SPISSN
	GPIO_PACFGL_REG &= ((~PA3_CFG)|(GPIOCFG_OUT<<PA3_CFG_BIT));
        
	GPIO_PACFGL_REG |= PA2_CFG;								//配置PA2为CLK输出
	GPIO_PACFGL_REG &= ((~PA2_CFG)|(GPIOCFG_OUT_CLK<<PA2_CFG_BIT));    
       
	GPIO_PACFGL_REG |= PA1_CFG;								//配置PA1为输入
	GPIO_PACFGL_REG &= ((~PA1_CFG)|(GPIOCFG_IN<<PA1_CFG_BIT));    
        
	GPIO_PACFGL_REG |= PA0_CFG;								//配置PA0为输出
	GPIO_PACFGL_REG &= ((~PA0_CFG)|(GPIOCFG_OUT_ALT<<PA0_CFG_BIT));

	SC2_MODE = SC2_MODE_SPI;								//选择SPI模式
	SC2_SPICFG |= SC_SPIMST;								//MASTER模式
	SC2_TWICTRL2 |= SC_TWIACK;		        				//

			                                				//Rate=12MHz/((LIN+1)x(2^EXP))
	SC2_RATEEXP = 1;										//EXP = 1
	SC2_RATELIN = 0;										//LIN = 0     6Mbps

															//时序配置
	SC2_SPICFG &= (~SC_SPIPOL_MASK);	        			//CPOL为0，CK低电平空闲
	SC2_SPICFG &= (~SC_SPIPHA_MASK);	        			//CPHA为0，数据在第一个时钟沿发送
	SC2_SPICFG |= SC_SPIORD;								//LSBFIRST 为1，LSB(bit0)先发送
	SC2_SPICFG |= SC_SPIMST;								//MSTR 为1，ASTER模式
        
        INT_SC2CFG = INT_SC2CFG_RESET;                  	//禁止所有串行端口2中断
	INT_CFGSET &= (~INT_SC2_MASK);		        			//在总中断中禁止SC2中断
}

void SetupSPI (void)
{
    MyByte8T value = 0x42;
    NTRXWriteSPI (0x00, &value, 1);
}


void NTRXReadSPI(MyByte8T address, MyByte8T *buffer, MyByte8T len)
{
  CS_ON;
  asm("NOP");
  //hwdelay(10);
  transmission(len & 0x7F);
  transmission(address); 
  while(len--)
  {
    *(buffer++) = transmission(0xff);
  }  
  //hwdelay(10); 
  asm("NOP"); 
  CS_OFF;
}

void NTRXWriteSPI(MyByte8T address, MyByte8T *buffer, MyByte8T len)
{
  CS_ON;
  asm("NOP");
  //hwdelay(10);
  transmission(0x80 | (len & 0x7F));
  transmission(address); 
  while(len--)
  {
    transmission(*buffer);
    buffer++;
    address++;
  }  
  //hwdelay(10);
  asm("NOP");  
  CS_OFF;
}

void NTRXReadSingleSPI(MyByte8T address, MyByte8T *buffer)
{
  NTRXReadSPI(address, buffer, 1);
}

void NTRXWriteSingleSPI(MyByte8T address, MyByte8T buffer)
{
  NTRXWriteSPI(address, &buffer, 1);
}


