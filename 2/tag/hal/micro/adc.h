/** @file /hal/micro/adc.h
 * @brief Header for A/D converter.
 *
 * <!--(C) COPYRIGHT 2010 STMicroelectronics. All rights reserved.        -->
 */
/** @addtogroup adc
 * Sample A/D converter driver.
 *
 * See adc.h for source code.
 *
 *
 * To use the ADC system, include this file and ensure that
 * ::halInternalInitAdc() is called whenever the microcontroller is
 * started.  
 *
 *
 * @note This code does not allow access to the continuous reading mode of
 * the ADC, which some clients may require.
 *
 * Many functions in this file return an ::StStatus value.  See
 * error-def.h for definitions of all ::StStatus return values.
 *
 *@{
 */
#ifndef __ADC_H__
#define __ADC_H__

#if defined(CORTEXM3_STM32W108)

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief ADC functions employ a user ID to keep different
 * users separate.  
 *
 * Avoid many users because each user requires some
 * amount of state storage.
 *
 * @sa ::NUM_ADC_USERS
 */
enum ADCUser
#else
// A type for the ADC User enumeration.
typedef int8u ADCUser;
enum
#endif //DOXYGEN_SHOULD_SKIP_THIS
{
  /** LQI User ID. */
  ADC_USER_LQI = 0,
  /** Application User ID */
  ADC_USER_APP = 1,
  /** Application User ID */
  ADC_USER_APP2 = 2
};

/** @brief Be sure to update ::NUM_ADC_USERS if additional users are added
 * to the ::ADCUser list.
 */
#define NUM_ADC_USERS 3 // make sure to update if the above is adjusted


/** @brief A type for the channel enumeration 
 */
typedef int8u ADCChannelType;

/** @brief A type for the reference voltage enumeration 
 */
typedef int8u ADCReferenceType;

/** @brief A type for the sample rate enumeration 
 */
typedef int8u ADCRateType;

#if defined(CORTEXM3)
  #include "cortexm3/adc.h"
#else
  // platform that doesn't have ADC support
#endif

/** @brief Initializes and powers-up the ADC.  Should also be
 * called to wake from sleep.
 */
void halInternalInitAdc(void);


/** @brief Shuts down the voltage reference and ADC system to
 * minimize power consumption in sleep.
 */
void halInternalSleepAdc(void);


/** @brief Starts an ADC conversion for the user specified by \c id.
 *
 * @appusage The application must set \c reference to the voltage
 * reference desired (see the ADC references enum,
 * set \c channel to the channel number
 * required (see the ADC channel enum), and set \c rate to reflect the
 * number of bits of accuracy desired (see the ADC rates enum)
 *
 * @param id        An ADC user.
 * 
 * @param reference Voltage reference to use, chosen from enum
 * ::ADCReferenceType
 * 
 * @param channel   Microprocessor channel number.
 * 
 * @param rate     Rate number
 * 
 * @return One of the following: 
 * - ST_ADC_CONVERSION_DEFERRED   if the conversion is still waiting 
 * to start.
 * - ST_ADC_CONVERSION_BUSY       if the conversion is currently taking 
 * place.
 * - ST_ERR_FATAL                 if a passed parameter is invalid.
 */
StStatus halStartAdcConversion(ADCUser id,
                                  ADCReferenceType reference,
                                  ADCChannelType channel,
                                  ADCRateType rate);

/** @brief Returns the status of a pending conversion
 * previously started by ::halStartAdcConversion().  If the conversion
 * is complete, writes the raw register value of the conversion (the unaltered
 * value taken directly from the ADC's data register) into \c value.
 *
 * @param id     An ADC user.
 *
 * @param value  Pointer to an int16u to be loaded with the new value. Take
 * note that the Atmel's ADC only generates 8-bit values which are loaded into
 * the lower 8 bits of \c value.
 * 
 * @return One of the following: 
 * - ::ST_ADC_CONVERSION_DONE       if the conversion is complete.
 * - ::ST_ADC_CONVERSION_DEFERRED   if the conversion is still waiting 
 * to start.
 * - ::ST_ADC_CONVERSION_BUSY       if the conversion is currently taking 
 * place.
 * - ::ST_ADC_NO_CONVERSION_PENDING if \c id does not have a pending 
 * conversion.
 */
StStatus halRequestAdcData(ADCUser id, int16u *value);


/** @brief Waits for the user's request to complete and then,
 * if a conversion was done, writes the raw register value of the conversion
 * (the unaltered value taken directly from the ADC's data register) into
 * \c value and returns ::ST_ADC_CONVERSION_DONE, or immediately
 * returns ::ST_ADC_NO_CONVERSION_PENDING.
 *
 * @param id     An ADC user.
 *
 * @param value  Pointer to an int16u to be loaded with the new value. Take
 * note that the Atmel's ADC only generates 8-bit values which are loaded into
 * the lower 8 bits of \c value.
 * 
 * @return One of the following: 
 * - ::ST_ADC_CONVERSION_DONE        if the conversion is complete.
 * - ::ST_ADC_NO_CONVERSION_PENDING  if \c id does not have a pending 
 * conversion.
 */
StStatus halReadAdcBlocking(ADCUser id, int16u *value);


/** @brief Calibrates or recalibrates the ADC system. 
 *
 * @appusage Use this function to (re)calibrate as needed. This function is
 * intended for the STM32W108 microcontroller, which requires proper calibration to calculate
 * a human readible value (a value in volts).  If the app does not call this
 * function, the first time (and only the first time) the function 
 * ::halConvertValueToVolts() is called, this function is invoked.  To
 * maintain accurate volt calculations, the application should call this
 * whenever it expects the temperature of the micro to change.
 *
 * @param id  An ADC user.
 *
 * @return One of the following: 
 * - ::ST_ADC_CONVERSION_DONE        if the calibration is complete.
 * - ::ST_ERR_FATAL                  if the calibration failed.
 */
StStatus halAdcCalibrate(ADCUser id);


/** @brief Convert the raw register value (the unaltered value taken
 * directly from the ADC's data register) into a signed fixed point value with
 * units 10^-4 Volts.  The returned value will be in the range -12000 to
 * +12000 (-1.2000 volts to +1.2000 volts).
 *
 * @appusage Use this function to get a human useful value.
 *
 * @param value  An int16u to be converted.
 * 
 * @return Volts as signed fixed point with units 10^-4 Volts. 
 */
int16s halConvertValueToVolts(int16u value);


/** @brief Calibrates Vref to be 1.2V +/-10mV.
 *
 *  @appusage This function must be called from halInternalInitAdc() before
 *  making ADC readings.  This function is not intended to be called from any
 *  function other than halInternalInitAdc().  This function ensures that the
 *  master cell voltage and current bias values are calibrated before
 *  calibrating Vref.
 */
void stCalibrateVref(void);

#endif /* CORTEXM3_STM32W108 */

#ifdef CORTEXM3_STM32F103
#include "micro/cortexm3/stm32f103ret/adc.h"
#endif /* CORTEXM3_STM32F103 */

#endif // __ADC_H__

/** @} // END addtogroup
 */


