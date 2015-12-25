#ifndef __XML_STRING_PARSE_H
#define __XML_STRING_PARSE_H

#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

typedef struct{

    xmlNodePtr node;
    void * next;
}UD_XML_NODE;

typedef UD_XML_NODE * UD_XML_NODE_PTR;

int get_value_from_xml_string_by_tag( char * message, char * taghead, char * tagtail, char * value, int length );

xmlDocPtr get_doc_object_from_string( char * msgxml );
xmlNodePtr get_root_object_from_doc_object( xmlDocPtr docobj );
void free_doc_object( xmlDocPtr docobj );

UD_XML_NODE_PTR create_ud_xml_list_node( xmlNodePtr node );
void add_node_into_ud_xml_list( UD_XML_NODE_PTR * list, UD_XML_NODE_PTR node );
void free_ud_xml_list( UD_XML_NODE_PTR * list );

int get_objects_by_tagname( xmlNodePtr node, char * tagname, UD_XML_NODE_PTR * list );
int get_value_from_objects( UD_XML_NODE_PTR udnode, char * value, int length );


int get_value_from_sn( xmlNodePtr node, char * tagname, int index, char * value, int length );
char * dup_strip_xml_space( char * xmlmsg );

int get_objects_number_from_next_layer_xml_object_by_tagname( xmlNodePtr node, char * tagname );
xmlNodePtr get_objects_from_next_layer_xml_object_by_tagname( xmlNodePtr node, char * tagname, int index );
int get_value_from_next_layer_xml_object_by_tagname( xmlNodePtr node, char * tagname, int index, char * value, int length );


#endif//__XML_STRING_PARSE_H
