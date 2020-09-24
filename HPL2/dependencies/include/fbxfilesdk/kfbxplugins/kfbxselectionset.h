/*!  \file kfbxselectionset.h
*/

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXSELECTIONSET_H
#define FBXFILESDK_KFBXPLUGINS_KFBXSELECTIONSET_H

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

#include <fbxfilesdk/fbxcore/fbxcollection/kfbxcollection.h>
#include <fbxfilesdk/components/kbaselib/klib/kname.h>
#include <fbxfilesdk/kfbxplugins/kfbxsdkmanager.h>
#include <fbxfilesdk/kfbxplugins/kfbxnode.h>
#include <fbxfilesdk/kfbxplugins/kfbxselectionnode.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** FBX SDK selection set class.
* \nosubgrouping
*  Represents a set of selected objects (KFbxObject) and components. This is a non-exclusive (multiple membership) collection. 
*  Objects (KfbxObject) can be added to a KFbxSelectionSet directly, but to add components (vertexes, edges, or faces) 
*  you create a selection node (KFbxSelectionNode) to group the object and its components together as a single item to be added. 
*  \see KFbxSelectionNode 
*/
class KFBX_DLL KFbxSelectionSet : public KFbxCollection
{
    KFBXOBJECT_DECLARE(KFbxSelectionSet, KFbxCollection);

public:
    /** This property stores annotation of the selection set.
    * Default value is "".
    */
    KFbxTypedProperty<fbxString>        SelectionSetAnnotation;

    /**
    * \name Utility functions
    */
    //@{

    /**  Get the selected faces of a specified object. 
    *    \param pObj                The specified object.
    *    \param pPolygonIndexArray  The array to take the indices of the selected faces.
    *    \remarks                   The indices of selected faces will be put in pPolygonIndexArray.
    */
    void GetFaceSelection( KFbxObject* pObj,KArrayTemplate<int>& pPolygonIndexArray ) const;

	/**  Get the selected edges of a specified object. 
	*    \param pObj                The specified object.
	*    \param pEdgeIndexArray     The array to take the indices of the selected edges.
	*    \remarks                   The indices of selected face will be put in pEdgeIndexArray.
	*/
    void GetEdgeSelection( KFbxObject* pObj,KArrayTemplate<int>& pEdgeIndexArray ) const; 

	/**  Get the selected vertices of a specified object. 
	*    \param pObj                The specified object.
	*    \param pVertexIndexArray   The array to take the indices of the selected vertices.
	*    \remarks                   The indices of selected face will be put in pVertexIndexArray.
	*/
    void GetVertexSelection( KFbxObject* pObj,KArrayTemplate<int>& pVertexIndexArray ) const; 

    /**  Get list of two types of member in the selection set: SelectionNodes and Directly contained objects.
    *    \param pSelectionNodeList  The array to take selection nodes of the selection set.
    *    \param pDirectObjectList   The array to take directly contained objects of the selection set.
    *    \remarks There might be two types members for a selection set: selection node and directly contained object.
    *    They will be listed in pSelectionNodeList and pDirectObjectList separately.
    */
    void GetSelectionNodesAndDirectObjects(KArrayTemplate<KFbxSelectionNode*> &pSelectionNodeList, KArrayTemplate<KFbxObject*> &pDirectObjectList) const; 

    //@}

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS

protected:
    KFbxSelectionSet(KFbxSdkManager& pManager, char const* pName);
    bool ConstructProperties(bool pForceSet); 

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif /* FBXFILESDK_KFBXSELECTIONSET_H */
