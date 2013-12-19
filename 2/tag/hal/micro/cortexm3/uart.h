/*
 * Copyrite (C) 2012, BeeLinker
 *
 * �ļ����ƣ�uart.h
 *
 * �ļ�˵����MC13213 ��������ͷ�ļ�
 * 
 * �汾��Ϣ��
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
    volatile uint16_t       rxCnt;          //���ռ�����
    
    /* Tx */
    uint8_t                 *txFifo;
    volatile uint16_t       txFifoRead;
    volatile uint16_t       txFifoWrite;
    volatile uint16_t       txCnt;          //���ͼ�����
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

