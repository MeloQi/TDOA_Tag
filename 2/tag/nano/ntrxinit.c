/********************************Copyright (c)**********************************
**                		
**                          				
**-File Info--------------------------------------------------------------------
** File name:			ntrxdil.c
** Last Version:		V1.0
** Descriptions:		nano无线测距初始化相关
**------------------------------------------------------------------------------
** modify by:	 	      QI
** modify date:     	2013-08-12
** Version:	       	V1.0
** Descriptions:    	
*******************************************************************************/

#include "config.h"
#include "ntrxtypes.h"
#include    "ntrxutil.h"
#include    "hwclock.h"
#include    "nnspi.h"


unsigned char yan;

/**
 * NtrxInit:
 *
 * NtrxInit() initializes the entire necessary hardware parts of
 * the nanoNET TRX chip. It also initializes all necessary software data.
 *
 * Returns: TRUE if everything O.K. else FALSE
 */
MyBoolT NtrxInit(void)
{
  settingVal.bw = NA_80MHz;
  settingVal.sd = NA_1us;               //1000ns
  settingVal.sr = NA_1M_S;              //2Mbps
  settingVal.fixnmap=NA_FIX_MODE;
  settingVal.rangingConst=122.500000;

  NTRXCheckTable();

  /* init SPI HW&SW */
  nanorest_init();
  
  InitSPI();
  NanoReset();
  hwdelay(500);
  /*
  * configure SPI output of chip MSB first / push pull
  */
  SetupSPI ();

  /* initialize shadow registers */
  NTRXInitShadowRegister ();

  hwdelay(0); //!! Some short delay seems necessary here??
  /* check connection and firmware version and revision */

  if(NTRXCheckVerRev() == FALSE){
    //err_type = NANO_VER_ERR;                            //错误信息为查询版本错误
    return FALSE;
  }

  NTRXSetupTRxMode (settingVal.bw, settingVal.sd, settingVal.sr);
  ntrxRun = FALSE;
  return TRUE;
}

/**
 * NtrxInit1:
 *
 * NtrxInit() initializes the entire necessary hardware parts of
 * the nanoNET TRX chip. It also initializes all necessary software data.
 *
 * Returns: TRUE if everything O.K. else FALSE
 */
MyBoolT NtrxInit1(void)
{
  settingVal.bw = NA_80MHz;
  settingVal.sd = NA_1us;               //1000ns
  settingVal.sr = NA_1M_S;              //2Mbps
  settingVal.fixnmap=NA_FIX_MODE;
  settingVal.rangingConst=122.493614;

  NTRXCheckTable();

  /* init SPI HW&SW */
  nanorest_init();
  
  InitSPI();
  NanoReset();
  hwdelay(500);
  /*
  * configure SPI output of chip MSB first / push pull
  */
  SetupSPI ();

  /* initialize shadow registers */
  NTRXInitShadowRegister ();

  hwdelay(0); //!! Some short delay seems necessary here??
  /* check connection and firmware version and revision */

  if(NTRXCheckVerRev() == FALSE){
    //err_type = NANO_VER_ERR;                            //错误信息为查询版本错误
    return FALSE;
  }

  NTRXSetupTRxMode (settingVal.bw, settingVal.sd, settingVal.sr);
  ntrxRun = FALSE;
  return TRUE;
}