/********************************Copyright (c)**********************************
**                        
**                          
**-File Info--------------------------------------------------------------------
** File name:			rfid.h
** Last Version:		
** Descriptions:		
**------------------------------------------------------------------------------
** Created by:	             zhang
** Created date:               2012-10-19
** Version:	               V1.0
** Descriptions:    
**    
**------------------------------------------------------------------------------
** Modify by:	               qi
** Created date:            2013-10-29
** Version:	              
** Descriptions:    
**  
*******************************************************************************/
#ifndef __RFID_H__
#define __RFID_H__

//define RF Channel 
#define TX_Channel             11
#define RX_Channel             21
#define ZIGBEE_POWER           0

#define BLOCK_NUM              4

#define CARD_STATUS_ADDR              0
#define CARD_NUM_ADDR                 4
#define CARD_BAT_ADDR                 8
#define CARD_SLEEP_ADDR              12
#define CARD_DETECT_ADDR             16
#define CARD_SEND_POWER_ADDR         20
#define CARD_CARRIER_ADDR            24
#define CARD_SEND_CHANNEL_ADDR       28
#define CARD_RECV_CHANNEL_ADDR       32
#define CARD_SEND_FAIL_ADDR          36
#define CARD_BIDIRECT_ADDR           40
#define CARD_SOFT_RESET_ADDR         44
#define CARD_WDOG_RESET_ADDR         48
#define CARD_POWER_RESET_ADDR        52
#define CARD_DELIVERY_TIME_ADDR      56
#define CARD_ABNORMAL_WAKE_ADDR      60
#define	CARD_NANO_CARRIER_ADDR		 64

#define CARD_IS_FIST_BOOT_ADDR       68
/*
typedef enum
{
	CARD_STATUS_ADDR    		= 0,
	CARD_NUM_ADDR   			= (CARD_STATUS_ADDR+4),
	CARD_BAT_ADDR   			= (CARD_STATUS_ADDR+8),
	CARD_SLEEP_ADDR   			= (CARD_STATUS_ADDR+12),
	CARD_DETECT_ADDR   			= (CARD_STATUS_ADDR+16),
	CARD_SEND_POWER_ADDR   		= (CARD_STATUS_ADDR+20),
	CARD_CARRIER_ADDR   		= (CARD_STATUS_ADDR+24),
	CARD_SEND_CHANNEL_ADDR   	= (CARD_STATUS_ADDR+28),
	CARD_RECV_CHANNEL_ADDR   	= (CARD_STATUS_ADDR+32),
	CARD_SEND_FAIL_ADDR   		= (CARD_STATUS_ADDR+36),
	CARD_BIDIRECT_ADDR   		= (CARD_STATUS_ADDR+40),
	CARD_SOFT_RESET_ADDR		= (CARD_STATUS_ADDR+44),
	CARD_WDOG_RESET_ADDR		= (CARD_STATUS_ADDR+48),	
	CARD_POWER_RESET_ADDR		= (CARD_STATUS_ADDR+52),
	CARD_DELIVERY_TIME_ADDR		= (CARD_STATUS_ADDR+56),
	CARD_ABNORMAL_WAKE_ADDR		= (CARD_STATUS_ADDR+60),
	CARD_IS_FIST_BOOT_ADDR		= (CARD_STATUS_ADDR+64)
	
} EEPROM_Addr;
*/

#define EEPROMOn()     (GPIO_PBOUT |= PB5)  //EEPROM µÁ‘¥ PB5  
#define EEPROMOff()    (GPIO_PBOUT &=~PB5)
#define EEPROM_Idle	 	((GPIO_PAIN&PA4)==PA4)

extern uint8_t WakeUP_Flag,Sleep_Flag;
extern uint8_t SelfDetect_Flag;
extern uint8_t TransmitStream_Flag;

void Read_CardID_Config();
void Read_SelfDetect_Config();
uint8_t RF_Power_Config();
void TransmitStream_Config();
void Read_WakeSleep_Config();
void Write_RFID_Data(uint16_t reg, const void *data, uint8_t count);
void Read_RFID_Data(uint16_t reg, void *data, uint8_t count);
void Write_Control_Reg(uint16_t reg, const void *data, uint8_t count);
void Read_Control_Reg(uint16_t reg, void *data, uint8_t count);

uint8_t RF_Send_Channel();
uint8_t RF_Recieve_Channel();
void WriteBATValue(int16u value);
void WriteSendFailTime(uint16_t time);
void WriteRecvSuccessTime(uint16_t time);
uint16_t ReadRecvSuccessTime(void);
uint16_t ReadSendFailTime(void);
uint16_t ReadAbnormalWakeTime(void);
void WriteAbnormalWakeTime(uint16_t time);
 uint8_t EEPROM_SelfDetect();
 void EEPROM_Process(void);
 uint8_t EEPROM_Fist_Boot();

#endif