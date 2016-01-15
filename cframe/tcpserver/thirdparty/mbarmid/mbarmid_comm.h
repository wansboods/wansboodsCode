#ifndef __MBAMID_COMM_H
#define __MBAMD_COMM_H

#include "systype.h"
#include "tp_trlv.h"

#define MBARINFO_MID_HDPLAYERMANAGE_MESSAGE_PORT 30000


INT32_T mid_agent_open_hdplayermanage_msg_channel( VOID );
INT32_T mid_exchange_data_in_mbarsys_msg_channel( INT32_T sock, INT32_T msgtype, PVOID sndmsg, INT32_T sndlen, PVOID * rcvmsg, INT32_T * rcvlen, INT32_T timeout );
RTSTATUS mid_close_mbarsys_msg_channel( INT32_T sock );




#endif//__MBAMID_COMM_H

