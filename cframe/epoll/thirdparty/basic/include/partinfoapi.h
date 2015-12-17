#ifndef __PARTITION_INFORMATION_API_H
#define __PARTITION_INFORMATION_API_H

#include "systype.h"

typedef struct
{
    INT32_T seqno;
    INT8_T * devname;
    INT8_T * devfile;
    INT8_T * uuid;
    INT8_T * fsfmt;
    UINT64_T totalsize;
    UINT64_T usedsize;
    UINT64_T freesize;
    UINT64_T availsize;
    INT32_T mnt_status;
    INT8_T * mntp;
}PARTITION_INFORMATION_T;

RTSTATUS api_get_partition_information( INT8_T * devname, PARTITION_INFORMATION_T ** info );
RTSTATUS api_update_partition_information( PARTITION_INFORMATION_T ** info );
VOID api_show_partition_information( PARTITION_INFORMATION_T * info );

#endif//__PARTITION_INFORMATION_API_H
