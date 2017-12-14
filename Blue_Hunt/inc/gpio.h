/*
 * gpio.h
 *
 *  Created on: Nov 25, 2017
 *      Author: Raj Kumar & Saritha Senguttuvan
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_

#include <stdint.h>
#include <stdbool.h>

#define LED_ON_D9()  	controlLed(0,true)
#define LED_ON_D10()	controlLed(1,true)
#define LED_OFF_D9()	controlLed(0,false)
#define LED_OFF_D10()	controlLed(1,false)

/* MAcros for Port and Pin numbers */
#define LED_PORT 		(gpioPortC)
#define LED_D9_PIN 		(10)
#define LED_D10_PIN 	(11)

#define GPS_PORT 		(gpioPortA)
#define BG_RX_GPS_TX	(1)
#define BG_TX_GPS_RX	(2)
#define GPS_MODULE_ON	(4)
#define GPS_PWR_EN		(5)

#define DEBUG_PORTS 	(gpioPortF)

#define GSM_PORT_UART	(gpioPortC)
#define GSM_PORT_POWER	(gpioPortB)
#define GSM_RX			(6)
#define GSM_TX			(7)
#define GSM_CTS			(8)
#define GSM_RTS			(9)
#define	GSM_PWR_SWITCH	(11)
#define GSM_MODULE_ON	(13)



/* function : gpioSetup()
 * brief	: This function is used to setup the GPIO pins of the BGM111
 * param	: None
 * return	: None
 */
void gpioSetup();

/* function : controlLed()
 * brief	: This function is used to turn ON and OFF the LED
 * param	: uint8_t - Choose the LED, bool - Turn ON or OFF
 * return	: None
 */
void controlLed(uint8_t led, bool set);

#endif /* INC_GPIO_H_ */


