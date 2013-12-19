#ifndef	_NTRXTYPES_H
#define	_NTRXTYPES_H

#define	BITS2BYTES(n)	(n/8)		/* calc. bits into bytes */

#ifndef	NULL
#define	NULL	(0)
#endif	/* NULL */

#ifndef	NIL
#define	NIL	(0)
#endif	/* NIL */

/**
 * MyChar8T:
 *
 * Signed character at least 8 bits in length.
 */
typedef	signed	char	MyChar8T;

/**
 * MyByte8T:
 *
 * Unsigned character at least 8 bits in length.
 */
typedef	unsigned	char	MyByte8T;

/**
 * MyInt16T:
 *
 * Signed integer at least 16 bits in length.
 */
typedef	signed	short	MyInt16T;

/**
 * MyWord16T:
 *
 * Unsigned integer at least 16 bits in length.
 */
typedef	unsigned	short	MyWord16T;

/**
 * MyLong32T:
 *
 * Signed long at least 32 bits in length.
 */
typedef	signed	long	MyLong32T;

/**
 * MyDword32T:
 *
 * Unsigned long at least 32 bits in length.
 */
typedef	unsigned	long	MyDword32T;

/**
 * MyDouble32T:
 *
 * double at least 32 bits in length.
 */
typedef	double	MyDouble32T;

#ifdef	TRUE
#undef	TRUE
#endif	/* TRUE */

#ifdef	FALSE
#undef	FALSE
#endif	/* FALSE */

/**
 * MyBoolT:
 *
 * Bool type.
 */
typedef	enum	{
	FALSE = 0,
	TRUE = 1
}	MyBoolT;

/**
 * MyPtrT:
 *
 * Void pointer type.
 */
typedef	void	*MyPtrT;

/**
 * MyFnPtrT:
 *
 * Void function pointer type.
 */
typedef	void	(*MyFnPtrT)(void);
/**
 * MyTimeT:
 *
 * Time type.
 * Stores the time in seconds.
 */
typedef	MyDword32T	MyTimeT;

/**
 * MyMtimeT:
 *
 * Mtime type.
 * Stores the time in microseconds.
 */
typedef	MyDword32T	MyMtimeT;

/**
 * MyAddressT:
 *
 * 48 bit address type.
 */
typedef	MyByte8T	MyAddressT[BITS2BYTES(48)];
typedef	MyAddressT	MyAddrT;

/**
 * MyByte24T:
 *
 * 24 bit data type.
 * Needed for Logical Netword Identification LIND
 */
typedef	MyByte8T	MyByte24T[BITS2BYTES(24)];

/**
 * MyByte64T:
 *
 * 64 bit data type.
 * Needed for Sync Word
 */
typedef	MyByte8T	MyByte64T[BITS2BYTES(64)];

/**
 * MyTmT:
 *
 * Time Structure
 */
typedef	struct	_tm_	{
	MyInt16T	tm_sec;
	MyInt16T	tm_min;
	MyInt16T	tm_hour;
	MyInt16T	tm_mday;
	MyInt16T	tm_mon;
	MyInt16T	tm_year;
	MyInt16T	tm_wday;
	MyInt16T	tm_yday;
	MyInt16T	tm_isdst;
}	MyTmT;

/**
 * MyListT:
 *
 * Generic list definition
 */
typedef	struct	_ppslistt_	{
	struct	_ppslistt_	*liNext;
	struct	_ppslistt_	*liPrev;
	void				*liData;
}	MyListT;

/*
 * variables used by the demo applicataion
 */
typedef struct appMemS
{
	MyAddrT    src;
	MyAddrT    dest;
} appMemT;

/*
 * ������
 */
typedef struct rang_dist
{
    MyAddrT         dest;		//Ŀ�ĵ�ַ
    unsigned short  dist;		//����
	MyDword32T		time;		//ʱ���
} Rang_Dist;

/*
 * ��λ��Ϣ
 */
typedef struct Location_info{                             
  MyDword32T time;              //��λ���ʱ��
  MyByte8T   flag;              //�Ƿ���ж�λ�ж�
  MyByte8T   state;             //Ŀǰ��λ״̬,��λ�ɹ���־
  MyByte8T 	 direction;			//����
} Locat_Info;

#endif	/* _NTRXTYPES_H */