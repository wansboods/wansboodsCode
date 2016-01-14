#ifndef __HARDDISK_INFORMATION_API_H
#define __HARDDISK_INFORMATION_API_H

#include "hdutils.h"
#include "systype.h"

typedef struct
{
    INT8_T * devname;
    INT8_T * devfile;
    INT8_T * serialno;
    INT8_T * model;
    UINT64_T size;
    UINT32_T removable;
    HD_CONNECTED_METHOD connected_method;
    INT8_T * partition_table_schema; //use udevadm info --query=all --name=sda | grep DKD_PARTITION_TABLE_SCHEME
    UINT32_T partition_num;
}HARDDISK_INFORMATION_T;

VOID api_free_harddisk_informaton( HARDDISK_INFORMATION_T ** info );
RTSTATUS api_get_harddisk_information( INT8_T * devname, HARDDISK_INFORMATION_T ** info );
VOID api_show_harddisk_information( HARDDISK_INFORMATION_T * info );

#endif//__HARDDISK_INFORMATION_API_H
