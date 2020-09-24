/*!  \file fbxfilesdk_new.h
 */

#ifndef FBXFILESDK_FBXFILESDK_NEW_H
#define FBXFILESDK_FBXFILESDK_NEW_H

/**************************************************************************************

 Copyright (C) 2005 - 2010 Autodesk, Inc. and/or its licensors.
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
// to define the memory allocators
//

#ifdef KARCH_DEV_MSC
	#pragma warning(push)
	#pragma warning(disable : 4345) //warning C4345: behavior change: an object of POD type constructed with an initializer of the form () will be default-initialized
	#define NAMESPACE_FRIEND_WORKAROUND
#endif

#include <new>      // for placement new
#include <stdlib.h> // for malloc

#include <fbxfilesdk/fbxfilesdk_memory.h>

#ifndef NAMESPACE_FRIEND_WORKAROUND
    #include <fbxfilesdk/fbxfilesdk_nsbegin.h>
    #ifdef FBXFILESDK_NAMESPACE_USE
        #define _FBXSDK_NAMESPACE FBXFILESDK_NAMESPACE
    #endif
#else
    #define _FBXSDK_NAMESPACE
#endif


// Type traits for primitive types
template<typename T> struct SimpleType {
    enum {value = 0};
};
template<typename T> struct SimpleType<T *> {
    enum {value = 1};
};
template<typename T> struct SimpleType<const T> {
    enum {value = SimpleType<T>::value};
};
template<typename T, size_t n> struct SimpleType<T[n]> {
    enum {value = SimpleType<T>::value};
};

#define KFBX_SIMPLE_TYPE(T)                 \
    template<> struct SimpleType<T> {       \
union {                                     \
    T t;                                    \
} catcherr;                                 \
    enum {value = 1};                       \
}

KFBX_SIMPLE_TYPE (bool);
KFBX_SIMPLE_TYPE (char);
KFBX_SIMPLE_TYPE (unsigned char);
KFBX_SIMPLE_TYPE (short);
KFBX_SIMPLE_TYPE (unsigned short);
KFBX_SIMPLE_TYPE (int);
KFBX_SIMPLE_TYPE (unsigned int);
KFBX_SIMPLE_TYPE (long);
KFBX_SIMPLE_TYPE (unsigned long);
KFBX_SIMPLE_TYPE (float);
KFBX_SIMPLE_TYPE (double);
KFBX_SIMPLE_TYPE (long double);
KFBX_SIMPLE_TYPE (long long);
KFBX_SIMPLE_TYPE (unsigned long long);

#define KFBX_IS_SIMPLE_TYPE(T) ((bool) SimpleType<T>::value)


template<typename T>
T* FbxSdkNew()
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T();
}
template<typename T, typename T1>
T* FbxSdkNew(T1& p1)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1);
}
template<typename T, typename T1>
T* FbxSdkNew(const T1& p1)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1);
}
template<typename T, typename T1, typename T2>
T* FbxSdkNew(T1& p1, T2& p2)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2);
}
template<typename T, typename T1, typename T2>
T* FbxSdkNew(T1& p1, const T2& p2)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2);
}
template<typename T, typename T1, typename T2>
T* FbxSdkNew(const T1& p1, T2& p2)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2);
}
template<typename T, typename T1, typename T2>
T* FbxSdkNew(const T1& p1, const T2& p2)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2);
}
template<typename T, typename T1, typename T2, typename T3>
T* FbxSdkNew(T1& p1, T2& p2, T3& p3)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3);
}
template<typename T, typename T1, typename T2, typename T3>
T* FbxSdkNew(T1& p1, T2& p2, const T3& p3)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3);
}
template<typename T, typename T1, typename T2, typename T3>
T* FbxSdkNew(T1& p1, const T2& p2, T3& p3)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3);
}
template<typename T, typename T1, typename T2, typename T3>
T* FbxSdkNew(T1& p1, const T2& p2, const T3& p3)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3);
}
template<typename T, typename T1, typename T2, typename T3>
T* FbxSdkNew(const T1& p1, T2& p2, T3& p3)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3);
}
template<typename T, typename T1, typename T2, typename T3>
T* FbxSdkNew(const T1& p1, T2& p2, const T3& p3)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3);
}
template<typename T, typename T1, typename T2, typename T3>
T* FbxSdkNew(const T1& p1, const T2& p2, T3& p3)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3);
}
template<typename T, typename T1, typename T2, typename T3>
T* FbxSdkNew(const T1& p1, const T2& p2, const T3& p3)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3);
}
template<typename T, typename T1, typename T2, typename T3, typename T4>
T* FbxSdkNew(T1& p1, T2& p2, T3& p3, T4& p4)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3, p4);
}
template<typename T, typename T1, typename T2, typename T3, typename T4>
T* FbxSdkNew(T1& p1, T2& p2, T3& p3, const T4& p4)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3, p4);
}
template<typename T, typename T1, typename T2, typename T3, typename T4>
T* FbxSdkNew(T1& p1, T2& p2, const T3& p3, T4& p4)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3, p4);
}
template<typename T, typename T1, typename T2, typename T3, typename T4>
T* FbxSdkNew(T1& p1, T2& p2, const T3& p3, const T4& p4)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3, p4);
}
template<typename T, typename T1, typename T2, typename T3, typename T4>
T* FbxSdkNew(T1& p1, const T2& p2, T3& p3, T4& p4)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3, p4);
}
template<typename T, typename T1, typename T2, typename T3, typename T4>
T* FbxSdkNew(T1& p1, const T2& p2, T3& p3, const T4& p4)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3, p4);
}
template<typename T, typename T1, typename T2, typename T3, typename T4>
T* FbxSdkNew(T1& p1, const T2& p2, const T3& p3, T4& p4)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3, p4);
}
template<typename T, typename T1, typename T2, typename T3, typename T4>
T* FbxSdkNew(T1& p1, const T2& p2, const T3& p3, const T4& p4)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3, p4);
}
template<typename T, typename T1, typename T2, typename T3, typename T4>
T* FbxSdkNew(const T1& p1, T2& p2, T3& p3, T4& p4)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3, p4);
}
template<typename T, typename T1, typename T2, typename T3, typename T4>
T* FbxSdkNew(const T1& p1, T2& p2, T3& p3, const T4& p4)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3, p4);
}
template<typename T, typename T1, typename T2, typename T3, typename T4>
T* FbxSdkNew(const T1& p1, T2& p2, const T3& p3, T4& p4)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3, p4);
}
template<typename T, typename T1, typename T2, typename T3, typename T4>
T* FbxSdkNew(const T1& p1, T2& p2, const T3& p3, const T4& p4)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3, p4);
}
template<typename T, typename T1, typename T2, typename T3, typename T4>
T* FbxSdkNew(const T1& p1, const T2& p2, T3& p3, T4& p4)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3, p4);
}
template<typename T, typename T1, typename T2, typename T3, typename T4>
T* FbxSdkNew(const T1& p1, const T2& p2, T3& p3, const T4& p4)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3, p4);
}
template<typename T, typename T1, typename T2, typename T3, typename T4>
T* FbxSdkNew(const T1& p1, const T2& p2, const T3& p3, T4& p4)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3, p4);
}
template<typename T, typename T1, typename T2, typename T3, typename T4>
T* FbxSdkNew(const T1& p1, const T2& p2, const T3& p3, const T4& p4)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1, p2, p3, p4);
}
template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
T* FbxSdkNew(const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1,p2,p3,p4,p5);
}
template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
T* FbxSdkNew(const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1,p2,p3,p4,p5,p6);
}
template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
T* FbxSdkNew(const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1,p2,p3,p4,p5,p6,p7);
}
template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
T* FbxSdkNew(const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7, const T8& p8)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1,p2,p3,p4,p5,p6,p7,p8);
}
template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
T* FbxSdkNew(const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7, const T8& p8, const T9& p9)
{
    T* p = (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T));
    return new(p)T(p1,p2,p3,p4,p5,p6,p7,p8,p9);
}

template<typename T>
void FbxSdkDelete(T* p)
{
    if (p)
    {
        ((T*)p)->~T();
        FBXFILESDK_NAMESPACE::FbxSdkFree(p);
    }
}

template<typename T>
void FbxSdkDelete(const T* p)
{
    if (p)
    {
        ((T*)p)->~T();
        FBXFILESDK_NAMESPACE::FbxSdkFree(const_cast<T*>(p));
    }
}

template<typename T>
T* FbxSdkNewArray(int n)
{
    if (KFBX_IS_SIMPLE_TYPE(T))
    {
        return (T*)FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T)*n);
    }
    else
    {
        void *pTmp = FBXFILESDK_NAMESPACE::FbxSdkMalloc(sizeof(T)*n+sizeof(int));
        T* p = (T*)((int*)pTmp+1);
        *((int*)pTmp) = n;
        for(int i=0;i<n;++i)
            new((T*)p+i)T;
        return p;
    }
}

template<typename T>
void FbxSdkDeleteArray(T* p)
{
    if (p)
    {
        if (!KFBX_IS_SIMPLE_TYPE(T))
        {
            for(int i=0;i<((int*)p)[-1];++i)
                ((T*)p)[i].~T();
            FBXFILESDK_NAMESPACE::FbxSdkFree((int*)p-1);
        }
        else
        {
            FBXFILESDK_NAMESPACE::FbxSdkFree((void*)p);
        }
    }
}


#define KFBXNEW_DECLARE_FRIEND        \
template<typename T>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew();        \
template<typename T, typename T1>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(T1& p1);        \
template<typename T, typename T1>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1);        \
template<typename T, typename T1, typename T2>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(T1& p1, T2& p2);        \
template<typename T, typename T1, typename T2>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(T1& p1, const T2& p2);        \
template<typename T, typename T1, typename T2>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, T2& p2);        \
template<typename T, typename T1, typename T2>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, const T2& p2);        \
template<typename T, typename T1, typename T2, typename T3>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(T1& p1, T2& p2, T3& p3);        \
template<typename T, typename T1, typename T2, typename T3>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(T1& p1, T2& p2, const T3& p3);        \
template<typename T, typename T1, typename T2, typename T3>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(T1& p1, const T2& p2, T3& p3);        \
template<typename T, typename T1, typename T2, typename T3>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(T1& p1, const T2& p2, const T3& p3);        \
template<typename T, typename T1, typename T2, typename T3>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, T2& p2, T3& p3);        \
template<typename T, typename T1, typename T2, typename T3>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, T2& p2, const T3& p3);        \
template<typename T, typename T1, typename T2, typename T3>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, const T2& p2, T3& p3);        \
template<typename T, typename T1, typename T2, typename T3>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, const T2& p2, const T3& p3);        \
template<typename T, typename T1, typename T2, typename T3, typename T4>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(T1& p1, T2& p2, T3& p3, T4& p4);        \
template<typename T, typename T1, typename T2, typename T3, typename T4>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(T1& p1, T2& p2, T3& p3, const T4& p4);        \
template<typename T, typename T1, typename T2, typename T3, typename T4>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(T1& p1, T2& p2, const T3& p3, T4& p4);        \
template<typename T, typename T1, typename T2, typename T3, typename T4>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(T1& p1, T2& p2, const T3& p3, const T4& p4);        \
template<typename T, typename T1, typename T2, typename T3, typename T4>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(T1& p1, const T2& p2, T3& p3, T4& p4);        \
template<typename T, typename T1, typename T2, typename T3, typename T4>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(T1& p1, const T2& p2, T3& p3, const T4& p4);        \
template<typename T, typename T1, typename T2, typename T3, typename T4>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(T1& p1, const T2& p2, const T3& p3, T4& p4);        \
template<typename T, typename T1, typename T2, typename T3, typename T4>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(T1& p1, const T2& p2, const T3& p3, const T4& p4);        \
template<typename T, typename T1, typename T2, typename T3, typename T4>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, T2& p2, T3& p3, T4& p4);        \
template<typename T, typename T1, typename T2, typename T3, typename T4>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, T2& p2, T3& p3, const T4& p4);        \
template<typename T, typename T1, typename T2, typename T3, typename T4>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, T2& p2, const T3& p3, T4& p4);        \
template<typename T, typename T1, typename T2, typename T3, typename T4>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, T2& p2, const T3& p3, const T4& p4);        \
template<typename T, typename T1, typename T2, typename T3, typename T4>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, const T2& p2, T3& p3, T4& p4);        \
template<typename T, typename T1, typename T2, typename T3, typename T4>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, const T2& p2, T3& p3, const T4& p4);        \
template<typename T, typename T1, typename T2, typename T3, typename T4>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, const T2& p2, const T3& p3, T4& p4);        \
template<typename T, typename T1, typename T2, typename T3, typename T4>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, const T2& p2, const T3& p3, const T4& p4);        \
template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5);        \
template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6);        \
template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7);        \
template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7, const T8& p8);        \
template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>        \
friend T* _FBXSDK_NAMESPACE::FbxSdkNew(const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7, const T8& p8, const T9& p9);        \
\
template<typename T>    \
friend void _FBXSDK_NAMESPACE::FbxSdkDelete(T* p);      \
template<typename T>    \
friend void _FBXSDK_NAMESPACE::FbxSdkDelete(const T* p);      \
template<typename T>    \
friend T* _FBXSDK_NAMESPACE::FbxSdkNewArray(int n);     \
template<typename T>    \
friend void _FBXSDK_NAMESPACE::FbxSdkDeleteArray(T* p); \

#ifdef KARCH_DEV_MSC
	#pragma warning(pop)
#endif

#ifndef NAMESPACE_FRIEND_WORKAROUND
	#include <fbxfilesdk/fbxfilesdk_nsend.h>
#endif

#endif // FBXFILESDK_FBXFILESDK_NEW_H

