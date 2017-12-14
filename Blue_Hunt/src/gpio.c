/*
 * gpio.c
 *
 *  Created on: Nov 25, 2017
 *      Author: Saritha
 */

#include <stdbool.h>
#include "em_gpio.h"
#include "gpio.h"

void gpioSetup(void)
{
	GPIO_DriveStrengthSet(GPS_PORT, gpioDriveStrengthWeakAlternateWeak); /* Set the GPIO drive strength for Port A*/
	GPIO_DriveStrengthSet(LED_PORT, gpioDriveStrengthWeakAlternateWeak);
	GPIO_DriveStrengthSet(GSM_PORT_POWER, gpioDriveStrengthWeakAlternateWeak);
	/*GSM UART Port and LED are Port C, but configured again for future Compatibility*/
	GPIO_DriveStrengthSet(GSM_PORT_UART, gpioDriveStrengthWeakAlternateWeak);

    /*********************************************************
     * Currently, neither control nor setup the GPIO Pin -  PA5
     * Because of the design issue, MCU is being powered by the Buck Converter powering the GPS,
     * this will help in IOVDD being at 1.8V instead of the 3.3V to achieve UART communication without level-shifters
     ***********************************************************/
    //GPIO_PinModeSet(GPS_PORT,GPS_PWR_EN,gpioModeWiredAndPullUp,1);	/* Set the mode - PortA, Pin 5 - Buck On/Off */
	GPIO_PinModeSet(GPS_PORT,GPS_MODULE_ON,gpioModePushPull,0);	/* Set the mode - PortA, Pin 4 - GPS On/Off */
    GPIO_PinModeSet(GPS_PORT,BG_RX_GPS_TX,gpioModeInput,1);		/* Set the mode - PortA, Pin 1 - LEUART RX/GPS TX */
    GPIO_PinModeSet(GPS_PORT,BG_TX_GPS_RX,gpioModePushPull,1);	/* Set the mode - PortA, Pin 2 - LEUART TX/GPS RX */

    /*LED Pin Modes*/
    GPIO_PinModeSet(LED_PORT,LED_D9_PIN,gpioModePushPull,0);	/*LED D9 Configuration*/
    GPIO_PinModeSet(LED_PORT,LED_D10_PIN,gpioModePushPull,0);	/*LED D10 Configuration*/

    /*GPS Pin Modes*/
    GPIO_PinModeSet(GSM_PORT_POWER,GSM_PWR_SWITCH,gpioModePushPull,0);	/*GSM-POWER Enable*/
    GPIO_PinModeSet(GSM_PORT_POWER,GSM_MODULE_ON,gpioModePushPull,0);	/*GSM - PWR_ON pin control*/
	GPIO_PinModeSet(GSM_PORT_POWER,GSM_RX,gpioModeInput,1);				/*GSM UART RX*/
	GPIO_PinModeSet(GSM_PORT_POWER,GSM_TX,gpioModePushPull,1);			/*GSM UART TX*/
	GPIO_PinModeSet(GSM_PORT_POWER,GSM_CTS,gpioModeInput,0);	/*GSM*/
	GPIO_PinModeSet(GSM_PORT_POWER,GSM_RTS,gpioModePushPull,0);	/*GSM*/

	GPIO_DriveStrengthSet(GSM_PORT_UART, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(GSM_PORT_UART, GSM_TX, gpioModePushPull, 1);
	GPIO_PinModeSet(GSM_PORT_UART, GSM_RX, gpioModeInput, 0);


}

void controlLed(uint8_t led, bool set)
{
	if(led == 0)
	{
		if(set == true)
		{
			GPIO_PinOutSet(LED_PORT, LED_D9_PIN);
		}
		else
		{
			GPIO_PinOutClear(LED_PORT, LED_D9_PIN);
		}
	}
	else
	{
		if(set == true)
		{
			GPIO_PinOutSet(LED_PORT, LED_D10_PIN);
		}
		else
		{
			GPIO_PinOutClear(LED_PORT, LED_D10_PIN);
		}
	}
}
