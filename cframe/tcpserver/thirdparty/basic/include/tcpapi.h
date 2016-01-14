#ifndef __TCP_API_H
#define __TCP_API_H

#include "systype.h"

INT32_T create_tcp_server_socket( UINT16_T port );
INT32_T create_tcp_client_socket( INT8_T * server, UINT16_T port, INT32_T * sock );
INT32_T create_tcp_accept_socket( INT32_T sock );
INT32_T create_tcp_accept_socket_with_args( INT32_T sock, INT8_T * ipaddr, INT32_T length, INT32_T * port );

INT32_T receive_data_from_tcp_socket( INT32_T sock, PVOID data, INT32_T length, INT32_T timeout );
INT32_T send_data_to_tcp_socket( INT32_T sock, PVOID data, INT32_T length );
INT32_T close_tcp_socket( INT32_T socket );

#endif//__TCP_API_H
