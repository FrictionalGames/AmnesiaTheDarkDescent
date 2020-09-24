/*
 * Summary: interface for the I/O interfaces used by the parser
 * Description: interface for the I/O interfaces used by the parser
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 */

#ifndef FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_XMLIO_H
#define FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_XMLIO_H

#include <stdio.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlversion.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/*
 * Those are the functions and datatypes for the parser input
 * I/O structures.
 */

/**
 * xmlInputMatchCallback:
 * @filename: the filename or URI
 *
 * Callback used in the I/O Input API to detect if the current handler 
 * can provide input functionalities for this resource.
 *
 * Returns 1 if yes and 0 if another Input module should be used
 */
typedef int (*xmlInputMatchCallback) (char const *filename);
/**
 * xmlInputOpenCallback:
 * @filename: the filename or URI
 *
 * Callback used in the I/O Input API to open the resource
 *
 * Returns an Input context or NULL in case or error
 */
typedef void * (*xmlInputOpenCallback) (char const *filename);
/**
 * xmlInputReadCallback:
 * @context:  an Input context
 * @buffer:  the buffer to store data read
 * @len:  the length of the buffer in bytes
 *
 * Callback used in the I/O Input API to read the resource
 *
 * Returns the number of bytes read or -1 in case of error
 */
typedef int (*xmlInputReadCallback) (void * context, char * buffer, int len);
/**
 * xmlInputCloseCallback:
 * @context:  an Input context
 *
 * Callback used in the I/O Input API to close the resource
 *
 * Returns 0 or -1 in case of error
 */
typedef int (*xmlInputCloseCallback) (void * context);

#ifdef LIBXML_OUTPUT_ENABLED
/*
 * Those are the functions and datatypes for the library output
 * I/O structures.
 */

/**
 * xmlOutputMatchCallback:
 * @filename: the filename or URI
 *
 * Callback used in the I/O Output API to detect if the current handler 
 * can provide output functionalities for this resource.
 *
 * Returns 1 if yes and 0 if another Output module should be used
 */
typedef int (*xmlOutputMatchCallback) (char const *filename);
/**
 * xmlOutputOpenCallback:
 * @filename: the filename or URI
 *
 * Callback used in the I/O Output API to open the resource
 *
 * Returns an Output context or NULL in case or error
 */
typedef void * (*xmlOutputOpenCallback) (char const *filename);
/**
 * xmlOutputWriteCallback:
 * @context:  an Output context
 * @buffer:  the buffer of data to write
 * @len:  the length of the buffer in bytes
 *
 * Callback used in the I/O Output API to write to the resource
 *
 * Returns the number of bytes written or -1 in case of error
 */
typedef int (*xmlOutputWriteCallback) (void * context, const char * buffer,
                                       int len);
/**
 * xmlOutputCloseCallback:
 * @context:  an Output context
 *
 * Callback used in the I/O Output API to close the resource
 *
 * Returns 0 or -1 in case of error
 */
typedef int (*xmlOutputCloseCallback) (void * context);
#endif /* LIBXML_OUTPUT_ENABLED */

#include <fbxfilesdk/fbxfilesdk_nsend.h>

// #include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/globals.h>

#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/tree.h>
// PJT: No need to include parser.h as parser.h already includes this
// file. 04/02/04
// #include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/parser.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/encoding.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

struct _xmlParserInputBuffer {
    void*                  context;
    xmlInputReadCallback   readcallback;
    xmlInputCloseCallback  closecallback;
    
    xmlCharEncodingHandlerPtr encoder; /* I18N conversions to UTF-8 */
    
    xmlBufferPtr buffer;    /* Local buffer encoded in UTF-8 */
    xmlBufferPtr raw;       /* if encoder != NULL buffer for raw input */
    int	compressed;	    /* -1=unknown, 0=not compressed, 1=compressed */
    int error;
};


#ifdef LIBXML_OUTPUT_ENABLED
struct _xmlOutputBuffer {
    void*                   context;
    xmlOutputWriteCallback  writecallback;
    xmlOutputCloseCallback  closecallback;
    
    xmlCharEncodingHandlerPtr encoder; /* I18N conversions to UTF-8 */
    
    xmlBufferPtr buffer;    /* Local buffer encoded in UTF-8 or ISOLatin */
    xmlBufferPtr conv;      /* if encoder != NULL buffer for output */
    int written;            /* total number of byte written */
    int error;
};
#endif /* LIBXML_OUTPUT_ENABLED */

/*
 * Interfaces for input
 */
XMLPUBFUN void XMLCALL	
	xmlCleanupInputCallbacks		(void);

XMLPUBFUN void XMLCALL	
	xmlRegisterDefaultInputCallbacks	(void);
XMLPUBFUN xmlParserInputBufferPtr XMLCALL
	xmlAllocParserInputBuffer		(xmlCharEncoding enc);

XMLPUBFUN xmlParserInputBufferPtr XMLCALL
	xmlParserInputBufferCreateFilename	(const char *URI,
                                                 xmlCharEncoding enc);
XMLPUBFUN xmlParserInputBufferPtr XMLCALL
	xmlParserInputBufferCreateFile		(FILE *file,
                                                 xmlCharEncoding enc);
XMLPUBFUN xmlParserInputBufferPtr XMLCALL
	xmlParserInputBufferCreateFd		(int fd,
	                                         xmlCharEncoding enc);
XMLPUBFUN xmlParserInputBufferPtr XMLCALL
	xmlParserInputBufferCreateMem		(const char *mem, int size,
	                                         xmlCharEncoding enc);
XMLPUBFUN xmlParserInputBufferPtr XMLCALL
	xmlParserInputBufferCreateStatic	(const char *mem, int size,
	                                         xmlCharEncoding enc);
XMLPUBFUN xmlParserInputBufferPtr XMLCALL
	xmlParserInputBufferCreateIO		(xmlInputReadCallback   ioread,
						 xmlInputCloseCallback  ioclose,
						 void *ioctx,
	                                         xmlCharEncoding enc);
XMLPUBFUN int XMLCALL	
	xmlParserInputBufferRead		(xmlParserInputBufferPtr in,
						 int len);
XMLPUBFUN int XMLCALL	
	xmlParserInputBufferGrow		(xmlParserInputBufferPtr in,
						 int len);
XMLPUBFUN int XMLCALL	
	xmlParserInputBufferPush		(xmlParserInputBufferPtr in,
						 int len,
						 const char *buf);
XMLPUBFUN void XMLCALL	
	xmlFreeParserInputBuffer		(xmlParserInputBufferPtr in);
XMLPUBFUN char * XMLCALL	
	xmlParserGetDirectory			(const char *filename);

XMLPUBFUN int XMLCALL     
	xmlRegisterInputCallbacks		(xmlInputMatchCallback matchFunc,
						 xmlInputOpenCallback openFunc,
						 xmlInputReadCallback readFunc,
						 xmlInputCloseCallback closeFunc);
#ifdef LIBXML_OUTPUT_ENABLED
/*
 * Interfaces for output
 */
XMLPUBFUN void XMLCALL	
	xmlCleanupOutputCallbacks		(void);
XMLPUBFUN void XMLCALL	
	xmlRegisterDefaultOutputCallbacks(void);
XMLPUBFUN xmlOutputBufferPtr XMLCALL
	xmlAllocOutputBuffer		(xmlCharEncodingHandlerPtr encoder);

XMLPUBFUN xmlOutputBufferPtr XMLCALL
	xmlOutputBufferCreateFilename	(const char *URI,
					 xmlCharEncodingHandlerPtr encoder,
					 int compression);

XMLPUBFUN xmlOutputBufferPtr XMLCALL
	xmlOutputBufferCreateFile	(FILE *file,
					 xmlCharEncodingHandlerPtr encoder);

XMLPUBFUN xmlOutputBufferPtr XMLCALL
	xmlOutputBufferCreateFd		(int fd,
					 xmlCharEncodingHandlerPtr encoder);

XMLPUBFUN xmlOutputBufferPtr XMLCALL
	xmlOutputBufferCreateIO		(xmlOutputWriteCallback   iowrite,
					 xmlOutputCloseCallback  ioclose,
					 void *ioctx,
					 xmlCharEncodingHandlerPtr encoder);

XMLPUBFUN int XMLCALL	
	xmlOutputBufferWrite		(xmlOutputBufferPtr out,
					 int len,
					 const char *buf);
XMLPUBFUN int XMLCALL	
	xmlOutputBufferWriteString	(xmlOutputBufferPtr out,
					 const char *str);

XMLPUBFUN int XMLCALL	
	xmlOutputBufferFlush		(xmlOutputBufferPtr out);
XMLPUBFUN int XMLCALL	
	xmlOutputBufferClose		(xmlOutputBufferPtr out);

XMLPUBFUN int XMLCALL     
	xmlRegisterOutputCallbacks	(xmlOutputMatchCallback matchFunc,
					 xmlOutputOpenCallback openFunc,
					 xmlOutputWriteCallback writeFunc,
					 xmlOutputCloseCallback closeFunc);
#endif /* LIBXML_OUTPUT_ENABLED */

/*  This function only exists if HTTP support built into the library  */
#ifdef LIBXML_HTTP_ENABLED
XMLPUBFUN void * XMLCALL	
	xmlIOHTTPOpenW			(const char * post_uri,
					 int   compression );
XMLPUBFUN void XMLCALL	
	xmlRegisterHTTPPostCallbacks	(void );
#endif
XMLPUBFUN xmlParserInputPtr XMLCALL
	xmlCheckHTTPInput		(xmlParserCtxtPtr ctxt,
					 xmlParserInputPtr ret);

/*
 * A predefined entity loader disabling network accesses
 */
XMLPUBFUN xmlParserInputPtr XMLCALL 
	xmlNoNetExternalEntityLoader	(const char *URL,
					 const char *ID,
					 xmlParserCtxtPtr ctxt);

/* 
 * xmlNormalizeWindowsPath is obsolete, don't use it. 
 * Check xmlCanonicPath in uri.h for a better alternative.
 */
XMLPUBFUN xmlChar * XMLCALL 
	xmlNormalizeWindowsPath		(const xmlChar *path);

XMLPUBFUN int XMLCALL	
	xmlCheckFilename		(const char *path);
/**
 * Default 'file://' protocol callbacks 
 */
XMLPUBFUN int XMLCALL	
	xmlFileMatch 			(const char *filename);
XMLPUBFUN void * XMLCALL	
	xmlFileOpen 			(const char *filename);
XMLPUBFUN int XMLCALL	
	xmlFileRead 			(void * context, 
					 char * buffer, 
					 int len);
XMLPUBFUN int XMLCALL	
	xmlFileClose 			(void * context);

/**
 * Default 'http://' protocol callbacks 
 */
#ifdef LIBXML_HTTP_ENABLED
XMLPUBFUN int XMLCALL	
	xmlIOHTTPMatch 			(const char *filename);
XMLPUBFUN void * XMLCALL	
	xmlIOHTTPOpen 			(const char *filename);
XMLPUBFUN int XMLCALL 	
	xmlIOHTTPRead			(void * context, 
					 char * buffer, 
					 int len);
XMLPUBFUN int XMLCALL	
	xmlIOHTTPClose 			(void * context);
#endif /* LIBXML_HTTP_ENABLED */

/**
 * Default 'ftp://' protocol callbacks 
 */
#ifdef LIBXML_FTP_ENABLED 
XMLPUBFUN int XMLCALL	
	xmlIOFTPMatch 			(const char *filename);
XMLPUBFUN void * XMLCALL	
	xmlIOFTPOpen 			(const char *filename);
XMLPUBFUN int XMLCALL 	
	xmlIOFTPRead			(void * context, 
					 char * buffer, 
					 int len);
XMLPUBFUN int XMLCALL 	
	xmlIOFTPClose 			(void * context);
#endif /* LIBXML_FTP_ENABLED */

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_XMLIO_H

