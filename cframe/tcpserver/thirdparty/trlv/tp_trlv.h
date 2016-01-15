#ifndef __TP_TRLV_H
#define __TP_TRLV_H

#include "systype.h"

typedef enum
{
    TRLV_TP_ERRCODE_OK                      = 0,
    TRLV_TP_ERRCODE_ERROR                   = 1,
    TRLV_TP_ERRCODE_INVALID_ARGS            = 2,
    TRLV_TP_ERRCODE_MALLOC_FAIL             = 3,
    TRLV_TP_ERRCODE_RECEIVE_MSGHEAD_FAIL    = 4,
    TRLV_TP_ERRCODE_RECEIVE_MSGBODY_FAIL    = 5,
    TRLV_TP_ERRCODE_SEND_MSGHEAD_FAIL       = 6,
    TRLV_TP_ERRCODE_SEND_MSGBODY_FAIL       = 7,
    TRLV_TP_ERRCODE_CHECK_TYPE_MISMATCH     = 8,
    TRLV_TP_ERRCODE_CHECK_RESULT_ERROR      = 9,
    TRLV_TP_ERRCODE_RECEIVE_MESSAGE_FAIL    = 10,
    TRLV_TP_ERRCODE_SEND_MESSAGE_FAIL       = 11,
    TRLV_TP_ERRCODE_RECEIVE_INVALID_MESSAGE = 12,
}TRLV_TP_ERRCODE_E;

typedef struct
{
    INT32_T type;
    INT32_T result;
    INT32_T length;
}TRLV_MESSAGE_HEAD;

typedef RTSTATUS ( * TRLV_MESSAGE_CB )( INT32_T msgtype, PVOID rcvmsgdata, INT32_T rcvmsglen, PVOID * replymsgdata, INT32_T * replymsglen );

#endif//__TP_TRLV_H
