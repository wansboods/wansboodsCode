#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>


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

#define MAX_SCAND_DEVINFO_LENGTH 512


/*   删除左边的空格   */
char * l_trim(char * szOutput, const char *szInput){
	assert(szInput != NULL);
	assert(szOutput != NULL);
	assert(szOutput != szInput);
	for( NULL; *szInput != '\0' && isspace(*szInput); ++szInput){
		;
	}
	
	return strcpy(szOutput, szInput);
}
 
/*   删除右边的空格   */
char *r_trim(char *szOutput, const char *szInput){
	char *p = NULL;
	assert(szInput != NULL);
	assert(szOutput != NULL);
	assert(szOutput != szInput);
	strcpy(szOutput, szInput);
	for(p = szOutput + strlen(szOutput) - 1; p >= szOutput && isspace(*p); --p){
		;
	}
	
	*(++p) = '\0';
	return szOutput;
}
 
/*   删除两边的空格   */
char * a_trim(char * szOutput, const char * szInput){
	char *p = NULL;
	assert(szInput != NULL);
	assert(szOutput != NULL);
	l_trim(szOutput, szInput);
	for( p = szOutput + strlen(szOutput) - 1;p >= szOutput && isspace(*p); --p){
		;
	}
	
	*( ++p ) = '\0';
	return szOutput;
}


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


int scand_newDirectory( char * dir ){
    if( NULL == dir ){
        warn( "传入值出错!" );
        return -1;
    }

    char * topdir = dir;
    char * zonedir = dir;
    char * splitdir = NULL;    
    while( NULL != ( splitdir = strchr( zonedir, '/' ) ) ){
        char localdir[ MAX_SCAND_DEVINFO_LENGTH ] = { '\0' };
        if( ( splitdir - topdir ) ){
            
            memset( localdir, 0, MAX_SCAND_DEVINFO_LENGTH );
            memcpy( localdir, topdir, splitdir - topdir );
            info( "创建目录:%s\n", localdir );
            if( mkdir( localdir, 0777 ) < 0 ){
                if( errno != EEXIST ){
                    warn( "创建目录%s 失败.. err: %s", localdir, strerror( errno ) );
                    return -1;
                }
            }
        }
        
        zonedir = splitdir + 1;
    }

    info( "创建目录:%s\n", topdir );
    if( mkdir( topdir, 0777 ) < 0 ){
        if( errno != EEXIST ){
            warn( "创建目录%s 失败.. err: %s", topdir, strerror( errno ) );            
            return -1;
        }
    }

    return 0;
}


int scand_ExecuteCommand( char * command ){
    if( NULL == command ){
		warn( "传入的参数出错!" );
        return -1;
    }
    
    if( system( command ) < 0 ){
        
        return -1;
    }
    
    return 0;
}        

