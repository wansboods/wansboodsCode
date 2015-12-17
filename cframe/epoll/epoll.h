#ifndef __EPOLL_H__
#define __EPOLL_H__


typedef int ( *EPOLL_MESSAGE_DEAL_CB )( int sock, char *ipaddr, int msgtype, void* rcvmsgdata, int rcvmsglen, void ** replymsgdata, int * replymsglen );



#endif  //__EPOLL_H__

