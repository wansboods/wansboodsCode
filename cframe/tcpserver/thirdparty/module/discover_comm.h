#ifndef __DISCOVER_COMM_H
#define __DISCOVER_COMM_H

#include "systype.h"
#include "tp_trlv.h"

#define DISCOVER_SERVER_REPLY_STRING "I'm hdplayer manager server"
//#define DISCOVER_CLIENT_REQUEST_STRING "Where is hdplayer server"

#define NETWORK_BROADCAST_ADDRESS "255.255.255.255"
#define HDPLAYER_MANAGE_DISCOVER_SERVICE_PORT 35000
#define AGAIN_HDPLAYER_MANAGE_DISCOVER_SERVICE_PORT 71118

#define DISCOVER_NFSVOD_SERVER_REPLY_STRING "VOD SERVER BASE ON NFS"
#define DISCOVER_HDVOD_SERVER_REPLY_STRING "HDVOD SERVER BASE ON STREAM"
#define DISCOVER_CLIENT_REQUEST_STRING "PLAYER SCAN MBAR SERVER"

#define NETWORK_BROADCAST_ADDRESS "255.255.255.255"
#define DISCOVER_SERVICE_PORT 34555
#define AGAIN_DISCOVER_SERVICE_PORT 71117
#define DISCOVER_NETIF  "eth0"


typedef enum
{
    DISCOVER_MESSAGE_TYPE_FOUND_STATION = 100,
}DISCOVER_MESSAGE_TYPE_E;

INT32_T svr_open_discover_msg_channel( INT32_T port );
INT32_T agent_open_discover_msg_channel( VOID );
RTSTATUS close_discover_msg_channel( INT32_T sock );
INT32_T send_data_to_discover_msg_channel( INT32_T sock, INT8_T keyword, INT8_T * msgdata, INT32_T msglen, INT8_T * ipaddr, UINT16_T port );
INT32_T receive_data_from_discover_msg_channel( INT32_T sock, INT8_T * keyword, PVOID * message, INT32_T * msglen, INT8_T * ipaddr, INT32_T length, UINT16_T * port, INT32_T timeout );
INT32_T exchange_data_in_discover_msg_channel( INT32_T sock, PVOID sndmsg, INT32_T sndlen, PVOID * rcvmsg, INT32_T * rcvlen, INT8_T * ipaddr, INT32_T length, UINT16_T * port, INT32_T timeout );
INT32_T deal_data_from_discover_msg_channel( INT32_T sock, TRLV_MESSAGE_CB cb );
INT32_T deal_data_from_custom_discover_msg_channel( INT32_T sock, INT32_T fd, TRLV_MESSAGE_CB cb );

#endif//__DISCOVER_COMM_H
