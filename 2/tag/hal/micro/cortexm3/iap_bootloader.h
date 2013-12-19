/** @file iap_bootloader.h
 * @author MCD Application Team
 * @brief IAP bootloader specific definitions and customization
 * <!--(C) COPYRIGHT 2010 STMicroelectronics. All rights reserved.        -->
 */

#ifndef __IAP_BOOTLOADER_H__
#define __IAP_BOOTLOADER_H__

/**
 * @brief Application start address
 */
#define IAP_BOOTLOADER_APPLICATION_ADDRESS   0x08003000

/**
 * @brief Radio PAN ID for OTA bootloader
 */
#define IAP_BOOTLOADER_PAN_ID 0xb00b

/**
 * @brief Signature used for switch to application
 */
#define IAP_BOOTLOADER_APP_SWITCH_SIGNATURE 0xb001204d

/**
 * @brief Radio default channel for OTA bootloader
 */
#define IAP_BOOTLOADER_DEFAULT_CHANNEL 0x0F
#ifndef INCLUDE_FROM_ASSEMBLER
/** @brief Indicate whether hardware activation is force for OTA bootloader
 *
 * This function is provided to allow users to customize the hardware activation of the OTA bootloader based on the specific
 * features of their board.
 * This code should be simple, reliable and small in footprint. It return TRUE or FALSE to indicate that
 * the OTA bootloader must be activated or not.
 * The default example implementation is for ST boards and it activates OTA bootloader by button S1 pressure.
 *
 * @return TRUE or FALSE to indicate that hardware activation is wanted or not.
 */
extern int8u bootloaderOTAHardwareActivation(void);

/** @brief Provide a mechanism to inform users that the OTA bootloader has been activated.
 *
 * This function is provided to allow users to customize the indication of activation of the OTA bootloader based on the specific
 * features of their board.
 * This code should be simple, reliable and small in footprint. 
 * The default example implementation is for ST boards and it will switch LED D1 on when the OTA bootloader is activated.
 *
 */
extern void bootloaderOTAIndicateActivation(void);
#endif // INCLUDE_FROM_ASSEMBLER
#endif // __IAP_BOOTLOADER_H__
