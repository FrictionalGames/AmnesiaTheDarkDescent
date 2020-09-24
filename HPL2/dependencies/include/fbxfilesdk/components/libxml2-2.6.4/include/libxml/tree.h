/*
 * Summary: interfaces for tree manipulation
 * Description: this module describes the structures found in an tree resulting
 *              from an XML or HTML parsing, as well as the API provided for
 *              various processing on that tree
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 */

#ifndef FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_TREE_H
#define FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_TREE_H

#include <stdio.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlversion.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/*
 * Some of the basic types pointer to structures:
 */
/* xmlIO.h */
typedef struct _xmlParserInputBuffer xmlParserInputBuffer;
typedef xmlParserInputBuffer *xmlParserInputBufferPtr;

typedef struct _xmlOutputBuffer xmlOutputBuffer;
typedef xmlOutputBuffer *xmlOutputBufferPtr;

/* parser.h */
typedef struct _xmlParserInput xmlParserInput;
typedef xmlParserInput *xmlParserInputPtr;

typedef struct _xmlParserCtxt xmlParserCtxt;
typedef xmlParserCtxt *xmlParserCtxtPtr;

typedef struct _xmlSAXLocator xmlSAXLocator;
typedef xmlSAXLocator *xmlSAXLocatorPtr;

typedef struct _xmlSAXHandler xmlSAXHandler;
typedef xmlSAXHandler *xmlSAXHandlerPtr;

/* entities.h */
typedef struct _xmlEntity xmlEntity;
typedef xmlEntity *xmlEntityPtr;

/**
 * BASE_BUFFER_SIZE:
 *
 * default buffer size 4000.
 */
#define BASE_BUFFER_SIZE 4096

/**
 * XML_XML_NAMESPACE:
 *
 * This is the namespace for the special xml: prefix predefined in the
 * XML Namespace specification.
 */
#define XML_XML_NAMESPACE \
    (const xmlChar *) "http://www.w3.org/XML/1998/namespace"

/*
 * The different element types carried by an XML tree.
 *
 * NOTE: This is synchronized with DOM Level1 values
 *       See http://www.w3.org/TR/REC-DOM-Level-1/
 *
 * Actually this had diverged a bit, and now XML_DOCUMENT_TYPE_NODE should
 * be deprecated to use an XML_DTD_NODE.
 */
typedef enum {
    XML_ELEMENT_NODE=		1,
    XML_ATTRIBUTE_NODE=		2,
    XML_TEXT_NODE=		3,
    XML_CDATA_SECTION_NODE=	4,
    XML_ENTITY_REF_NODE=	5,
    XML_ENTITY_NODE=		6,
    XML_PI_NODE=		7,
    XML_COMMENT_NODE=		8,
    XML_DOCUMENT_NODE=		9,
    XML_DOCUMENT_TYPE_NODE=	10,
    XML_DOCUMENT_FRAG_NODE=	11,
    XML_NOTATION_NODE=		12,
    XML_HTML_DOCUMENT_NODE=	13,
    XML_DTD_NODE=		14,
    XML_ELEMENT_DECL=		15,
    XML_ATTRIBUTE_DECL=		16,
    XML_ENTITY_DECL=		17,
    XML_NAMESPACE_DECL=		18,
    XML_XINCLUDE_START=		19,
    XML_XINCLUDE_END=		20
#ifdef LIBXML_DOCB_ENABLED
   ,XML_DOCB_DOCUMENT_NODE=	21
#endif
} xmlElementType;

/**
 * xmlChar:
 *
 * This is a basic byte in an UTF-8 encoded string.
 * It's unsigned allowing to pinpoint case where char * are assigned
 * to xmlChar * (possibly making serialization back impossible).
 */

typedef unsigned char xmlChar;

/**
 * BAD_CAST:
 *
 * Macro to cast a string to an xmlChar * when one know its safe.
 */
#define BAD_CAST (xmlChar *)

/**
 * xmlNotation:
 *
 * A DTD Notation definition.
 */

typedef struct _xmlNotation xmlNotation;
typedef xmlNotation *xmlNotationPtr;
struct _xmlNotation {
    const xmlChar               *name;	        /* Notation name */
    const xmlChar               *PublicID;	/* Public identifier, if any */
    const xmlChar               *SystemID;	/* System identifier, if any */
};

/**
 * xmlAttributeType:
 *
 * A DTD Attribute type definition.
 */

typedef enum {
    XML_ATTRIBUTE_CDATA = 1,
    XML_ATTRIBUTE_ID,
    XML_ATTRIBUTE_IDREF	,
    XML_ATTRIBUTE_IDREFS,
    XML_ATTRIBUTE_ENTITY,
    XML_ATTRIBUTE_ENTITIES,
    XML_ATTRIBUTE_NMTOKEN,
    XML_ATTRIBUTE_NMTOKENS,
    XML_ATTRIBUTE_ENUMERATION,
    XML_ATTRIBUTE_NOTATION
} xmlAttributeType;

/**
 * xmlAttributeDefault:
 *
 * A DTD Attribute default definition.
 */

typedef enum {
    XML_ATTRIBUTE_NONE = 1,
    XML_ATTRIBUTE_REQUIRED,
    XML_ATTRIBUTE_IMPLIED,
    XML_ATTRIBUTE_FIXED
} xmlAttributeDefault;

/**
 * xmlEnumeration:
 *
 * List structure used when there is an enumeration in DTDs.
 */

typedef struct _xmlEnumeration xmlEnumeration;
typedef xmlEnumeration *xmlEnumerationPtr;
struct _xmlEnumeration {
    struct _xmlEnumeration    *next;	/* next one */
    const xmlChar            *name;	/* Enumeration name */
};

/**
 * xmlAttribute:
 *
 * An Attribute declaration in a DTD.
 */

typedef struct _xmlAttribute xmlAttribute;
typedef xmlAttribute *xmlAttributePtr;
struct _xmlAttribute {
    void           *_private;	        /* application data */
    xmlElementType          type;       /* XML_ATTRIBUTE_DECL, must be second ! */
    const xmlChar          *name;	/* Attribute name */
    struct _xmlNode    *children;	/* NULL */
    struct _xmlNode        *last;	/* NULL */
    struct _xmlDtd       *parent;	/* -> DTD */
    struct _xmlNode        *next;	/* next sibling link  */
    struct _xmlNode        *prev;	/* previous sibling link  */
    struct _xmlDoc          *doc;       /* the containing document */

    struct _xmlAttribute  *nexth;	/* next in hash table */
    xmlAttributeType       atype;	/* The attribute type */
    xmlAttributeDefault      def;	/* the default */
    const xmlChar  *defaultValue;	/* or the default value */
    xmlEnumerationPtr       tree;       /* or the enumeration tree if any */
    const xmlChar        *prefix;	/* the namespace prefix if any */
    const xmlChar          *elem;	/* Element holding the attribute */
};

/**
 * xmlElementContentType:
 *
 * Possible definitions of element content types.
 */
typedef enum {
    XML_ELEMENT_CONTENT_PCDATA = 1,
    XML_ELEMENT_CONTENT_ELEMENT,
    XML_ELEMENT_CONTENT_SEQ,
    XML_ELEMENT_CONTENT_OR
} xmlElementContentType;

/**
 * xmlElementContentOccur:
 *
 * Possible definitions of element content occurrences.
 */
typedef enum {
    XML_ELEMENT_CONTENT_ONCE = 1,
    XML_ELEMENT_CONTENT_OPT,
    XML_ELEMENT_CONTENT_MULT,
    XML_ELEMENT_CONTENT_PLUS
} xmlElementContentOccur;

/**
 * xmlElementContent:
 *
 * An XML Element content as stored after parsing an element definition
 * in a DTD.
 */

typedef struct _xmlElementContent xmlElementContent;
typedef xmlElementContent *xmlElementContentPtr;
struct _xmlElementContent {
    xmlElementContentType     type;	/* PCDATA, ELEMENT, SEQ or OR */
    xmlElementContentOccur    ocur;	/* ONCE, OPT, MULT or PLUS */
    const xmlChar             *name;	/* Element name */
    struct _xmlElementContent *c1;	/* first child */
    struct _xmlElementContent *c2;	/* second child */
    struct _xmlElementContent *parent;	/* parent */
    const xmlChar             *prefix;	/* Namespace prefix */
};

/**
 * xmlElementTypeVal:
 *
 * The different possibilities for an element content type.
 */

typedef enum {
    XML_ELEMENT_TYPE_UNDEFINED = 0,
    XML_ELEMENT_TYPE_EMPTY = 1,
    XML_ELEMENT_TYPE_ANY,
    XML_ELEMENT_TYPE_MIXED,
    XML_ELEMENT_TYPE_ELEMENT
} xmlElementTypeVal;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlregexp.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/**
 * xmlElement:
 *
 * An XML Element declaration from a DTD.
 */

typedef struct _xmlElement xmlElement;
typedef xmlElement *xmlElementPtr;
struct _xmlElement {
    void           *_private;	        /* application data */
    xmlElementType          type;       /* XML_ELEMENT_DECL, must be second ! */
    const xmlChar          *name;	/* Element name */
    struct _xmlNode    *children;	/* NULL */
    struct _xmlNode        *last;	/* NULL */
    struct _xmlDtd       *parent;	/* -> DTD */
    struct _xmlNode        *next;	/* next sibling link  */
    struct _xmlNode        *prev;	/* previous sibling link  */
    struct _xmlDoc          *doc;       /* the containing document */

    xmlElementTypeVal      etype;	/* The type */
    xmlElementContentPtr content;	/* the allowed element content */
    xmlAttributePtr   attributes;	/* List of the declared attributes */
    const xmlChar        *prefix;	/* the namespace prefix if any */
#ifdef LIBXML_REGEXP_ENABLED
    xmlRegexpPtr       contModel;	/* the validating regexp */
#else
    void	      *contModel;
#endif
};


/**
 * XML_LOCAL_NAMESPACE:
 *
 * A namespace declaration node.
 */
#define XML_LOCAL_NAMESPACE XML_NAMESPACE_DECL
typedef xmlElementType xmlNsType;

/**
 * xmlNs:
 *
 * An XML namespace.
 * Note that prefix == NULL is valid, it defines the default namespace
 * within the subtree (until overridden).
 *
 * xmlNsType is unified with xmlElementType.
 */

typedef struct _xmlNs xmlNs;
typedef xmlNs *xmlNsPtr;
struct _xmlNs {
    struct _xmlNs  *next;	/* next Ns link for this node  */
    xmlNsType      type;	/* global or local */
    const xmlChar *href;	/* URL for the namespace */
    const xmlChar *prefix;	/* prefix for the namespace */
    void           *_private;   /* application data */
};

/**
 * xmlDtd:
 *
 * An XML DTD, as defined by <!DOCTYPE ... There is actually one for
 * the internal subset and for the external subset.
 */
typedef struct _xmlDtd xmlDtd;
typedef xmlDtd *xmlDtdPtr;
struct _xmlDtd {
    void           *_private;	/* application data */
    xmlElementType  type;       /* XML_DTD_NODE, must be second ! */
    const xmlChar *name;	/* Name of the DTD */
    struct _xmlNode *children;	/* the value of the property link */
    struct _xmlNode *last;	/* last child link */
    struct _xmlDoc  *parent;	/* child->parent link */
    struct _xmlNode *next;	/* next sibling link  */
    struct _xmlNode *prev;	/* previous sibling link  */
    struct _xmlDoc  *doc;	/* the containing document */

    /* End of common part */
    void          *notations;   /* Hash table for notations if any */
    void          *elements;    /* Hash table for elements if any */
    void          *attributes;  /* Hash table for attributes if any */
    void          *entities;    /* Hash table for entities if any */
    const xmlChar *ExternalID;	/* External identifier for PUBLIC DTD */
    const xmlChar *SystemID;	/* URI for a SYSTEM or PUBLIC DTD */
    void          *pentities;   /* Hash table for param entities if any */
};

/**
 * xmlAttr:
 *
 * An attribute on an XML node.
 */
typedef struct _xmlAttr xmlAttr;
typedef xmlAttr *xmlAttrPtr;
struct _xmlAttr {
    void           *_private;	/* application data */
    xmlElementType   type;      /* XML_ATTRIBUTE_NODE, must be second ! */
    const xmlChar   *name;      /* the name of the property */
    struct _xmlNode *children;	/* the value of the property */
    struct _xmlNode *last;	/* NULL */
    struct _xmlNode *parent;	/* child->parent link */
    struct _xmlAttr *next;	/* next sibling link  */
    struct _xmlAttr *prev;	/* previous sibling link  */
    struct _xmlDoc  *doc;	/* the containing document */
    xmlNs           *ns;        /* pointer to the associated namespace */
    xmlAttributeType atype;     /* the attribute type if validating */
    void            *psvi;	/* for type/PSVI informations */
};

/**
 * xmlID:
 *
 * An XML ID instance.
 */

typedef struct _xmlID xmlID;
typedef xmlID *xmlIDPtr;
struct _xmlID {
    struct _xmlID    *next;	/* next ID */
    const xmlChar    *value;	/* The ID name */
    xmlAttrPtr        attr;	/* The attribute holding it */
    const xmlChar    *name;	/* The attribute if attr is not available */
    int               lineno;	/* The line number if attr is not available */
    struct _xmlDoc   *doc;	/* The document holding the ID */
};

/**
 * xmlRef:
 *
 * An XML IDREF instance.
 */

typedef struct _xmlRef xmlRef;
typedef xmlRef *xmlRefPtr;
struct _xmlRef {
    struct _xmlRef    *next;	/* next Ref */
    const xmlChar     *value;	/* The Ref name */
    xmlAttrPtr        attr;	/* The attribute holding it */
    const xmlChar    *name;	/* The attribute if attr is not available */
    int               lineno;	/* The line number if attr is not available */
};

/**
 * xmlBufferAllocationScheme:
 *
 * A buffer allocation scheme can be defined to either match exactly the
 * need or double it's allocated size each time it is found too small.
 */

typedef enum {
    XML_BUFFER_ALLOC_DOUBLEIT,
    XML_BUFFER_ALLOC_EXACT,
    XML_BUFFER_ALLOC_IMMUTABLE
} xmlBufferAllocationScheme;

/**
 * xmlBuffer:
 *
 * A buffer structure.
 */
typedef struct _xmlBuffer xmlBuffer;
typedef xmlBuffer *xmlBufferPtr;
struct _xmlBuffer {
    xmlChar *content;		/* The buffer content UTF8 */
    unsigned int use;		/* The buffer size used */
    unsigned int size;		/* The buffer size */
    xmlBufferAllocationScheme alloc; /* The realloc method */
};

/**
 * xmlNode:
 *
 * A node in an XML tree.
 */
typedef struct _xmlNode xmlNode;
typedef xmlNode *xmlNodePtr;
struct _xmlNode {
    void           *_private;	/* application data */
    xmlElementType   type;	/* type number, must be second ! */
    const xmlChar   *name;      /* the name of the node, or the entity */
    struct _xmlNode *children;	/* parent->childs link */
    struct _xmlNode *last;	/* last child link */
    struct _xmlNode *parent;	/* child->parent link */
    struct _xmlNode *next;	/* next sibling link  */
    struct _xmlNode *prev;	/* previous sibling link  */
    struct _xmlDoc  *doc;	/* the containing document */

    /* End of common part */
    xmlNs           *ns;        /* pointer to the associated namespace */
    xmlChar         *content;   /* the content */
    struct _xmlAttr *properties;/* properties list */
    xmlNs           *nsDef;     /* namespace definitions on this node */
    void            *psvi;	/* for type/PSVI informations */
    unsigned short   line;	/* line number */
    unsigned short   extra;	/* extra data for XPath/XSLT */
};

/**
 * XML_GET_CONTENT:
 *
 * Macro to extract the content pointer of a node.
 */
#define XML_GET_CONTENT(n)					\
    ((n)->type == XML_ELEMENT_NODE ? NULL : (n)->content)

/**
 * XML_GET_LINE:
 *
 * Macro to extract the line number of an element node. 
 */
#define XML_GET_LINE(n)						\
    (xmlGetLineNo(n))


/**
 * xmlDoc:
 *
 * An XML document.
 */
typedef struct _xmlDoc xmlDoc;
typedef xmlDoc *xmlDocPtr;
struct _xmlDoc {
    void           *_private;	/* application data */
    xmlElementType  type;       /* XML_DOCUMENT_NODE, must be second ! */
    char           *name;	/* name/filename/URI of the document */
    struct _xmlNode *children;	/* the document tree */
    struct _xmlNode *last;	/* last child link */
    struct _xmlNode *parent;	/* child->parent link */
    struct _xmlNode *next;	/* next sibling link  */
    struct _xmlNode *prev;	/* previous sibling link  */
    struct _xmlDoc  *doc;	/* autoreference to itself */

    /* End of common part */
    int             compression;/* level of zlib compression */
    int             standalone; /* standalone document (no external refs) */
    struct _xmlDtd  *intSubset;	/* the document internal subset */
    struct _xmlDtd  *extSubset;	/* the document external subset */
    struct _xmlNs   *oldNs;	/* Global namespace, the old way */
    const xmlChar  *version;	/* the XML version string */
    const xmlChar  *encoding;   /* external initial encoding, if any */
    void           *ids;        /* Hash table for ID attributes if any */
    void           *refs;       /* Hash table for IDREFs attributes if any */
    const xmlChar  *URL;	/* The URI for that document */
    int             charset;    /* encoding of the in-memory content
				   actually an xmlCharEncoding */
    struct _xmlDict *dict;      /* dict used to allocate names or NULL */
    void           *psvi;	/* for type/PSVI informations */
};

/**
 * xmlChildrenNode:
 *
 * Macro for compatibility naming layer with libxml1. Maps
 * to "children."
 */
#ifndef xmlChildrenNode
#define xmlChildrenNode children
#endif

/**
 * xmlRootNode:
 *
 * Macro for compatibility naming layer with libxml1. Maps 
 * to "children".
 */
#ifndef xmlRootNode
#define xmlRootNode children
#endif

/*
 * Variables.
 */

/*
 * Some helper functions
 */
XMLPUBFUN int XMLCALL
		xmlValidateNCName	(const xmlChar *value,
					 int space);
XMLPUBFUN int XMLCALL		
		xmlValidateQName	(const xmlChar *value,
					 int space);
XMLPUBFUN int XMLCALL		
		xmlValidateName		(const xmlChar *value,
					 int space);
XMLPUBFUN int XMLCALL		
		xmlValidateNMToken	(const xmlChar *value,
					 int space);

XMLPUBFUN xmlChar * XMLCALL	
		xmlBuildQName		(const xmlChar *ncname,
					 const xmlChar *prefix,
					 xmlChar *memory,
					 int len);
XMLPUBFUN xmlChar * XMLCALL	
		xmlSplitQName2		(const xmlChar *name,
					 xmlChar **prefix);
XMLPUBFUN const xmlChar * XMLCALL	
		xmlSplitQName3		(const xmlChar *name,
					 int *len);

/*
 * Handling Buffers.
 */

XMLPUBFUN void XMLCALL		
		xmlSetBufferAllocationScheme(xmlBufferAllocationScheme scheme);
XMLPUBFUN xmlBufferAllocationScheme XMLCALL	 
		xmlGetBufferAllocationScheme(void);

XMLPUBFUN xmlBufferPtr XMLCALL	
		xmlBufferCreate		(void);
XMLPUBFUN xmlBufferPtr XMLCALL	
		xmlBufferCreateSize	(size_t size);
XMLPUBFUN xmlBufferPtr XMLCALL	
		xmlBufferCreateStatic	(void *mem,
					 size_t size);
XMLPUBFUN int XMLCALL		
		xmlBufferResize		(xmlBufferPtr buf,
					 unsigned int size);
XMLPUBFUN void XMLCALL		
		xmlBufferFree		(xmlBufferPtr buf);
XMLPUBFUN int XMLCALL		
		xmlBufferDump		(FILE *file,
					 xmlBufferPtr buf);
XMLPUBFUN void XMLCALL		
		xmlBufferAdd		(xmlBufferPtr buf,
					 const xmlChar *str,
					 int len);
XMLPUBFUN void XMLCALL		
		xmlBufferAddHead	(xmlBufferPtr buf,
					 const xmlChar *str,
					 int len);
XMLPUBFUN void XMLCALL		
		xmlBufferCat		(xmlBufferPtr buf,
					 const xmlChar *str);
XMLPUBFUN void XMLCALL	
		xmlBufferCCat		(xmlBufferPtr buf,
					 const char *str);
XMLPUBFUN int XMLCALL		
		xmlBufferShrink		(xmlBufferPtr buf,
					 unsigned int len);
XMLPUBFUN int XMLCALL		
		xmlBufferGrow		(xmlBufferPtr buf,
					 unsigned int len);
XMLPUBFUN void XMLCALL		
		xmlBufferEmpty		(xmlBufferPtr buf);
XMLPUBFUN const xmlChar* XMLCALL	
		xmlBufferContent	(const xmlBufferPtr buf);
XMLPUBFUN void XMLCALL		
		xmlBufferSetAllocationScheme(xmlBufferPtr buf,
					 xmlBufferAllocationScheme scheme);
XMLPUBFUN int XMLCALL		
		xmlBufferLength		(const xmlBufferPtr buf);

/*
 * Creating/freeing new structures.
 */
XMLPUBFUN xmlDtdPtr XMLCALL	
		xmlCreateIntSubset	(xmlDocPtr doc,
					 const xmlChar *name,
					 const xmlChar *ExternalID,
					 const xmlChar *SystemID);
XMLPUBFUN xmlDtdPtr XMLCALL	
		xmlNewDtd		(xmlDocPtr doc,
					 const xmlChar *name,
					 const xmlChar *ExternalID,
					 const xmlChar *SystemID);
XMLPUBFUN xmlDtdPtr XMLCALL	
		xmlGetIntSubset		(xmlDocPtr doc);
XMLPUBFUN void XMLCALL		
		xmlFreeDtd		(xmlDtdPtr cur);
XMLPUBFUN xmlNsPtr XMLCALL	
		xmlNewGlobalNs		(xmlDocPtr doc,
					 const xmlChar *href,
					 const xmlChar *prefix);
XMLPUBFUN xmlNsPtr XMLCALL	
		xmlNewNs		(xmlNodePtr node,
					 const xmlChar *href,
					 const xmlChar *prefix);
XMLPUBFUN void XMLCALL		
		xmlFreeNs		(xmlNsPtr cur);
XMLPUBFUN void XMLCALL		
		xmlFreeNsList		(xmlNsPtr cur);
XMLPUBFUN xmlDocPtr XMLCALL 	
		xmlNewDoc		(const xmlChar *version);
XMLPUBFUN void XMLCALL		
		xmlFreeDoc		(xmlDocPtr cur);
XMLPUBFUN xmlAttrPtr XMLCALL	
		xmlNewDocProp		(xmlDocPtr doc,
					 const xmlChar *name,
					 const xmlChar *value);
XMLPUBFUN xmlAttrPtr XMLCALL	
		xmlNewProp		(xmlNodePtr node,
					 const xmlChar *name,
					 const xmlChar *value);
XMLPUBFUN xmlAttrPtr XMLCALL	
		xmlNewNsProp		(xmlNodePtr node,
					 xmlNsPtr ns,
					 const xmlChar *name,
					 const xmlChar *value);
XMLPUBFUN xmlAttrPtr XMLCALL	
		xmlNewNsPropEatName	(xmlNodePtr node,
					 xmlNsPtr ns,
					 xmlChar *name,
					 const xmlChar *value);
XMLPUBFUN void XMLCALL		
		xmlFreePropList		(xmlAttrPtr cur);
XMLPUBFUN void XMLCALL		
		xmlFreeProp		(xmlAttrPtr cur);
XMLPUBFUN xmlAttrPtr XMLCALL	
		xmlCopyProp		(xmlNodePtr target,
					 xmlAttrPtr cur);
XMLPUBFUN xmlAttrPtr XMLCALL	
		xmlCopyPropList		(xmlNodePtr target,
					 xmlAttrPtr cur);
#ifdef LIBXML_TREE_ENABLED
XMLPUBFUN xmlDtdPtr XMLCALL	
		xmlCopyDtd		(xmlDtdPtr dtd);
XMLPUBFUN xmlDocPtr XMLCALL	
		xmlCopyDoc		(xmlDocPtr doc,
					 int recursive);
#endif /* LIBXML_TREE_ENABLED */

/*
 * Creating new nodes.
 */
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewDocNode		(xmlDocPtr doc,
					 xmlNsPtr ns,
					 const xmlChar *name,
					 const xmlChar *content);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewDocNodeEatName	(xmlDocPtr doc,
					 xmlNsPtr ns,
					 xmlChar *name,
					 const xmlChar *content);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewDocRawNode	(xmlDocPtr doc,
					 xmlNsPtr ns,
					 const xmlChar *name,
					 const xmlChar *content);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewNode		(xmlNsPtr ns,
					 const xmlChar *name);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewNodeEatName	(xmlNsPtr ns,
					 xmlChar *name);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewChild		(xmlNodePtr parent,
					 xmlNsPtr ns,
					 const xmlChar *name,
					 const xmlChar *content);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewTextChild		(xmlNodePtr parent,
					 xmlNsPtr ns,
					 const xmlChar *name,
					 const xmlChar *content);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewDocText		(xmlDocPtr doc,
					 const xmlChar *content);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewText		(const xmlChar *content);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewPI		(const xmlChar *name,
					 const xmlChar *content);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewDocTextLen	(xmlDocPtr doc,
					 const xmlChar *content,
					 int len);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewTextLen		(const xmlChar *content,
					 int len);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewDocComment	(xmlDocPtr doc,
					 const xmlChar *content);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewComment		(const xmlChar *content);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewCDataBlock	(xmlDocPtr doc,
					 const xmlChar *content,
					 int len);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewCharRef		(xmlDocPtr doc,
					 const xmlChar *name);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewReference		(xmlDocPtr doc,
					 const xmlChar *name);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlCopyNode		(const xmlNodePtr node,
					 int recursive);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlDocCopyNode		(const xmlNodePtr node,
					 xmlDocPtr doc,
					 int recursive);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlCopyNodeList		(const xmlNodePtr node);
#ifdef LIBXML_TREE_ENABLED
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlNewDocFragment	(xmlDocPtr doc);
#endif /* LIBXML_TREE_ENABLED */

/*
 * Navigating.
 */
XMLPUBFUN long XMLCALL		
		xmlGetLineNo		(xmlNodePtr node);
#ifdef LIBXML_TREE_ENABLED
XMLPUBFUN xmlChar * XMLCALL	
		xmlGetNodePath		(xmlNodePtr node);
#endif /* LIBXML_TREE_ENABLED */
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlDocGetRootElement	(xmlDocPtr doc);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlGetLastChild		(xmlNodePtr parent);
XMLPUBFUN int XMLCALL		
		xmlNodeIsText		(xmlNodePtr node);
XMLPUBFUN int XMLCALL		
		xmlIsBlankNode		(xmlNodePtr node);

#ifdef LIBXML_TREE_ENABLED
/*
 * Changing the structure.
 */
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlDocSetRootElement	(xmlDocPtr doc,
					 xmlNodePtr root);
XMLPUBFUN void XMLCALL		
		xmlNodeSetName		(xmlNodePtr cur,
					 const xmlChar *name);
#endif /* LIBXML_TREE_ENABLED */
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlAddChild		(xmlNodePtr parent,
					 xmlNodePtr cur);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlAddChildList		(xmlNodePtr parent,
					 xmlNodePtr cur);
#ifdef LIBXML_TREE_ENABLED
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlReplaceNode		(xmlNodePtr old,
					 xmlNodePtr cur);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlAddPrevSibling	(xmlNodePtr cur,
					 xmlNodePtr elem);
#endif /* LIBXML_TREE_ENABLED */
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlAddSibling		(xmlNodePtr cur,
					 xmlNodePtr elem);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlAddNextSibling	(xmlNodePtr cur,
					 xmlNodePtr elem);
XMLPUBFUN void XMLCALL		
		xmlUnlinkNode		(xmlNodePtr cur);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlTextMerge		(xmlNodePtr first,
					 xmlNodePtr second);
XMLPUBFUN int XMLCALL		
		xmlTextConcat		(xmlNodePtr node,
					 const xmlChar *content,
					 int len);
XMLPUBFUN void XMLCALL		
		xmlFreeNodeList		(xmlNodePtr cur);
XMLPUBFUN void XMLCALL		
		xmlFreeNode		(xmlNodePtr cur);
XMLPUBFUN void XMLCALL		
		xmlSetTreeDoc		(xmlNodePtr tree,
					 xmlDocPtr doc);
XMLPUBFUN void XMLCALL		
		xmlSetListDoc		(xmlNodePtr list,
					 xmlDocPtr doc);
/*
 * Namespaces.
 */
XMLPUBFUN xmlNsPtr XMLCALL	
		xmlSearchNs		(xmlDocPtr doc,
					 xmlNodePtr node,
					 const xmlChar *nameSpace);
XMLPUBFUN xmlNsPtr XMLCALL	
		xmlSearchNsByHref	(xmlDocPtr doc,
					 xmlNodePtr node,
					 const xmlChar *href);
#ifdef LIBXML_TREE_ENABLED
XMLPUBFUN xmlNsPtr * XMLCALL	
		xmlGetNsList		(xmlDocPtr doc,
					 xmlNodePtr node);
#endif /* LIBXML_TREE_ENABLED */

XMLPUBFUN void XMLCALL		
		xmlSetNs		(xmlNodePtr node,
					 xmlNsPtr ns);
XMLPUBFUN xmlNsPtr XMLCALL	
		xmlCopyNamespace	(xmlNsPtr cur);
XMLPUBFUN xmlNsPtr XMLCALL	
		xmlCopyNamespaceList	(xmlNsPtr cur);

/*
 * Changing the content.
 */
#ifdef LIBXML_TREE_ENABLED
XMLPUBFUN xmlAttrPtr XMLCALL	
		xmlSetProp		(xmlNodePtr node,
					 const xmlChar *name,
					 const xmlChar *value);
#endif /* LIBXML_TREE_ENABLED */
XMLPUBFUN xmlChar * XMLCALL	
		xmlGetNoNsProp		(xmlNodePtr node,
					 const xmlChar *name);
XMLPUBFUN xmlChar * XMLCALL	
		xmlGetProp		(xmlNodePtr node,
					 const xmlChar *name);
XMLPUBFUN xmlAttrPtr XMLCALL	
		xmlHasProp		(xmlNodePtr node,
					 const xmlChar *name);
XMLPUBFUN xmlAttrPtr XMLCALL	
		xmlHasNsProp		(xmlNodePtr node,
					 const xmlChar *name,
					 const xmlChar *nameSpace);
#ifdef LIBXML_TREE_ENABLED
XMLPUBFUN xmlAttrPtr XMLCALL	
		xmlSetNsProp		(xmlNodePtr node,
					 xmlNsPtr ns,
					 const xmlChar *name,
					 const xmlChar *value);
#endif /* LIBXML_TREE_ENABLED */
XMLPUBFUN xmlChar * XMLCALL	
		xmlGetNsProp		(xmlNodePtr node,
					 const xmlChar *name,
					 const xmlChar *nameSpace);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlStringGetNodeList	(xmlDocPtr doc,
					 const xmlChar *value);
XMLPUBFUN xmlNodePtr XMLCALL	
		xmlStringLenGetNodeList	(xmlDocPtr doc,
					 const xmlChar *value,
					 int len);
XMLPUBFUN xmlChar * XMLCALL	
		xmlNodeListGetString	(xmlDocPtr doc,
					 xmlNodePtr list,
					 int inLine);
#ifdef LIBXML_TREE_ENABLED
XMLPUBFUN xmlChar * XMLCALL	
		xmlNodeListGetRawString	(xmlDocPtr doc,
					 xmlNodePtr list,
					 int inLine);
#endif /* LIBXML_TREE_ENABLED */
XMLPUBFUN void XMLCALL		
		xmlNodeSetContent	(xmlNodePtr cur,
					 const xmlChar *content);
#ifdef LIBXML_TREE_ENABLED
XMLPUBFUN void XMLCALL		
		xmlNodeSetContentLen	(xmlNodePtr cur,
					 const xmlChar *content,
					 int len);
#endif /* LIBXML_TREE_ENABLED */
XMLPUBFUN void XMLCALL		
		xmlNodeAddContent	(xmlNodePtr cur,
					 const xmlChar *content);
XMLPUBFUN void XMLCALL		
		xmlNodeAddContentLen	(xmlNodePtr cur,
					 const xmlChar *content,
					 int len);
XMLPUBFUN xmlChar * XMLCALL	
		xmlNodeGetContent	(xmlNodePtr cur);
XMLPUBFUN int XMLCALL
		xmlNodeBufGetContent	(xmlBufferPtr buffer,
					 xmlNodePtr cur);
XMLPUBFUN xmlChar * XMLCALL	
		xmlNodeGetLang		(xmlNodePtr cur);
XMLPUBFUN int XMLCALL		
		xmlNodeGetSpacePreserve	(xmlNodePtr cur);
#ifdef LIBXML_TREE_ENABLED
XMLPUBFUN void XMLCALL		
		xmlNodeSetLang		(xmlNodePtr cur,
					 const xmlChar *lang);
XMLPUBFUN void XMLCALL		
		xmlNodeSetSpacePreserve (xmlNodePtr cur,
					 int val);
#endif /* LIBXML_TREE_ENABLED */
XMLPUBFUN xmlChar * XMLCALL	
		xmlNodeGetBase		(xmlDocPtr doc,
					 xmlNodePtr cur);
#ifdef LIBXML_TREE_ENABLED
XMLPUBFUN void XMLCALL		
		xmlNodeSetBase		(xmlNodePtr cur,
					 const xmlChar *uri);

/*
 * Removing content.
 */
XMLPUBFUN int XMLCALL		
		xmlRemoveProp		(xmlAttrPtr cur);
XMLPUBFUN int XMLCALL		
		xmlUnsetProp		(xmlNodePtr node,
					 const xmlChar *name);
XMLPUBFUN int XMLCALL		
		xmlUnsetNsProp		(xmlNodePtr node,
					 xmlNsPtr ns,
					 const xmlChar *name);
#endif /* LIBXML_TREE_ENABLED */

/*
 * Internal, don't use.
 */
XMLPUBFUN void XMLCALL		
		xmlBufferWriteCHAR	(xmlBufferPtr buf,
					 const xmlChar *string);
XMLPUBFUN void XMLCALL		
		xmlBufferWriteChar	(xmlBufferPtr buf,
					 const char *string);
XMLPUBFUN void XMLCALL		
		xmlBufferWriteQuotedString(xmlBufferPtr buf,
					 const xmlChar *string);

/*
 * Namespace handling.
 */
XMLPUBFUN int XMLCALL		
		xmlReconciliateNs	(xmlDocPtr doc,
					 xmlNodePtr tree);

#ifdef LIBXML_OUTPUT_ENABLED
/*
 * Saving.
 */
XMLPUBFUN void XMLCALL		
		xmlDocDumpFormatMemory	(xmlDocPtr cur,
					 xmlChar **mem,
					 int *size,
					 int format);
XMLPUBFUN void XMLCALL		
		xmlDocDumpMemory	(xmlDocPtr cur,
					 xmlChar **mem,
					 int *size);
XMLPUBFUN void XMLCALL		
		xmlDocDumpMemoryEnc	(xmlDocPtr out_doc,
					 xmlChar **doc_txt_ptr,
					 int * doc_txt_len,
					 const char *txt_encoding);
XMLPUBFUN void XMLCALL		
		xmlDocDumpFormatMemoryEnc(xmlDocPtr out_doc,
					 xmlChar **doc_txt_ptr,
					 int * doc_txt_len,
					 const char *txt_encoding,
					 int format);
XMLPUBFUN int XMLCALL		
		xmlDocFormatDump	(FILE *f,
					 xmlDocPtr cur,
					 int format);
XMLPUBFUN int XMLCALL	
		xmlDocDump		(FILE *f,
					 xmlDocPtr cur);
XMLPUBFUN void XMLCALL		
		xmlElemDump		(FILE *f,
					 xmlDocPtr doc,
					 xmlNodePtr cur);
XMLPUBFUN int XMLCALL		
		xmlSaveFile		(const char *filename,
					 xmlDocPtr cur);
XMLPUBFUN int XMLCALL		
		xmlSaveFormatFile	(const char *filename,
					 xmlDocPtr cur,
					 int format);
XMLPUBFUN int XMLCALL		
		xmlNodeDump		(xmlBufferPtr buf,
					 xmlDocPtr doc,
					 xmlNodePtr cur,
					 int level,
					 int format);

XMLPUBFUN int XMLCALL		
		xmlSaveFileTo		(xmlOutputBufferPtr buf,
					 xmlDocPtr cur,
					 const char *encoding);
XMLPUBFUN int XMLCALL             
		xmlSaveFormatFileTo     (xmlOutputBufferPtr buf,
					 xmlDocPtr cur,
				         const char *encoding,
				         int format);
XMLPUBFUN void XMLCALL		
		xmlNodeDumpOutput	(xmlOutputBufferPtr buf,
					 xmlDocPtr doc,
					 xmlNodePtr cur,
					 int level,
					 int format,
					 const char *encoding);

XMLPUBFUN int XMLCALL		
		xmlSaveFormatFileEnc    (const char *filename,
					 xmlDocPtr cur,
					 const char *encoding,
					 int format);

XMLPUBFUN int XMLCALL		
		xmlSaveFileEnc		(const char *filename,
					 xmlDocPtr cur,
					 const char *encoding);

#endif /* LIBXML_OUTPUT_ENABLED */
/*
 * XHTML
 */
XMLPUBFUN int XMLCALL		
		xmlIsXHTML		(const xmlChar *systemID,
					 const xmlChar *publicID);

/*
 * Compression.
 */
XMLPUBFUN int XMLCALL		
		xmlGetDocCompressMode	(xmlDocPtr doc);
XMLPUBFUN void XMLCALL		
		xmlSetDocCompressMode	(xmlDocPtr doc,
					 int mode);
XMLPUBFUN int XMLCALL		
		xmlGetCompressMode	(void);
XMLPUBFUN void XMLCALL		
		xmlSetCompressMode	(int mode);

#include <fbxfilesdk/fbxfilesdk_nsend.h>

// PJT: Tail inclusion of headers is a no-no. They serve no purpose in
// the header file and just save having to include the header in a C file.
// Well tough! Include it where you need it. 04/02/04
// #ifndef __XML_PARSER_H__
// #include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlmemory.h>
// #endif

#endif // FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_TREE_H

