#ifndef __RECYCLE_BIN_DATA_H
#define __RECYCLE_BIN_DATA_H

#include "systype.h"

typedef struct
{
    INT32_T sock;
    INT32_T timeout;
}RECYCLEBIN_SOCKET_INFO_T;

typedef struct
{
    PVOID next;
    RECYCLEBIN_SOCKET_INFO_T * info;
}RECYCLEBIN_SOCKET_INFO_LIST_T;

VOID delete_recyclebin_socket_information( RECYCLEBIN_SOCKET_INFO_T ** info );
RECYCLEBIN_SOCKET_INFO_T * create_recyclebin_socket_information( INT32_T sock, INT32_T timeout );
VOID delete_recyclebin_socket_information_list_node( RECYCLEBIN_SOCKET_INFO_LIST_T ** node );
VOID delete_recyclebin_socket_information_list( RECYCLEBIN_SOCKET_INFO_LIST_T ** list );
RECYCLEBIN_SOCKET_INFO_LIST_T * create_recyclebin_socket_information_list_node( RECYCLEBIN_SOCKET_INFO_T * info );
VOID add_node_into_recyclebin_socket_information_list( RECYCLEBIN_SOCKET_INFO_LIST_T ** list, RECYCLEBIN_SOCKET_INFO_LIST_T * node );

#endif//__RECYCLE_BIN_DATA_H
