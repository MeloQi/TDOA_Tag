/* @file hal/micro/cortexm3/system-timer.c
 * @brief STM32W108 System Timer HAL functions.
 * 
 * \b NOTE:  The Sleep Timer count and compare registers are only 16 bits, but
 * the counter and comparators themselves are actually 32bits.  To deal with
 * this, there are High ("H") and Low ("L") registers.  The High register is
 * the "action" register.  When working with SLEEPTMR_CNT, reading the "H"
 * register will return the upper 16 bits and simultaneously trigger the
 * capture of the lower 16bits in the "L" register.  The "L" register may then
 * be read.  When working with the SLEEPTMR_CMP registers, writing "L" will
 * set a shadow register.  Writing "H" will cause the value from the "H" write
 * and the "L" shadow register to be combined and simultaneously loaded into
 * the true 32bit comparator.
 * 
 * <!--(C) COPYRIGHT 2010 STMicroelectronics. All rights reserved.        -->
 */
#include PLATFORM_HEADER
#include "error.h"
#include "hal/micro/micro-common.h"
#include "hal/micro/cortexm3/micro-common.h"
#include "micro/system-timer.h"


//A simple flag used by internalSleepForQs to check that it has exited
//from sleep mode at the request of the expected timer interrupt.
static boolean sleepTimerInterruptOccurred = FALSE;

// halInternalStartSystemTimer() was moved to micro.c

/**
 * Return a 16 bit real time clock value.  With 1024 clock ticks per second,
 * a single clock tick occurs every 0.9769625 milliseconds, and a rollover
 * of the 16-bit timer occurs every 64 seconds.
 */
int16u halCommonGetInt16uMillisecondTick(void)
{
  return (int16u)halCommonGetInt32uMillisecondTick();
}

int16u halCommonGetInt16uQuarterSecondTick(void)
{
  return (int16u)(halCommonGetInt32uMillisecondTick() >> 8);
}


//sleepTmrArtificalCnt is the value expected for the SLEEPTMR_CNT if
//internalSleepForQs completed successfully due to a timer wakeup.  This
//is the value artifically returned when forceSleepTmrCnt is TRUE.
int32u sleepTmrArtificalCnt = 0;
//forceSleepTmrCnt is a flag indicating that the system timer module
//should force a fake value for SLEEPTMR_CNT whenever the timer is
//accessed.  For FogBugz 11909/11920 workaround.
extern boolean forceSleepTmrCnt;
//wakeupSleepTmrCnt is a capturing of the SLEEPTMR_CNT immediately upon
//waking up from deep sleep.  This value is used to know when the counter
//has ticked forward.  For FogBugz 11909/11920 workaround.
extern int32u wakeupSleepTmrCnt;


/**
 * Return a 32 bit real time clock value.  With 1024 clock ticks per second,
 * a single clock tick occurs every 0.9769625 milliseconds, and a rollover
 * of the 32-bit timer occurs approximately every 48.5 days.
 */
int32u halCommonGetInt32uMillisecondTick(void)
{
  int32u currentTime;
  currentTime = SLEEPTMR_CNTH<<16;
  currentTime |= SLEEPTMR_CNTL;
  
  //forceSleepTmrCnt is set in halInternalSleep instead of performing
  //the delay.  When we slept for exactly the correct amount of time
  //we know what the time is supposed to be.  So, as long as the timer
  //counter is still stuck on a stale value, we can return the
  //value it should be (according to halSleepForQuarterSeconds).
  if(forceSleepTmrCnt && (currentTime == wakeupSleepTmrCnt)) {
    return sleepTmrArtificalCnt;
  }
  forceSleepTmrCnt = FALSE;
  
  return currentTime;
}


void halSleepTimerIsr(void)
{
  //clear the second level interrupts
  INT_SLEEPTMRFLAG = INT_SLEEPTMRWRAP | INT_SLEEPTMRCMPA | INT_SLEEPTMRCMPB;
  
  //mark a sleep timer interrupt as having occurred
  sleepTimerInterruptOccurred = TRUE;
}

#define CONVERT_QS_TO_TICKS(x) ((x) << 8)
#define CONVERT_TICKS_TO_QS(x) ((x) >> 8)
#define TIMER_MAX_QS           0x1000000 // = 4194304 seconds * 4 = 16777216
static StStatus internalSleepForQs(boolean useGpioWakeMask,
                                      int32u *duration,
                                      int32u gpioWakeBitMask)
{
  StStatus status = ST_SUCCESS;
  int32u sleepOverflowCount;
  int32u remainder;
  int32u startCount;
  
  //There is really no reason to bother with a duration of 0qs
  if(*duration==0) {
    INTERRUPTS_ON();
    return status;
  }
  
  ATOMIC(
    //disable top-level interrupt while configuring
    INT_CFGCLR = INT_SLEEPTMR;
    
    //Our tick is calibrated to 1024Hz, giving a tick of 976.6us and an
    //overflow of 4194304.0 seconds.  Calculate the number of sleep overflows
    //in the duration
    sleepOverflowCount = (*duration)/TIMER_MAX_QS;
    //calculate the remaining ticks
    remainder = CONVERT_QS_TO_TICKS((*duration)%TIMER_MAX_QS);
    //grab the starting sleep count
    startCount = halCommonGetInt32uMillisecondTick();
    
    sleepTimerInterruptOccurred = FALSE;
    
    if(remainder) {
      //set CMPA value
      SLEEPTMR_CMPAL = (startCount+remainder)&0xFFFF;
      SLEEPTMR_CMPAH = ((startCount+remainder)>>16)&0xFFFF;
      //clear any stale interrupt flag and set the CMPA interrupt
      INT_SLEEPTMRFLAG = INT_SLEEPTMRCMPA;
      INT_SLEEPTMRCFG = INT_SLEEPTMRCMPA;
    }
    if(sleepOverflowCount) {
      //set CMPB value
      SLEEPTMR_CMPBL = startCount&0xFFFF;
      SLEEPTMR_CMPBH = (startCount>>16)&0xFFFF;
      //clear any stale interrupt flag and set the CMPB interrupt
      //this will also disable the CMPA interrupt, since we only want to wake
      //on overflows, not the remainder yet
      INT_SLEEPTMRFLAG = INT_SLEEPTMRCMPB;
      INT_SLEEPTMRCFG = INT_SLEEPTMRCMPB;
    }
    
    //enable top-level interrupt
    INT_CFGSET = INT_SLEEPTMR;
  )
  
  while(*duration > 0) {
    //Record what the SLEEPTMR_CNT is supposed to be when waking up
    //after a timed sleep.  halInternalSleep is responsible for setting
    //the forceSleepTmrCnt flag.
    if(INT_SLEEPTMRCFG&INT_SLEEPTMRCMPB) {
      //use the value written into SLEEPTMR_CMPB
      sleepTmrArtificalCnt = startCount;
    } else { //INT_SLEEPTMRCFG&INT_SLEEPTMRCMPA
      //use the value written into SLEEPTMR_CMPA
      sleepTmrArtificalCnt = (startCount+remainder);
    }
    
    halSleepWithOptions(SLEEPMODE_WAKETIMER, gpioWakeBitMask);
    
    INT_SLEEPTMRCFG = INT_SLEEPTMRCFG_RESET; //disable all SleepTMR interrupts
    
    //If we didn't come out of sleep via a compare or overflow interrupt,
    //it was an abnormal sleep interruption; report the event.
    if(!sleepTimerInterruptOccurred) {
      status = ST_SLEEP_INTERRUPTED;
      //Update duration to account for how long last sleep was.  Using the
      //startCount variable is always valid because full timer wraps always
      //return to this value and the remainder is an offset from this value.
      //And since the duration is always reduced after each full timer wrap,
      //we only need to calculate the final duration here.
      *duration -= CONVERT_TICKS_TO_QS(halCommonGetInt32uMillisecondTick() -
                                       startCount);
      break;
    } else {
      if(sleepOverflowCount) {
        sleepOverflowCount--;
        *duration -= TIMER_MAX_QS;
      } else {
        *duration -= CONVERT_TICKS_TO_QS(remainder);
      }
      sleepTimerInterruptOccurred = FALSE;
      if(sleepOverflowCount) {
        //enable sleeping for a full timer wrap
        INT_SLEEPTMRFLAG = INT_SLEEPTMRCMPB;
        INT_SLEEPTMRCFG = INT_SLEEPTMRCMPB;
      } else if(!sleepOverflowCount && (*duration>0)){
        //enable sleeping for the remainder
        INT_SLEEPTMRFLAG = INT_SLEEPTMRCMPA;
        INT_SLEEPTMRCFG = INT_SLEEPTMRCMPA;
      }
    }
  }
  
  return status;
}

static StStatus internalSleepForMilliseconds(boolean useGpioWakeMask,
					     int32u *duration,
					     int32u gpioWakeBitMask)
{
  StStatus status = ST_SUCCESS;
  int32u startCount;
  
  //There is really no reason to bother with a duration of 0 ms
  if(*duration==0) {
    INTERRUPTS_ON();
    return status;
  }
  
  ATOMIC(
    //disable top-level interrupt while configuring
    INT_CFGCLR = INT_SLEEPTMR;
    
    //Our tick is calibrated to 1024Hz, giving a tick of 976.6us and an
    //overflow of 4194304.0 seconds.  As a result, our seconds are comprised of
    //1024 "milliseconds".
    
    //grab the starting sleep count
    startCount = halCommonGetInt32uMillisecondTick();
    sleepTimerInterruptOccurred = FALSE;
    //set CMPA value
    SLEEPTMR_CMPAL = (startCount + *duration) & 0xFFFF;
    SLEEPTMR_CMPAH = ((startCount + *duration) >> 16) & 0xFFFF;
    //clear any stale interrupt flag and set the CMPA interrupt
    INT_SLEEPTMRFLAG = INT_SLEEPTMRCMPA;
    INT_SLEEPTMRCFG = INT_SLEEPTMRCMPA;
    //enable top-level interrupt
    INT_CFGSET = INT_SLEEPTMR;
  )
  
  while(*duration > 0) {
    //Record what the SLEEPTMR_CNT is supposed to be when waking up
    //after a timed sleep.  halInternalSleep is responsible for setting
    //the forceSleepTmrCnt flag.
    sleepTmrArtificalCnt = (startCount + *duration);
    
    halSleepWithOptions(SLEEPMODE_WAKETIMER, gpioWakeBitMask);
    
    INT_SLEEPTMRCFG = INT_SLEEPTMRCFG_RESET; //disable all SleepTMR interrupts
    
    //If we didn't come out of sleep via a compare or overflow interrupt,
    //it was an abnormal sleep interruption; report the event.
    if(!sleepTimerInterruptOccurred) {
      status = ST_SLEEP_INTERRUPTED;
      //Update duration to account for how long last sleep was.  Using the
      //startCount variable is valid because full timer wraps do not occur
      //in this function.
      *duration -= halCommonGetInt32uMillisecondTick() - startCount;
      break;
    } else {
      //We have completed the full sleep cycle uninterrupted, set duration to 0
      //so we exit the function
      *duration = 0;
      sleepTimerInterruptOccurred = FALSE;
    }
  }
  
  return status;
}

StStatus halSleepForQsWithOptions(int32u *duration, int32u gpioWakeBitMask)
{
  return internalSleepForQs(TRUE, duration, gpioWakeBitMask);
}

StStatus halSleepForMillisecondsWithOptions(int32u *duration,
					    int32u gpioWakeBitMask)
{
  return internalSleepForMilliseconds(TRUE, duration, gpioWakeBitMask);
}
