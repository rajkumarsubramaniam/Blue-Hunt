/*
 * gsm.c
 *
 *  Created on: Dec 6, 2017
 *      Author: Saritha
 */

#include "gsm.h"
#include "em_usart.h"

void USARTSetup(void)
{
	USART_InitAsync_TypeDef initAsync = USART_INITASYNC_DEFAULT;
	USART_InitAsync(USART0, &initAsync);
	//USART0->ROUTEPEN |= USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
	USART0->ROUTELOC0 = USART_ROUTELOC0_TXLOC_LOC12 | USART_ROUTELOC0_RXLOC_LOC10;
	USART0->CMD = USART_CMD_CLEARTX | USART_CMD_CLEARRX;    // Insure that no data is in the transmit & receive buffers before enabling
	USART0->ROUTEPEN = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
	USART0->CTRL |= USART_CTRL_LOOPBK;
	NVIC_EnableIRQ(USART0_RX_IRQn);
	NVIC_EnableIRQ(USART0_TX_IRQn);
	USART_Enable(USART0, usartEnable);
}

void USART0_RX_IRQHandler(void)
{
	int32_t flags;
	flags = USART0->IF;
	USART0->IFC = USART_IF_RXDATAV;
	CORE_AtomicDisableIrq();
	if((flags & USART_IF_RXDATAV) != 0)
	{

	}
	CORE_AtomicEnableIrq();

}
