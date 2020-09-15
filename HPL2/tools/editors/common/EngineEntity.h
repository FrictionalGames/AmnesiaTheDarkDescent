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

#ifndef HPLEDITOR_ENGINE_ENTITY_H
#define HPLEDITOR_ENGINE_ENTITY_H

#include "StdAfx.h"

using namespace hpl;

//-----------------------------------------------------------------------

class iEntityWrapper;
class cEntityIcon;

class cEditorWindowViewport;

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// ENGINE ENTITY BASE
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

class iEngineEntity
{
public:
	iEngineEntity(iEntityWrapper* apParent):mpParent(apParent), mpEntity(NULL) {}
	virtual ~iEngineEntity() {}

	iEntity3D* GetEntity() { return mpEntity; }
	virtual cMeshEntity* GetMeshEntity() { return NULL; }

	virtual bool Create(const tString& asName)=0;
	bool IsCreated() { return mpEntity!=NULL; }

	bool IsCulledByFrustum(cCamera* apCamera);
	bool IsInsideBoundingVolume(cBoundingVolume* apBV, cEditorWindowViewport* apViewport);
	bool CheckRayBoundingVolumeIntersect(cEditorWindowViewport* apViewport, cVector3f* apPos, float* apT=NULL);

	virtual bool Check2DBoxIntersect(cEditorWindowViewport* apViewport, const cRect2l& aBox);
	virtual bool CheckRayIntersect(cEditorWindowViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT=NULL);

	virtual cBoundingVolume* GetRenderBV();
	virtual cBoundingVolume* GetPickBV(cEditorWindowViewport* apViewport)=0;

	void SetMatrix(const cMatrixf& amtxX);
	const cMatrixf& GetWorldMatrix();

	virtual void Update() {}
	virtual void UpdateVisibility()=0;

	virtual void Draw(cEditorWindowViewport* apViewport, 
					  cRendererCallbackFunctions* apFunctions,  
					  bool abIsSelected,
					  bool abIsActive,
					  const cColor& aHighlightCol=cColor(1,1)) {};
	virtual void DrawProgram(cEditorWindowViewport* apViewport, 
							 cRendererCallbackFunctions* apFunctions, 
							 iGpuProgram* apProg, 
							 const cColor& aCol) {}

protected:
	iEntityWrapper* mpParent;
	iEntity3D* mpEntity;
};

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// MESH ENTITY BASE + TYPES
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

class iEngineEntityMesh : public iEngineEntity
{
public:
	iEngineEntityMesh(iEntityWrapper* apParent);
	~iEngineEntityMesh();

	void SetUpMesh();
	cMeshEntity* GetMeshEntity() { return (cMeshEntity*)mpEntity; }

	virtual bool Create(const tString& asName);

	bool CheckRayIntersect(cEditorWindowViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT=NULL);

	cBoundingVolume* GetPickBV(cEditorWindowViewport* apViewport) { return mpEntity->GetBoundingVolume(); }

	void Update();
	void UpdateVisibility();

	void Draw(cEditorWindowViewport* apViewport, 
				cRendererCallbackFunctions* apFunctions,  
				bool abIsSelected,
				bool abIsActive, const cColor& aHighlightCol);
	void DrawProgram(cEditorWindowViewport* apViewport, 
					 cRendererCallbackFunctions* apFunctions, 
					 iGpuProgram* apProg, 
					 const cColor& aCol);

	// Own funcs
	void SetCastShadows(bool abX);
	void SetCoverage(float afX);
	bool SetMaterial(const tString&);
	bool SetCustomMaterial(const tString&, bool abDelete=true);
	bool SetCustomMaterial(cMaterial*, bool abDelete=true);

	static void SetDisabledCoverage(float afX) { mfDisabledCoverage = afX; }
	static float GetDisabledCoverage() { return mfDisabledCoverage; }

protected:
	void SetMesh(cMesh* apMesh) { mpMesh = apMesh; }

	cMesh* mpMesh;

	static float mfDisabledCoverage;
};

//-----------------------------------------------------------------------

class cEngineEntityLoadedMesh : public iEngineEntityMesh
{
public:
	cEngineEntityLoadedMesh(iEntityWrapper* apParent, const tString& asFilename);
};

//-----------------------------------------------------------------------

class cEngineEntityLoadedMeshAggregate : public iEngineEntityMesh
{
public:
	cEngineEntityLoadedMeshAggregate(iEntityWrapper* apParent, const tString& asFilename);
	~cEngineEntityLoadedMeshAggregate();

	bool Create(const tString& asName);

	void Update();

	void SetLightsActive(bool abX) { mbLightsActive = abX; }
	void SetParticlesActive(bool abX) { mbParticleSystemsActive = abX; }
	void SetBillboardsActive(bool abX) { mbBillboardsActive = abX; }

protected:
	tString msFilename;

	bool mbLightsActive;
	bool mbParticleSystemsActive;
	bool mbBillboardsActive;

	tLightVec mvLights;
	std::vector<cParticleSystem*> mvParticleSystems;
	std::vector<cBillboard*> mvBillboards;
	std::vector<cSoundEntity*> mvSounds;
};

//-----------------------------------------------------------------------

class cEngineEntityGeneratedMesh : public iEngineEntityMesh
{
public:
	cEngineEntityGeneratedMesh(iEntityWrapper* apParent, cMesh* apMesh);

	bool ReCreate(cMesh* apMesh);

	iVertexBuffer* GetVertexBuffer();
};

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// ICON ENTITY BASE
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

class iIconEntity : public iEngineEntity
{
public:
	iIconEntity(iEntityWrapper* apParent, const tString& asIconFile);
	~iIconEntity();

	virtual bool Create(const tString& asName);
	
	cBoundingVolume* GetPickBV(cEditorWindowViewport* apViewport);

	virtual bool Check2DBoxIntersect(cEditorWindowViewport* apViewport, const cRect2l& aBox);
	virtual bool CheckRayIntersect(cEditorWindowViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT=NULL);

	void UpdateVisibility() { Update(); }

	void Draw(cEditorWindowViewport* apViewport, 
					  cRendererCallbackFunctions* apFunctions, 
					  bool abIsSelected,
					  bool abIsActive,
					  const cColor& aHighlightCol=cColor(1,1));

protected:
	cEntityIcon* mpIcon;
};

#endif //HPLEDITOR_ENGINE_ENTITY_H
