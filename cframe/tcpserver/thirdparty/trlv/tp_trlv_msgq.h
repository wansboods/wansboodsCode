#ifndef __TP_TRLV_MSGQ_H
#define __TP_TRLV_MSGQ_H

#include "systype.h"
#include "tp_trlv.h"

INT32_T receive_trlv_data_from_msgq_communication_channel( INT32_T msgid, INT64_T msgtype, TRLV_MESSAGE_HEAD * head, INT32_T headlen, PVOID * message, INT32_T timeout );
INT32_T send_trlv_data_to_msgq_communication_channel( INT32_T msgid, INT64_T msgtype, TRLV_MESSAGE_HEAD * head, INT32_T headlen, PVOID message );
RTSTATUS exchange_trlv_data_using_msgq_communication_channel( INT32_T sndmsgid, INT64_T sndmsgtype, TRLV_MESSAGE_HEAD * head, INT32_T headlen, PVOID sndmsgdata, INT32_T rcvmsgid, INT64_T rcvmsgtype, PVOID * rcvmsgdata, INT32_T * rcvlen, INT32_T timeout );

#endif//__TP_TRLV_MSGQ_H
