/*
 * Summary: The DTD validation
 * Description: API for the DTD handling and the validity checking
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 */


#ifndef FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_VALID_H
#define FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_VALID_H

#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlversion.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlerror.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/tree.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/list.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlautomata.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlregexp.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/*
 * Validation state added for non-determinist content model.
 */
typedef struct _xmlValidState xmlValidState;
typedef xmlValidState *xmlValidStatePtr;

/**
 * xmlValidityErrorFunc:
 * @ctx:  an xmlValidCtxtPtr validity error context
 * @msg:  the string to format *printf like vararg
 * @...:  remaining arguments to the format
 *
 * Callback called when a validity error is found. This is a message
 * oriented function similar to an *printf function.
 */
typedef void (*xmlValidityErrorFunc) (void *ctx,
			     const char *msg,
			     ...);

/**
 * xmlValidityWarningFunc:
 * @ctx:  an xmlValidCtxtPtr validity error context
 * @msg:  the string to format *printf like vararg
 * @...:  remaining arguments to the format
 *
 * Callback called when a validity warning is found. This is a message
 * oriented function similar to an *printf function.
 */
typedef void (*xmlValidityWarningFunc) (void *ctx,
			       const char *msg,
			       ...);

/*
 * xmlValidCtxt:
 * An xmlValidCtxt is used for error reporting when validating.
 */
typedef struct _xmlValidCtxt xmlValidCtxt;
typedef xmlValidCtxt *xmlValidCtxtPtr;
struct _xmlValidCtxt {
    void *userData;			/* user specific data block */
    xmlValidityErrorFunc error;		/* the callback in case of errors */
    xmlValidityWarningFunc warning;	/* the callback in case of warning */

    /* Node analysis stack used when validating within entities */
    xmlNodePtr         node;          /* Current parsed Node */
    int                nodeNr;        /* Depth of the parsing stack */
    int                nodeMax;       /* Max depth of the parsing stack */
    xmlNodePtr        *nodeTab;       /* array of nodes */

    int              finishDtd;       /* finished validating the Dtd ? */
    xmlDocPtr              doc;       /* the document */
    int                  valid;       /* temporary validity check result */

    /* state state used for non-determinist content validation */
    xmlValidState     *vstate;        /* current state */
    int                vstateNr;      /* Depth of the validation stack */
    int                vstateMax;     /* Max depth of the validation stack */
    xmlValidState     *vstateTab;     /* array of validation states */

#ifdef LIBXML_REGEXP_ENABLED
    xmlAutomataPtr            am;     /* the automata */
    xmlAutomataStatePtr    state;     /* used to build the automata */
#else
    void                     *am;
    void                  *state;
#endif
};

/*
 * ALL notation declarations are stored in a table.
 * There is one table per DTD.
 */

typedef struct _xmlHashTable xmlNotationTable;
typedef xmlNotationTable *xmlNotationTablePtr;

/*
 * ALL element declarations are stored in a table.
 * There is one table per DTD.
 */

typedef struct _xmlHashTable xmlElementTable;
typedef xmlElementTable *xmlElementTablePtr;

/*
 * ALL attribute declarations are stored in a table.
 * There is one table per DTD.
 */

typedef struct _xmlHashTable xmlAttributeTable;
typedef xmlAttributeTable *xmlAttributeTablePtr;

/*
 * ALL IDs attributes are stored in a table.
 * There is one table per document.
 */

typedef struct _xmlHashTable xmlIDTable;
typedef xmlIDTable *xmlIDTablePtr;

/*
 * ALL Refs attributes are stored in a table.
 * There is one table per document.
 */

typedef struct _xmlHashTable xmlRefTable;
typedef xmlRefTable *xmlRefTablePtr;

/* Allocate/Release Validation Contexts */
XMLPUBFUN xmlValidCtxtPtr XMLCALL	    
		xmlNewValidCtxt(void);
XMLPUBFUN void XMLCALL		    
		xmlFreeValidCtxt(xmlValidCtxtPtr);

/* Notation */
XMLPUBFUN xmlNotationPtr XMLCALL	    
		xmlAddNotationDecl	(xmlValidCtxtPtr ctxt,
					 xmlDtdPtr dtd,
					 const xmlChar *name,
					 const xmlChar *PublicID,
					 const xmlChar *SystemID);
XMLPUBFUN xmlNotationTablePtr XMLCALL 
		xmlCopyNotationTable	(xmlNotationTablePtr table);
XMLPUBFUN void XMLCALL		    
		xmlFreeNotationTable	(xmlNotationTablePtr table);
#ifdef LIBXML_OUTPUT_ENABLED
XMLPUBFUN void XMLCALL		    
		xmlDumpNotationDecl	(xmlBufferPtr buf,
					 xmlNotationPtr nota);
XMLPUBFUN void XMLCALL		    
		xmlDumpNotationTable	(xmlBufferPtr buf,
					 xmlNotationTablePtr table);
#endif /* LIBXML_OUTPUT_ENABLED */

/* Element Content */
XMLPUBFUN xmlElementContentPtr XMLCALL 
		xmlNewElementContent	(const xmlChar *name,
					 xmlElementContentType type);
XMLPUBFUN xmlElementContentPtr XMLCALL 
		xmlCopyElementContent	(xmlElementContentPtr content);
XMLPUBFUN void XMLCALL		     
		xmlFreeElementContent	(xmlElementContentPtr cur);
XMLPUBFUN void XMLCALL		     
		xmlSnprintfElementContent(char *buf,
					 int size,
	                                 xmlElementContentPtr content,
					 int glob);
/* DEPRECATED */
XMLPUBFUN void XMLCALL		     
		xmlSprintfElementContent(char *buf,
	                                 xmlElementContentPtr content,
					 int glob);
/* DEPRECATED */

/* Element */
XMLPUBFUN xmlElementPtr XMLCALL	   
		xmlAddElementDecl	(xmlValidCtxtPtr ctxt,
					 xmlDtdPtr dtd,
					 const xmlChar *name,
					 xmlElementTypeVal type,
					 xmlElementContentPtr content);
XMLPUBFUN xmlElementTablePtr XMLCALL 
		xmlCopyElementTable	(xmlElementTablePtr table);
XMLPUBFUN void XMLCALL		   
		xmlFreeElementTable	(xmlElementTablePtr table);
#ifdef LIBXML_OUTPUT_ENABLED
XMLPUBFUN void XMLCALL		   
		xmlDumpElementTable	(xmlBufferPtr buf,
					 xmlElementTablePtr table);
XMLPUBFUN void XMLCALL		   
		xmlDumpElementDecl	(xmlBufferPtr buf,
					 xmlElementPtr elem);
#endif /* LIBXML_OUTPUT_ENABLED */

/* Enumeration */
XMLPUBFUN xmlEnumerationPtr XMLCALL 
		xmlCreateEnumeration	(const xmlChar *name);
XMLPUBFUN void XMLCALL		   
		xmlFreeEnumeration	(xmlEnumerationPtr cur);
XMLPUBFUN xmlEnumerationPtr XMLCALL  
		xmlCopyEnumeration	(xmlEnumerationPtr cur);

/* Attribute */
XMLPUBFUN xmlAttributePtr XMLCALL	    
		xmlAddAttributeDecl	(xmlValidCtxtPtr ctxt,
					 xmlDtdPtr dtd,
					 const xmlChar *elem,
					 const xmlChar *name,
					 const xmlChar *ns,
					 xmlAttributeType type,
					 xmlAttributeDefault def,
					 const xmlChar *defaultValue,
					 xmlEnumerationPtr tree);
XMLPUBFUN xmlAttributeTablePtr XMLCALL 
		xmlCopyAttributeTable  (xmlAttributeTablePtr table);
XMLPUBFUN void XMLCALL		     
		xmlFreeAttributeTable  (xmlAttributeTablePtr table);
#ifdef LIBXML_OUTPUT_ENABLED
XMLPUBFUN void XMLCALL		     
		xmlDumpAttributeTable  (xmlBufferPtr buf,
					xmlAttributeTablePtr table);
XMLPUBFUN void XMLCALL		     
		xmlDumpAttributeDecl   (xmlBufferPtr buf,
					xmlAttributePtr attr);
#endif /* LIBXML_OUTPUT_ENABLED */

/* IDs */
XMLPUBFUN xmlIDPtr XMLCALL	
		xmlAddID	       (xmlValidCtxtPtr ctxt,
					xmlDocPtr doc,
					const xmlChar *value,
					xmlAttrPtr attr);
XMLPUBFUN void XMLCALL		
		xmlFreeIDTable	       (xmlIDTablePtr table);
XMLPUBFUN xmlAttrPtr XMLCALL	
		xmlGetID	       (xmlDocPtr doc,
					const xmlChar *ID);
XMLPUBFUN int XMLCALL		
		xmlIsID		       (xmlDocPtr doc,
					xmlNodePtr elem,
					xmlAttrPtr attr);
XMLPUBFUN int XMLCALL		
		xmlRemoveID	       (xmlDocPtr doc, 
					xmlAttrPtr attr);

/* IDREFs */
XMLPUBFUN xmlRefPtr XMLCALL	
		xmlAddRef	       (xmlValidCtxtPtr ctxt,
					xmlDocPtr doc,
					const xmlChar *value,
					xmlAttrPtr attr);
XMLPUBFUN void XMLCALL		
		xmlFreeRefTable	       (xmlRefTablePtr table);
XMLPUBFUN int XMLCALL		
		xmlIsRef	       (xmlDocPtr doc,
					xmlNodePtr elem,
					xmlAttrPtr attr);
XMLPUBFUN int XMLCALL		
		xmlRemoveRef	       (xmlDocPtr doc, 
					xmlAttrPtr attr);
XMLPUBFUN xmlListPtr XMLCALL	
		xmlGetRefs	       (xmlDocPtr doc,
					const xmlChar *ID);

/**
 * The public function calls related to validity checking.
 */

XMLPUBFUN int XMLCALL		
		xmlValidateRoot		(xmlValidCtxtPtr ctxt,
					 xmlDocPtr doc);
XMLPUBFUN int XMLCALL		
		xmlValidateElementDecl	(xmlValidCtxtPtr ctxt,
					 xmlDocPtr doc,
		                         xmlElementPtr elem);
XMLPUBFUN xmlChar * XMLCALL	
		xmlValidNormalizeAttributeValue(xmlDocPtr doc,
					 xmlNodePtr elem,
					 const xmlChar *name,
					 const xmlChar *value);
XMLPUBFUN xmlChar * XMLCALL	
		xmlValidCtxtNormalizeAttributeValue(xmlValidCtxtPtr ctxt,
					 xmlDocPtr doc,
					 xmlNodePtr elem,
					 const xmlChar *name,
					 const xmlChar *value);
XMLPUBFUN int XMLCALL		
		xmlValidateAttributeDecl(xmlValidCtxtPtr ctxt,
					 xmlDocPtr doc,
		                         xmlAttributePtr attr);
XMLPUBFUN int XMLCALL		
		xmlValidateAttributeValue(xmlAttributeType type,
					 const xmlChar *value);
XMLPUBFUN int XMLCALL		
		xmlValidateNotationDecl	(xmlValidCtxtPtr ctxt,
					 xmlDocPtr doc,
		                         xmlNotationPtr nota);
XMLPUBFUN int XMLCALL		
		xmlValidateDtd		(xmlValidCtxtPtr ctxt,
					 xmlDocPtr doc,
					 xmlDtdPtr dtd);
XMLPUBFUN int XMLCALL		
		xmlValidateDtdFinal	(xmlValidCtxtPtr ctxt,
					 xmlDocPtr doc);
XMLPUBFUN int XMLCALL		
		xmlValidateDocument	(xmlValidCtxtPtr ctxt,
					 xmlDocPtr doc);
XMLPUBFUN int XMLCALL		
		xmlValidateElement	(xmlValidCtxtPtr ctxt,
					 xmlDocPtr doc,
					 xmlNodePtr elem);
XMLPUBFUN int XMLCALL		
		xmlValidateOneElement	(xmlValidCtxtPtr ctxt,
					 xmlDocPtr doc,
		                         xmlNodePtr elem);
XMLPUBFUN int XMLCALL	
		xmlValidateOneAttribute	(xmlValidCtxtPtr ctxt,
					 xmlDocPtr doc,
					 xmlNodePtr	elem,
					 xmlAttrPtr attr,
					 const xmlChar *value);
XMLPUBFUN int XMLCALL		
		xmlValidateOneNamespace	(xmlValidCtxtPtr ctxt,
					 xmlDocPtr doc,
					 xmlNodePtr elem,
					 const xmlChar *prefix,
					 xmlNsPtr ns,
					 const xmlChar *value);
XMLPUBFUN int XMLCALL		
		xmlValidateDocumentFinal(xmlValidCtxtPtr ctxt,
					 xmlDocPtr doc);
XMLPUBFUN int XMLCALL		
		xmlValidateNotationUse	(xmlValidCtxtPtr ctxt,
					 xmlDocPtr doc,
					 const xmlChar *notationName);
XMLPUBFUN int XMLCALL		
		xmlIsMixedElement	(xmlDocPtr doc,
					 const xmlChar *name);
XMLPUBFUN xmlAttributePtr XMLCALL	
		xmlGetDtdAttrDesc	(xmlDtdPtr dtd,
					 const xmlChar *elem,
					 const xmlChar *name);
XMLPUBFUN xmlAttributePtr XMLCALL	
		xmlGetDtdQAttrDesc	(xmlDtdPtr dtd,
					 const xmlChar *elem,
					 const xmlChar *name,
					 const xmlChar *prefix);
XMLPUBFUN xmlNotationPtr XMLCALL	
		xmlGetDtdNotationDesc	(xmlDtdPtr dtd,
					 const xmlChar *name);
XMLPUBFUN xmlElementPtr XMLCALL	
		xmlGetDtdQElementDesc	(xmlDtdPtr dtd,
					 const xmlChar *name,
					 const xmlChar *prefix);
XMLPUBFUN xmlElementPtr XMLCALL	
		xmlGetDtdElementDesc	(xmlDtdPtr dtd,
					 const xmlChar *name);

XMLPUBFUN int XMLCALL		
		xmlValidGetValidElements(xmlNode *prev,
					 xmlNode *next,
					 const xmlChar **list,
					 int max);
XMLPUBFUN int XMLCALL		
		xmlValidGetPotentialChildren(xmlElementContent *ctree,
					 const xmlChar **list,
					 int *len,
					 int max);
XMLPUBFUN int XMLCALL		
		xmlValidateNameValue	(const xmlChar *value);
XMLPUBFUN int XMLCALL		
		xmlValidateNamesValue	(const xmlChar *value);
XMLPUBFUN int XMLCALL		
		xmlValidateNmtokenValue	(const xmlChar *value);
XMLPUBFUN int XMLCALL		
		xmlValidateNmtokensValue(const xmlChar *value);

#ifdef LIBXML_REGEXP_ENABLED
/*
 * Validation based on the regexp support
 */
XMLPUBFUN int XMLCALL		
		xmlValidBuildContentModel(xmlValidCtxtPtr ctxt,
					 xmlElementPtr elem);

XMLPUBFUN int XMLCALL		
		xmlValidatePushElement	(xmlValidCtxtPtr ctxt,
					 xmlDocPtr doc,
					 xmlNodePtr elem,
					 const xmlChar *qname);
XMLPUBFUN int XMLCALL		
		xmlValidatePushCData	(xmlValidCtxtPtr ctxt,
					 const xmlChar *data,
					 int len);
XMLPUBFUN int XMLCALL		
		xmlValidatePopElement	(xmlValidCtxtPtr ctxt,
					 xmlDocPtr doc,
					 xmlNodePtr elem,
					 const xmlChar *qname);
#endif /* LIBXML_REGEXP_ENABLED */

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_VALID_H

