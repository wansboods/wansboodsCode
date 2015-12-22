#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "scand_basiclib.h"

#ifdef OPENZLOG
	#define info    dzlog_info	
	#define warn    dzlog_warn
	#define fatal   dzlog_fatal
	#define error   dzlog_error
	#define debug   dzlog_debug
	#define notice  dzlog_notice
#else
	#define info   printf
	#define warn   printf
	#define error  printf
	#define fatal  printf
	#define debug  printf
	#define notice printf
#endif

void scand_safe_free( void * ptr ){
	if( ptr ){
		free( ptr );
        ptr = NULL;
    }
}

char *scand_safestrdup( const char * str ){
	if( str )
        return strdup( str );

    return NULL;
}

int scand_safe_memcmp( char * sstring, char * tstring, int length ){
    
    if( ( NULL == sstring ) || ( NULL == tstring ) ){
        warn( "传入值为nul" );
        return -1;
    }

    if( strlen( sstring ) < length ){
        return -1;
    }
    
    if( strlen( tstring ) < length ){
        return -1;
    }
    
    if( memcmp( sstring, tstring, length ) == 0 ){
        return 0;
    }

    return -1;
}

int scand_safe_strcmp(  char * sstring, char * tstring ){
    if( ( NULL == sstring ) || ( NULL == tstring ) ){
        warn( "传入值为nul" );
        return -1;
    }

	if( strcmp( sstring, tstring ) == 0 ){
		return 0;
    }

    return -1;
}

int scand_safe_casestrcmp( char * sstring, char * tstring ){
    if( ( NULL == sstring ) || ( NULL == tstring ) ){
        return -1;
    }
    
    if( strlen( sstring ) == strlen( tstring ) ){
        if( strcasecmp( sstring, tstring ) == 0 ){
            return 0;
        }
    }

    return -1;
}


int scand_valid_num_values( char * filename ){
	double d;
	char * endptr = NULL;
	d = strtod( filename, &endptr );
    if ( ( endptr != NULL && *endptr != '\0' ) ) {
        //字符串不是有效数值
        info( "字符串(%s) 不是有效数值\n", filename );
        return -1;
    }

    info( "字符串(%s)  是有效值\n", filename );
    return 0;
}
        