/*
 * btModule.h
 *
 *  Created on: Nov 25, 2017
 *      Author: Raj Kumar & Saritha Senguttuvan
 */

#ifndef INC_BTMODULE_H_
#define INC_BTMODULE_H_

#include "main.h"


/*Macros for Bluetooth Configuration*/
#define ADV_INTERVAL 		((float)800)		/*Expected Advertising Interval in ms*/
#define ADV_INTERVAL_SET 	(ADV_INTERVAL*1.6)	/*Calculated Advertising Interval to set in API*/
#define ADV_CHANNELS		(7)					/*Number of Advertising Channels*/
#define CONN_INTERVAL 		(75.0)			/*Conn Interval in ms*/
#define CONN_INTERVAL_SET 	(CONN_INTERVAL/1.25)/*Calculated Connection Interval to set in API*/
#define CONN_SLAVE_LATENCY 	(4.0)
/*The value in milliseconds must be larger than (1 + latency) *max_interval * 2, where max_interval is given in milliseconds*/
#define CONN_TIMEOUT (((1.0+CONN_SLAVE_LATENCY)*CONN_INTERVAL*2)+1) /* +1 added to maintain the value greater than the formula*/


/* function : configureAdvertising()
 * brief	: This function is used to configure the advertising parameters of BLE
 * param	: None
 * return	: None
 */
void configureAdvertising(void);

/* function : configureConnParam()
 * brief	: This function is used to configure the Connection parameters of BLE
 * param	: None
 * return	: Success or Failure status
 */
EStatus configureConnParam(void);

EStatus configureSecurity(void);

#endif /* INC_BTMODULE_H_ */
