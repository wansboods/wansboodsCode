#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "zlog.h"
#include "tp_trlv_udp.h"
#include "discover_comm.h"
#include "charingmanage_discover.h"

#ifdef OPENZLOG
	#define info    dzlog_info	
	#define warn    dzlog_warn
	#define fatal   dzlog_fatal
	#define error   dzlog_error
	#define debug   dzlog_debug
	#define notice  dzlog_notice
#else
	#define info  printf
	#define warn printf 
    #define error  printf
	#define fatal  printf
	#define debug  printf
	#define notice printf
#endif


int deal_data_from_charing_discover_msg_channel( int sock, int fd, TRLV_MESSAGE_CB cb  );


//int fdsock = -1;
#define CHARING_MANAGE_DISCOVER_SERVICE_PORT 2500
#define AGAIN_CHARING_MANAGE_DISCOVER_SERVICE_PORT 71119
#define DISCOVER_MESSAGE_TYPE_FOUND_STATION  1000

#define DISCOVER_CHARING_REQUEST_STRING "where is charging server"
#define DISCOVER_CHARING_SERVER_REPLY_STRING  "I'm charging server" 

static HEART_INFO_T * s_heart_information_list = NULL;


static void * deal_charing_manage_discover_event( void * args ){
	pthread_detach( pthread_self() );
    dzlog_info( "DISCOVER �ػ����̿���ͨѶͨ��, ��ʼ����DISCOVER ��Ϣ" );
    int sock;
    if( ( sock = svr_open_discover_msg_channel( CHARING_MANAGE_DISCOVER_SERVICE_PORT ) ) < 0 ){
        printf( "����DISCOVER�ػ����̵Ľ���SOCKETʧ��. ������:%d\n", sock );
        return NULL;
    }

    int fdsock = -1;
    fdsock = svr_open_custom_msg_channel( AGAIN_CHARING_MANAGE_DISCOVER_SERVICE_PORT, "eth0" );
    while( 1 ){
        deal_data_from_charing_discover_msg_channel( sock, fdsock, deal_discover_scan_message );
    }

    close_discover_msg_channel( sock );    
}

#define DISCOVER_KEYWORD 0xab
int deal_data_from_charing_discover_msg_channel( int sock, int fd, TRLV_MESSAGE_CB cb  ){
    if( ( NULL == cb ) || ( sock < 0 ) )
        return -1;

    int retcode = 0;
    int rcvmsglen = 0;
    unsigned short port = 0;
    
    char keyword;
    char ipaddr[ 64 ] = { '\0' };
    void * rcvmsgdata = NULL;
    char * replyinfo = NULL;
    
    
    retcode = receive_data_from_discover_msg_channel( sock, &keyword, &rcvmsgdata, &rcvmsglen, ipaddr, sizeof( ipaddr ), &port, 0 );
    if( retcode < 0 )
        return retcode;
    if( ( keyword & 0xff ) != DISCOVER_KEYWORD ){
        warn( "0x%x VS 0x%x", keyword, DISCOVER_KEYWORD );
        warn( "��Ч��DISCOVER ��Ӧ��Ϣ, �ؼ���0x%02x����", keyword & 0xff );
        safe_free( rcvmsgdata );
        return -1;
    }

    info( "����������%s(%d)��DISCOVER��Ϣ,��Ϣ�峤��(%d)\n", ipaddr, port, rcvmsglen );
    void * replymsgdata = NULL;
    int replymsglen = 0;
    retcode = cb( DISCOVER_MESSAGE_TYPE_FOUND_STATION, rcvmsgdata, rcvmsglen, &replymsgdata, &replymsglen );
    if( retcode ){
        warn( "������Ϣʧ��,����Ӧ�Զ�. ������: %d\n", retcode  );
        return retcode;
    }
    
    if( fd < 0 ){    
        retcode = send_data_to_discover_msg_channel( sock, keyword, replymsgdata, replymsglen, ipaddr, port );
    }else{
        retcode = send_data_to_discover_msg_channel( fd, keyword, replymsgdata, replymsglen, ipaddr, port );
    }

    info( "��Ӧ��Ϣ��%s(%d), ��Ϣ�峤��(%d)", ipaddr, port, replymsglen );    
    safe_free( rcvmsgdata );
    safe_free( replymsgdata );    
    if( retcode < 0 )
        return retcode;
    return retcode;
}

int deal_discover_scan_message( int msgtype, void * rcvmsgdata, int rcvmsglen, void ** replymsgdata, int * replymsglen ){
    int retcode = -1;
    char * replyinfo = DISCOVER_CHARING_SERVER_REPLY_STRING;
    switch( msgtype ){
        case DISCOVER_MESSAGE_TYPE_FOUND_STATION:
            info( "show string: %s(%d)\n", DISCOVER_CHARING_REQUEST_STRING, rcvmsglen );
            if( rcvmsglen == strlen( DISCOVER_CHARING_REQUEST_STRING ) + 1 ){
                if( safe_memcmp( rcvmsgdata, DISCOVER_CHARING_REQUEST_STRING, rcvmsglen ) == 0 ){
                    char * replymsg = NULL;
                    if( NULL == ( replymsg = safe_strdup( replyinfo ) ) ){
                        printf( "����Ӧ���ַ���(%s)����.����ԭ��: %s\n", replyinfo, strerror( errno ) );
                    }else{
                        *replymsgdata = replymsg;
                        *replymsglen = strlen( replyinfo );
                        retcode = 0;
                    }
                }
            }
            
            break;
        default:
            break;
    }

    return retcode;
}


int start_charing_manage_discover_monitor_task(){

    pthread_t pid;
    if( pthread_create( &pid, NULL, deal_charing_manage_discover_event, NULL ) < 0 ){
        return -1;
    }
    
    return 0;
}



int start_charing_heart( void ){
    pthread_t pid;
    if( pthread_create( &pid, NULL, deal_charing_manage_heart_event, NULL ) < 0 ){
        dzlog_warn( "��������������!" );
        return -1;
    }
    
    return 0;
}



void * deal_charing_manage_heart_event( void * args ){
	info( "��������߳��ѿ���" );
	while(1){	
		check_charing_handler(); //������⴦����		
		sleep(10); //��ʱ10��
	}
	
	return NULL;
}


void check_charing_handler( void ){
    if( NULL == s_heart_information_list ){
        warn( "δ��⵽ע������б�" );
        return;
    }

    lock();
    //dzlog_debug( "������ѯ:" );
    //dzlog_debug( "========================================================" );
    HEART_INFO_T *ppNode = s_heart_information_list;
	while( ppNode ){

	    if( ppNode->count == 6 ){
	        
			dzlog_warn( "������6�Σ��ͻ���IP(%s),�û�(%s) �Ѿ�����!", ppNode->peerip, ppNode->name );
            safe_message_node( ppNode->name );
            dzlog_debug( "�����������ж��(%s)!", ppNode->name );            
            safe_heart_node( ppNode->name );
            dzlog_debug( "�������������ж��(%s)!", ppNode->name );
		}else if( ppNode->count > 0 ){		
            //dzlog_warn("�ͻ���IP(%s),�û�(%s) �����쳣!", ppNode->peerip,ppNode->name );
		    ppNode->count ++;
		    //dzlog_debug("count(%d)", ppNode->count);//�鿴����������
	    }else if( ppNode->count == 0 ){
		    ppNode->count++;
		    //dzlog_debug("count(%d)", ppNode->count);//�鿴����������
	    }
	    
	    ppNode = ppNode->next;
    } 

    unlock();

}

HEART_INFO_T * found_node_from_heart_information_list_by_uuid( HEART_INFO_T * list, char * hostname ){

    if( ( NULL == list ) || ( NULL == hostname ) ){
        return NULL;
    }

    HEART_INFO_T * listnode = list;
    while( listnode ){        
        if( safe_strcmp( listnode->hostname, hostname ) == 0 ){
            return listnode;
        }
        
        listnode = listnode->next;
    }

    return NULL;
}

HEART_INFO_T * create_heart_information_list_node_ext( int fd, char * hostname, char * ipaddr ){
    HEART_INFO_T * node;
    if( NULL == ( node = ( HEART_INFO_T * )calloc( 1, sizeof( HEART_INFO_T ) ) ) ){
        return NULL;
    }

    if( strlen( hostname ) + 1 < sizeof( node->hostname ) ){
        memcpy( node->hostname, hostname , strlen( hostname ) + 1 );        
    }else{
        info( "hostname(%s)  ����ֵ����(%d)", node->hostname, strlen( hostname ) );
    }

    if( strlen( ipaddr ) + 1 < sizeof( node->peerip ) ){
        memcpy( node->peerip, ipaddr , strlen( ipaddr ) + 1 );
    }else{
        dzlog_warn( "ipaddr(%s)  ����ֵ����   (%d)", ipaddr, strlen( ipaddr ) );
    }

    node->sockfd = fd;
    node->count = 0;    
    return node;
}

void add_node_into_heart_information_list( HEART_INFO_T ** list, HEART_INFO_T * node ){
    if( NULL == list ){
        return;
    }

    if( NULL == *list ){
        
        *list = node;
        return;
    }
    
    HEART_INFO_T * listnode = *list;
    while( listnode ){
        
        if( NULL == listnode->next ){
            
            listnode->next = node;
            break;
        }
        
        listnode = listnode->next;
    }    
}

int update_register_information_in_heart_information_node( HEART_INFO_T * node, int fd, char * hostname, char * ipaddr ){
    if( ( NULL == node ) || ( NULL == uuid ) || ( NULL == ipaddr ) )
        return -1;

    HEART_INFO_T * info = node;
    if( info ){    
        if( safe_strcmp( info->hostname, hostname ) == 0 ){
            if( strlen( hostname ) + 1 < sizeof( info->hostname ) ){
                memcpy( info->hostname, hostname , strlen( hostname ) + 1 );        
            }else{
                dzlog_warn( "uuid ����ֵ����(%ld)", strlen( uuid ) );
            }
            
            if( strlen( ipaddr ) + 1 < sizeof( info->peerip ) ){            
                memcpy( info->peerip, ipaddr , strlen( ipaddr ) + 1 );
            }else{
                dzlog_warn( "ipaddr ����ֵ����(%ld)", strlen( ipaddr ) );
            }
            
            node->sockfd = fd;
            node->count = 0;
            return 0;
        }
    }

    return -1;
}



int registration_hostname( int sock, char * hostname, char * ipaddr ){

    info( " ����ע��Ʒѳ���,������(%s) ��Ϣ", hostname );
    lock();
    info( " �Ǽ�������(%s)  ��Ϣ", hostname );    
    info( " ��ȡ������( %s ) IP ��ַΪ( %s )", hostname, ipaddr );
    info( " ������( %s )  ע����������Ϣ", hostname );
    HEART_INFO_T  *heart_node = NULL;
    if( NULL == ( heart_node = found_node_from_heart_information_list_by_uuid( s_heart_information_list, hostname ) ) ){
        if( NULL == ( heart_node = create_heart_information_list_node_ext( sock, hostname, ipaddr ) ) ){
            retcode = -1;            
        }else{
            add_node_into_heart_information_list( &s_heart_information_list, heart_node );
        }
    }else{
        retcode = update_register_information_in_heart_information_node( heart_node, sock, hostname, ipaddr );
    }

    //manually_player_status( uuid );
    unlock();
    info( "��ϲ! ������( %s ) ע�������!", hostname );  
    return 0;
}