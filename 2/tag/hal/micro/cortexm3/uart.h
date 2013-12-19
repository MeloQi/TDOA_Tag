/*
 * Copyrite (C) 2012, BeeLinker
 *
 * 文件名称：uart.h
 *
 * 文件说明：MC13213 串口驱动头文件
 * 
 * 版本信息：
 * v0.0.1       wzy         2012/02/29
 *
 */
 
#ifndef  __UART_H  
#define  __UART_H

#include "ysizet.h"



/********************************************
* Defines *
********************************************/





/********************************************
* Typedefs *
********************************************/
typedef struct
{
    /* Rx */
    uint8_t                 *rxFifo;
    volatile uint16_t       rxFifoRead;
    volatile uint16_t       rxFifoWrite;
    volatile uint16_t       rxCnt;          //接收计数器
    
    /* Tx */
    uint8_t                 *txFifo;
    volatile uint16_t       txFifoRead;
    volatile uint16_t       txFifoWrite;
    volatile uint16_t       txCnt;          //发送计数器
    volatile bool_t         txIsWorking;
}uartInfo_t;


/********************************************
* Globals *
********************************************/ 




/********************************************
* Function defines *
********************************************/ 
/* uart Initialization*/
void uartInit(uint32_t baudrate, uint8_t databits, char parity, uint8_t stopbits);

/* Transmit */
size_t uartSend(const uint8_t *buf, size_t len);

/* Receive */
size_t uartRecv(uint8_t *buf, size_t bufSize);

#endif

