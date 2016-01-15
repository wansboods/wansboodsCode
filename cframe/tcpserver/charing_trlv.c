#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "zlog.h"
#include "udpapi.h"
#include "tcpapi.h"
#include "safeop.h"
#include "charing_trlv.h"

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

int receive_charing_data_from_tcp_communication_channel( int sockfd, CHARING_TRLV_MESSAGE_HEAD * head, int headlen, void ** message, int timeout )
{
    if( NULL == head )
        return -1;

    int totalen = 0;
    int recvlen;
    void * msgdata = NULL;
    if( ( recvlen = receive_data_from_tcp_socket( sockfd, head, headlen, timeout ) ) < 0 )
    {
        printf( "从TCP通道中读取数据失败.错误原因: %s\n", strerror( errno ) );
        return -1;
    }

    if( recvlen < sizeof( CHARING_TRLV_MESSAGE_HEAD ) )
    {
        printf( "从TCP通道中读取的数据长度%d小于消息头的长度%ld,这是一个无效的消息.丢弃\n", recvlen, sizeof( CHARING_TRLV_MESSAGE_HEAD ) );
        safe_free( msgdata );
        return -1;
    }
    
    totalen += recvlen;

    if( head->length && message )
    {
        char * msgdata;
        if( NULL == ( msgdata = ( char * )malloc( head->length + 1 ) ) )
        {
            printf( "分配%d字节内存失败.错误原因: %s\n", head->length + 1, strerror( errno ) );
            return -1;
        }
        
        memset( msgdata, 0, head->length + 1 );
        if( ( recvlen = receive_data_from_tcp_socket( sockfd, msgdata, head->length, timeout ) ) < 0 )
        {
            printf( "从TCP通道中读取数据失败.错误原因: %s\n", strerror( errno ) );
            return -1;
        }
        
        totalen += recvlen;
        *message = msgdata;
    }

    return totalen;
}

int send_trlv_data_to_tcp_communication_channel( int sockfd, CHARING_TRLV_MESSAGE_HEAD * head, int headlen, void * message, int msglen )
{
    int totalen = 0;
    int sndlen = 0;
    int sndbodylen = msglen;
    if( head && headlen ){
        //dzlog_debug( "过程" );
        if( ( sndlen = send_data_to_tcp_socket( sockfd, head, headlen ) ) < 0 )    
        {
            printf( "向TCP通道中写入数据失败.错误原因: %s\n", strerror( errno ) );
            return -1;
        }

        //dzlog_debug( "过程" );
        totalen += sndlen;
        //dzlog_debug( "totalen(%d)..\n", totalen );
        sndbodylen = head->length;
    }
    
    if( message )
    {
        //dzlog_debug( "过程" );
        if( ( sndlen = send_data_to_tcp_socket( sockfd, message, sndbodylen ) ) < 0 )    
        {
            printf( "向TCP通道中写入数据失败.错误原因: %s\n", strerror( errno ) );
            return -1;
        }

        //dzlog_debug( "过程" );
        totalen += sndlen;
        //dzlog_debug( "totalen(%d)..\n", totalen );
    }

    //dzlog_debug( "totalen = %d", totalen );
    return totalen;
}


int receive_data_from_mbarsys_msg_channel( int sock, CHARING_TRLV_MESSAGE_HEAD * head, int headlen, void ** message, int timeout ) {
    if( ( sock == -1 ) || ( NULL == head ) )
        return -1;

    int length = receive_charing_data_from_tcp_communication_channel( sock, head, headlen, message, timeout );
    return length;
}

int send_data_to_mbarsys_msg_channel( int sock, CHARING_TRLV_MESSAGE_HEAD * head, int headlen, void * message ) 
{
    if( ( sock == -1 ) || ( NULL == head ) )
        return -1;

    int length = send_trlv_data_to_tcp_communication_channel( sock, head, headlen, message, head->length );
    return length;
}


int charing_data_in_mbarsys_msg_channel( int sock, int msgtype, void * sndmsg, int sndlen, void * * rcvmsg, int * rcvlen, int timeout )
{
    if( sock < 0 )
        return -1;

    CHARING_TRLV_MESSAGE_HEAD msghead;
    memset( &msghead, '\0', sizeof( CHARING_TRLV_MESSAGE_HEAD ) );
    
    msghead.type = msgtype;
    msghead.result = 0;
    msghead.length = sndlen;

    RTSTATUS retcode;
    if( ( retcode = send_data_to_mbarsys_msg_channel( sock, &msghead, sizeof( msghead ), sndmsg ) ) < 0 ){
        warn( "error, send data lens : %d\n", retcode );
        return retcode;

    }

    //printf( "send data lens : %d\n", retcode );
    void * msgbody = NULL;
    CHARING_TRLV_MESSAGE_HEAD rcvhead;
    if( ( retcode = receive_data_from_mbarsys_msg_channel( sock, &rcvhead, sizeof( rcvhead ), &msgbody, timeout ) ) < 0 ){
        warn( "(%d) failed to receive_data_from_mbarsys_msg_channel", retcode );
        return retcode;
    }

    if( rcvhead.type != msgtype )
    {
        warn( "发送的消息类型%d与接收的消息类型%d不匹配\n", msgtype, rcvhead.type );        
        safe_free( msgbody );
        return -1;
    }

    
    if( rcvhead.result != 0 )
    {
        warn( "出错, 接受结果(%d)", rcvhead.result );
        safe_free( msgbody );
        return rcvhead.result;
    }
    
    if( rcvmsg )
        *rcvmsg = msgbody;
    else
        safe_free( msgbody );

    if( rcvlen )
        *rcvlen = rcvhead.length;

    return 0;
}

int charing_close_mbarsys_msg_channel( int sock ){
    int retcode = close_tcp_socket( sock );
    return retcode;
}

int charing_open_discover_msg_channel( int port ){
    int sock;    
    if( ( sock = create_udp_server_socket( port ) ) < 0 )
        return -1;

    return sock;    
}

int charing_open_custom_msg_channel( int port, char * netif ){
    int sock;    
    if( ( sock = create_udp_server_custom_socket( port, netif ) ) < 0 )
    {
        printf( "failed to create_udp_server_custom_socket.." );
        return -1;
    }
    
    return sock;
}

int monitor_charing_msg_channel( int sock ){
    int newsock;
    if( ( newsock = create_tcp_accept_socket( sock ) ) < 0 )
        return -1;

    return newsock;
}

int send_data_to_discover_msg_channel( int sock, char keyword, char * msgdata, int msglen, char * ipaddr, unsigned short port )
{
    if( ( sock == -1 ) || ( NULL == msgdata ) || ( NULL == ipaddr ) )
        return -1;


    DISCOVER_TRANSFER_DATA_T tpdata;
    tpdata.keyword = keyword;
    tpdata.length = msglen;
    if( strlen( msgdata ) + 1 > sizeof( tpdata.data ) )
        return -1;
    memcpy( tpdata.data, msgdata, strlen( msgdata ) + 1 );

    int length = send_data_to_udp_socket( sock, &tpdata, sizeof( DISCOVER_TRANSFER_DATA_T ), ipaddr, port );

    return length;
}

int receive_data_from_discover_msg_channel( int sock, char * keyword, void ** message, int * msglen, char * ipaddr, int length, unsigned short * port, int timeout )
{
    if( ( sock == -1 ) || ( NULL == keyword ) || ( NULL == message ) )
        return -1;

    DISCOVER_TRANSFER_DATA_T tpdata;
    int recvlen = receive_data_from_udp_socket( sock, &tpdata, sizeof( tpdata ), ipaddr, length, port, timeout );

    if( recvlen < sizeof( DISCOVER_TRANSFER_DATA_T ) )
        return -1;

    char * msgdata;
    if( NULL == ( msgdata = safe_strdup( tpdata.data ) ) )
        return -1;

    *keyword = tpdata.keyword;
    *message = msgdata;
    *msglen = tpdata.length;

    return recvlen;
}