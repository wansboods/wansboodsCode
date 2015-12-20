#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "cJSON.h"  //需要把该头文件放在后面包含  否则会找不到size_t

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


//解析JSON
void parse_json(const char *filename)
{
    printf("----------------parse json start-------------------------------\n");
    
    //从文件中读取要解析的JSON数据
    FILE *fp = fopen(filename, "r");
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *data = (char*)malloc(len + 1);
    fread(data, 1, len, fp);
    fclose(fp);
    printf("%s", data);

    //解析JSON数据
    cJSON *root_json = cJSON_Parse( data );    //将字符串解析成json结构体
    if (NULL == root_json){
        
        printf( "error:%s\n", cJSON_GetErrorPtr());
        cJSON_Delete(root_json);
        return;
    }
	
	// 根据结构体获取数组大小
	int size = cJSON_GetArraySize( root_json );
    printf( "获取结构体数组大小:%d \n", size );
    cJSON * arrayItem = NULL;

    int i;
    for( i = 0; i < size; i ++ ){

		//获取第i个数组项
        arrayItem = cJSON_GetArrayItem( root_json, i );
        //info( "第一层元素内容: (%s) \n", arrayItem->string );
        char *parseSecondData = cJSON_Print( arrayItem );
        //info( "第二层内容: (%s) \n", parseSecondData );
        int sizeSecond = cJSON_GetArraySize( arrayItem );
		info( "第二层数组个数: ( %d )\n", sizeSecond );
#if 1
        int j;
		cJSON * arrayItem2 = NULL;
        info( "第三层内容:\n" );
        for( j = 0; j < sizeSecond; j ++ ){
			arrayItem2 = cJSON_GetArrayItem( arrayItem, j );
            char * parseThirdData = cJSON_Print( arrayItem2 );
			//info( "--:  (%s) \n", parseThirdData );

            cJSON * firstName = cJSON_GetObjectItem( arrayItem2, "firstName" );
            cJSON * lastName = cJSON_GetObjectItem( arrayItem2, "lastName" );
            cJSON * email = cJSON_GetObjectItem( arrayItem2, "email" );

            char * na = cJSON_Print( firstName );
            char * ln = cJSON_Print( lastName );
            char * el = cJSON_Print( email );

            printf( "-------------内容-------------------\n" );
            printf( "firstName  : %s \n", na );
			printf( "lastName  : %s \n ", ln );
            printf( "email         : %s \n", el );
            printf( "-------------结束-------------------\n\n\n" );
        }
#endif        
 
    }


    cJSON_Delete( root_json ); 
    printf("----------------parse json end--------------------------------\n");
}

//创建JSON
void create_json()
{
    printf("----------------create json start-----------------------------\n");
    //组JSON
    cJSON *root_json = cJSON_CreateObject();
    cJSON_AddItemToObject(root_json, "name", cJSON_CreateString("EVDI"));
    cJSON *data_json = cJSON_CreateObject();
    cJSON_AddItemToObject(root_json, "data", data_json);
    //添加的另一种方式:cJSON_AddNumberToObject(data_json, "id", 1);通过源码发现仅仅是对cJSON_AddItemToObject的define
    cJSON_AddItemToObject(data_json, "id", cJSON_CreateNumber(1));
    //添加的另一种方式:cJSON_AddStringToObject(data_json, "username", "hahaya");
    cJSON_AddItemToObject(data_json, "username", cJSON_CreateString("hahaya"));
    cJSON_AddItemToObject(data_json, "userpass", cJSON_CreateString("123456"));
    cJSON_AddItemToObject(data_json, "version", cJSON_CreateString("1.0"));

    //打印JSON
    char *out = cJSON_Print(root_json);
    printf("%s\n", out);
    free(out);
    printf("----------------create json end-------------------------------\n");
}

int main()
{
    parse_json( "test.jason" );
    create_json();
    return 0;
}
