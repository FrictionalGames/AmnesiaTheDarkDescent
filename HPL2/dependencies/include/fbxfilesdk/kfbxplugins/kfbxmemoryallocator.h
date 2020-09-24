/*!  \file kfbxmemoryallocator.h
 */
 
#ifndef FBXFILESDK_KFBXPLUGINS_KFBXMEMORYALLOCATOR_H
#define FBXFILESDK_KFBXPLUGINS_KFBXMEMORYALLOCATOR_H

/**************************************************************************************

 Copyright (C) 2001 - 2010 Autodesk, Inc. and/or its licensors.
 All Rights Reserved.

 The coded instructions, statements, computer programs, and/or related material 
 (collectively the "Data") in these files contain unpublished information 
 proprietary to Autodesk, Inc. and/or its licensors, which is protected by 
 Canada and United States of America federal copyright law and by international 
 treaties. 
 
 The Data may not be disclosed or distributed to third parties, in whole or in
 part, without the prior written consent of Autodesk, Inc. ("Autodesk").

 THE DATA IS PROVIDED "AS IS" AND WITHOUT WARRANTY.
 ALL WARRANTIES ARE EXPRESSLY EXCLUDED AND DISCLAIMED. AUTODESK MAKES NO
 WARRANTY OF ANY KIND WITH RESPECT TO THE DATA, EXPRESS, IMPLIED OR ARISING
 BY CUSTOM OR TRADE USAGE, AND DISCLAIMS ANY IMPLIED WARRANTIES OF TITLE, 
 NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE OR USE. 
 WITHOUT LIMITING THE FOREGOING, AUTODESK DOES NOT WARRANT THAT THE OPERATION
 OF THE DATA WILL BE UNINTERRUPTED OR ERROR FREE. 
 
 IN NO EVENT SHALL AUTODESK, ITS AFFILIATES, PARENT COMPANIES, LICENSORS
 OR SUPPLIERS ("AUTODESK GROUP") BE LIABLE FOR ANY LOSSES, DAMAGES OR EXPENSES
 OF ANY KIND (INCLUDING WITHOUT LIMITATION PUNITIVE OR MULTIPLE DAMAGES OR OTHER
 SPECIAL, DIRECT, INDIRECT, EXEMPLARY, INCIDENTAL, LOSS OF PROFITS, REVENUE
 OR DATA, COST OF COVER OR CONSEQUENTIAL LOSSES OR DAMAGES OF ANY KIND),
 HOWEVER CAUSED, AND REGARDLESS OF THE THEORY OF LIABILITY, WHETHER DERIVED
 FROM CONTRACT, TORT (INCLUDING, BUT NOT LIMITED TO, NEGLIGENCE), OR OTHERWISE,
 ARISING OUT OF OR RELATING TO THE DATA OR ITS USE OR ANY OTHER PERFORMANCE,
 WHETHER OR NOT AUTODESK HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH LOSS
 OR DAMAGE. 

**************************************************************************************/
#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** This class provides services for memory management.
  * \nosubgrouping
  * The FBX SDK Manager uses an object of type KFbxMemoryAllocator
  * to allocate and free memory, which is registered to KFbxSdkManager.
  * The calls of memory management routines
  * (FbxSdkMalloc, FbxSdkFree, FbxSdkNew, FbxSdkDelete, FbxSdkNewArray and FbxSdkDeleteArray)
  * will be redirected to corresponding registered functions.
  * Implement your own allocator if your application requires custom memory management.
  * \see KFbxSdkManager::SetMemoryAllocator(KFbxMemoryAllocator*)
  */
class KFBX_DLL KFbxMemoryAllocator
{
public:
	/** Constructor.
	  * \param pMallocHandler      Pointer to a function implementing malloc. This function allocates memory blocks.
	  * \param pCallocHandler      Pointer to a function implementing calloc. This function allocates an array in memory with elements initialized to 0.
	  * \param pReallocHandler     Pointer to a function implementing realloc. This function reallocate memory blocks.
      * \param pFreeHandler        Pointer to a function implementing free. This function deallocates memory blocks.
      * \param pMsizeHandler       Pointer to a function implementing msize. This function returns the size of a memory block allocated in the heap. See the macro _msize for its default implementation on different platforms.
	  */
	KFbxMemoryAllocator(void* (*pMallocHandler)(size_t),
						void* (*pCallocHandler)(size_t,size_t),
						void* (*pReallocHandler)(void*,size_t),
                        void  (*pFreeHandler)(void*),
                        size_t (*pMsizeHandler)(void*))
		: mMallocHandler(pMallocHandler)
		, mCallocHandler(pCallocHandler)
		, mReallocHandler(pReallocHandler)
        , mFreeHandler(pFreeHandler)
        , mMsizeHandler(pMsizeHandler)
		, mMallocHandler_debug(0)
		, mCallocHandler_debug(0)
		, mReallocHandler_debug(0)
		, mFreeHandler_debug(0)
        , mMsizeHandler_debug(0)
	{
	}

	/** Constructor.
      * \param pMallocHandler       Pointer to a function implementing malloc. This function allocates memory blocks.
      * \param pCallocHandler       Pointer to a function implementing calloc. This function allocates an array in memory with elements initialized to 0.
      * \param pReallocHandler      Pointer to a function implementing realloc. This function reallocate memory blocks.
      * \param pFreeHandler         Pointer to a function implementing free. This function deallocates memory blocks.
      * \param pMsizeHandler        Pointer to a function implementing msize. This function returns the size of a memory block allocated in the heap. See the macro _msize for its default implementation on different platforms.
      * \param pMallocHandler_debug Pointer to a function implementing malloc_debug. This function allocates memory blocks, and store information for debugging purposes
      * \param pCallocHandler_debug Pointer to a function implementing calloc_debug. This function allocates an array in memory with elements initialized to 0, and store information for debugging purposes.
      * \param pReallocHandler_debug Pointer to a function implementing realloc_debug. This function reallocate memory blocks, and store information for debugging purposes.
      * \param pFreeHandler_debug   Pointer to a function implementing free_debug. This function deallocates memory blocks, and store information for debugging purposes.
      * \param pMsizeHandler_debug  Pointer to a function implementing msize_debug. This function returns the size of a memory block allocated in the heap, and store information for debugging purposes.
	  */
    KFbxMemoryAllocator(void* (*pMallocHandler)(size_t),
						void* (*pCallocHandler)(size_t,size_t),
						void* (*pReallocHandler)(void*,size_t),
                        void  (*pFreeHandler)(void*),
                        size_t (*pMsizeHandler)(void*),
						void* (*pMallocHandler_debug)(size_t,int,const char *,int),
						void* (*pCallocHandler_debug)(size_t, size_t,int,const char *,int),
						void* (*pReallocHandler_debug)(void*, size_t,int,const char *,int),
						void  (*pFreeHandler_debug)(void*,int),
                        size_t (*pMsizeHandler_debug)(void*,int)
						)
		: mMallocHandler(pMallocHandler)
		, mCallocHandler(pCallocHandler)
		, mReallocHandler(pReallocHandler)
        , mFreeHandler(pFreeHandler)
        , mMsizeHandler(pMsizeHandler)
		, mMallocHandler_debug(pMallocHandler_debug)
		, mCallocHandler_debug(pCallocHandler_debug)
		, mReallocHandler_debug(pReallocHandler_debug)
		, mFreeHandler_debug(pFreeHandler_debug)
        , mMsizeHandler_debug(pMsizeHandler_debug)
	{
	}
	virtual ~KFbxMemoryAllocator() = 0;	

	void* (*mMallocHandler)(size_t);
	void* (*mCallocHandler)(size_t,size_t);
	void* (*mReallocHandler)(void*,size_t);
    void  (*mFreeHandler)(void*);
    size_t (*mMsizeHandler)(void*);
	void* (*mMallocHandler_debug)(size_t,int,const char *,int);
	void* (*mCallocHandler_debug)(size_t, size_t,int,const char *,int);
	void* (*mReallocHandler_debug)(void*, size_t,int,const char *,int);
    void  (*mFreeHandler_debug)(void*,int);
    size_t (*mMsizeHandler_debug)(void*,int);
};

/** Default implementation of memory allocator.
  * \nosubgrouping
  * This default implementation uses malloc, calloc, realloc, and free from the C runtime library.
  */
class KFBX_DLL KFbxDefaultMemoryAllocator : public KFbxMemoryAllocator
{
public:
	KFbxDefaultMemoryAllocator();
	~KFbxDefaultMemoryAllocator();
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXMEMORYALLOCATOR_H

