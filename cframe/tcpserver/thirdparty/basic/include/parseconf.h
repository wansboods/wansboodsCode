#ifndef __PARSE_CONF_H
#define __PARSE_CONF_H

#include "systype.h"

RTSTATUS get_int_data_from_line_by_tag( INT8_T * tag, INT8_T * line, INT32_T * data );
RTSTATUS get_ptr_data_from_line_by_tag( INT8_T * tag, INT8_T * line, INT8_T ** data );

#endif//__PARSE_CONF_H
