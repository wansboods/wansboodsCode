#include <stdio.h>
#include <stdarg.h>

#define warn( format, ... ) text_print( format, __FUNCTION__, __LINE__,  ##__VA_ARGS__ )
//printf( strcpy(format, '\n'), ##__VA_ARGS__ )


#define  DEFAULT_BUFFER_LEN        1024
int text_print( char *fmt, char * func, int line, ...)
{
    int cnt = 0;
    va_list argptr;
    char buffer[DEFAULT_BUFFER_LEN];
    if (fmt == NULL)        
        return -1;
    va_start(argptr, fmt);
    cnt = vsprintf(buffer, fmt, argptr);
    printf( "[%s][%d] %s\n", func, line, buffer );
    va_end(argptr);
    return(cnt);
}

int main()
{
    warn("dadasdas" );
    warn( "ssss=%s", "sdasdas" );
    return 0;
}




#if 0
int main(){

   warn( "--->%s", "ssss" );
return 0;
}
#endif


