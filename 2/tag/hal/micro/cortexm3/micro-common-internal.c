/*
 * File: micro-common-internal.c
 * Description: STM32W108 internal, micro specific HAL functions.
 * This file is provided for completeness and it should not be modified
 * by customers as it comtains code very tightly linked to undocumented
 * device features
 *
 * <!--(C) COPYRIGHT 2010 STMicroelectronics. All rights reserved.        -->
 */

#include PLATFORM_HEADER
#include "error.h"
#include "hal/micro/micro-common.h"
#include "hal/micro/cortexm3/micro-common.h"
#include "hal/micro/cortexm3/mfg-token.h"
#include "hal/micro/adc.h"

// Specify whether the regulator trimming is set for normal mode or radio boost mode
static int8u regTrimBoostMode = FALSE;

#define AUXADC_REG (0xC0u)

#define ADC_6MHZ_CLOCK          0
#define ADC_1MHZ_CLOCK          1

#define CAL_ADC_CHANNEL_VDD_4   0x00  //VDD_PADS/4
#define CAL_ADC_CHANNEL_VREG_2  0x01  //VREG_OUT/2
#define CAL_ADC_CHANNEL_TEMP    0x02
#define CAL_ADC_CHANNEL_GND     0x03
#define CAL_ADC_CHANNEL_VREF    0x04
#define CAL_ADC_CHANNEL_I       0x06
#define CAL_ADC_CHANNEL_Q       0x07
#define CAL_ADC_CHANNEL_ATEST_A 0x09
  
void calStartAdcConversion(int8u channel,
                           ADCRateType rate) {
  // Disable the Calibration ADC interrupt so that we can poll it.
  INT_MGMTCFG &= ~INT_MGMTCALADC;

  ATOMIC(
    // Enable the Calibration ADC, choose source, set rate
    //  Always use 6MHz clock
    CAL_ADC_CONFIG =((CAL_ADC_CONFIG_CAL_ADC_EN)                  |
                     (channel << CAL_ADC_CONFIG_CAL_ADC_MUX_BIT)  |
                     (rate << CAL_ADC_CONFIG_CAL_ADC_RATE_BIT)    |
                     (ADC_6MHZ_CLOCK << CAL_ADC_CONFIG_CAL_ADC_CLKSEL_BIT) );
    // Clear any pending Calibration ADC interrupt.  Since we're atomic, the
    // one we're interested in hasn't happened yet (will take ~10us at minimum).
    // We're only clearing stale info.
    INT_MGMTFLAG = INT_MGMTCALADC;
  )
}


int16u calReadAdcBlocking(void) 
{
  // Wait for conversion to complete.
  while ( ! (INT_MGMTFLAG & INT_MGMTCALADC) ) {
    // Idle the processor while we are waiting
    // Use a delicately modified variant of the normal idle code in order
    //  to optimize it down to the fewest possible instructions.
    #ifndef DISABLE_IDLING_DELAY
      // Normal ATOMIC/INTERRUPTS_OFF/INTERRUPTS_ON uses the BASEPRI mask
      //  to juggle priority levels so that the fault handlers can always
      //  be serviced.  But, the WFI instruction is only capable of
      //  working with the PRIMASK bit.  Therefore, we use PRIMASK to keep 
      //  interrupts disabled so that the WFI can return on an interrupt
      // Since the MGMT interrupt is higher priority than ATOMIC, we have
      //  the additional benefit that we don't have to save/restore
      //  BASEPRI as we normally would in order to protect ourselves from
      //  being called from both within and outside of an ATOMIC block.
      // Globally disable interrupts with PRIMASK
      _setPriMask();        
        
      // Enable the cal ADC interrupt so that we can use it to wake ourselves
      //  up from idle.  Note that the ISR should never actually be serviced 
      //  since we will disable it again before clearing PRIMASK below
      INT_CFGSET = INT_MGMT;
      INT_MGMTCFG |= INT_MGMTCALADC;
        
      //an internal function call is made here instead of injecting the
      //"WFI" assembly instruction because injecting assembly code will
      //cause the compiler's optimizer to reduce efficiency.
      #ifdef FLASH_ACCESS_FLASH_LP
        BYPASS_MPU(
          int32u FLASH_ACCESS_SAVED = FLASH_ACCESS;
          FLASH_ACCESS &= ~FLASH_ACCESS_FLASH_LP;
          halInternalIdleSleep();
          FLASH_ACCESS = FLASH_ACCESS_SAVED;
        )
      #else //FLASH_ACCESS_FLASH_LP
        halInternalIdleSleep();
      #endif //FLASH_ACCESS_FLASH_LP

      // Now that we have woken back up, re-disable the cal ADC interrupt so that
      //  we don't have to bother with it getting serviced.  We will deal with
      //  acknowledging the interrupt here after our polling loop ends
      INT_MGMTCFG &= ~INT_MGMTCALADC;
    
      // However, even though we just disabled the mask, we still need to
      //  acknowledge the top level interrupt, or else it will continue to be
      //  pending after primask is cleared
      INT_PENDCLR = INT_MGMT;
        
      // The WFI instruction does not actually clear the PRIMASK bit, it
      //  only allows the PRIMASK bit to be bypassed.  Therefore, we must
      //  manually clear PRIMASK to reenable all interrupts.
      // We did not modify BASEPRI, so normal ATOMIC/INTERRUPTS_OFF/INTERRUPTS_ON
      //  handling will be left in its prior state.
      _clearPriMask();
    #endif // DISABLE_IDLING_DELAY
  }
  // Clear the interrupt for this conversion.
  INT_MGMTFLAG = INT_MGMTCALADC;
  // Get the result.
  return (int16u)CAL_ADC_DATA;
}


int16u calStartAndReadAdc(int8u channel, ADCRateType rate)
{
  calStartAdcConversion(channel,rate);
  return calReadAdcBlocking();
}


StStatus calRequestAdcData(int16u *value) {
  if ( ! (INT_MGMTFLAG & INT_MGMTCALADC) ) {
    // Conversion not done yet.
    return ST_ADC_CONVERSION_BUSY;
  }
  // Clear the interrupt for this conversion.
  INT_MGMTFLAG = INT_MGMTCALADC;
  // Get the result.
  *value = (int16u)CAL_ADC_DATA;
  return ST_ADC_CONVERSION_DONE;
}


void calDisableAdc(void) {
  // Disable the Calibration ADC to save current.
  CAL_ADC_CONFIG &= ~CAL_ADC_CONFIG_CAL_ADC_EN;
}


int16u halMeasureVdd(ADCRateType rate)
{
  int32u Ngnd;
  int32u Nreg;
  int32u Nvdd;
  int16u Vreg = halInternalGetVreg(); // Get 'known' Vreg for normal/boost mode

  //Measure GND
  Ngnd = (int32u) calStartAndReadAdc(CAL_ADC_CHANNEL_GND, rate);
  
  //Measure VREG_OUT/2
  Nreg = (int32u) calStartAndReadAdc(CAL_ADC_CHANNEL_VREG_2, rate);

  //Measure VDD_PADS/4
  Nvdd = (int32u) calStartAndReadAdc(CAL_ADC_CHANNEL_VDD_4, rate);
  
  calDisableAdc();
  
  //Convert the value into mV - multiply ratio Nvdd/Nreg by known Vreg
  return ((((((Nvdd-Ngnd)<<16)/(Nreg-Ngnd))*Vreg)*2)>>16);
}


#ifdef HAL_STANDALONE
// This is a simplified version of the full stCalibrateVref
// It is meant to be used on build for hal only application not linked with
// simplemac library. The full version is embeeded in the simplemac library.
// The simplified version does not support calibration when manufacturing tokens are missing
// or radio boost mode is activated
void stCalibrateVref(void)
{
  // Calibrate Vref by measuring a known voltage, Vdd/2.
  //
  // FIXME: add support for calibration if done in boost mode.
  tokTypeMfgAnalogueTrimBoth biasTrim;
  
  halCommonGetMfgToken(&biasTrim, TOKEN_MFG_ANALOG_TRIM_BOTH);
  
  if ((biasTrim.auxadc == 0xFFFF) | regTrimBoostMode) {
    assert(FALSE);
  } else {
    //The bias trim token is set, so use the trim directly
    int16u temp_value;
    int16u mask = 0xFFFF;

    // halClearLed(BOARDLED3);

    while (SCR_BUSY_REG) ;

    SCR_ADDR_REG = AUXADC_REG ;  // prepare the address to write to

    // initiate read (starts on falling edge of SCR_CTRL_SCR_READ)
    SCR_CTRL_REG = SCR_CTRL_SCR_READ_MASK;
    SCR_CTRL_REG = 0;

    // wait for read to complete
    while (SCR_BUSY_REG) ;

    temp_value = SCR_READ_REG & ~mask;
    temp_value |= biasTrim.auxadc & mask;
    
    SCR_WRITE_REG = temp_value;

    // initiate write (starts on falling edge of SCR_CTRL_SCR_WRITE_MASK)
    SCR_CTRL_REG = SCR_CTRL_SCR_WRITE_MASK;
    SCR_CTRL_REG = 0;

    while (SCR_BUSY_REG) ;
    
  }
}

#endif // HAL_STANDALONE

void halCommonCalibratePads(void)
{
  // We don't need extreme accuracy for this measurement, so we can use
  //  the fastest ADC sample rate
  if(halMeasureVdd(ADC_SAMPLE_CLOCKS_32) < 2700) {
    GPIO_DBGCFG |= GPIO_DBGCFGRSVD;
  } else {
    GPIO_DBGCFG &= ~GPIO_DBGCFGRSVD;
  }
}


void halInternalSetRegTrim(boolean boostMode)
{
  tokTypeMfgRegTrim regTrim;
  int8u trim1V2;
  int8u trim1V8;
  
  halCommonGetMfgToken(&regTrim, TOKEN_MFG_REG_TRIM);
  // The compiler can optimize this function a bit more and keep the 
  // values in processor registers if we use separate local vars instead
  // of just accessing via the structure fields
  trim1V8 = regTrim.regTrim1V8;
  trim1V2 = regTrim.regTrim1V2;
  
  //If tokens are erased, default to reasonable values, otherwise use the
  //token values.
  if((trim1V2 == 0xFF) && (trim1V8 == 0xFF)) {
    trim1V8 = 4;
    trim1V2 = 0;
  }
  
  //When the radio is in boost mode, we have to increase the 1.8V trim.
  if(boostMode) {
    trim1V8 += 2;
    regTrimBoostMode = TRUE;
  } else {
    regTrimBoostMode = FALSE;
  }
  
  //Clamp at 7 to ensure we don't exceed max values, accidentally set
  //other bits, or wrap values.
  if(trim1V8>7) {
    trim1V8 = 7;
  }
  if(trim1V2>7) {
    trim1V2 = 7;
  }
  
  //only set the trim fields so that any test bits are not overwritten
  //(they are used by some test scripts to force an external voltage in)
  SET_REG_FIELD(VREG, VREG_VREG_1V8_TRIM, trim1V8);
  SET_REG_FIELD(VREG, VREG_VREG_1V2_TRIM, trim1V2);
}

void halInternalGetRegTrim(boolean boostMode)
{
  tokTypeMfgRegTrim regTrim;
  int8u trim1V2;
  int8u trim1V8;
  
  halCommonGetMfgToken(&regTrim, TOKEN_MFG_REG_TRIM);
  // The compiler can optimize this function a bit more and keep the 
  // values in processor registers if we use separate local vars instead
  // of just accessing via the structure fields
  trim1V8 = regTrim.regTrim1V8;
  trim1V2 = regTrim.regTrim1V2;
  
  //If tokens are erased, default to reasonable values, otherwise use the
  //token values.
  if((trim1V2 == 0xFF) && (trim1V8 == 0xFF)) {
    trim1V8 = 4;
    trim1V2 = 0;
  }
  
  //When the radio is in boost mode, we have to increase the 1.8V trim.
  if(boostMode) {
    trim1V8 += 2;
  }
  
  //Clamp at 7 to ensure we don't exceed max values, accidentally set
  //other bits, or wrap values.
  if(trim1V8>7) {
    trim1V8 = 7;
  }
  if(trim1V2>7) {
    trim1V2 = 7;
  }
  
  //only set the trim fields so that any test bits are not overwritten
  //(they are used by some test scripts to force an external voltage in)
  SET_REG_FIELD(VREG, VREG_VREG_1V8_TRIM, trim1V8);
  SET_REG_FIELD(VREG, VREG_VREG_1V2_TRIM, trim1V2);
}




int16u halInternalGetVreg(void)
{
  tokTypeMfgRegVoltage1V8 vregOutTok;

  //Convert the value into mV.  VREG_OUT is ideally 1.8V, but it won't be
  //exactly 1.8V.  The actual value is stored in the manufacturing token
  //TOKEN_MFG_1V8_REG_VOLTAGE.  The token stores the value in 10^-4, but we
  //need 10^-3 so divide by 10.  If this token is not set (0xFFFF), then
  //assume 1800mV.
  halCommonGetMfgToken(&vregOutTok, TOKEN_MFG_1V8_REG_VOLTAGE);
  if(vregOutTok == 0xFFFF) {
    vregOutTok = 1800;
  } else {
    vregOutTok /= 10;
  }
  //Adjust vregOutTok if in boost mode, which raises Vreg by 2 clicks.
  //Multiply it by the ratio (47 - regTrimNormal) / (47 - regTrimBoost)
  //where regTrimNormal and regTrimBoost are the regulator trim values for
  //the two modes (from regTrim1V8 token).  The 47 - trim comes from the
  //ratio of resistors used in the bandgap circuit.  The correction is
  //typically +5% (regTrimNormal is usually 3 or 4 and boost is 2 higher).
  if (regTrimBoostMode) {
    tokTypeMfgRegTrim regTrim;
    int8u regTrimNormal, regTrimBoost;

    halCommonGetMfgToken(&regTrim, TOKEN_MFG_REG_TRIM);
    regTrimNormal = regTrim.regTrim1V8;
    if (regTrimNormal == 0xFF) { // If token erased, assume reasonable defaults
      regTrimNormal = 4; // 4 from halInternalSetRegTrim() and Stromboli-S-015_*.doc
    }
    regTrimBoost = regTrimNormal + 2;
    if (regTrimBoost > 7) {
      regTrimBoost = 7; // Peg it at max of 7
    }
    // Increase vregOutTok by the above-indicated ratio for boost compensation
    vregOutTok = (((((int32u) vregOutTok) * (47 - regTrimNormal))
                                          + (47 - regTrimBoost )/2) //rounding
                                          / (47 - regTrimBoost));
  }
  return ((int16u) vregOutTok);
}


// halCommonDelayMicroseconds
// -enables MAC Timer and leaves it enabled.
// -does not touch MAC Timer Compare registers.
// -max delay is 65535 usec.
// NOTE: This function primarily designed for when the chip is running off of
//       the XTAL, which is the most common situation.  When running from
//       OSCHF, though, the clock speed is cut in half, so the input parameter
//       is divided by two.  With respect to accuracy, we're now limited by
//       the accuracy of OSCHF (much lower than XTAL).
void halCommonDelayMicroseconds(int16u us)
{
  int32u beginTime = MAC_TIMER;
  
  //If we're not using the XTAL, the MAC Timer is running off OSCHF,
  //that means the clock is half speed, 6MHz.  We need to halve our delay
  //time.
  if((OSC24M_CTRL&OSC24M_CTRL_OSC24M_SEL)!=OSC24M_CTRL_OSC24M_SEL) {
    us >>= 1;
  }
    
  //we have about 2us of overhead in the calculations
  if(us<=2) {
    return;
  }
  
  // MAC Timer is enabled in stmRadioInit, which may not have been called yet.
  // This algorithm needs the MAC Timer so we enable it here.
  MAC_TIMER_CTRL |= MAC_TIMER_CTRL_MAC_TIMER_EN;

  // since our max delay (65535<<1) is less than half the size of the 
  //  20 bit mac timer, we can easily just handle the potential for
  //  mac timer wrapping by subtracting the time delta and masking out
  //  the extra bits
  while( ((MAC_TIMER-beginTime)&MAC_TIMER_MAC_TIMER_MASK) < us ) {
    ; // spin
  }
}


//Burning cycles for milliseconds is generally a bad idea, but it is
//necessary in some situations.  If you have to burn more than 65ms of time,
//the halCommonDelayMicroseconds function becomes cumbersome, so this
//function gives you millisecond granularity.
void halCommonDelayMilliseconds(int16u ms)
{
  if(ms==0) {
    return;
  }
  
  while(ms-->0) {
    halCommonDelayMicroseconds(1000);
  }
}
