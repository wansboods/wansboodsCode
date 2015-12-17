#ifndef __SYSTEM_OPERATE_H
#define __SYSTEM_OPERATE_H

#include "systype.h"

RTSTATUS get_process_name_by_pid( INT32_T pid, INT8_T * progname, INT32_T length );
RTSTATUS file_is_dir( INT8_T * filepath );

#endif//__SYSTEM_OPERATE_H
