/*!  \file kfbxselectionnode.h
*/

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXSELECTIONNODE_H
#define FBXFILESDK_KFBXPLUGINS_KFBXSELECTIONNODE_H

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
#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/kfbxplugins/kfbxsdkmanager.h>
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** KFbxSelectionNode is an auxiliary class for Selection Set. 
* \nosubgrouping
* Used to group objects with their components (e.g. vertex, edge, and face), when adding it to a selection set (KFbxSelectionSet).
* \see KFbxSelectionSet 
*/
class KFBX_DLL KFbxSelectionNode : public KFbxObject
{
    KFBXOBJECT_DECLARE(KFbxSelectionNode, KFbxObject);

public:
    /** \enum ESelectType               SelectionNode type identifier.
    * - \e eVERTEX_LEVEL                Vertex level selection.
    * - \e eEDGE_LEVEL                  Edge level selection.
    * - \e eFACE_LEVEL                  Face level selection.
    * - \e eOBJECT_LEVEL                Object level selection.
    * - \e eSELECT_TYPE_NUM             Number of Select Types.
    */
    enum ESelectType
    {
        eVERTEX_LEVEL,
        eEDGE_LEVEL,
        eFACE_LEVEL,
        eOBJECT_LEVEL,
        eSELECT_TYPE_NUM
    };

    /** Set an object whose components or itself is contained in the SelectionNode.
    * \param pObject      The object whose components or itself is contained in the SelectionNode. 
    * \return \c true if the object is set successfully. \c false otherwise.
    * \remarks It is possible a SDK user will try to set multiple objects to one SelectionNode, but only the last one will be kept.
    */
    bool SetSelectionObject(KFbxObject* pObject);

    /** Get the object whose components or itself or both are contained in the SelectionNode.
    * \return The object whose components or itself or both are contained in the SelectionNode. 
    */
    KFbxObject* GetSelectionObject() const;

    /** To detect if the SelectionNode is valid.
    *  \return \c true if this is a valid SelectionNode. \c false otherwise.
    *  \remarks SelectionNode is valid if selection object is set.
    *   SelectionNode is not valid if no selection object is set.
    */
    bool IsValid() const;

    /** \c true means the object itself is also in the selection set; 
    *   \c false means only the object's components are in the selection set, the object is not.
    */
    bool mIsTheNodeInSet;

    /** Index array for selected vertices.
    */
    KArrayTemplate<int> mVertexIndexArray;  

    /** Index array for selected edges.
    */
    KArrayTemplate<int> mEdgeIndexArray; 

    /** Index array for selected faces.
    */
    KArrayTemplate<int> mPolygonIndexArray;

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS

    KArrayTemplate<KArrayTemplate<int>*> mSubTypeSelectArray;
    static const char* SELECT_TYPE_NAMES[(int)eSELECT_TYPE_NUM];

protected:
    KFbxSelectionNode(KFbxSdkManager& pManager, char const* pName);
    bool ConnecNotify (KFbxConnectEvent const &pEvent);

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};


#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif /* FBXFILESDK_KFBXSELECTIONNODE_H */
