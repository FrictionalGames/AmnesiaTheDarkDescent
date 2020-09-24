/****************************************************************************************
 
   Copyright (C) 2013 Autodesk, Inc.
   All rights reserved.
 
   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.
 
****************************************************************************************/

//! \file fbxset.h
#ifndef _FBXSDK_CORE_BASE_SET_H_
#define _FBXSDK_CORE_BASE_SET_H_

#include <fbxsdk/fbxsdk_def.h>

#include <fbxsdk/core/base/fbxredblacktree.h>
#include <fbxsdk/core/base/fbxmap.h>

#include <fbxsdk/fbxsdk_nsbegin.h>

/** Class to manipulate set
* \nosubgrouping
*/
class FBXSDK_DLL FbxSet
{
public:
	/**
	  * \name Constructors and Destructor
	  */
	//@{
		/** Int constructor.
		  * \param pItemPerBlock The number of items that every block included
		  */
		FbxSet(int pItemPerBlock=20);

		/** Copy constructor.
		  * \param other Given object.
		  */
		FbxSet(const FbxSet& other);

		//! Destructor.
		~FbxSet();
	//@}

    // Add and remove
	/** If can't find the matching item,append a item at the end of the array.
	* If find the matching item ,insert the new item before the matching item. 
    * \param pReference The value of Reference in new item, also is the character for matching.
	* \param pItem The value of Item in new item.
	* \return If add successfully return true,otherwise return false.
    */
    bool Add(FbxHandle pReference, FbxHandle pItem);
	
	/** Remove the first matching item, whose reference is the same as given.
	* \param pReference The given reference.
	* \return If remove successfully return true,otherwise return false.
	*/
    bool Remove(FbxHandle pReference);
	
	/** Remove all the matching item, whose item is the same as given.
	* \param pItem The given item.
	* \return If remove successfully return true,otherwise return false.
	*/
    bool RemoveItem(FbxHandle pItem);

    /** Set first matching item with the given parameter.
    * \param pReference The character for matching.
	* \param pItem  The value of Item that the matching item will be set.
	* \return If set successfully return true,otherwise return false.
    */
    bool SetItem(FbxHandle pReference, FbxHandle pItem);

    /** Get first matching item with the given parameter.
    * \param pReference The character for matching.
	* \param pIndex The pointer to the index of the matching item.
	* \return The value of Item in the matching item.
    * \remarks If there are multiple elements that match the character, the index returned is unspecified.
    */
    FbxHandle Get(FbxHandle pReference, int* pIndex=NULL) const;

	//! Delete the array.
    void Clear();

    // Index manipulation
	/** Get the item of the given index.
    * \param pIndex The index for matching.
	* \param pReference The pointer to the Reference of the matching item.
	* \return The value of Item in the matching item.
    */
    FbxHandle GetFromIndex(int pIndex, FbxHandle* pReference=NULL) const;

	/** Remove the item of the given index
	* \param pIndex The given index.
	* \return If remove successfully return true,otherwise return false.
	*/
    bool RemoveFromIndex(int pIndex);

    // Get The Count
	/** Get number of items in the array.
	* \return The number of items in the array.
	*/
    int GetCount() const { return mSetCount; }

    // Sorting
	/** Swap the value of Reference and Item in every item of array,
	 *  and sort the new array with the value of Reference. 
	 * \return If swap successfully return true,otherwise return false.
	 */
    bool Swap()const;

	//The array can be sorted only if the private member:mIsChanged be true.
	/** Sort the array according the value of Reference in each item.
	* \return If sort successfully return true,otherwise return false.
	*/
    bool Sort()const;

    //! FbxString assignment operator.
    const FbxSet& operator=(const FbxSet&);

/*****************************************************************************************************************************
** WARNING! Anything beyond these lines is for internal use, may not be documented and is subject to change without notice! **
*****************************************************************************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
	struct SSet;

private:
    // internal functions for Sets manipulation
    SSet* FindEqual(FbxHandle pReference) const;

private:
    SSet*			mSetArray;
    int				mSetCount;
    int				mBlockCount;
    int				mItemPerBlock;
    mutable bool	mIsChanged;
#endif /* !DOXYGEN_SHOULD_SKIP_THIS *****************************************************************************************/
};

/** This class implements an efficient set based on value comparison, which stores values.
* It executes insertion, deletion and query operations in O(log(n)) time. */
template <typename Type, typename Compare=FbxLessCompare<Type>, typename Allocator=FbxBaseAllocator> class FbxSet2
{
protected:
	//! This class defines the value type used by the set.
	class Value
	{
	/*****************************************************************************************************************************
	** WARNING! Anything beyond these lines is for internal use, may not be documented and is subject to change without notice! **
	*****************************************************************************************************************************/
	#ifndef DOXYGEN_SHOULD_SKIP_THIS
	public:
		typedef const Type KeyType;
		typedef const Type ConstKeyType;
		typedef const Type ValueType;
		typedef const Type ConstValueType;

		inline Value(const Type& pValue) : mValue(pValue){}
		inline KeyType& GetKey() const { return mValue; }
		inline ConstKeyType& GetKey(){ return mValue; }
		inline ValueType& GetValue() const { return mValue; }
		inline ConstValueType& GetValue(){ return mValue; }

	protected:
		ValueType mValue;

	private:
		Value& operator=(const Value&);
	#endif /* !DOXYGEN_SHOULD_SKIP_THIS *****************************************************************************************/
	};

	//! Declaration of the storage type used by the set.
	typedef FbxRedBlackTree<Value, Compare, Allocator> StorageType;

public:
	typedef Type ValueType;
	typedef typename StorageType::RecordType        RecordType;
	typedef typename StorageType::IteratorType      Iterator;
	typedef typename StorageType::ConstIteratorType ConstIterator;

	/** Preallocate memory.
	* \param pRecordCount The number of elements.
	*/
	inline void Reserve(unsigned int pRecordCount)
	{
		mTree.Reserve(pRecordCount);
	}

	//! Retrieve the number of values it holds.
	inline int GetSize() const
	{
		return mTree.GetSize();
	}

	/** Insert a value.
	* \param pValue The value.
	* \return If the value is already present in the map, returns the existing value and false; else returns the pointer to the new value and true. */
	inline FbxPair<RecordType*, bool> Insert(const ValueType& pValue)
	{
		return mTree.Insert(Value(pValue));
	}

	/** Delete a value.
	* \param pValue The value.
	* \return \c true if success, \c false if value is not found. */
	inline int Remove(const ValueType& pValue)
	{
		return mTree.Remove(pValue);
	}

	//! Clear the set.
	inline void Clear()
	{
		mTree.Clear();
	}

	//! Query whether the set is empty.
	inline bool Empty() const
	{
		return mTree.Empty();
	}

	//! Retrieve the begin iterator of the set.
	Iterator Begin()
	{
		return Iterator(Minimum());
	}

	//! Retrieve the end iterator of the set.
	Iterator End()
	{
		return Iterator();
	}

	//! Retrieve the begin iterator of the set.
	ConstIterator Begin() const
	{
		return ConstIterator(Minimum());
	}

	//! Retrieve the end iterator of the set.
	ConstIterator End() const
	{
		return ConstIterator();
	}

	/** Find a given value in the set.
	* \param pValue The value to find.
	* \return The value in the set, or NULL if the value is not found in the set. */
	inline const RecordType* Find(const ValueType& pValue) const
	{
		return mTree.Find(pValue);
	}

	/** Find a given value in the set.
	* \param pValue The value to find.
	* \return The value in the set, or NULL if the value is not found in the set. */
	inline RecordType* Find(const ValueType& pValue)
	{
		return mTree.Find(pValue);
	}

	//! Retrieve the minimum value in the set.
	inline const RecordType* Minimum() const
	{
		return mTree.Minimum();
	}

	//! Retrieve the minimum value in the set.
	inline RecordType* Minimum()
	{
		return mTree.Minimum();
	}

	//! Retrieve the maximum value in the set.
	inline const RecordType* Maximum() const
	{
		return mTree.Maximum();
	}

	//! Retrieve the maximum value in the set.
	inline RecordType* Maximum()
	{
		return mTree.Maximum();
	}

	//! Equality operator.
	inline bool operator==(const FbxSet2<Type, Compare, Allocator>& pOther) const
	{
		return (this == &pOther) || (mTree == pOther.mTree);
	}

	//! Inequality operator.
	inline bool operator != (const FbxSet2<Type, Compare, Allocator>& pOther) const
	{
		return !(*this == pOther);
	}

	/** Intersect with another set.
	* \param pOther The other set.
	* \return The intersection set of the two sets. */
	inline FbxSet2 Intersect(const FbxSet2& pOther) const
	{
		FbxSet2 lReturn;
		ConstIterator lBegin = Begin();
		for (; lBegin != End(); ++lBegin)
		{
			if (pOther.Find(lBegin->GetValue()) != NULL)
				lReturn.Insert(lBegin->GetValue());
		}
		return lReturn;
	}

	/** Unite with another set.
	* \param pOther The other set.
	* \return The union set of the two sets (no duplicated items). */
	inline FbxSet2 Union(const FbxSet2& pOther) const
	{
		FbxSet2 lReturn(*this);
		ConstIterator lBegin = pOther.Begin();
		for (; lBegin != End(); ++lBegin)
		{
			if (Find(lBegin->GetValue()) == NULL)
				lReturn.Insert(lBegin->GetValue());
		}
		return lReturn;
	}

/*****************************************************************************************************************************
** WARNING! Anything beyond these lines is for internal use, may not be documented and is subject to change without notice! **
*****************************************************************************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    inline FbxSet2(){}
    inline FbxSet2(const FbxSet2& pSet) : mTree(pSet.mTree){}
    inline ~FbxSet2(){ Clear(); }

private:
    StorageType mTree;
#endif /* !DOXYGEN_SHOULD_SKIP_THIS *****************************************************************************************/
};

#include <fbxsdk/fbxsdk_nsend.h>

#endif /* _FBXSDK_CORE_BASE_SET_H_ */
