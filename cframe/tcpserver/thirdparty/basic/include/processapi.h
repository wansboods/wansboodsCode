#ifndef __PROCESS_API_H
#define __PROCESS_API_H

#include "systype.h"

RTSTATUS goto_daemon( INT32_T nochdir, INT32_T noclose );

RTSTATUS get_process_status( INT32_T pid, INT8_T * status );
INT32_T get_pid_by_process_name( INT8_T * process );
RTSTATUS kill_process( INT32_T pid );
RTSTATUS execute_process( INT8_T * cmdpath, INT8_T * cmdline, INT8_T * envline );
#endif//__PROCESS_API_H
