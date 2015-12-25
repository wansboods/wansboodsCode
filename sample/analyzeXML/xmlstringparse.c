#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "xmlstringparse.h"

int xmlstring_safe_strcmp( char * s1, char * s2 )
{
    if( ( NULL == s1 ) || ( NULL == s2 ) )
    {
        return -1;
    }
    
    if( strlen( s1 ) == strlen( s2 ) )
    {
        if( memcmp( s1, s2, strlen( s1 ) ) == 0 )
        {
            return 0;
        }
    }
    
    return -1;
}


int get_value_from_xml_string_by_tag( char * message, char * taghead, char * tagtail, char * value, int length )
{
    if( ( NULL == message ) || ( NULL == taghead ) || ( NULL == tagtail ) || ( NULL == value ) )
    {
        return -1;
    }

    char * startp;
    char * stopp;
    
    if( NULL == ( startp = strstr( message, taghead ) ) )
    {
        return -1;
    }
    startp += strlen( taghead );
    if( NULL == ( stopp = strstr( startp, tagtail ) ) )
    {
        return -1;
    }
    if( stopp - startp >= length )
    {
        return -1;
    }
    memcpy( value, startp, stopp - startp );
    return 0;
}

char * dup_strip_xml_space( char * xmlmsg )
{
    int length = strlen( xmlmsg );
    char * newmsg;
    if( NULL == ( newmsg = ( char * )malloc( length + 1 ) ) )
    {
        return NULL;
    }
    memset( newmsg, 0, length + 1 );
    
    int i, j = 0;
    char * pold = xmlmsg;
    char * pnew = newmsg;
    int ignore = 1;
    
    for( i = 0; i < length; i++ )
    {
        if( pold[i] == '<' )
        {
            ignore = 0;
        }
        if( !ignore )
        {
            pnew[ j++ ] = pold[ i ];
        }
        if( pold[i] == '>' )
        {
            ignore = 1;
        }
        
    }
    return newmsg;
}

xmlDocPtr get_doc_object_from_string( char * msgxml )
{
    int length = strlen( msgxml );
    xmlDocPtr doc;
    if( NULL == ( doc = xmlParseMemory( msgxml, length ) ) )
    {
        printf( "failed to xmlParseMemory..\n--->msgxml[%s]\n", msgxml );
        return NULL;
    }
    
    return doc;
}

xmlNodePtr get_root_object_from_doc_object( xmlDocPtr docobj )
{
    return xmlDocGetRootElement( docobj );
}

void free_doc_object( xmlDocPtr docobj )
{
    xmlFreeDoc( docobj );
}


int get_value_of_xml_object( xmlNodePtr node, char * value, int length )
{
    if( ( NULL == node ) || ( NULL == value ) )
    {
        return -1;
    }
    char * content = ( char * )xmlNodeGetContent( node );
    if( content )
    {
        if( strlen( content ) + 1 > length )
        {
            xmlFree( content );
            return -1;
        }
        memcpy( value, content, strlen( content ) + 1 );
        xmlFree( content );
    }    
    return 0;
}


int get_value_from_sn( xmlNodePtr node, char * tagname, int index, char * value, int length )
{
    if( ( NULL == node ) || ( NULL == tagname ) || ( NULL == value ) )
    {
        return -1;
    }

    xmlNodePtr curNode = node->xmlChildrenNode;    
    int count = 0;
    while( curNode )
    {    
        //char *content = ( char * )xmlNodeGetContent( curNode );
        if( count == index )
        {
            char *content = ( char * )xmlNodeGetContent( curNode );
            if( content )
            {
                if( strlen( content ) + 1 > length )
                {
                    xmlFree( content );
                    return -1;
                }

                memcpy( value, content, strlen( content ) + 1 );
                xmlFree( content );
            }
            
            return 0;
        }

        count++;        
        curNode = curNode->next;
    }
    
    return -1;
}



int get_value_from_next_layer_xml_object_by_tagname( xmlNodePtr node, char * tagname, int index, char * value, int length )
{
    if( ( NULL == node ) || ( NULL == tagname ) || ( NULL == value ) )
    {
        printf( "--[%s][%d]\n", __FUNCTION__, __LINE__ );
        return -1;
    }

    xmlNodePtr curNode = node->xmlChildrenNode;    
    int count = 0;
    while( curNode )
    {
        if( xmlstring_safe_strcmp( ( char * )curNode->name, tagname ) == 0 )
        {
            if( count == index )
            {
                char * content = ( char * )xmlNodeGetContent( curNode );
                if( content )
                {
                    if( strlen( content ) + 1 > length )
                    {
                        xmlFree( content );
                        return -1;
                    }

                    memcpy( value, content, strlen( content ) + 1 );
                    xmlFree( content );
                }
                return 0;
            }
            count++;
        }
        curNode = curNode->next;
    }

    
    return -1;
}

int get_objects_number_from_next_layer_xml_object( xmlNodePtr node )
{
    if( NULL == node )
    {
        return -1;
    }
    
    xmlNodePtr childrenNode = node->xmlChildrenNode;
    int count = 0;
    while( childrenNode )
    {
        count++;
        childrenNode = childrenNode->next;
    }
    return count;
}

int get_objects_number_from_next_layer_xml_object_by_tagname( xmlNodePtr node, char * tagname )
{
    if( ( NULL == node ) || ( NULL == tagname ) )
    {
        return -1;
    }
    
    xmlNodePtr childrenNode = node->xmlChildrenNode;
    int count = 0;
    while( childrenNode )
    {
        if( xmlstring_safe_strcmp( ( char * )childrenNode->name, tagname ) == 0 )
        {
            count++;
        }
        childrenNode = childrenNode->next;
    }
    
    return count;
}

xmlNodePtr get_objects_from_next_layer_xml_object_by_tagname( xmlNodePtr node, char * tagname, int index )
{
    if( ( NULL == node ) || ( NULL == tagname ) )
    {
        return NULL;
    }

    xmlNodePtr curNode = node->xmlChildrenNode;
    int count = 0;
    while( curNode )
    {
        if( xmlstring_safe_strcmp( ( char * )curNode->name, tagname ) == 0 )
        {
            if( count == index )
            {
                return curNode;
            }
            
            count++;
        }
        
        curNode = curNode->next;
    }
    
    return NULL;
}


