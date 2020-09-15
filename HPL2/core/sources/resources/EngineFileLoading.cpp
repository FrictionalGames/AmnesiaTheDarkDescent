/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "resources/EngineFileLoading.h"

#include "resources/XmlDocument.h"
#include "resources/Resources.h"
#include "resources/TextureManager.h"
#include "resources/MaterialManager.h"

#include "math/Math.h"

#include "system/String.h"

#include "scene/World.h"
#include "scene/LightPoint.h"
#include "scene/LightSpot.h"
#include "scene/LightBox.h"
#include "scene/MeshEntity.h"
#include "scene/SoundEntity.h"
#include "scene/ParticleEmitter.h"
#include "scene/ParticleSystem.h"
#include "scene/BillBoard.h"
#include "scene/Beam.h"
#include "scene/GuiSetEntity.h"
#include "scene/RopeEntity.h"
#include "scene/FogArea.h"

#include "graphics/Graphics.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/VertexBuffer.h"
#include "graphics/Mesh.h"
#include "graphics/SubMesh.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// DEFINES
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	#define kBeginWorldEntityLoad()		\
		tString sName = apElement->GetAttributeString("Name");
	
	#define kEndWorldEntityLoad(pEntity)		\
		SetupWorldEntity(pEntity, apElement);	\
		return pEntity;

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CREATE ENTITIES
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cFogArea* cEngineFileLoading::LoadFogArea(cXmlElement* apElement, const tString& asNamePrefix, cWorld *apWorld, bool abStatic)
	{
		kBeginWorldEntityLoad();

		cFogArea *pFog = apWorld->CreateFogArea(asNamePrefix+sName, abStatic);

		if(pFog)
		{
			pFog->SetColor(apElement->GetAttributeColor("Color",cColor(1,1)));
			pFog->SetStart(apElement->GetAttributeFloat("Start", 0));
			pFog->SetEnd(apElement->GetAttributeFloat("End", 0));
			pFog->SetFalloffExp(apElement->GetAttributeFloat("FalloffExp", 0));
			pFog->SetShowBacksideWhenInside(apElement->GetAttributeBool("ShownBacksideWhenInside", true));
			pFog->SetShowBacksideWhenOutside(apElement->GetAttributeBool("ShownBacksideWhenOutside", true));
		}

		kEndWorldEntityLoad(pFog);
	}

	//-----------------------------------------------------------------------

	cParticleSystem* cEngineFileLoading::LoadParticleSystem(cXmlElement* apElement, const tString& asNamePrefix, cWorld *apWorld)
	{
		kBeginWorldEntityLoad();
		
		tString sFile = apElement->GetAttributeString("File");

		cParticleSystem *pPS = apWorld->CreateParticleSystem(asNamePrefix+sName,sFile,1);

		if(pPS)
		{
			pPS->SetColor(apElement->GetAttributeColor("Color",cColor(1,1)));
			pPS->SetFadeAtDistance(apElement->GetAttributeBool("FadeAtDistance", false));
			pPS->SetMinFadeDistanceStart(apElement->GetAttributeFloat("MinFadeDistanceStart"));
			pPS->SetMinFadeDistanceEnd(apElement->GetAttributeFloat("MinFadeDistanceEnd"));
			pPS->SetMaxFadeDistanceStart(apElement->GetAttributeFloat("MaxFadeDistanceStart"));
			pPS->SetMaxFadeDistanceEnd(apElement->GetAttributeFloat("MaxFadeDistanceEnd"));
		}
		
		kEndWorldEntityLoad(pPS);
	}
	
	//-----------------------------------------------------------------------

	cSoundEntity* cEngineFileLoading::LoadSound(cXmlElement* apElement, const tString& asNamePrefix, cWorld *apWorld)
	{
		kBeginWorldEntityLoad();
		
		tString sSoundFile = apElement->GetAttributeString("SoundEntityFile");
		bool bUseDefault = apElement->GetAttributeBool("UseDefault");

		cSoundEntity *pSound = apWorld->CreateSoundEntity(asNamePrefix+sName,sSoundFile,false);
		if(pSound==NULL) return NULL;

		if(bUseDefault==false)
		{
			pSound->SetMinDistance(apElement->GetAttributeFloat("MinDistance"));
			pSound->SetMaxDistance(apElement->GetAttributeFloat("MaxDistance"));
			pSound->SetVolume(apElement->GetAttributeFloat("Volume"));
		}


		kEndWorldEntityLoad(pSound);
	}

	
	//-----------------------------------------------------------------------

	static eBillboardType ToBillboardType(const tString& asType)
	{
		if(asType == "Axis") return eBillboardType_Axis;
		if(asType == "Point") return eBillboardType_Point;
		if(asType == "FixedAxis") return eBillboardType_FixedAxis;

		return eBillboardType_Point;
	}

	cBillboard* cEngineFileLoading::LoadBillboard(cXmlElement* apElement, const tString& asNamePrefix, cWorld *apWorld, cResources *apResources, bool abStatic,
													tEFL_LightBillboardConnectionList *apLightBillboardList)
	{
		kBeginWorldEntityLoad();
		
		cVector2f vSize = apElement->GetAttributeVector2f("BillboardSize");
		tString sMat = apElement->GetAttributeString("MaterialFile");
		eBillboardType bbType = ToBillboardType(apElement->GetAttributeString("BillboardType"));

		cBillboard *pBillboard = apWorld->CreateBillboard(asNamePrefix+sName,vSize,bbType,sMat, abStatic);
		if(pBillboard==NULL) return NULL;

		pBillboard->SetForwardOffset(apElement->GetAttributeFloat("BillboardOffset"));
		pBillboard->SetColor(apElement->GetAttributeColor("BillboardColor",cColor(1,1)));

		pBillboard->SetIsHalo(apElement->GetAttributeBool("IsHalo",false));
		pBillboard->SetHaloSourceSize(apElement->GetAttributeVector3f("HaloSourceSize",1));

		tString sConnectLight = apElement->GetAttributeString("ConnectLight");
		if(apLightBillboardList && sConnectLight!="")
		{
			cEFL_LightBillboardConnection lightBBConnection;
			
			lightBBConnection.msBillboardID = apElement->GetAttributeInt("ID");
			lightBBConnection.msLightName = asNamePrefix+apElement->GetAttributeString("ConnectLight");

			apLightBillboardList->push_back(lightBBConnection);
		}
		
		kEndWorldEntityLoad(pBillboard);
	}

	//-----------------------------------------------------------------------
	
	static eShadowMapResolution ToShadowMapResolution(const tString& asType)
	{
		tString sLowType = cString::ToLowerCase(asType);

        if(sLowType == "high") return eShadowMapResolution_High;
		if(sLowType == "medium") return eShadowMapResolution_Medium;
		if(sLowType == "low") return eShadowMapResolution_Low;
		return eShadowMapResolution_High;
	}

	static eTextureAnimMode ToTextureAnimMode(const tString& asType)
	{
		if(cString::ToLowerCase(asType) == "none") return eTextureAnimMode_None;
		else if(cString::ToLowerCase(asType) == "loop") return eTextureAnimMode_Loop;
		else if(cString::ToLowerCase(asType) == "oscillate") return eTextureAnimMode_Oscillate;

		return eTextureAnimMode_None;
	}
	
	iLight* cEngineFileLoading::LoadLight(	cXmlElement* apElement, const tString& asNamePrefix, cWorld *apWorld, cResources *apResources, bool abStatic)
	{
		kBeginWorldEntityLoad();
		
		iLight *pLight = NULL;

		bool bStatic = abStatic;

		//////////////////////////
		// Box Light
		if(apElement->GetValue() == "BoxLight")
		{
			cLightBox *pLightBox = apWorld->CreateLightBox(asNamePrefix+sName, bStatic);
			pLight = pLightBox;

			pLightBox->SetSize(apElement->GetAttributeVector3f("Size", 1));
			pLightBox->SetBlendFunc((eLightBoxBlendFunc) apElement->GetAttributeInt("BlendFunc", 1));
		}
		//////////////////////////
		// Spotlightt
		else if(apElement->GetValue() == "SpotLight")
		{
			cLightSpot *pLightSpot = apWorld->CreateLightSpot(asNamePrefix+sName,"", bStatic); 
			pLight = pLightSpot;

			//Frustum related
			pLightSpot->SetFOV(apElement->GetAttributeFloat("FOV", 1.0f));
			pLightSpot->SetAspect(apElement->GetAttributeFloat("Aspect", 1.0f));
			pLightSpot->SetNearClipPlane(apElement->GetAttributeFloat("NearClipPlane", 0.1f));

			//Spot fall off
			tString sSpotFalloffMap = apElement->GetAttributeString("SpotFalloffMap");
			if(sSpotFalloffMap != "")
			{
				iTexture *pFalloff = apResources->GetTextureManager()->Create1D(sSpotFalloffMap,true);
				if(pFalloff) pLightSpot->SetSpotFalloffMap(pFalloff);
			}
		}
		//////////////////////////
		// Point Light
		else if(apElement->GetValue() == "PointLight")
		{
			cLightPoint *pLightPoint  = apWorld->CreateLightPoint(asNamePrefix+sName,"", bStatic); 
			pLight = pLightPoint;
		}
		else
		{
			Error("Unknown light type '%s'\n", apElement->GetValue().c_str());
			return NULL;
		}

		//////////////////////////
		// General properties
		eLightType lightType = pLight->GetLightType();

		//Spot and point
		if(lightType == eLightType_Point || lightType == eLightType_Spot)
		{
			//Falloff
			tString sFalloffMap = apElement->GetAttributeString("FalloffMap");
			if(sFalloffMap != "")
			{
				iTexture *pFalloff = apResources->GetTextureManager()->Create1D(sFalloffMap,true);
				if(pFalloff) pLight->SetFalloffMap(pFalloff);
			}

			//Gobo
			tString sGobo = apElement->GetAttributeString("Gobo","");
			if(sGobo  != "")
			{
				eTextureAnimMode animMode = ToTextureAnimMode(apElement->GetAttributeString("GoboAnimMode",""));
				float fAnimFrameTime = apElement->GetAttributeFloat("GoboAnimFrameTime", 1);

				iTexture *pGoboTex=NULL;
				if(lightType  == eLightType_Spot)
				{
					if(animMode == eTextureAnimMode_None)
						pGoboTex = apResources->GetTextureManager()->Create2D(sGobo,true);
					else
						pGoboTex = apResources->GetTextureManager()->CreateAnim(sGobo, true, eTextureType_2D);
				}
				else
				{
					if(animMode == eTextureAnimMode_None)
						pGoboTex = apResources->GetTextureManager()->CreateCubeMap(sGobo,true);
					else
						pGoboTex = apResources->GetTextureManager()->CreateAnim(sGobo,true, eTextureType_CubeMap);
				}
	
				if(pGoboTex)
				{
					pLight->SetGoboTexture(pGoboTex);
					pGoboTex->SetFrameTime(fAnimFrameTime);
				}
			}
		}

		//All types
		pLight->SetCastShadows(apElement->GetAttributeBool("CastShadows", false));
		pLight->SetDiffuseColor(apElement->GetAttributeColor("DiffuseColor", cColor(1)));
		pLight->SetDefaultDiffuseColor(pLight->GetDiffuseColor());
		pLight->SetRadius(apElement->GetAttributeFloat("Radius", 1));

		pLight->SetShadowMapResolution( ToShadowMapResolution(apElement->GetAttributeString("ShadowResolution", "High")) );
		
		bool bShadowsAffectDynamic = apElement->GetAttributeBool("ShadowsAffectDynamic", true);
		bool bShadowsAffectStatic = apElement->GetAttributeBool("ShadowsAffectStatic", true);
		tObjectVariabilityFlag lFlags =0;
		if(bShadowsAffectDynamic)	lFlags |= eObjectVariabilityFlag_Dynamic;
		if(bShadowsAffectStatic)	lFlags |= eObjectVariabilityFlag_Static;
		pLight->SetShadowCastersAffected(lFlags);

		//////////////////////
		// Backwards compitabilty:
		float fDefaultFadeOn = apElement->GetAttributeFloat("FlickerOnFadeLength",0);
		float fDefaultFadeOff = apElement->GetAttributeFloat("FlickerOffFadeLength",0);

		pLight->SetFlickerActive(apElement->GetAttributeBool("FlickerActive", false));
		pLight->SetFlicker(
			apElement->GetAttributeColor("FlickerOffColor"),
			apElement->GetAttributeFloat("FlickerOffRadius"),

			apElement->GetAttributeFloat("FlickerOnMinLength"),
			apElement->GetAttributeFloat("FlickerOnMaxLength"),
			apElement->GetAttributeString("FlickerOnSound"),
			apElement->GetAttributeString("FlickerOnPS"),

			apElement->GetAttributeFloat("FlickerOffMaxLength"),
			apElement->GetAttributeFloat("FlickerOffMinLength"),
			apElement->GetAttributeString("FlickerOffSound"),
			apElement->GetAttributeString("FlickerOffPS"),

			apElement->GetAttributeBool("FlickerFade"),
			apElement->GetAttributeFloat("FlickerOnFadeMinLength", fDefaultFadeOn),
			apElement->GetAttributeFloat("FlickerOnFadeMaxLength", fDefaultFadeOn),
			
			apElement->GetAttributeFloat("FlickerOffFadeMinLength", fDefaultFadeOff),
			apElement->GetAttributeFloat("FlickerOffFadeMaxLength", fDefaultFadeOff)
			);
 

		kEndWorldEntityLoad(pLight);
	}
	

	//-----------------------------------------------------------------------

	int glDecalNumOfElements[4] = {4,3,3,4};
	eVertexBufferElement glDecalElementType[4] = {	eVertexBufferElement_Position, 
													eVertexBufferElement_Normal,
													eVertexBufferElement_Texture0,
													eVertexBufferElement_Texture1Tangent};
	cMesh* cEngineFileLoading::LoadDecalMeshHelper(cXmlElement* apElement, cGraphics* apGraphics, cResources* apResources, const tString& asName, const tString& asMaterial, const cColor& aColor)
	{
		////////////////////////////////
		//Load Vertex data
		if(apElement==NULL)return NULL;

		int lNumOfVtx = apElement->GetAttributeInt("NumVerts", 0);
		int lNumOfIdx = apElement->GetAttributeInt("NumInds", 0);

		if(lNumOfIdx <=0 || lNumOfVtx<=0)
		{
			Warning("Decal %s is missing geometry, skipping!\n", asName.c_str());
			return NULL;
		}
			
		cXmlElement *pDataArrayElem[4];
		pDataArrayElem[0] = apElement->GetFirstElement("Positions");
		pDataArrayElem[1] = apElement->GetFirstElement("Normals");
		pDataArrayElem[2] = apElement->GetFirstElement("TexCoords");
		pDataArrayElem[3] = apElement->GetFirstElement("Tangents");
		cXmlElement *pIndicesElem = apElement->GetFirstElement("Indices");

		tFloatVec vDataArrays[4];
		tIntVec vIdxArray;
		tString sSepp=" ";
		for(int i=0; i<4; ++i)
		{
			vDataArrays->reserve(lNumOfVtx * glDecalNumOfElements[i]);
			cString::GetFloatVec(pDataArrayElem[i]->GetAttributeString("Array"), vDataArrays[i],&sSepp);
		}
		vIdxArray.reserve(lNumOfIdx);
		cString::GetIntVec(pIndicesElem->GetAttributeString("Array"), vIdxArray,&sSepp);
	
		//////////////////////////////////
		// Create vertex buffer
		iVertexBuffer *pVtxBuffer = apGraphics->GetLowLevel()->CreateVertexBuffer(eVertexBufferType_Software, eVertexBufferDrawType_Tri, 
																					eVertexBufferUsageType_Static,lNumOfVtx, lNumOfIdx);

		//Create arrays	
		for(int i=0; i<4; ++i)
			pVtxBuffer->CreateElementArray(glDecalElementType[i],eVertexBufferElementFormat_Float, glDecalNumOfElements[i]);
		pVtxBuffer->CreateElementArray(eVertexBufferElement_Color0,eVertexBufferElementFormat_Float,4);
		
		//Copy the data!
		// TODO: This needs to be made faster so that data is loaded directly into mesh!
		for(int vtx=0; vtx<lNumOfVtx; ++vtx)
		{
			for(int i=0; i<4; ++i)
			{
				float *pData = &vDataArrays[i][vtx*glDecalNumOfElements[i]];

				if(glDecalNumOfElements[i]==2)
					pVtxBuffer->AddVertexVec3f(glDecalElementType[i], cVector3f(pData[0],pData[1],0) );
				else if(glDecalNumOfElements[i]==3)
					pVtxBuffer->AddVertexVec3f(glDecalElementType[i], cVector3f(pData[0],pData[1],pData[2]) );
				else if(glDecalNumOfElements[i]==4)
					pVtxBuffer->AddVertexVec4f(glDecalElementType[i], cVector3f(pData[0],pData[1],pData[2]),pData[3]);
			}

			pVtxBuffer->AddVertexColor(eVertexBufferElement_Color0, aColor);
		}

		for(int i=0; i<lNumOfIdx; ++i)
			pVtxBuffer->AddIndex(vIdxArray[i]);

		//Compile
		pVtxBuffer->Compile(0);
		
		/////////////////////////
		// Create the mesh
		cMesh *pMesh = hplNew( cMesh, (asName, _W(""), apResources->GetMaterialManager(), apResources->GetAnimationManager()) );

		cSubMesh *pSubMesh = pMesh->CreateSubMesh("Main");
		
		cMaterial *pMat = apResources->GetMaterialManager()->CreateMaterial(asMaterial);
		pSubMesh->SetMaterial(pMat);
		pSubMesh->SetVertexBuffer(pVtxBuffer);
		pSubMesh->SetMaterialName(asMaterial);


		return pMesh;
	}

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cEngineFileLoading::SetupWorldEntity(iEntity3D *apEntity, cXmlElement* apElement)
	{
		if(apEntity==NULL) return;

		int lID = apElement->GetAttributeInt("ID");
		cVector3f vPosition = apElement->GetAttributeVector3f("WorldPos",0);
		cVector3f vScale = apElement->GetAttributeVector3f("Scale",1);
		cVector3f vRotation = apElement->GetAttributeVector3f("Rotation",0);

		cMatrixf mtxTransform = cMath::MatrixMul(cMath::MatrixRotate(vRotation, eEulerRotationOrder_XYZ),cMath::MatrixScale(vScale));
		mtxTransform.SetTranslation(vPosition);

		apEntity->SetMatrix(mtxTransform);
		apEntity->SetUniqueID(lID);
	}

    //-----------------------------------------------------------------------
}
