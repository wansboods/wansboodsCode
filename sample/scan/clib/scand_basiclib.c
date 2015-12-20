#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "scand_basiclib.h"

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
