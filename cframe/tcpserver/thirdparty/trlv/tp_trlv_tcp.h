#ifndef __TP_TRLV_TCP_H
#define __TP_TRLV_TCP_H

#include "systype.h"
#include "tp_trlv.h"

INT32_T receive_trlv_data_from_tcp_communication_channel( INT32_T sockfd, TRLV_MESSAGE_HEAD * head, INT32_T headlen, PVOID * message, INT32_T timeout );
INT32_T send_trlv_data_to_tcp_communication_channel( INT32_T sockfd, TRLV_MESSAGE_HEAD * head, INT32_T headlen, PVOID message, INT32_T msglen );
RTSTATUS exchange_trlv_data_using_tcp_communication_channel( INT32_T sockfd, TRLV_MESSAGE_HEAD * head, INT32_T headlen, PVOID sndmsgdata, PVOID * rcvmsgdata, INT32_T * rcvlen, INT32_T timeout );
RTSTATUS deal_duplex_trlv_data_using_tcp_communication_channel( INT32_T sockfd, TRLV_MESSAGE_CB cb );
RTSTATUS deal_simplex_trlv_data_using_tcp_communication_channel( INT32_T sockfd, TRLV_MESSAGE_CB cb );

#endif//__TP_TRLV_TCP_H
