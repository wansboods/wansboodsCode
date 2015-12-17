#ifndef __EPOLLTRANSMISSION_H__
#define __EPOLLTRANSMISSION_H__

#include <stdio.h>

typedef struct{
    int type;
    int result;
    int length;    
}EPOLL_MESSAGE_HEAD;


int send_epoll_data_to_tcp_communication_channel( int sockfd, EPOLL_MESSAGE_HEAD * head, int headlen, void * message );
int receive_epoll_data_from_tcp_communication_channel( int sockfd, EPOLL_MESSAGE_HEAD * head, int headlen, void ** message, int timeout );


#endif //__EPOLLTRANSMISSION_H__

