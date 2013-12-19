/********************************Copyright (c)**********************************
**                        
**                          
**-File Info--------------------------------------------------------------------
** File name:			i2c
** Last Version:		
** Descriptions:		
**------------------------------------------------------------------------------
** Created by:	              zhang
** Created date:               2012-10-19
** Version:	               V1.0
** Descriptions:    
**                  
*******************************************************************************/
 
#ifndef  __IIC_H  
#define  __IIC_H

#include "ysizet.h"
#include "iar.h"
#include "pub_def.h"


/********************************************
* Defines *
********************************************/




/********************************************
* Typedefs *
********************************************/
typedef enum {                      //!< twi_speed_t - bus speed constants

    I2C_SPEED_STANDARD  = 100000,   //!< standard, 100k bit-per-second
    I2C_SPEED_FAST      = 400000    //!< fast, 400k bit-per-second

} i2c_speed_t;


/********************************************
* Globals *
********************************************/ 

#define User_Memory    0x53     //用户存储区地址
#define Sys_Memory     0x57     //系统存储区地址

#define Page_Write     4        //逐字节写操作
#define Byte_Write     1        //逐页写操作

#define REPETIR          0x80

//extern uint8_t iic_time;
/********************************************
* Function defines *
********************************************/ 
#define SEND_BYTE(data) do{ SC1_DATA=(data); SC1_TWICTRL1 |= SC_TWISEND; }while(0)

#define WAIT_CMD_FIN()  do{iic_time++;}while(((SC1_TWISTAT&SC_TWICMDFIN)!=SC_TWICMDFIN)&&(iic_time<=0xFA00));iic_time=0
#define WAIT_TX_FIN()   do{iic_time++;}while(((SC1_TWISTAT&SC_TWITXFIN)!=SC_TWITXFIN)&&(iic_time<=0xFA00));iic_time=0
#define WAIT_RX_FIN()   do{iic_time++;}while(((SC1_TWISTAT&SC_TWIRXFIN)!=SC_TWIRXFIN)&&(iic_time<=0xFA00));iic_time=0

void I2cInit(uint32_t);

status_t I2cWrite(uint8_t dev, uint16_t reg, const void * data, size_t count);

status_t I2cRead(uint8_t dev, uint16_t reg, void * data, size_t count);

status_t I2cRegWrite(uint8_t dev, uint16_t reg, uint8_t data);

status_t I2cRegRead(uint8_t dev, uint16_t reg, uint8_t *data);
uint8_t I2cSendValue (uint8_t dev, uint16_t reg, uint8_t value, uint8_t NoOfBytes);


#endif

