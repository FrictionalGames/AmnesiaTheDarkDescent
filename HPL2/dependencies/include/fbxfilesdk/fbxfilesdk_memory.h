/*!  \file fbxfilesdk_memory.h
 */

#ifndef FBXFILESDK_FBXFILESDK_MEMORY_H
#define FBXFILESDK_FBXFILESDK_MEMORY_H

/**************************************************************************************

 Copyright (C) 2005 - 2006 Autodesk, Inc. and/or its licensors.
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
//
// This file must be included before any other file in the FBX SDK in order
// to re-define the memory allocators
//
#include <fbxfilesdk/components/kbaselib/karch/arch.h>
#include <fbxfilesdk/components/kbaselib/karch/types.h>

#if defined(_DEBUG) && !defined(NDEBUG)
    #if defined(_MSC_VER) && !defined(_MFC_VER)
        #include <crtdbg.h>
    #endif
#endif

// Protect the standard C function declaration from macros defined below
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#ifdef KARCH_ENV_WIN
    #include <mbstring.h>
#endif

#ifndef KARCH_DEV_MACOSX
	#include <malloc.h>
#endif

#include <new>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

#undef FbxSdkMsize

#ifndef KFBX_DLL	
	#define KFBX_DLL KFBX_DLLIMPORT	
#endif

// If we are compiling the FBX plugins or the public FBX SDK, we want to
// use the FBX SDK internal memory allocator instead of the standard one.
KFBX_DLL void* FbxSdkMalloc(size_t pBlockSize);
KFBX_DLL void* FbxSdkCalloc(size_t pCount, size_t pBlockSize);
KFBX_DLL void* FbxSdkRealloc(void* pData, size_t pBlockSize);
KFBX_DLL void  FbxSdkFree(void* pData);
KFBX_DLL size_t FbxSdkMsize(void* pData);
KFBX_DLL void* FbxSdkMalloc_Debug(size_t pBlockSize,int _BlockType,const char * _Filename,int _LineNumber);
KFBX_DLL void* FbxSdkCalloc_Debug(size_t pCount, size_t pBlockSize,int _BlockType,const char * _Filename,int _LineNumber);
KFBX_DLL void* FbxSdkRealloc_Debug(void* pData, size_t pBlockSize,int _BlockType,const char * _Filename,int _LineNumber);
KFBX_DLL void  FbxSdkFree_Debug(void* pData,int _BlockType);
KFBX_DLL size_t FbxSdkMsize_Debug(void* pData,int _BlockType);

KFBX_DLL char* FbxStrDup(const char* pString);
KFBX_DLL char* FbxStrDup_Debug(const char* pString, int _BlockType, const char* _Filename, int _LineNumber);
KFBX_DLL wchar_t* FbxStrDupWC(const wchar_t* pString);
KFBX_DLL wchar_t* FbxStrDupWC_Debug(const wchar_t* pString, int _BlockType, const char* _Filename, int _LineNumber);
KFBX_DLL unsigned char* FbxStrDupMB(const unsigned char* pString);
KFBX_DLL unsigned char* FbxStrDupMB_Debug(const unsigned char* pString, int _BlockType, const char* _Filename, int _LineNumber);

// Functions to overwrite the default allocator functions
KFBX_DLL void SetMallocHandler(void* (*pHandler)(size_t));
KFBX_DLL void SetCallocHandler(void* (*pHandler)(size_t,size_t));
KFBX_DLL void SetReallocHandler(void* (*pHandler)(void*,size_t));
KFBX_DLL void SetFreeHandler(void (*pHandler)(void*));
KFBX_DLL void SetMsizeHandler(size_t (*pHandler)(void*));

KFBX_DLL void SetMallocHandler_Debug(void* (*pHandler)(size_t,int,const char *,int));
KFBX_DLL void SetCallocHandler_Debug(void* (*pHandler)(size_t,size_t,int,const char *,int));
KFBX_DLL void SetReallocHandler_Debug(void* (*pHandler)(void*,size_t,int,const char *,int));
KFBX_DLL void SetFreeHandler_Debug(void (*pHandler)(void*,int));
KFBX_DLL void SetMsizeHandler_Debug(size_t (*pHandler)(void*,int));


class KFBX_DLL KFbxPointerFreeGuard
{
public:
    KFbxPointerFreeGuard(void* p = NULL):mPtr(p)
    {
    }
    ~KFbxPointerFreeGuard()
    {
        FbxSdkFree(mPtr);
        mPtr = NULL;
    }
    void SetPointer (void* p)
    {
        mPtr = p;
    }
private:
    void *mPtr;
}; 


#undef strdup
#undef _strdup
#undef _wcsdup
#undef _mbsdup

#if defined(_DEBUG) && defined(KARCH_ENV_WIN)
    #define strdup(s)   FBXFILESDK_NAMESPACE::FbxStrDup_Debug((s), _NORMAL_BLOCK, __FILE__, __LINE__)
    #define _strdup(s)  FBXFILESDK_NAMESPACE::FbxStrDup_Debug((s), _NORMAL_BLOCK, __FILE__, __LINE__)
    #define _wcsdup(s)  FBXFILESDK_NAMESPACE::FbxStrDupWC_Debug((s), _NORMAL_BLOCK, __FILE__, __LINE__)
    #define _mbsdup(s)  FBXFILESDK_NAMESPACE::FbxStrDupMB_Debug((s), _NORMAL_BLOCK, __FILE__, __LINE__)
#else
    #define strdup(s)   FBXFILESDK_NAMESPACE::FbxStrDup((s))
    #define _strdup(s)  FBXFILESDK_NAMESPACE::FbxStrDup((s))
    #define _wcsdup(s)  FBXFILESDK_NAMESPACE::FbxStrDupWC((s))
    #define _mbsdup(s)  FBXFILESDK_NAMESPACE::FbxStrDupMB((s))
#endif

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#include <fbxfilesdk/fbxfilesdk_new.h>

#ifndef MB_FBXSDK

    // 
    // In the FBX SDK, We redefine the m/c/realloc/free functions and the new/delete operators so the
    // FbxSdkManager can manage all the memory allocation/deallocation. 
    //
    #undef FBXSDK_NEW_REDIRECTED
    #define FBXSDK_NEW_REDIRECTED

    #undef MEMORY_DEBUG_FBXFILESDK_MEMORY

    #ifndef DISABLE_FBXSDK_ALLOCATOR
        #if defined(_DEBUG) && !defined(NDEBUG)
            #define MEMORY_DEBUG_FBXFILESDK_MEMORY
        #endif
    #endif

    #ifdef DISABLE_FBXSDK_ALLOCATOR
        #define   FbxSdkMalloc(s)       malloc(s)         
        #define   FbxSdkCalloc(c, s)    calloc(c, s)      
        #define   FbxSdkRealloc(p, s)   realloc(p, s)     
        #define   FbxSdkFree(p)         free(p)           
        #define   FbxSdkMsize(p)        _msize(p)
    #else
        #if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
            #define   FbxSdkMalloc(s)       FbxSdkMalloc_Debug(s, _NORMAL_BLOCK, __FILE__, __LINE__)
            #define   FbxSdkCalloc(c, s)    FbxSdkCalloc_Debug(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
            #define   FbxSdkRealloc(p, s)   FbxSdkRealloc_Debug(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
            #define   FbxSdkFree(p)         FbxSdkFree_Debug(p, _NORMAL_BLOCK)
            #define   FbxSdkMsize(p)        FbxSdkMsize_Debug(p, _NORMAL_BLOCK)
        #endif
    #endif

#endif /* MB_FBXSDK */

#endif // FBXFILESDK_FBXFILESDK_MEMORY_H


