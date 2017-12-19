/*
 * gps.c
 *
 *  Created on: Nov 22, 2017
 *      Author: Raj Kumar & Saritha Senguttuvan
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "em_core.h"
#include "em_leuart.h"
#include "em_gpio.h"
#include "native_gecko.h"
#include "gpio.h"
#include "gps.h"

int8_t* gpsSentence;	/*Variable to store received GPS data*/
GPS_Buffer *gpsData;
size_t parser = 0;		/*Array index for GPS data variable*/

/*Global Variables for Location data*/
float latitude = 0;
float longitude = 0;

extern uint32_t externalSignals;
extern bool readingNow;				/*Flag to indicate whether the buffer is being read*/
extern BHStates stateChange;
extern BHStates bhCurrentState;

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
#ifdef TESTING
	LEUART0->CTRL |= LEUART_CTRL_LOOPBK;
#endif
	LEUART0->IEN = LEUART_IEN_RXDATAV;

	/*Block sleep mode should be set to EM2*/

	/*Enable LEUART TX and RX*/
	LEUART0->CMD |= LEUART_CMD_TXEN | LEUART_CMD_RXEN;
	LEUART0->CMD |= LEUART_CMD_CLEARRX;
	LEUART0->IFC = LEUART0->IF;
	NVIC_EnableIRQ(LEUART0_IRQn);
	/*Initially Only Tx is enabled to avoid unnecessary GPS data*/
	LEUART_Enable(LEUART0, leuartEnableTx);

	gpsData = (GPS_Buffer *)malloc(sizeof(GPS_Buffer));
	gpsData->buffer = (uint8_t*)malloc(GPS_CIRBUFF_LENGTH);
	memset((void*)gpsData->buffer,0,GPS_CIRBUFF_LENGTH);
	gpsData->rdPtr = 0;
	gpsData->wrPtr = 0;
	gpsData->count = 0;
}

void LEUART0_IRQHandler()
{
	int32_t flags;
	flags = LEUART0->IF;
	LEUART0->IFC = LEUART_IF_RXDATAV;
	CORE_AtomicDisableIrq();
 	if((flags & LEUART_IF_RXDATAV) != 0)
	{
 		gpsData->buffer[gpsData->wrPtr] = LEUART0->RXDATA;				/*Storing Incoming byte into the GPS data*/
 		gpsData->wrPtr = (gpsData->wrPtr+1)%GPS_CIRBUFF_LENGTH;
 		gpsData->count++;
	}
 	CORE_AtomicEnableIrq();
}

EStatus decodeLocation(uint8_t* loc_gpsData, float* loc_longitude, float* loc_latitude)
{
	char *msgField;
	size_t parser = 0;

	msgField = strtok((char*)loc_gpsData, GPS_SENTENCE_DELIMITER);
	while(msgField != NULL)
	{
		if(parser == 0)
		{
			if(strcmp(msgField, GPS_GGA_MSGID) != 0)	/*Comparing MSG ID for GPS Location data*/
			{
				return E_INVALID_SENTENCE;
			}
			else
			{
				/*Turn OFF GPS*/
				externalSignals |= EXT_GPS_RXDATA_READY;
				gecko_external_signal(externalSignals);
			}
		}
		if(parser == 2)
		{
			sscanf(msgField,"%f",loc_latitude);
		}
		if(parser == 4)
		{
			sscanf(msgField,"%f",loc_longitude);
			if((stateChange == BH_STATE_DRIVING) && (bhCurrentState == BH_STATE_PARKED))	/*If prev state was driving and current state is parked*/
			{
				stateChange = BH_STATE_PARKED;
				externalSignals |= EXT_STATE_DRIVE_TO_PARKED;
				gecko_external_signal(externalSignals);
			}
			break;
		}
		msgField = strtok(NULL,GPS_SENTENCE_DELIMITER);
		parser++;
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

/* Enable/Disable the Buck converter for the GPS */
void gpsBuckEnable(bool enable)
{
	if(enable)			/* If enable */
		GPIO_PinOutSet(GPS_PORT, GPS_PWR_EN);
	else									/* If disable */
		GPIO_PinOutClear(GPS_PORT, GPS_PWR_EN);
}

EStatus testTransfer(uint8_t * transferData, size_t len)
{
	for(int i=0;i<len;i++)
	{
		while((LEUART0->IFC & LEUART_IF_RXDATAV));
		while(!(LEUART0->IF & LEUART_IF_TXBL));
		LEUART0->TXDATA = transferData[i];
		while(LEUART0->SYNCBUSY & LEUART_SYNCBUSY_TXDATA);// LEUART_Sync(LEUART0, LEUART_SYNCBUSY_TXDATA);
		while(!(LEUART0->IF & LEUART_IF_TXC));
		//while(!(LEUART0->IF & LEUART_IF_RXDATAV));
		//gpsSentence[i] = LEUART0->RXDATA;
	}
	return E_SUCCESS;
}

EStatus leuartTransfer(uint8_t * transferData, size_t len)
{
	for(int i=0;i<len;i++)
	{
		while(!(LEUART0->IF & LEUART_IF_TXBL));
		LEUART0->TXDATA = transferData[i];
		while(LEUART0->SYNCBUSY & LEUART_SYNCBUSY_TXDATA);
		while(!(LEUART0->IF & LEUART_IF_TXC));
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

EStatus setDBGMode(void)
{
	leuartTransfer(GPS_TURN_OFF_DBG, strlen((char*)GPS_TURN_OFF_DBG));
	return E_SUCCESS;
}

EStatus enableOnlyGGA(void)
{
	setGPSGGA_FilterCmd(GPS_QUERY_GGA_MSG);		/*Query GGA data*/
	setGPSGGA_FilterCmd(GPS_DISABLE_GLL);		/*Disable GLL Data*/
	setGPSGGA_FilterCmd(GPS_DISABLE_GSA);		/*Disable GSA Data*/
	setGPSGGA_FilterCmd(GPS_DISABLE_GSV);		/*Disable GSV Data*/
	setGPSGGA_FilterCmd(GPS_DISABLE_RMC);		/*Disable RMC Data*/
	return E_SUCCESS;
}

/* Function to enable only the GGA data from GPS */
EStatus setGPSGGA_FilterCmd(uint8_t* data)
{
	/*Message ID $PSRF103 PSRF103 protocol header
	Msg 00 Message to control.
	Mode 01 0=Set Rate
			1=Query one time
			2=ABP On
			3=ABP Off
	Rate 00 Output Rate, 0 = Off
			1–255 = seconds between messages(2)
	CksumEnable 01 	0=Disable Checksum
					1=Enable Checksum
	Checksum *25
	<CR><LF> 	End of message termination*/
	uint8_t checkSum = 0;
	uint8_t cmd[80];
	strcpy(cmd, data);
	for(int i=1; cmd[i] != '*';i++)
	{
		checkSum = checkSum^cmd[i];
	}
	sprintf((char*)cmd,"%s%02x\r\n\0",(char*)cmd,checkSum);
	leuartTransfer(cmd, strlen((char*)cmd));			/* Add 2, to include \r\n*/
	return E_SUCCESS;
}

void readGPSBuffer(void)
{
	uint8_t character;
	uint8_t readSentence[100];	/*80 bytes is the standard NMEA size*/
	uint8_t count = 0;
	while(gpsData->count)
	{
		character = gpsData->buffer[gpsData->rdPtr];
		gpsData->buffer[gpsData->rdPtr] = '\0';
		gpsData->rdPtr = (gpsData->rdPtr+1)%GPS_CIRBUFF_LENGTH;
		gpsData->count--;
		if(character == '$')
		{
			count = 0;
		}
		else if(character == '\r')
		{
			readSentence[count] = '\0';
			decodeLocation(readSentence, &latitude, &longitude);
		}
		else
		{
			readSentence[count] = character;
			count++;
		}
	}
	readingNow = false;	/*After reading all the data exit*/
	return;
}
