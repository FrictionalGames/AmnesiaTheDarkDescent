/*!  \file kcontainerallocators.h
 */

#ifndef FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KCONTAINERALLOCATORS_H
#define FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KCONTAINERALLOCATORS_H

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

#include <fbxfilesdk/components/kbaselib/klib/kdebug.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/**An allocator class for use as a template parameter to one of the
  *container class (KMap, KSet2, KDynamicArray...) must implement these.
  */

class KFBX_DLL KBaseAllocator
{
public:
    /** The class constructor.  
      * \param pRecordSize the size of one record held by the container. 
      * \remarks The parameter pRecordSize is not necessarily the same 
      *  size as of the value type, since the
      *  container may wrap the value into a private class.
      */
    KBaseAllocator(size_t const pRecordSize)
        : mRecordSize(pRecordSize)
    {
    }

    /** This tells the allocator that we are about to call AllocateRecords
      * one or many times to allocate pRecordCount records. 
      * \param pRecordCount
      * \remarks This gives the allocator a chance to do whatever it deems necessary
      * to optimize subsequent allocations, for example, by preallocating a
      * sufficiently large pool of memory.
      */
    void Reserve(size_t const pRecordCount)
    {
        // By default, ignore all preallocating requests.
    }

    /** Returns a pointer to a uninitialized continuous block of memory
      * able to hold pRecordCount * pRecordSize  bytes.  
      * \param pRecordCount
      * \remarks pRecordSize was defined in the Constructor description, above.
      */
    void* AllocateRecords(size_t const pRecordCount = 1)
    {
        return FbxSdkMalloc(pRecordCount * mRecordSize);
    }

    /** Frees a block of memory returned by AllocateRecords. 
      * \param pRecord
      */
    void FreeMemory(void* pRecord)
    {
        FbxSdkFree(pRecord);
    }

    /** \return the size of each record allocated. 
      */
    size_t GetRecordSize() const
    {
        return mRecordSize;
    }

    // Use default copy / assignment
    /*    operator=()
        *        The allocator must have appropriate copy semantics; does not need to
        *        copy its allocated blocks, but it should adjust its record size.
        */

private:
    size_t mRecordSize;
};

/**
  *This allocator only frees the allocated memory when it is deleted.
  *This is a good allocator for building dictionaries, where we only
  *add things to a container, but never remove them.
  */
class KHungryAllocator
{
public:
    KHungryAllocator(size_t pRecordSize)
        : mRecordSize(pRecordSize)
        , mData(NULL)
        , mRecordPoolSize(0)
    {
    }

    KHungryAllocator(const KHungryAllocator& pOther)
        : mRecordSize(pOther.mRecordSize)
        , mData(0)
        , mRecordPoolSize(pOther.mRecordPoolSize)
    {
    }

	~KHungryAllocator()
	{
		MemoryBlock* lCurrent = mData;
		MemoryBlock* lNext = lCurrent ? lCurrent->mNextBlock : 0;
		while (lCurrent)
		{
			FbxSdkDelete(lCurrent);
			lCurrent = lNext;
			lNext = lCurrent ? lCurrent->mNextBlock : 0;
		}
	}

    void Reserve(size_t const pRecordCount)
    {
        MemoryBlock* lMem = FbxSdkNew< MemoryBlock >(pRecordCount * mRecordSize);
        lMem->mNextBlock = mData;
        mData = lMem;
        mRecordPoolSize += pRecordCount;
    }

    void* AllocateRecords(size_t const pRecordCount = 1)
    {
        MemoryBlock* lBlock = mData;
        void* lRecord = NULL;

        while ((lBlock != NULL) &&
            ((lRecord = lBlock->GetChunk(pRecordCount * mRecordSize)) == NULL))
        {
            lBlock = lBlock->mNextBlock;
        }

        if (lRecord == NULL)
        {
            size_t lNumRecordToAllocate = mRecordPoolSize / 8 == 0 ? 2 : mRecordPoolSize / 8;
            if (lNumRecordToAllocate < pRecordCount)
            {
                lNumRecordToAllocate = pRecordCount;
            }
            Reserve(lNumRecordToAllocate);
            lRecord = AllocateRecords(pRecordCount);
        }

        return lRecord;
    }

    void FreeMemory(void* pRecord)
    {
        // "Hungry": release memory only when the allocator is destroyed.
    }

    size_t GetRecordSize() const
    {
        return mRecordSize;
    }

    KHungryAllocator& operator=(const KHungryAllocator& pOther)
    {
        if( this != &pOther )
        {
            // The next call to AllocateRecords() may skip over currently reserved
            // records if the size changes drastically, but otherwise GetChunk()
            // is size-oblivious.
            if( mRecordSize < pOther.mRecordSize )
            {
                mRecordPoolSize = 0;
            }

            mRecordSize = pOther.mRecordSize;
        }

        return(*this);
    }

private:
    class MemoryBlock
    {
    public:
        MemoryBlock(size_t pSize)
            : mNextBlock(NULL)
            , mData(NULL)
            , mFreeData(NULL)
            , mEnd(NULL)
        {
            mData = FbxSdkMalloc(pSize);
            mFreeData = mData;
            mEnd = reinterpret_cast<char*>(mData) + pSize;
        };

        ~MemoryBlock()
        {
            FbxSdkFree(mData);
        }

        void* GetChunk(size_t const pSize)
        {
            if (reinterpret_cast<char*>(mFreeData) + pSize < mEnd)
            {
                void* lChunk = mFreeData;
                mFreeData = reinterpret_cast<char*>(mFreeData) + pSize;
                return lChunk;
            }

            return NULL;
        }

        MemoryBlock* mNextBlock;
        void* mData;
        void* mFreeData;
        void* mEnd;
    };

	size_t mRecordSize;
    MemoryBlock* mData;
    size_t mRecordPoolSize;
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KCONTAINERALLOCATORS_H

