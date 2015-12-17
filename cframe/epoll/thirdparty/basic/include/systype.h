#ifndef __SYS_TYPE_H
#define __SYS_TYPE_H

typedef char                         INT8_T;
typedef unsigned char               UINT8_T;
typedef short                       INT16_T;
typedef unsigned short             UINT16_T;
typedef int                         INT32_T;
typedef unsigned int               UINT32_T;
typedef long long int               INT64_T;
typedef unsigned long long int     UINT64_T;
typedef void                           VOID;
typedef void *                        PVOID;
typedef int                        RTSTATUS;

typedef float                         FLOAT;
typedef double                       DOUBLE;

#define MAX_CMDLINE_LENGTH 1024
#define MAX_FILEPATH_LENGTH 1024
#define MAX_LINE_LENGTH 1024
#define MAX_PROCESS_NAME_LENGTH 1024
#define MAX_DEVICE_NAME_LENGTH 64
#define MAX_HD_SERIALNO_LENGTH 128
#define MAX_MOVIE_CODE_LENGTH 128

#define MAX_PT_UUID_LENGTH 128
#define MAX_IPADDR_LENGTH  64
#define MAX_MACADDR_LENGTH 64

#define DEVICE_FILE_TOPDIR "/dev"
#define SYS_HD_BLOCK_TOPDIR "/sys/block"
#define LOCALHOST "127.0.0.1"
#define LOCAL_NETADDR 0

#define MAX_CATEGORY_NAME_LENGTH 64

#define MAX_INT_VALUE_LENGTH 20

#define MAX_CPUID_LENGTH 128


#define ZLOG_BASIC_PATH "/data/d2dcopy/running/config/"
#define ZLOG_MABR_CATEGROY "MABR"


#endif//__SYS_TYPE_H
