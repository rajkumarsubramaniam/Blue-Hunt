/*
 * gsm.c
 *
 *  Created on: Dec 6, 2017
 *      Author: Raj Kumar & Saritha Senguttuvan
 */

#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "em_usart.h"
#include "em_core.h"
#include "em_gpio.h"
#include "gsm.h"
#include "gpio.h"

/*Global Variables*/
GSM_Buffer* gsmUARTRxData;
bool gsmPhnoAck = false;

/*Current location*/
extern float latitude;
extern float longitude;

void USARTSetup(void)
{
	USART_InitAsync_TypeDef initAsync; 		//USART_INITASYNC_DEFAULT;
	initAsync.enable = usartDisable;     	/* Enable RX/TX when init completed. */
	initAsync.refFreq = 0;               	/* Use current configured reference clock for configuring baudrate. */
	initAsync.baudrate = 115200;         	/* 115200 bits/s. */
	initAsync.oversampling = usartOVS16;    /* 16x oversampling. */
	initAsync.databits = usartDatabits8; 	/* 8 databits. */
	initAsync.parity = usartNoParity;  		/* No parity. */
	initAsync.stopbits = usartStopbits1; 	/* 1 stopbit. */
	initAsync.mvdis =  false;          		/* Do not disable majority vote. */
	initAsync.prsRxEnable = false;          /* Not USART PRS input mode. */
	initAsync.prsRxCh = usartPrsRxCh0;  	/* PRS channel 0. */
	initAsync.autoCsEnable = false;         /* Auto CS functionality enable/disable switch */
	initAsync.autoCsHold = 0;              	/* Auto CS Hold cycles */
	initAsync.autoCsSetup = 0;              /* Auto CS Setup cycles */
	USART_InitAsync(USART0, &initAsync);

	USART0->ROUTELOC0 = USART_ROUTELOC0_TXLOC_LOC12 | USART_ROUTELOC0_RXLOC_LOC10;
	USART0->CMD = USART_CMD_CLEARTX | USART_CMD_CLEARRX;    // Insure that no data is in the transmit & receive buffers before enabling
	USART0->ROUTEPEN = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
	//USART0->CTRL |= USART_CTRL_LOOPBK;

	USART0->IEN = USART_IEN_RXDATAV;
	NVIC_EnableIRQ(USART0_RX_IRQn);
	NVIC_EnableIRQ(USART0_TX_IRQn);

	gsmUARTRxData = (GSM_Buffer*)malloc(sizeof(GSM_Buffer));
	gsmUARTRxData->buffer = (uint8_t*)malloc(GSM_CIRBUFF_LENGTH);
	memset((void*)gsmUARTRxData->buffer,0,GSM_CIRBUFF_LENGTH);
	gsmUARTRxData->rdPtr = 0;
	gsmUARTRxData->wrPtr = 0;
	gsmUARTRxData->count = 0;

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
		gsmUARTRxData->buffer[gsmUARTRxData->wrPtr] = USART0->RXDATA;				/*Storing Incoming byte into the GSM buffer*/
		if(gsmUARTRxData->buffer[gsmUARTRxData->wrPtr] == '>')
		{
			gsmPhnoAck = true;
		}
		gsmUARTRxData->wrPtr = (gsmUARTRxData->wrPtr+1)%GSM_CIRBUFF_LENGTH;			/*GSM Write pointer increment - % helps to jump to first location*/
		gsmUARTRxData->count++;
	}
	CORE_AtomicEnableIrq();
	//gecko_external_signal(externalSignals);
}

void sendDataToGSM(uint8_t *data, size_t len)
{
	size_t gsmIter = 0;
	for(gsmIter=0; gsmIter<len; gsmIter++)
	{
		USART0->TXDATA = data[gsmIter];
		while ((USART0->STATUS & USART_STATUS_TXC) == 0);
	}
}

void gsmPWR_ONInput(bool enable)
{
	if(enable)			/* If enable */
		GPIO_PinOutSet(GSM_PORT_POWER,GSM_MODULE_ON);
	else				/* If disable */
		GPIO_PinOutClear(GSM_PORT_POWER,GSM_MODULE_ON);
}

void sendGSM_SMS_Alert()
{
		uint32_t waitTime = 0;
		uint8_t msgLocData[200];
		sendDataToGSM((uint8_t*)GSM_NTWK_REG_INFO, strlen(GSM_NTWK_REG_INFO));	/*Set the preferred message as SMS*/
		for(waitTime = 0; waitTime < GSM_SEND_WAIT_TIME; waitTime++);

		sendDataToGSM((uint8_t*)GSM_TXT_MSG_MODE_CMD, strlen(GSM_TXT_MSG_MODE_CMD));	/*Check the memory for SMS Storage*/
		for(waitTime = 0; waitTime < GSM_SEND_WAIT_TIME; waitTime++);

		sendDataToGSM((uint8_t*)GSM_CHECK_MEMORIES_SMS, strlen(GSM_CHECK_MEMORIES_SMS));	/*Set SIM card as storage*/
		for(waitTime = 0; waitTime < GSM_SEND_WAIT_TIME; waitTime++);

		sendDataToGSM((uint8_t*)GSM_SMS_STORAGE_SIM, strlen(GSM_SMS_STORAGE_SIM));		/*Directly send without storing SMS*/
		for(waitTime = 0; waitTime < GSM_SEND_WAIT_TIME; waitTime++);

		sendDataToGSM((uint8_t*)GSM_SEND_SMS, strlen(GSM_SEND_SMS));

		while(!gsmPhnoAck);
		gsmPhnoAck = false;
		sprintf((char*)msgLocData,"%s Lat:%d Long:%d\x1A", GSM_MSG_PAYLOAD,(int)latitude, (int)longitude);

		sendDataToGSM(msgLocData, strlen((char*)msgLocData));
}


