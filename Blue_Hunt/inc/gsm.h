/*
 * gsm.h
 *
 *  Created on: Dec 6, 2017
 *      Author: Raj Kumar & Saritha Senguttuvan
 */

#ifndef GSM_H_
#define GSM_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/*GSM Macros*/
#define GSM_CIRBUFF_LENGTH 		(1000)
#define GSM_TEST_VERSION_INFO	"AT+UPSV?\r"
#define GSM_NTWK_REG_INFO		"AT+COPS?\r"
#define GSM_TXT_MSG_MODE_CMD	"AT+CMGF=1\r"
#define GSM_CHECK_MEMORIES_SMS	"AT+CPMS?\r"
#define GSM_SMS_STORAGE_SIM		"AT+CPMS=\"SM\"\r"
#define GSM_SEND_SMS			"AT+CMGS=\"+17204519932\"\r"
#define GSM_MSG_PAYLOAD			"This is Blue Hunt. Somebody is accessing your car. Please check."

/*GSM Pin Controls*/
#define MCU_READY_TO_RX_GSMDATA()		GPIO_PinOutClear(GSM_PORT_UART, GSM_RTS) 	/*Pulling RTS low*/
#define MCU_NOTREADY_TO_RX_GSMDATA()	GPIO_PinOutSet(GSM_PORT_UART, GSM_RTS) 		/*Pulling RTS low*/

#define GSM_SEND_WAIT_TIME	(700000)
/*Structure for storing GSM Data*/
typedef struct
{
	uint8_t* buffer;
	size_t wrPtr;
	size_t rdPtr;
	size_t count;
}GSM_Buffer;

/* function : USARTSetup()
 * brief	: This function is used to setup the USART as UART for communication with GSM
 * param	: None
 * return	: None
 */
void USARTSetup(void);

/* function : sendDataToGSM()
 * brief	: This function is used to send data to the GSM over the UART
 * param	: uint8_t* - pointer to the string to send, size_t - length of the string
 * return	: None
 */
void sendDataToGSM(uint8_t *data, size_t len);

/* function : gsmPWR_ONInput()
 * brief	: This function is to control the GSM PWR_ON pin
 * param	: bool - true sets logic high, false - sets logic low
 * return	: None
 */
void gsmPWR_ONInput(bool enable);

/* function : sendGSM_SMS_Alert()
 * brief	: This function is to send message to phone
 * param	: void
 * return	: None
 */
void sendGSM_SMS_Alert();

#endif /* GSM_H_ */
