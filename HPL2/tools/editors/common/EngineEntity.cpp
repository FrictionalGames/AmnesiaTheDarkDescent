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

#include "EngineEntity.h"

#include "EntityIcon.h"
#include "EntityWrapper.h"
#include "EditorWorld.h"

#include "EditorWindowViewport.h"
#include "EditorHelper.h"

//-----------------------------------------------------------------------

void iEngineEntity::SetMatrix(const cMatrixf& amtxX)
{
	if(mpEntity)
		mpEntity->SetWorldMatrix(amtxX);
}

const cMatrixf& iEngineEntity::GetWorldMatrix()
{
	return mpEntity->GetWorldMatrix();
}

//-----------------------------------------------------------------------

bool iEngineEntity::IsCulledByFrustum(cCamera* apCamera)
{
	cBoundingVolume* pBV = GetRenderBV();
	cFrustum* pFrustum = apCamera->GetFrustum();
	if(pBV)
		return (pFrustum->CollideBoundingVolume(pBV)!=eCollision_Outside)==false;
	
	return pFrustum->CollidePoint(mpParent->GetPosition())==false;
}

//-----------------------------------------------------------------------

bool iEngineEntity::IsInsideBoundingVolume(cBoundingVolume* apBV, cEditorWindowViewport* apViewport)
{
	cBoundingVolume* pBV = GetPickBV(apViewport);

	return cMath::CheckAABBIntersection(apBV->GetMin(), apBV->GetMax(), pBV->GetMin(), pBV->GetMax());
}

//-----------------------------------------------------------------------

bool iEngineEntity::CheckRayBoundingVolumeIntersect(cEditorWindowViewport* apViewport, cVector3f* apPos, float* apT)
{
	cBoundingVolume* pBV = GetPickBV(apViewport);

	return cMath::CheckAABBLineIntersection(pBV->GetMin(), pBV->GetMax(), apViewport->GetUnprojectedStart(), apViewport->GetUnprojectedEnd(), apPos, apT);
}

//-----------------------------------------------------------------------

bool iEngineEntity::Check2DBoxIntersect(cEditorWindowViewport* apViewport, const cRect2l& aBox)
{
	cRect2l rect;
	cFrustum* pFrustum = apViewport->GetCamera()->GetFrustum();
	cBoundingVolume* pBV = GetPickBV(apViewport);
	cMath::GetClipRectFromBV(rect,*pBV,pFrustum,apViewport->GetGuiViewportSizeInt(), pFrustum->GetFOV()*0.5f);

	return cMath::CheckRectIntersection(aBox, rect);
}

bool iEngineEntity::CheckRayIntersect(cEditorWindowViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT)
{
	return IsInsideBoundingVolume(apViewport->GetRayBV(), apViewport) &&
			CheckRayBoundingVolumeIntersect(apViewport, apPos, apT);
}


cBoundingVolume* iEngineEntity::GetRenderBV()
{
	if(mpEntity)
		return mpEntity->GetBoundingVolume(); 
	
	return NULL;
}
//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// MESH ENTITY - CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

float iEngineEntityMesh::mfDisabledCoverage = 0.5f;

//-----------------------------------------------------------------------

iEngineEntityMesh::iEngineEntityMesh(iEntityWrapper* apParent) : iEngineEntity(apParent), mpMesh(NULL)
{
}

iEngineEntityMesh::~iEngineEntityMesh()
{
	if(mpEntity)
	{
		cWorld* pWorld = mpParent->GetEditorWorld()->GetWorld();
		pWorld->DestroyMeshEntity((cMeshEntity*)mpEntity);
	}	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// MESH ENTITY - PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iEngineEntityMesh::SetUpMesh()
{
	cMeshEntity* pMeshEntity = GetMeshEntity();
	if(pMeshEntity==NULL){
		Error("Engine entity had a NULL MeshEntity during SetUpMesh\n");
		return;
	}

	for(int i=0;i<pMeshEntity->GetSubMeshEntityNum();++i)
	{
		cSubMeshEntity* pSubMeshEntity = pMeshEntity->GetSubMeshEntity(i);
        pSubMeshEntity->SetUserData(mpParent);
	}
}

bool iEngineEntityMesh::Create(const tString& asName)
{
	if(mpMesh==NULL) return false;

	cWorld* pWorld = mpParent->GetEditorWorld()->GetWorld();
	mpEntity = pWorld->CreateMeshEntity(asName, mpMesh);
	SetUpMesh();

	return IsCreated();
}

//-----------------------------------------------------------------------

bool iEngineEntityMesh::CheckRayIntersect(cEditorWindowViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT)
{
	if(iEngineEntity::CheckRayIntersect(apViewport, apPos, apTriangle, apT)==false)
		return false;

	return cEditorHelper::CheckRayMeshEntityIntersect(apViewport->GetUnprojectedStart(), apViewport->GetUnprojectedEnd(), 
														(cMeshEntity*)mpEntity, apPos, apTriangle, apT);
}

//-----------------------------------------------------------------------

void iEngineEntityMesh::Update()
{
	float fCoverage = mpParent->IsActive()? 1.0f : mfDisabledCoverage ;
	SetCoverage(fCoverage);
}

//-----------------------------------------------------------------------

void iEngineEntityMesh::UpdateVisibility()
{
	Update();
	((cMeshEntity*)mpEntity)->SetVisible(mpParent->IsVisible() && mpParent->IsCulledByClipPlanes()==false);
}

//-----------------------------------------------------------------------

void iEngineEntityMesh::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, bool abIsSelected,	bool abIsActive, const cColor& aHighlightCol)
{
	if(abIsSelected==false)
		return;

	apFunctions->SetBlendMode(eMaterialBlendMode_Alpha);
	apFunctions->SetDepthTest(true);
	apFunctions->SetDepthWrite(false);

	cMeshEntity* pMeshEntity = GetMeshEntity();
	for(int i=0;i<pMeshEntity->GetSubMeshEntityNum();++i)
	{
		cSubMeshEntity* pSubMeshEntity = pMeshEntity->GetSubMeshEntity(i);
		apFunctions->SetMatrix(pSubMeshEntity->GetModelMatrix(NULL));
		apFunctions->DrawWireFrame(pSubMeshEntity->GetVertexBuffer(), aHighlightCol);
	}
	apFunctions->SetMatrix(NULL);
	apFunctions->SetBlendMode(eMaterialBlendMode_None);
	apFunctions->SetDepthTest(false);
}

//-----------------------------------------------------------------------

void iEngineEntityMesh::DrawProgram(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iGpuProgram* apProg, const cColor& aCol)
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

void iEngineEntityMesh::SetCastShadows(bool abX)
{
	((cMeshEntity*)mpEntity)->SetRenderFlagBit(eRenderableFlag_ShadowCaster,abX);
}

//-----------------------------------------------------------------------

void iEngineEntityMesh::SetCoverage(float afX)
{
	((cMeshEntity*)mpEntity)->SetCoverageAmount(afX);
}

//-----------------------------------------------------------------------

bool iEngineEntityMesh::SetMaterial(const tString& asX)
{
	cMaterial* pMat = NULL;
	if(cEditorHelper::LoadResourceFile(eEditorResourceType_Material, asX,(void**) &pMat))
	{
		cMesh* pMesh = ((cMeshEntity*)mpEntity)->GetMesh();
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

	return false;
}

//-----------------------------------------------------------------------

bool iEngineEntityMesh::SetCustomMaterial(const tString& asX, bool abDelete)
{
	cMaterial* pMat = NULL;
	if(cEditorHelper::LoadResourceFile(eEditorResourceType_Material, asX,(void**) &pMat))
	{
		cMeshEntity* pMeshEnt = ((cMeshEntity*)mpEntity);
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

bool iEngineEntityMesh::SetCustomMaterial(cMaterial* apMat, bool abDelete)
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

cEngineEntityLoadedMesh::cEngineEntityLoadedMesh(iEntityWrapper* apParent, const tString& asFilename) : iEngineEntityMesh(apParent)
{
	iEditorWorld* pWorld = mpParent->GetEditorWorld();

	cMesh *pMesh = pWorld->GetEditor()->GetEngine()->GetResources()->GetMeshManager()->CreateMesh(asFilename);
	SetMesh(pMesh);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// LOADED MESH AGGREGATE ENTITY - CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cEngineEntityLoadedMeshAggregate::cEngineEntityLoadedMeshAggregate(iEntityWrapper* apParent, const tString& asFilename) : iEngineEntityMesh(apParent)
{
	msFilename = asFilename;
	mbLightsActive = true;
	mbParticleSystemsActive = true;
}

cEngineEntityLoadedMeshAggregate::~cEngineEntityLoadedMeshAggregate()
{
	cWorld* pWorld = mpParent->GetEditorWorld()->GetWorld();
	for(int i=0;i<(int)mvLights.size();++i)
		pWorld->DestroyLight(mvLights[i]);
	for(int i=0;i<(int)mvBillboards.size();++i)
		pWorld->DestroyBillboard(mvBillboards[i]);
	for(int i=0;i<(int)mvParticleSystems.size();++i)
		pWorld->DestroyParticleSystem(mvParticleSystems[i]);
	for(int i=0;i<(int)mvSounds.size();++i)
		pWorld->DestroySoundEntity(mvSounds[i]);	
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// MESH AGGREGATE ENTITY - PUBLIC METHODS
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cEngineEntityLoadedMeshAggregate::Create(const tString& asName)
{
	iEditorWorld* pWorld = mpParent->GetEditorWorld();
	cEditorEntityLoader* pLoader = pWorld->GetEditor()->GetEngineEntityLoader();

	mpEntity = pLoader->LoadEntFile(mpParent->GetID(), asName, msFilename, pWorld->GetWorld(), false, true, true, false, true);
	mvLights = pLoader->GetLights();
	mvBillboards = pLoader->GetBillboards();
	mvParticleSystems = pLoader->GetParticleSystems();
	mvSounds = pLoader->GetSounds();

	for(int i=0;i<(int)mvLights.size();++i)
		mpEntity->AddChild(mvLights[i]);
	for(int i=0;i<(int)mvBillboards.size();++i)
		mpEntity->AddChild(mvBillboards[i]);
	for(int i=0;i<(int)mvParticleSystems.size();++i)
		mpEntity->AddChild(mvParticleSystems[i]);
	for(int i=0;i<(int)mvSounds.size();++i)
		mpEntity->AddChild(mvSounds[i]);

	SetUpMesh();

	return IsCreated();
}

//-----------------------------------------------------------------------

void cEngineEntityLoadedMeshAggregate::Update()
{
	iEngineEntityMesh::Update();

	bool bActive = mpParent->IsActive();
	bool bVisible = mpParent->IsVisible();
	iEditorWorld* pWorld = mpParent->GetEditorWorld();
	bool bLightsVisible = pWorld->GetTypeVisibility(eEditorEntityType_Light);
	bool bPSVisible = pWorld->GetTypeVisibility(eEditorEntityType_ParticleSystem);

	for(int i=0;i<(int)mvLights.size();++i)
		mvLights[i]->SetVisible(mbLightsActive && bLightsVisible && bActive && bVisible);
	for(int i=0;i<(int)mvParticleSystems.size();++i)
		mvParticleSystems[i]->SetVisible(mbParticleSystemsActive && bPSVisible && bActive && bVisible);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// GENERATED MESH ENTITY - CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cEngineEntityGeneratedMesh::cEngineEntityGeneratedMesh(iEntityWrapper* apParent, cMesh* apMesh) : iEngineEntityMesh(apParent)
{
	SetMesh(apMesh);
}

bool cEngineEntityGeneratedMesh::ReCreate(cMesh* apMesh)
{
	cWorld* pWorld = mpParent->GetEditorWorld()->GetWorld();
	tString sName; 
	if(mpEntity)
	{
		sName = mpEntity->GetName();
		pWorld->DestroyMeshEntity((cMeshEntity*)mpEntity);
		mpEntity = NULL;
	}

	SetMesh(apMesh);

	return Create(sName);
}

iVertexBuffer* cEngineEntityGeneratedMesh::GetVertexBuffer()
{
	return mpMesh->GetSubMesh(0)->GetVertexBuffer();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// ICON ENTITY BASE - CONSTRUCTORS
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------


iIconEntity::iIconEntity(iEntityWrapper* apParent, const tString& asIconFile) : iEngineEntity(apParent)
{
	mpIcon = hplNew(cEntityIcon,(apParent, asIconFile));
}

iIconEntity::~iIconEntity()
{
	if(mpIcon)
		hplDelete(mpIcon);
}

bool iIconEntity::Create(const tString& asName)
{
	return true;
}

cBoundingVolume* iIconEntity::GetPickBV(cEditorWindowViewport* apViewport)
{
	if(mpIcon)
		return mpIcon->GetPickBV(apViewport);
	
	return NULL;
}

bool iIconEntity::Check2DBoxIntersect(cEditorWindowViewport* apViewport, const cRect2l& aBox)
{
	return mpIcon && mpIcon->Check2DBoxIntersect(apViewport, aBox);
}

bool iIconEntity::CheckRayIntersect(cEditorWindowViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT)
{
	return mpIcon && mpIcon->CheckRayIntersect(apViewport, apPos, apTriangle, apT);
}

void iIconEntity::Draw(cEditorWindowViewport* apViewport, 
					  cRendererCallbackFunctions* apFunctions, 
					  bool abIsSelected,
					  bool abIsActive,
					  const cColor& aHighlightCol)
{
	if(mpIcon) mpIcon->DrawIcon(apViewport, apFunctions, NULL, abIsSelected, mpParent->GetPosition(), mpParent->IsActive());
}

