#ifndef __CODEC_DATA_H
#define __CODEC_DATA_H

#include "systype.h"

INT32_T calc_codec_int_data_length( VOID );
INT32_T calc_codec_int64_data_length( VOID );
INT32_T calc_codec_ptr_data_length( INT8_T * ptrdata );

INT32_T encode_int_data_to_buffer( INT32_T intdata, INT8_T * msgdata, INT32_T msglen );
INT32_T encode_int64_data_to_buffer( INT64_T intdata, INT8_T * msgdata, INT32_T msglen );
INT32_T encode_ptr_data_to_buffer( INT8_T * ptrdata, INT8_T * msgdata, INT32_T msglen );

INT32_T decode_int_data_from_buffer( INT8_T * msgdata, INT32_T msglen, INT32_T * intdata );
INT32_T decode_int64_data_from_buffer( INT8_T * msgdata, INT32_T msglen, INT64_T * intdata );
INT32_T decode_ptr_data_from_buffer( INT8_T * msgdata, INT32_T msglen, INT8_T ** ptrdata, INT32_T * datalen );

#endif//__CODEC_DATA_H
