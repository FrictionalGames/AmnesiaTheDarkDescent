/*
 * Summary: API to build regexp automata
 * Description: the API to build regexp automata
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 */

#ifndef FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_XMLAUTOMATA_H
#define FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_XMLAUTOMATA_H

#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlversion.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/tree.h>

#ifdef LIBXML_AUTOMATA_ENABLED
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlregexp.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/**
 * xmlAutomataPtr:
 *
 * A libxml automata description, It can be compiled into a regexp
 */
typedef struct _xmlAutomata xmlAutomata;
typedef xmlAutomata *xmlAutomataPtr;

/**
 * xmlAutomataStatePtr:
 *
 * A state int the automata description,
 */
typedef struct _xmlAutomataState xmlAutomataState;
typedef xmlAutomataState *xmlAutomataStatePtr;

/*
 * Building API
 */
XMLPUBFUN xmlAutomataPtr XMLCALL		
		    xmlNewAutomata		(void);
XMLPUBFUN void XMLCALL			
		    xmlFreeAutomata		(xmlAutomataPtr am);

XMLPUBFUN xmlAutomataStatePtr XMLCALL	
		    xmlAutomataGetInitState	(xmlAutomataPtr am);
XMLPUBFUN int XMLCALL			
		    xmlAutomataSetFinalState	(xmlAutomataPtr am,
						 xmlAutomataStatePtr state);
XMLPUBFUN xmlAutomataStatePtr XMLCALL	
		    xmlAutomataNewState		(xmlAutomataPtr am);
XMLPUBFUN xmlAutomataStatePtr XMLCALL	
		    xmlAutomataNewTransition	(xmlAutomataPtr am,
						 xmlAutomataStatePtr from,
						 xmlAutomataStatePtr to,
						 const xmlChar *token,
						 void *data);
XMLPUBFUN xmlAutomataStatePtr XMLCALL	
		    xmlAutomataNewTransition2	(xmlAutomataPtr am,
						 xmlAutomataStatePtr from,
						 xmlAutomataStatePtr to,
						 const xmlChar *token,
						 const xmlChar *token2,
						 void *data);
XMLPUBFUN xmlAutomataStatePtr XMLCALL	
		    xmlAutomataNewCountTrans	(xmlAutomataPtr am,
						 xmlAutomataStatePtr from,
						 xmlAutomataStatePtr to,
						 const xmlChar *token,
						 int min,
						 int max,
						 void *data);
XMLPUBFUN xmlAutomataStatePtr XMLCALL	
		    xmlAutomataNewOnceTrans	(xmlAutomataPtr am,
						 xmlAutomataStatePtr from,
						 xmlAutomataStatePtr to,
						 const xmlChar *token,
						 int min,
						 int max,
						 void *data);
XMLPUBFUN xmlAutomataStatePtr XMLCALL	
		    xmlAutomataNewAllTrans	(xmlAutomataPtr am,
						 xmlAutomataStatePtr from,
						 xmlAutomataStatePtr to,
						 int lax);
XMLPUBFUN xmlAutomataStatePtr XMLCALL	
		    xmlAutomataNewEpsilon	(xmlAutomataPtr am,
						 xmlAutomataStatePtr from,
						 xmlAutomataStatePtr to);
XMLPUBFUN xmlAutomataStatePtr XMLCALL	
		    xmlAutomataNewCountedTrans	(xmlAutomataPtr am,
						 xmlAutomataStatePtr from,
						 xmlAutomataStatePtr to,
						 int counter);
XMLPUBFUN xmlAutomataStatePtr XMLCALL	
		    xmlAutomataNewCounterTrans	(xmlAutomataPtr am,
						 xmlAutomataStatePtr from,
						 xmlAutomataStatePtr to,
						 int counter);
XMLPUBFUN int XMLCALL			
		    xmlAutomataNewCounter	(xmlAutomataPtr am,
						 int min,
						 int max);

XMLPUBFUN xmlRegexpPtr XMLCALL		
		    xmlAutomataCompile		(xmlAutomataPtr am);
XMLPUBFUN int XMLCALL	    		
		    xmlAutomataIsDeterminist	(xmlAutomataPtr am);

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif /* LIBXML_AUTOMATA_ENABLED */
#endif // FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_XMLAUTOMATA_H

