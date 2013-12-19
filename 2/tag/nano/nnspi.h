#ifndef  _NNSPI_H
#define  _NNSPI_H

#include "config.h"
#include "ntrxtypes.h"

#define NANO_OFF P2OUT &= 0xfe;
typedef enum 
{
    READ_CMD = 0x00,                          /* SPI read command */
    WRITE_CMD = 0x80                          /* SPI write command */
} CMDT;

#define NANONETRESETDELAY   1000             /* reset of TRX in us */

void nanorest_init(void);
void NanoReset	(void);
void InitSPI	(void);
void SetupSPI 	(void);
void NTRXReadSPI(MyByte8T address, MyByte8T *buffer, MyByte8T len);
void NTRXWriteSPI(MyByte8T address, MyByte8T *buffer, MyByte8T len);
void NTRXReadSingleSPI(MyByte8T address, MyByte8T *buffer);
void NTRXWriteSingleSPI(MyByte8T address, MyByte8T buffer);
unsigned char transmission(unsigned value);

#endif   /* _NNSPI_H */
