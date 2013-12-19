/** @file board.c
 * @brief Board file x STM32W108 Kits boards 
 *
 * This file implements a software layer to support all the ST kits boards
 * and deal with the difference in leds, buttons and sensors connected to the board.
 * This a bit too complex for customer boards adaptation and customer can simplify it
 * to adapt it to their hardware environment as they normally don't need to have the software
 * running on several boards.
 * The boards supported are:
 * - MB851 A
 * - MB851 B
 * - MB851 C
 * - MB954 A
 * - MB954 B
 * - MB950 A
 * - MB951 A
 * - PC998 PR2
 * <!--(C) COPYRIGHT 2010 STMicroelectronics. All rights reserved.        -->
 */
#include PLATFORM_HEADER
#include BOARD_HEADER
#ifdef EMBERZNET_HAL
#include "stack/include/ember.h"
#include "stack/include/error.h"
#include "hal/hal.h"
#define PORTA (0 << 3)
#define PORTB (1 << 3)
#define PORTC (2 << 3)
#else
#include "error.h"
#include "hal/hal.h"
#include "hal/micro/cortexm3/mfg-token.h"
#endif
#include <string.h>

const LedResourceType LedsMB851A[] = {
  {
    "D1", /* Green LED */
    PORTB,
    6
  },
  {
    "D3", /* Yellow LED */
    PORTB,
    5
  }
};

const InfraRedLedResourceType infraRedLedMB851A = {
  "D5", /* Infrared LED */
  PORTB,
  4
};


const LedResourceType LedsMB954A[] = {
  {
    "D1", /* Green LED */
    PORTA,
    5
  },
  {
    "D3", /* Yellow LED */
    PORTA,
    0
  }
};

const LedResourceType LedsPC998PR2[] = {
  {
    "D1", /* Yellow LED */
    PORTA,
    7
  }
};

const ButtonResourceType ButtonsPC998PR2[] = {
  {
    "S1",
    PORTA,
    3
  }
};

const ButtonResourceType ButtonsMB851A[] = {
  {
    "S1",
    PORTA,
    7
  }
};

const ButtonResourceType ButtonsMB954A[] = {
  {
    "S1",
    PORTB,
    3
  }
};

const ButtonResourceType ButtonsMB950A[] = {
  {
    "S1",
    PORTB,
    3
  },
  {
    "S2",
    PORTA,
    7
  },
  {
    "S3",
    PORTB,
    7
  },
  {
    "S4",
    PORTB,
    0
  },
  {
    "S5",
    PORTB,
    6
  }
};

const ButtonResourceType ButtonsMB951A[] = {
  {
    "S1",
    PORTA,
    3
  },
};

const TempSensorResourceType tempSensorMB851A = {
  "STLM20",
  PORTB,
  7,
  FALSE
};

const TempSensorResourceType tempSensorMB954A = {
  "STLM20",
  PORTA,
  4,
  TRUE
};

const MemsResourceType memsSensor = {
  "LIS302",
  0
};

const BoardIOType ioMB851A = {
  LedsMB851A,
  ButtonsMB851A,  
};

const BoardIOType ioMB954A = {
  LedsMB954A,
  ButtonsMB954A,  
};

const BoardIOType ioMB950A = {
  LedsMB954A,
  ButtonsMB950A,  
};

const BoardIOType ioMB951A = {
  LedsMB954A,
  ButtonsMB951A,  
};

const BoardIOType ioPC998PR2 = {
  LedsPC998PR2,
  ButtonsPC998PR2
};

const BoardResourcesType MB851A = {
  "MB851 A",
  (BOARD_HAS_MEMS | BOARD_HAS_TEMP_SENSOR | BOARD_HAS_FTDI),
  BUTTONS_MB851A,
  LEDS_MB851A,
  &ioMB851A,
  &infraRedLedMB851A,
  &memsSensor,
  &tempSensorMB851A,
};

const BoardResourcesType MB851B = {
  "MB851 B",
  (BOARD_HAS_MEMS | BOARD_HAS_TEMP_SENSOR | BOARD_HAS_FTDI),
  BUTTONS_MB851B,
  LEDS_MB851B,
  &ioMB851A,
  &infraRedLedMB851A,
  &memsSensor,
  &tempSensorMB851A,
};

const BoardResourcesType MB851C = {
  "MB851 C",
  (BOARD_HAS_MEMS | BOARD_HAS_TEMP_SENSOR | BOARD_HAS_STM32F | BOARD_HAS_EEPROM),
  BUTTONS_MB851C,
  LEDS_MB851C,
  &ioMB954A,
  &infraRedLedMB851A,
  &memsSensor,
  &tempSensorMB954A,
};

const BoardResourcesType MB954A = {
  "MB954 A",
  (BOARD_HAS_MEMS | BOARD_HAS_TEMP_SENSOR | BOARD_HAS_FTDI | BOARD_HAS_PA),
  BUTTONS_MB954A,
  LEDS_MB954A,
  &ioMB954A,
  &infraRedLedMB851A,
  &memsSensor,
  &tempSensorMB954A,
};

const BoardResourcesType MB954B = {
  "MB954 B",
  (BOARD_HAS_MEMS | BOARD_HAS_TEMP_SENSOR | BOARD_HAS_STM32F | BOARD_HAS_EEPROM | BOARD_HAS_PA),
  BUTTONS_MB954B,
  LEDS_MB954B,
  &ioMB954A,
  &infraRedLedMB851A,
  &memsSensor,
  &tempSensorMB954A,
};

const BoardResourcesType MB950A = {
  "MB950 A",
  (BOARD_HAS_MEMS | BOARD_HAS_TEMP_SENSOR | BOARD_HAS_STM32F),
  BUTTONS_MB950A,
  LEDS_MB950A,
  &ioMB950A,
  &infraRedLedMB851A,
  &memsSensor,
  &tempSensorMB954A,
};

const BoardResourcesType MB951A = {
  "MB951 A",
  (BOARD_HAS_STM32F),
  BUTTONS_MB951A,
  LEDS_MB951A,
  &ioMB951A,
  NULL,
  NULL,
  NULL,
};

const BoardResourcesType PC998PR2 = {
  "PC998 PR2",
  (BOARD_HAS_STM32F | BOARD_HAS_LSE | BOARD_LED_NOT_INVERTED),
  BUTTONS_PC998PR2,
  LEDS_PC998PR2,
  &ioPC998PR2,
  NULL,
  NULL,
  NULL
};

static const BoardResourcesType *boardList [] = {
  &MB851A,
  &MB851B,
  &MB851C,
  &MB954A,
  &MB954B,
  &MB950A,
  &MB951A,
  &PC998PR2
};

BoardResourcesType const *boardDescription = NULL;

#define PORT_NAME(x) ((x == PORTA) ? "A": ((x == PORTB) ? "B" : ((x == PORTC) ? "C": "?")))

void halBoardInit(void)
{
  char boardName[16];
  int8s i;
  boardDescription = NULL;
#ifdef EMBERZNET_HAL
  halCommonGetToken(boardName, TOKEN_MFG_BOARD_NAME);
#else
  halCommonGetMfgToken(boardName, TOKEN_MFG_BOARD_NAME);
#endif

  i = 15;
  while ((i >= 0) && (boardName[i] == 0xFF)) {
    boardName[i] = 0;
    i--;
  }

  for (i = 0; i < (sizeof(boardList)/4) ; i++) 
    if (strcmp(boardName, (boardList[i])->name) == 0) {
      boardDescription = (BoardResourcesType *) boardList[i];
      break;
    }

  if (boardDescription == NULL) {
    /* Board type not identified default to MB851A also to support legacy boards */
    boardDescription = (BoardResourcesType *) &MB851A;
  }
  return;
}
BoardResourcesType const *halBoardGetDescription(void)
{
  return boardDescription;
}

void halBoardPowerDown(void)
{
  GPIO_PACFGL = (GPIOCFG_IN              <<PA0_CFG_BIT)|		/* SPI */
                (GPIOCFG_IN              <<PA1_CFG_BIT)|		/* SPI */	
                (GPIOCFG_IN              <<PA2_CFG_BIT)|  		/* SPICLK */
                (GPIOCFG_OUT             <<PA3_CFG_BIT);  		/* SPISSN*/
  GPIO_PACFGH = (GPIOCFG_OUT             <<PA4_CFG_BIT)|		/* RF W/B*/
                (GPIOCFG_IN              <<PA5_CFG_BIT)|  		/* ROOT*/
                (GPIOCFG_IN              <<PA6_CFG_BIT)|		/* DIIO0*/
                (GPIOCFG_IN              <<PA7_CFG_BIT);		
  GPIO_PAOUT  =  0x00000018;

  GPIO_PBCFGL = (GPIOCFG_IN              <<PB0_CFG_BIT)|  		/* UCIRQ*/
                (GPIOCFG_OUT             <<PB1_CFG_BIT)|  		/* SC1SDA */
                (GPIOCFG_IN              <<PB2_CFG_BIT)|		/* SC1SCL */
                (GPIOCFG_IN			     <<PB3_CFG_BIT);		
  GPIO_PBCFGH = (GPIOCFG_OUT           	 <<PB4_CFG_BIT)|		/* PONREST */
  				(GPIOCFG_OUT             <<PB5_CFG_BIT)|        /* RFID控制脚 */
                (GPIOCFG_OUT             <<PB6_CFG_BIT)| 		/* FMQ */
                (GPIOCFG_IN              <<PB7_CFG_BIT);        /* AD_V */
  GPIO_PBOUT  = 0x00000012;
  
  GPIO_PCCFGL = (GPIOCFG_IN              <<PC0_CFG_BIT)| 		/* BAT_IRQ */
                (GPIOCFG_OUT              <<PC1_CFG_BIT)| 		/*MOTOR */
                (GPIOCFG_OUT             <<PC2_CFG_BIT)|		/*LED_R*/	
                (GPIOCFG_OUT          	 <<PC3_CFG_BIT);		/*LED_G*/
  GPIO_PCCFGH = (GPIOCFG_IN              <<PC4_CFG_BIT)|
                (GPIOCFG_IN              <<PC5_CFG_BIT)|
                (GPIOCFG_IN              <<PC6_CFG_BIT)|  
                (GPIOCFG_OUT           	 <<PC7_CFG_BIT);  		/*S1按键*/
  GPIO_PCOUT = 0x00000000;
}

void halBoardPowerUp(void)
{
  GPIO_PACFGL = (GPIOCFG_IN              <<PA0_CFG_BIT)|		/* SPI */
                (GPIOCFG_IN              <<PA1_CFG_BIT)|		/* SPI */	
                (GPIOCFG_IN              <<PA2_CFG_BIT)|  		/* SPICLK */
                (GPIOCFG_OUT             <<PA3_CFG_BIT);  		/* SPISSN*/
  GPIO_PACFGH = (GPIOCFG_IN              <<PA4_CFG_BIT)|		/* RF W/B*/
                (GPIOCFG_IN              <<PA5_CFG_BIT)|  		/* ROOT*/
                (GPIOCFG_OUT             <<PA6_CFG_BIT)|		/* DIIO0*/
                (GPIOCFG_IN              <<PA7_CFG_BIT);		
  GPIO_PAOUT  =  0x00000008;

  GPIO_PBCFGL = (GPIOCFG_IN              <<PB0_CFG_BIT)|  		/* UCIRQ*/
                (GPIOCFG_OUT             <<PB1_CFG_BIT)|  		/* SC1SDA */
                (GPIOCFG_IN              <<PB2_CFG_BIT)|		/* SC1SCL */
                (GPIOCFG_IN			     <<PB3_CFG_BIT);		
  GPIO_PBCFGH = (GPIOCFG_OUT           	 <<PB4_CFG_BIT)|		/* PONREST */
  				(GPIOCFG_OUT             <<PB5_CFG_BIT)|        /* RFID控制脚 */
                (GPIOCFG_OUT             <<PB6_CFG_BIT)| 		/* FMQ */
                (GPIOCFG_IN              <<PB7_CFG_BIT);        /* AD_V */
  GPIO_PBOUT  = 0x00000012;
  
  GPIO_PCCFGL = (GPIOCFG_IN              <<PC0_CFG_BIT)| 		/* BAT_IRQ */
                (GPIOCFG_OUT             <<PC1_CFG_BIT)| 		/*MOTOR */
                (GPIOCFG_OUT             <<PC2_CFG_BIT)|		/*LED_R*/	
                (GPIOCFG_OUT          	 <<PC3_CFG_BIT);		/*LED_G*/
  GPIO_PCCFGH = (GPIOCFG_IN              <<PC4_CFG_BIT)|
                (GPIOCFG_IN              <<PC5_CFG_BIT)|
                (GPIOCFG_IN              <<PC6_CFG_BIT)|  
                (GPIOCFG_IN_PUD          <<PC7_CFG_BIT);  		/*S1按键*/
  GPIO_PCOUT = 0x00000080;
}

#ifdef TEST_COMMANDS
#include <stdio.h>
void printLeds (LedResourceType *leds)
{
  while (leds->name != NULL) {
    printf ("Led %s (port:%s, bit:%d)\r\n", leds->name, PORT_NAME(leds->gpioPort), leds->gpioPin);
    leds++;
  }
}

void printButtons (ButtonResourceType *buttons)
{
  while (buttons->name != NULL) {
    printf ("Button %s (port:%s, bit:%d)\r\n", buttons->name, PORT_NAME(buttons->gpioPort), buttons->gpioPin);
    buttons++;
  }
}

void boardPrintStringDescription(void)
{
  int8u i = 0;

  while (boardList[i] != NULL) {
    if ((boardDescription == boardList[i]) || (boardDescription == NULL)) {
      BoardResourcesType *ptr = boardList[i];
      printf ("*************************************\r\n");
      printf ("Board name = %s\r\n", ptr->name);
      printf ("*************************************\r\n");
      printLeds(ptr->leds);
      printButtons(ptr->buttons);
      if (ptr->mems) {
	printf ("MEMS = %s\r\n", ptr->mems->name);
      }
      if (ptr->temperatureSensor) {
	printf ("Temp sensor = %s, port:%s, pin:%d, adcFix:%s\r\n",
		ptr->temperatureSensor->name,
		PORT_NAME(ptr->temperatureSensor->gpioPort),
		ptr->temperatureSensor->gpioPin,
		ptr->temperatureSensor->adcFix ? "Yes" : "No"
		);
      }
      printf ("EEProm:%s\r\n", ptr->EEProm ? "Yes" : "No");
      printf ("PC i/f:%s\r\n", ptr->FTDInotSTM32 ? "FTDI" : "STM32F");
      printf ("Power Amplifier:%s\r\n", ptr->PowerAmplifier ? "Yes" : "No");
    }
    i++;
  }
  return NULL;
}
#endif
