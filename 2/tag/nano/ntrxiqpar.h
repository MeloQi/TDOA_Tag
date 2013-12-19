#ifndef NTRXIQPAR_H
#define NTRXIQPAR_H

#include    "config.h"
#include    "ntrxtypes.h"

void NTRXSetAgcValues (MyByte8T bandwidth, MyByte8T symbolDur, MyByte8T symbolRate);
void NTRXSetTxIQMatrix (MyByte8T bandwidth, MyByte8T symbolDur);
void NTRXSetRxIQMatrix (MyByte8T bandwidth, MyByte8T symbolDur);
void NTRXSetCorrThreshold (MyByte8T bandwidth, MyByte8T symbolDur);

#endif