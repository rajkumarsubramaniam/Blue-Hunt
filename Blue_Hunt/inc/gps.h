/*
 * gps.h
 *
 *  Created on: Nov 22, 2017
 *      Author: Raj Kumar Subramaniam, Saritha Senguttuvan
 */

#ifndef INC_GPS_H_
#define INC_GPS_H_

#include <stdint.h>
#include "em_leuart.h"
#include "main.h"

#define LEUART_BAUDRATE (9600)
#define MAX_GPS_SENTENCE_LENGTH (80)
#define GPS_SENTENCE_DELIMITER (",")
#define GPS_GLL_MSGID ("$GPGLL")
#define GPS_HIBERNATE_MODE ("$PSRF117,16*0B\r\n")
#define FULL_POWER_MODE ("$PSRF117,16*0B\r\n")
#define SIRF_SMART_GNSS_MODE ("$PSRF117,16*0B\r\n")
#define STANDBY_MODE ("$PSRF117,16*0B\r\n")
#define PUSH_TO_FIX ("$PSRF117,16*0B\r\n")
#define SET_SIRF_AWARE ("$PSRF117,16*0B\r\n")

/* function : LEUARTSetup()
 * brief	: This function is used to setup the LEUART for communication with GPS
 * param	: None
 * return	: None
 */
void LEUARTSetup(void);

/* function : decodeLocation()
 * brief	: This function is used to decode GPS location information from the string
 * param	: int8_t* - pointer to GPS data, float* - Pointers to return the Longitude and Latitude data
 * return	: E_SUCCESS on success and Error values in case of failure.
 */
EStatus decodeLocation(uint8_t* gpsSentence, float* longitude, float* latitude);

/* function : gpsBuckEnable()
 * brief	: This function is used to enable the buck converter for the GPS
 * param	: bool - to enable/disable
 * return	: None
 */
void gpsBuckEnable(bool enable);

/* function : gpsModuleEnable()
 * brief	: This function is used to enable the GPS module
 * param	: bool - to enable/disable
 * return	: None
 */
void gpsModuleEnable(bool enable);

/* function : leuartTransfer()
 * brief	: This function is used to transfer data via UART
 * param	: int8_t * - pointer to the data to be sent, size_t - Length of the data
 * return	: E_SUCCESS on success and Error values in case of failure.
 */
EStatus leuartTransfer(int8_t * transferData, size_t len);

EStatus setHibernateMode(void);

EStatus setFullPowerMode(void);

EStatus setSiRFSmartGNSSMode(void);

EStatus setTricklePower(void);

EStatus setPushToFix(void);

EStatus setSiRFAware(void);

#endif /* INC_GPS_H_ */