/****************************************************************************************

Copyright (C) 2010 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxcollectionexclusive.h
 */

#ifndef FBXFILESDK_FBXCORE_FBXCOLLECTION_KFBXCOLLECTIONEXCLUSIVE_H
#define FBXFILESDK_FBXCORE_FBXCOLLECTION_KFBXCOLLECTIONEXCLUSIVE_H

#include <fbxfilesdk/fbxcore/fbxcollection/kfbxcollection.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** Class for exclusive collections. An object (KFbxObject) should belong to only one exclusive collection at most.
  * \nosubgrouping
  */
class KFBX_DLL KFbxCollectionExclusive : public KFbxCollection
{
    KFBXOBJECT_DECLARE(KFbxCollectionExclusive, KFbxCollection);

public:
    /** Add a member if it's not a member of any other KFbxCollectionExclusive objects.
      * \param pMember Object to be added
      */
    bool AddMember(KFbxObject* pMember);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    ///////////////////////////////////////////////////////////////////////////////
    //  WARNING!
    //	Anything beyond these lines may not be documented accurately and is 
    // 	subject to change without notice.
    ///////////////////////////////////////////////////////////////////////////////
protected:
    KFbxCollectionExclusive(KFbxSdkManager& pManager, char const *pName);

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_FBXCORE_FBXCOLLECTION_KFBXCOLLECTIONEXCLUSIVE_H
