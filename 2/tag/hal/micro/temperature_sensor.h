/** @file temperature_sensor.h
 * @brief Header for temperature sensor driver 
 *
 *
 * <!--(C) COPYRIGHT 2010 STMicroelectronics. All rights reserved.        -->
 */
#ifndef _TEMP_SENSOR_H_
#define _TEMP_SENSOR_H_

/** @addtogroup TS Temperature sensor
 *@{
 */

/* Include --------------------------------------------------------------------*/
#include "hal/micro/adc.h"
/* Define --------------------------------------------------------------------*/

/* Typedef -------------------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/** @brief Temperature Sensor Initialization function 
*/
void halTempSensorInit(void);

/** @brief  Get temperature sensor value 
 * @return The temperature value expressed in tenth of Celsius degrees e.g decimal value 273 means 27.3 C
*/
int32u halTempSensorGetValue(void);

/** @} // END addtogroup
 */

#endif /* _TEMP_SENSOR_H_ */

