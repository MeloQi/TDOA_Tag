/*
 * Copyrite (C) 2010, BeeLinker
 *
 * 文件名称：pub_def.h
 * 文件说明：public typedef
 * 
 * 版本信息：
 * v0.1      wzy        2007/02/16
 * v0.2      wzy        2010/08/23
 * v0.3      wzy        2011/10/03
 */

#ifndef		PUB_DEF_H
#define		PUB_DEF_H



#ifndef NULL
    #define NULL                ((void *)0)
#endif

#ifndef VALID
    #define VALID               1
#endif

#ifndef INVALID
    #define INVALID             0
#endif


typedef void FILE;

typedef uint32_t sysTime_t;

typedef enum  {
	STATUS_OK		=     0, //!< Success
	ERR_IO_ERROR		=    -1, //!< I/O error
	ERR_FLUSHED		=    -2, //!< Request flushed from queue
	ERR_TIMEOUT		=    -3, //!< Operation timed out
	ERR_BAD_DATA		=    -4, //!< Data integrity check failed
	ERR_PROTOCOL		=    -5, //!< Protocol error
	ERR_UNSUPPORTED_DEV	=    -6, //!< Unsupported device
	ERR_NO_MEMORY		=    -7, //!< Insufficient memory
	ERR_INVALID_ARG		=    -8, //!< Invalid argument
	ERR_BAD_ADDRESS		=    -9, //!< Bad address
	ERR_BUSY		=   -10, //!< Resource is busy
	ERR_BAD_FORMAT          =   -11, //!< Data format not recognized

	/**
	 * \brief Operation in progress
	 *
	 * This status code is for driver-internal use when an operation
	 * is currently being performed.
	 *
	 * \note Drivers should never return this status code to any
	 * callers. It is strictly for internal use.
	 */
	OPERATION_IN_PROGRESS	= -128,
}status_t;

    




#endif