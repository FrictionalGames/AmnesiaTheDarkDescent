/*!  \file kmap.h
 */

#ifndef FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KMAP_H
#define FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KMAP_H

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
#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/components/kbaselib/klib/kpair.h>
#include <fbxfilesdk/components/kbaselib/klib/kcontainerallocators.h>
#include <fbxfilesdk/components/kbaselib/klib/kdebug.h>

#include <new>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/* Examples of KEY_COMPARE_FUNCTOR class

   with KEY_TYPE = int
    class IntCompare {
        inline int operator()(int pKeyA, int pKeyB) const
        {
            return (pKeyA < pKeyB) ? -1 : ((pKeyB < pKeyA) ? 1 : 0);
        }
    };

   with KEY_TYPE = Class
    class ClassCompare {
        inline int operator()(Class const& pKeyA, Class const& pKeyB) const
        {
            return (pKeyA < pKeyB) ? -1 : ((pKeyB < pKeyA) ? 1 : 0);
        }
    };

   with KEY_TYPE = char*
    class StrCompare {
        inline int operator()(char const* pKeyA, char const* pKeyB) const
        {
            return strcmp(pKeyA, pKeyB);
        }
    };

*/

template <typename RecordType>    class RedBack_ConstIteratorType;
template <typename RecordType>    class RedBack_IteratorType;

template <typename RecordType>

/** Implements an efficient ordered data storage.
  */
class RedBack_IteratorType
{
public:
    RedBack_IteratorType() : mRecord(0) {}
    RedBack_IteratorType(RecordType* pRecord) : mRecord(pRecord) {}
    RedBack_IteratorType(RedBack_IteratorType<RecordType> const& pV) : mRecord(pV.mRecord) {}

    // prefix ++
    RedBack_IteratorType & operator++()
    {
        K_ASSERT( mRecord != NULL );
        mRecord = mRecord->Successor();
        return *this;
    }

    // postfix ++
    const RedBack_IteratorType operator++(int)
    {
        RedBack_IteratorType t(*this);
        operator++();
        return t;
    }

    // prefix --
    RedBack_IteratorType & operator--()
    {
        K_ASSERT( mRecord );
        mRecord = mRecord->Predecessor();
        return *this;
    }

    // postfix --
    const RedBack_IteratorType operator--(int)
    {
        RedBack_IteratorType t(*this);
        operator--();
        return t;
    }

    RecordType const& operator*() const
    {
        K_ASSERT( mRecord );

        return *mRecord;
    }

    RecordType & operator*()
    {
        K_ASSERT( mRecord );

        return *mRecord;
    }

    RecordType const* operator->() const
    {
        K_ASSERT( mRecord );

        return mRecord;
    }

    RecordType* operator->()
    {
        K_ASSERT( mRecord );

        return mRecord;
    }

    inline bool operator==(const RedBack_IteratorType& pOther) const
    {
        return mRecord == pOther.mRecord;
    }

    inline bool operator !=(const RedBack_IteratorType& pOther) const
    {
        return mRecord != pOther.mRecord;
    }

protected:
    friend class RedBack_ConstIteratorType<RecordType>;

    RecordType* mRecord;
};

template <typename RecordType>
class RedBack_ConstIteratorType
{
public:
    RedBack_ConstIteratorType() : mRecord(0) {}
    RedBack_ConstIteratorType(const RecordType* pRecord) : mRecord(pRecord) {}
    RedBack_ConstIteratorType(RedBack_IteratorType<RecordType> const& pV) : mRecord(pV.mRecord) {}
    RedBack_ConstIteratorType(RedBack_ConstIteratorType<RecordType> const& pV) : mRecord(pV.mRecord) {}

    // prefix ++
    RedBack_ConstIteratorType & operator++()
    {
        K_ASSERT( mRecord != NULL );
        mRecord = mRecord->Successor();
        return *this;
    }

    // postfix ++
    const RedBack_ConstIteratorType operator++(int)
    {
        RedBack_ConstIteratorType t(*this);
        operator++();
        return t;
    }

    // prefix --
    RedBack_ConstIteratorType & operator--()
    {
        K_ASSERT( mRecord );
        mRecord = mRecord->Predecessor();
        return *this;
    }

    // postfix --
    const RedBack_ConstIteratorType operator--(int)
    {
        RedBack_ConstIteratorType t(*this);
        operator--();
        return t;
    }

    RecordType const& operator*() const
    {
        K_ASSERT( mRecord );

        return *mRecord;
    }

    RecordType const& operator*()
    {
        K_ASSERT( mRecord );

        return *mRecord;
    }

    RecordType const* operator->() const
    {
        K_ASSERT( mRecord );

        return mRecord;
    }

    RecordType const* operator->()
    {
        K_ASSERT( mRecord );

        return mRecord;
    }

    inline bool operator==(const RedBack_ConstIteratorType& pOther) const
    {
        return mRecord == pOther.mRecord;
    }

    inline bool operator !=(const RedBack_ConstIteratorType& pOther) const
    {
        return mRecord != pOther.mRecord;
    }

protected:
    friend class RedBack_IteratorType<RecordType>;

    RecordType const* mRecord;
};

/**Implements an efficient ordered data storage.
  */
template <typename DATA_TYPE,
          typename KEY_COMPARE_FUNCTOR,
          typename ALLOCATOR>
class  KRedBlackTree
{
public:
    typedef DATA_TYPE DataType;
    typedef typename DATA_TYPE::KeyType         KeyType;
    typedef typename DATA_TYPE::ConstKeyType    ConstKeyType;
    typedef typename DATA_TYPE::ValueType       ValueType;
    typedef typename DATA_TYPE::ConstValueType  ConstValueType;
    typedef ALLOCATOR AllocatorType;

    /**
       This class represents a node in the tree. It contains the key,
       the value, and internal tree management data.
    */
    class RecordType
    {
    public:
        inline ConstKeyType& GetKey() const { return mData.GetKey(); }
        inline ConstValueType& GetValue() const { return mData.GetValue(); }
        inline ValueType& GetValue() { return mData.GetValue(); }

        inline RecordType const* Minimum() const
        {
            RecordType const* lParent = 0;
            RecordType const* lNode = this;
            while (lNode != 0)
            {
                lParent = lNode;
                lNode = lNode->mLeftChild;
            }

            return lParent;
        }

        inline RecordType* Minimum()
        {
            RecordType* lParent = 0;
            RecordType* lNode = this;
            while (lNode != 0)
            {
                lParent = lNode;
                lNode = lNode->mLeftChild;
            }

            return lParent;
        }

        inline RecordType const* Maximum() const
        {
            RecordType const* lParent = 0;
            RecordType const* lNode = this;
            while (lNode != 0)
            {
                lParent = lNode;
                lNode = lNode->mRightChild;
            }

            return lParent;
        }

        inline RecordType* Maximum()
        {
            RecordType* lParent = 0;
            RecordType* lNode = this;
            while (lNode != 0)
            {
                lParent = lNode;
                lNode = lNode->mRightChild;
            }

            return lParent;
        }

        inline RecordType const* Predecessor() const
        {
            if (mLeftChild)
            {
                return mLeftChild->Maximum();
            }
            else
            {
                RecordType const* lParent = mParent;
                RecordType const* lNode = this;

                while (lParent && lParent->mLefttChild == lNode)
                {
                    lNode = lParent;
                    lParent = lParent->mParent;
                }

                return lParent;
            }
        }

        inline RecordType* Predecessor()
        {
            if (mLeftChild)
            {
                return mLeftChild->Maximum();
            }
            else
            {
                RecordType* lParent = mParent;
                RecordType* lNode = this;

                while (lParent && lParent->mLeftChild == lNode)
                {
                    lNode = lParent;
                    lParent = lParent->mParent;
                }

                return lParent;
            }
        }

        inline RecordType const* Successor() const
        {
            if (mRightChild)
            {
                return mRightChild->Minimum();
            }
            else
            {
                RecordType const* lParent = mParent;
                RecordType const* lNode = this;

                while (lParent && lParent->mRightChild == lNode)
                {
                    lNode = lParent;
                    lParent = lParent->mParent;
                }

                return lParent;
            }
        }

        inline RecordType* Successor()
        {
            if (mRightChild)
            {
                return mRightChild->Minimum();
            }
            else
            {
                RecordType* lParent = mParent;
                RecordType* lNode = this;

                while (lParent && lParent->mRightChild == lNode)
                {
                    lNode = lParent;
                    lParent = lParent->mParent;
                }

                return lParent;
            }
        }

        inline int GetBlackDepth() const { return mBlackDepth; }

    private:
        enum { eRed, eBlack };

        inline RecordType(DataType const& pData)
            : mData(pData)
            , mParent(0)
            , mLeftChild(0)
            , mRightChild(0)
            , mColor(eRed)
            , mBlackDepth(0)
        {
        }

        inline RecordType(RecordType const& pRecordType)
            : mData(pRecordType.mData)
            , mParent(0)
            , mLeftChild(0)
            , mRightChild(0)
            , mColor(pRecordType.mColor)
            , mBlackDepth(pRecordType.mBlackDepth)
        {
        }

        DataType mData;

        friend class KRedBlackTree;

        RecordType* mParent;
        RecordType* mLeftChild;
        RecordType* mRightChild;
        unsigned int mColor:2;
        unsigned int mBlackDepth:30;
    };

public:
    typedef RedBack_ConstIteratorType<RecordType>  ConstIteratorType;
    typedef RedBack_IteratorType<RecordType>       IteratorType;

public:
    inline KRedBlackTree() : mRoot(0), mSize(0), mAllocator(sizeof(RecordType)) {}
    inline KRedBlackTree(KRedBlackTree const& pTree) : mRoot(0), mSize(0), mAllocator(sizeof(RecordType)) { operator=(pTree); }
    inline ~KRedBlackTree() { Clear(); }

    /** Deep copy pTree in this. 
      * \param pTree
      */
    inline KRedBlackTree& operator=(KRedBlackTree const& pTree)
    {
        if( this != &pTree )
        {
            Clear();

            mAllocator = pTree.mAllocator;

            if( pTree.mRoot )
            {
                void* lBuffer = mAllocator.AllocateRecords();
                #if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
                    #pragma push_macro("new")
                    #undef new
                #endif
                mRoot = new(lBuffer) RecordType(*(pTree.mRoot));
                #if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
                    #pragma pop_macro("new")
                #endif
                mRoot->mLeftChild = DuplicateSubTree(pTree.mRoot->mLeftChild);
                mRoot->mRightChild = DuplicateSubTree(pTree.mRoot->mRightChild);

                if (mRoot->mLeftChild)
                {
                    mRoot->mLeftChild->mParent = mRoot;
                }

                if (mRoot->mRightChild)
                {
                    mRoot->mRightChild->mParent = mRoot;
                }
            }
            else
            {
                K_ASSERT( pTree.mSize == 0 );
                K_ASSERT( mRoot == 0 );
            }

            mSize = pTree.mSize;
        }

        return *this;
    }

    inline bool operator==(KRedBlackTree const& pTree) const
    {
        // Check a few quick shortcuts
        if( this == &pTree )
            return true;

        if( GetSize() != pTree.GetSize() )
            return false;

        // Iterator through all nodes; if we reach the end of both iterators at the same
        // time then we have two iterators that match.
        ConstIteratorType End;
        ConstIteratorType Iter1(Minimum());
        ConstIteratorType Iter2(pTree.Minimum());

        while( (Iter1 != End) && (Iter2 != End) &&
               (Iter1->GetKey() == Iter2->GetKey()) &&
               (Iter1->GetValue() == Iter2->GetValue()) )
        {
            ++Iter1;
            ++Iter2;
        }

        return Iter1 == End && Iter2 == End;
    }

    /** Ask ALLOCATOR to reserve space to hold pRecordCount elements. 
      * \param pRecordCount
      */
    inline void Reserve(unsigned int pRecordCount)
    {
        mAllocator.Reserve(pRecordCount);
    }

    /** Get the number of elements in the tree. Takes O(1) time.
      * \return The number of elements in the tree.
      */
    inline int GetSize() const { return mSize; }

    inline bool Empty() const { return mSize == 0; }

    /** Insert a new element in the tree. Takes O(log n) time.
      * \param pData The element to insert.
      * \return If pData.GetKey() is already present in the tree, returns the
      *         existing record and false; else returns the new record and true.
      */
    inline KPair<RecordType*, bool> Insert(DataType const& pData)
    {
        KEY_COMPARE_FUNCTOR lCompareKeys;
        bool lResult = false;
        RecordType* lParent = 0;
        RecordType* lNode = mRoot;
        while (lNode != 0)
        {
            KeyType const& lNodeKey = lNode->GetKey();
            KeyType const& lDataKey = pData.GetKey();

            if (lCompareKeys(lNodeKey, lDataKey) < 0)
            {
                lParent = lNode;
                lNode = lNode->mRightChild;
            }
            else if (lCompareKeys(lNodeKey, lDataKey) > 0)
            {
                lParent = lNode;
                lNode = lNode->mLeftChild;
            }
            else
            {
                break;
            }
        }

        if (lNode == 0)
        {
            void* lBuffer = mAllocator.AllocateRecords();
            #if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
                #pragma push_macro("new")
                #undef new
            #endif
            lNode = new(lBuffer) RecordType(pData);
            #if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
                #pragma pop_macro("new")
            #endif
            mSize++;

            #if ( !defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
                K_ASSERT(lNode == lBuffer);
            #endif

            if (lParent)
            {
                if (lCompareKeys(lParent->GetKey(), pData.GetKey()) < 0)
                {
                    K_ASSERT(lParent->mRightChild == 0);
                    lParent->mRightChild = lNode;
                    lNode->mParent = lParent;
                }
                else
                {
                    K_ASSERT(lParent->mLeftChild == 0);
                    lParent->mLeftChild = lNode;
                    lNode->mParent = lParent;
                }
            }
            else
            {
                mRoot = lNode;
            }

            // Fix red black tree property
            FixNodesAfterInsertion(lNode);

            lResult = true;
        }

#ifdef SANITY_CHECK
        IsSane();
#endif

        return KPair<RecordType*, bool>(lNode, lResult);
    }

    /** Remove an element identified by a key from the tree. Takes O(log n) time.
      * \param pKey The key identifying the element to remove.
      */
    inline bool Remove(KeyType const& pKey)
    {
        KEY_COMPARE_FUNCTOR lCompareKeys;
        bool lResult = false;
        RecordType* lNode = mRoot;
        while (lNode != 0)
        {
            if (lCompareKeys(lNode->GetKey(), pKey) < 0)
            {
                lNode = lNode->mRightChild;
            }
            else if (lCompareKeys(lNode->GetKey(), pKey) > 0)
            {
                lNode = lNode->mLeftChild;
            }
            else
            {
                break;
            }
        }

        if (lNode)
        {
            RemoveNode(lNode);
            mSize--;
            lNode->~RecordType();
            mAllocator.FreeMemory(lNode);

            lResult = true;
        }

#ifdef SANITY_CHECK
        IsSane();
#endif

        return lResult;
    }

    /** Remove all elements from the tree. Takes O(n) time. Recursive.
      */
    inline void Clear()
    {
        if (mRoot)
        {
            ClearSubTree(mRoot->mLeftChild);
            ClearSubTree(mRoot->mRightChild);
            mRoot->~RecordType();
            mAllocator.FreeMemory(mRoot);
            mRoot = 0;
            mSize = 0;
        }
    }

    /** Find the smallest element in the tree.
      * Takes O(log n) time.
      */
    inline RecordType const* Minimum() const
    {
        if (0 != mRoot)
        {
            return mRoot->Minimum();
        }
        else
        {
            return 0;
        }
    }

    /** Find the smallest element in the tree.
      * Takes O(log n) time.
      */
    inline RecordType* Minimum()
    {
        if (0 != mRoot)
        {
            return mRoot->Minimum();
        }
        else
        {
            return 0;
        }
    }

    /** Find the largest element in the tree.
      * Takes O(log n) time.
      */
    inline RecordType const* Maximum() const
    {
        if (0 != mRoot)
        {
            return mRoot->Maximum();
        }
        else
        {
            return 0;
        }
    }

    /** Find the largest element in the tree.
      * Takes O(log n) time.
      */
    inline RecordType* Maximum()
    {
        if (0 != mRoot)
        {
            return mRoot->Maximum();
        }
        else
        {
            return 0;
        }
    }

    /** Find the key-value pair with key pKey.
      * Takes O(log n) time.
      * \param pKey The key to look for.
      */
    inline RecordType const* Find(KeyType const& pKey) const
    {
        KEY_COMPARE_FUNCTOR lCompareKeys;
        RecordType const* lNode = mRoot;
        while (lNode != 0)
        {
            if (lCompareKeys(lNode->GetKey(), pKey) < 0)
            {
                lNode = lNode->mRightChild;
            }
            else if (lCompareKeys(lNode->GetKey(), pKey) > 0)
            {
                lNode = lNode->mLeftChild;
            }
            else
            {
                break;
            }
        }

        return lNode;
    }

    /** Find the key-value pair with key pKey.
      * Takes O(log n) time.
      * \param pKey The key to look for.
      */
    inline RecordType* Find(KeyType const& pKey)
    {
        KEY_COMPARE_FUNCTOR lCompareKeys;
        RecordType* lNode = mRoot;
        while (lNode != 0)
        {
            if (lCompareKeys(lNode->GetKey(), pKey) < 0)
            {
                lNode = lNode->mRightChild;
            }
            else if (lCompareKeys(lNode->GetKey(), pKey) > 0)
            {
                lNode = lNode->mLeftChild;
            }
            else
            {
                break;
            }
        }

        return lNode;
    }

    /** Find the key-value pair with the smallest key greater than pKey.
      * Takes O(log n) time.
      * \param pKey The key to look for.
      */
    inline RecordType const* UpperBound(KeyType const& pKey) const
    {
        KEY_COMPARE_FUNCTOR lCompareKeys;
        RecordType const* lNode = mRoot;
        RecordType const* lCandidate = 0;
        while (lNode != 0)
        {
            if (lCompareKeys(lNode->GetKey(), pKey) <= 0)
            {
                lNode = lNode->mRightChild;
            }
            else if (lCompareKeys(lNode->GetKey(), pKey) > 0)
            {
                lCandidate = lNode;
                lNode = lNode->mLeftChild;
            }
        }
        
        return lCandidate;
    }

    /** Find the key-value pair with the smallest key greater than pKey.
      * Takes O(log n) time.
      * \param pKey The key to look for.
      */
    inline RecordType* UpperBound(KeyType const& pKey)
    {
        KEY_COMPARE_FUNCTOR lCompareKeys;
        RecordType* lNode = mRoot;
        RecordType* lCandidate = 0;
        while (lNode != 0)
        {
            if (lCompareKeys(lNode->GetKey(), pKey) <= 0)
            {
                lNode = lNode->mRightChild;
            }
            else if (lCompareKeys(lNode->GetKey(), pKey) > 0)
            {
                lCandidate = lNode;
                lNode = lNode->mLeftChild;
            }
        }
        
        return lCandidate;
    }

protected:
    RecordType* mRoot;
    int mSize;

    AllocatorType mAllocator;

    /** Sanity check on the tree. Check if all red-black tree
      * properties hold. Also check if all key-values pairs are ordered
      * properly.
      */
    inline void IsSane()
    {
        K_ASSERT((mRoot == 0) || (mRoot->mColor == RecordType::eBlack));
        K_ASSERT(((mRoot == 0) && (mSize == 0)) || (mRoot != 0) && (mSize != 0));
        IsSubTreeSane(mRoot);

        ComputeBlackDepth(mRoot, 0);

        RecordType* lNode = mRoot;
        unsigned int lLeafBlackDepth = 0;
        while (lNode)
        {
            if (lNode->mLeftChild == 0)
            {
                lLeafBlackDepth = lNode->mBlackDepth + ((lNode->mColor == RecordType::eBlack) ? 1 : 0);
            }

            lNode = lNode->mLeftChild;
        }

        CheckLeavesBlackDepth(mRoot, lLeafBlackDepth);
    }

    inline void IsSubTreeSane(RecordType const* pNode) const
    {
        KEY_COMPARE_FUNCTOR lCompareKeys;

        if (pNode)
        {
            K_ASSERT(pNode != pNode->mParent);
            K_ASSERT(pNode != pNode->mLeftChild);
            K_ASSERT(pNode != pNode->mRightChild);

            // Check for two consecutive red nodes
            K_ASSERT((pNode->mColor == RecordType::eBlack) ||
                     (pNode->mLeftChild == NULL) ||
                     (pNode->mLeftChild->mColor == RecordType::eBlack));

            K_ASSERT((pNode->mColor == RecordType::eBlack) ||
                     (pNode->mRightChild == NULL) ||
                     (pNode->mRightChild->mColor == RecordType::eBlack));

            // Check key ordering
            K_ASSERT((pNode->mLeftChild == 0 ||
                      lCompareKeys(pNode->GetKey(), pNode->mLeftChild->GetKey()) > 0));

            K_ASSERT((pNode->mRightChild == 0 ||
                      lCompareKeys(pNode->GetKey(), pNode->mRightChild->GetKey()) < 0));

            IsSubTreeSane(pNode->mLeftChild);
            IsSubTreeSane(pNode->mRightChild);
        }
    }

    inline void ComputeBlackDepth(RecordType* pNode, unsigned int pDepth)
    {
        if (pNode)
        {
            pNode->mBlackDepth = pDepth;
            if (pNode->mColor == RecordType::eBlack)
            {
                pDepth++;
            }

            ComputeBlackDepth(pNode->mLeftChild, pDepth);
            ComputeBlackDepth(pNode->mRightChild, pDepth);
        }
    }

    inline void CheckLeavesBlackDepth(RecordType* pNode, unsigned int pBlackDepth)
    {
        if (pNode)
        {
            if ((pNode->mLeftChild == 0) || pNode->mRightChild == 0)
            {
                int lBlackDepth = pNode->mBlackDepth + ((pNode->mColor == RecordType::eBlack) ? 1 : 0);
                K_ASSERT(lBlackDepth == pBlackDepth);
            }

            CheckLeavesBlackDepth(pNode->mLeftChild, pBlackDepth);
            CheckLeavesBlackDepth(pNode->mRightChild, pBlackDepth);
        }
    }

    inline RecordType* DuplicateSubTree(RecordType const* pNode)
    {
        RecordType* lNewSubTree = 0;

        if (pNode)
        {
            void* lBuffer = mAllocator.AllocateRecords();
            #if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
                #pragma push_macro("new")
                #undef new
            #endif
            lNewSubTree = new(lBuffer) RecordType(*pNode);
            #if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
                #pragma pop_macro("new")
            #endif
            lNewSubTree->mLeftChild = DuplicateSubTree(pNode->mLeftChild);
            lNewSubTree->mRightChild = DuplicateSubTree(pNode->mRightChild);

            if (lNewSubTree->mLeftChild)
            {
                lNewSubTree->mLeftChild->mParent = lNewSubTree;
            }

            if (lNewSubTree->mRightChild)
            {
                lNewSubTree->mRightChild->mParent = lNewSubTree;
            }
        }

        return lNewSubTree;
    }

    inline void FixNodesAfterInsertion(RecordType* pNode)
    {
        RecordType* lNode = pNode;
        bool lDone = false;

        while (!lDone)
        {
            lDone = true;

            if (lNode->mParent == 0)
            {
                lNode->mColor = RecordType::eBlack;
            }
            else if (lNode->mParent->mColor == RecordType::eRed)
            {
                RecordType* lUncle = 0;
                if (lNode->mParent == lNode->mParent->mParent->mLeftChild)
                {
                    lUncle = lNode->mParent->mParent->mRightChild;
                }
                else if (lNode->mParent == lNode->mParent->mParent->mRightChild)
                {
                    lUncle = lNode->mParent->mParent->mLeftChild;
                }

                // since lNode->mParent is red, lNode->mParent->mParent exists

                if (lUncle && lUncle->mColor == RecordType::eRed)
                {
                    lNode->mParent->mColor = RecordType::eBlack;
                    lUncle->mColor = RecordType::eBlack;
                    lNode->mParent->mParent->mColor = RecordType::eRed;
                    lNode = lNode->mParent->mParent;

                    lDone = false;
                }
                else
                {
                    if ((lNode == lNode->mParent->mRightChild) &&
                        (lNode->mParent == lNode->mParent->mParent->mLeftChild))
                    {
                        LeftRotate(lNode->mParent);
                        lNode = lNode->mLeftChild;
                    }
                    else if ((lNode == lNode->mParent->mLeftChild) &&
                            (lNode->mParent == lNode->mParent->mParent->mRightChild))
                    {
                        RightRotate(lNode->mParent);
                        lNode = lNode->mRightChild;
                    }

                    lNode->mParent->mColor = RecordType::eBlack;
                    lNode->mParent->mParent->mColor = RecordType::eRed;
                    if ((lNode == lNode->mParent->mLeftChild) &&
                        (lNode->mParent == lNode->mParent->mParent->mLeftChild))
                    {
                        RightRotate(lNode->mParent->mParent);
                    }
                    else
                    {
                        LeftRotate(lNode->mParent->mParent);
                    }
                }
            }
        }

        mRoot->mColor = RecordType::eBlack;
    }

    inline void LeftRotate(RecordType* pNode)
    {
        RecordType* lNode = pNode->mRightChild;

#ifdef _DEBUG
        RecordType* A = pNode->mLeftChild;
        RecordType* B = lNode->mLeftChild;
        RecordType* C = lNode->mRightChild;
        RecordType* Z = pNode->mParent;
#endif

        pNode->mRightChild = lNode->mLeftChild;
        if (pNode->mRightChild)
        {
            pNode->mRightChild->mParent = pNode;
        }

        lNode->mParent = pNode->mParent;
        if (pNode->mParent == 0)
        {
            K_ASSERT(mRoot == pNode);
            mRoot = lNode;
        }
        else if (pNode == pNode->mParent->mLeftChild)
        {
            pNode->mParent->mLeftChild = lNode;
        }
        else
        {
            pNode->mParent->mRightChild = lNode;
        }
        pNode->mParent = lNode;
        lNode->mLeftChild = pNode;

        K_ASSERT(pNode->mLeftChild == A);
        K_ASSERT(pNode->mRightChild == B);
        K_ASSERT(pNode->mParent == lNode);

        K_ASSERT(lNode->mLeftChild == pNode);
        K_ASSERT(lNode->mRightChild == C);
        K_ASSERT(lNode->mParent == Z);

        K_ASSERT(A == 0 || A->mParent == pNode);
        K_ASSERT(B == 0 || B->mParent == pNode);
        K_ASSERT(C == 0 || C->mParent == lNode);
        K_ASSERT(Z == 0 || Z->mLeftChild == lNode || Z->mRightChild == lNode);
    }


    inline void RightRotate(RecordType* pNode)
    {
        RecordType* lNode = pNode->mLeftChild;

#ifdef _DEBUG
        RecordType* A = lNode->mLeftChild;
        RecordType* B = lNode->mRightChild;
        RecordType* C = pNode->mRightChild;
        RecordType* Z = pNode->mParent;
#endif

        pNode->mLeftChild = lNode->mRightChild;
        if (pNode->mLeftChild)
        {
            pNode->mLeftChild->mParent = pNode;
        }

        lNode->mParent = pNode->mParent;
        if (pNode->mParent == 0)
        {
            K_ASSERT(mRoot == pNode);
            mRoot = lNode;
        }
        else if (pNode == pNode->mParent->mRightChild)
        {
            pNode->mParent->mRightChild = lNode;
        }
        else
        {
            pNode->mParent->mLeftChild = lNode;
        }
        pNode->mParent = lNode;
        lNode->mRightChild = pNode;

        K_ASSERT(lNode->mLeftChild == A);
        K_ASSERT(lNode->mRightChild == pNode);
        K_ASSERT(lNode->mParent == Z);

        K_ASSERT(pNode->mLeftChild == B);
        K_ASSERT(pNode->mRightChild == C);
        K_ASSERT(pNode->mParent == lNode);

        K_ASSERT(A == 0 || A->mParent == lNode);
        K_ASSERT(B == 0 || B->mParent == pNode);
        K_ASSERT(C == 0 || C->mParent == pNode);
        K_ASSERT(Z == 0 || Z->mLeftChild == lNode || Z->mRightChild == lNode);
    }


    inline void RemoveNode(RecordType* pNode)
    {
        if (pNode->mLeftChild == 0)
        {
            if (pNode->mRightChild == 0)
            {
                if (pNode->mParent)
                {
                    if (pNode->mParent->mLeftChild == pNode)
                    {
                        pNode->mParent->mLeftChild = 0;
                    }
                    else if (pNode->mParent->mRightChild == pNode)
                    {
                        pNode->mParent->mRightChild = 0;
                    }
                    else
                    {
                        K_ASSERT_MSG_NOW("Node not found in KRedBlackTree");
                    }
                }
                else
                {
                    K_ASSERT(mRoot == pNode);
                    mRoot = 0;
                }

                if (pNode->mColor == RecordType::eBlack)
                {
                    FixNodesAfterRemoval(pNode->mParent, 0);
                }
            }
            else
            {
                if (pNode->mParent)
                {
                    if (pNode->mParent->mLeftChild == pNode)
                    {
                        pNode->mParent->mLeftChild = pNode->mRightChild;
                        pNode->mRightChild->mParent = pNode->mParent;
                    }
                    else if (pNode->mParent->mRightChild == pNode)
                    {
                        pNode->mParent->mRightChild = pNode->mRightChild;
                        pNode->mRightChild->mParent = pNode->mParent;
                    }
                    else
                    {
                        K_ASSERT_MSG_NOW("Node not found in KRedBlackTree");
                    }
                }
                else
                {
                    K_ASSERT(mRoot == pNode);
                    mRoot = pNode->mRightChild;
                    pNode->mRightChild->mParent = 0;
                }

                if (pNode->mColor == RecordType::eBlack)
                {
                    FixNodesAfterRemoval(pNode->mRightChild->mParent, pNode->mRightChild);
                }
            }
        }
        else
        {
            if (pNode->mRightChild == 0)
            {
                if (pNode->mParent)
                {
                    if (pNode->mParent->mLeftChild == pNode)
                    {
                        pNode->mParent->mLeftChild = pNode->mLeftChild;
                        pNode->mLeftChild->mParent = pNode->mParent;
                    }
                    else if (pNode->mParent->mRightChild == pNode)
                    {
                        pNode->mParent->mRightChild = pNode->mLeftChild;
                        pNode->mLeftChild->mParent = pNode->mParent;
                    }
                    else
                    {
                        K_ASSERT_MSG_NOW("Node not found in KRedBlackTree");
                    }
                }
                else
                {
                    K_ASSERT(mRoot == pNode);
                    mRoot = pNode->mLeftChild;
                    pNode->mLeftChild->mParent = 0;
                }

                if (pNode->mColor == RecordType::eBlack)
                {
                    FixNodesAfterRemoval(pNode->mLeftChild->mParent, pNode->mLeftChild);
                }
            }
            else
            {
                RecordType* lMinRightNode = pNode->mRightChild->Minimum();
                RemoveNode(lMinRightNode);

                lMinRightNode->mColor = pNode->mColor;
                ReplaceNode(pNode, lMinRightNode);
            }
        }

        pNode->mParent = 0;
        pNode->mLeftChild = 0;
        pNode->mRightChild = 0;
    }


    inline void ReplaceNode(RecordType* pNodeToReplace, RecordType* pReplacement)
    {
        pReplacement->mParent = pNodeToReplace->mParent;
        if (pNodeToReplace->mParent)
        {
            if (pNodeToReplace->mParent->mLeftChild == pNodeToReplace)
            {
                pNodeToReplace->mParent->mLeftChild = pReplacement;
            }
            else if (pNodeToReplace->mParent->mRightChild == pNodeToReplace)
            {
                pNodeToReplace->mParent->mRightChild = pReplacement;
            }
        }
        else
        {
            K_ASSERT(mRoot == pNodeToReplace);
            mRoot = pReplacement;
        }

        pReplacement->mLeftChild = pNodeToReplace->mLeftChild;
        if (pReplacement->mLeftChild)
        {
            pReplacement->mLeftChild->mParent = pReplacement;
        }

        pReplacement->mRightChild = pNodeToReplace->mRightChild;
        if (pReplacement->mRightChild)
        {
            pReplacement->mRightChild->mParent = pReplacement;
        }
    }

    inline RecordType* Sibling(RecordType const* pParent, RecordType const* pNode) const
    {
        if (pParent)
        {
            if (pParent->mLeftChild == pNode)
            {
                return pParent->mRightChild;
            }
            else if (pParent->mRightChild == pNode)
            {
                return pParent->mLeftChild;
            }
        }

        return 0;
    }

    inline bool IsBlack(RecordType const* pNode)
    {
        return ((pNode == 0) || (pNode->mColor == RecordType::eBlack));
    }

    inline void FixNodesAfterRemoval(RecordType* pParent, RecordType* pNode)
    {
        RecordType* lParent = pParent;
        RecordType* lNode = pNode;
        bool lDone = false;

        while (!lDone)
        {
            lDone = true;

            if (!IsBlack(lNode))
            {
                lNode->mColor = RecordType::eBlack;
            }
            else if (lParent != NULL)
            {
                RecordType* lSibling = Sibling(lParent, lNode);

                if (!IsBlack(lSibling))
                {
                    lParent->mColor = RecordType::eRed;
                    lSibling->mColor = RecordType::eBlack;
                    if (lNode == lParent->mLeftChild)
                    {
                        LeftRotate(lParent);
                    }
                    else
                    {
                        RightRotate(lParent);
                    }

                    // update sibling: it may have change after rotation
                    // parent was not affected by this rotation
                    lSibling = Sibling(lParent, lNode);
                }

                /* check this for null sibling */
                if (lSibling &&
                    IsBlack(lParent) &&
                    IsBlack(lSibling) &&
                    IsBlack(lSibling->mLeftChild) &&
                    IsBlack(lSibling->mRightChild))
                {
                    lSibling->mColor = RecordType::eRed;
                    lNode = lParent;
                    lParent = lParent->mParent;
                    lDone = false;
                }
                else
                {
                    if (!IsBlack(lParent) &&
                        IsBlack(lSibling) &&
                        ((lSibling == 0) || IsBlack(lSibling->mLeftChild)) &&
                        ((lSibling == 0) || IsBlack(lSibling->mRightChild)))
                    {
                        if (lSibling)
                        {
                            lSibling->mColor = RecordType::eRed;
                        }
                        lParent->mColor = RecordType::eBlack;
                    }
                    else // lSibling != 0
                    {
                        if ((lNode == lParent->mLeftChild) &&
                            IsBlack(lSibling) &&
                            !IsBlack(lSibling->mLeftChild) &&
                            IsBlack(lSibling->mRightChild))
                        {
                            lSibling->mColor = RecordType::eRed;
                            lSibling->mLeftChild->mColor = RecordType::eBlack;
                            RightRotate(lSibling);
                        }
                        else if ((lNode == lParent->mRightChild) &&
                                 IsBlack(lSibling) &&
                                 IsBlack(lSibling->mLeftChild) &&
                                 !IsBlack(lSibling->mRightChild))
                        {
                            lSibling->mColor = RecordType::eRed;
                            lSibling->mRightChild->mColor = RecordType::eBlack;
                            LeftRotate(lSibling);
                        }

                        // update sibling: it may have change after rotation
                        lSibling = Sibling(lParent, lNode);
                        K_ASSERT(lSibling != 0); // lSibling is now
                                                 // the former red
                                                 // child of the
                                                 // former sibling

                        lSibling->mColor = lParent->mColor;
                        lParent->mColor = RecordType::eBlack;
                        if (lNode == lParent->mLeftChild)
                        {
                            if (lSibling->mRightChild)
                            {
                                lSibling->mRightChild->mColor = RecordType::eBlack;
                            }
                            LeftRotate(lParent);
                        }
                        else
                        {
                            if (lSibling->mLeftChild)
                            {
                                lSibling->mLeftChild->mColor = RecordType::eBlack;
                            }
                            RightRotate(lParent);
                        }
                    }
                }
            }
        }

        if (mRoot)
        {
            mRoot->mColor = RecordType::eBlack;
        }
    }


    inline void ClearSubTree(RecordType* pNode)
    {
        if (pNode)
        {
            ClearSubTree(pNode->mLeftChild);
            ClearSubTree(pNode->mRightChild);
            pNode->~RecordType();
            mAllocator.FreeMemory(pNode);
        }
    }

    inline int GetSubTreeSize(RecordType* pNode) const
    {
        if (pNode)
        {
            return GetSubTreeSize(pNode->mLeftChild) + GetSubTreeSize(pNode->mRightChild) + 1;
        }
        else
        {
            return 0;
        }
    }

};

/*************************************************************************
 *
 * Default comparator for maps.  Assumes operator < is defined.
 *
 *************************************************************************/

template <typename T>
struct KFbxLessCompare
{
    inline int operator()(const T& left, const T& right) const
    {
        return (left < right) ? -1 : ((right < left) ? 1 : 0);
    }
};

/*************************************************************************
 * class KMap
 *
 * Implements an efficient dictionnary.
 *
 *************************************************************************/


template <typename KEY_TYPE,
          typename VALUE_TYPE,
          typename KEY_COMPARE_FUNCTOR = KFbxLessCompare<KEY_TYPE>,
          typename ALLOCATOR = KBaseAllocator>
class KMap
{
protected:
    class KKeyValuePair : private KPair<const KEY_TYPE, VALUE_TYPE>
    {
    public:
        KKeyValuePair(KEY_TYPE const& pFirst, VALUE_TYPE const& pSecond)
            : KPair<const KEY_TYPE, VALUE_TYPE>(pFirst, pSecond) {}

        // Key is always const.
        typedef const KEY_TYPE      KeyType;
        typedef const KEY_TYPE      ConstKeyType;
        typedef VALUE_TYPE          ValueType;
        typedef const VALUE_TYPE    ConstValueType;

        ConstKeyType & GetKey() const       { return this->mFirst; }
        KeyType & GetKey()                  { return this->mFirst; }

        ConstValueType& GetValue() const    { return this->mSecond; }
        ValueType& GetValue()               { return this->mSecond; }
    };

    typedef KRedBlackTree<KKeyValuePair, KEY_COMPARE_FUNCTOR, ALLOCATOR> StorageType;
    StorageType mTree;


public:
    typedef VALUE_TYPE ValueType;
    typedef KEY_TYPE KeyType;
    typedef typename StorageType::RecordType         RecordType;
    typedef typename StorageType::IteratorType       Iterator;
    typedef typename StorageType::ConstIteratorType  ConstIterator;

    inline KMap() {}
    inline KMap(KMap const& pMap) : mTree(pMap.mTree) {}
    inline ~KMap() {Clear();}

    inline void Reserve(unsigned int pRecordCount)
    {
        mTree.Reserve(pRecordCount);
    }

    inline int GetSize() const
    {
        return mTree.GetSize();
    }

    inline KPair<RecordType*, bool> Insert(KeyType const& pKey, ValueType const& pValue)
    {
        return mTree.Insert(KKeyValuePair(pKey, pValue));
    }

    inline int Remove(KeyType const& pKey)
    {
        return mTree.Remove(pKey);
    }

    inline void Clear()
    {
        mTree.Clear();
    }

    inline bool Empty() const
    {
        return mTree.Empty();
    }

    Iterator Begin()
    {
        return Iterator(Minimum());
    }

    Iterator End()
    {
        return Iterator();
    }

    ConstIterator Begin() const
    {
        return ConstIterator(Minimum());
    }

    ConstIterator End() const
    {
        return ConstIterator();
    }

    inline RecordType const* Find(KeyType const& pKey) const
    {
        return mTree.Find(pKey);
    }

    inline RecordType* Find(KeyType const& pKey)
    {
        return mTree.Find(pKey);
    }

    inline RecordType const* UpperBound(KeyType const& pKey) const
    {
        return mTree.UpperBound(pKey);
    }

    inline RecordType* UpperBound(KeyType const& pKey)
    {
        return mTree.UpperBound(pKey);
    }

    inline ValueType& operator[](KeyType const& pKey)
    {
        RecordType* lRecord = Find(pKey);

        if( !lRecord )
        {
            lRecord = Insert(pKey, ValueType()).mFirst;
        }

        return lRecord->GetValue();
    }

    inline RecordType const* Minimum() const
    {
        return mTree.Minimum();
    }

    inline RecordType* Minimum()
    {
        return mTree.Minimum();
    }

    inline RecordType const* Maximum() const
    {
        return mTree.Maximum();
    }

    inline RecordType* Maximum()
    {
        return mTree.Maximum();
    }
};

/*************************************************************************
 * class KSet2
 *
 * Implements a data storage with efficient Find.
 *
 *************************************************************************/


template <typename VALUE_TYPE,
          typename KEY_COMPARE_FUNCTOR = KFbxLessCompare<VALUE_TYPE>,
          typename ALLOCATOR = KBaseAllocator>
class KSet2
{
protected:
    class KValue
    {
    public:
        inline KValue(VALUE_TYPE const& pValue)
            : mValue(pValue) {}

        // Cannot modify the value in a set, which would change its ordering in the tree
        typedef const VALUE_TYPE KeyType;
        typedef const VALUE_TYPE ConstKeyType;
        typedef const VALUE_TYPE ValueType;
        typedef const VALUE_TYPE ConstValueType;

        inline KeyType& GetKey() const      { return mValue; }
        inline ConstKeyType& GetKey()       { return mValue; }

        inline ValueType& GetValue() const  { return mValue; }
        inline ConstValueType& GetValue()   { return mValue; }

    protected:
        ValueType mValue;
    };

    typedef KRedBlackTree<KValue, KEY_COMPARE_FUNCTOR, ALLOCATOR> StorageType;
    StorageType mTree;


public:
    typedef VALUE_TYPE ValueType;
    typedef typename StorageType::RecordType        RecordType;
    typedef typename StorageType::IteratorType      Iterator;
    typedef typename StorageType::ConstIteratorType ConstIterator;

    inline KSet2() {}
    inline KSet2(KSet2 const& pSet) : mTree(pSet.mTree) {}
    inline ~KSet2() {Clear();}

    inline void Reserve(unsigned int pRecordCount)
    {
        mTree.Reserve(pRecordCount);
    }

    inline int GetSize() const
    {
        return mTree.GetSize();
    }

    inline KPair<RecordType*, bool> Insert(ValueType const& pValue)
    {
        return mTree.Insert(KValue(pValue));
    }

    inline int Remove(ValueType const& pValue)
    {
        return mTree.Remove(pValue);
    }

    inline void Clear()
    {
        mTree.Clear();
    }

    inline bool Empty() const
    {
        return mTree.Empty();
    }

    Iterator Begin()
    {
        return Iterator(Minimum());
    }

    Iterator End()
    {
        return Iterator();
    }

    ConstIterator Begin() const
    {
        return ConstIterator(Minimum());
    }

    ConstIterator End() const
    {
        return ConstIterator();
    }

    inline RecordType const* Find(ValueType const& pValue) const
    {
        return mTree.Find(pValue);
    }

    inline RecordType* Find(ValueType const& pValue)
    {
        return mTree.Find(pValue);
    }

    inline RecordType const* Minimum() const
    {
        return mTree.Minimum();
    }

    inline RecordType* Minimum()
    {
        return mTree.Minimum();
    }

    inline RecordType const* Maximum() const
    {
        return mTree.Maximum();
    }

    inline RecordType* Maximum()
    {
        return mTree.Maximum();
    }

    inline bool operator==(const KSet2<VALUE_TYPE, KEY_COMPARE_FUNCTOR, ALLOCATOR>& pOther) const
    {
        return (this == &pOther) || (mTree == pOther.mTree);
    }

    inline bool operator != (const KSet2<VALUE_TYPE, KEY_COMPARE_FUNCTOR, ALLOCATOR>& pOther) const
    {
        return !(*this == pOther);
    }

    /** Intersect with another set.
    * \param The other set.
    * \return The intersection set of the two sets.
    */
    inline KSet2 Intersect(const KSet2& pOther) const
    {
        KSet2 lReturn;
        ConstIterator lBegin = Begin();
        for (; lBegin != End(); ++lBegin)
        {
            if (pOther.Find(lBegin->GetValue()) != NULL)
                lReturn.Insert(lBegin->GetValue());
        }
        return lReturn;
    }

    /** Unite with another set.
    * \param The other set.
    * \return The union set of the two sets (no duplicated items).
    */
    inline KSet2 Union(const KSet2& pOther) const
	{
		KSet2 lReturn(*this);
		ConstIterator lBegin = pOther.Begin();
		for (; lBegin != End(); ++lBegin)
		{
			if (Find(lBegin->GetValue()) == NULL)
				lReturn.Insert(lBegin->GetValue());
		}
		return lReturn;
	}    
};
#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KMAP_H

