/*!  \file kscopedptr.h
 */

#ifndef FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KSCOPEDPTR_H
#define FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KSCOPEDPTR_H

/**************************************************************************************

 Copyright (C) 2001 - 2009 Autodesk, Inc. and/or its licensors.
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

// FBX includes
#include "kdebug.h"

// Begin FBX namespace
#include <fbxfilesdk/fbxfilesdk_nsbegin.h> // namespace

//
//  KScopedPtr mimics a built-in pointer except that it guarantees deletion
//  of the object pointed to, either on destruction of the KScopedPtr or via
//  an explicit Reset()
//

//
// Deletion policy dictates the way the pointer is destroyed
// By default, KScopedPtr uses the DefaultDeletionPolicy
//
template<class T>
class DefaultDeletionPolicy
{
public:
    static inline void DeleteIt(T** ptr)
    {
        if ( *ptr != NULL )
        {
            delete *ptr;
            *ptr = NULL;
        }
    }
};

template<class T>
class FreeDeletionPolicy
{
public:
    static inline void DeleteIt(T** ptr)
    {
        if ( *ptr != NULL )
        {
            FbxSdkFree( *ptr );
            *ptr = NULL;
        }
    }
};

//---------------------------------------------------------------------
template<class T, class DeletionPolicyT = DefaultDeletionPolicy<T> >
class KScopedPtr
{
private:
    T* ptr;

    // Non copyable object
    KScopedPtr(KScopedPtr const &);
    KScopedPtr& operator=(KScopedPtr const &);

    typedef KScopedPtr<T, DeletionPolicyT> ThisType;
    typedef DeletionPolicyT DeletionPolicy;

public:
    //////////////////////////////
    explicit KScopedPtr(T* p = 0): ptr(p){}

    //////////////////////////////
    ~KScopedPtr()
    {
        DeletionPolicy::DeleteIt(&ptr);
    }

    //////////////////////////////
    inline void Reset(T* p = 0)
    {
        K_ASSERT(p == 0 || p != ptr); // catch self-reset errors
        ThisType(p).Swap(*this);
    }

    //////////////////////////////
    inline T & operator*() const
    {
        K_ASSERT(ptr != 0);
        return *ptr;
    }

    //////////////////////////////
    inline T* operator->() const
    {
        K_ASSERT(ptr != 0);
        return ptr;
    }

    //////////////////////////////
    inline T* Get() const
    {
        return ptr;
    }

    inline operator T* () const
    {
        return ptr;
    }

    //////////////////////////////
    // Implicit conversion to "bool"
    operator bool () const
    {
        return ptr != 0;
    }

    //////////////////////////////
    bool operator! () const
    {
        return ptr == 0;
    }

    //////////////////////////////
    void Swap(KScopedPtr & b)
    {
        T * tmp = b.ptr;
        b.ptr = ptr;
        ptr = tmp;
    }

    //////////////////////////////
    T* Release()
    {
        T* tmp = ptr;
        ptr = NULL;

        return tmp;
    }
};

//----------------------------------------
//
// Deletion policy dictates the way the pointer is destroyed
// The FBXObjectDeletionPolicy, dictate the way we destroy
// KFbxObject. This policy is used by KFBXObjectScopedPtr
//
template <class FBXObjectT>
class FBXObjectDeletionPolicy
{
public:
    static inline void DeleteIt(FBXObjectT** ptr)
    {
        if (*ptr != NULL)
        {
            (*ptr)->Destroy();
            *ptr = NULL;
        }
    }
};

template <class FBXObjectT>
class KFBXObjectScopedPtr: public KScopedPtr<FBXObjectT, FBXObjectDeletionPolicy<FBXObjectT> >
{
public:
    explicit KFBXObjectScopedPtr(FBXObjectT* p = 0):KScopedPtr<FBXObjectT, FBXObjectDeletionPolicy<FBXObjectT> >(p){}
};

//---------------------------------


template <class T>
class FbxSdkDeletionPolicy
{
public:
    static inline void DeleteIt(T** ptr)
    {
        if (*ptr != NULL)
        {
            FbxSdkDelete(*ptr);
            *ptr = NULL;
        }
    }
};

//---------------------------------
template <class T>
class KFbxSdkScopedPtr: public KScopedPtr<T, FbxSdkDeletionPolicy<T> >
{
public:
    explicit KFbxSdkScopedPtr(T* p = 0):KScopedPtr<T, FbxSdkDeletionPolicy<T> >(p){}
};

// End FBX namespace
#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KSCOPEDPTR_H

