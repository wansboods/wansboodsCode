#ifndef __TMYSQL_H__
#define __TMYSQL_H__

typedef struct{

    int tablebody_id;
    char * data;

    void * next;
}TABLEBODY_DETAILS;

typedef struct{    
    TABLEBODY_DETAILS * list;
    void * next;
}HORIZONTAL_DETAILS_LIST;


typedef enum{
    DBINSERT  = 1,
    DBDELETE  = 2,
    DBUPDATE = 3,
    DBQUERY  = 4, 
}DB_TYPE;


int queryMysql( char * querystatement, HORIZONTAL_DETAILS_LIST ** pdata );



#endif //__TMYSQL_H__
