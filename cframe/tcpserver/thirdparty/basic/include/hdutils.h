#ifndef __HARDDISK_UTILS_H
#define __HARDDISK_UTILS_H

#include "systype.h"

typedef enum
{
    HD_CONNECTED_METHOD_OTHER,
    HD_CONNECTED_METHOD_SATA,
    HD_CONNECTED_METHOD_USB,
}HD_CONNECTED_METHOD;

RTSTATUS get_harddisk_serialno( INT8_T * devfile, INT8_T * serialno, INT32_T length );
RTSTATUS get_harddisk_totalsize( INT8_T * devname, UINT64_T * size );
RTSTATUS get_harddisk_type_desc( INT8_T * devname, INT8_T * typedesc, INT32_T length );
RTSTATUS get_harddisk_model( INT8_T * devname, INT8_T * model, INT32_T length );
RTSTATUS get_harddisk_connected_method( INT8_T * devname, HD_CONNECTED_METHOD * method );
RTSTATUS get_harddisk_removable( INT8_T * devname, UINT32_T * removable );
RTSTATUS get_harddisk_parttion_table_schema( INT8_T * devname, INT8_T * schema, INT32_T length );
RTSTATUS get_harddisk_parttion_table_type( INT8_T * devname, INT8_T * type, INT32_T length );
RTSTATUS get_harddisk_partition_number( INT8_T * devname, UINT32_T * number );


#endif//__HARDDISK_UTILS_H
