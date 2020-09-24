/****************************************************************************************

   Copyright (C) 2011 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxbindingtablebase.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXBINDINGTABLEBASE_H 
#define FBXFILESDK_KFBXPLUGINS_KFBXBINDINGTABLEBASE_H

#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/kfbxplugins/kfbxbindingtableentry.h>
#include <fbxfilesdk/components/kbaselib/klib/kdynamicarray.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** A binding table represents a collection of bindings
  * from source types such as KFbxObjects, or KFbxLayerElements
  * to destinations which can be of similar types.
  * \See KFbxBindingTableEntry.
  * \nosubgrouping
  */
class KFBX_DLL KFbxBindingTableBase : public KFbxObject
{
    KFBXOBJECT_DECLARE_ABSTRACT(KFbxBindingTableBase,KFbxObject);

public:
    /** Adds a new entry to the binding table.
      * \return The new entry.
      */
    KFbxBindingTableEntry& AddNewEntry();

    /** Query the number of table entries.
      * \return The number of entries.
      */
    size_t GetEntryCount() const;

    /** Access a table entry. 
      * \param pIndex Valid range is [0, GetEntryCount()-1].
      * \return A valid table entry if pIndex is valid. Otherwise the value is undefined.
      */
    KFbxBindingTableEntry const& GetEntry( size_t pIndex ) const;

    /** Access a table entry.
      * \param pIndex Valid range is [0, GetEntryCount()-1].
      * \return A valid table entry if pIndex is valid. Otherwise the value is undefined.
      */
    KFbxBindingTableEntry& GetEntry( size_t pIndex );

    /** Retrieve the table entry  for the given source value.
      * \param pSrcName The source value to query.
      * \return The corresponding entry, or NULL if no entry in
      *             the table has a source equal in value to pSrcName.
      */
    KFbxBindingTableEntry const* GetEntryForSource(char const* pSrcName) const;

    /** Retrieve the table entry for the given destination value.
      * \param pDestName The destination value to query.
      * \return The corresponding entry, or NULL if no entry in
      *             the table has a destination equal in value to pDestName.
      */
    KFbxBindingTableEntry const* GetEntryForDestination(char const* pDestName) const;

    ///////////////////////////////////////////////////////////////////////////////
    //
    //  WARNING!
    //
    //    Anything beyond these lines may not be documented accurately and is 
    //     subject to change without notice.
    //
    ///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    virtual KFbxObject& Copy(const KFbxObject& pObject);

protected:
    KFbxBindingTableBase(KFbxSdkManager& pManager, char const* pName);
    virtual ~KFbxBindingTableBase() = 0; // make this class abstract

private:
    KDynamicArray<KFbxBindingTableEntry> mEntries;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXBINDINGTABLEBASE_H

