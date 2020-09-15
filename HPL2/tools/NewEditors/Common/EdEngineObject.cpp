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

#include "../Common/EdEngineObject.h"

#include "../Common/EdObjectIcon.h"
#include "../Common/EdScnObject.h"
#include "../Common/EdWorld.h"

#include "../Common/Editor.h"

#include "../Common/EdResource.h"
#include "../Common/EdHelper.h"
//#include "../Common/EdWindowViewport.h"
//#include "../Common/EditorHelper.h"

//-----------------------------------------------------------------------

void iEngineObject::SetMatrix(const cMatrixf& amtxX)
{
	if(mpObject)
	{
		mpObject->SetWorldMatrix(amtxX);
	}
}

const cMatrixf& iEngineObject::GetWorldMatrix()
{
	return mpObject->GetWorldMatrix();
}

//-----------------------------------------------------------------------

bool iEngineObject::IsCulledByFrustum(cCamera* apCamera)
{
	cBoundingVolume* pBV = GetRenderBV();
	cFrustum* pFrustum = apCamera->GetFrustum();
	if(pBV)
		return (pFrustum->CollideBoundingVolume(pBV)!=eCollision_Outside)==false;
	
	return pFrustum->CollidePoint(mpParent->GetTranslation())==false;
}

//-----------------------------------------------------------------------

bool iEngineObject::IsInsideBoundingVolume(cBoundingVolume* apBV, iEdViewport* apViewport)
{
	cBoundingVolume* pBV = GetPickBV(apViewport);

	return cMath::CheckAABBIntersection(apBV->GetMin(), apBV->GetMax(), pBV->GetMin(), pBV->GetMax());
}

//-----------------------------------------------------------------------

bool iEngineObject::CheckRayBoundingVolumeIntersect(iEdViewport* apViewport, cVector3f* apPos, float* apT)
{
	cBoundingVolume* pBV = GetPickBV(apViewport);

	return cMath::CheckAABBLineIntersection(pBV->GetMin(), pBV->GetMax(), 
											apViewport->GetUnprojectedStart(), apViewport->GetUnprojectedEnd(), 
											apPos, apT);
}

//-----------------------------------------------------------------------

bool iEngineObject::Check2DBoxIntersect(iEdViewport* apViewport, const cRect2l& aBox)
{
	cRect2l rect;
	cFrustum* pFrustum = apViewport->GetEngCamera()->GetFrustum();
	cBoundingVolume* pBV = GetPickBV(apViewport);
	cMath::GetClipRectFromBV(rect,*pBV,pFrustum,apViewport->GetGuiViewportSizeInt(), pFrustum->GetFOV()*0.5f);

	return cMath::CheckRectIntersection(aBox, rect);
}

bool iEngineObject::CheckRayIntersect(iEdViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT)
{
	return IsInsideBoundingVolume(apViewport->GetRayBV(), apViewport) &&
			CheckRayBoundingVolumeIntersect(apViewport, apPos, apT);
}


cBoundingVolume* iEngineObject::GetRenderBV()
{
	if(mpObject)
		return mpObject->GetBoundingVolume(); 
	
	return NULL;
}
//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// MESH ENTITY - CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

float iEngineMesh::mfDisabledCoverage = 0.5f;

//-----------------------------------------------------------------------

iEngineMesh::iEngineMesh(iEdScnObject* apParent) : iEngineObject(apParent), mpMesh(NULL)
{
}

iEngineMesh::~iEngineMesh()
{
	if(mpObject)
	{
		cWorld* pWorld = mpParent->GetWorld()->GetEngWorld();
		pWorld->DestroyMeshEntity((cMeshEntity*)mpObject);
	}	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// MESH ENTITY - PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iEngineMesh::SetUpMesh()
{
	cMeshEntity* pMeshEntity = GetMeshEntity();
	if(pMeshEntity==NULL){
		Error("Engine entity had a NULL MeshEntity during SetUpMesh\n");
		return;
	}

	for(int i=0;i<pMeshEntity->GetSubMeshEntityNum();++i)
	{
		cSubMeshEntity* pSubMeshEntity = pMeshEntity->GetSubMeshEntity(i);

        pSubMeshEntity->SetRenderableUserData(mpParent);
	}
}

bool iEngineMesh::Create(const tString& asName)
{
	if(mpMesh==NULL) return false;

	cWorld* pWorld = mpParent->GetWorld()->GetEngWorld();
	mpObject = pWorld->CreateMeshEntity(asName, mpMesh);
	SetUpMesh();

	return IsCreated();
}

//-----------------------------------------------------------------------

bool iEngineMesh::CheckRayIntersect(iEdViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT)
{
	if(iEngineObject::CheckRayIntersect(apViewport, apPos, apTriangle, apT)==false)
		return false;

	cMeshEntity* pObj = static_cast<cMeshEntity*>(mpObject);
	return cEdHelper::CheckRayMeshEntityIntersect(apViewport->GetUnprojectedStart(), apViewport->GetUnprojectedEnd(), 
													pObj, apPos, apTriangle, apT);
}

//-----------------------------------------------------------------------

void iEngineMesh::Update()
{
	float fCoverage = mpParent->IsEnabled()? 1.0f : mfDisabledCoverage ;
	SetCoverage(fCoverage);
}

//-----------------------------------------------------------------------

void iEngineMesh::UpdateVisibility()
{
	Update();
	if(mpObject)
	{
		static_cast<cMeshEntity*>(mpObject)->SetVisible(mpParent->IsVisible() && mpParent->IsCulledByClipPlanes()==false);
	}
}

//-----------------------------------------------------------------------

void iEngineMesh::Draw(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions, bool abIsSelected,	bool abIsActive, const cColor& aHighlightCol)
{
	if(abIsSelected==false)
		return;

	apViewport->DrawSolidColorMesh(GetMeshEntity(), aHighlightCol, apFunctions);
}

//-----------------------------------------------------------------------

void iEngineMesh::DrawProgram(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions, iGpuProgram* apProg, const cColor& aCol)
{
	apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);
	apFunctions->SetProgram(apProg);
	apFunctions->SetTextureRange(NULL, 0);

	cMeshEntity* pMeshEntity = GetMeshEntity();
	for(int i=0;i<pMeshEntity->GetSubMeshEntityNum();++i)
	{
		cSubMeshEntity* pSubMeshEntity = pMeshEntity->GetSubMeshEntity(i);
		apFunctions->SetMatrix(pSubMeshEntity->GetModelMatrix(NULL));
		apFunctions->SetVertexBuffer(pSubMeshEntity->GetVertexBuffer());
		apFunctions->GetLowLevelGfx()->SetColor(aCol);
		
		apFunctions->DrawCurrent();
	}
	apFunctions->SetMatrix(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_None);
	apFunctions->SetDepthTest(false);
}

//-----------------------------------------------------------------------

void iEngineMesh::SetCastShadows(bool abX)
{
	((cMeshEntity*)mpObject)->SetRenderFlagBit(eRenderableFlag_ShadowCaster,abX);
}

//-----------------------------------------------------------------------

void iEngineMesh::SetCoverage(float afX)
{
	if(mpObject) ((cMeshEntity*)mpObject)->SetCoverageAmount(afX);
}

//-----------------------------------------------------------------------

bool iEngineMesh::SetMaterial(const tString& asX)
{
	/*cMaterial* pMat = NULL;
	if(cEdHelper::LoadResourceFile(eEdResourceType_Material, asX,(void**) &pMat))
	{
		cMesh* pMesh = ((cMeshEntity*)mpObject)->GetMesh();
		if(pMesh)
		{
			for(int i=0;i<pMesh->GetSubMeshNum();++i)
			{
				cSubMesh* pSubMesh = pMesh->GetSubMesh(i);
				pSubMesh->SetMaterial(pMat);
			}
		}

		return true;
	}
	*/

	return false;
}

//-----------------------------------------------------------------------

bool iEngineMesh::SetCustomMaterial(const tString& asX, bool abDelete)
{
	cMaterial* pMat = NULL;
	if(cEdLoader::LoadMaterial(asX,&pMat))
	{
		cMeshEntity* pMeshEnt = static_cast<cMeshEntity*>(mpObject);
		if(pMeshEnt)
		{
			for(int i=0;i<pMeshEnt->GetSubMeshEntityNum();++i)
			{
				cSubMeshEntity* pSubMeshEnt = pMeshEnt->GetSubMeshEntity(i);
				pSubMeshEnt->SetCustomMaterial(pMat, abDelete);
			}
		}

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------

bool iEngineMesh::SetCustomMaterial(cMaterial* apMat, bool abDelete)
{
	cMeshEntity* pMeshEnt = GetMeshEntity();
	if(pMeshEnt==NULL)
		return false;

	{
		cSubMeshEntity* pSubMeshEntity = pMeshEnt->GetSubMeshEntity(0);
		if(pSubMeshEntity->GetCustomMaterial()==apMat)
			return false;
	}
	for(int i=0;i<pMeshEnt->GetSubMeshEntityNum();++i)
	{
		cSubMeshEntity* pSubMeshEnt = pMeshEnt->GetSubMeshEntity(i);
		pSubMeshEnt->SetCustomMaterial(apMat, abDelete);
	}
	return true;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// LOADED MESH ENTITY - CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cEngineLoadedMesh::cEngineLoadedMesh(iEdScnObject* apParent, const tString& asFilename) : iEngineMesh(apParent)
{
	iEdWorld* pWorld = mpParent->GetWorld();

	cMesh *pMesh = pWorld->GetEditor()->GetEngine()->GetResources()->GetMeshManager()->CreateMesh(asFilename);
	SetMesh(pMesh);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// GENERATED MESH ENTITY - CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cEngineGeneratedMesh::cEngineGeneratedMesh(iEdScnObject* apParent, cMesh* apMesh) : iEngineMesh(apParent)
{
	SetMesh(apMesh);
}

//-----------------------------------------------------------------------

bool cEngineGeneratedMesh::ReCreate(cMesh* apMesh)
{
	cWorld* pWorld = mpParent->GetType()->GetWorld()->GetEngWorld();
	tString sName;
	tMaterialVec vMaterials;
	if(mpObject)
	{
		cMeshEntity* pMesh = static_cast<cMeshEntity*>(mpObject);

		sName = mpObject->GetName();
		for(int i=0;i<pMesh->GetSubMeshEntityNum(); ++i)
		{
			cSubMeshEntity* pSubMesh = pMesh->GetSubMeshEntity(i);

			vMaterials.push_back(pSubMesh->GetCustomMaterial());
			pSubMesh->SetCustomMaterial(NULL, false);
		}

		pWorld->DestroyMeshEntity(static_cast<cMeshEntity*>(mpObject));
		mpObject = NULL;
	}

	SetMesh(apMesh);

	return Create(sName);
}

//-----------------------------------------------------------------------

iVertexBuffer* cEngineGeneratedMesh::GetVertexBuffer()
{
	if(mpMesh)
		return mpMesh->GetSubMesh(0)->GetVertexBuffer();

	return NULL;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// ICON ENTITY - CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


iIconObject::iIconObject(iEdScnObject* apParent, const tString& asIconFile) : iEngineObject(apParent)
{
	mpIcon = hplNew(cObjectIcon,(apParent, asIconFile));
}

iIconObject::~iIconObject()
{
	if(mpIcon)
		hplDelete(mpIcon);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// ICON ENTITY - PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool iIconObject::Create(const tString& asName)
{
	return true;
}

//-----------------------------------------------------------------------

cBoundingVolume* iIconObject::GetPickBV(iEdViewport* apViewport)
{
	if(mpIcon)
		return mpIcon->GetPickBV(apViewport);
	
	return NULL;
}

//-----------------------------------------------------------------------

bool iIconObject::Check2DBoxIntersect(iEdViewport* apViewport, const cRect2l& aBox)
{
	return mpIcon && mpIcon->Check2DBoxIntersect(apViewport, aBox);
}

//-----------------------------------------------------------------------

bool iIconObject::CheckRayIntersect(iEdViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT)
{
	return mpIcon && mpIcon->CheckRayIntersect(apViewport, apPos, apTriangle, apT);
}

//-----------------------------------------------------------------------

void iIconObject::Draw(iEdViewport* apViewport, 
					  cRendererCallbackFunctions* apFunctions, 
					  bool abIsSelected,
					  bool abIsActive,
					  const cColor& aHighlightCol)
{
	if(mpIcon) 
		mpIcon->DrawIcon(apViewport, apFunctions, abIsSelected, mpParent->IsEnabled(), aHighlightCol);
}

//-----------------------------------------------------------------------

