/*!  \file kfbxmaterialconverter.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXMATERIALCONVERTER_H
#define FBXFILESDK_KFBXPLUGINS_KFBXMATERIALCONVERTER_H

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

#include <fbxfilesdk/kfbxplugins/kfbxlayer.h>
#include <fbxfilesdk/components/kbaselib/klib/kpair.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxObject;
class KFbxSdkManager;
class KFbxScene;
class KFbxGeometry;
class KFbxLayer;
class KFbxSurfaceMaterial;
class KFbxLayeredTexture;
class KFbxTexture;

class LayerConfig;

class KFbxMaterialConverterImpl;

/** 
  * \brief This class provides functions to restructure the material and textures
  * applied to geometries from FBX v5 material system to v6-and-up or the other way around.
  * \nosubgrouping
  * \see KFbxSurfaceMaterial, KFbxTexture
  */
class KFBX_DLL KFbxMaterialConverter
{
public:
	KFbxMaterialConverter( KFbxSdkManager& mManager, KFbxSurfaceMaterial* pDefaultMaterial = NULL);
	~KFbxMaterialConverter();

	/** Moves textures in texture layer elements to connections
	  * on the corresponding material's color properties, for all geometries
	  * in the scene.(Convert scene from FBX v5 material system to v6-and-up)
	  * \param pScene The scene whose geometries should be converted.
	  * \return true on success, false otherwise
	  */
	bool ConnectTexturesToMaterials( KFbxScene& pScene );

	/** Moves textures in texture layer elements to connections
	* on the corresponding material's color properties, for the given geometry
	* in the scene.(Convert scene from FBX v5 material system to v6-and-up)
	* \param pNode The geometry node to be converted.
	* \return true on success, false otherwise
	*/
	bool ConnectTexturesToMaterials( KFbxNode* pNode );

	/** This is the reverse operation of ConnectTexturesToMaterials()
	  * Textures connected to Materials' color properties are stored
	  * in layer elements, and their connections to the color properties
	  * are broken.(Convert scene from FBX v6-and-up material system to v5)
	  * \param pScene The scene whose geometries should be converted.
	  * \return true if all geometries were converted, false otherwise
	  */
	bool AssignTexturesToLayerElements( KFbxScene& pScene );

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
	
	typedef KPair<KFbxTexture*, KFbxLayerElementTexture::EBlendMode> TexData;

	KFbxSdkManager& mManager;
	KFbxSurfaceMaterial* mDefaultMaterial;
    KFbxMaterialConverterImpl* mImpl;


	void GetTextures( int pComponent, KFbxLayer* pLayer, LayerConfig& pLayerConfig ) const;
	KFbxSurfaceMaterial* GetMaterial( int pComponent, KFbxLayer* pLayer, KFbxNode* pNode, bool pLookOnNode );
	int                  GetMaterialOrder( int pComponent, KFbxLayer* pLayer, KFbxNode* pNode, bool pLookOnNode );

	bool HasGoodMappingModes( KFbxNode* pNode, KFbxGeometry* pGeom ) const;
	void ConnectTextures( KFbxSurfaceMaterial* pMat, KFbxObject* pTexture, int pTextureType ) const;
	bool HasPerFaceMaterialMapping( KFbxGeometry* pGeom ) const;
	void SetTextureUVSets( KFbxGeometry* pGeom ) const;
	bool HasTextures( KFbxGeometry* pGeom ) const;

	void GetTextureList( KArrayTemplate<TexData>& pTextures, KFbxLayeredTexture* pTex ) const;

	KFbxLayer* FindLayerForTexture( KFbxTexture* pTex, 
								  KFbxLayerElement::ELayerElementType pTexType, 
                                  KFbxLayerElementTexture::EBlendMode pTexBlendMode, 
								  KFbxGeometry* pGeom, 
								  int lComponentIndex, 
								  int lStartIndex = 0 ) const;

	void InitTextureElement( KFbxLayerElementTexture* pTexElm, int pComponentCount,
        KFbxLayerElementTexture::EBlendMode pMode) const;

	bool AssignTexturesToLayerElements( KFbxNode* pNode); 

	bool HasTextureLayerElements( KFbxGeometry& pGeom ) const;

	void ConvertToPerFaceMapping( KFbxMesh* pGeom ) const;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS 
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXMATERIALCONVERTER_H

