/*
 * clockSetup.c
 *
 *  Created on: Nov 22, 2017
 *      Author: Raj Kumar & Saritha Senguttuvan
 */
#include <stdbool.h>
#include "em_cmu.h"
#include "clockSetup.h"

void clockSetup(void)
{
	/*
	 * Upto this point:
	 * HFRCO is disabled
	 * HFXO is enabled
	 * LFXO is enabled
	 * LFA, LFB, LFE are using LFXO
	 */	
	//SystemHFXOClockSet(19000000);

	/* USART */
	CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);            // Enable HFRC0
  	CMU_ClockSelectSet(cmuClock_HFPER, cmuSelect_HFRCO);

	/*GPS-LEUART is driven by LFB-LFXO - 32768 Hz*/
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_HFLE, true);
	CMU_ClockEnable(cmuClock_CORELE, true);

	CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
	CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);

	CMU_ClockEnable(cmuClock_LEUART0, true);	/*Works in LFB clock*/
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_LETIMER0, true);	/*Works in LFA clock*/
	CMU_ClockEnable(cmuClock_USART0, true);
}
