/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbximplementationutils.h
 */
#ifndef FBXFILESDK_KFBXPLUGINS_KFBXIMPLEMENTATIONUTILS_H 
#define FBXFILESDK_KFBXPLUGINS_KFBXIMPLEMENTATIONUTILS_H

#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbximplementation.h>
#include <fbxfilesdk/kfbxplugins/kfbxbindingoperator.h>
#include <fbxfilesdk/kfbxplugins/kfbxoperatorentryview.h>
#include <fbxfilesdk/kfbxplugins/kfbxpropertyentryview.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** Get KFbxImplementation from KFbxObject.
  * \param pObject                  KFbxObject to get KFbxImplementation.
  * \param pImplementationTarget    Name of the implementation property to get.
  * \return KFbxImplementation      Pointer to KFbxImplementation.
  */
KFBX_DLL KFbxImplementation const* GetImplementation( KFbxObject const* pObject, char const* pImplementationTarget );

/** Get bound property value from KFbxBindingTable.
  * \param pBindingTable            KFbxBindingTable to get bound property value.
  * \param pEntryName               Name of the Entry type to get.
  * \param pImplementation          KFbxImplementation of the bound property value to get if the Entry type is KFbxOperatorEntryView::sEntryType.
  * \param pBoundObject             KFbxObject of the bound property value to get if the Entry type is KFbxPropertyEntryView::sEntryType.
  * \param pValue                   Pointer to bound property value from KFbxBindingTable.
  * \return Whether get bound property value success or not.
  */
template <typename T> bool GetBoundPropertyValue(KFbxBindingTable const* pBindingTable,
                                                 char const* pEntryName, 
                                                 KFbxImplementation const* pImplementation,
                                                 KFbxObject const* pBoundObject,
                                                 T& pValue)
{
    if ((NULL != pImplementation) && (NULL != pBindingTable) && (NULL != pBoundObject) && (NULL != pEntryName))
    {
        KFbxBindingTableEntry const* lEntry = pBindingTable->GetEntryForDestination(pEntryName);

        if (NULL != lEntry)
        {
            if (strcmp(lEntry->GetEntryType(true), KFbxPropertyEntryView::sEntryType) == 0)
            {
                char const* lPropName = lEntry->GetSource();
                KFbxProperty lProp = pBoundObject->FindPropertyHierarchical(lPropName);
                if (lProp.IsValid())
                {
                    KFbxGet(lProp, pValue);
                    return true;
                }
            }
            else if (strcmp(lEntry->GetEntryType(true), KFbxOperatorEntryView::sEntryType) == 0)
            {
                char const* lOperatorName = lEntry->GetSource();
                KFbxBindingOperator const* lOp = pImplementation->GetOperatorByTargetName(lOperatorName);
                if (lOp)
                {
                    return lOp->Evaluate(pBoundObject, &pValue);
                }
            }
        }
    }

    return false;
}

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXIMPLEMENTATIONUTILS_H

