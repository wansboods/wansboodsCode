#ifndef __SAFE_OP_H
#define __SAFE_OP_H

#include "systype.h"

INT8_T * strip_char_space_and_return_from_string( INT8_T * string );
VOID skip_nonchar_and_nonnum_in_string( INT8_T * string );
INT8_T * strip_character_from_string( INT8_T * string, INT8_T character );
VOID safe_free( PVOID ptr );
INT8_T * safe_strdup( INT8_T * ptr );
RTSTATUS safe_strcasecmp( INT8_T * ptr1, INT8_T * ptr2 );
INT32_T safe_strcmp( INT8_T * ptr1, INT8_T * ptr2 );
INT32_T safe_memcmp( PVOID ptr1, PVOID ptr2, INT32_T length );
INT32_T safe_memcpy( PVOID ptr1, PVOID ptr2, INT32_T length );
INT32_T safe_strlen( INT8_T * ptr );
INT32_T safe_ipcmp( INT8_T * ipfmt1, INT8_T * ipfmt2 );
RTSTATUS extra_mkdir( INT8_T * dir );
VOID show_indent( INT32_T indent );

#endif//__SAFE_OP_H
