/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxstatisticsfbx.h
 */

#ifndef FBXFILESDK_KFBXIO_KFBXSTATISTICSFBX_H
#define FBXFILESDK_KFBXIO_KFBXSTATISTICSFBX_H


#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxstatistics.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** Local class used only inside the kfbxreaderfbxXXX.cxx files
*/
class KFbxStatisticsFbx : public KFbxStatistics
{
public:
    virtual bool AddItem(KString& pItemName, int pItemCount)
    {
        mItemName.Add( FbxSdkNew< KString >(pItemName) );
        mItemCount.Add( pItemCount);
        return true;
    };
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXIO_KFBXSTATISTICSFBX_H

