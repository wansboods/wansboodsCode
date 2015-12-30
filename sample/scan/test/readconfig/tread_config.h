#ifndef __TREAD_CONFIG_H__
#define __TREAD_CONFIG_H__


typedef struct{
    char * l_value;
    char * r_value;

    void *next;
}READCFN_CONFIG_LIST;


int cfn_read_config( char * path, READCFN_CONFIG_LIST ** glist );


#endif //__TREAD_CONFIG_H__
