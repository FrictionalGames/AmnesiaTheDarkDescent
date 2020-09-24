/****************************************************************************************
 
   Copyright (C) 2013 Autodesk, Inc.
   All rights reserved.
 
   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.
 
****************************************************************************************/

//! \file fbxdynamicarray.h
#ifndef _FBXSDK_CORE_BASE_DYNAMICARRAY_H_
#define _FBXSDK_CORE_BASE_DYNAMICARRAY_H_

#include <fbxsdk/fbxsdk_def.h>

#include <fbxsdk/core/base/fbxcontainerallocators.h>

#include <fbxsdk/fbxsdk_nsbegin.h>

/** Template class for dynamic array holding objects.
  * \nosubgrouping
  * \see FbxStaticArray
  */
template <typename VALUE_TYPE, typename ALLOCATOR = FbxBaseAllocator> class FbxDynamicArray
{
public:
    //! Type of the elements in the array.
    typedef VALUE_TYPE ValueType;
    //! Type of the class used for allocating memory.
    typedef ALLOCATOR AllocatorType;

    //! Default constructor.
    FbxDynamicArray()
        : mArray(NULL)
        , mArrayCapacity(0)
        , mValueCount(0)
        , mAllocator(sizeof(ValueType))
    {
    }

    /** Constructor.
    * \param pInitialSize initial capacity of this array
     */
    FbxDynamicArray(const size_t pInitialSize)
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
    FbxDynamicArray(const FbxDynamicArray& pArray)
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
    ~FbxDynamicArray()
    {        
        for (size_t i = 0; i < mValueCount; i++)
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
    void Reserve(const size_t pCount)
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
    void PushBack(const ValueType& pValue, const size_t pNCopies = 1)
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

        FBX_ASSERT(mValueCount + pNCopies <= mArrayCapacity);

        Fill(mArray + mValueCount, pValue, pNCopies);

        mValueCount += pNCopies;
    }

    /** Inserts n objects at the specified position.
    * \param pIndex position index
    * \param pValue object to insert
    * \param pNCopies number of copies to append
    */
    void Insert(const size_t pIndex, const ValueType& pValue, const size_t pNCopies = 1)
    {
        FBX_ASSERT(pIndex >= 0);
        FBX_ASSERT(pIndex <= mValueCount);

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
        FBX_ASSERT(pNElements <= mValueCount);
         
        for (size_t i = mValueCount - pNElements; i < mValueCount; i++)
        {
            mArray[i].~VALUE_TYPE();
        }

        mValueCount -= pNElements;
    }

    /** Removes n objects at the specified position.
    * \param pIndex position index
    * \param pNElements number of objects to remove
    */
    void Remove(const size_t pIndex, size_t pNElements = 1)
    {
        FBX_ASSERT(pIndex >= 0);
        FBX_ASSERT(pIndex <= mValueCount);
        FBX_ASSERT(pIndex + pNElements <= mValueCount);

        if (pIndex + pNElements >= mValueCount)
        {
            PopBack(pNElements);
        }
        else
        {            
            for (size_t i = pIndex; i < pIndex + pNElements; i++)
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
    ValueType& operator[](const size_t pIndex)
    {
        return *(mArray + pIndex);
    }

    /** Gets nth object in the array.
    * \param pIndex position index
    */
    ValueType const& operator[](const size_t pIndex) const
    {
        return *(mArray + pIndex);
    }

    /** Assignment operator.
    * \remarks The copy constructor of \c VALUE_TYPE will be 
    * invoked in order to copy the value of elements to the
    * new array.
    */
    FbxDynamicArray& operator=(const FbxDynamicArray& pArray)
    {
        Reserve(pArray.mArrayCapacity);
        CopyArray(mArray, pArray.mArray, pArray.mValueCount);
        mValueCount = pArray.mValueCount;

        return *this;
    }

/*****************************************************************************************************************************
** WARNING! Anything beyond these lines is for internal use, may not be documented and is subject to change without notice! **
*****************************************************************************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    static void CopyArray(ValueType* pDest, const ValueType* pSrc, size_t pCount)
    {
        for( int i = 0; i < int(pCount); i++ )
        {
			new(&(pDest[i])) ValueType(pSrc[i]);	//in-place new won't allocate memory, so it is safe
        }
    }

    static void MoveArray(ValueType* pDest, const ValueType* pSrc, size_t pCount)
    {
        for( int i = 0; i < int(pCount); i++ )
        {
			new(&(pDest[i])) ValueType(pSrc[i]);	//in-place new won't allocate memory, so it is safe
        }

        for( int i = 0; i < int(pCount); i++ )
        {
            pSrc[i].~VALUE_TYPE();
        }
    }

    static void MoveOverlappingArray(ValueType* pDest, const ValueType* pSrc, size_t pCount)
    {
		for( int i = 0; i < int(pCount); i++ )
		{
			new(&(pDest[i])) ValueType(pSrc[i]);	//in-place new won't allocate memory, so it is safe
			pSrc[i].~VALUE_TYPE();
		}
    }

    static void MoveArrayBackwards(ValueType* pDest, const ValueType* pSrc, size_t pCount)
    {
		for( int i = 0; i < int(pCount); ++i )
		{
			new(&(pDest[pCount-1-i])) ValueType(pSrc[pCount-1-i]);	//in-place new won't allocate memory, so it is safe
			pSrc[pCount-1-i].~VALUE_TYPE();
		}
    }

    static void Fill(ValueType* pDest, const ValueType& pValue, size_t pCount)
    {
		for( int i = 0; i < int(pCount); i++ )
		{
			new(&(pDest[i])) ValueType(pValue);	//in-place new won't allocate memory, so it is safe
		}
    }

    ValueType*		mArray;
    size_t			mArrayCapacity;
    size_t			mValueCount;
    AllocatorType	mAllocator;
#endif /* !DOXYGEN_SHOULD_SKIP_THIS *****************************************************************************************/
};

#include <fbxsdk/fbxsdk_nsend.h>

#endif /* _FBXSDK_CORE_BASE_DYNAMICARRAY_H_ */
