/*!  \file kdynamicarray.h
 */

#ifndef FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KDYNAMICARRAY_H
#define FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KDYNAMICARRAY_H

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

#include <fbxfilesdk/components/kbaselib/klib/kcontainerallocators.h>
#include <fbxfilesdk/components/kbaselib/klib/kdebug.h>

#include <new>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** Template class for dynamic array holding objects.
  * \nosubgrouping
  * \see KStaticArray
  */
template <typename VALUE_TYPE, typename ALLOCATOR = KBaseAllocator>
class KDynamicArray
{
public:
    //! Type of the elements in the array.
    typedef VALUE_TYPE ValueType;
    //! Type of the class used for allocating memory.
    typedef ALLOCATOR AllocatorType;

    //! Default constructor.
    KDynamicArray()
        : mArray(NULL)
        , mArrayCapacity(0)
        , mValueCount(0)
        , mAllocator(sizeof(ValueType))
    {
    }

    /** Constructor.
    * \param pInitialSize initial capacity of this array
     */
    KDynamicArray(size_t const pInitialSize)
        : mArray(NULL)
        , mArrayCapacity(0)
        , mValueCount(0)
        , mAllocator(sizeof(ValueType))
    {
        Reserve(pInitialSize);
    }

    /** Copy constructor.
    * \remarks The copy constructor of \c VALUE_TYPE will be 
    * invoked in order to copy the value of elements to the
    * new array.
    */
    KDynamicArray(KDynamicArray const& pArray)
        : mArray(NULL)
        , mArrayCapacity(0)
        , mValueCount(0)
        , mAllocator(sizeof(ValueType))
    {
        Reserve(pArray.mArrayCapacity);
        CopyArray(mArray, pArray.mArray, pArray.mValueCount);
        mValueCount = pArray.mValueCount;
    }

    //! Destructor.
    ~KDynamicArray()
    {
        size_t i;
        for (i = 0; i < mValueCount; i++)
        {
            mArray[i].~VALUE_TYPE();
        }

        mAllocator.FreeMemory(mArray);
    }

    //! Gets the current capacity of the array.
    size_t GetCapacity() const
    {
        return mArrayCapacity;
    }

    //! Gets the size of the array.
    size_t GetSize() const
    {
        return mValueCount;
    }

    /** Assures that sufficient memory is allocated to hold n objects
    * in the array, and increases the capacity if necessary.
    * \param pCount Number of objects to reserve
    */
    void Reserve(size_t const pCount)
    {
        if (pCount > mArrayCapacity)
        {
            // We don't use mAllocator.PreAllocate, because we want our array
            // to be continuous in memory.
            void* lBuffer = mAllocator.AllocateRecords(pCount);
            ValueType* lNewArray = reinterpret_cast<ValueType*>(lBuffer);

            MoveArray(lNewArray, mArray, mValueCount);

            mAllocator.FreeMemory(mArray);
            mArray = lNewArray;
            mArrayCapacity = pCount;
        }
    }

    /** Appends n objects at the end of the array.
    * \param pValue object to append
    * \param pNCopies number of copies to append
    */
    void PushBack(ValueType const& pValue, size_t const pNCopies = 1)
    {
        if (mValueCount + pNCopies > mArrayCapacity)
        {
            // grow by 50%
            size_t lNewSize = mArrayCapacity + mArrayCapacity / 2;

            if (mValueCount + pNCopies > lNewSize)
            {
                lNewSize = mValueCount + pNCopies;
            }

            Reserve(lNewSize);
        }

        K_ASSERT(mValueCount + pNCopies <= mArrayCapacity);

        Fill(mArray + mValueCount, pValue, pNCopies);

        mValueCount += pNCopies;
    }

    /** Inserts n objects at the specified position.
    * \param pIndex position index
    * \param pValue object to insert
    * \param pNCopies number of copies to append
    */
    void Insert(size_t const pIndex, ValueType const& pValue, size_t const pNCopies = 1)
    {
        K_ASSERT(pIndex >= 0);
        K_ASSERT(pIndex <= mValueCount);

        ValueType lValue = pValue; // in case pValue is in array

        if (pNCopies == 0)
        {
        }
        else if (pIndex >= mValueCount)
        {
            PushBack(pValue, pNCopies);
        }
        else if (mValueCount + pNCopies > mArrayCapacity)
        {
            // not enough room
            // grow by 50%
            size_t lNewSize = mArrayCapacity + mArrayCapacity / 2;

            if (mValueCount + pNCopies > lNewSize)
            {
                lNewSize = mValueCount + pNCopies;
            }

            void* lBuffer = mAllocator.AllocateRecords(lNewSize);
            ValueType* lNewArray = reinterpret_cast<ValueType*>(lBuffer);

            MoveArray(lNewArray, mArray, pIndex); // copy prefix
            Fill(lNewArray + pIndex, pValue, pNCopies); // copy values
            MoveArray(lNewArray + pIndex + pNCopies, mArray + pIndex, mValueCount - pIndex); // copy suffix

            mAllocator.FreeMemory(mArray);
            mArray = lNewArray;
            mValueCount += pNCopies;
            mArrayCapacity = lNewSize;
        }
        else
        {
            // copy suffix backwards
            MoveArrayBackwards(mArray + pIndex + pNCopies, mArray + pIndex, mValueCount - pIndex);
            Fill(mArray + pIndex, pValue, pNCopies); // copy values
            mValueCount += pNCopies;
        }
    }

    /** Removes n objects at the end.
    * \param pNElements number of objects to remove
    */
    void PopBack(size_t pNElements = 1)
    {
        K_ASSERT(pNElements <= mValueCount);

        size_t i;
        for (i = mValueCount - pNElements; i < mValueCount; i++)
        {
            mArray[i].~VALUE_TYPE();
        }

        mValueCount -= pNElements;
    }

    /** Removes n objects at the specified position.
    * \param pIndex position index
    * \param pNElements number of objects to remove
    */
    void Remove(size_t const pIndex, size_t pNElements = 1)
    {
        K_ASSERT(pIndex >= 0);
        K_ASSERT(pIndex <= mValueCount);
        K_ASSERT(pIndex + pNElements <= mValueCount);

        if (pIndex + pNElements >= mValueCount)
        {
            PopBack(pNElements);
        }
        else
        {
            size_t i;
            for (i = pIndex; i < pIndex + pNElements; i++)
            {
                mArray[i].~VALUE_TYPE();
            }

            MoveOverlappingArray(mArray + pIndex, mArray + pIndex + pNElements, mValueCount - pNElements);

            mValueCount -= pNElements;
        }
    }

    /** Gets nth object in the array.
    * \param pIndex position index
    */
    ValueType& operator[](size_t const pIndex)
    {
        return *(mArray + pIndex);
    }

    /** Gets nth object in the array.
    * \param pIndex position index
    */
    ValueType const& operator[](size_t const pIndex) const
    {
        return *(mArray + pIndex);
    }

    /** Assignment operator.
    * \remarks The copy constructor of \c VALUE_TYPE will be 
    * invoked in order to copy the value of elements to the
    * new array.
    */
    KDynamicArray& operator=(KDynamicArray const& pArray)
    {
        Reserve(pArray.mArrayCapacity);
        CopyArray(mArray, pArray.mArray, pArray.mValueCount);
        mValueCount = pArray.mValueCount;

        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    //
    //  WARNING!
    //
    //  Anything beyond these lines may not be documented accurately and is
    //  subject to change without notice.
    //
    ///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS

private:
    static void CopyArray(ValueType* pDest, ValueType const* pSrc, size_t pCount)
    {
        for( int i = 0; i < int(pCount); i++ )
        {
			#if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
			#pragma push_macro("new")
			#undef new
			#endif

			new(&(pDest[i])) ValueType(pSrc[i]);

			#if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
			#pragma pop_macro("new")
			#endif
        }
    }

    static void MoveArray(ValueType* pDest, ValueType const* pSrc, size_t pCount)
    {
        for( int i = 0; i < int(pCount); i++ )
        {
			#if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
			#pragma push_macro("new")
			#undef new
			#endif

			new(&(pDest[i])) ValueType(pSrc[i]);

			#if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
			#pragma pop_macro("new")
			#endif
        }

        for( int i = 0; i < int(pCount); i++ )
        {
            pSrc[i].~VALUE_TYPE();
        }
    }

    static void MoveOverlappingArray(ValueType* pDest, ValueType const* pSrc, size_t pCount)
    {
		for( int i = 0; i < int(pCount); i++ )
		{
			#if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
			#pragma push_macro("new")
			#undef new
			#endif

			new(&(pDest[i])) ValueType(pSrc[i]);

			#if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
			#pragma pop_macro("new")
			#endif

			pSrc[i].~VALUE_TYPE();
		}
    }

    static void MoveArrayBackwards(ValueType* pDest, ValueType const* pSrc, size_t pCount)
    {
		for( int i = 0; i < int(pCount); ++i )
		{
			#if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
			#pragma push_macro("new")
			#undef new
			#endif

			new(&(pDest[pCount-1-i])) ValueType(pSrc[pCount-1-i]);

			#if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
			#pragma pop_macro("new")
			#endif

			pSrc[pCount-1-i].~VALUE_TYPE();
		}
    }

    static void Fill(ValueType* pDest, ValueType const& pValue, size_t pCount)
    {
		for( int i = 0; i < int(pCount); i++ )
		{
			#if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32) )
			#pragma push_macro("new")
			#undef new
			#endif

			new(&(pDest[i])) ValueType(pValue);

			#if ( defined(MEMORY_DEBUG) && defined(_DEBUG) && defined(KARCH_ENV_WIN32))
			#pragma pop_macro("new")
			#endif
		}
    }


    ValueType* mArray;
    size_t mArrayCapacity;
    size_t mValueCount;

    AllocatorType mAllocator;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_COMPONENTS_KBASELIB_KLIB_KDYNAMICARRAY_H

