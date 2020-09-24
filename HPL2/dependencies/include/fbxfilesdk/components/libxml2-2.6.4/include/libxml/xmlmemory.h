/*
 * Summary: interface for the memory allocator
 * Description: provides interfaces for the memory allocator,
 *              including debugging capabilities.
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 */


#ifndef FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_XMLMEMORY_H
#define FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_XMLMEMORY_H

#include <stdio.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlversion.h>
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/**
 * DEBUG_MEMORY:
 *
 * DEBUG_MEMORY replaces the allocator with a collect and debug
 * shell to the libc allocator.
 * DEBUG_MEMORY should only be activated when debugging 
 * libxml i.e. if libxml has been configured with --with-debug-mem too.
 */
/* #define DEBUG_MEMORY_FREED */
/* #define DEBUG_MEMORY_LOCATION */

#ifdef DEBUG
#ifndef DEBUG_MEMORY
// RR: Don't define it anymore: it generate "noise" on output consoles
//#define DEBUG_MEMORY
#endif
#endif

/**
 * DEBUG_MEMORY_LOCATION:
 *
 * DEBUG_MEMORY_LOCATION should be activated only when debugging 
 * libxml i.e. if libxml has been configured with --with-debug-mem too.
 */
#ifdef DEBUG_MEMORY_LOCATION
#endif

/*
 * The XML memory wrapper support 4 basic overloadable functions.
 */
/**
 * xmlFreeFunc:
 * @mem: an already allocated block of memory
 *
 * Signature for a free() implementation.
 */
typedef void (*xmlFreeFunc)(void *mem);
/**
 * xmlMallocFunc:
 * @size:  the size requested in bytes
 *
 * Signature for a malloc() implementation.
 *
 * Returns a pointer to the newly allocated block or NULL in case of error.
 */
typedef void *(*xmlMallocFunc)(size_t size);

/**
 * xmlReallocFunc:
 * @mem: an already allocated block of memory
 * @size:  the new size requested in bytes
 *
 * Signature for a realloc() implementation.
 *
 * Returns a pointer to the newly reallocated block or NULL in case of error.
 */
typedef void *(*xmlReallocFunc)(void *mem, size_t size);

/**
 * xmlStrdupFunc:
 * @str: a zero terminated string
 *
 * Signature for an strdup() implementation.
 *
 * Returns the copy of the string or NULL in case of error.
 */
typedef char *(*xmlStrdupFunc)(const char *str);

/*
 * The way to overload the existing functions.
 * The xmlGc function have an extra entry for atomic block
 * allocations useful for garbage collected memory allocators
 */
XMLPUBFUN int XMLCALL
	xmlMemSetup	(xmlFreeFunc freeFunc,
			 xmlMallocFunc mallocFunc,
			 xmlReallocFunc reallocFunc,
			 xmlStrdupFunc strdupFunc);
XMLPUBFUN int XMLCALL     
	xmlMemGet	(xmlFreeFunc *freeFunc,
			 xmlMallocFunc *mallocFunc,
			 xmlReallocFunc *reallocFunc,
			 xmlStrdupFunc *strdupFunc);
XMLPUBFUN int XMLCALL     
	xmlGcMemSetup	(xmlFreeFunc freeFunc,
			 xmlMallocFunc mallocFunc,
			 xmlMallocFunc mallocAtomicFunc,
			 xmlReallocFunc reallocFunc,
			 xmlStrdupFunc strdupFunc);
XMLPUBFUN int XMLCALL     
	xmlGcMemGet	(xmlFreeFunc *freeFunc,
			 xmlMallocFunc *mallocFunc,
			 xmlMallocFunc *mallocAtomicFunc,
			 xmlReallocFunc *reallocFunc,
			 xmlStrdupFunc *strdupFunc);

/*
 * Initialization of the memory layer.
 */
XMLPUBFUN int XMLCALL	
	xmlInitMemory	(void);

/*
 * These are specific to the XML debug memory wrapper.
 */
XMLPUBFUN int XMLCALL	
	xmlMemUsed	(void);
XMLPUBFUN void XMLCALL	
	xmlMemDisplay	(FILE *fp);
XMLPUBFUN void XMLCALL	
	xmlMemShow	(FILE *fp, int nr);
XMLPUBFUN void XMLCALL	
	xmlMemoryDump	(void);
XMLPUBFUN void * XMLCALL	
	xmlMemMalloc	(size_t size);
XMLPUBFUN void * XMLCALL	
	xmlMemRealloc	(void *ptr,size_t size);
XMLPUBFUN void XMLCALL	
	xmlMemFree	(void *ptr);
XMLPUBFUN char * XMLCALL	
	xmlMemoryStrdup	(const char *str);
XMLPUBFUN void * XMLCALL  
	xmlMallocLoc	(size_t size, const char *file, int line);
XMLPUBFUN void * XMLCALL	
	xmlReallocLoc	(void *ptr, size_t size, const char *file, int line);
XMLPUBFUN void * XMLCALL	
	xmlMallocAtomicLoc (size_t size, const char *file, int line);
XMLPUBFUN char * XMLCALL	
	xmlMemStrdupLoc	(const char *str, const char *file, int line);


#ifdef DEBUG_MEMORY_LOCATION
/**
 * xmlMalloc:
 * @size:  number of bytes to allocate
 *
 * Wrapper for the malloc() function used in the XML library.
 *
 * Returns the pointer to the allocated area or NULL in case of error.
 */
#define xmlMalloc(size) xmlMallocLoc((size), __FILE__, __LINE__)
/**
 * xmlMallocAtomic:
 * @size:  number of bytes to allocate
 *
 * Wrapper for the malloc() function used in the XML library for allocation
 * of block not containing pointers to other areas.
 *
 * Returns the pointer to the allocated area or NULL in case of error.
 */
#define xmlMallocAtomic(size) xmlMallocAtomicLoc((size), __FILE__, __LINE__)
/**
 * xmlRealloc:
 * @ptr:  pointer to the existing allocated area
 * @size:  number of bytes to allocate
 *
 * Wrapper for the realloc() function used in the XML library.
 *
 * Returns the pointer to the allocated area or NULL in case of error.
 */
#define xmlRealloc(ptr, size) xmlReallocLoc((ptr), (size), __FILE__, __LINE__)
/**
 * xmlMemStrdup:
 * @str:  pointer to the existing string
 *
 * Wrapper for the strdup() function, xmlStrdup() is usually preferred.
 *
 * Returns the pointer to the allocated area or NULL in case of error.
 */
#define xmlMemStrdup(str) xmlMemStrdupLoc((str), __FILE__, __LINE__)

#endif /* DEBUG_MEMORY_LOCATION */

// PJT: Tail inclusion of headers is a no-no. They serve no purpose in
// the header file and just save having to include the header in a C file.
// Well tough! Include it where you need it. 04/02/04
// #ifndef __XML_GLOBALS_H
// #ifndef __XML_THREADS_H__
// #include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/threads.h>
// #include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/globals.h>
// #endif
// #endif
// 
#include <fbxfilesdk/fbxfilesdk_nsend.h>
#endif // FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_XMLMEMORY_H

