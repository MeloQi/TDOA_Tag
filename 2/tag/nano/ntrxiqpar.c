#include "config.h"
#include "ntrxtypes.h"
#include "ntrxutil.h"
#include "ntrxiqpar.h"
#include "nnspi.h"

typedef struct NTRXagcValSt
{
    MyByte8T thresh1;
    MyByte8T thresh2;
    MyByte8T deadtime;
    MyByte8T nreg;
    MyInt16T inttime;
} NTRXagcValSt;

static const MyByte8T NA5TR1_Rx_80MHz_1000ns[]  =
{
    16,
    0x02, 0x63, 0x39, 0xC7, 0x0E, 0x0F, 0xC0, 0x1F, 0xFF, 0xE0, 0x0F, 0xC3, 0xC3, 0x8E, 0x73, 0x39,
    0x00, 0xCE, 0x63, 0x1C, 0x38, 0x3E, 0x03, 0xFF, 0xFF, 0xFF, 0x01, 0xF0, 0xF0, 0xE7, 0x19, 0x8C,
};

static const MyByte8T NA5TR1_Tx_80MHz_1000ns[]  =
{
    61,
    0x20, 0xE0, 0x1D, 0xDE, 0x27, 0xA3, 0x12, 0x17, 0xF1, 0xB1, 0x10, 0x04, 0xA4, 0xFF, 0x6D, 0x09,
    0x03, 0xE2, 0xBD, 0x75, 0xD4, 0xC0, 0x0D, 0x2C, 0x7F, 0x35, 0x19, 0x03, 0xC3, 0x57, 0xB1, 0xBF,
    0x39, 0xA5, 0xCF, 0x02, 0xC2, 0x0F, 0xE2, 0x73, 0xFD, 0xFE, 0x77, 0xEA, 0x5B, 0x4F, 0x85, 0x01,
    0x80, 0x84, 0x49, 0x10, 0xD7, 0x9E, 0xE4, 0x2A, 0xEE, 0xB1, 0x33, 0x75, 0x75,
    0x08, 0xF7, 0xF7, 0x48, 0x1A, 0x56, 0xA4, 0x09, 0xCD, 0xE8, 0x81, 0xA4, 0xED, 0x4E, 0x36, 0xA0,
    0xE4, 0xFC, 0x9F, 0x89, 0x21, 0xC0, 0x97, 0xDE, 0x5F, 0x9A, 0x13, 0x20, 0xD2, 0xE9, 0xAE, 0x5F,
    0xDC, 0x16, 0x51, 0x60, 0xB1, 0xA6, 0xFA, 0x8E, 0x9F, 0x9E, 0xAC, 0x28, 0x05, 0x52, 0xE0, 0x90,
    0x40, 0xE1, 0x63, 0xB5, 0xE6, 0xF8, 0xF9, 0xDB, 0xCB, 0xAC, 0x8D, 0x7D, 0x6D,
    0x7E, 0x79, 0x85, 0x96, 0x6E, 0x51, 0x95, 0xCF, 0x7A, 0x14, 0x5C, 0xEA, 0xDB, 0x42, 0x04, 0x6C,
    0xEE, 0xE3, 0x62, 0x04, 0x30, 0xB1, 0xFF, 0xCF, 0x56, 0x04, 0x1C, 0x7D, 0xE2, 0xFF, 0xC7, 0x62,
    0x14, 0x04, 0x30, 0x81, 0xD2, 0xFB, 0xF7, 0xC7, 0x86, 0x45, 0x14, 0x00, 0x08, 0x24, 0x4C, 0x7D,
    0xA6, 0xCA, 0xE7, 0xF7, 0xFF, 0xFF, 0xFB, 0xF7, 0xEF, 0xE7, 0xE3, 0xDF, 0xDB,
};

const NTRXagcValSt NA5TR1_AGC_80MHz_1000ns[]  =
{
/* symbolDur NA_1ns */
    {5, 1, 12, 0, 122}, /* symbolRate NA_500k */
    {3, 1, 16, 0, 130}, /* symbolRate NA_1M */
    {0, 0,  0, 0,   0}, /* dummy */
    {0, 0,  0, 0,   0}, /* dummy */
    {5, 1, 12, 0, 122}, /* symbolRate NA_31k25 */
    {5, 1, 12, 0, 122}, /* symbolRate NA_62k5 */
    {5, 1, 12, 0, 122}, /* symbolRate NA_125k */
    {5, 1, 12, 0, 122}, /* symbolRate NA_250k */
};

static MyByte8T NA5TR1_THRES_80MHz_1000ns[]  =
{
    0x00, 0x36, 0x36, 0x36, 0x36, /* sd:1/sr:4 */
};


void memcpy_cpy(unsigned char *destdata,unsigned char *scrdata,unsigned char length)
{
  while(length--)
  {
    *destdata = *scrdata;
    scrdata++;
    destdata++; 
  }
}

void NTRXSetRxIQMatrix (MyByte8T bandwidth, MyByte8T symbolDur)
{
    MyByte8T tmpBuffer[64];
    MyByte8T *ptr;
    MyWord16T len;

    ptr = (MyByte8T *)NA5TR1_Rx_80MHz_1000ns; 

    len =  (ptr[0]);
    /* Device select 3 */
    memcpy_cpy (tmpBuffer, &(ptr[1]), len);
    NTRXSetIndexReg (0x30);
    NTRXWriteSPI ((NA_RamD3lPatI_O & 0xff),     tmpBuffer, len);

    memcpy_cpy (tmpBuffer, &(ptr[1+ len]), len);
    NTRXSetIndexReg (0x31);
    NTRXWriteSPI ((NA_RamD3lPatQ_O & 0xff),     tmpBuffer, len);

    NTRXSetIndexReg (0x00);
}


void NTRXSetTxIQMatrix (MyByte8T bandwidth, MyByte8T symbolDur)
{
    MyByte8T tmpBuffer[245];
    MyWord16T len;
    MyWord16T offset;
    MyByte8T *ptr;

    ptr = (MyByte8T *)NA5TR1_Tx_80MHz_1000ns;

    len =  (ptr[0]);

    NTRXSetIndexReg (0x20);
    // TrxDI "w CsqMemAddrInit 1";

    NTRXSetRegister (NA_CsqMemAddrInit, TRUE);
    offset = 0;
    if (len > 128)
    {
        len -= 128;
        memcpy_cpy (tmpBuffer, &(ptr[1]), 128);
        NTRXWriteSPI (0x80, tmpBuffer, 128);
        offset = 128;
    }
    memcpy_cpy (tmpBuffer, &(ptr[1 + offset]), len);
    NTRXWriteSPI (0x80, tmpBuffer, len);

    NTRXSetRegister (NA_CsqMemAddrInit, TRUE);
    NTRXSetIndexReg (0x21);
    offset = 0;
    len =  (ptr[0]);
    if (len > 128)
    {
        len -= 128;
        memcpy_cpy (tmpBuffer, &(ptr[1 +  (ptr[0])]), 128);
        NTRXWriteSPI (0x80, tmpBuffer, 128);
        offset = 128;
    }
    memcpy_cpy (tmpBuffer, &(ptr[1 +  (ptr[0]) + offset]), len);
    NTRXWriteSPI (0x80, tmpBuffer, len);

    NTRXSetRegister (NA_CsqMemAddrInit, TRUE);
    NTRXSetIndexReg (0x22);

    len =  (ptr[0]);
    offset = 0;
    if (len > 128)
    {
         len -= 128;
        memcpy_cpy (tmpBuffer, &(ptr[1 + (2 *  (ptr[0]))]), 128);
        NTRXWriteSPI (0x80, tmpBuffer, 128);
        offset = 128;
    }
    memcpy_cpy (tmpBuffer, &(ptr[1 + (2 *  (ptr[0])) + offset]), len);
    NTRXWriteSPI (0x80, tmpBuffer, len);

    NTRXSetIndexReg (0x00);
}

void NTRXSetAgcValues (MyByte8T bandwidth, MyByte8T symbolDur, MyByte8T symbolRate)
{
    NTRXagcValSt *agcValS = NULL;
    MyByte8T value[2];
    MyInt16T inttime;

    agcValS = (NTRXagcValSt *)&(NA5TR1_AGC_80MHz_1000ns[symbolRate]); 

    NTRXSetRegister (NA_FdmaEnable, FALSE);
    NTRXSetRegister (NA_PulseDetDelay, 4);

    NTRXSetRegister (NA_LnaFreqAdjust, FALSE);
    NTRXSetRegister (NA_GateSizeFramesync, NA_GateSize9Slots_VC_C);

     /*select 0: MAP (maximum a posteriori) mode (peak detection)
         or     1: FIX mode (threshold detection)
    */
    NTRXSetRegister (NA_D3lFixnMap, settingVal.fixnmap); 

    NTRXSetRegister (NA_SymbolDur, symbolDur);
    NTRXSetRegister (NA_SymbolRate, symbolRate);

	NTRXSetRegister (NA_AgcThresHold1,  (agcValS->thresh1));
    NTRXSetRegister (NA_AgcThresHold2,  (agcValS->thresh2));
    NTRXSetRegister (NA_AgcDeadTime,  (agcValS->deadtime));
    NTRXSetRegister (NA_AgcNregLength,  (agcValS->nreg));

    inttime =  (agcValS->inttime); 
    value[0] = 0xFF & (inttime);
    value[1] = 0xFF & ((inttime)>>8);

    NTRXSetNRegister (NA_AgcIntTime, value);
}

void NTRXSetCorrThreshold (MyByte8T bandwidth, MyByte8T symbolDur)
{
    MyByte8T threshold[5];
    MyByte8T *ptr;

    NTRXSetIndexReg (0x32);

    ptr = NA5TR1_THRES_80MHz_1000ns; 

    memcpy_cpy (threshold, ptr, 5);
    NTRXWriteSPI (0x80, threshold, 5);
    NTRXSetIndexReg (0x00);
}
