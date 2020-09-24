#ifndef FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KINTRUSIVELIST_H
#define FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KINTRUSIVELIST_H

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

////
//// NOTE:  This container should not be used by external user. It is intended to
////        be used internally in FBX SDK
////

#include <fbxfilesdk/fbxfilesdk_def.h>

// FBX namespace begin
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

#define KFBX_LISTNODE(classT, NodeCount)\
    public: inline KListNode<classT>& GetListNode(int index = 0){ return this->mNode[index]; }\
    private:  KListNode<classT> mNode[NodeCount];

//-----------------------------------------------------------------
template <typename T>
class KListNode
{
    typedef KListNode<T> NodeT; 

public:
    explicit KListNode(T* pData = 0):mNext(0),mPrev(0),mData(pData){}
    ~KListNode()
    {
        Disconnect();
    }

    void Disconnect()
    {
        if ( mPrev != 0 )
            mPrev->mNext = mNext;

        if ( mNext != 0 )
            mNext->mPrev = mPrev;

        mPrev = mNext = 0;
    }

    NodeT* mNext;
    NodeT* mPrev;

    T* mData;
};

//-----------------------------------------------------------------
// template arg T: Type listed
//          arg NodeIndex: If an object listed has  multiple list node, which
//                         index corresponds to the right node
template <typename T, int NodeIndex=0>
class KIntrusiveList
{
public:
    typedef T         allocator_type;
    typedef T         value_type;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T*        pointer;
    typedef const T*  const_pointer;

    typedef KListNode<T> NodeT;

    // Construction / Destruction
    KIntrusiveList():mHead(0)
    {
        mHead.mNext = mHead.mPrev = &mHead;
    }
    ~KIntrusiveList()
    {
        while(!Empty())
            Begin().Get()->Disconnect();  // LINUXNote:  should be Erase(Begin()); but there's an issue with gcc 4.2
    };

    // true if the list's size is 0.
    bool Empty() const
    {
        return ((mHead.mNext==&mHead)&&(mHead.mPrev==&mHead));
    }

    // Back Insertion Sequence  Inserts a new element at the end.  
    void PushBack(T& pElement)
    {
        NodeT* pNode = &pElement.GetListNode(NodeIndex);
        pNode->mData = &pElement;

        if (Empty())
        {
            pNode->mNext = &mHead;
            pNode->mPrev = &mHead;
            mHead.mNext = pNode;
            mHead.mPrev = pNode;
        }
        else
        {
            pNode->mNext = &mHead;
            pNode->mPrev = mHead.mPrev;

            pNode->mPrev->mNext = pNode;
            mHead.mPrev = pNode;
        }
    }

    void PushFront(T& pElement)
    {
        NodeT* pNode = &pElement.GetListNode(NodeIndex);
        pNode->mData = &pElement;

        if (Empty())
        {
            pNode->mNext = &mHead;
            pNode->mPrev = &mHead;
            mHead.mNext = pNode;
            mHead.mPrev = pNode;
        }
        else
        {
            pNode->mNext = mHead.mNext;
            pNode->mPrev = &mHead;

            pNode->mNext->mPrev = pNode;
            mHead.mNext = pNode;
        }
    }

    void PopFront()
    {
        iterator begin = Begin();
        Erase(begin);
    }

    void PopBack()
    {
        Erase(--(End()));
    }

public:
    class IntrusiveListIterator
    {
    public:
        explicit IntrusiveListIterator(NodeT* ptr=0):mPtr(ptr){}

        // pre-increment
        IntrusiveListIterator& operator++()
        {
            mPtr = mPtr->mNext;return (*this);
        }
        // post-increment
        const IntrusiveListIterator operator++(int)
        {
            IntrusiveListIterator temp = *this;
            ++*this;
            return (temp);
        }
        // pre-decrement
        IntrusiveListIterator& operator--()
        {
            mPtr = mPtr->mPrev;return *this;
        }
        // post-decrement
        const IntrusiveListIterator operator--(int)
        {
            IntrusiveListIterator temp = *this;
            --*this;
            return (temp);
        }
        IntrusiveListIterator& operator=(const IntrusiveListIterator &other){mPtr = other.mPtr; return *this;}

        reference operator*() const { return *(mPtr->mData); }
        pointer operator->() const { return (&**this); }
        bool operator==(const IntrusiveListIterator& other)const{ return mPtr==other.mPtr; } 
        bool operator!=(const IntrusiveListIterator& other)const{ return !(*this == other); } 

        inline NodeT* Get()const { return mPtr; }

    private:
        NodeT* mPtr;
    };

    class  IntrusiveListConstIterator
    {
    public:
        explicit IntrusiveListConstIterator(const NodeT* ptr=0):mPtr(ptr){}

       // pre-increment
        IntrusiveListConstIterator& operator++()
        {
            mPtr = mPtr->mNext;return (*this);
        }
        // post-increment
        const IntrusiveListConstIterator operator++(int)
        {
            IntrusiveListConstIterator temp = *this;
            ++*this;
            return (temp);
        }
        // pre-decrement
        IntrusiveListConstIterator& operator--()
        {
            mPtr = mPtr->mPrev;return *this;
        }
        // post-decrement
        const IntrusiveListConstIterator operator--(int)
        {
            IntrusiveListConstIterator temp = *this;
            --*this;
            return (temp);
        }
        IntrusiveListConstIterator& operator=(const IntrusiveListConstIterator &other){mPtr = other.mPtr; return *this;}

        const_reference operator*() const { return *(mPtr->mData); }
        const_pointer operator->() const { return (&**this); }
        bool operator==(const IntrusiveListConstIterator& other)const{ return mPtr==other.mPtr; } 
        bool operator!=(const IntrusiveListConstIterator& other)const{ return !(*this == other); } 

        inline const NodeT* Get()const { return mPtr; }

    private:
        mutable const NodeT* mPtr;
    };

    // --- Iterator definitions ---
    typedef IntrusiveListIterator iterator;
    typedef IntrusiveListConstIterator const_iterator;

    // iterator support
    inline iterator Begin() { return iterator(mHead.mNext); }
    inline const_iterator Begin() const { return const_iterator(mHead.mNext); }
    inline iterator End() { return iterator(&mHead); }
    inline const_iterator End() const { return const_iterator(&mHead); }

    // Because there is no real use, for the reverse iterators, 
    // they have not been implemented. 

    reference Front(){return (*Begin());}
    const_reference Front() const { return (*Begin()); }
    reference Back(){ return (*(--End())); }
    const_reference Back() const{ return (*(--End())); }

    iterator& Erase(iterator& it)
    {
        it.Get()->Disconnect();
        return (++it);
    }
private:
    NodeT mHead;

    // Not copyable
    KIntrusiveList(const KIntrusiveList&);
    KIntrusiveList& operator=(const KIntrusiveList& Right){return (*this);}
};

// FBX namespace end
#include <fbxfilesdk/fbxfilesdk_nsend.h>




#endif // FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KINTRUSIVELIST_H

