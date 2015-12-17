#ifndef __NET_IF_API_H
#define __NET_IF_API_H

#define MAX_IPADDR_LENGTH 64
#define MAX_NETIF_NAME_LENGTH 64

#include "systype.h"

RTSTATUS get_netif_address( INT8_T * netif, INT8_T * ipaddr, INT32_T length );
RTSTATUS set_netif_address( INT8_T * netif, INT8_T * ipaddr );
RTSTATUS get_netif_netmask( INT8_T * netif, INT8_T * netmask, INT32_T length );
RTSTATUS set_netif_netmask( INT8_T * netif, INT8_T * netmask );
RTSTATUS get_netif_broadcast( INT8_T * netif, INT8_T * broadcast, INT32_T length );
RTSTATUS get_netif_mac( INT8_T * netif, INT8_T * mac, INT32_T length );
RTSTATUS get_netif_status( INT8_T * netif, UINT32_T * status );
RTSTATUS get_netif_link_status( INT8_T * netif, INT32_T * status );
RTSTATUS get_netif_ipzone_information( INT8_T * netif, INT8_T * ipzone, INT32_T length, UINT32_T * maskbit, UINT32_T * range );

RTSTATUS get_default_gateway( INT8_T * gatewayip, INT32_T size );
RTSTATUS delete_default_gateway( VOID );
RTSTATUS delete_gateway( INT8_T * gateway );
RTSTATUS set_default_gateway( INT8_T * netif, INT8_T * gateway );

RTSTATUS get_dns( INT8_T * dns1, INT32_T len1, INT8_T * dns2, INT32_T len2 );
RTSTATUS set_dns( INT8_T * dns1, INT8_T * dns2 );

#endif//__NET_IF_API_H
