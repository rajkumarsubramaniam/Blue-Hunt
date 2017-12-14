// This file is generated by Simplicity Studio.  Please do not edit manually.
//
//

// Enclosing macro to prevent multiple inclusion
#ifndef __BLE_CONFIG__
#define __BLE_CONFIG__




// Top level macros
#define SILABS_AF_DEVICE_NAME "soc-empty"


// Generated plugin macros

// Use this macro to check if bg_stack plugin is included
#define SILABS_AF_PLUGIN_BG_STACK

// Use this macro to check if BGAPI plugin is included
#define SILABS_AF_PLUGIN_BGAPI

// Use this macro to check if CMSIS plugin is included
#define SILABS_AF_PLUGIN_CMSIS

// Use this macro to check if DEVICE plugin is included
#define SILABS_AF_PLUGIN_DEVICE

// Use this macro to check if EMDRV plugin is included
#define SILABS_AF_PLUGIN_EMDRV

// Use this macro to check if EMLIB plugin is included
#define SILABS_AF_PLUGIN_EMLIB

// Use this macro to check if KIT_BOARD plugin is included
#define SILABS_AF_PLUGIN_KIT_BOARD

// Use this macro to check if KIT_FLASHPWR plugin is included
#define SILABS_AF_PLUGIN_KIT_FLASHPWR

// Use this macro to check if Legacy BLE OTA Bootloader plugin is included
#define SILABS_AF_PLUGIN_LEGACY_BLE_OTA_BOOTLOADER

// Use this macro to check if RAIL plugin is included
#define SILABS_AF_PLUGIN_RAIL


// Custom macros

#ifdef EMBER_AF_BOARD_TYPE
#undef EMBER_AF_BOARD_TYPE
#endif
#define EMBER_AF_BOARD_TYPE BRD4300AREVA03


#endif // __BLE_CONFIG__
