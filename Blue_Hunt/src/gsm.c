/*
 * gsm.c
 *
 *  Created on: Dec 6, 2017
 *      Author: Raj Kumar & Saritha Senguttuvan
 */

#include "gsm.h"
#include "gpio.h"
#include "em_usart.h"
#include "em_core.h"
#include "em_gpio.h"
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

uint8_t gsmData[50];
size_t gsmIter = 0;
void USARTSetup(void)
{
	USART_InitAsync_TypeDef initAsync; 		//USART_INITASYNC_DEFAULT;
	initAsync.enable = usartDisable;     /* Enable RX/TX when init completed. */
	initAsync.refFreq = 0;               /* Use current configured reference clock for configuring baudrate. */
	initAsync.baudrate = 115200;         /* 115200 bits/s. */
	initAsync.oversampling = usartOVS16;     /* 16x oversampling. */
	initAsync.databits = usartDatabits8; /* 8 databits. */
	initAsync.parity = usartNoParity;  /* No parity. */
	initAsync.stopbits = usartStopbits1; /* 1 stopbit. */
	initAsync.mvdis =  false;          /* Do not disable majority vote. */
	initAsync.prsRxEnable = false;          /* Not USART PRS input mode. */
	initAsync.prsRxCh = usartPrsRxCh0;  /* PRS channel 0. */
	initAsync.autoCsEnable = false;          /* Auto CS functionality enable/disable switch */
	initAsync.autoCsHold = 0;              /* Auto CS Hold cycles */
	initAsync.autoCsSetup = 0;              /* Auto CS Setup cycles */
	USART_InitAsync(USART0, &initAsync);

	USART0->ROUTELOC0 = USART_ROUTELOC0_TXLOC_LOC12 | USART_ROUTELOC0_RXLOC_LOC10;
	USART0->CMD = USART_CMD_CLEARTX | USART_CMD_CLEARRX;    // Insure that no data is in the transmit & receive buffers before enabling
	USART0->ROUTEPEN = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
	//USART0->CTRL |= USART_CTRL_LOOPBK;

	USART0->IEN = USART_IEN_RXDATAV;
	NVIC_EnableIRQ(USART0_RX_IRQn);
	NVIC_EnableIRQ(USART0_TX_IRQn);

	USART_Enable(USART0, usartEnable);

	/*As the MCU will now be ready to Rx data, RTS should be pulled LOW*/
	GPIO_PinOutClear(GSM_PORT_UART, GSM_RTS);
}

void USART0_RX_IRQHandler(void)
{
	int32_t flags;
	flags = USART0->IF;
	USART0->IFC = USART_IF_RXDATAV;
	CORE_AtomicDisableIrq();
	if((flags & USART_IF_RXDATAV) != 0)
	{
		gsmData[gsmIter] = USART0->RXDATA;
		gsmIter++;
		gsmIter = gsmIter%50;
	}
	CORE_AtomicEnableIrq();
}
