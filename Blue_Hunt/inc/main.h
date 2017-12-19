/*
 * main.h
 *
 *  Created on: Nov 22, 2017
 *      Author: Raj Kumar & Saritha Senguttuvan
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

/*GPS External Signal Macros*/
#define EXT_GPS_RXDATA_READY 	(0x0001)

/*GSM External Signal Macros*/
#define EXT_GSM_RXDATA_READY	(0x0010)

/*GSM External Signal Macros*/
#define EXT_LETIMER_ON			(0x0100)
#define EXT_LETIMER_OFF			(0x0200)

/*Parked Location*/
#define EXT_STATE_DRIVE_TO_PARKED	(0x1000)

/*Error Status enum*/
typedef enum
{
	E_SUCCESS = 0,
	E_FAILURE,
	E_BAD_PTR,
	E_MEM_LIMIT_EXCEEDED,
	E_RX_ERROR,
	E_TX_ERROR,
	E_INVALID_SENTENCE,
	E_INVALID_DATA
}EStatus;

/*Enum for Blue Hunt States*/
typedef enum
{
	BH_STATE_RESET = 0,	/*State when the BLE haven't made a connection for the first time*/
	BH_STATE_PARKED,	/*State when the car is parked i.e., when BLE not connected*/
	BH_STATE_DRIVING,	/*State when driving i.e., when BLE is connected*/
	BH_STATE_HUNT		/*State when the car is stolen i.e., GSM will be turned ON*/
}BHStates;

#endif /* INC_MAIN_H_ */
