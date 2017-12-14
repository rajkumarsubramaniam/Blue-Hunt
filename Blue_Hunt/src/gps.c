/*
 * gps.c
 *
 *  Created on: Nov 22, 2017
 *      Author: Raj Kumar
 */

#include <stdlib.h>
#include <string.h>
#include "em_core.h"
#include "em_leuart.h"
#include "em_gpio.h"
#include "native_gecko.h"
#include "gpio.h"
#include "gps.h"

static int8_t *gpsSentence;	/*Variable to store received GPS data*/
static size_t parser = 0;		/*Array index for GPS data variable*/
//extern uint32_t externalSignals;

void LEUARTSetup(void)
{
	LEUART_Reset(LEUART0);
	LEUART_Init_TypeDef leuartInit;
	leuartInit.baudrate = LEUART_BAUDRATE;
	leuartInit.databits = leuartDatabits8;
	leuartInit.enable = leuartDisable;	//leuartEnable
	leuartInit.parity = leuartNoParity;
	leuartInit.refFreq = 0;
	leuartInit.stopbits = leuartStopbits1;

	/*Initializing the LEUART*/
	LEUART_Init(LEUART0, &leuartInit);

	/*ROUTE LOCATION - TX at 1 and RX at 0*/
	LEUART0->ROUTELOC0 = LEUART_ROUTELOC0_RXLOC_LOC0 | LEUART_ROUTELOC0_TXLOC_LOC2;
	LEUART0->ROUTEPEN  = LEUART_ROUTEPEN_RXPEN | LEUART_ROUTEPEN_TXPEN;
	LEUART0->CTRL |= LEUART_CTRL_LOOPBK;

	LEUART0->IEN = LEUART_IEN_RXDATAV;//| LEUART_IEN_TXC;//| LEUART_IF_TXBL;

	/*Block sleep mode should be set to EM2*/

	/*Enable LEUART TX and RX*/
	LEUART0->CMD |= LEUART_CMD_TXEN | LEUART_CMD_RXEN;
	LEUART0->CMD |= LEUART_CMD_CLEARRX;
	LEUART0->IFC = LEUART0->IF;
	NVIC_EnableIRQ(LEUART0_IRQn);
	LEUART_Enable(LEUART0, leuartEnable);

	gpsSentence = (int8_t*)malloc(MAX_GPS_SENTENCE_LENGTH);
}

void LEUART0_IRQHandler()
{
	int32_t flags;
	flags = LEUART0->IF;
	LEUART0->IFC = LEUART_IF_RXDATAV;
	CORE_AtomicDisableIrq();
	if((flags & LEUART_IF_RXDATAV) != 0)
	{
		gpsSentence[parser] = LEUART0->RXDATA;	/*Storing Incoming byte into the GPS data*/
		if(gpsSentence[parser] == '\n')
		{
		//	parser = 0;
		//	externalSignals |= EXT_GPS_RXDATA_READY;
		}
		else
		{
			parser++;
		}
	}
	CORE_AtomicEnableIrq();
	if(parser == 0)
	{
	//	gecko_external_signal(externalSignals);
	}
}

EStatus decodeLocation(uint8_t* gpsData, float* longitude, float* latitude)
{
	char *msgField;
	char *tempStr;
	char *errCheck;
	size_t i=0;
	for(i=0, tempStr = (char*)gpsData;;i++,tempStr = NULL)
	{
		msgField = strtok(tempStr, GPS_SENTENCE_DELIMITER);
		switch (i)
		{
			case 0:
				if(strcmp(msgField, GPS_GLL_MSGID) != 0)	/*Comparing MSG ID for GPS Location data*/
				{
					return E_INVALID_SENTENCE;
				}
				break;
			case 1:
				*latitude = (float)strtod(msgField, &errCheck); /*Converting String to Float latitude*/
				if(errCheck == msgField)
				{
					return E_INVALID_DATA;
				}
				break;
		}
	}
	return E_SUCCESS;
}

/* Enable/Disable the GPS enable pin */
void gpsModuleEnable(bool enable)
{
	if(enable)			/* If enable */
		GPIO_PinOutSet(GPS_PORT, GPS_MODULE_ON);
	else									/* If disable */
		GPIO_PinOutClear(GPS_PORT, GPS_MODULE_ON);
}

#if 0
/* Enable/Disable the Buck converter for the GPS */
void gpsBuckEnable(bool enable)
{
	if(enable)			/* If enable */
		//GPIO_PinOutSet(gpioPortA, 5);
	else									/* If disable */
		//GPIO_PinOutClear(gpioPortA, 5);
}
#endif

EStatus leuartTransfer(int8_t * transferData, size_t len)
{
	while(!(LEUART0->IF & LEUART_IF_TXBL));
	for(int i=0;i<len;i++)
	{
		LEUART0->TXDATA = transferData[i];
		while(LEUART0->SYNCBUSY & LEUART_SYNCBUSY_TXDATA);// LEUART_Sync(LEUART0, LEUART_SYNCBUSY_TXDATA);
		while(!(LEUART0->IF & LEUART_IF_TXC));
		GPIO_PinOutClear(gpioPortF, 6);
		GPIO_PinOutSet(gpioPortF, 6);
	}
	return E_SUCCESS;
}

//dummygpsData = "$GPGLL,3723.2475,,12158.3416,W,161229.487,A,A*41\r\n";

EStatus setHibernateMode(void)
{
	leuartTransfer(GPS_HIBERNATE_MODE, strlen(GPS_HIBERNATE_MODE));
	return E_SUCCESS;
}

EStatus setFullPowerMode(void)
{
	leuartTransfer(GPS_HIBERNATE_MODE, strlen(GPS_HIBERNATE_MODE));
	return E_SUCCESS;
}

EStatus setSiRFSmartGNSSMode(void)
{
	return E_SUCCESS;
}

EStatus setTricklePower(void)
{

	return E_SUCCESS;
}

EStatus setPushToFix(void)
{

	return E_SUCCESS;
}

EStatus setSiRFAware(void)
{

	return E_SUCCESS;
}
