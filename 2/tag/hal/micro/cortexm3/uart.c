/*
 * Copyrite (C) 2012, BeeLinker
 *
 * 文件名称：uart.c
 *
 * 文件说明：STM32W108 串口驱动
 * 
 * 版本信息：
 * v0.0.1       wzy         2012/02/29
 */

#include "regs.h"
#include "iar.h"
#include "uart.h"

/********************************************
* Defines *
********************************************/
#define UART_RXFIFO_SIZE        255
#define UART_TXFIFO_SIZE        255


/********************************************
* Typedefs *
********************************************/



/********************************************
* Globals * 
********************************************/
uint8_t uartRxFifo[UART_RXFIFO_SIZE];
uint8_t uartTxFifo[UART_TXFIFO_SIZE];  

uartInfo_t  uart1;

/********************************************
* Function defines *
********************************************/

/* uart Initialization*/
void uartInit(uint32_t baudrate, uint8_t databits, char parity, uint8_t stopbits)
{
    uint32_t    tmp;
    uint32_t    tmpcfg;
    
    /* rx */
    uart1.rxFifo = uartRxFifo;
    uart1.rxFifoRead = 0;
    uart1.rxFifoWrite = 0;
    uart1.rxCnt = 0;
    
    /* tx */
    uart1.txFifo = uartTxFifo;
    uart1.txFifoRead = 0;
    uart1.txFifoWrite = 0;
    uart1.txCnt = 0;
    uart1.txIsWorking = FALSE;
    
    tmp = (24000000UL + (baudrate>>1)) / baudrate;
    SC1_UARTFRAC_REG = tmp & 1;
    SC1_UARTPER_REG = tmp >> 1;
    
    if (databits == 7)
    {
        tmpcfg = 0;
    }
    else
    {
        tmpcfg = SC_UART8BIT;
    }
    
    switch(parity)
    {
      case 'O':
      case 'o':
        tmpcfg |= SC_UARTPAR | SC_UARTODD;
        break;
        
      case 'E':
      case 'e':
        tmpcfg |= SC_UARTPAR;
        break;
        
      case 'N':
      case 'n':
      default:
        break;
    }

    if ((stopbits & 0x0F) >= 2) 
    {
        tmpcfg |= SC_UART2STP;
    }
    
    SC1_UARTCFG_REG = tmpcfg;
    SC1_MODE_REG = SC1_MODE_UART;
    
    GPIO_PBCFGL_REG &= ~(0x0F<<(1*4));
    GPIO_PBCFGL_REG |= (GPIOCFG_OUT_ALT<<(1*4));
    
    GPIO_PBCFGL_REG &= ~(0x0F<<(2*4));
    GPIO_PBCFGL_REG |= (GPIOCFG_IN<<(2*4));

    // Make the RX Valid interrupt level sensitive (instead of edge)
    SC1_INTMODE = (SC_RXVALLEVEL | SC_TXIDLELEVEL | SC_TXFREELEVEL);
    // Enable just RX interrupts; TX interrupts are controlled separately
    INT_SC1CFG |= ( INT_SCRXVAL   |
                    INT_SCRXOVF   |
                    INT_SC1FRMERR |
                    INT_SC1PARERR );
    
    INT_SC1FLAG = 0xFFFF; // Clear any stale interrupts
    INT_CFGSET = INT_SC1;

}

/* Transmit 
size_t uartSend(const uint8_t *buf, size_t len)
{
    size_t cnt = 0;
    
    while(len --)
    {
        //spin until data register has room for more data
        while ((SC1_UARTSTAT&SC_UARTTXFREE)!=SC_UARTTXFREE);
        SC1_DATA = *buf ++;
       
        cnt ++;
    }
    
    return cnt;
}
*/

static void uartTxStart(void)
{
    INT_SC1FLAG |= INT_SCTXFREE;
    
    ATOMIC
    (
        SC1_DATA = uart1.txFifo[uart1.txFifoRead ++];
        if (uart1.txFifoRead == UART_TXFIFO_SIZE)
        {
            uart1.txFifoRead = 0;
        }
        uart1.txCnt --;
    )
  
    INT_SC1CFG |= INT_SCTXFREE;         //开启串口发送中断  
    INT_SC1CFG &= ~INT_SCTXIDLE;
}

static void uartTxStop(void)
{
    INT_SC1CFG &= ~(INT_SCTXFREE | INT_SCTXIDLE);     
}

/* Transmit */
size_t uartSend(const uint8_t *buf, size_t len)
{
    bool_t  ok;
    size_t  cnt = 0;
    
    while(len --)
    {
        ATOMIC
        (
            ok = (uart1.txCnt == UART_TXFIFO_SIZE);
        )
          
        if (ok) break;
        
        uart1.txFifo[uart1.txFifoWrite ++] = *buf ++;
        if (uart1.txFifoWrite == UART_TXFIFO_SIZE)
        {
            uart1.txFifoWrite = 0;
        }
        
        ATOMIC
        (
            uart1.txCnt ++;  
        )
        cnt ++;
    }
    
    ATOMIC
    (
        if ( (uart1.txCnt != 0) && (!uart1.txIsWorking) )
        {
            uart1.txIsWorking = TRUE;
            uartTxStart();            
        }
    )
    
    return cnt;
}   
          
/* Receive */
size_t uartRecv(uint8_t *buf, size_t bufSize)
{
    size_t  len;
    size_t  cnt = 0;
    
    ATOMIC
    (
        len = (uart1.rxCnt > bufSize)?bufSize:uart1.rxCnt;
    )
    
    while(len --)
    {
        *buf ++ = uart1.rxFifo[uart1.rxFifoRead ++];
        
        if (uart1.rxFifoRead == UART_RXFIFO_SIZE)
            uart1.rxFifoRead = 0;
        
        ATOMIC
        (
            uart1.rxCnt --;
        )
        
        cnt ++;
    }
    
    return cnt;
}

/* Interrupts */
/* Rx Events */
static void halInternalUart1RxIsr(void)
{
    while ( SC1_UARTSTAT & SC_UARTRXVAL ) 
    {
        uint8_t     u8tmp;
        uint8_t     errors;
        
        errors = SC1_UARTSTAT & (SC_UARTFRMERR |
                                 SC_UARTRXOVF  |
                                 SC_UARTPARERR );
          
        u8tmp = (uint8_t)SC1_DATA;
        
        if ( (errors == 0) && (uart1.rxCnt < UART_RXFIFO_SIZE) )
        {
            uint16_t tmpWrite = uart1.rxFifoWrite;
            
            uart1.rxFifo[tmpWrite ++] = u8tmp;
            
            if (tmpWrite == UART_RXFIFO_SIZE)
                tmpWrite = 0;
            
            uart1.rxFifoWrite = tmpWrite;
            
            uart1.rxCnt ++;
        }
    }
}
        
/* Tx Events */
static void halInternalUart1TxIsr(void)
{
    while(SC1_UARTSTAT & SC_UARTTXFREE)
    {
        if (uart1.txCnt != 0)
        {
            SC1_DATA = uart1.txFifo[uart1.txFifoRead ++];
            if (uart1.txFifoRead == UART_TXFIFO_SIZE)
            {
                uart1.txFifoRead = 0;
            }
        
            uart1.txCnt --;
        }
        else 
        {
            while(!(SC1_UARTSTAT & SC_UARTTXIDLE));
            
            uartTxStop();
            uart1.txIsWorking = FALSE;
            break;
        }
    }
}

void halSc1Isr(void)
{
    uint32_t interrupt;

    //this read and mask is performed in two steps otherwise the compiler
    //will complain about undefined order of volatile access
    interrupt = INT_SC1FLAG;
    interrupt &= INT_SC1CFG;
    
    while(interrupt != 0)
    {    
        // RX events
        if ( interrupt & (INT_SCRXVAL   | // RX has data
                          INT_SCRXOVF   | // RX Overrun error
                          INT_SC1FRMERR | // RX Frame error
                          INT_SC1PARERR ) // RX Parity error
           )
        {
            halInternalUart1RxIsr();
        }
         
        // TX events
        if ( interrupt & INT_SCTXFREE ) // TX has room
        {
            halInternalUart1TxIsr();
        }
        
        INT_SC1FLAG = interrupt;    //clear flag

        interrupt = INT_SC1FLAG;
        interrupt &= INT_SC1CFG;
    }
}


/* STDIN/OUT */
int putchar(int c)
{
    while ((SC1_UARTSTAT&SC_UARTTXFREE)!=SC_UARTTXFREE);
    SC1_DATA = (char)c;
    
    return c;
}
        
    















