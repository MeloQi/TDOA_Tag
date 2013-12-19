#ifndef _NTRXDIL_H
#define _NTRXDIL_H

#include "config.h"
#include "ntrxtypes.h"
#include "OffstMapInit.h"

#define SAPALLOCERR       0
#define SAPALLOCID        1
#define TXWAIT          100                  /* ms */
#define TXPOWER_10dBm    49

#define NTRX_VER_INIT       NA_Version_I
#define NTRX_REV_INIT       NA_Revision_I
#define NTRX_VERSION        NA_Version_O
#define NTRX_TX_PUSH_PULL   NA_SpiTxDriver_O
typedef enum
{
    Receiver_off,
    Receiver_on
} RECEIVER_STATE;

typedef struct TXstat
{
    MyDword32T   allCount;
    MyDword32T   successCount;
    MyDword32T   ARQcount;
    MyDword32T   urunCount;
} PPSTXstatT;

typedef struct RXstat
{
    MyDword32T   allCount;
    MyDword32T   successCount;
    MyDword32T   orunCount;
    MyDword32T   CRC1failsCount;
    MyDword32T   CRC2failsCount;
    MyDword32T   corrBitErrCount;
    MyByte8T     lastRSSI;
}PPSRXstatT;

extern struct TXstat sTXstat;
extern struct RXstat sRXstat;



typedef enum
{
    frameBroadcast,
    frameTimeB
} RXFTYPE_T;

typedef enum
{
    crc2_1,
    crc2_2,
    crc2_3
} PPS_CRC2_T;

typedef enum
{
    cs_off,
    cs_symbols,
    cs_rssi,
    cs_symb_rssi
} PPS_MACMETH_T;

typedef enum
{
    arqMode_off,
    arqMode_crc1,
    arq_Mode_crc2
} RXARQMODE_T;

typedef enum
{
    data_rate_500kps,
    data_rate_1mps
} DATA_RATE_T;


typedef enum
{
	PacketTypeData    = 0,
	PacketTypeTimeB   = 2,
	PacketTypeBrdcast = 3,
	PacketTypeRanging = 7
} NtrxPacketType ;


MyBoolT NtrxInit(void);
MyBoolT NTRXStart (void);
MyBoolT NTRXUpdate (void);
MyByte8T NTRXGetTxARQCnt (void);
void nanoInterrupt (void);
void NTRXSetIndexReg (MyByte8T page);
void NTRXSetCalInterval (MyDword32T ci);
double NTRXRange(MyAddrT dest);

void NTRXSendMessage (NtrxPacketType type,MyByte8T *addr, MyPtrT payload, MyWord16T len);
void NTRXSendMessageExt (MyAddressT addr, MyPtrT payload, MyWord16T len, MyByte8T opt);
#endif   /* _NTRXDIL_H */