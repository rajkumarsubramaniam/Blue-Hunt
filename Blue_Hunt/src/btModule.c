/*
 * btModule.c
 *
 *  Created on: Nov 25, 2017
 *      Author: Raj Kumar
 */

#include "main.h"
#include "native_gecko.h"
#include "btModule.h"

void configureAdvertising(void)
{
	/* Set advertising parameters. 100ms advertisement interval. All channels used.
	 * The first two parameters are minimum and maximum advertising interval, both in
	 * units of (milliseconds * 1.6). The third parameter '7' sets advertising on all channels. */
	gecko_cmd_le_gap_set_adv_parameters(ADV_INTERVAL_SET, ADV_INTERVAL_SET, ADV_CHANNELS);

	/* Start general advertising and enable connections. */
	gecko_cmd_le_gap_set_mode(le_gap_general_discoverable,le_gap_undirected_connectable);
}

EStatus configureConnParam(void)
{
	/*Return value of gecko_cmd_le_connection_set_parameters to be stored here*/
	struct gecko_msg_le_connection_set_parameters_rsp_t* respMsg;
	respMsg = gecko_cmd_le_connection_set_parameters(0xFF,\
			(uint16)CONN_INTERVAL_SET, (uint16)CONN_INTERVAL_SET,(uint16)CONN_SLAVE_LATENCY,(uint16)CONN_TIMEOUT);
	if(respMsg->result != 0)
		return E_FAILURE;
	else
		return E_SUCCESS;
}

EStatus configureSecurity(void)
{
	struct gecko_msg_sm_configure_rsp_t* respMsg;
	struct gecko_msg_sm_set_bondable_mode_rsp_t *respMsgBond;
	struct gecko_msg_sm_set_passkey_rsp_t *respMsgPassKey;

	gecko_cmd_sm_delete_bondings();

	respMsgBond = gecko_cmd_sm_set_bondable_mode(0x01);
	if(respMsgBond->result != 0)
		return E_FAILURE;

	respMsgPassKey = gecko_cmd_sm_set_passkey(12345);
	if(respMsgPassKey->result != 0)
		return E_FAILURE;

	respMsg = gecko_cmd_sm_configure(0x01,sm_io_capability_displayonly);
	if(respMsg->result != 0)
		return E_FAILURE;

	return E_SUCCESS;
}
