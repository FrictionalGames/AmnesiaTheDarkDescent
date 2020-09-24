/*!  \file kfbxlayercontainer.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXLAYERCONTAINER_H
#define FBXFILESDK_KFBXPLUGINS_KFBXLAYERCONTAINER_H

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
#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/kfbxplugins/kfbxnodeattribute.h>
#include <fbxfilesdk/kfbxplugins/kfbxlayer.h>

#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>

#include <fbxfilesdk/kfbxmath/kfbxvector4.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;


/** \brief Contains a collection of KFbxLayer objects. 
  * This class is used for managing layers construction, destruction and access.
  * See KFbxLayerElement for more details.
  * \nosubgrouping
  * \see KFbxLayer
  */
class KFBX_DLL KFbxLayerContainer : public KFbxNodeAttribute
{
	KFBXOBJECT_DECLARE(KFbxLayerContainer,KFbxNodeAttribute);
public:

	/** Returns the type of node attribute.
	  */
	virtual EAttributeType GetAttributeType() const;

	/**
	  * \name Layer Management 
	  */
	//@{

	/** Creates a new layer on top of existing layers.
	  * \return     Index of created layer or -1 if an error occurs.
	  */
	int CreateLayer();

	//! Deletes all layers.
    void ClearLayers();

	/** Returns the number of layers.
	 * \return     The number of layers.
	 */
	int GetLayerCount() const;

	/** Returns the number of layers that contain the specified layer element type.
	  * \param pType     The specified Layer Element type.
      * \param pUVCount  When \c true, requests the UV layer element corresponding to the specified texture type.
	  * \return          The number of layers containing the specified layer element type.
	  */
	int GetLayerCount(KFbxLayerElement::ELayerElementType pType,  bool pUVCount=false) const;

	/** Returns the layer at the specified index.
	  *	\param pIndex     Layer index.
	  * \return           Pointer to the layer, or \c NULL if pIndex is out of range.
	  */
	KFbxLayer* GetLayer(int pIndex);

	/** Returns the layer at the specified index.
	  *	\param pIndex     Layer index.
	  * \return           Pointer to the layer, or \c NULL if pIndex is out of range.
	  */
	KFbxLayer const* GetLayer(int pIndex) const;

	/** Returns the n'th layer as specified by pIndex that contains the specified layer element type. 
      * If the pType is KFbxLayerElement::eUV, this method will return the n'th layer as specified by pIndex that contains the diffuse UV.
      * For example, GetLayer(int pIndex, KFbxLayerElement::eUV) is same as GetLayer(int pIndex, KFbxLayerElement::eDIFFUSE_TEXTURES, true).
	  *	\param pIndex     Layer index.
	  * \param pType      The specified layer element type.
      * \param pIsUV      When \c true, requests the UV layer element that corresponds with the specified texture type.
	  * \return           Pointer to the layer, or \c NULL if pIndex is out of range.
	  */
	KFbxLayer* GetLayer(int pIndex, KFbxLayerElement::ELayerElementType pType, bool pIsUV=false);

	/** Returns the n'th layer as specified by pIndex that contains the specified layer element type.
      * If the pType is KFbxLayerElement::eUV, this method will return the n'th layer as specified by pIndex that contains the diffuse UV.
      * For example, GetLayer(int pIndex, KFbxLayerElement::eUV) is same as GetLayer(int pIndex, KFbxLayerElement::eDIFFUSE_TEXTURES, true).
	  *	\param pIndex     Layer index.
	  * \param pType      The specified layer element type.
      * \param pIsUV      When \c true, requests the UV layer element that corresponds with the specified texture type.
	  * \return           Pointer to the layer, or \c NULL if pIndex is out of range.
	  */
	KFbxLayer const* GetLayer(int pIndex, KFbxLayerElement::ELayerElementType pType, bool pIsUV=false) const;

	/**	Returns the global index of the n'th layer as specified by pIndex that contains the specified layer element type.
	  * \param pIndex     Layer index of the specified type.
	  * \param pType      The specified layer element type.
      * \param pIsUV      When \c true, requests the UV layer element that corresponds with the specified texture type.
	  * \return           Global index of the n'th layer as specified by pIndex that contains the specified layer element type, or -1 if the layer is not found.
	  * \remarks          The returned index is the position of the layer in the global array of layers.
	  *                   You can use the returned index to call GetLayer(int pIndex).
	  */
	int GetLayerIndex(int pIndex, KFbxLayerElement::ELayerElementType pType, bool pIsUV=false) const;

	/** Converts the layer's global index to a type-specific index.
	  * \param pGlobalIndex     The index of the layer in the global array of layers.
	  * \param pType            The type upon which the type-specific index will be returned.
      * \param pIsUV            When \c true, requests the UV layer element that corresponds with the specified texture type.
	  * \return                 Layer index of the specified layer element type, or -1 if the layer element type is not found on the layer.
	  */
	int GetLayerTypedIndex(int pGlobalIndex, KFbxLayerElement::ELayerElementType pType, bool pIsUV=false) const;
	//@}

	/** Converts the reference mode from eDIRECT to eINDEX_TO_DIRECT.
	  * \param pLayer     The Layer to convert.
	  * \return           \c True if conversion is successful, or \c false otherwise.
	  */
	bool ConvertDirectToIndexToDirect(int pLayer);

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS

	virtual KFbxObject& Copy(const KFbxObject& pObject);

	int  GTC(kUInt i, int j);
	void* GT (int  i,    kUInt l, int j); 
	int  AT (void* t,    kUInt l, int j);
	int  GTI(char const* n, kUInt l, int j);
	int  GMC(kUInt i, void* n = NULL);
	void* GM (int  i,    kUInt l, void* n = NULL);
	int  AM (void* m,    kUInt l, void* n = NULL, bool b = false);
	int  GMI(char const* n, kUInt l, void* d = NULL);

	int AddToLayerElementsList(KFbxLayerElement* pLEl);
	void RemoveFromLayerElementsList(KFbxLayerElement* pLEl);

protected:
	KFbxLayerContainer(KFbxSdkManager& pManager, char const* pName);
	virtual void Destruct(bool pRecursive, bool pDependents);

	void CopyLayers(KFbxLayerContainer const* pLayerContainer);

	virtual void SetDocument(KFbxDocument* pDocument);
	virtual	bool ConnecNotify (KFbxConnectEvent const &pEvent);

	KArrayTemplate<KFbxLayer*> mLayerArray;
	KArrayTemplate<KFbxLayerElement*> mLayerElementsList;

	friend class KFbxScene;
	friend class KFbxGeometryConverter;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXLAYERCONTAINER_H

