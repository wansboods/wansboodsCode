#ifndef __FILE_INFORMATION_API_H
#define __FILE_INFORMATION_API_H

#include "systype.h"

typedef enum
{
    FILE_INFO_TYPE_UNKNOWN,
    FILE_INFO_TYPE_DIR,
    FILE_INFO_TYPE_REG,
    FILE_INFO_TYPE_LNK,
    FILE_INFO_TYPE_OTHER,
}FILE_INFO_TYPE_E;

typedef struct
{
    FILE_INFO_TYPE_E filetype;
    INT8_T * filename;
    INT8_T * filepath;
    UINT64_T filesize;
    INT8_T * linkedfile;
}FILE_INFORMATION_T;

RTSTATUS api_get_file_information( INT8_T * filepath, FILE_INFORMATION_T ** info );
VOID api_free_file_informaton( FILE_INFORMATION_T ** info );
VOID api_show_file_information( FILE_INFORMATION_T * info );
RTSTATUS api_get_basedir_from_filepath( INT8_T * filepath, INT8_T * basedir, INT32_T length );
INT64_T api_get_file_modify_time( INT8_T * filepath );
INT64_T api_get_file_size( INT8_T * filepath );
#endif//__FILE_INFORMATION_API_H
