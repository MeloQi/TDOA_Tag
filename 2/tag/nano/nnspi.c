/********************************Copyright (c)**********************************
**                			
**                          			
**-File Info--------------------------------------------------------------------
** File name:			nnspi.c
** Last Version:		V1.0
** Descriptions:		��nano�������,SPI����
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
 * nanorest_init() ����PB4����Ϊnano��λ����
 *
 * Returns: none
 */
void nanorest_init(void)
{
	GPIO_PBCFGH_REG |= PB4_CFG;			//����PB4Ϊ�������ģʽPONRESET
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
 * transmission() SPI�������
 *
 *���� : Ҫ���͵�ֵ 
 *
 * Returns: ���յ�������
 */
unsigned char transmission(unsigned value)
{ 
    uint32_t dely = 1000;
    SC2_DATA = value; 
    while (!(SC2_SPISTAT & SC_SPITXFREE)); 		        //�ȴ��������
    while (!(SC2_SPISTAT & SC_SPIRXVAL)){		        //�ȴ��������
	dely--;
	if(dely == 0)
	  break;
    }	
    return(SC2_DATA); 
}

/**
 * InitSPI:
 *
 * InitSPI() SPI��ʼ��
 *
 *���� : none
 *
 * Returns: none
 */
void InitSPI(void)
{
	GPIO_PACFGL_REG |= PA3_CFG;								//����PA3Ϊ�������ģʽSPISSN
	GPIO_PACFGL_REG &= ((~PA3_CFG)|(GPIOCFG_OUT<<PA3_CFG_BIT));
        
	GPIO_PACFGL_REG |= PA2_CFG;								//����PA2ΪCLK���
	GPIO_PACFGL_REG &= ((~PA2_CFG)|(GPIOCFG_OUT_CLK<<PA2_CFG_BIT));    
       
	GPIO_PACFGL_REG |= PA1_CFG;								//����PA1Ϊ����
	GPIO_PACFGL_REG &= ((~PA1_CFG)|(GPIOCFG_IN<<PA1_CFG_BIT));    
        
	GPIO_PACFGL_REG |= PA0_CFG;								//����PA0Ϊ���
	GPIO_PACFGL_REG &= ((~PA0_CFG)|(GPIOCFG_OUT_ALT<<PA0_CFG_BIT));

	SC2_MODE = SC2_MODE_SPI;								//ѡ��SPIģʽ
	SC2_SPICFG |= SC_SPIMST;								//MASTERģʽ
	SC2_TWICTRL2 |= SC_TWIACK;		        				//

			                                				//Rate=12MHz/((LIN+1)x(2^EXP))
	SC2_RATEEXP = 1;										//EXP = 1
	SC2_RATELIN = 0;										//LIN = 0     6Mbps

															//ʱ������
	SC2_SPICFG &= (~SC_SPIPOL_MASK);	        			//CPOLΪ0��CK�͵�ƽ����
	SC2_SPICFG &= (~SC_SPIPHA_MASK);	        			//CPHAΪ0�������ڵ�һ��ʱ���ط���
	SC2_SPICFG |= SC_SPIORD;								//LSBFIRST Ϊ1��LSB(bit0)�ȷ���
	SC2_SPICFG |= SC_SPIMST;								//MSTR Ϊ1��ASTERģʽ
        
        INT_SC2CFG = INT_SC2CFG_RESET;                  	//��ֹ���д��ж˿�2�ж�
	INT_CFGSET &= (~INT_SC2_MASK);		        			//�����ж��н�ֹSC2�ж�
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


