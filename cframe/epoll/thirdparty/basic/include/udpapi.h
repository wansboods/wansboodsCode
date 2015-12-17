#ifndef __UDP_API_H
#define __UDP_API_H

#include "systype.h"

INT32_T create_udp_server_socket( UINT16_T port );
INT32_T create_udp_client_socket( VOID );
INT32_T create_udp_broadcast_client_socket( VOID );
INT32_T monitor_udp_socket( INT32_T socket, INT32_T timeout );
INT32_T send_data_to_udp_socket( INT32_T sock, PVOID data, INT32_T length, INT8_T * ipaddr, UINT16_T port );
INT32_T receive_data_from_udp_socket( INT32_T sock, PVOID data, INT32_T datalen, INT8_T * ipaddr, INT32_T iplen, UINT16_T * port, INT32_T timeout );
INT32_T close_udp_socket( INT32_T socket );
RTSTATUS join_to_mcast_group( INT32_T sock, INT8_T * mcast, INT8_T * ipaddr );

#endif//__UDP_API_H
