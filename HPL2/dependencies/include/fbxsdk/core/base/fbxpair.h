/****************************************************************************************
 
   Copyright (C) 2013 Autodesk, Inc.
   All rights reserved.
 
   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.
 
****************************************************************************************/

//! \file fbxpair.h
#ifndef _FBXSDK_CORE_BASE_PAIR_H_
#define _FBXSDK_CORE_BASE_PAIR_H_

#include <fbxsdk/fbxsdk_def.h>

#include <fbxsdk/fbxsdk_nsbegin.h>

/** This class template holds a pair of objects.
* \nosubgrouping
*/
template <typename S, typename T> class FbxPair
{
public:
    //! Constructor.
	inline FbxPair() : mFirst(), mSecond() {}

    /** Constructor.
    * \param pFirst The first object.
    * \param pSecond The second object.
    */
	inline FbxPair(const S& pFirst, const T& pSecond) : mFirst(pFirst), mSecond(pSecond) {}

    /** Assignment operator.
    * \param pPair The pair to be copied.
    */
	inline FbxPair<S,T>& operator=(const FbxPair<S,T>& pPair)
	{
		mFirst = pPair.mFirst;
		mSecond = pPair.mSecond;

		return *this;
	}

    //! The first object in the pair.
	S mFirst;
    //! The second object in the pair.
	T mSecond;
};

#include <fbxsdk/fbxsdk_nsend.h>

#endif /* _FBXSDK_CORE_BASE_PAIR_H_ */
