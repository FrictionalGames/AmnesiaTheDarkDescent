/****************************************************************************************

Copyright (C) 2010 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxpropertymap.h
 */

#ifndef FBXFILESDK_FBXCORE_FBXCOLLECTION_KFBXPROPERTYMAP_H
#define FBXFILESDK_FBXCORE_FBXCOLLECTION_KFBXPROPERTYMAP_H

#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/components/kbaselib/klib/kmap.h>
#include <fbxfilesdk/kfbxplugins/kfbxproperty.h>
#include <fbxfilesdk/kfbxplugins/kfbxobject.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** A simple map class representing a dictionary-like data structure.
  * \nosubgrouping
  */
template <class K, class T, class Compare>
class KFBX_DLL KFbxMap
{
///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    typedef KMap<K, T, Compare> KMapDef;
    KMapDef mMap;
#endif // DOXYGEN_SHOULD_SKIP_THIS

public:
    typedef typename KMapDef::RecordType* kIterator;

    //! Constructor
    inline KFbxMap()
    {
    }

    /** Add a key-value pair as an element.
      * \param pKey The new key.
      * \param pValue The new value.
      */
    inline void Add(K const &pKey, T const &pValue)
    {
        mMap.Insert(pKey, pValue);
    }

    /** Find an element with a given key.
      * \param pKey The given key.
      * \return The iterator pointing to the found element or NULL if fails.
      */
    inline kIterator Find(K const &pKey) const
    {
        return (kIterator)mMap.Find( pKey );
    }

    /** Find an element with a given value.
      * \param pValue The given value.
      * \return The iterator pointing to the found element or NULL if fails.
      */
    inline kIterator Find(T const &pValue) const
    {
        kIterator lIterator = GetFirst();
        while (lIterator)
        {
            if (lIterator->GetValue()==pValue)
            {
                return lIterator;
            }
            lIterator = GetNext(lIterator);
        }
        return 0;
    }

    /** Remove an element from the map.
      * \param pIterator The given element.
      */
    inline void Remove(kIterator pIterator)
    {
        if (pIterator) mMap.Remove( pIterator->GetKey() );
    }

    /** Get the first element.
      * \return The the heading element.
      */
    inline kIterator GetFirst() const
    {
        return (kIterator)mMap.Minimum();
    }

    /** Get the next element of a given element.
      * \param The given element.
      * \return The next element.
      */
    inline kIterator GetNext(kIterator pIterator) const
    {
        return (kIterator)pIterator ? pIterator->Successor() : 0;
    }

    /** Remove all of the elements.
      */
    inline void Clear() 
    {
        mMap.Clear();
    }

    /** Reserve the space for given number elements.
      * \param pSize The given number.
      */
    inline void Reserve(int pSize)
    {
        mMap.Reserve( pSize );
    }

    /** Query the count of elements in the map.
      * \return The count of elements.
      */
    inline int GetCount() const
    {
        return mMap.GetSize();
    }
};

/** This class maps types to properties.
  * \nosubgrouping
  */
template <class T,class Compare>
class KFBX_DLL KFbxPropertyMap : public KFbxMap<T,KFbxProperty,Compare>
{
public:
    //! Constructor
    inline KFbxPropertyMap()
    {
    }

    /** Get the property contained in an element.
      * \param pIterator The given element.
      * \return The property.
      */
    inline KFbxProperty Get(typename KFbxMap<T,KFbxProperty,Compare>::kIterator pIterator)
    {
        return pIterator ? pIterator->GetValue() : KFbxProperty();
    }
};

/** This class maps types to objects.
  * \nosubgrouping
  */
template <class T, class Compare>
class KFBX_DLL KFbxObjectMap : public KFbxMap<T,KFbxObject*,Compare>
{
public:
    //! Constructor
    inline KFbxObjectMap()
    {
    }

    /** Get the object contained in an element.
      * \param pIterator The given element.
      * \return The object.
      */
    inline KFbxObject* Get(typename KFbxMap<T,KFbxObject*,Compare>::kIterator pIterator)
    {
        return pIterator ? pIterator->GetValue() : 0;
    }
};

/** This class is used to compare strings.
  * \nosubgrouping
  */
class KFbxMapKStringCompare
{
public:
    /** Functor used to compared two strings.
      * \pKeyA The left string.
      * \pKeyB The right string.
      * \return If the left string is greater than the right, return 1; If the left string is less than the right, return -1; Or return 0 if both are false.
      */
    inline int operator()(KString const &pKeyA, KString const &pKeyB) const
    {
        return (pKeyA < pKeyB) ? -1 : ((pKeyB < pKeyA) ? 1 : 0);
    }
};

/** This class maps strings to properties.
  * \nosubgrouping
  */
class KFBX_DLL KFbxPropertyStringMap : public KFbxPropertyMap<KString,KFbxMapKStringCompare>
{
public:
    //! Constructor
    inline KFbxPropertyStringMap()
    {
    }
};

/** This class maps name strings to objects.
  * \nosubgrouping
  */
class KFBX_DLL KFbxObjectStringMap : public KFbxObjectMap<class KString,class KFbxMapKStringCompare>
{
public:
    //! Constructor
    inline KFbxObjectStringMap()
    {
    }
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_FBXCORE_FBXCOLLECTION_KFBXPROPERTYMAP_H

