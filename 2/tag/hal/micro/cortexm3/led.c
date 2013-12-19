/** @file hal/micro/cortexm3/led.c
 *  @brief LED manipulation routines; stack and example APIs
 *
 * <!--(C) COPYRIGHT 2010 STMicroelectronics. All rights reserved.        -->
 */

#include PLATFORM_HEADER
#include BOARD_HEADER
#include "hal/micro/led.h"
#include "hal/micro/micro-common.h"
#include "hal/micro/cortexm3/micro-common.h"

void halInitLed(void)
{
    LedResourceType *leds = (LedResourceType *) boardDescription->io->leds;
    int8u i;

   for (i = 0; i < boardDescription->leds; i++) {
      /* LED default off */
      halGpioConfig(PORTx_PIN(leds[i].gpioPort, leds[i].gpioPin), GPIOCFG_OUT);
      halGpioSet(PORTx_PIN(leds[i].gpioPort, leds[i].gpioPin), LED_OFF_VALUE);
  }
}

void halSetLed(HalBoardLed led)
{
  if (led != DUMMY_LED)
    halGpioSet(led, LED_ON_VALUE);
}

void halClearLed(HalBoardLed led)
{
  if (led != DUMMY_LED)
    halGpioSet(led, LED_OFF_VALUE);
}

void halToggleLed(HalBoardLed led)
{
  //to avoid contention with other code using the other pins for other
  //purposes, we disable interrupts since this is a read-modify-write
  if (led != DUMMY_LED) {
    ATOMIC(
	   if(led/8 < 3) {
	     *((volatile int32u *)(GPIO_PxOUT_BASE+(GPIO_Px_OFFSET*(led/8)))) ^= BIT(led&7);
	   }
	   )
  }
}
