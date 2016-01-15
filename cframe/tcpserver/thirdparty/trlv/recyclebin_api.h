#ifndef __RECYCLE_BIN_API_H
#define __RECYCLE_BIN_API_H

#include "systype.h"

RTSTATUS start_socket_resource_retrieve_task( VOID );
RTSTATUS safe_put_socket_to_recycle_bin( INT32_T socket );

#endif//__RECYCLE_BIN_API_H
