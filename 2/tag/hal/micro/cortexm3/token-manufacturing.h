/** @file hal/micro/cortexm3/token-manufacturing.h
 * @brief Definitions for manufacturing tokens.
 *
 * This file should not be included directly. It is accessed by the other
 * token files.
 * 
 * Please see hal/micro/cortexm3/mfg-token.h for a full explanation of the tokens.
 * 
 * The tokens listed below are the manufacturing tokens.  This
 * token definitions file is included from the master definitions
 * file: hal/micro/cortexm3/mfg-token.h  Please see that file for more details. 
 *
 * The user application can include its own manufacturing tokens in a header
 * file similar to this one. The macro ::APPLICATION_MFG_TOKEN_HEADER should
 * be defined to equal the name of the header file in which application
 * manufacturing tokens are defined.
 *
 * The macro DEFINE_MFG_TOKEN() should be used when instantiating a
 * manufacturing token.  Refer to the list of *_LOCATION defines to
 * see what memory is allocated and what memory is unused/available.
 *
 * REMEMBER: By definition, manufacturing tokens exist at fixed addresses.
 *           Tokens should not overlap.
 *
 * Here is a basic example of a manufacturing token header file:
 *
 * @code
 * #define CREATOR_MFG_EXAMPLE 0x4242
 * #ifdef DEFINETYPES
 * typedef int8u tokTypeMfgExample[8];
 * #endif //DEFINETYPES
 * #ifdef DEFINETOKENS
 * #define MFG_EXAMPLE_LOCATION 0x0980
 * DEFINE_MFG_TOKEN(MFG_EXAMPLE,
 *                  tokTypeMfgExample,
 *                  MFG_EXAMPLE_LOCATION,
 *                  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF})
 * #endif //DEFINETOKENS
 * @endcode
 *
 * Since this file contains both the typedefs and the token defs, there are
 * two \#defines used to select which one is needed when this file is included.
 * \#define DEFINETYPES is used to select the type definitions and
 * \#define DEFINETOKENS is used to select the token definitions.
 * Refer to token.h and token.c to see how these are used.
 * 
 * <!--(C) COPYRIGHT 2010 STMicroelectronics. All rights reserved.        -->
 */

/**
 * @name Convenience Macros
 * @brief The following convenience macros are used to simplify the definition
 * process for commonly specified parameters to the basic TOKEN_DEF macro.
 * Please see hal/micro/token.h for a more complete explanation.
 *@{
 */
#define DEFINE_MFG_TOKEN(name, type, address, ...)  \
  TOKEN_NEXT_ADDRESS(name,(address))                  \
  TOKEN_MFG(name, CREATOR_##name, 0, 0, type, 1,  __VA_ARGS__)
/** @} END Convenience Macros */

#ifndef TOKEN_NEXT_ADDRESS
  #define TOKEN_NEXT_ADDRESS(region, address)
#endif

//////////////////////////////////////////////////////////////////////////////
// MANUFACTURING DATA
// *the addresses of these tokens must not change*


// MANUFACTURING CREATORS
// The creator codes are here in one list instead of next to their token
// definitions so comparision of the codes is easier.  The only requirement
// on these creator definitions is that they all must be unique.  A favorite
// method for picking creator codes is to use two ASCII characters in order
// to make the codes more memorable.  Also, the msb of Stack and Manufacturing
// token creator codes is always 1, while the msb of Application token creator
// codes is always 0.  This distinction allows Application tokens
// to freely use the lower 15 bits for creator codes without the risk of
// duplicating a Stack or Manufacturing token creator code.
//--- Fixed Information Block ---
#define CREATOR_MFG_CHIP_DATA          0xC344 // msb+'C'+'D'
#define CREATOR_MFG_PART_DATA          0xF064 // msb+'p'+'d'
#define CREATOR_MFG_TESTER_DATA        0xF464 // msb+'t'+'d'
#define CREATOR_MFG_ST_EUI_64       0xE545 // msb+'e'+'E'
#define CREATOR_MFG_ANALOG_TRIM_NORMAL 0xF46E // msb+'t'+'n'
#define CREATOR_MFG_ANALOG_TRIM_BOOST  0xF442 // msb+'t'+'B'
#define CREATOR_MFG_ANALOG_TRIM_BOTH   0xF462 // msb+'t'+'b'
#define CREATOR_MFG_REG_TRIM           0xF274 // msb+'r'+'t'
#define CREATOR_MFG_1V8_REG_VOLTAGE    0xF276 // msb+'r'+'v'
#define CREATOR_MFG_VREF_VOLTAGE       0xF676 // msb+'v'+'v'
#define CREATOR_MFG_TEMP_CAL           0xF463 // msb+'t'+'c'
#define CREATOR_MFG_FIB_VERSION        0xFF09
#define CREATOR_MFG_FIB_CHECKSUM       0xE663 // msb+'f'+'c'
#define CREATOR_MFG_FIB_OBS            0xE66F // msb+'f'+'o'
//--- Customer Information Block ---
#define CREATOR_MFG_CIB_OBS            0xE36F // msb+'c'+'o'
#define CREATOR_MFG_CUSTOM_VERSION     0xC356
#define CREATOR_MFG_CUSTOM_EUI_64      0xE345
#define CREATOR_MFG_STRING             0xED73
#define CREATOR_MFG_BOARD_NAME         0xC24E // msb+'B'+'N' (Board Name)
#define CREATOR_MFG_EUI_64             0xB634
#define CREATOR_MFG_MANUF_ID           0xC944 // msb+'I'+'D' (Id)
#define CREATOR_MFG_PHY_CONFIG         0xD043 // msb+'P'+'C' (Phy Config)
#define CREATOR_MFG_BOOTLOAD_AES_KEY   0xC24B // msb+'B'+'K' (Bootloader Key)
#define CREATOR_MFG_EZSP_STORAGE       0xCD53
#define CREATOR_MFG_ASH_CONFIG         0xC143 // msb+'A'+'C' (ASH Config)
#define CREATOR_MFG_CBKE_DATA          0xC342 // msb+'C'+'B' (CBke)
#define CREATOR_MFG_INSTALLATION_CODE  0xC943 // msb+'I'+'C' (Installation Code)
#define CREATOR_MFG_OSC24M_BIAS_TRIM   0xB254 // msb+'2'+'T' (2[4mHz] Trim)
#define CREATOR_MFG_SYNTH_FREQ_OFFSET  0xD346 // msb+'S'+'F' (Synth Freq)

// The master defines indicating the verions number these definitions work with.
#define CURRENT_MFG_TOKEN_VERSION 0x01FE //MSB is version, LSB is complement
#define CURRENT_MFG_CUSTOM_VERSION 0x01FE //MSB is version, LSB is complement


#ifdef DEFINETYPES
//--- Fixed Information Block ---
typedef int8u tokTypeMfgChipData[24];
typedef int8u tokTypeMfgPartData[6];
typedef int8u tokTypeMfgTesterData[6];
typedef int8u tokTypeMfgStEui64[8];
typedef struct {
  int16u iffilterL;
  int16u lna;
  int16u ifamp;
  int16u rxadcH;
  int16u prescalar;
  int16u phdet;
  int16u vco;
  int16u loopfilter;
  int16u pa;
  int16u iqmixer;
} tokTypeMfgAnalogueTrim;
typedef struct {
  int16u iffilterH;
  int16u biasmaster;
  int16u moddac;
  int16u auxadc;
  int16u caladc;
} tokTypeMfgAnalogueTrimBoth;
typedef struct {
  int8u regTrim1V2;
  int8u regTrim1V8;
} tokTypeMfgRegTrim;
typedef int16u tokTypeMfgRegVoltage1V8;
typedef int16u tokTypeMfgAdcVrefVoltage;
typedef int16u tokTypeMfgTempCal;
typedef int16u tokTypeMfgFibVersion;
typedef int16u tokTypeMfgFibChecksum;
typedef struct {
  int16u ob2;
  int16u ob3;
  int16u ob0;
  int16u ob1;
} tokTypeMfgFibObs;
//--- Customer Information Block ---
typedef struct {
  int16u ob0;
  int16u ob1;
  int16u ob2;
  int16u ob3;
  int16u ob4;
  int16u ob5;
  int16u ob6;
  int16u ob7;
} tokTypeMfgCibObs;
typedef int16u tokTypeMfgCustomVersion;
typedef int8u tokTypeMfgCustomEui64[8];
typedef int8u tokTypeMfgString[16];
typedef int8u tokTypeMfgBoardName[16];
typedef int16u tokTypeMfgManufId;
typedef int16u tokTypeMfgPhyConfig;
typedef int8u tokTypeMfgBootloadAesKey[16];
typedef int8u tokTypeMfgEui64[8];
typedef int8u tokTypeMfgEzspStorage[8];
typedef int16u tokTypeMfgAshConfig;
typedef struct {
  int8u certificate[48];
  int8u caPublicKey[22];
  int8u privateKey[21];
  // The bottom flag bit is 1 for uninitialized, 0 for initialized.
  // The other flag bits should be set to 0 at initialization.
  int8u flags;
} tokTypeMfgCbkeData;
typedef struct {
  // The bottom flag bit is 1 for uninitialized, 0 for initialized.
  // Bits 1 and 2 give the size of the value string:
  // 0 = 6 bytes, 1 = 8 bytes, 2 = 12 bytes, 3 = 16 bytes.
  // The other flag bits should be set to 0 at initialization.
  // Special flags support.  Due to a bug in the way some customers
  // had programmed the flags field, we will also examine the upper
  // bits 9 and 10 for the size field.  Those bits are also reserved.
  int16u flags;
  int8u value[16];
  int16u crc;
} tokTypeMfgInstallationCode;
typedef int16u tokTypeMfgOsc24mBiasTrim;
typedef int16u tokTypeMfgSynthFreqOffset;
#endif //DEFINETYPES


#ifdef DEFINETOKENS
//The Manufacturing tokens need to be stored at well-defined locations.
//None of these addresses should ever change without extremely great care.
//All locations are OR'ed with DATA_BIG_INFO_BASE to make a full 32bit address.
//--- Fixed Information Block ---
// FIB Bootloader                       0x0000  //1918 bytes
#define MFG_CHIP_DATA_LOCATION          0x077E  //  24 bytes
#define MFG_PART_DATA_LOCATION          0x0796  //   6 bytes
#define MFG_TESTER_DATA_LOCATION        0x079C  //   6 bytes
#define MFG_ST_EUI_64_LOCATION       0x07A2  //   8 bytes
#define MFG_ANALOG_TRIM_NORMAL_LOCATION 0x07AA  //  20 bytes
#define MFG_ANALOG_TRIM_BOOST_LOCATION  0x07BE  //  20 bytes
#define MFG_ANALOG_TRIM_BOTH_LOCATION   0x07D2  //  10 bytes
#define MFG_REG_TRIM_LOCATION           0x07DC  //   2 bytes
#define MFG_1V8_REG_VOLTAGE_LOCATION    0x07DE  //   2 bytes
#define MFG_VREF_VOLTAGE_LOCATION       0x07E0  //   2 bytes
#define MFG_TEMP_CAL_LOCATION           0x07E2  //   2 bytes
//reserved                              0x07E4  //  16 bytes
#define MFG_FIB_VERSION_LOCATION        0x07F4  //   2 bytes
#define MFG_FIB_CHECKSUM_LOCATION       0x07F6  //   2 bytes
#define MFG_FIB_OBS_LOCATION            0x07F8  //   8 bytes
//--- Customer Information Block ---
#define MFG_CIB_OBS_LOCATION            0x0800  //  16 bytes
#define MFG_CUSTOM_VERSION_LOCATION     0x0810  //   2 bytes
#define MFG_CUSTOM_EUI_64_LOCATION      0x0812  //   8 bytes
#define MFG_STRING_LOCATION             0x081A  //  16 bytes
#define MFG_BOARD_NAME_LOCATION         0x082A  //  16 bytes
#define MFG_MANUF_ID_LOCATION           0x083A  //   2 bytes
#define MFG_PHY_CONFIG_LOCATION         0x083C  //   2 bytes
#define MFG_BOOTLOAD_AES_KEY_LOCATION   0x083E  //  16 bytes
#define MFG_EZSP_STORAGE_LOCATION       0x084E  //   8 bytes
#define MFG_ASH_CONFIG_LOCATION         0x0856  //  40 bytes
#define MFG_CBKE_DATA_LOCATION          0x087E  //  92 bytes
#define MFG_INSTALLATION_CODE_LOCATION  0x08DA  //  20 bytes
#define MFG_OSC24M_BIAS_TRIM_LOCATION   0x08EE  //   2 bytes
#define MFG_SYNTH_FREQ_OFFSET_LOCATION  0x08F0  //   2 bytes
// reserved                             0x08F2  // 142 bytes
// unused                               0x097E  //1662 bytes
//--- Virtual MFG Tokens ---
#define MFG_EUI_64_LOCATION             0x8000  // Special Trigger - see token.c

// Define the size of indexed token array
#define MFG_ASH_CONFIG_ARRAY_SIZE       20

//--- Fixed Information Block ---
TOKEN_NEXT_ADDRESS(MFG_CHIP_DATA_ADDR,MFG_CHIP_DATA_LOCATION)
TOKEN_MFG(MFG_CHIP_DATA, CREATOR_MFG_CHIP_DATA,
          0, 0, tokTypeMfgChipData, 1,
          {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
           0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
           0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF})

TOKEN_NEXT_ADDRESS(MFG_PART_DATA_ADDR,MFG_PART_DATA_LOCATION)
TOKEN_MFG(MFG_PART_DATA, CREATOR_MFG_PART_DATA,
          0, 0, tokTypeMfgPartData, 1,
          {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF})

TOKEN_NEXT_ADDRESS(MFG_TESTER_DATA_ADDR,MFG_TESTER_DATA_LOCATION)
TOKEN_MFG(MFG_TESTER_DATA, CREATOR_MFG_TESTER_DATA,
          0, 0, tokTypeMfgTesterData, 1,
          {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF})

TOKEN_NEXT_ADDRESS(MFG_ST_EUI_64_ADDR,MFG_ST_EUI_64_LOCATION)
TOKEN_MFG(MFG_ST_EUI_64, CREATOR_MFG_ST_EUI_64,
          0, 0, tokTypeMfgStEui64, 1,
          {3,0,0,0,0,0,0,3})

TOKEN_NEXT_ADDRESS(MFG_ANALOG_TRIM_NORMAL_ADDR,MFG_ANALOG_TRIM_NORMAL_LOCATION)
TOKEN_MFG(MFG_ANALOG_TRIM_NORMAL, CREATOR_MFG_ANALOG_TRIM_NORMAL,
          0, 0, tokTypeMfgAnalogueTrim, 1,
          {0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
           0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF})

TOKEN_NEXT_ADDRESS(MFG_ANALOG_TRIM_BOOST_ADDR,MFG_ANALOG_TRIM_BOOST_LOCATION)
TOKEN_MFG(MFG_ANALOG_TRIM_BOOST, CREATOR_MFG_ANALOG_TRIM_BOOST,
          0, 0, tokTypeMfgAnalogueTrim, 1,
          {0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
           0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF})

TOKEN_NEXT_ADDRESS(MFG_ANALOG_TRIM_BOTH_ADDR,MFG_ANALOG_TRIM_BOTH_LOCATION)
TOKEN_MFG(MFG_ANALOG_TRIM_BOTH, CREATOR_MFG_ANALOG_TRIM_BOTH,
          0, 0, tokTypeMfgAnalogueTrimBoth, 1,
          {0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF})

TOKEN_NEXT_ADDRESS(MFG_REG_TRIM_ADDR,MFG_REG_TRIM_LOCATION)
TOKEN_MFG(MFG_REG_TRIM, CREATOR_MFG_REG_TRIM,
          0, 0, tokTypeMfgRegTrim, 1,
          {0xFF, 0xFF})

TOKEN_NEXT_ADDRESS(MFG_1V8_REG_VOLTAGE_ADDR,MFG_1V8_REG_VOLTAGE_LOCATION)
TOKEN_MFG(MFG_1V8_REG_VOLTAGE, CREATOR_MFG_1V8_REG_VOLTAGE,
          0, 0, tokTypeMfgRegVoltage1V8, 1,
          0xFFFF)

TOKEN_NEXT_ADDRESS(MFG_VREF_VOLTAGE_ADDR,MFG_VREF_VOLTAGE_LOCATION)
TOKEN_MFG(MFG_VREF_VOLTAGE, CREATOR_MFG_VREF_VOLTAGE,
          0, 0, tokTypeMfgAdcVrefVoltage, 1,
          0xFFFF)

TOKEN_NEXT_ADDRESS(MFG_TEMP_CAL_ADDR,MFG_TEMP_CAL_LOCATION)
TOKEN_MFG(MFG_TEMP_CAL, CREATOR_MFG_TEMP_CAL,
          0, 0, tokTypeMfgTempCal, 1,
          0xFFFF)

TOKEN_NEXT_ADDRESS(MFG_FIB_VERSION_ADDR,MFG_FIB_VERSION_LOCATION)
TOKEN_MFG(MFG_FIB_VERSION, CREATOR_MFG_FIB_VERSION,
          0, 0, tokTypeMfgFibVersion, 1,
          CURRENT_MFG_TOKEN_VERSION)

TOKEN_NEXT_ADDRESS(MFG_FIB_CHECKSUM_ADDR,MFG_FIB_CHECKSUM_LOCATION)
TOKEN_MFG(MFG_FIB_CHECKSUM, CREATOR_MFG_FIB_CHECKSUM,
          0, 0, tokTypeMfgFibChecksum, 1,
          0xFFFF)

TOKEN_NEXT_ADDRESS(MFG_FIB_OBS_ADDR,MFG_FIB_OBS_LOCATION)
TOKEN_MFG(MFG_FIB_OBS, CREATOR_MFG_FIB_OBS,
          0, 0, tokTypeMfgFibObs, 1,
          {0xFFFF,0x03FC,0xAA55,0xFFFF})


//--- Customer Information Block ---
TOKEN_NEXT_ADDRESS(MFG_CIB_OBS_ADDR,MFG_CIB_OBS_LOCATION)
TOKEN_MFG(MFG_CIB_OBS, CREATOR_MFG_CIB_OBS,
          0, 0, tokTypeMfgCibObs, 1,
          {0x5AA5,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF})

TOKEN_NEXT_ADDRESS(MFG_CUSTOM_VERSION_ADDR,MFG_CUSTOM_VERSION_LOCATION)
TOKEN_MFG(MFG_CUSTOM_VERSION, CREATOR_MFG_CUSTOM_VERSION,
          0, 0, tokTypeMfgCustomVersion, 1,
          CURRENT_MFG_CUSTOM_VERSION)

TOKEN_NEXT_ADDRESS(MFG_CUSTOM_EUI_64_ADDR,MFG_CUSTOM_EUI_64_LOCATION)
TOKEN_MFG(MFG_CUSTOM_EUI_64, CREATOR_MFG_CUSTOM_EUI_64,
          0, 0, tokTypeMfgCustomEui64, 1,
          {0,3,3,3,3,3,3,0})

TOKEN_NEXT_ADDRESS(MFG_STRING_ADDR,MFG_STRING_LOCATION)
TOKEN_MFG(MFG_STRING, CREATOR_MFG_STRING,
          0, 0, tokTypeMfgString, 1,
          {0,})
          
TOKEN_NEXT_ADDRESS(MFG_BOARD_NAME_ADDR,MFG_BOARD_NAME_LOCATION)
TOKEN_MFG(MFG_BOARD_NAME, CREATOR_MFG_BOARD_NAME,
          0, 0, tokTypeMfgBoardName, 1,
          {0,})
          
TOKEN_NEXT_ADDRESS(MFG_MANUF_ID_ADDR,MFG_MANUF_ID_LOCATION)
TOKEN_MFG(MFG_MANUF_ID, CREATOR_MFG_MANUF_ID,
          0, 0, tokTypeMfgManufId, 1,
          {0x00,0x00,}) // default to 0 for st
          
TOKEN_NEXT_ADDRESS(MFG_PHY_CONFIG_ADDR,MFG_PHY_CONFIG_LOCATION)
TOKEN_MFG(MFG_PHY_CONFIG, CREATOR_MFG_PHY_CONFIG,
          0, 0, tokTypeMfgPhyConfig, 1,
          {0x00,0x00,}) // default to non-boost mode, internal pa.
          
TOKEN_NEXT_ADDRESS(MFG_BOOTLOAD_AES_KEY_ADDR,MFG_BOOTLOAD_AES_KEY_LOCATION)
TOKEN_MFG(MFG_BOOTLOAD_AES_KEY, CREATOR_MFG_BOOTLOAD_AES_KEY,
          0, 0, tokTypeMfgBootloadAesKey, 1,
          {0xFF,}) // default key is all f's
          
TOKEN_NEXT_ADDRESS(MFG_EZSP_STORAGE_ADDR,MFG_EZSP_STORAGE_LOCATION)
TOKEN_MFG(MFG_EZSP_STORAGE, CREATOR_MFG_EZSP_STORAGE,
          0, 0, tokTypeMfgEzspStorage, 1,
          { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF })

TOKEN_NEXT_ADDRESS(MFG_ASH_CONFIG_ADDR,MFG_ASH_CONFIG_LOCATION)
TOKEN_MFG(MFG_ASH_CONFIG, CREATOR_MFG_ASH_CONFIG,
          0, 1, tokTypeMfgAshConfig, MFG_ASH_CONFIG_ARRAY_SIZE,
          { 0xFFFF, })

TOKEN_NEXT_ADDRESS(MFG_CBKE_DATA_ADDR,MFG_CBKE_DATA_LOCATION)
TOKEN_MFG(MFG_CBKE_DATA, CREATOR_MFG_CBKE_DATA,
          0, 0, tokTypeMfgCbkeData, 1,
          {0xFF,})

TOKEN_NEXT_ADDRESS(MFG_INSTALLATION_CODE_ADDR,MFG_INSTALLATION_CODE_LOCATION)
TOKEN_MFG(MFG_INSTALLATION_CODE, CREATOR_MFG_INSTALLATION_CODE,
          0, 0, tokTypeMfgInstallationCode, 1,
          {0xFF,})

TOKEN_NEXT_ADDRESS(MFG_OSC24M_BIAS_TRIM_ADDR,MFG_OSC24M_BIAS_TRIM_LOCATION)
TOKEN_MFG(MFG_OSC24M_BIAS_TRIM, CREATOR_MFG_OSC24M_BIAS_TRIM,
          0, 0, tokTypeMfgOsc24mBiasTrim, 1,
          {0xFF,})
          
TOKEN_NEXT_ADDRESS(MFG_SYNTH_FREQ_OFFSET_ADDR,MFG_SYNTH_FREQ_OFFSET_LOCATION)
TOKEN_MFG(MFG_SYNTH_FREQ_OFFSET, CREATOR_MFG_SYNTH_FREQ_OFFSET,
          0, 0, tokTypeMfgSynthFreqOffset, 1,
          {0xFF,0xFF,})
          
TOKEN_NEXT_ADDRESS(MFG_EUI_64_ADDR,MFG_EUI_64_LOCATION)
TOKEN_MFG(MFG_EUI_64, CREATOR_MFG_EUI_64,
          0, 0, tokTypeMfgEui64, 1,
          {3,3,3,3,0,0,0,0})


#ifdef APPLICATION_MFG_TOKEN_HEADER
  #include APPLICATION_MFG_TOKEN_HEADER
#endif
          
#undef TOKEN_NEXT_ADDRESS
          
#endif //DEFINETOKENS

