#ifndef	_HWCLOCK_H
#define	_HWCLOCK_H

#include "config.h"
#include "ntrxtypes.h"



#define NKEYS 4

#define L_POWER 1       //低功耗模式
#define H_POWER 0       //高速模式

extern  MyByte8T power;
extern  MyDword32T sleep_cnt;

extern  void            ex_clkinit(void);
extern  void            sleep(MyByte8T s);
extern	void		hwclock_init(void);
extern  void 		hwclock_stop (void);
extern  void 		StartTimer2 (void);
extern	MyDword32T	hwclock(void);
extern	void		hwdelay(MyDword32T t);
void	hwclockRestart(MyDword32T start);
void 	SetWdtCounter (MyDword32T value);

#endif	/* _HWCLOCK_H */