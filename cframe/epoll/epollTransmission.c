#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "safeop.h"
#include "tcpapi.h"
#include "epollTransmission.h"


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


int send_epoll_data_to_tcp_communication_channel( int sockfd, EPOLL_MESSAGE_HEAD * head, int headlen, void * message ){

    int totalen = 0;
    int sndlen = 0;
	int sndbodylen = 0;
    if( message )
    	sndbodylen = strlen( message );
    
    if( head && headlen ){
        if( ( sndlen = send_data_to_tcp_socket( sockfd, head, headlen ) ) < 0 ){
            
            warn( "向TCP通道中写入数据失败.错误原因:%s", strerror( errno ) );
            return -1;
        }
        
        totalen += sndlen;
        sndbodylen = head->length;
    }
    
    if( message ){
        if( ( sndlen = send_data_to_tcp_socket( sockfd, message, sndbodylen ) ) < 0 ){
            warn( "向TCP通道中写入数据失败.错误原因: %s\n", strerror( errno ) );
            return -1;
        }

        totalen += sndlen;
    }

    return totalen;
}

int receive_epoll_data_from_tcp_communication_channel( int sockfd, EPOLL_MESSAGE_HEAD * head, int headlen, void ** message, int timeout ){    
    if( NULL == head ){
		warn( "传入参数出错!" );
        return -1;
	}
    
    int totalen = 0;
    int recvlen;
    PVOID msgdata = NULL;
    if( ( recvlen = receive_data_from_tcp_socket( sockfd, head, headlen, timeout ) ) < 0 ){
        warn( "从TCP通道中读取数据失败.错误原因: %s\n", strerror( errno ) );
        return -1;
    }
        
    if( recvlen != sizeof( EPOLL_MESSAGE_HEAD ) ){
        warn( "从TCP通道中读取的数据长度%d 不等于消息头的长度%ld, 这是一个无效的消息.丢弃", recvlen, sizeof( EPOLL_MESSAGE_HEAD ) );
        return -1;
    }
    
    totalen += recvlen;
    if( head->length && message ){
        char * msgdata;
        if( NULL == ( msgdata = ( char * )calloc( head->length + 1, 1 ) ) ){
            
            warn( "分配%d字节内存失败.错误原因: %s\n", head->length + 1, strerror( errno ) );
            return -1;
        }
        
        if( ( recvlen = receive_data_from_tcp_socket( sockfd, msgdata, head->length, timeout ) ) < 0 ){            
            warn( "从TCP通道中读取数据失败.错误原因: %s\n", strerror( errno ) );
            return -1;
        }
        
        totalen += recvlen;
        *message = msgdata;
    }

    return totalen;
}   
