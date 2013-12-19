/********************************Copyright (c)**********************************
**                        
**                      
**-File Info--------------------------------------------------------------------
** File name:			system.h
** Last Version:		
** Descriptions:		
**------------------------------------------------------------------------------
** Created by:	               zhang
** Created date:               2013-1-9
** Version:	               V1.0
** Descriptions:               
*******************************************************************************/
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#define SOFT_RESET         0x10
#define WATCHDOG_RESET     0x11
#define POWER_RESET        0x12

void WriteBootType(void);
uint16_t AcquireVolt(void);
void HandleEEPROM(void);
void ValidWarning(void);
void WriteFactDate(uint16_t date_byte);
uint16_t ReadFactDate(void);
void WriteResetTime(uint8_t reset_type,uint16_t reset_num);
#endif