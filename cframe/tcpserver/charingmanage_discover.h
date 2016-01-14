#ifndef __CHARINGMANAGE_DISCOVER_H
#define __CHARINGMANAGE_DISCOVER_H



static pthread_mutex_t s_mutex = PTHREAD_MUTEX_INITIALIZER;
int lock( VOID ){
    return pthread_mutex_lock( &s_mutex );
}

int unlock( VOID ){
    return pthread_mutex_unlock( &s_mutex );
}

typedef struct session{
	int sockfd;
	int count;
    
	char ipaddr[ 64 ];
	char hostname[ 128 ];    

    struct session *next;
}HEART_INFO_T; 


int start_charing_heart( void );
int start_charing_manage_discover_monitor_task();

HEART_INFO_T * create_heart_information_list_node_ext( int fd, char * hostname, char * ipaddr );
void add_node_into_heart_information_list( HEART_INFO_T ** list, HEART_INFO_T * node );

int update_register_information_in_heart_information_node( HEART_INFO_T * node, int fd, char * hostname, char * ipaddr );
int registration_hostname(  int sock, char * hostname, char * ipaddr );

#endif //__CHARINGMANAGE_DISCOVER_H
