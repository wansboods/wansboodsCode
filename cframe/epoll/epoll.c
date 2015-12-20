#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>       /* basic system data types */
#include <sys/socket.h>      /* basic socket definitions */
#include <netinet/in.h>       /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>       /* inet(3) functions */
#include <sys/epoll.h>       /* epoll function */
#include <fcntl.h>     /* nonblocking */
#include <sys/resource.h> /*setrlimit */

#include "epoll.h"
#include "epollTransmission.h"

#define MAXEPOLLSIZE 10000
#define MAXLINE 10240


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


//int handle( int connfd );
int setnonblocking( int sockfd );
int deal_data_from_mbarsys_msg_channel( int connfd, char * ipaddr );
//int deal_mbarsys_message_event( int sock, char *ippadr, int type, char * message, int length, void ** msgdata, int * msglen );


#define MAX_EPOOL_MSG_RECEIVE_TIMEOUT 2

int main( int argc, char **argv )
{
    int servPort = 6888;
    int listenq = 1024;

    int listenfd, connfd, kdpfd, nfds, n, nread, curfds,acceptCount = 0;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t socklen = sizeof(struct sockaddr_in);
    struct epoll_event ev;
    struct epoll_event events[MAXEPOLLSIZE];
    struct rlimit rt;
    char buf[MAXLINE];

    /* 设置每个进程允许打开的最大文件数 */
    rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
    if (setrlimit(RLIMIT_NOFILE, &rt) == -1){        
        perror("setrlimit error");
        return -1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
    servaddr.sin_port = htons (servPort);

    listenfd = socket(AF_INET, SOCK_STREAM, 0); 
    if ( listenfd == -1 ){
        perror("can't create socket file");
        return -1;
    }

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if( setnonblocking( listenfd ) < 0 ) {
        perror("setnonblock error");
    }

    if( bind( listenfd, ( struct sockaddr *) &servaddr, sizeof( struct sockaddr ) ) == -1 ){
        perror("bind error");
        return -1;
    } 

    if (listen(listenfd, listenq) == -1){
        perror( "listen error" );        
        return -1;
    }
    
    /* 创建 epoll 句柄，把监听 socket 加入到 epoll 集合里 */
    kdpfd = epoll_create( MAXEPOLLSIZE );
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listenfd;
    if (epoll_ctl( kdpfd, EPOLL_CTL_ADD, listenfd, &ev ) < 0) 
    {
        fprintf( stderr, "epoll set insertion error: fd=%d\n", listenfd );
        return -1;
    }
    
    curfds = 1;
    info( "epollserver startup,port %d, max connection is %d, backlog is %d\n", servPort, MAXEPOLLSIZE, listenq);

    for (;;){
        /* 等待有事件发生 */
        nfds = epoll_wait( kdpfd, events, curfds, -1 );
        if (nfds == -1)
        {
            perror( "epoll_wait" );
            continue;
        }
        
        /* 处理所有事件 */
        for ( n = 0; n < nfds; ++n)
        {
            if ( events[n].data.fd == listenfd ){
                connfd = accept( listenfd, (struct sockaddr *)&cliaddr, &socklen );
                if ( connfd < 0 ) {
                    perror("accept error");
                    continue;
                }

                sprintf( buf, "accept form %s:%d\n", inet_ntoa( cliaddr.sin_addr ), cliaddr.sin_port );
                info( "%d:%s", ++acceptCount, buf );
                if ( curfds >= MAXEPOLLSIZE ) {
                    fprintf( stderr, "too many connection, more than %d\n", MAXEPOLLSIZE );
                    close( connfd );
                    continue;
                }
                
                if ( setnonblocking( connfd ) < 0 ) {
                    perror("setnonblocking error");
                }
                
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = connfd;
                if ( epoll_ctl( kdpfd, EPOLL_CTL_ADD, connfd, &ev ) < 0)
                {
                    fprintf(stderr, "add socket '%d' to epoll failed: %s\n", connfd, strerror(errno));
                    return -1;
                }
                curfds++;
                continue;
            }
            
            // 处理客户端请求
            if( deal_data_from_mbarsys_msg_channel( events[n].data.fd, inet_ntoa( cliaddr.sin_addr ) ) < 0 ){
                epoll_ctl( kdpfd, EPOLL_CTL_DEL, events[n].data.fd, &ev );
                curfds --;
            }            
        }
    }
    
    close( listenfd );
    return 0;
}


int setnonblocking( int sockfd ){
    
    if( fcntl( sockfd, F_SETFL, fcntl( sockfd, F_GETFD, 0 )|O_NONBLOCK ) == -1) {
        return -1;
    }
    
    return 0;
}

int receive_epoll_data( int connfd, char * msg, int length ){
    int nread = 0;
    nread = read( connfd, msg, length ); //读取客户端socket流
    if ( nread == 0 ) {
        warn( "client close the connection\n" );
        return -1;
    } 
    
    if ( nread < 0 ) {
        perror("read error");
        return -1;
    }

    return nread;
}


int deal_data_from_mbarsys_msg_channel( int connfd, char * ipaddr ) {
	if( connfd < 0  ){
        warn( "传入参数出错!" );
		return -1;
    }

    int retcode = 0;
	EPOLL_MESSAGE_HEAD rcvmsghead;
	memset( &rcvmsghead, 0, sizeof( EPOLL_MESSAGE_HEAD ) );    

    void * rcvmsgdata = NULL;
    retcode = receive_epoll_data_from_tcp_communication_channel( connfd, &rcvmsghead, sizeof( rcvmsghead ), &rcvmsgdata, MAX_EPOOL_MSG_RECEIVE_TIMEOUT );
    if( retcode < 0 ){
        warn( "接收数据出错!" );
        return retcode;
	}
    
    info( "影柜系统消息通道: 接收消息,消息类型(%d), 消息体长度(%d)\n", rcvmsghead.type, rcvmsghead.length );
    void * replymsgdata = NULL;
    int replymsglen = 0;
    retcode = deal_mbarsys_message_event( connfd, ipaddr, rcvmsghead.type, rcvmsgdata, rcvmsghead.length, &replymsgdata, &replymsglen );
    
    EPOLL_MESSAGE_HEAD replymsghead;
	memset( &replymsghead, 0, sizeof( EPOLL_MESSAGE_HEAD ) );

    replymsghead.type = rcvmsghead.type;
    replymsghead.result = retcode;
    replymsghead.length = replymsglen;

    info( "影柜系统消息通道: 回应消息,消息类型(%d), 消息体长度(%d), 消息结果(%d)", replymsghead.type, replymsghead.length, replymsghead.result );
    retcode = send_epoll_data_to_tcp_communication_channel( connfd, &replymsghead, sizeof( replymsghead ), replymsgdata );
	
    safe_free( rcvmsgdata );
    safe_free( replymsgdata );

    if( retcode < 0 )
        return retcode;

    return retcode;

#if 0

    char readbuf[ MAXLINE ] = {'\0'};
    int nread = receive_epoll_data( connfd, readbuf, sizeof( readbuf ) );
    close( connfd );
	
	printf( "读取到的数据:%s\n", readbuf );	
	
	time_t now;
	now = time(NULL);
	memset( readbuf, 0 , sizeof( readbuf ) );
    /*ctime将系统时间转换为字符串，sprintf使转化后的字符串保存在buf*/
    sprintf( readbuf, "%24s\r\n", ctime( &now ) );
    write( connfd, readbuf, nread );//响应客户端  
    return 0;
#endif
}



