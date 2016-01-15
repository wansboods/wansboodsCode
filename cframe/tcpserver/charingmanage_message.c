#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "zlog.h"
#include "tcpapi.h"
#include "safeop.h"
#include "cJSON.h"
#include "xmlstringparse.h"
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
	#define info printf
	#define warn printf 
    #define error  printf
	#define fatal  printf
	#define debug  printf
	#define notice printf
#endif

#define CHARING_MESSAGE_PORT  7001
#define CHARING_MESSAGE_PORT2 7002
#define MAX_CHARING_MSG_RECEIVE_TIMEOUT 2

int play_controls( char * rcvmsg, int rcvmsglen,char ** replymsg,int * replymsglen );
int do_device_register_process( int sock, int msgtype, char * ipaddr, char * rcvmsg, int rcvmsglen,char ** replymsg,int * replymsglen );
int get_play_status( char * rcvmsg, int rcvmsglen, char ** replymsg,int * replymsglen );
int get_hostname_from_xmldata( char *msgdata, char *hostname, int hnlen );
int deal_data_from_charing_msg_channel( int sock, char * ipaddr, CHARINGMANAGE_MESSAGE_DEAL_CB cb );
int send_data_to_charing( int sock, void *message, int *sedsize );
int receive_data_from_charing( int sock, CHARING_TRLV_MESSAGE_HEAD * head, int hlen, void ** message, int *revsize );



int deal_data_from_hdplayermange_msg_channel( int sock, char * ipaddr, CHARINGMANAGE_MESSAGE_DEAL_CB cb ){
    if( ( NULL == cb ) || ( sock < 0 ) )
        return -1;
    
    int retcode = 0;
    CHARING_TRLV_MESSAGE_HEAD rcvmsghead;
    void * rcvmsgdata = NULL;
    retcode = receive_charing_data_from_tcp_communication_channel( sock, &rcvmsghead, sizeof( rcvmsghead ), &rcvmsgdata, MAX_CHARING_MSG_RECEIVE_TIMEOUT );
    if( retcode < 0 ){
        warn( "failed to receive_charing_data_from_tcp_communication_channel" );        
        close_tcp_socket( sock );
        return retcode;
    }
    
    void * replymsgdata = NULL;
    int replymsglen = 0;
    retcode = cb( sock, ipaddr, rcvmsghead.type, rcvmsgdata, rcvmsghead.length, &replymsgdata, &replymsglen );
    CHARING_TRLV_MESSAGE_HEAD replymsghead;
    replymsghead.type = rcvmsghead.type;
    replymsghead.result = retcode;
    replymsghead.length = replymsglen;    
    retcode = send_trlv_data_to_tcp_communication_channel( sock, &replymsghead, sizeof( replymsghead ), replymsgdata, replymsglen );

    safe_free( rcvmsgdata );
    safe_free( replymsgdata );
    return retcode;
}

int deal_data_from_charing_msg_channel( int sock, char * ipaddr, CHARINGMANAGE_MESSAGE_DEAL_CB cb ){
    if( ( NULL == cb ) || ( sock < 0 ) )
        return -1;

    int retcode = 0;
    int rcvlength = 0;
    CHARING_TRLV_MESSAGE_HEAD rcvmsghead;
    memset( &rcvmsghead, 0, sizeof( CHARING_TRLV_MESSAGE_HEAD ) );
    
    void * rcvmsgdata = NULL;
    retcode = receive_data_from_charing( sock, &rcvmsghead, sizeof( rcvmsghead ), &rcvmsgdata, &rcvlength );
    if( retcode < 0 ){
        warn( "failed to receive_data_from_charing" );        
        close_tcp_socket( sock );
        return retcode;
    }
    
    void * replymsgdata = NULL;
    int replymsglen = 0;
    retcode = cb( sock, ipaddr, rcvmsghead.type, rcvmsgdata, rcvmsghead.length, &replymsgdata, &replymsglen );
	retcode = send_data_to_charing( sock, replymsgdata, replymsglen );
    
    safe_free( rcvmsgdata );
    safe_free( replymsgdata );
    return retcode;
}

int deal_hdplayermange_message_event( int sock, char *ipaddr, int msgtype, void * rcvmsg, int rcvmsglen, void ** replymsg, int * replymsglen ){
    int retcode = 0;
    info( "HDPLAYERMANAGE MESSAGE 事件值%d", msgtype );    
    switch( msgtype ){
        case CHARINGMANAGE_MSGTYPE_PLAY_CHANGE_STATUS:
            retcode = play_change_status( rcvmsg, rcvmsglen, replymsg, replymsglen );
            break;
        case CHARINGMANAGE_MSGTYPE_VOL_CHANGE_STATUS:
            retcode = vol_change_status( rcvmsg, rcvmsglen, replymsg, replymsglen );
			break;
        case CHARINGMANAGE_MSGTYPE_CHANGE_MUTE:
            retcode = mute_change_status( rcvmsg, rcvmsglen, replymsg, replymsglen );
            break;
        default:
            return -1;
    }
    
    return retcode;
}

int deal_charing_message_event( int sock, char *ipaddr, int msgtype, void * rcvmsg, int rcvmsglen, void ** replymsg, int * replymsglen ){
    int retcode = 0;
    info( "CHARING MESSAGE 事件值%d", msgtype );    
    switch( msgtype ){
        case CHARINGMANAGE_MSGTYPE_PLAY_CONTROLS:
            retcode = play_controls( rcvmsg, rcvmsglen, replymsg, replymsglen );
            break;
        case CHARINGMANAGE_MSGTYPE_REGISTER_DEVICE:
            retcode = do_device_register_process( sock, msgtype, ipaddr, rcvmsg, rcvmsglen, replymsg, replymsglen );
            break;
        case CHARINGMANAGE_MSGTYPE_GET_PLAY_STATUS:
            retcode = get_play_status( rcvmsg, rcvmsglen, replymsg, replymsglen );
            break;                      
        default:
            return -1;
    }

    return retcode;
}

int get_headmsg_from_message( char * msghead, int type, int result, int length ){
	if( NULL == msghead ){
		warn( "传入出错!" );
        return -1;
    }
    
    sprintf( msghead,"%04d|%04d|%04d|", type, result, length );    
    return 0;
}

int get_headbuf_from_message( char *headbuf, CHARING_TRLV_MESSAGE_HEAD * head, int hlen ){
	if( hlen < sizeof( int ) ){
		warn( "传入值出错!" );
		return -1;
    }
    
    char type[ 64 ]  = { '\0' };
    char result[ 64 ] = { '\0' };
    char length[ 64 ] = { '\0' };
    if( sscanf( headbuf, "%[^|]|%[^|]|%[^|]|%*s", type, result, length ) != 3 ){
        warn( "解析头部文件出错!" );
        return -1;
    }
    
    sscanf( type,   "%04d", &head->type );
    sscanf( result, "%04d", &head->result );
    sscanf( length, "%04d", &head->length );	
    return 0;
}

int send_data_to_charing( int sock, void *message, int *sedsize ){
    if( NULL == message || sock <= 0 ){
		warn( "传入值出错!" );
        return -1;
    }

    int sedhlen = 0;
    int totalen = sedhlen;
    int sedbodylen = 0;
    if( sedsize && message ){
        sedbodylen = send_data_to_tcp_socket( sock, message, sedsize );
        if( sedbodylen < 0 ){
        	warn( "(%d) socket 发送字节(%d) 内容失败!", sedhlen, sedsize );            
            return -1;
        }
        
        totalen += sedbodylen;
    }
    
    if( sedsize )
		*sedsize = totalen;
    return 0;
}


int receive_data_from_charing( int sock, CHARING_TRLV_MESSAGE_HEAD * head, int hlen, void ** message, int *revsize  ){
    if( NULL == head || sock <= 0 ){
		warn( "传入值出错!\n" );
        return -1;
    }

    int revhlen = 0;
    int retcode = 0;
	int totalen = 0;
    char headbuf[ 512 ] = { '\0' };
    info( "接收数据\n" );    
    revhlen = receive_data_from_tcp_socket( sock, headbuf, sizeof( headbuf ), MAX_CHARING_MSG_RECEIVE_TIMEOUT );
    if( revhlen < 0 ){
        warn( "(%d) socket 接收字节(%ld) 内容失败!", revhlen, strlen( headbuf ) );
        return -1;
    }
    
    info( "获取网站数据:%s\n", headbuf );
	retcode = get_headbuf_from_message( headbuf, head, hlen );
	if( retcode != 0 ){
		warn( "生成的头文件出错!\n" );
        return -1;
    }

    info( "type(%d) result(%d) length(%d)\n", head->type, head->result, head->length );
	if( head->result != 0 ){
		warn( "接收到服务器结果集出错,result = ( %d )\n", head->result );
		return -1;
	}   		

	if( head->length == 0 ){
        info( "未收到的内容, 结束接收!\n" );
		return 0;
    }

	char * revbuf = NULL;
    char * pheadbuf = NULL;
    if( !( pheadbuf = ( strrchr( headbuf, '|' ) ) ) ){
		warn( "未找到有效数据..\n" );
        return -1;
    }
    
    pheadbuf ++;
    if( pheadbuf == '\0' ){
        info( "收到的内容为空!\n" );
		return 0;
    }
    
	revbuf = ( char * )calloc( strlen( pheadbuf ) + 1, 1 );
	if( NULL == revbuf  ){
		warn( "创建内存失败!\n" );
		return -1;
	}

	memcpy( revbuf, pheadbuf, strlen( pheadbuf ) + 1 );   
	*message = ( void *)revbuf;
	*revsize = totalen;
    return 0;
}


int get_typeid_room_from_xmldata( char *msgdata, int * typeid, int * roomid ){

	// 读取数据
	xmlDocPtr doc = get_doc_object_from_string( msgdata );
    if( NULL == doc ){
        warn( "读取数据(%s) 出错!", msgdata );
        return -1;
    }
    
    xmlNodePtr rootobj = get_root_object_from_doc_object( doc );
    if( NULL == rootobj ){
        warn( "获取根节点出错!" );
        free_doc_object( doc );
        return -1;
    }

    int retcode = 0;
    char xmlbuf[ 512 ] = { '\0' };
    retcode = get_value_from_next_layer_xml_object_by_tagname( rootobj, "roomid", 0, xmlbuf, sizeof( xmlbuf ) );
    memset( xmlbuf, '\0', sizeof( xmlbuf ) );
    *roomid = atoi( xmlbuf );
    retcode = get_value_from_next_layer_xml_object_by_tagname( rootobj, "type", 0, xmlbuf, sizeof( xmlbuf ) );
    memset( xmlbuf, '\0', sizeof( xmlbuf ) );
	*typeid = atoi( xmlbuf );    
    free_doc_object( doc );
	return 0;
}

int get_hostname_from_xmldata( char *msgdata, char *hostname, int hnlen ){
	// 读取数据
	xmlDocPtr doc = get_doc_object_from_string( msgdata );
    if( NULL == doc ){
        warn( "读取数据(%s) 出错!", msgdata );
        return -1;
    }
    
    xmlNodePtr rootobj = get_root_object_from_doc_object( doc );
    if( NULL == rootobj ){
        warn( "获取根节点出错!" );
        free_doc_object( doc );
        return -1;
    }

    int retcode = 0;
    retcode = get_value_from_next_layer_xml_object_by_tagname( rootobj, "hostname", 0, hostname, hnlen  );
    info( "hostname:%s", hostname );
    free_doc_object( doc );
	return 0;
}


int get_buuid_from_xmldata( char *msgdata, char *buuid, int uuidlen ){
	// 读取数据
	xmlDocPtr doc = get_doc_object_from_string( msgdata );
    if( NULL == doc ){
        warn( "读取数据(%s) 出错!", msgdata );
        return -1;
    }
    
    xmlNodePtr rootobj = get_root_object_from_doc_object( doc );
    if( NULL == rootobj ){
        warn( "获取根节点出错!" );
        free_doc_object( doc );
        return -1;
    }

    int retcode = 0;
    retcode = get_value_from_next_layer_xml_object_by_tagname( rootobj, "uuid", 0, buuid, uuidlen  );
    info( "buuid:%s", buuid );
    free_doc_object( doc );
	return 0;
}

int play_controls( char * rcvmsg, int rcvmsglen, char ** replymsg, int * replymsglen ){
	int retcode = 0;
#if 0    
    int typeid = 0;
    int roomid = 0;
    retcode = get_typeid_room_from_xmldata( rcvmsg, &typeid, &roomid );
    if( retcode != 0 ){
		warn( "(%d) 获取主机名出错!", retcode );
        return -1;
    }
#endif

#define MSGTYPE_PLAYSTATUS 40309
    char * msgdata = NULL;
	int msglen = 0;
	retcode = send_hdplayermanage_data( MSGTYPE_PLAYSTATUS, rcvmsg, rcvmsglen, &msgdata, &msglen );
	if( retcode != 0 ){
		
    }
    
    safe_free( msgdata );    
    //
	return 0;
}

#define HDPLAYERMANAGE_MESSAGE_PORT 30000
int send_hdplayermanage_data( int msgtype, char * senddata, int slength, char ** msgdata, int * msglen ){

	int sock = 0;
    int retcode = 0;
    if( create_tcp_client_socket( "127.0.0.1", HDPLAYERMANAGE_MESSAGE_PORT, &sock ) < 0 )
        return -1;
    retcode = charing_data_in_mbarsys_msg_channel( sock, msgtype, senddata, slength, msgdata, msglen, MAX_CHARING_MSG_RECEIVE_TIMEOUT );
    charing_close_mbarsys_msg_channel( sock );
    return retcode;
}


int get_play_status( char * rcvmsg, int rcvmsglen, char ** replymsg,int * replymsglen ){

	int retcode = 0;
#if 0    
    char buuid[ 512 ] = { '\0' };
	retcode = get_buuid_from_xmldata( rcvmsg, buuid, sizeof( buuid ) );
    if( retcode != 0 ){
		warn( "(%d) 获取主机名出错!", retcode );
        return -1;
    }
#endif

#define MSGTYPE_PLAYSTATUS 40308
    char * msgdata = NULL;
	int msglen = 0;
	retcode = send_hdplayermanage_data( MSGTYPE_PLAYSTATUS, rcvmsg, rcvmsglen, &msgdata, &msglen );
	if( retcode != 0 ){
		
    }
    
    safe_free( msgdata );
	return 0;
}


int play_change_status( void * rcvmsgdata, int rcvmsglen, void ** replymsgdata, int * replymsglen ){
	if( NULL == rcvmsgdata ){
        warn( "出错，传入值为空值" );
		return -1;
	}
    
    int status = 0;
    int retcode = 0;
    char *desc = NULL;
    char *buuid = 0;
    //retcode = decode_change_status_information_from_buffer( rcvmsgdata, rcvmsglen, &room, &status, &desc );    
	char msg[ 1024 ] = { '\0' }; 
    char replybuf[ 1024 ] = { '\0' };
    char headbuf[ 512 ] = { '\0' };
    if( retcode >= 0 ){
 		get_headmsg_from_message( headbuf, 50003, -1, strlen( msg ) );
        char * COMMANDLINE = "<respond><respcode>%d<respcode><errdesc><![CDATA[%s]]></errdesc></respond>";       
    	sprintf( msg, COMMANDLINE, retcode, "消息处理失败" );       
        sprintf( replybuf, "%s%s", headbuf, msg );
    }else{
		get_headmsg_from_message( headbuf, 50003, 0, strlen( msg ) );
        char * COMMANDLINE = "<request><buuid>%s</buuid><status>%d</status><desc><![CDATA[%s]]></desc></request>";
    	sprintf( msg, COMMANDLINE, buuid, status, desc );        
	    sprintf( replybuf, "%s%s", headbuf, msg );
    }

    char * preplymsg = safe_strdup( replybuf );
    HEART_INFO_T * heart_list = NULL;
    while( heart_list ){		
		send_data_to_charing( heart_list->sockfd, preplymsg, strlen( replybuf ) );
    	heart_list = heart_list->next;
    }
    
	return 0;
}

int vol_change_status( char * rcvmsgdata, int rcvmsglen, char ** replymsg, int * replymsglen ){
	if( NULL == rcvmsgdata ){
        warn( "出错，传入值为空值" );
		return -1;
	}

    int vol = 0;
    int retcode = 0;
    char *buuid = NULL;    
    //retcode = decode_change_status_information_from_buffer( rcvmsgdata, rcvmsglen, &buuid, &status, &desc );    
	char msg[ 1024 ] = { '\0' }; 
    char replybuf[ 1024 ] = { '\0' };
    char headbuf[ 512 ] = { '\0' };
    if( retcode >= 0 ){
 		get_headmsg_from_message( headbuf, 50003, -1, strlen( msg ) );
        char * COMMANDLINE = "<respond><respcode>%d<respcode><errdesc><![CDATA[%s]]></errdesc></respond>";       
    	sprintf( msg, COMMANDLINE, retcode, "消息处理失败" );       
        sprintf( replybuf, "%s%s", headbuf, msg );
    }else{
		get_headmsg_from_message( headbuf, 50003, 0, strlen( msg ) );
        char * COMMANDLINE = "<respond><buuid>%s<buuid><volume>%d</volume></respond>";        
    	sprintf( msg, COMMANDLINE, buuid, vol );        
	    sprintf( replybuf, "%s%s", headbuf, msg );
    }

    char * preplymsg = safe_strdup( replybuf );
    HEART_INFO_T * heart_list = NULL;
    while( heart_list ){		
		send_data_to_charing( heart_list->sockfd, preplymsg, strlen( replybuf ) );
    	heart_list = heart_list->next;
    }
    
	return 0;
}

int mute_change_status( char * rcvmsgdata, int rcvmsglen, char ** replymsg, int * replymsglen ){
	if( NULL == rcvmsgdata ){
        warn( "出错，传入值为空值" );
		return -1;
	}

    int mute = 0;
    int status = 0;
    int retcode = 0;
    char *buuid = NULL;    
    //retcode = decode_change_status_information_from_buffer( rcvmsgdata, rcvmsglen, &buuid, &status, &desc );    
	char msg[ 1024 ] = { '\0' }; 
    char replybuf[ 1024 ] = { '\0' };
    char headbuf[ 512 ] = { '\0' };
    if( retcode >= 0 ){
 		get_headmsg_from_message( headbuf, 50003, -1, strlen( msg ) );
        char * COMMANDLINE = "<respond><respcode>%d<respcode><errdesc><![CDATA[%s]]></errdesc></respond>";       
    	sprintf( msg, COMMANDLINE, retcode, "消息处理失败" );       
        sprintf( replybuf, "%s%s", headbuf, msg );
    }else{
		get_headmsg_from_message( headbuf, 50003, 0, strlen( msg ) );
        char * COMMANDLINE = "<respond><buuid>%s<buuid><mute>%d</mute></respond>";        
    	sprintf( msg, COMMANDLINE, buuid, mute );        
	    sprintf( replybuf, "%s%s", headbuf, msg );
    }

    char * preplymsg = safe_strdup( replybuf );
    HEART_INFO_T * heart_list = NULL;
    while( heart_list ){		
		send_data_to_charing( heart_list->sockfd, preplymsg, strlen( replybuf ) );
    	heart_list = heart_list->next;
    }
    
	return 0;
    
}

int do_device_register_process( int sock, int msgtype, char * ipaddr, char * rcvmsg, int rcvmsglen,char ** replymsg,int * replymsglen ){
	if( NULL == ipaddr || NULL == rcvmsg ){
        warn( "传入参数输错, ipaddr(%p) rcvmsg(%p)", ipaddr, rcvmsg );
		return -1;
    }

    int retcode = 0;
    char msghead[ 64 ] = { '\0' };
	char hostname[ 128 ] = { '\0' };
	retcode = get_hostname_from_xmldata( rcvmsg, hostname, sizeof( hostname ) );
    if( retcode != 0 ){
		warn( "(%d) 获取主机名出错!", retcode );
        return -1;
    }

    //msgtype 
    retcode = registration_hostname( sock, hostname, ipaddr );
	char msg[ 512 ] = { '\0' }; 
    char replybuf[ 1024 ] = { '\0' };        
    char * COMMANDLINE = "<respond><respcode>%d<respcode><errdesc><![CDATA[%s]]></errdesc></respond>";
    if( retcode != 0 ){
    	sprintf( msg, COMMANDLINE, retcode, "消息处理失败" );
        get_headmsg_from_message( msghead, msgtype, retcode, strlen( msg ) );
        sprintf( replybuf, "%s%s", msghead, msg );
    }else{
    	sprintf( msg, COMMANDLINE, retcode, "消息处理成功" );
        get_headmsg_from_message( msghead, msgtype, retcode, strlen( msg ) );
    }

    sprintf( replybuf, "%s%s", msghead, msg );
    char * preplymsg = safe_strdup( replybuf );
	*replymsg = preplymsg;
    *replymsglen = strlen( msg );
    return retcode;
}


int start_receive_deal_charing_manage_message( void ){
    int sock;
    if( ( sock = create_tcp_server_socket( CHARING_MESSAGE_PORT ) ) < 0 ){
        warn( "创建 mbarsys msg TCP 出错, sock值为%d\n", sock );
        return -1;
    }

    while( 1 ){
        int newsock;
        if( ( newsock = monitor_charing_msg_channel( sock ) ) < 0 ){
            warn( "监听 mbarsys msg 出错!\n" );
            continue;
        }

        char ipaddr[ 64 ] = { '\0' };
        newsock = create_tcp_accept_socket_with_args( sock, ipaddr, sizeof( ipaddr ), NULL );
        if( newsock < 0 ){
            warn( "监控CHARINGMANAGE 消息通道出错/错误原因:%s", strerror( errno ) );                        
            continue;
        }

            info( ">> 4 \n" );
        deal_data_from_charing_msg_channel( newsock, ipaddr, deal_charing_message_event );
    }

    close_mbarsys_msg_channel( sock );    
    return 0;
}



int deal_hdplayer_manage_message(  void * args ){
  	pthread_detach( pthread_self() );
    
    int sock;
    if( ( sock = create_tcp_server_socket( CHARING_MESSAGE_PORT2 ) ) < 0 ){
        warn( "创建HDPLAYERMANAGE 消息通道出错.错误原因:%s", strerror( errno ) );
        return -1;
    }

    while( 1 ){
        int newsock;
        if( ( newsock = monitor_charing_msg_channel( sock ) ) < 0 ){
            warn( "监听 mbarsys msg 出错!\n" );
            continue;
        }

        char ipaddr[ 64 ] = { '\0' };
        newsock = create_tcp_accept_socket_with_args( sock, ipaddr, sizeof( ipaddr ), NULL );
        if( newsock < 0 ){
            warn( "监控CHARINGMANAGE 消息通道出错/错误原因:%s", strerror( errno ) );                        
            continue;
        }
            
        deal_data_from_hdplayermange_msg_channel( newsock, ipaddr, deal_hdplayermange_message_event );
    }

    close_mbarsys_msg_channel( sock );    
    return 0;
}

int start_receive_deal_hdplayer_manage_message(){
    pthread_t pid;
    if( pthread_create( &pid, NULL, deal_hdplayer_manage_message, NULL ) < 0 ){
        return -1;
    }
    
    return 0;    
}

