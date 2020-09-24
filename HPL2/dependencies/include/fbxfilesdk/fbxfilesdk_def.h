/*!  \file fbxfilesdk_def.h
 */

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
#ifndef FBXFILESDK_FBXFILESDK_DEF_H
#define FBXFILESDK_FBXFILESDK_DEF_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

///////////////////////////////////////////////////////////////////////////////////////
//Define FBX SDK version and global namespace
#include <fbxfilesdk/fbxfilesdk_version.h>

#define FBXFILESDK_NAMESPACE_USE

#ifndef FBXFILESDK_NAMESPACE
	#ifdef FBXFILESDK_NAMESPACE_USE
		#define FBXFILESDK_NAMESPACE FBXSDK_NAMESPACE
	#else
		#define FBXFILESDK_NAMESPACE
	#endif
#endif

///////////////////////////////////////////////////////////////////////////////////////
//Define architecture, before everything else
#include <fbxfilesdk/components/kbaselib/karch/arch.h>
#include <fbxfilesdk/components/kbaselib/karch/types.h>

///////////////////////////////////////////////////////////////////////////////////////
//Define platform standardization
#if defined(FBXSDK_SHARED) && defined(KARCH_ENV_WIN)
	#define KFBX_DLLIMPORT __declspec(dllimport)
	#define KFBX_DLLEXPORT __declspec(dllexport)
#else
	#define KFBX_DLLIMPORT
	#define KFBX_DLLEXPORT
#endif

#ifndef NULL
	#if defined __GNUG__ && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 8))
		#define NULL (__null)
	#else	
    	#ifdef __cplusplus
    		#define NULL    0
    	#else
    		#define NULL    ((void *)0)
    	#endif
    #endif
#endif

#if !defined(_MAX_PATH)
	#define _MAX_PATH 1024
#endif

#define KFBX_FORWARD( ClassName ) class ClassName; typedef ClassName *H##ClassName;
#define KFBX_FORWARD_HI( ClassName ) class ClassName; typedef ClassName *H##ClassName; typedef H##ClassName HI##ClassName;

#define KFBX_SAFE_DELETE_PTR(p)         { FbxSdkDelete(p); (p)=NULL; }
#define KFBX_SAFE_DELETE_ARRAY_PTR(a)   { FbxSdkDeleteArray(a); (a)=NULL; }
#define KFBX_SAFE_DESTROY_OBJECT(p)	    if(p){ (p)->Destroy(); (p)=NULL; }

#ifdef KARCH_ENV_WIN
	#define snprintf _snprintf // for stdio.h platform compatibility
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN  // Defined to speed up compilation
	#endif
#endif

#if !defined(KARCH_DEV_MSC)
	#ifndef strcmpi
		#define strcmpi strcasecmp
	#endif
	#ifndef stricmp
		#define stricmp strcasecmp
	#endif
	#ifndef strncmpi
		#define strncmpi strncasecmp
	#endif
	#ifndef strnicmp
		#define strnicmp strncasecmp
	#endif
#endif

#if defined(KARCH_ENV_LINUX)
	#include <malloc.h>
	#define _msize ((size_t(*)(void*))malloc_usable_size)
#endif

#if defined(KARCH_ENV_MACOSX)
	#include <malloc/malloc.h>
	#define _msize ((size_t(*)(void*))malloc_size)
#endif

///////////////////////////////////////////////////////////////////////////////////////
//Define compiler specifics
#ifdef KARCH_DEV_MSC
	#pragma warning(disable : 4251)	//'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif

#if defined(KARCH_DEV_GNUC) || defined(KARCH_DEV_GNUC_MACOSX) || defined(KARCH_DEV_GNUC_EE) // GNU compiler
    #define K_DEPRECATED __attribute__((deprecated))
#elif defined(KARCH_DEV_INTEL) // Intel compiler
    #if (__INTEL_COMPILER >= 810) // Maybe we could use an earlier version, we should check this some day
        #define K_DEPRECATED __declspec(deprecated)
    #else
        #define K_DEPRECATED
    #endif
#elif defined(KARCH_DEV_MSC) // Microsoft compiler
    #if (_MSC_VER >= 1300) // 7.1
        #define K_DEPRECATED __declspec(deprecated)
    #else
        #define K_DEPRECATED
    #endif
#elif defined(KARCH_DEV_MACOSX) // Metrowerks compiler
    #define K_DEPRECATED
#else // Unknown compiler
	#error Unknown compiler, can't define K_DECRECATED
#endif

#include <fbxfilesdk/fbxfilesdk_memory.h>

#endif // FBXFILESDK_FBXFILESDK_DEF_H

