#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>

#include "tread_config.h"

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

void free_config_info_list( READCFN_CONFIG_LIST ** list );
READCFN_CONFIG_LIST * create_config_info_node( void );
void add_node_into_config_info_list( READCFN_CONFIG_LIST ** list, READCFN_CONFIG_LIST * node );




int cfn_read_config( char * path, READCFN_CONFIG_LIST ** glist ){
    if( NULL == path ){
        warn( "传入值出错" );
        return -1;
    }
    
    if( access( path, R_OK ) < 0 ){

        warn( "文件不存在(%s)", path );
        return -1;
    }
    
    FILE * fp = NULL;
	char readline[ 1024 ] = {0};
	if( NULL == ( fp = fopen( path, "r" ) ) ){        
		warn( "failed to open file %s. err: %s", path, strerror( errno ) );
		return -1;
	}
	
	READCFN_CONFIG_LIST * list = NULL;
    char * pline = NULL;
    char * pstart = NULL;
	while( fgets( readline, sizeof( readline ), fp ) ){
        
        if( strstr( readline, "#" ) != NULL ){
            continue;
        }

	    char l_value[ 512 ] = { '\0' };
	    char r_value[ 512 ] = { '\0' }; 
		char buf[ 512 ] = { '\0' };
        pstart = readline;
        if( ( pline = strrchr( readline, '=' ) ) ){
			
			READCFN_CONFIG_LIST * node = create_config_info_node();
			
			printf( "----------\n" );
            memcpy( l_value, pstart, pline - pstart );
			a_trim( buf, l_value );
            printf( "左边值:%s\n", buf );                
            node->l_value = strdup( buf );
			pline ++;
            memcpy( r_value, pline, strlen( pline ) + 1 );
            a_trim( buf, r_value );
			printf( "右边值:%s\n", buf );
			
			
			node->r_value = strdup( buf );
			
			add_node_into_config_info_list( &list, node );
			printf( "----------\n\n" );
		}

	}

	fclose( fp );
    *glist = list;
	return 0;
}



void free_config_info_list( READCFN_CONFIG_LIST ** list ){
    if( NULL == list ){
        return;
    }

    if( NULL == *list ){
        return;
    }

    READCFN_CONFIG_LIST * stepnode = *list;
    while( stepnode ){
        READCFN_CONFIG_LIST * nextnode = stepnode->next;
		config_safe_free( stepnode->l_value );
		config_safe_free( stepnode->r_value );		
        free( stepnode );
        stepnode = nextnode;
    }
    
    *list = NULL;   
    return;
}

READCFN_CONFIG_LIST * create_config_info_node( void ){  
    READCFN_CONFIG_LIST * node = ( READCFN_CONFIG_LIST * )calloc( sizeof( READCFN_CONFIG_LIST ), 1 );
    if( NULL == node ){
		warn( "创建node 值为nul" );
        return NULL;        
	}

    //node->next = NULL;
    return node;
}

void config_safe_free( char * ptr ){
	if( ptr ){
		free( ptr );
        ptr = NULL;
    }
}


void add_node_into_config_info_list( READCFN_CONFIG_LIST ** list, READCFN_CONFIG_LIST * node ){
    if( NULL == list ){
        return;
    }
    
    if( NULL == *list ){
        *list = node;
        return;
    }
    
    READCFN_CONFIG_LIST * stepnode = *list;
    while( stepnode ){
        if( !stepnode->next ){
            stepnode->next = node;
            return;
        }
        
        stepnode = stepnode->next;
    }
}

