/**
 * @file error-def.h
 * @brief Return-code definitions for API functions.
 * 
 * See @ref status_codes for documentation.
 * <!--(C) COPYRIGHT 2010 STMicroelectronics. All rights reserved.        -->
*/

/**
 * @addtogroup status_codes Status codes
 *
 * Many SimpleMAC API functions return an StStatus value to indicate
 * the success or failure of the call.
 * Return codes are one byte long.
 * This page documents the possible status codes and their meanings.
 *
 * See error-def.h for source code.
 *
 * See also error.h for information on how the values for the return codes are
 * built up from these definitions.
 * The file error-def.h is separated from error.h because utilities will use this file
 * to parse the return codes.
 *
 * @note Do not include error-def.h directly. It is included by
 * error.h inside an enum typedef, which is in turn included by st.h.
 *
 * @{
 */

/**
 * @name Generic Messages
 * These messages are system wide.
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The generic "no error" message.
 */
#define ST_SUCCESS(0x00)
#else
DEFINE_ERROR(SUCCESS, 0)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The generic "fatal error" message.
 */
#define ST_ERR_FATAL(0x01)
#else
DEFINE_ERROR(ERR_FATAL, 0x01)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief An invalid value was passed as an argument to a function.
 */
#define ST_BAD_ARGUMENT(0x02)
#else
DEFINE_ERROR(BAD_ARGUMENT, 0x02)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@} // END Generic Messages


/**
 * @name Serial Manager Errors
 */
//@{ 

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Specified an invalid baud rate.
 */
#define ST_SERIAL_INVALID_BAUD_RATE(0x20)
#else
DEFINE_ERROR(SERIAL_INVALID_BAUD_RATE, 0x20)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Specified an invalid serial port.
 */
#define ST_SERIAL_INVALID_PORT(0x21)
#else
DEFINE_ERROR(SERIAL_INVALID_PORT, 0x21)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Tried to send too much data.
 */
#define ST_SERIAL_TX_OVERFLOW(0x22)
#else
DEFINE_ERROR(SERIAL_TX_OVERFLOW, 0x22)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief There was not enough space to store a received character
 * and the character was dropped.
 */
#define ST_SERIAL_RX_OVERFLOW(0x23)
#else
DEFINE_ERROR(SERIAL_RX_OVERFLOW, 0x23)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Detected a UART framing error.
 */
#define ST_SERIAL_RX_FRAME_ERROR(0x24)
#else
DEFINE_ERROR(SERIAL_RX_FRAME_ERROR, 0x24)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Detected a UART parity error.
 */
#define ST_SERIAL_RX_PARITY_ERROR(0x25)
#else
DEFINE_ERROR(SERIAL_RX_PARITY_ERROR, 0x25)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief There is no received data to process.
 */
#define ST_SERIAL_RX_EMPTY(0x26)
#else
DEFINE_ERROR(SERIAL_RX_EMPTY, 0x26)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The receive interrupt was not handled in time, and a 
 * character was dropped.
 */
#define ST_SERIAL_RX_OVERRUN_ERROR(0x27)
#else
DEFINE_ERROR(SERIAL_RX_OVERRUN_ERROR, 0x27)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@}


/**
 * @name MAC Errors
 */  
//@{


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief We expected to receive an ACK following the transmission, but
 * the MAC level ACK was never received.
 */
#define ST_MAC_NO_ACK_RECEIVED(0x40)
#else
DEFINE_ERROR(MAC_NO_ACK_RECEIVED, 0x40)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@}

/**
 * @name  Flash Errors
 */  
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** 
 * @brief A fatal error has occured while trying to write data to the
 * Flash.  The target memory attempting to be programmed is already programmed.
 * The flash write routines were asked to flip a bit from a 0 to 1, which is
 * physically impossible and the write was therefore inhibited.  The data in
 * the flash cannot be trusted after this error.
 */
#define ST_ERR_FLASH_WRITE_INHIBITED(0x46)
#else
DEFINE_ERROR(ERR_FLASH_WRITE_INHIBITED, 0x46)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** 
 * @brief A fatal error has occured while trying to write data to the
 * Flash and the write verification has failed.  The data in the flash
 * cannot be trusted after this error, and it is possible this error is the
 * result of exceeding the life cycles of the flash.
 */
#define ST_ERR_FLASH_VERIFY_FAILED(0x47)
#else
DEFINE_ERROR(ERR_FLASH_VERIFY_FAILED, 0x47)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** 
 * @brief A fatal error has occured while trying to write data to the
 * flash, possibly due to write protection or an invalid address.  The data in
 * the flash cannot be trusted after this error, and it is possible this error
 * is the result of exceeding the life cycles of the flash.
 */
#define ST_ERR_FLASH_PROG_FAIL(0x4B)
#else
DEFINE_ERROR(ERR_FLASH_PROG_FAIL, 0x4B)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** 
 * @brief A fatal error has occured while trying to erase flash, possibly
 * due to write protection.  The data in the flash cannot be trusted after
 * this error, and it is possible this error is the result of exceeding the
 * life cycles of the flash.
 */
#define ST_ERR_FLASH_ERASE_FAIL(0x4C)
#else
DEFINE_ERROR(ERR_FLASH_ERASE_FAIL, 0x4C)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@}


/**
 * @name  HAL Module Errors
 */  
//@{
  

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Conversion is complete.
 */
#define ST_ADC_CONVERSION_DONE(0x80)
#else
DEFINE_ERROR(ADC_CONVERSION_DONE, 0x80)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Conversion cannot be done because a request is being 
 * processed.
 */
#define ST_ADC_CONVERSION_BUSY(0x81)
#else
DEFINE_ERROR(ADC_CONVERSION_BUSY, 0x81)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Conversion is deferred until the current request has been 
 * processed.
 */
#define ST_ADC_CONVERSION_DEFERRED(0x82)
#else
DEFINE_ERROR(ADC_CONVERSION_DEFERRED, 0x82)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief No results are pending.
 */
#define ST_ADC_NO_CONVERSION_PENDING(0x84)
#else
DEFINE_ERROR(ADC_NO_CONVERSION_PENDING, 0x84)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Sleeping (for a duration) has been abnormally interrupted
 * and exited prematurely.
 */
#define ST_SLEEP_INTERRUPTED(0x85)
#else
DEFINE_ERROR(SLEEP_INTERRUPTED, 0x85)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@}

/**
 * @name  PHY Errors
 */  
//@{


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The transmit hardware buffer underflowed.
 */
#define ST_PHY_TX_UNDERFLOW(0x88)
#else
DEFINE_ERROR(PHY_TX_UNDERFLOW, 0x88)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The transmit hardware did not finish transmitting a packet.
 */
#define ST_PHY_TX_INCOMPLETE(0x89)
#else
DEFINE_ERROR(PHY_TX_INCOMPLETE, 0x89)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief An unsupported channel setting was specified.
 */
#define ST_PHY_INVALID_CHANNEL(0x8A)
#else
DEFINE_ERROR(PHY_INVALID_CHANNEL, 0x8A)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief An unsupported power setting was specified.
 */
#define ST_PHY_INVALID_POWER(0x8B)
#else
DEFINE_ERROR(PHY_INVALID_POWER, 0x8B)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The requested operation cannot be completed because the radio
 * is currently busy, either transmitting a packet or performing calibration.
 */
#define ST_PHY_TX_BUSY(0x8C)
#else
DEFINE_ERROR(PHY_TX_BUSY, 0x8C)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The transmit attempt failed because all CCA attempts indicated that
 * the channel was busy.
 */
#define ST_PHY_TX_CCA_FAIL(0x8D)
#else
DEFINE_ERROR(PHY_TX_CCA_FAIL, 0x8D)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The software installed on the hardware doesn't recognize the
 * hardware radio type.
 */
#define ST_PHY_OSCILLATOR_CHECK_FAILED(0x8E)
#else
DEFINE_ERROR(PHY_OSCILLATOR_CHECK_FAILED, 0x8E)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The expected ACK was received after the last transmission.
 */
#define ST_PHY_ACK_RECEIVED(0x8F)
#else
DEFINE_ERROR(PHY_ACK_RECEIVED, 0x8F)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@}


/**
 * @name  Miscellaneous Utility Errors
 */  
//@{


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief A critical and fatal error indicating that the version of the
 * stack trying to run does not match with the chip it is running on. The
 * software (stack) on the chip must be replaced with software that is
 * compatible with the chip.
 */
#define ST_STACK_AND_HARDWARE_MISMATCH(0xB0)
#else
DEFINE_ERROR(STACK_AND_HARDWARE_MISMATCH, 0xB0)
#endif //DOXYGEN_SHOULD_SKIP_THIS


#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** 
 * @brief An index was passed into the function that was larger
 * than the valid range.
 */
#define ST_INDEX_OUT_OF_RANGE(0xB1)
#else
DEFINE_ERROR(INDEX_OUT_OF_RANGE, 0xB1)
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief There are no empty entries left in the table.
 */
#define ST_TABLE_FULL(0xB4)
#else
DEFINE_ERROR(TABLE_FULL, 0xB4)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The requested table entry has been erased and contains
 *   no valid data.
 */
#define ST_TABLE_ENTRY_ERASED(0xB6)
#else
DEFINE_ERROR(TABLE_ENTRY_ERASED, 0xB6)
#endif

//@}

/**
 * @name  Application Errors
 * These error codes are available for application use.
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief This error is reserved for customer application use.
 *  This will never be returned from any portion of the network stack or HAL.
 */
#define ST_APPLICATION_ERROR_0(0xF0)
#define ST_APPLICATION_ERROR_1(0xF1)
#define ST_APPLICATION_ERROR_2(0xF2)
#define ST_APPLICATION_ERROR_3(0xF3)
#define ST_APPLICATION_ERROR_4(0xF4)
#define ST_APPLICATION_ERROR_5(0xF5)
#define ST_APPLICATION_ERROR_6(0xF6)
#define ST_APPLICATION_ERROR_7(0xF7)
#define ST_APPLICATION_ERROR_8(0xF8)
#define ST_APPLICATION_ERROR_9(0xF9)
#define ST_APPLICATION_ERROR_10(0xFA)
#define ST_APPLICATION_ERROR_11(0xFB)
#define ST_APPLICATION_ERROR_12(0xFC)
#define ST_APPLICATION_ERROR_13(0xFD)
#define ST_APPLICATION_ERROR_14(0xFE)
#define ST_APPLICATION_ERROR_15(0xFF)
#else
DEFINE_ERROR( APPLICATION_ERROR_0, 0xF0)
DEFINE_ERROR( APPLICATION_ERROR_1, 0xF1)
DEFINE_ERROR( APPLICATION_ERROR_2, 0xF2)
DEFINE_ERROR( APPLICATION_ERROR_3, 0xF3)
DEFINE_ERROR( APPLICATION_ERROR_4, 0xF4)
DEFINE_ERROR( APPLICATION_ERROR_5, 0xF5)
DEFINE_ERROR( APPLICATION_ERROR_6, 0xF6)
DEFINE_ERROR( APPLICATION_ERROR_7, 0xF7)
DEFINE_ERROR( APPLICATION_ERROR_8, 0xF8)
DEFINE_ERROR( APPLICATION_ERROR_9, 0xF9)
DEFINE_ERROR( APPLICATION_ERROR_10, 0xFA)
DEFINE_ERROR( APPLICATION_ERROR_11, 0xFB)
DEFINE_ERROR( APPLICATION_ERROR_12, 0xFC)
DEFINE_ERROR( APPLICATION_ERROR_13, 0xFD)
DEFINE_ERROR( APPLICATION_ERROR_14, 0xFE)
DEFINE_ERROR( APPLICATION_ERROR_15, 0xFF)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@} // END name group

/** @} END addtogroup */

/**
 * <!-- HIDDEN
 * @page 2p5_to_3p0
 * <hr>
 * All error status codes defined in error-def.h 
 * (and described in @ref status_codes) now have an \c ST_ prefix. 
 * Status code changes also include the following:
 * <ul>
 * <li> <b>New items</b>
 *   - ::ST_ERR_FLASH_WRITE_INHIBITED
 *   - ::ST_ERR_FLASH_VERIFY_FAILED
 *   - ::ST_ADDRESS_TABLE_INDEX_OUT_OF_RANGE
 *   - ::ST_ADDRESS_TABLE_ENTRY_IS_ACTIVE
 *   - ::ST_CANNOT_JOIN_AS_ROUTER
 *   - ::ST_INSUFFICIENT_RANDOM_DATA
 *   - ::ST_APS_ENCRYPTION_ERROR
 *   - ::ST_TRUST_CENTER_MASTER_KEY_NOT_SET
 *   - ::ST_SECURITY_STATE_NOT_SET
 *   - ::ST_SOURCE_ROUTE_FAILURE
 *   - ::ST_MANY_TO_ONE_ROUTE_FAILURE
 *   .
 * <li> <b>Changed items</b>
 *   -
 *   -
 *   .
 * <li> <b>Removed items</b>
 *   - ::SIM_EEPROM_FLASH_WRITE_FAILED
 *   - ::TOO_MANY_CONNECTIONS
 *   - ::CONNECTION_OPEN
 *   - ::CONNECTION_FAILED
 *   - ::CONNECTION_CLOSED
 *   - ::CONNECTION_CLOSING
 *   - ::CONNECTION_NOT_YET_OPEN
 *   - ::INCOMING_SEQUENCED_MESSAGES_LOST
 *   - ::NODEID_INVALID
 *   .
 * </ul>
 * HIDDEN -->
 */

/**
 * <!-- HIDDEN
 * @page 3p0_to_350
 * <hr>
 * <ul>
 * <li> <b>Removed items</b>
 *   - ::ST_ERR_TOKEN_UNKNOWN
 *   - ::ST_ERR_TOKEN_EXISTS
 *   - ::ST_ERR_TOKEN_INVALID_SIZE
 *   - ::ST_ERR_TOKEN_READ_ONLY
 *   .
 * </ul>
 * HIDDEN -->
 */

