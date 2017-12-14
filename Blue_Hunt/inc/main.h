/*
 * main.h
 *
 *  Created on: Nov 22, 2017
 *      Author: Raj Kumar
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

#endif /* INC_MAIN_H_ */
