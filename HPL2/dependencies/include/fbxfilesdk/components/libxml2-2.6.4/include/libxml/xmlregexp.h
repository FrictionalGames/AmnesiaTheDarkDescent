/*
 * Summary: regular expressions handling
 * Description: basic API for libxml regular expressions handling used
 *              for XML Schemas and validation.
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 */

#ifndef FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_XMLREGEXP_H
#define FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_XMLREGEXP_H

#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlversion.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

#ifdef LIBXML_REGEXP_ENABLED

/**
 * xmlRegexpPtr:
 *
 * A libxml regular expression, they can actually be far more complex
 * thank the POSIX regex expressions.
 */
typedef struct _xmlRegexp xmlRegexp;
typedef xmlRegexp *xmlRegexpPtr;

/**
 * xmlRegExecCtxtPtr:
 *
 * A libxml progressive regular expression evaluation context
 */
typedef struct _xmlRegExecCtxt xmlRegExecCtxt;
typedef xmlRegExecCtxt *xmlRegExecCtxtPtr;

// PJT: tree.h is dependent on this file. Don't make it dependent
// on tree.h too! 04/02/04
// #include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/tree.h>

/*
 * The POSIX like API
 */
XMLPUBFUN xmlRegexpPtr XMLCALL
		    xmlRegexpCompile	(const xmlChar *regexp);
XMLPUBFUN void XMLCALL			 xmlRegFreeRegexp(xmlRegexpPtr regexp);
XMLPUBFUN int XMLCALL			
		    xmlRegexpExec	(xmlRegexpPtr comp,
					 const xmlChar *value);
XMLPUBFUN void XMLCALL			
    		    xmlRegexpPrint	(FILE *output,
					 xmlRegexpPtr regexp);
XMLPUBFUN int XMLCALL			
		    xmlRegexpIsDeterminist(xmlRegexpPtr comp);

/*
 * Callback function when doing a transition in the automata
 */
typedef void (*xmlRegExecCallbacks) (xmlRegExecCtxtPtr exec,
	                             const xmlChar *token,
				     void *transdata,
				     void *inputdata);

/*
 * The progressive API
 */
XMLPUBFUN xmlRegExecCtxtPtr XMLCALL	
    		    xmlRegNewExecCtxt	(xmlRegexpPtr comp,
					 xmlRegExecCallbacks callback,
					 void *data);
XMLPUBFUN void XMLCALL			
		    xmlRegFreeExecCtxt	(xmlRegExecCtxtPtr exec);
XMLPUBFUN int XMLCALL			
    		    xmlRegExecPushString(xmlRegExecCtxtPtr exec,
					 const xmlChar *value,
					 void *data);
XMLPUBFUN int XMLCALL			
		    xmlRegExecPushString2(xmlRegExecCtxtPtr exec,
					 const xmlChar *value,
					 const xmlChar *value2,
					 void *data);

#endif /* LIBXML_REGEXP_ENABLED */

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_XMLREGEXP_H

