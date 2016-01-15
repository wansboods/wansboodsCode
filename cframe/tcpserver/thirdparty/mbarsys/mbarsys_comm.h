#ifndef __MBARSYS_COMM_H
#define __MBARSYS_COMM_H

#include "systype.h"
#include "tp_trlv.h"

#define MBARINFO_MESSAGE_CHANNEL_PORT 40001

INT32_T svr_open_mbarsys_msg_channel( VOID );
INT32_T agent_open_mbarsys_msg_channel( VOID );
INT32_T monitor_mbarsys_msg_channel( INT32_T sock );
RTSTATUS close_mbarsys_msg_channel( INT32_T sock );
INT32_T send_data_to_mbarsys_msg_channel( INT32_T sock, TRLV_MESSAGE_HEAD * head, INT32_T headlen, PVOID message );
INT32_T receive_data_from_mbarsys_msg_channel( INT32_T sock, TRLV_MESSAGE_HEAD * head, INT32_T headlen, PVOID * message, INT32_T timeout );
INT32_T exchange_data_in_mbarsys_msg_channel( INT32_T sock, INT32_T msgtype, PVOID sndmsg, INT32_T sndlen, PVOID * rcvmsg, INT32_T * rcvlen, INT32_T timeout );
INT32_T deal_data_from_mbarsys_msg_channel( INT32_T sock, TRLV_MESSAGE_CB cb );
INT32_T exchange_data_in_net_communication_channel( INT32_T sock, INT32_T msgtype, INT8_T * sndmsg, INT32_T sndlen, PVOID * rcvmsg, INT32_T * rcvlen, INT32_T timeout );
#endif//__MBARSYS_COMM_H
