/*!  \file kdebug.h
 */

#ifndef FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KDEBUG_H
#define FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KDEBUG_H
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

#include <fbxfilesdk/components/kbaselib/klib/ktime.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>


    /**
      *\name Constants
      */
    //@{

    /** Tracing disabled, this level must not be used with the KTrace function/
      */
    #define  K_NO_TRACE          0  

    /** Show only assert traces
      */
    #define  K_ASSERT_ONLY_TRACE 1   

    /** Descriptive trace
      */
    #define  K_NORMAL_TRACE      3   

    /** Highly detailed level of trace
      */
    #define  K_ALL_TRACE         4  

    /**Default value, K_TRACE_LEVEL 
      *environment variable is used when level TraceLevel is set to that value
      *Never use this value as an argument to KTrace
      */
    #define  K_DEFAULT_TRACE     20          

    //@}

    // Functions prototypes
    KFBX_DLL   void KRegisterAssertStubFct(int (*pFct)(const char *pFileName, const char *pFunctionName, kULong pLineNumber, 
                                                        const char *pMessage,  bool pAbortEnabled));
    KFBX_DLL   void* KGetAssertStubFct();
    KFBX_DLL   void KAssertFailed( const char *pFileName, const char *pFunctionName, kULong pLineNumber, const char *pMessage=NULL,bool *pHideForEver=NULL );
    KFBX_DLL   void KAssertFailedWithLevel( const char *pFileName, const char *pFunctionName, kULong pLineNumber, 
                                            kULong pLevel, const char *pMessage=NULL,bool *pHideForEver=NULL );

    KFBX_DLL   void ShowLastError( char* pOperation );

    KFBX_DLL   void KTraceFnc(char* pStr, ... );
    inline void KNoTraceFnc( char* /*pStr*/, ... ) {}

    KFBX_DLL	bool KIsAssertDisplayed();

    /** To debug in both Debug and Release, all calls to this function must be removed before shipping.
    */
    #define KAlwaysTrace	KTraceFnc

    /**  Add another indirection level, so that in can work in conjunction with the __FILE__ usage.
    */
    #define KSTRINGITIZE(a) (a)

    // Macros
    #ifdef _DEBUG

        #ifndef __MWERKS__
    	    #define __func__	KSTRINGITIZE("")
        #endif

        #undef KFBX_ASSERT_NAMESPACE

        #if !defined(FBXFILESDK_NAMESPACE_USE)
        #   define KFBX_ASSERT_NAMESPACE(a)     a
        #else
        #   define KFBX_ASSERT_NAMESPACE(a)     FBXFILESDK_NAMESPACE::a
        #endif

	    #define K_ASSERT( pCondition )   \
						    if( !(pCondition) )\
						    {\
							    static bool sHideForEver=false; \
                                if (!sHideForEver) KFBX_ASSERT_NAMESPACE(KAssertFailed)( KSTRINGITIZE(__FILE__), __func__, __LINE__, KSTRINGITIZE(#pCondition),&sHideForEver ); \
						    }((void)0)

	    #define K_ASSERT_MSG_NOW( pMessage )   \
						    { \
							    static bool sHideForEver=false; \
							    if (!sHideForEver) KFBX_ASSERT_NAMESPACE(KAssertFailed)( KSTRINGITIZE(__FILE__), __func__, __LINE__,  pMessage,&sHideForEver ); \
						    }((void)0)

	    #define K_ASSERT_MSG( pCondition, pMessage )   \
						    if( !(pCondition) )\
						    {\
							    static bool sHideForEver=false; \
							    if (!sHideForEver) KFBX_ASSERT_NAMESPACE(KAssertFailed)( KSTRINGITIZE(__FILE__), __func__, __LINE__,  pMessage,&sHideForEver ); \
						    }((void)0)

	    #define K_ASSERT_MSG_NOT_IMPLEMENTED \
						    { \
							    static bool sHideForEver=false; \
							    if (!sHideForEver) KFBX_ASSERT_NAMESPACE(KAssertFailed)( KSTRINGITIZE(__FILE__), __func__, __LINE__,  "Not implemented",&sHideForEver ); \
						    }((void)0)

        template <bool x> struct k_static_assert_type;
        template<> struct k_static_assert_type<true>  { enum { value = 1  }; };
        // will force a compile error when attempting to do an array of this size
        template<> struct k_static_assert_type<false> { enum { value = -1 }; };   

        #define K_STATIC_ASSERT( pCondition )  \
                            typedef char build_break_if_false[ k_static_assert_type<(bool)(pCondition)>::value ];

	    // To debug in both Debug only
	    #define KTrace			KTraceFnc


    #else
	    #define K_ASSERT( pCondition )								((void)0)
	    #define K_ASSERT_MSG_NOW( pMessage )   						((void)0)
	    #define K_ASSERT_MSG(pCondition,  pMessage )				((void)0)
	    #define K_ASSERT_MSG_NOT_IMPLEMENTED						((void)0)
        #define K_STATIC_ASSERT( pCondition )
	    // To debug in both Debug only
	    #define KTrace			KNoTraceFnc


    #endif    

    struct KDebugTimeEvent {
	    KTime   EventTime;
	    KTime   EventWantedtime;
	    int    Id;
	    int    ToId;
	    float  Color[3];
	    char  InternalComment[52];
	    int    UserId;
	    char* Comment;
    };

    enum { KTRACE_SYNC=-1,KTRACE_START=-1,KTRACE_END=-2 };

    KFBX_DLL   void KTimerTrace_Start();
    KFBX_DLL   int  KTimerTrace_GetId(char* pName, bool pProfile=false);
    KFBX_DLL   int  KTimerTrace (int pId,KTime pTime,KTime pScheduledTime,int LastId,float R,float G,float B,char* pStr,bool pCopy=false);
    KFBX_DLL   int  KTimerPrintf(int pId,KTime pTime,KTime pScheduledTime,int LastId,float R,float G,float B,char* pStr, ...);
    KFBX_DLL   void KTimerTrace_Stop();
    KFBX_DLL   void KTimerTrace_Release();
    KFBX_DLL   int	 KTimerTrace_GetEventCount();
    KFBX_DLL   KDebugTimeEvent *KTimerTrace_GetEvent(int pIndex);

    KFBX_DLL   int		KTimerTrace_GetIdCount();
    KFBX_DLL   char* KTimerTrace_GetIdName(int pId);

/** Checks the condition, if it evaluates to false
    asserts and returns value.
  */
#define K_CHECKRTNV( condition, value )     \
    if( !(condition) ){                     \
        K_ASSERT_MSG_NOW( #condition );     \
        return value;                       \
    } 

/** Checks the condition, if it evaluates to false
    asserts and returns.
  */
#define K_CHECKRTN( condition )             \
    if( !(condition) ){                     \
        K_ASSERT_MSG_NOW( #condition );     \
        return;                             \
    } 

// Type traits for class KArrayTemplate
template<typename T> struct KFbxIncompatibleWithKArrayTemplate {
    enum {value = 0};
};


#define KFBX_INCOMPATIBLE_WITH_KARRAYTEMPLATE_TEMPLATE(T)       \
    struct KFbxIncompatibleWithKArrayTemplate< T > {            \
        union {                                                 \
            T t();                                              \
        } catcherr;                                             \
        enum {value = 1};                                       \
    }

#define KFBX_INCOMPATIBLE_WITH_KARRAYTEMPLATE(T)                \
    template<> KFBX_INCOMPATIBLE_WITH_KARRAYTEMPLATE_TEMPLATE(T)


#define KFBX_IS_INCOMPATIBLE_WITH_KARRAYTEMPLATE(T) ((bool) KFbxIncompatibleWithKArrayTemplate<T>::value)


#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KDEBUG_H

