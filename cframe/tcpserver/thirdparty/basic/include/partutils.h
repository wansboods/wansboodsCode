#ifndef __PARTITION_UTILS_H
#define __PARTITION_UTILS_H

#include "systype.h"

RTSTATUS get_seqno_by_hddevname_and_ptdevname( INT8_T * hddevname, INT8_T * ptdevname, INT32_T * seqno );
RTSTATUS get_uuid_and_fsfmt_by_shell_command( INT8_T * devfile, INT8_T * uuid, INT32_T uuid_len, INT8_T * fsfmt, INT32_T fsfmt_len );
RTSTATUS get_mount_status_by_devfile( INT8_T * devfile, INT32_T * status, INT8_T * mntp, INT32_T length );
RTSTATUS get_partition_totalsize( INT8_T * hddevname, INT8_T * ptdevname, UINT64_T * size );
RTSTATUS get_partition_free_space( INT8_T * mntp, UINT64_T * freesize, UINT64_T * availsize );

#endif//__PARTITION_UTILS_H
