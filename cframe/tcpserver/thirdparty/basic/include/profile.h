#ifndef __PROFILE_H__
#define __PROFILE_H__




int init( const char *filepath, void **handle );
int del( void *handle, const char *key );
int add( void *handle, const char *key, const char *value );
int getCount( void *handle, int *count );
int getValue(void *handle, const char *key, char *value);
int setValue( void *handle, const char *key, const char *value );
int getVideoValue( void *handle, const char *key );
int getKeys(void *handle, char *** keys, int *keyscount );
int free_keys(char ***keys,int *keyscount);
int free_values(char ***values, int *valuescount);
int release( void **handle );






#endif //__PROFILE_H__
