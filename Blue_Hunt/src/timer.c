/*
 * timer.c
 *
 *  Created on: Dec 4, 2017
 *      Author: Raj Kumar & Saritha Senguttuvan
 */

#include "em_letimer.h"
#include "native_gecko.h"
#include "em_core.h"
#include "main.h"
#include "timer.h"

/*Global variables*/
extern uint32_t externalSignals;
void letimerSetup()
{
	int COMP0_SET;
	int COMP1_SET;
	int flagInt;

	/* Set configurations for LETIMER 0 */
	LETIMER_Init_TypeDef letimerInit;

	letimerInit.enable         = false;                 /* Start counting when init completed. */
	letimerInit.debugRun       = false;                 /* Counter shall not keep running during debug halt. */
	letimerInit.comp0Top       = true;                  /* Load COMP0 register into CNT when counter underflows. COMP0 is used as TOP */
	letimerInit.bufTop         = false;                 /* Don't load COMP1 into COMP0 when REP0 reaches 0. */
	letimerInit.out0Pol        = 0;                     /* Idle value for output 0. */
	letimerInit.out1Pol        = 1;                     /* Idle value for output 1. */
	letimerInit.ufoa0          = letimerUFOANone;       /* PWM output on output 0 */
	letimerInit.ufoa1          = letimerUFOANone;       /* Pulse output on output 1*/
	letimerInit.repMode        = letimerRepeatFree;     /* Count until stopped */
	/* Initialize LETIMER */
	LETIMER_Init(LETIMER0, &letimerInit);

	/*Setting OFF TIME*/
	COMP0_SET = OFF_TIME * ULFRCO_FREQUENCY; 	/*COMP0 register value will have the count for the OFF time of the LED*/
	LETIMER0->CNT = COMP0_SET;					/*Load the COMP0 register to the counter*/
	LETIMER_CompareSet(LETIMER0, 0, COMP0_SET); /*LOad the COMP0 register*/

	/*Setting ON TIME*/
	COMP1_SET = ON_TIME * ULFRCO_FREQUENCY;
	LETIMER_CompareSet(LETIMER0, 1, COMP1_SET);
	while(LETIMER0->SYNCBUSY);					/*Waiting for the SYNC to complete*/

	flagInt = LETIMER0->IF;
	LETIMER0->IFC = flagInt;	/*Clearing Interrupts*/

	LETIMER_IntEnable(LETIMER0, (LETIMER_IEN_UF | LETIMER_IEN_COMP1)); /*Enabling UF and COMP1 Interrupts*/
	//blockSleepMode();					//FIX ME With BLE APIs	/*Block the sleep mode to a particular mode*/
	NVIC_EnableIRQ(LETIMER0_IRQn);	/*Enabling the NVIC*/
}

void LETIMER0_IRQHandler(void)
{
	int flagInt;
	CORE_ATOMIC_IRQ_DISABLE();	/*Disable other interrupts*/
	flagInt = LETIMER0->IF;
	LETIMER0->IFC = flagInt;	/*Clear the interrupts*/
	if(flagInt& LETIMER_IF_COMP1)
	{
		externalSignals |= EXT_LETIMER_ON;
	}
	else if(flagInt& LETIMER_IF_UF)
	{
		externalSignals |= EXT_LETIMER_OFF;
	}
	CORE_ATOMIC_IRQ_ENABLE();	/*Enable Interrupts*/
	gecko_external_signal(externalSignals);
}
