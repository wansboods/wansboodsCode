#ifndef __MBARMID_CODEC_H__
#define __MBARMID_CODEC_H__

#include "systype.h"


INT32_T decode_radpl_information( PVOID msgdata, INT32_T msglen, RADPL_INFO_T ** info );
INT32_T encode_submit_promo_to_buffer( INT8_T *media, INT8_T *uuid, INT32_T sid, INT32_T rid, PVOID * msgdata, INT32_T * msglen );
INT32_T encode_demand_on_film_to_buffer( INT8_T * uuid, INT32_T mid, INT32_T vid, INT32_T timeoff, INT32_T dimension, INT32_T aid, INT32_T acodecid, INT8_T * alang, INT32_T sid, INT8_T *slang, INT32_T mode, INT32_T rid, PVOID * msgdata, INT32_T * msglen );
INT32_T encode_event_demand_on_film_argument_to_buffer( INT8_T * uuid, INT32_T mid, INT32_T vid, INT32_T timeoff, INT32_T dimension, INT32_T aid, INT32_T acodecid, INT8_T * alang, INT32_T sid, INT8_T * slang, INT32_T mode, INT32_T rid, PVOID message, INT32_T length );


#endif
