#ifndef __MESSAGE_QUEUE_API_H
#define __MESSAGE_QUEUE_API_H

#include "systype.h"

#define MAX_MSGQ_RECEIVE_BUFFER_LENGTH 4096

INT32_T get_message_queue_id( INT8_T * keypath );
INT32_T send_msgdata_to_message_queue( INT32_T msgid, INT64_T msgtype, PVOID msgdata, INT32_T msglen );
INT32_T receive_msgdata_from_message_queue( INT32_T msgid, INT64_T msgtype, PVOID * msgdata, INT32_T timeout );
INT32_T clean_message_queue( INT32_T msgid, INT64_T msgtype );
INT32_T destroy_message_queue( INT32_T msgid );

#endif//__MESSAGE_QUEUE_API_H
