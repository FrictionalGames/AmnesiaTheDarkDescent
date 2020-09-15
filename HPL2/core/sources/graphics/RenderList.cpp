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

#include "graphics/RenderList.h"

#include "graphics/Renderable.h"
#include "graphics/Material.h"
#include "graphics/MaterialType.h"
#include "graphics/Renderer.h"
#include "graphics/SubMesh.h"

#include "scene/MeshEntity.h"
#include "scene/SubMeshEntity.h"
#include "scene/Light.h"
#include "scene/FogArea.h"

#include "math/Math.h"
#include "math/Frustum.h"

#include <algorithm>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cRenderList::cRenderList()
	{
		mfFrameTime =0;
		mpFrustum = NULL;
	}

	//-----------------------------------------------------------------------

	cRenderList::~cRenderList()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	void cRenderList::Setup(float afFrameTime, cFrustum *apFrustum)
	{
		mfFrameTime = afFrameTime;
		mpFrustum = apFrustum;
	}

	//-----------------------------------------------------------------------

	void cRenderList::AddObject(iRenderable *apObject)
	{
		eRenderableType renderType =apObject->GetRenderType();

		////////////////////////////////////////
		//Update material, if not already done this frame
		cMaterial *pMaterial = apObject->GetMaterial();
		iMaterialType *pMaterialType = pMaterial ? pMaterial->GetType() : NULL;
		if(pMaterial && pMaterial->GetRenderFrameCount() != iRenderer::GetRenderFrameCount())
		{
			pMaterial->SetRenderFrameCount(iRenderer::GetRenderFrameCount());
			pMaterial->UpdateBeforeRendering(mfFrameTime);
		}

		////////////////////////////////////////
		// Update per frame things, if not done yet.
		if(apObject->GetRenderFrameCount() != iRenderer::GetRenderFrameCount())
		{
			apObject->SetRenderFrameCount(iRenderer::GetRenderFrameCount());
			apObject->UpdateGraphicsForFrame(mfFrameTime);
		}

		////////////////////////////////////////
		//Update per viewport specific and set amtrix point
		//Skip this for non-decal translucent! This is because the water rendering might mess it up otherwise!
		if(pMaterialType==NULL || pMaterialType->IsTranslucent()==false || pMaterialType->IsDecal() )
		{
			//skip rendering if the update return false
			if(apObject->UpdateGraphicsForViewport(mpFrustum,mfFrameTime)==false)
			{
				return;
			}

			apObject->SetModelMatrixPtr(apObject->GetModelMatrix(mpFrustum));
		}
		//Only set a matrix used for sorting. Calculate the proper in the trans rendering!
		else
		{
			apObject->SetModelMatrixPtr(apObject->GetModelMatrix(NULL));
		}

		////////////////////////////////////////
        //Calculate the View Z value
		// For transparent and non decals!
		if(pMaterialType && pMaterialType->IsTranslucent() && pMaterialType->IsDecal()==false)
		{
			cVector3f vIntersectionPos;
			cBoundingVolume *pBV = apObject->GetBoundingVolume();

			//If there is an intersection (which happens unless inside), use that. Else use world center
			if(cMath::CheckAABBLineIntersection(pBV->GetMin(), pBV->GetMax(), mpFrustum->GetOrigin(), pBV->GetWorldCenter(), &vIntersectionPos, NULL)==false)
			{
				vIntersectionPos = pBV->GetWorldCenter();		
			}
			
			vIntersectionPos = cMath::MatrixMul(mpFrustum->GetViewMatrix(), vIntersectionPos);
			apObject->SetViewSpaceZ(vIntersectionPos.z);			
		}
		else
		{
			cVector3f vCameraPos = cMath::MatrixMul(mpFrustum->GetViewMatrix(), apObject->GetBoundingVolume()->GetWorldCenter());
			apObject->SetViewSpaceZ(vCameraPos.z);
		}
		
		

		//////////////////////////////
		// If objects uses occlusion queries, add it as such
		if(apObject->UsesOcclusionQuery())
		{
			mvOcclusionQueryObjects.push_back(apObject);
		}
        
		//////////////////////////////
		// Light, add to special list
		if(renderType == eRenderableType_Light)
		{
			mvLights.push_back( static_cast<iLight*>(apObject) );
		}
		//////////////////////////////
		// Fog area, add to special list
		if(renderType == eRenderableType_FogArea)
		{
			mvFogAreas.push_back( static_cast<cFogArea*>(apObject) );
		}
		//////////////////////////////
		// GuiSet, add to special list
		else if(renderType == eRenderableType_GuiSet)
		{
			//TODO....
		}
		///////////////////////////
		// Normal addition
		else
		{
			if(pMaterial==NULL) return; //Skip if it has no material...

			////////////////////////
			// Transparent
			if(pMaterialType->IsTranslucent())
			{
				if(pMaterialType->IsDecal())
					mvDecalObjects.push_back(apObject);
				else
					mvTransObjects.push_back(apObject);
			}
			////////////////////////
			// Solid
			else
			{
				mvSolidObjects.push_back(apObject);
				if(pMaterial->GetTexture(eMaterialTexture_Illumination) && apObject->GetIlluminationAmount()>0)
				{
					mvIllumObjects.push_back(apObject);
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	void cRenderList::Compile(tRenderListCompileFlag aFlags)
	{
		if(aFlags & eRenderListCompileFlag_Z) CompileArray(eRenderListType_Z);
		if(aFlags & eRenderListCompileFlag_Diffuse) CompileArray(eRenderListType_Diffuse);
		if(aFlags & eRenderListCompileFlag_Decal) CompileArray(eRenderListType_Decal);
		if(aFlags & eRenderListCompileFlag_Illumination) CompileArray(eRenderListType_Illumination);
		if(aFlags & eRenderListCompileFlag_Translucent)
		{
			FindNearestLargeSurfacePlane();
			CompileArray(eRenderListType_Translucent);
		}

	}

	//-----------------------------------------------------------------------

	void cRenderList::Clear()
	{
		// Use resize instead of clear, because that way capacity is preserved and allocation is never 
		// needed unless there is a need to increase the vector size.

		mvOcclusionQueryObjects.resize(0);
		mvTransObjects.resize(0);
		mvDecalObjects.resize(0);
		mvSolidObjects.resize(0);
		mvIllumObjects.resize(0);
		mvLights.resize(0);
		mvFogAreas.resize(0);

		for(int i=0; i<eRenderListType_LastEnum; ++i)
		{
			mvSortedArrays[i].resize(0);
		}
	}

	//-----------------------------------------------------------------------

	void cRenderList::PrintAllObjects()
	{
		Log("---------------------------------\n");
		Log("------ RENDER LIST CONTENTS -----\n");

		Log("Trans Objects:\n");
		for(size_t i=0; i<mvTransObjects.size(); ++i)
			Log(" '%s' ViewspaceZ: %f LargeSurfacePlacement: %d Mat: '%s' RenderCount: %d\n", mvTransObjects[i]->GetName().c_str(), 
					mvTransObjects[i]->GetViewSpaceZ(),
					mvTransObjects[i]->GetLargePlaneSurfacePlacement(),
					mvTransObjects[i]->GetMaterial()->GetName().c_str(), 
					mvTransObjects[i]->GetRenderFrameCount());

		Log("Solid Objects:\n");
		for(size_t i=0; i<mvSolidObjects.size(); ++i)
			Log(" '%s' Mat: '%s' RenderCount: %d\n", mvSolidObjects[i]->GetName().c_str(), mvSolidObjects[i]->GetMaterial()->GetName().c_str(), mvSolidObjects[i]->GetRenderFrameCount());
		
		Log("Decal Objects:\n");
		for(size_t i=0; i<mvDecalObjects.size(); ++i)
			Log(" '%s' Mat: '%s' RenderCount: %d\n", mvDecalObjects[i]->GetName().c_str(), mvDecalObjects[i]->GetMaterial()->GetName().c_str(), mvSolidObjects[i]->GetRenderFrameCount());
		
		Log("Illum Objects:\n");
		for(size_t i=0; i<mvIllumObjects.size(); ++i)
			Log(" '%s' Mat: '%s' RenderCount: %d\n", mvIllumObjects[i]->GetName().c_str(), mvIllumObjects[i]->GetMaterial()->GetName().c_str(), mvSolidObjects[i]->GetRenderFrameCount());
	

		Log("---------------------------------\n");
	}

	//-----------------------------------------------------------------------

	bool cRenderList::ArrayHasObjects(eRenderListType aType)
	{
		return mvSortedArrays[aType].empty()==false;
	}

	//-----------------------------------------------------------------------


	cRenderableVecIterator cRenderList::GetArrayIterator(eRenderListType aType)
	{
		return cRenderableVecIterator(&mvSortedArrays[aType]);
	}

	//-----------------------------------------------------------------------

	cRenderableVecIterator cRenderList::GetOcclusionQueryObjectIterator()
	{
		return cRenderableVecIterator(&mvOcclusionQueryObjects);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	static bool SortFunc_Z(iRenderable* apObjectA, iRenderable *apObjectB)
	{
		cMaterial *pMatA = apObjectA->GetMaterial();
		cMaterial *pMatB = apObjectB->GetMaterial();

		//////////////////////////
		//Alpha mode
		if(pMatA->GetAlphaMode() != pMatB->GetAlphaMode())
		{
			return pMatA->GetAlphaMode() < pMatB->GetAlphaMode();
		}

		//////////////////////////
		//If alpha, sort by texture (we know alpha is same for both materials, so can just test one)
		if(	pMatA->GetAlphaMode() == eMaterialAlphaMode_Trans )
		{
			if(pMatA->GetProgram(0,eMaterialRenderMode_Z) != pMatB->GetProgram(0,eMaterialRenderMode_Z))
			{
				return pMatA->GetProgram(0,eMaterialRenderMode_Z) < pMatB->GetProgram(0,eMaterialRenderMode_Z);
			}

			if(pMatA->GetTexture(eMaterialTexture_Diffuse) != pMatB->GetTexture(eMaterialTexture_Diffuse))
			{
				return pMatA->GetTexture(eMaterialTexture_Diffuse) < pMatB->GetTexture(eMaterialTexture_Diffuse);
			}
		}

		//////////////////////////
		//View space depth, no need to test further since Z should almost never be the same for two objects.
		// use ">" since we want to render things closest to the screen first.
		return apObjectA->GetViewSpaceZ() > apObjectB->GetViewSpaceZ();
		//return apObjectA < apObjectB;
	}

	//-----------------------------------------------------------------------

	static bool SortFunc_Diffuse(iRenderable* apObjectA, iRenderable *apObjectB)
	{
		cMaterial *pMatA = apObjectA->GetMaterial();
		cMaterial *pMatB = apObjectB->GetMaterial();

		//////////////////////////
		//Program
		if(pMatA->GetProgram(0,eMaterialRenderMode_Diffuse) != pMatB->GetProgram(0,eMaterialRenderMode_Diffuse))
		{
			return pMatA->GetProgram(0,eMaterialRenderMode_Diffuse) < pMatB->GetProgram(0,eMaterialRenderMode_Diffuse);
		}

		//////////////////////////
		//Texture
		for(int i=0;i<kMaxTextureUnits; ++i)
		{
			iTexture *pTexA = pMatA->GetTextureInUnit(eMaterialRenderMode_Diffuse,i);
			iTexture *pTexB = pMatB->GetTextureInUnit(eMaterialRenderMode_Diffuse,i);
			if(pTexA != pTexB) return pTexA < pTexB;
		}

		//////////////////////////
		//Vertex buffer
        if(apObjectA->GetVertexBuffer() != apObjectB->GetVertexBuffer())
		{
			return apObjectA->GetVertexBuffer() < apObjectB->GetVertexBuffer();
		}

		//////////////////////////
		//Matrix
		if(apObjectA->GetModelMatrixPtr() != apObjectB->GetModelMatrixPtr())
		{
			return apObjectA->GetModelMatrixPtr() < apObjectB->GetModelMatrixPtr();
		}
		
		//////////////////////////
		//Object
        return apObjectA < apObjectB;
	}

	//-----------------------------------------------------------------------

	static bool SortFunc_Translucent(iRenderable* apObjectA, iRenderable *apObjectB)
	{
		////////////////////////
		//If there is a large plane, then need to sort objects according to this first!
		if(apObjectA->GetLargePlaneSurfacePlacement() != apObjectB->GetLargePlaneSurfacePlacement())
		{
			return apObjectA->GetLargePlaneSurfacePlacement() < apObjectB->GetLargePlaneSurfacePlacement();
		}

		//////////////////////////
		//View space depth, no need to test further since Z should almost never be the same for two objects.
		return apObjectA->GetViewSpaceZ() < apObjectB->GetViewSpaceZ();		
	}
	
	//-----------------------------------------------------------------------

	static bool SortFunc_Decal(iRenderable* apObjectA, iRenderable *apObjectB)
	{
		cMaterial *pMatA = apObjectA->GetMaterial();
		cMaterial *pMatB = apObjectB->GetMaterial();
		
		//////////////////////////
		//Texture
		if(pMatA->GetTexture(eMaterialTexture_Illumination) != pMatB->GetTexture(eMaterialTexture_Illumination))
		{
			return pMatA->GetTexture(eMaterialTexture_Illumination) < pMatB->GetTexture(eMaterialTexture_Illumination);
		}

		//////////////////////////
		//Vertex buffer
		if(apObjectA->GetVertexBuffer() != apObjectB->GetVertexBuffer())
		{
			return apObjectA->GetVertexBuffer() < apObjectB->GetVertexBuffer();
		}

		//////////////////////////
		//Matrix
		if(apObjectA->GetModelMatrixPtr() != apObjectB->GetModelMatrixPtr())
		{
			return apObjectA->GetModelMatrixPtr() < apObjectB->GetModelMatrixPtr();
		}
		
		//////////////////////////
		//Vector (just so that order stays the same
        return apObjectA->GetWorldPosition()  < apObjectB->GetWorldPosition();
	}

	//-----------------------------------------------------------------------

	static bool SortFunc_Illumination(iRenderable* apObjectA, iRenderable *apObjectB)
	{
		cMaterial *pMatA = apObjectA->GetMaterial();
		cMaterial *pMatB = apObjectB->GetMaterial();

		//////////////////////////
		//Texture
		if(pMatA->GetTexture(eMaterialTexture_Illumination) != pMatB->GetTexture(eMaterialTexture_Illumination))
		{
			return pMatA->GetTexture(eMaterialTexture_Illumination) < pMatB->GetTexture(eMaterialTexture_Illumination);
		}

		//////////////////////////
		//Vertex buffer
		if(apObjectA->GetVertexBuffer() != apObjectB->GetVertexBuffer())
		{
			return apObjectA->GetVertexBuffer() < apObjectB->GetVertexBuffer();
		}

		//////////////////////////
		//Matrix
		if(apObjectA->GetModelMatrixPtr() != apObjectB->GetModelMatrixPtr())
		{
			return apObjectA->GetModelMatrixPtr() < apObjectB->GetModelMatrixPtr();
		}

		//////////////////////////
		//Illumination amount
		return apObjectA->GetIlluminationAmount() < apObjectB->GetIlluminationAmount();
	}

	//-----------------------------------------------------------------------

	typedef bool (*tSortRenderableFunc)(iRenderable*,iRenderable*); 

	static tSortRenderableFunc vSortFunctions[eRenderListType_LastEnum] = {SortFunc_Z,SortFunc_Diffuse,SortFunc_Translucent,SortFunc_Decal,SortFunc_Illumination};

	//-----------------------------------------------------------------------

	
	void cRenderList::CompileArray(eRenderListType aType)
	{
		tRenderableVec *pSourceVec = NULL;
		if(aType == eRenderListType_Translucent)		pSourceVec = &mvTransObjects;
		else if(aType == eRenderListType_Decal)			pSourceVec = &mvDecalObjects;
		else if(aType == eRenderListType_Illumination)	pSourceVec = &mvIllumObjects;
		else											pSourceVec = &mvSolidObjects;

		mvSortedArrays[aType] = *pSourceVec;	//Should be fastest way to copy right, or use memcopy?
		
		std::sort(mvSortedArrays[aType].begin(), mvSortedArrays[aType].end(), vSortFunctions[aType]);
	}

	//-----------------------------------------------------------------------

	void cRenderList::FindNearestLargeSurfacePlane()
	{
		////////////////////////////////////
		// Setup variables
		cPlanef nearestSurfacePlane;
		bool bHasLargeSurfacePlane = false;
		float fClosestDist =0;
		iRenderable *pLargeSurfaceObject = NULL;
		
		////////////////////////////////////
		// Find the neareest surface plane
		for(size_t i=0; i<mvTransObjects.size(); ++i)
		{
			/////////////////////////////////
			// Check so object is of right type
			iRenderable *pObject = mvTransObjects[i];
			if(pObject->GetRenderType() != eRenderableType_SubMesh) continue;

			cMaterial *pMat = pObject->GetMaterial();
            if(pMat->GetLargeTransperantSurface()==false) continue;
			
			/////////////////////////////////
			// Check so sub mesh is one sided
			cSubMeshEntity *pSubEnt = static_cast<cSubMeshEntity*>(pObject);
			cSubMesh *pSubMesh = pSubEnt->GetSubMesh();
			if(pSubMesh->GetIsOneSided()==false) continue;

			cVector3f vSurfaceNormal = cMath::Vector3Normalize(cMath::MatrixMul3x3(pSubEnt->GetWorldMatrix(), pSubMesh->GetOneSidedNormal()));
			cVector3f vSurfacePos = cMath::MatrixMul(pSubEnt->GetWorldMatrix(), pSubMesh->GetOneSidedPoint());

			/////////////////////////////////
			// Make sure it does not face away from frustum.
			float fDot = cMath::Vector3Dot(vSurfacePos - mpFrustum->GetOrigin(), vSurfaceNormal);
			if(fDot >= 0) continue;

			/////////////////////////////////
			// Create surface normal and check if nearest.
			cPlanef surfacePlane;
			surfacePlane.FromNormalPoint(vSurfaceNormal, vSurfacePos);
			
			float fDist = cMath::PlaneToPointDist(surfacePlane, mpFrustum->GetOrigin());
			if(fDist < fClosestDist || bHasLargeSurfacePlane==false)
			{
				bHasLargeSurfacePlane = true;
				fClosestDist = fDist;
				nearestSurfacePlane = surfacePlane;
				pLargeSurfaceObject = pObject;
			}
		}

		////////////////////////////////////
		// Check if objects are above or below surface plane
		if(bHasLargeSurfacePlane)
		{
			//Log(" LargePlaneObject: '%s' Mat:'%s'\n", pLargeSurfaceObject->GetName().c_str(),  pLargeSurfaceObject->GetMaterial()->GetName().c_str());
			for(size_t i=0; i<mvTransObjects.size(); ++i)
			{
				iRenderable *pObject = mvTransObjects[i];
				
				//If this is large plane, then set value to 0
				if(pObject == pLargeSurfaceObject)
				{
					pObject->SetLargePlaneSurfacePlacement(0);
					continue;
				}
				
				float fDist = cMath::PlaneToPointDist(nearestSurfacePlane, pObject->GetBoundingVolume()->GetWorldCenter());
				pObject->SetLargePlaneSurfacePlacement(fDist < 0 ? -1 : 1);

				//Log(" %d Object: '%s' %d  Mat:'%s'\n",i, pObject->GetName().c_str(), pObject->GetLargePlaneSurfacePlacement(),  pLargeSurfaceObject->GetMaterial()->GetName().c_str());
			}	
		}
		////////////////////////////////////
		// Set all to same value.
		else
		{
			for(size_t i=0; i<mvTransObjects.size(); ++i)
			{
				iRenderable *pObject = mvTransObjects[i];
				pObject->SetLargePlaneSurfacePlacement(0);
			}
		}

	}
	
	//-----------------------------------------------------------------------

}
