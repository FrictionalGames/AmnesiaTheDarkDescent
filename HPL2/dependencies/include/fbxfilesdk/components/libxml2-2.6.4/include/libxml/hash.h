/*
 * Summary: chained hash tables
 * description: this module implement the hash table support used in 
 * various place in the library.
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Bjorn Reese <bjorn.reese@systematic.dk>
 */

#ifndef FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_HASH_H
#define FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_HASH_H

/*
 * The hash table.
 */
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

typedef struct _xmlHashTable xmlHashTable;
typedef xmlHashTable *xmlHashTablePtr;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlversion.h>
// PJT: Shouldn't need parser.h, just xmlexports.h.
// #include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/parser.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/xmlexports.h>
#include <fbxfilesdk/components/libxml2-2.6.4/include/libxml/tree.h>		// For xmlChar.h of all things. :-(

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/*
 * function types:
 */
/**
 * xmlHashDeallocator:
 * @payload:  the data in the hash
 * @name:  the name associated
 *
 * Callback to free data from a hash.
 */
typedef void (*xmlHashDeallocator)(void *payload, xmlChar *name);
/**
 * xmlHashCopier:
 * @payload:  the data in the hash
 * @name:  the name associated
 *
 * Callback to copy data from a hash.
 *
 * Returns a copy of the data or NULL in case of error.
 */
typedef void *(*xmlHashCopier)(void *payload, xmlChar *name);
/**
 * xmlHashScanner:
 * @payload:  the data in the hash
 * @data:  extra scanner data
 * @name:  the name associated
 *
 * Callback when scanning data in a hash with the simple scanner.
 */
typedef void (*xmlHashScanner)(void *payload, void *data, xmlChar *name);
/**
 * xmlHashScannerFull:
 * @payload:  the data in the hash
 * @data:  extra scanner data
 * @name:  the name associated
 * @name2:  the second name associated
 * @name3:  the third name associated
 *
 * Callback when scanning data in a hash with the full scanner.
 */
typedef void (*xmlHashScannerFull)(void *payload, void *data,
				   const xmlChar *name, const xmlChar *name2,
				   const xmlChar *name3);

/*
 * Constructor and destructor.
 */
XMLPUBFUN xmlHashTablePtr XMLCALL
			xmlHashCreate	(int size);
XMLPUBFUN void XMLCALL			
			xmlHashFree	(xmlHashTablePtr table,
					 xmlHashDeallocator f);

/*
 * Add a new entry to the hash table.
 */
XMLPUBFUN int XMLCALL			
			xmlHashAddEntry	(xmlHashTablePtr table,
		                         const xmlChar *name,
		                         void *userdata);
XMLPUBFUN int XMLCALL			
			xmlHashUpdateEntry(xmlHashTablePtr table,
		                         const xmlChar *name,
		                         void *userdata,
					 xmlHashDeallocator f);
XMLPUBFUN int XMLCALL		    
			xmlHashAddEntry2(xmlHashTablePtr table,
		                         const xmlChar *name,
		                         const xmlChar *name2,
		                         void *userdata);
XMLPUBFUN int XMLCALL			
			xmlHashUpdateEntry2(xmlHashTablePtr table,
		                         const xmlChar *name,
		                         const xmlChar *name2,
		                         void *userdata,
					 xmlHashDeallocator f);
XMLPUBFUN int XMLCALL			
			xmlHashAddEntry3(xmlHashTablePtr table,
		                         const xmlChar *name,
		                         const xmlChar *name2,
		                         const xmlChar *name3,
		                         void *userdata);
XMLPUBFUN int XMLCALL			
			xmlHashUpdateEntry3(xmlHashTablePtr table,
		                         const xmlChar *name,
		                         const xmlChar *name2,
		                         const xmlChar *name3,
		                         void *userdata,
					 xmlHashDeallocator f);

/*
 * Remove an entry from the hash table.
 */
XMLPUBFUN int XMLCALL     
			xmlHashRemoveEntry(xmlHashTablePtr table, const xmlChar *name,
                           xmlHashDeallocator f);
XMLPUBFUN int XMLCALL     
			xmlHashRemoveEntry2(xmlHashTablePtr table, const xmlChar *name,
                            const xmlChar *name2, xmlHashDeallocator f);
XMLPUBFUN int  XMLCALL    
			xmlHashRemoveEntry3(xmlHashTablePtr table, const xmlChar *name,
                            const xmlChar *name2, const xmlChar *name3,
                            xmlHashDeallocator f);

/*
 * Retrieve the userdata.
 */
XMLPUBFUN void * XMLCALL			
			xmlHashLookup	(xmlHashTablePtr table,
					 const xmlChar *name);
XMLPUBFUN void * XMLCALL			
			xmlHashLookup2	(xmlHashTablePtr table,
					 const xmlChar *name,
					 const xmlChar *name2);
XMLPUBFUN void * XMLCALL			
			xmlHashLookup3	(xmlHashTablePtr table,
					 const xmlChar *name,
					 const xmlChar *name2,
					 const xmlChar *name3);
XMLPUBFUN void * XMLCALL			
			xmlHashQLookup	(xmlHashTablePtr table,
					 const xmlChar *name,
					 const xmlChar *prefix);
XMLPUBFUN void * XMLCALL			
			xmlHashQLookup2	(xmlHashTablePtr table,
					 const xmlChar *name,
					 const xmlChar *prefix,
					 const xmlChar *name2,
					 const xmlChar *prefix2);
XMLPUBFUN void * XMLCALL			
			xmlHashQLookup3	(xmlHashTablePtr table,
					 const xmlChar *name,
					 const xmlChar *prefix,
					 const xmlChar *name2,
					 const xmlChar *prefix2,
					 const xmlChar *name3,
					 const xmlChar *prefix3);

/*
 * Helpers.
 */
XMLPUBFUN xmlHashTablePtr XMLCALL		
			xmlHashCopy	(xmlHashTablePtr table,
					 xmlHashCopier f);
XMLPUBFUN int XMLCALL			
			xmlHashSize	(xmlHashTablePtr table);
XMLPUBFUN void XMLCALL			
			xmlHashScan	(xmlHashTablePtr table,
					 xmlHashScanner f,
					 void *data);
XMLPUBFUN void XMLCALL			
			xmlHashScan3	(xmlHashTablePtr table,
					 const xmlChar *name,
					 const xmlChar *name2,
					 const xmlChar *name3,
					 xmlHashScanner f,
					 void *data);
XMLPUBFUN void XMLCALL			
			xmlHashScanFull	(xmlHashTablePtr table,
					 xmlHashScannerFull f,
					 void *data);
XMLPUBFUN void XMLCALL			
			xmlHashScanFull3(xmlHashTablePtr table,
					 const xmlChar *name,
					 const xmlChar *name2,
					 const xmlChar *name3,
					 xmlHashScannerFull f,
					 void *data);

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_LIBXML2_2_6_4_INCLUDE_LIBXML_HASH_H

