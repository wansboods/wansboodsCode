#ifndef __XML_API_H
#define __XML_API_H

#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

#include "systype.h"

xmlDocPtr get_doc_object_from_file( INT8_T * xmlfile );
xmlDocPtr get_doc_object_from_string( INT8_T * msgxml );
xmlNodePtr get_root_object_from_doc_object( xmlDocPtr docobj );
xmlNodePtr get_next_layer_object( xmlDocPtr xmlobj );
xmlNodePtr get_object_from_objlist( xmlNodePtr objlist, INT8_T * tagname, INT32_T index );
INT32_T get_objects_number_from_objlist_by_tagname( xmlNodePtr objlist, INT8_T * tagname );
INT32_T get_value_from_object_list_by_tagname_and_index( xmlNodePtr node, INT8_T * tagname, INT32_T index, INT8_T * value, INT32_T length );
VOID free_doc_object( xmlDocPtr docobj );
xmlNodePtr get_object_from_next_layer_xml_object_by_tagname( xmlNodePtr node, INT8_T * tagname, INT32_T index );
INT32_T get_value_from_next_layer_object_by_tagname_and_index( xmlNodePtr object, INT8_T * tagname, INT32_T index, INT8_T * value, INT32_T length );
INT32_T get_objects_number_from_next_layer_xml_object_by_tagname( xmlNodePtr node, INT8_T * tagname );


#endif//__XML_API_H
