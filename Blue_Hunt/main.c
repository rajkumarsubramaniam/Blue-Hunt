/***********************************************************************************************//**
 * \file   main.c
 * \brief  Silicon Labs Empty Example Project
 *
 * This example demonstrates the bare minimum needed for a Blue Gecko C application
 * that allows Over-the-Air Device Firmware Upgrading (OTA DFU). The application
 * starts advertising after boot and restarts advertising after a connection is closed.
 ***************************************************************************************************
 * <b> (C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

/* Board headers */
#include "boards.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include "aat.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include "em_letimer.h"
#ifdef FEATURE_BOARD_DETECTED
#include "bspconfig.h"
#include "pti.h"
#endif

/* Device initialization header */
#include "InitDevice.h"
#include "main.h"
#include "clockSetup.h"
#include "gps.h"
#include "btModule.h"
#include "gpio.h"
#include "timer.h"
#include "sleep.h"
#include "gsm.h"

#ifdef FEATURE_SPI_FLASH
#include "em_usart.h"
#include "mx25flash_spi.h"
#endif /* FEATURE_SPI_FLASH */

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS 4
#endif
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];

#ifdef FEATURE_PTI_SUPPORT
//static const RADIO_PTIInit_t ptiInit = RADIO_PTI_INIT;
static const RADIO_PTIInit_t ptiInit = {                                                                        \
		RADIO_PTI_MODE_DISABLED,/* RADIO_PTI_MODE_UART,    Simplest output mode is UART mode */        \
	    1600000,                /* Choose 1.6 MHz for best compatibility */    \
	    2,                      /* WSTK uses location 3 for DOUT */            \
	    gpioPortA,              /* Get the port for this loc */                \
	    3,                      /* Get the pin, location should match above */ \
	    22,                      /* WSTK uses location 6 for DCLK */            \
	    gpioPortD,              /* Get the port for this loc */                \
	    14,                     /* Get the pin, location should match above */ \
	    19,                      /* WSTK uses location 6 for DFRAME */          \
	    gpioPortD,              /* Get the port for this loc */                \
	    13,                     /* Get the pin, location should match above */ \
	  };
#endif

/* Gecko configuration parameters (see gecko_configuration.h) */
static const gecko_configuration_t config = {
  .config_flags = 0,
  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap),
  .bluetooth.sleep_clock_accuracy = 100, // ppm
  .gattdb = &bg_gattdb_data,
  .ota.flags = 0,
  .ota.device_name_len = 3,
  .ota.device_name_ptr = "OTA",
  #ifdef FEATURE_PTI_SUPPORT
  .pti = &ptiInit,
  #endif
};

/* Flag for indicating DFU Reset must be performed */
uint8_t boot_to_dfu = 0;

/*External Signals to be used to trigger the event*/
uint32_t externalSignals = 0;

/*Variable to keep note of the Clients need for indication*/
bool notificationON = false;

/*Storing the Connection Handle*/
uint8_t myPhone = 0;

/*GPS Buffer Reading Flag*/
bool readingNow = false;

/*Temporary first attempt*/
bool firstTry = true;

/*Blue Hunt State Variable*/
BHStates bhCurrentState = BH_STATE_RESET;

/*Flag to set the parked Location*/
BHStates stateChange = BH_STATE_RESET;

/*Parked location*/
float parkedLatitude = 0;
float parkedLongitude = 0;

/*Current location*/
extern float latitude;
extern float longitude;

/**
 * @brief  Main function
 */
int main(void)
{
#ifdef FEATURE_SPI_FLASH
  /* Put the SPI flash into Deep Power Down mode for those radio boards where it is available */
  MX25_init();
  MX25_DP();
  /* We must disable SPI communication */
  USART_Reset(USART1);

#endif /* FEATURE_SPI_FLASH */

  /* Initialize peripherals */
  enter_DefaultMode_from_RESET();

  /* Initialize stack */
  gecko_init(&config);

  /*Clock Setup*/
  clockSetup();

  /*GPIO Setup*/
  gpioSetup();

  /*Notify System Turned ON*/
  LED_ON_D9();

  /*LETIMER Setup*/
  letimerSetup();

  /*LEUART Setup*/
  LEUARTSetup();

  /* USART Setup */
  USARTSetup();

  /*Helps to block the system going to EM4*/
 // SLEEP_SleepBlockBegin(sleepEM4);

  /*For testing*/
  //SLEEP_SleepBlockBegin(sleepEM3);

#if 0
  /*Enabling GPS Buck-Converter - Starts the 1.8V Power Rail*/
  gpsBuckEnable(true);
#endif
  /*Turn ON GSM PWR_ON pin*/
  gsmPWR_ONInput(false);

  /*As the MCU will now be ready to Rx data, RTS should be pulled LOW*/
  MCU_READY_TO_RX_GSMDATA();

  /*Turn ON GPS*/
  gpsModuleEnable(true);


  for(int i = 0;i<10000;i++);
  /*Turn Debug OFF in GPS*/
  setDBGMode();

  /*Set Filter for only GGA*/
  enableOnlyGGA();

  /*Turn OFF GPS*/
  //gpsModuleEnable(false);

  while(1)
  {
    /* Event pointer for handling events */
    struct gecko_cmd_packet* evt;

    /* Check for stack event. */
    evt = gecko_wait_event();

    /* Handle events */
    switch (BGLIB_MSG_ID(evt->header))
    {
		case gecko_evt_system_external_signal_id:
			/*GPS DATA Received should be processed*/
			if((EXT_GPS_RXDATA_READY & evt->data.evt_system_external_signal.extsignals) != 0)
			{
				externalSignals &= ~EXT_GPS_RXDATA_READY;
				/*Turn Off GPS*/
				gpsModuleEnable(false);
				gpsClearBuffer();
			}
			/*GSM data received should be processed*/
			if((EXT_GSM_RXDATA_READY & evt->data.evt_system_external_signal.extsignals) != 0)
			{
				externalSignals &= ~EXT_GSM_RXDATA_READY;
			}

			/*Notification to save the parked location*/
			if((EXT_STATE_DRIVE_TO_PARKED & evt->data.evt_system_external_signal.extsignals) != 0)
			{
				externalSignals &= ~EXT_STATE_DRIVE_TO_PARKED;
				parkedLatitude = latitude;
				parkedLongitude = longitude;
			}

			/*LETIMER Events occurred*/
			if((EXT_LETIMER_ON & evt->data.evt_system_external_signal.extsignals) != 0)
			{
				externalSignals &= ~EXT_LETIMER_ON;
				LED_ON_D10();

				if((stateChange == BH_STATE_PARKED) &&(bhCurrentState == BH_STATE_PARKED))
				{
					sendGSM_SMS_Alert();
				}

				/*Turn ON GPS*/
				gpsModuleEnable(true);

				/*Periodic reception of only GGA data*/
				setGPSGGA_FilterCmd(GPS_QUERY_GGA_MSG);
			}
			if((EXT_LETIMER_OFF & evt->data.evt_system_external_signal.extsignals) != 0)
			{
				externalSignals &= ~EXT_LETIMER_OFF;

				LED_OFF_D10();

				if(bhCurrentState == BH_STATE_DRIVING)
				{
					/*Disable LETIMER - as GPS and GSM will be diable after this point*/
					LETIMER_Enable(LETIMER0,false);
				}
				if(readingNow == false)
				{
					readingNow = true;
					readGPSBuffer();
				}
			}
			break;

		/* This boot event is generated when the system boots up after reset.
		* Here the system is set to start advertising immediately after boot procedure. */
		case gecko_evt_system_boot_id:
			configureSecurity();
			/*Set Tx power to )dBm at startup*/
			gecko_cmd_system_set_tx_power(-200);//gecko_cmd_system_set_tx_power(0);
			configureAdvertising();
			break;

		/*Event generated when a bonding is successfully completed*/
		case gecko_evt_sm_bonded_id:
			myPhone = evt->data.evt_sm_bonded.connection;
			uint8 bondingData = evt->data.evt_sm_bonded.bonding;
			break;

		/*Event generated when a Client device tries to connect to the Blue Gecko*/
		case gecko_evt_le_connection_opened_id:
			notificationON = true;
			configureConnParam();
			bhCurrentState = BH_STATE_DRIVING;	/*When the BLE is connected the state is Driving*/
			stateChange = BH_STATE_DRIVING;
			/*Now enable the GPS Rx reception*/
			LEUART_Enable(LEUART0, leuartEnable);
			myPhone = evt->data.evt_le_connection_opened.connection;
			break;

		/*Event generated when passkey is requested*/
		case gecko_evt_sm_passkey_display_id:

			break;

		/*Event generated when a Master device tries to connect to the Blue Gecko*/
		case gecko_evt_le_connection_closed_id:
			/*Set Tx power to )dBm to advertise*/
			gecko_cmd_system_set_tx_power(-200);//gecko_cmd_system_set_tx_power(0);
			/* Check if need to boot to dfu mode */
			if (boot_to_dfu)
			{
				/* Enter to DFU OTA mode */
				gecko_cmd_system_reset(2);
			}
			else
			{
				/* Restart advertising after client has disconnected */
				gecko_cmd_le_gap_set_mode(le_gap_general_discoverable, le_gap_undirected_connectable);
			}
			bhCurrentState = BH_STATE_PARKED;
			/*Enable LETIMER to track the GPS location*/
			LETIMER_Enable(LETIMER0,true);
			break;

		/* Events related to OTA upgrading
		-----------------------------------------------------------------------------
		Check if the user-type OTA Control Characteristic was written.
		If ota_control was written, boot the device into Device Firmware Upgrade (DFU) mode. */
		case gecko_evt_gatt_server_user_write_request_id:
			if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control)
			{
				/* Set flag to enter to OTA mode */
				boot_to_dfu = 1;
				/* Send response to Write Request */
				gecko_cmd_gatt_server_send_user_write_response(evt->data.evt_gatt_server_user_write_request.connection, \
										  gattdb_ota_control,bg_err_success);

				/* Close connection to enter to DFU OTA mode */
				gecko_cmd_endpoint_close(evt->data.evt_gatt_server_user_write_request.connection);
			}
			break;

		default:
			break;
    }
  }
  return 0;
}
