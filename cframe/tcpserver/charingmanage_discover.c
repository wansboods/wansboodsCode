#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>

#include "zlog.h"
#include "safeop.h"
#include "charing_trlv.h"
#include "charingmanage_discover.h"
#include "charingmanage_message.h"

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

void check_charing_handler( void );
void safe_heart_node( char * hostname );

int deal_discover_scan_message( int sock, char * ipaddr, int msgtype, void * rcvmsgdata, int rcvmsglen, void ** replymsgdata, int * replymsglen );
void delete_node_from_heart_information_list_by_fd( HEART_INFO_T ** list, char * hostname );
int deal_data_from_charing_discover_msg_channel( int sock, int fd, CHARINGMANAGE_MESSAGE_DEAL_CB cb );

//int fdsock = -1;
#define CHARING_MANAGE_DISCOVER_SERVICE_PORT 7003
#define AGAIN_CHARING_MANAGE_DISCOVER_SERVICE_PORT 7004
#define DISCOVER_MESSAGE_TYPE_FOUND_STATION  1000

#define DISCOVER_CHARING_REQUEST_STRING "where is charging server"
#define DISCOVER_CHARING_SERVER_REPLY_STRING  "I'm charging server" 

void * deal_charing_manage_heart_event( void * args );

static HEART_INFO_T * s_heart_information_list = NULL;    
static pthread_mutex_t s_mutex = PTHREAD_MUTEX_INITIALIZER;
int lock( void ){
    return pthread_mutex_lock( &s_mutex );
}

int unlock( void ){
    return pthread_mutex_unlock( &s_mutex );
}

INT32_T fdsock = -1;
static void * deal_charing_manage_discover_event( void * args ){
	pthread_detach( pthread_self() );
    info( "DISCOVER 守护进程开启通讯通道, 开始监听DISCOVER 消息" );   
    int sock;
    if( ( sock = charing_open_discover_msg_channel( CHARING_MANAGE_DISCOVER_SERVICE_PORT ) ) < 0 ){
        printf( "创建DISCOVER守护进程的接收SOCKET失败. 错误码:%d\n", sock );
        return NULL;
    }
        
    fdsock = -1;
    fdsock = charing_open_custom_msg_channel( AGAIN_CHARING_MANAGE_DISCOVER_SERVICE_PORT, "eth0" );
    if( fdsock <= 0 ){
		warn( "failed to charing_open_custom_msg_channel" );
        return -1;
    }

    return -1;


    while( 1 ){
        deal_data_from_charing_discover_msg_channel( sock, fdsock, deal_discover_scan_message );
    }

    charing_close_mbarsys_msg_channel( sock );    
}

#define DISCOVER_KEYWORD 0xab
int deal_data_from_charing_discover_msg_channel( int sock, int fd, CHARINGMANAGE_MESSAGE_DEAL_CB cb  ){
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
        warn( "无效的DISCOVER 回应消息, 关键字0x%02x有误", keyword & 0xff );
        safe_free( rcvmsgdata );
        return -1;
    }

    info( "接收来自于%s(%d)的DISCOVER消息,消息体长度(%d)\n", ipaddr, port, rcvmsglen );
    void * replymsgdata = NULL;
    int replymsglen = 0;
    retcode = cb( 0, NULL, DISCOVER_MESSAGE_TYPE_FOUND_STATION, rcvmsgdata, rcvmsglen, &replymsgdata, &replymsglen );
    if( retcode ){
        warn( "处理消息失败,不回应对端. 错误码: %d\n", retcode  );
        return retcode;
    }
    
    if( fd < 0 ){    
        retcode = send_data_to_discover_msg_channel( sock, keyword, replymsgdata, replymsglen, ipaddr, port );
    }else{
        retcode = send_data_to_discover_msg_channel( fd, keyword, replymsgdata, replymsglen, ipaddr, port );
    }

    info( "回应消息到%s(%d), 消息体长度(%d)", ipaddr, port, replymsglen );    
    safe_free( rcvmsgdata );
    safe_free( replymsgdata );    
    if( retcode < 0 )
        return retcode;
    return retcode;
}

int deal_discover_scan_message( int sock, char * ipaddr, int msgtype, void * rcvmsgdata, int rcvmsglen, void ** replymsgdata, int * replymsglen ){
    int retcode = -1;
    char * replyinfo = DISCOVER_CHARING_SERVER_REPLY_STRING;
    switch( msgtype ){
        case DISCOVER_MESSAGE_TYPE_FOUND_STATION:
            info( "show string: %s(%d)\n", DISCOVER_CHARING_REQUEST_STRING, rcvmsglen );
            if( rcvmsglen == strlen( DISCOVER_CHARING_REQUEST_STRING ) + 1 ){
                if( safe_memcmp( rcvmsgdata, DISCOVER_CHARING_REQUEST_STRING, rcvmsglen ) == 0 ){
                    char * replymsg = NULL;
                    if( NULL == ( replymsg = safe_strdup( replyinfo ) ) ){
                        printf( "复制应答字符串(%s)出错.错误原因: %s\n", replyinfo, strerror( errno ) );
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
        info( "启动心跳包出错!" );
        return -1;
    }
    
    return 0;
}

void * deal_charing_manage_heart_event( void * args ){
	info( "心跳检测线程已开启" );
	while( 1 ){	
		check_charing_handler(); //心跳检测处理函数		
		sleep(10); //定时10秒
	}
	
	return NULL;
}


void check_charing_handler( void ){
    if( NULL == s_heart_information_list ){
        warn( "未检测到注册盒子列表" );
        return;
    }

    lock();
    //info( "心跳查询:" );
    //info( "========================================================" );
    HEART_INFO_T *ppNode = s_heart_information_list;
	while( ppNode ){

	    if( ppNode->count == 6 ){
	        
			info( "心跳已6次，客户端IP(%s),用户(%s) 已经掉线!", ppNode->ipaddr, ppNode->hostname );
            //safe_message_node( ppNode->ipaddr );
            info( "完成连接链表卸载(%s)!", ppNode->hostname );            
            safe_heart_node( ppNode->hostname );
            info( "完成心跳包连接卸载(%s)!", ppNode->hostname );
		}else if( ppNode->count > 0 ){		
            //info("客户端IP(%s),用户(%s) 出现异常!", ppNode->peerip,ppNode->name );
		    ppNode->count ++;
		    //info("count(%d)", ppNode->count);//查看计数器内容
	    }else if( ppNode->count == 0 ){
		    ppNode->count++;
		    //info("count(%d)", ppNode->count);//查看计数器内容
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
        info( "hostname(%s)  保存值过大(%ld)", node->hostname, strlen( hostname ) );
    }

    if( strlen( ipaddr ) + 1 < sizeof( node->ipaddr ) ){
        memcpy( node->ipaddr, ipaddr , strlen( ipaddr ) + 1 );
    }else{
        info( "ipaddr(%s)  保存值过大(%ld)", ipaddr, strlen( ipaddr ) );
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
    if( ( NULL == node ) || ( NULL == hostname ) || ( NULL == ipaddr ) )
        return -1;

    HEART_INFO_T * pinfo = node;
    if( pinfo ){    
        if( safe_strcmp( pinfo->hostname, hostname ) == 0 ){
            if( strlen( hostname ) + 1 < sizeof( pinfo->hostname ) ){
                memcpy( pinfo->hostname, hostname , strlen( hostname ) + 1 );        
            }else{
                info( "uuid 保存值过大(%ld)", strlen( hostname ) );
            }
            
            if( strlen( ipaddr ) + 1 < sizeof( pinfo->ipaddr ) ){            
                memcpy( pinfo->ipaddr, ipaddr , strlen( ipaddr ) + 1 );
            }else{
                info( "ipaddr 保存值过大(%ld)", strlen( ipaddr ) );
            }
            
            node->sockfd = fd;
            node->count = 0;
            return 0;
        }
    }

    return -1;
}

int registration_hostname( int sock, char * hostname, char * ipaddr ){

    int retcode = 0;
    info( " 正在注册计费程序,主机名(%s) 信息", hostname );
    lock();
    info( " 登记主机名(%s)  信息", hostname );    
    info( " 获取主机名( %s ) IP 地址为( %s )", hostname, ipaddr );
    info( " 主机名( %s )  注册心跳包信息", hostname );
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
    info( "恭喜! 主机名( %s ) 注册已完毕!", hostname );  
    return retcode;
}

void safe_heart_node( char * hostname  ){
    delete_node_from_heart_information_list_by_fd( &s_heart_information_list, hostname );    
}

void delete_node_from_heart_information_list_by_fd( HEART_INFO_T ** list, char * hostname ){
    if( NULL == list ) {
        return;
    }
    
    HEART_INFO_T * listnode = *list;
    HEART_INFO_T * prevnode = NULL;
    while( listnode ){
        
        HEART_INFO_T * nextnode = listnode->next;
        if( safe_strcmp( listnode->hostname, hostname ) == 0 ){
            if( NULL == prevnode ){
                *list = nextnode;
            }
            else{
                prevnode->next = nextnode;
            }
            
            free( listnode );            
            return;
        }
		
        prevnode = listnode;
        listnode = nextnode;
    }    
}
