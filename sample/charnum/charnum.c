#include <stdio.h>
#include <stdlib.h>

int main(){

    double d = 0;
    char *endptr;
    d = strtod( "23", &endptr );
	if ( ( endptr != NULL && *endptr != '\0')) {
            //字符串不是有效数值
            printf( "字符串不是有效数值" );
	}else{
            printf( "字符串是有效值" );
        }

      return 0;
}
