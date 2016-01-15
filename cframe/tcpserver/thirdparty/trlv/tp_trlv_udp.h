#ifndef __TP_TRLV_UDP_H
#define __TP_TRLV_UDP_H

#include "systype.h"
#include "tp_trlv.h"

#define MAX_UDP_DATA_LENGTH ( 32 * 1024 )
 
INT32_T receive_trlv_data_from_udp_communication_channel( INT32_T sockfd, TRLV_MESSAGE_HEAD * head, INT32_T headlen, PVOID * message, INT8_T * ipaddr, INT32_T length, UINT16_T * port, INT32_T timeout );
INT32_T send_trlv_data_to_udp_communication_channel( INT32_T sockfd, TRLV_MESSAGE_HEAD * head, INT32_T headlen, PVOID message, INT32_T msglen, INT8_T * ipaddr, UINT16_T port );
RTSTATUS exchange_trlv_data_using_udp_communication_channel( INT32_T sockfd, TRLV_MESSAGE_HEAD * head, INT32_T headlen, PVOID sndmsgdata, PVOID * rcvmsgdata, INT32_T * rcvlen, INT8_T * ipaddr, UINT16_T port, INT32_T timeout );
RTSTATUS deal_duplex_trlv_data_using_udp_communication_channel( INT32_T sockfd, TRLV_MESSAGE_CB cb );
RTSTATUS deal_simplex_trlv_data_using_udp_communication_channel( INT32_T sockfd, TRLV_MESSAGE_CB cb );

#endif//__TP_TRLV_UDP_H
