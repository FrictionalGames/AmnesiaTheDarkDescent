/*
 * Summary: string dictionnary
 * Description: dictionary of reusable strings, just used to avoid allocation
 *         and freeing operations.
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 */

#ifndef FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_DICT_H
#define FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_DICT_H

#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlversion.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/tree.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/*
 * The dictionnary.
 */
typedef struct _xmlDict xmlDict;
typedef xmlDict *xmlDictPtr;

/*
 * Constructor and destructor.
 */
XMLPUBFUN xmlDictPtr XMLCALL
			xmlDictCreate	(void);
XMLPUBFUN int XMLCALL
			xmlDictReference(xmlDictPtr dict);
XMLPUBFUN void XMLCALL			
			xmlDictFree	(xmlDictPtr dict);

/*
 * Lookup of entry in the dictionnary.
 */
XMLPUBFUN const xmlChar * XMLCALL		
			xmlDictLookup	(xmlDictPtr dict,
		                         const xmlChar *name,
		                         int len);
XMLPUBFUN const xmlChar * XMLCALL		
			xmlDictQLookup	(xmlDictPtr dict,
		                         const xmlChar *prefix,
		                         const xmlChar *name);
XMLPUBFUN int XMLCALL
			xmlDictOwns	(xmlDictPtr dict,
					 const xmlChar *str);
XMLPUBFUN int XMLCALL			
			xmlDictSize	(xmlDictPtr dict);

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_DICT_H

