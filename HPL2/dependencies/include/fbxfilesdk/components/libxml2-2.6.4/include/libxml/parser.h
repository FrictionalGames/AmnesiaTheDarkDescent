/*
 * Summary: the core parser module
 * Description: Interfaces, constants and types related to the XML parser
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 */

#ifndef FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_PARSER_H
#define FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_PARSER_H

#include <stdarg.h>

#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlversion.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/tree.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/dict.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/hash.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/valid.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/entities.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlerror.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/**
 * XML_DEFAULT_VERSION:
 *
 * The default version of XML used: 1.0
 */
#define XML_DEFAULT_VERSION	"1.0"

/**
 * xmlParserInput:
 *
 * An xmlParserInput is an input flow for the XML processor.
 * Each entity parsed is associated an xmlParserInput (except the
 * few predefined ones). This is the case both for internal entities
 * - in which case the flow is already completely in memory - or
 * external entities - in which case we use the buf structure for
 * progressive reading and I18N conversions to the internal UTF-8 format.
 */

/**
 * xmlParserInputDeallocate:
 * @str:  the string to deallocate
 *
 * Callback for freeing some parser input allocations.
 */
typedef void (* xmlParserInputDeallocate)(xmlChar *str);

struct _xmlParserInput {
    /* Input buffer */
    xmlParserInputBufferPtr buf;      /* UTF-8 encoded buffer */

    const char *filename;             /* The file analyzed, if any */
    const char *directory;            /* the directory/base of the file */
    const xmlChar *base;              /* Base of the array to parse */
    const xmlChar *cur;               /* Current char being parsed */
    const xmlChar *end;               /* end of the array to parse */
    int length;                       /* length if known */
    int line;                         /* Current line */
    int col;                          /* Current column */
    /*
     * NOTE: consumed is only tested for equality in the parser code,
     *       so even if there is an overflow this should not give troubles
     *       for parsing very large instances.
     */
    unsigned long consumed;           /* How many xmlChars already consumed */
    xmlParserInputDeallocate free;    /* function to deallocate the base */
    const xmlChar *encoding;          /* the encoding string for entity */
    const xmlChar *version;           /* the version string for entity */
    int standalone;                   /* Was that entity marked standalone */
    int id;                           /* an unique identifier for the entity */
};

/**
 * xmlParserNodeInfo:
 *
 * The parser can be asked to collect Node informations, i.e. at what
 * place in the file they were detected. 
 * NOTE: This is off by default and not very well tested.
 */
typedef struct _xmlParserNodeInfo xmlParserNodeInfo;
typedef xmlParserNodeInfo *xmlParserNodeInfoPtr;

struct _xmlParserNodeInfo {
  const struct _xmlNode* node;
  /* Position & line # that text that created the node begins & ends on */
  unsigned long begin_pos;
  unsigned long begin_line;
  unsigned long end_pos;
  unsigned long end_line;
};

typedef struct _xmlParserNodeInfoSeq xmlParserNodeInfoSeq;
typedef xmlParserNodeInfoSeq *xmlParserNodeInfoSeqPtr;
struct _xmlParserNodeInfoSeq {
  unsigned long maximum;
  unsigned long length;
  xmlParserNodeInfo* buffer;
};

/**
 * xmlParserInputState:
 *
 * The parser is now working also as a state based parser.
 * The recursive one use the state info for entities processing.
 */
typedef enum {
    XML_PARSER_EOF = -1,	/* nothing is to be parsed */
    XML_PARSER_START = 0,	/* nothing has been parsed */
    XML_PARSER_MISC,		/* Misc* before int subset */
    XML_PARSER_PI,		/* Within a processing instruction */
    XML_PARSER_DTD,		/* within some DTD content */
    XML_PARSER_PROLOG,		/* Misc* after internal subset */
    XML_PARSER_COMMENT,		/* within a comment */
    XML_PARSER_START_TAG,	/* within a start tag */
    XML_PARSER_CONTENT,		/* within the content */
    XML_PARSER_CDATA_SECTION,	/* within a CDATA section */
    XML_PARSER_END_TAG,		/* within a closing tag */
    XML_PARSER_ENTITY_DECL,	/* within an entity declaration */
    XML_PARSER_ENTITY_VALUE,	/* within an entity value in a decl */
    XML_PARSER_ATTRIBUTE_VALUE,	/* within an attribute value */
    XML_PARSER_SYSTEM_LITERAL,	/* within a SYSTEM value */
    XML_PARSER_EPILOG, 		/* the Misc* after the last end tag */
    XML_PARSER_IGNORE,		/* within an IGNORED section */
    XML_PARSER_PUBLIC_LITERAL 	/* within a PUBLIC value */
} xmlParserInputState;

/**
 * XML_DETECT_IDS:
 *
 * Bit in the loadsubset context field to tell to do ID/REFs lookups.
 * Use it to initialize xmlLoadExtDtdDefaultValue.
 */
#define XML_DETECT_IDS		2

/**
 * XML_COMPLETE_ATTRS:
 *
 * Bit in the loadsubset context field to tell to do complete the
 * elements attributes lists with the ones defaulted from the DTDs.
 * Use it to initialize xmlLoadExtDtdDefaultValue.
 */
#define XML_COMPLETE_ATTRS	4

/**
 * XML_SKIP_IDS:
 *
 * Bit in the loadsubset context field to tell to not do ID/REFs registration.
 * Used to initialize xmlLoadExtDtdDefaultValue in some special cases.
 */
#define XML_SKIP_IDS		8

/**
 * xmlParserCtxt:
 *
 * The parser context.
 * NOTE This doesn't completely define the parser state, the (current ?)
 *      design of the parser uses recursive function calls since this allow
 *      and easy mapping from the production rules of the specification
 *      to the actual code. The drawback is that the actual function call
 *      also reflect the parser state. However most of the parsing routines
 *      takes as the only argument the parser context pointer, so migrating
 *      to a state based parser for progressive parsing shouldn't be too hard.
 */
struct _xmlParserCtxt {
    struct _xmlSAXHandler *sax;       /* The SAX handler */
    void            *userData;        /* For SAX interface only, used by DOM build */
    xmlDocPtr           myDoc;        /* the document being built */
    int            wellFormed;        /* is the document well formed */
    int       replaceEntities;        /* shall we replace entities ? */
    const xmlChar    *version;        /* the XML version string */
    const xmlChar   *encoding;        /* the declared encoding, if any */
    int            standalone;        /* standalone document */
    int                  html;        /* an HTML(1)/Docbook(2) document */

    /* Input stream stack */
    xmlParserInputPtr  input;         /* Current input stream */
    int                inputNr;       /* Number of current input streams */
    int                inputMax;      /* Max number of input streams */
    xmlParserInputPtr *inputTab;      /* stack of inputs */

    /* Node analysis stack only used for DOM building */
    xmlNodePtr         node;          /* Current parsed Node */
    int                nodeNr;        /* Depth of the parsing stack */
    int                nodeMax;       /* Max depth of the parsing stack */
    xmlNodePtr        *nodeTab;       /* array of nodes */

    int record_info;                  /* Whether node info should be kept */
    xmlParserNodeInfoSeq node_seq;    /* info about each node parsed */

    int errNo;                        /* error code */

    int     hasExternalSubset;        /* reference and external subset */
    int             hasPErefs;        /* the internal subset has PE refs */
    int              external;        /* are we parsing an external entity */

    int                 valid;        /* is the document valid */
    int              validate;        /* shall we try to validate ? */
    xmlValidCtxt        vctxt;        /* The validity context */

    xmlParserInputState instate;      /* current type of input */
    int                 token;        /* next char look-ahead */    

    char           *directory;        /* the data directory */

    /* Node name stack */
    const xmlChar     *name;          /* Current parsed Node */
    int                nameNr;        /* Depth of the parsing stack */
    int                nameMax;       /* Max depth of the parsing stack */
    const xmlChar *   *nameTab;       /* array of nodes */

    long               nbChars;       /* number of xmlChar processed */
    long            checkIndex;       /* used by progressive parsing lookup */
    int             keepBlanks;       /* ugly but ... */
    int             disableSAX;       /* SAX callbacks are disabled */
    int               inSubset;       /* Parsing is in int 1/ext 2 subset */
    const xmlChar *    intSubName;    /* name of subset */
    xmlChar *          extSubURI;     /* URI of external subset */
    xmlChar *          extSubSystem;  /* SYSTEM ID of external subset */

    /* xml:space values */
    int *              space;         /* Should the parser preserve spaces */
    int                spaceNr;       /* Depth of the parsing stack */
    int                spaceMax;      /* Max depth of the parsing stack */
    int *              spaceTab;      /* array of space infos */

    int                depth;         /* to prevent entity substitution loops */
    xmlParserInputPtr  entity;        /* used to check entities boundaries */
    int                charset;       /* encoding of the in-memory content
				         actually an xmlCharEncoding */
    int                nodelen;       /* Those two fields are there to */
    int                nodemem;       /* Speed up large node parsing */
    int                pedantic;      /* signal pedantic warnings */
    void              *_private;      /* For user data, libxml won't touch it */

    int                loadsubset;    /* should the external subset be loaded */
    int                linenumbers;   /* set line number in element content */
    void              *catalogs;       /* document's own catalog */
    int                recovery;      /* run in recovery mode */
    int                progressive;   /* is this a progressive parsing */
    xmlDictPtr         dict;          /* dictionnary for the parser */
    const xmlChar *   *atts;          /* array for the attributes callbacks */
    int                maxatts;       /* the size of the array */
    int                docdict;       /* use strings from dict to build tree */

    /*
     * pre-interned strings
     */
    const xmlChar *str_xml;
    const xmlChar *str_xmlns;
    const xmlChar *str_xml_ns;

    /*
     * Everything below is used only by the new SAX mode
     */
    int                sax2;          /* operating in the new SAX mode */
    int                nsNr;          /* the number of inherited namespaces */
    int                nsMax;         /* the size of the arrays */
    const xmlChar *   *nsTab;         /* the array of prefix/namespace name */
    int               *attallocs;     /* which attribute were allocated */
    void *            *pushTab;       /* array of data for push */
    xmlHashTablePtr    attsDefault;   /* defaulted attributes if any */
    xmlHashTablePtr    attsSpecial;   /* non-CDATA attributes if any */
    int                nsWellFormed;  /* is the document XML Nanespace okay */
    int                options;       /* Extra options */

    /*
     * Those fields are needed only for treaming parsing so far
     */
    int               dictNames;    /* Use dictionary names for the tree */
    int               freeElemsNr;  /* number of freed element nodes */
    xmlNodePtr        freeElems;    /* List of freed element nodes */
    int               freeAttrsNr;  /* number of freed attributes nodes */
    xmlAttrPtr        freeAttrs;    /* List of freed attributes nodes */

    /*
     * the complete error informations for the last error.
     */
    xmlError          lastError;
};

/**
 * xmlSAXLocator:
 *
 * A SAX Locator.
 */
struct _xmlSAXLocator {
    const xmlChar *(*getPublicId)(void *ctx);
    const xmlChar *(*getSystemId)(void *ctx);
    int (*getLineNumber)(void *ctx);
    int (*getColumnNumber)(void *ctx);
};

/**
 * xmlSAXHandler:
 *
 * A SAX handler is bunch of callbacks called by the parser when processing
 * of the input generate data or structure informations.
 */

/**
 * resolveEntitySAXFunc:
 * @ctx:  the user data (XML parser context)
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 *
 * Callback:
 * The entity loader, to control the loading of external entities,
 * the application can either:
 *    - override this resolveEntity() callback in the SAX block
 *    - or better use the xmlSetExternalEntityLoader() function to
 *      set up it's own entity resolution routine
 *
 * Returns the xmlParserInputPtr if inlined or NULL for DOM behavior.
 */
typedef xmlParserInputPtr (*resolveEntitySAXFunc) (void *ctx,
				const xmlChar *publicId,
				const xmlChar *systemId);
/**
 * internalSubsetSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @name:  the root element name
 * @ExternalID:  the external ID
 * @SystemID:  the SYSTEM ID (e.g. filename or URL)
 *
 * Callback on internal subset declaration.
 */
typedef void (*internalSubsetSAXFunc) (void *ctx,
				const xmlChar *name,
				const xmlChar *ExternalID,
				const xmlChar *SystemID);
/**
 * externalSubsetSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @name:  the root element name
 * @ExternalID:  the external ID
 * @SystemID:  the SYSTEM ID (e.g. filename or URL)
 *
 * Callback on external subset declaration.
 */
typedef void (*externalSubsetSAXFunc) (void *ctx,
				const xmlChar *name,
				const xmlChar *ExternalID,
				const xmlChar *SystemID);
/**
 * getEntitySAXFunc:
 * @ctx:  the user data (XML parser context)
 * @name: The entity name
 *
 * Get an entity by name.
 *
 * Returns the xmlEntityPtr if found.
 */
typedef xmlEntityPtr (*getEntitySAXFunc) (void *ctx,
				const xmlChar *name);
/**
 * getParameterEntitySAXFunc:
 * @ctx:  the user data (XML parser context)
 * @name: The entity name
 *
 * Get a parameter entity by name.
 *
 * Returns the xmlEntityPtr if found.
 */
typedef xmlEntityPtr (*getParameterEntitySAXFunc) (void *ctx,
				const xmlChar *name);
/**
 * entityDeclSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @name:  the entity name 
 * @type:  the entity type 
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 * @content: the entity value (without processing).
 *
 * An entity definition has been parsed.
 */
typedef void (*entityDeclSAXFunc) (void *ctx,
				const xmlChar *name,
				int type,
				const xmlChar *publicId,
				const xmlChar *systemId,
				xmlChar *content);
/**
 * notationDeclSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @name: The name of the notation
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 *
 * What to do when a notation declaration has been parsed.
 */
typedef void (*notationDeclSAXFunc)(void *ctx,
				const xmlChar *name,
				const xmlChar *publicId,
				const xmlChar *systemId);
/**
 * attributeDeclSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @elem:  the name of the element
 * @fullname:  the attribute name 
 * @type:  the attribute type 
 * @def:  the type of default value
 * @defaultValue: the attribute default value
 * @tree:  the tree of enumerated value set
 *
 * An attribute definition has been parsed.
 */
typedef void (*attributeDeclSAXFunc)(void *ctx,
				const xmlChar *elem,
				const xmlChar *fullname,
				int type,
				int def,
				const xmlChar *defaultValue,
				xmlEnumerationPtr tree);
/**
 * elementDeclSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @name:  the element name 
 * @type:  the element type 
 * @content: the element value tree
 *
 * An element definition has been parsed.
 */
typedef void (*elementDeclSAXFunc)(void *ctx,
				const xmlChar *name,
				int type,
				xmlElementContentPtr content);
/**
 * unparsedEntityDeclSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @name: The name of the entity
 * @publicId: The public ID of the entity
 * @systemId: The system ID of the entity
 * @notationName: the name of the notation
 *
 * What to do when an unparsed entity declaration is parsed.
 */
typedef void (*unparsedEntityDeclSAXFunc)(void *ctx,
				const xmlChar *name,
				const xmlChar *publicId,
				const xmlChar *systemId,
				const xmlChar *notationName);
/**
 * setDocumentLocatorSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @loc: A SAX Locator
 *
 * Receive the document locator at startup, actually xmlDefaultSAXLocator.
 * Everything is available on the context, so this is useless in our case.
 */
typedef void (*setDocumentLocatorSAXFunc) (void *ctx,
				xmlSAXLocatorPtr loc);
/**
 * startDocumentSAXFunc:
 * @ctx:  the user data (XML parser context)
 *
 * Called when the document start being processed.
 */
typedef void (*startDocumentSAXFunc) (void *ctx);
/**
 * endDocumentSAXFunc:
 * @ctx:  the user data (XML parser context)
 *
 * Called when the document end has been detected.
 */
typedef void (*endDocumentSAXFunc) (void *ctx);
/**
 * startElementSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @name:  The element name, including namespace prefix
 * @atts:  An array of name/value attributes pairs, NULL terminated
 *
 * Called when an opening tag has been processed.
 */
typedef void (*startElementSAXFunc) (void *ctx,
				const xmlChar *name,
				const xmlChar **atts);
/**
 * endElementSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @name:  The element name
 *
 * Called when the end of an element has been detected.
 */
typedef void (*endElementSAXFunc) (void *ctx,
				const xmlChar *name);
/**
 * attributeSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @name:  The attribute name, including namespace prefix
 * @value:  The attribute value
 *
 * Handle an attribute that has been read by the parser.
 * The default handling is to convert the attribute into an
 * DOM subtree and past it in a new xmlAttr element added to
 * the element.
 */
typedef void (*attributeSAXFunc) (void *ctx,
				const xmlChar *name,
				const xmlChar *value);
/**
 * referenceSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @name:  The entity name
 *
 * Called when an entity reference is detected. 
 */
typedef void (*referenceSAXFunc) (void *ctx,
				const xmlChar *name);
/**
 * charactersSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @ch:  a xmlChar string
 * @len: the number of xmlChar
 *
 * Receiving some chars from the parser.
 */
typedef void (*charactersSAXFunc) (void *ctx,
				const xmlChar *ch,
				int len);
/**
 * ignorableWhitespaceSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @ch:  a xmlChar string
 * @len: the number of xmlChar
 *
 * Receiving some ignorable whitespaces from the parser.
 * UNUSED: by default the DOM building will use characters.
 */
typedef void (*ignorableWhitespaceSAXFunc) (void *ctx,
				const xmlChar *ch,
				int len);
/**
 * processingInstructionSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @target:  the target name
 * @data: the PI data's
 *
 * A processing instruction has been parsed.
 */
typedef void (*processingInstructionSAXFunc) (void *ctx,
				const xmlChar *target,
				const xmlChar *data);
/**
 * commentSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @value:  the comment content
 *
 * A comment has been parsed.
 */
typedef void (*commentSAXFunc) (void *ctx,
				const xmlChar *value);
/**
 * cdataBlockSAXFunc:
 * @ctx:  the user data (XML parser context)
 * @value:  The pcdata content
 * @len:  the block length
 *
 * Called when a pcdata block has been parsed.
 */
typedef void (*cdataBlockSAXFunc) (
	                        void *ctx,
				const xmlChar *value,
				int len);
/**
 * warningSAXFunc:
 * @ctx:  an XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 * 
 * Display and format a warning messages, callback.
 */
typedef void (*warningSAXFunc) (void *ctx,
				const char *msg, ...);
/**
 * errorSAXFunc:
 * @ctx:  an XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 * 
 * Display and format an error messages, callback.
 */
typedef void (*errorSAXFunc) (void *ctx,
				const char *msg, ...);
/**
 * fatalErrorSAXFunc:
 * @ctx:  an XML parser context
 * @msg:  the message to display/transmit
 * @...:  extra parameters for the message display
 * 
 * Display and format fatal error messages, callback.
 * Note: so far fatalError() SAX callbacks are not used, error()
 *       get all the callbacks for errors.
 */
typedef void (*fatalErrorSAXFunc) (void *ctx,
				const char *msg, ...);
/**
 * isStandaloneSAXFunc:
 * @ctx:  the user data (XML parser context)
 *
 * Is this document tagged standalone?
 *
 * Returns 1 if true
 */
typedef int (*isStandaloneSAXFunc) (void *ctx);
/**
 * hasInternalSubsetSAXFunc:
 * @ctx:  the user data (XML parser context)
 *
 * Does this document has an internal subset.
 *
 * Returns 1 if true
 */
typedef int (*hasInternalSubsetSAXFunc) (void *ctx);

/**
 * hasExternalSubsetSAXFunc:
 * @ctx:  the user data (XML parser context)
 *
 * Does this document has an external subset?
 *
 * Returns 1 if true
 */
typedef int (*hasExternalSubsetSAXFunc) (void *ctx);

/************************************************************************
 *									*
 *			The SAX version 2 API extensions		*
 *									*
 ************************************************************************/
/**
 * XML_SAX2_MAGIC:
 *
 * Special constant found in SAX2 blocks initialized fields
 */
#define XML_SAX2_MAGIC 0xDEEDBEAF

/**
 * startElementNsSAX2Func:
 * @ctx:  the user data (XML parser context)
 * @localname:  the local name of the element
 * @prefix:  the element namespace prefix if available
 * @URI:  the element namespace name if available
 * @nb_namespaces:  number of namespace definitions on that node
 * @namespaces:  pointer to the array of prefix/URI pairs namespace definitions
 * @nb_attributes:  the number of attributes on that node
 * @nb_defaulted:  the number of defaulted attributes. The defaulted
 *                  ones are at the end of the array
 * @attributes:  pointer to the array of (localname/prefix/URI/value/end)
 *               attribute values.
 *
 * SAX2 callback when an element start has been detected by the parser.
 * It provides the namespace informations for the element, as well as
 * the new namespace declarations on the element.
 */

typedef void (*startElementNsSAX2Func) (void *ctx,
					const xmlChar *localname,
					const xmlChar *prefix,
					const xmlChar *URI,
					int nb_namespaces,
					const xmlChar **namespaces,
					int nb_attributes,
					int nb_defaulted,
					const xmlChar **attributes);
 
/**
 * endElementNsSAX2Func:
 * @ctx:  the user data (XML parser context)
 * @localname:  the local name of the element
 * @prefix:  the element namespace prefix if available
 * @URI:  the element namespace name if available
 *
 * SAX2 callback when an element end has been detected by the parser.
 * It provides the namespace informations for the element.
 */

typedef void (*endElementNsSAX2Func)   (void *ctx,
					const xmlChar *localname,
					const xmlChar *prefix,
					const xmlChar *URI);


struct _xmlSAXHandler {
    internalSubsetSAXFunc internalSubset;
    isStandaloneSAXFunc isStandalone;
    hasInternalSubsetSAXFunc hasInternalSubset;
    hasExternalSubsetSAXFunc hasExternalSubset;
    resolveEntitySAXFunc resolveEntity;
    getEntitySAXFunc getEntity;
    entityDeclSAXFunc entityDecl;
    notationDeclSAXFunc notationDecl;
    attributeDeclSAXFunc attributeDecl;
    elementDeclSAXFunc elementDecl;
    unparsedEntityDeclSAXFunc unparsedEntityDecl;
    setDocumentLocatorSAXFunc setDocumentLocator;
    startDocumentSAXFunc startDocument;
    endDocumentSAXFunc endDocument;
    startElementSAXFunc startElement;
    endElementSAXFunc endElement;
    referenceSAXFunc reference;
    charactersSAXFunc characters;
    ignorableWhitespaceSAXFunc ignorableWhitespace;
    processingInstructionSAXFunc processingInstruction;
    commentSAXFunc comment;
    warningSAXFunc warning;
    errorSAXFunc error;
    fatalErrorSAXFunc fatalError; /* unused error() get all the errors */
    getParameterEntitySAXFunc getParameterEntity;
    cdataBlockSAXFunc cdataBlock;
    externalSubsetSAXFunc externalSubset;
    unsigned int initialized;
    /* The following fields are extensions available only on version 2 */
    void *_private;
    startElementNsSAX2Func startElementNs;
    endElementNsSAX2Func endElementNs;
    xmlStructuredErrorFunc serror;
};

/*
 * SAX Version 1
 */
typedef struct _xmlSAXHandlerV1 xmlSAXHandlerV1;
typedef xmlSAXHandlerV1 *xmlSAXHandlerV1Ptr;
struct _xmlSAXHandlerV1 {
    internalSubsetSAXFunc internalSubset;
    isStandaloneSAXFunc isStandalone;
    hasInternalSubsetSAXFunc hasInternalSubset;
    hasExternalSubsetSAXFunc hasExternalSubset;
    resolveEntitySAXFunc resolveEntity;
    getEntitySAXFunc getEntity;
    entityDeclSAXFunc entityDecl;
    notationDeclSAXFunc notationDecl;
    attributeDeclSAXFunc attributeDecl;
    elementDeclSAXFunc elementDecl;
    unparsedEntityDeclSAXFunc unparsedEntityDecl;
    setDocumentLocatorSAXFunc setDocumentLocator;
    startDocumentSAXFunc startDocument;
    endDocumentSAXFunc endDocument;
    startElementSAXFunc startElement;
    endElementSAXFunc endElement;
    referenceSAXFunc reference;
    charactersSAXFunc characters;
    ignorableWhitespaceSAXFunc ignorableWhitespace;
    processingInstructionSAXFunc processingInstruction;
    commentSAXFunc comment;
    warningSAXFunc warning;
    errorSAXFunc error;
    fatalErrorSAXFunc fatalError; /* unused error() get all the errors */
    getParameterEntitySAXFunc getParameterEntity;
    cdataBlockSAXFunc cdataBlock;
    externalSubsetSAXFunc externalSubset;
    unsigned int initialized;
};


/**
 * xmlExternalEntityLoader:
 * @URL: The System ID of the resource requested
 * @ID: The Public ID of the resource requested
 * @context: the XML parser context 
 *
 * External entity loaders types.
 *
 * Returns the entity input parser.
 */
typedef xmlParserInputPtr (*xmlExternalEntityLoader) (const char *URL,
					 const char *ID,
					 xmlParserCtxtPtr context);

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/encoding.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlIO.h>
// #include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/globals.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/*
 * Init/Cleanup
 */
XMLPUBFUN void XMLCALL		
		xmlInitParser		(void);
XMLPUBFUN void XMLCALL		
		xmlCleanupParser	(void);

/*
 * Input functions
 */
XMLPUBFUN int XMLCALL		
		xmlParserInputRead	(xmlParserInputPtr in,
					 int len);
XMLPUBFUN int XMLCALL		
		xmlParserInputGrow	(xmlParserInputPtr in,
					 int len);

/*
 * xmlChar handling
 */
XMLPUBFUN xmlChar * XMLCALL	
		xmlStrdup		(const xmlChar *cur);
XMLPUBFUN xmlChar * XMLCALL	
		xmlStrndup		(const xmlChar *cur,
					 int len);
XMLPUBFUN xmlChar * XMLCALL	
		xmlCharStrndup		(const char *cur,
					 int len);
XMLPUBFUN xmlChar * XMLCALL	
		xmlCharStrdup		(const char *cur);
XMLPUBFUN xmlChar * XMLCALL	
		xmlStrsub		(const xmlChar *str,
					 int start,
					 int len);
XMLPUBFUN const xmlChar * XMLCALL	
		xmlStrchr		(const xmlChar *str,
					 xmlChar val);
XMLPUBFUN const xmlChar * XMLCALL	
		xmlStrstr		(const xmlChar *str,
					 const xmlChar *val);
XMLPUBFUN const xmlChar * XMLCALL	
		xmlStrcasestr		(const xmlChar *str,
					 xmlChar *val);
XMLPUBFUN int XMLCALL		
		xmlStrcmp		(const xmlChar *str1,
					 const xmlChar *str2);
XMLPUBFUN int XMLCALL		
		xmlStrncmp		(const xmlChar *str1,
					 const xmlChar *str2,
					 int len);
XMLPUBFUN int XMLCALL		
		xmlStrcasecmp		(const xmlChar *str1,
					 const xmlChar *str2);
XMLPUBFUN int XMLCALL		
		xmlStrncasecmp		(const xmlChar *str1,
					 const xmlChar *str2,
					 int len);
XMLPUBFUN int XMLCALL		
		xmlStrEqual		(const xmlChar *str1,
					 const xmlChar *str2);
XMLPUBFUN int XMLCALL
		xmlStrQEqual		(const xmlChar *pref,
					 const xmlChar *name,
					 const xmlChar *str);
XMLPUBFUN int XMLCALL		
		xmlStrlen		(const xmlChar *str);
XMLPUBFUN xmlChar * XMLCALL	
		xmlStrcat		(xmlChar *cur,
					 const xmlChar *add);
XMLPUBFUN xmlChar * XMLCALL	
		xmlStrncat		(xmlChar *cur,
					 const xmlChar *add,
					 int len);

XMLPUBFUN int XMLCALL	
		xmlStrPrintf		(xmlChar *buf,
					 int len,
					 const xmlChar *msg,
					 ...);

XMLPUBFUN int XMLCALL	
		xmlStrVPrintf		(xmlChar *buf,
					 int len,
					 const xmlChar *msg,
					 va_list ap);

/*
 * Basic parsing Interfaces
 */
XMLPUBFUN xmlDocPtr XMLCALL	
		xmlParseDoc		(xmlChar *cur);
XMLPUBFUN xmlDocPtr XMLCALL	
		xmlParseMemory		(const char *buffer,
					 int size);
XMLPUBFUN xmlDocPtr XMLCALL	
		xmlParseFile		(const char *filename);
XMLPUBFUN int XMLCALL		
		xmlSubstituteEntitiesDefault(int val);
XMLPUBFUN int XMLCALL		
		xmlKeepBlanksDefault	(int val);
XMLPUBFUN void XMLCALL		
		xmlStopParser		(xmlParserCtxtPtr ctxt);
XMLPUBFUN int XMLCALL		
		xmlPedanticParserDefault(int val);
XMLPUBFUN int XMLCALL		
		xmlLineNumbersDefault	(int val);

/*
 * Recovery mode 
 */
XMLPUBFUN xmlDocPtr XMLCALL	
		xmlRecoverDoc		(xmlChar *cur);
XMLPUBFUN xmlDocPtr XMLCALL	
		xmlRecoverMemory	(const char *buffer,
					 int size);
XMLPUBFUN xmlDocPtr XMLCALL	
		xmlRecoverFile		(const char *filename);

/*
 * Less common routines and SAX interfaces
 */
XMLPUBFUN int XMLCALL		
		xmlParseDocument	(xmlParserCtxtPtr ctxt);
XMLPUBFUN int XMLCALL		
		xmlParseExtParsedEnt	(xmlParserCtxtPtr ctxt);
XMLPUBFUN xmlDocPtr XMLCALL	
		xmlSAXParseDoc		(xmlSAXHandlerPtr sax,
					 xmlChar *cur,
					 int recovery);
XMLPUBFUN int XMLCALL		
		xmlSAXUserParseFile	(xmlSAXHandlerPtr sax,
					 void *user_data,
					 const char *filename);
XMLPUBFUN int XMLCALL		
		xmlSAXUserParseMemory	(xmlSAXHandlerPtr sax,
					 void *user_data,
					 const char *buffer,
					 int size);
XMLPUBFUN xmlDocPtr XMLCALL	
		xmlSAXParseMemory	(xmlSAXHandlerPtr sax,
					 const char *buffer,
                                   	 int size,
					 int recovery);
XMLPUBFUN xmlDocPtr XMLCALL	
		xmlSAXParseMemoryWithData (xmlSAXHandlerPtr sax,
					 const char *buffer,
                                   	 int size,
					 int recovery,
					 void *data);
XMLPUBFUN xmlDocPtr XMLCALL	
		xmlSAXParseFile		(xmlSAXHandlerPtr sax,
					 const char *filename,
					 int recovery);
XMLPUBFUN xmlDocPtr XMLCALL	
		xmlSAXParseFileWithData	(xmlSAXHandlerPtr sax,
					 const char *filename,
					 int recovery,
					 void *data);
XMLPUBFUN xmlDocPtr XMLCALL	
		xmlSAXParseEntity	(xmlSAXHandlerPtr sax,
					 const char *filename);
XMLPUBFUN xmlDocPtr XMLCALL	
		xmlParseEntity		(const char *filename);
XMLPUBFUN xmlDtdPtr XMLCALL	
		xmlParseDTD		(const xmlChar *ExternalID,
					 const xmlChar *SystemID);
XMLPUBFUN xmlDtdPtr XMLCALL	
		xmlSAXParseDTD		(xmlSAXHandlerPtr sax,
					 const xmlChar *ExternalID,
					 const xmlChar *SystemID);
XMLPUBFUN xmlDtdPtr XMLCALL	
		xmlIOParseDTD		(xmlSAXHandlerPtr sax,
					 xmlParserInputBufferPtr input,
					 xmlCharEncoding enc);
XMLPUBFUN int XMLCALL	
		xmlParseBalancedChunkMemory(xmlDocPtr doc,
					 xmlSAXHandlerPtr sax,
					 void *user_data,
					 int depth,
					 const xmlChar *string,
					 xmlNodePtr *lst);
XMLPUBFUN int XMLCALL          
		xmlParseBalancedChunkMemoryRecover(xmlDocPtr doc,
                     xmlSAXHandlerPtr sax,
                     void *user_data,
                     int depth,
                     const xmlChar *string,
                     xmlNodePtr *lst,
                     int recover);
XMLPUBFUN int XMLCALL		
		xmlParseExternalEntity	(xmlDocPtr doc,
					 xmlSAXHandlerPtr sax,
					 void *user_data,
					 int depth,
					 const xmlChar *URL,
					 const xmlChar *ID,
					 xmlNodePtr *lst);
XMLPUBFUN int XMLCALL		
		xmlParseCtxtExternalEntity(xmlParserCtxtPtr ctx,
					 const xmlChar *URL,
					 const xmlChar *ID,
					 xmlNodePtr *lst);

/*
 * Parser contexts handling.
 */
XMLPUBFUN xmlParserCtxtPtr XMLCALL	
		xmlNewParserCtxt	(void);
XMLPUBFUN int XMLCALL		
		xmlInitParserCtxt	(xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL		
		xmlClearParserCtxt	(xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL		
		xmlFreeParserCtxt	(xmlParserCtxtPtr ctxt);
XMLPUBFUN void XMLCALL		
		xmlSetupParserForBuffer	(xmlParserCtxtPtr ctxt,
					 const xmlChar* buffer,
					 const char *filename);
XMLPUBFUN xmlParserCtxtPtr XMLCALL 
		xmlCreateDocParserCtxt	(const xmlChar *cur);

/*
 * Reading/setting optional parsing features.
 */

XMLPUBFUN int XMLCALL		
		xmlGetFeaturesList	(int *len,
					 const char **result);
XMLPUBFUN int XMLCALL		
		xmlGetFeature		(xmlParserCtxtPtr ctxt,
					 const char *name,
					 void *result);
XMLPUBFUN int XMLCALL		
		xmlSetFeature		(xmlParserCtxtPtr ctxt,
					 const char *name,
					 void *value);

#ifdef LIBXML_PUSH_ENABLED
/*
 * Interfaces for the Push mode.
 */
XMLPUBFUN xmlParserCtxtPtr XMLCALL 
		xmlCreatePushParserCtxt(xmlSAXHandlerPtr sax,
					 void *user_data,
					 const char *chunk,
					 int size,
					 const char *filename);
XMLPUBFUN int XMLCALL		 
		xmlParseChunk		(xmlParserCtxtPtr ctxt,
					 const char *chunk,
					 int size,
					 int terminate);
#endif /* LIBXML_PUSH_ENABLED */

/*
 * Special I/O mode.
 */

XMLPUBFUN xmlParserCtxtPtr XMLCALL 
		xmlCreateIOParserCtxt	(xmlSAXHandlerPtr sax,
					 void *user_data,
					 xmlInputReadCallback   ioread,
					 xmlInputCloseCallback  ioclose,
					 void *ioctx,
					 xmlCharEncoding enc);

XMLPUBFUN xmlParserInputPtr XMLCALL 
		xmlNewIOInputStream	(xmlParserCtxtPtr ctxt,
					 xmlParserInputBufferPtr input,
					 xmlCharEncoding enc);

/*
 * Node infos.
 */
XMLPUBFUN const xmlParserNodeInfo* XMLCALL
		xmlParserFindNodeInfo	(const xmlParserCtxtPtr ctxt,
				         const xmlNodePtr node);
XMLPUBFUN void XMLCALL		
		xmlInitNodeInfoSeq	(xmlParserNodeInfoSeqPtr seq);
XMLPUBFUN void XMLCALL		
		xmlClearNodeInfoSeq	(xmlParserNodeInfoSeqPtr seq);
XMLPUBFUN unsigned long XMLCALL 
		xmlParserFindNodeInfoIndex(const xmlParserNodeInfoSeqPtr seq,
                                         const xmlNodePtr node);
XMLPUBFUN void XMLCALL		
		xmlParserAddNodeInfo	(xmlParserCtxtPtr ctxt,
					 const xmlParserNodeInfoPtr info);

/*
 * External entities handling actually implemented in xmlIO.
 */

XMLPUBFUN void XMLCALL		
		xmlSetExternalEntityLoader(xmlExternalEntityLoader f);
XMLPUBFUN xmlExternalEntityLoader XMLCALL
		xmlGetExternalEntityLoader(void);
XMLPUBFUN xmlParserInputPtr XMLCALL
		xmlLoadExternalEntity	(const char *URL,
					 const char *ID,
					 xmlParserCtxtPtr ctxt);
/*
 * New set of simpler/more flexible APIs
 */
/**
 * xmlParserOption:
 *
 * This is the set of XML parser options that can be passed down
 * to the xmlReadDoc() and similar calls.
 */
typedef enum {
    XML_PARSE_RECOVER	= 1<<0,	/* recover on errors */
    XML_PARSE_NOENT	= 1<<1,	/* substitute entities */
    XML_PARSE_DTDLOAD	= 1<<2,	/* load the external subset */
    XML_PARSE_DTDATTR	= 1<<3,	/* default DTD attributes */
    XML_PARSE_DTDVALID	= 1<<4,	/* validate with the DTD */
    XML_PARSE_NOERROR	= 1<<5,	/* suppress error reports */
    XML_PARSE_NOWARNING	= 1<<6,	/* suppress warning reports */
    XML_PARSE_PEDANTIC	= 1<<7,	/* pedantic error reporting */
    XML_PARSE_NOBLANKS	= 1<<8,	/* remove blank nodes */
    XML_PARSE_SAX1	= 1<<9,	/* use the SAX1 interface internally */
    XML_PARSE_XINCLUDE	= 1<<10,/* Implement XInclude substitition  */
    XML_PARSE_NONET	= 1<<11,/* Forbid network access */
    XML_PARSE_NODICT	= 1<<12,/* Do not reuse the context dictionnary */
    XML_PARSE_NSCLEAN	= 1<<13,/* remove redundant namespaces declarations */
    XML_PARSE_NOCDATA	= 1<<14 /* merge CDATA as text nodes */
} xmlParserOption;

XMLPUBFUN void XMLCALL
		xmlCtxtReset		(xmlParserCtxtPtr ctxt);
XMLPUBFUN int XMLCALL
		xmlCtxtResetPush	(xmlParserCtxtPtr ctxt,
					 const char *chunk,
					 int size,
					 const char *filename,
					 const char *encoding);
XMLPUBFUN int XMLCALL
		xmlCtxtUseOptions	(xmlParserCtxtPtr ctxt,
					 int options);
XMLPUBFUN xmlDocPtr XMLCALL
		xmlReadDoc		(const xmlChar *cur,
					 const char *URL,
					 const char *encoding,
					 int options);
XMLPUBFUN xmlDocPtr XMLCALL
		xmlReadFile		(const char *URL,
					 const char *encoding,
					 int options);
XMLPUBFUN xmlDocPtr XMLCALL
		xmlReadMemory		(const char *buffer,
					 int size,
					 const char *URL,
					 const char *encoding,
					 int options);
XMLPUBFUN xmlDocPtr XMLCALL
		xmlReadFd		(int fd,
					 const char *URL,
					 const char *encoding,
					 int options);
XMLPUBFUN xmlDocPtr XMLCALL
		xmlReadIO		(xmlInputReadCallback ioread,
					 xmlInputCloseCallback ioclose,
					 void *ioctx,
					 const char *URL,
					 const char *encoding,
					 int options);
XMLPUBFUN xmlDocPtr XMLCALL
		xmlCtxtReadDoc		(xmlParserCtxtPtr ctxt,
					 const xmlChar *cur,
					 const char *URL,
					 const char *encoding,
					 int options);
XMLPUBFUN xmlDocPtr XMLCALL
		xmlCtxtReadFile		(xmlParserCtxtPtr ctxt,
					 const char *filename,
					 const char *encoding,
					 int options);
XMLPUBFUN xmlDocPtr XMLCALL
		xmlCtxtReadMemory		(xmlParserCtxtPtr ctxt,
					 const char *buffer,
					 int size,
					 const char *URL,
					 const char *encoding,
					 int options);
XMLPUBFUN xmlDocPtr XMLCALL
		xmlCtxtReadFd		(xmlParserCtxtPtr ctxt,
					 int fd,
					 const char *URL,
					 const char *encoding,
					 int options);
XMLPUBFUN xmlDocPtr XMLCALL
		xmlCtxtReadIO		(xmlParserCtxtPtr ctxt,
					 xmlInputReadCallback ioread,
					 xmlInputCloseCallback ioclose,
					 void *ioctx,
					 const char *URL,
					 const char *encoding,
					 int options);

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_PARSER_H

