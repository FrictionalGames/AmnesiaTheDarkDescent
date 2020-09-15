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

#ifndef ED_ENGINE_OBJECT_H
#define ED_ENGINE_OBJECT_H

#include "../Common/StdAfx.h"

using namespace hpl;

//-----------------------------------------------------------------------

class cEdMapObjRoot;
class iEdScnObject;
class cObjectIcon;

class iEdViewport;

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// ENGINE ENTITY BASE
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

class iEngineObject
{
public:
	iEngineObject(iEdScnObject* apParent):mpParent(apParent), mpObject(NULL) {}
	virtual ~iEngineObject() {}

	iEntity3D* GetObject() { return mpObject; }
	virtual cMeshEntity* GetMeshEntity() { return NULL; }

	virtual bool Create(const tString& asName)=0;
	bool IsCreated() { return mpObject!=NULL; }

	bool IsCulledByFrustum(cCamera* apCamera);
	bool IsInsideBoundingVolume(cBoundingVolume* apBV, iEdViewport* apViewport);
	bool CheckRayBoundingVolumeIntersect(iEdViewport* apViewport, cVector3f* apPos, float* apT=NULL);

	virtual bool Check2DBoxIntersect(iEdViewport* apViewport, const cRect2l& aBox);
	virtual bool CheckRayIntersect(iEdViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT=NULL);

	virtual cBoundingVolume* GetRenderBV();
	virtual cBoundingVolume* GetPickBV(iEdViewport* apViewport)=0;

	void SetMatrix(const cMatrixf& amtxX);
	const cMatrixf& GetWorldMatrix();

	virtual void Update() {}
	virtual void UpdateVisibility()=0;

	virtual void Draw(iEdViewport* apViewport, 
					  cRendererCallbackFunctions* apFunctions,  
					  bool abIsSelected,
					  bool abIsActive,
					  const cColor& aHighlightCol=cColor(1,1)) {};
	virtual void DrawProgram(iEdViewport* apViewport, 
							 cRendererCallbackFunctions* apFunctions, 
							 iGpuProgram* apProg, 
							 const cColor& aCol) {}

protected:
	iEdScnObject* mpParent;
	iEntity3D* mpObject;
};

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// MESH ENTITY BASE + TYPES
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

class iEngineMesh : public iEngineObject
{
public:
	iEngineMesh(iEdScnObject* apParent);
	~iEngineMesh();

	void SetUpMesh();
	cMeshEntity* GetMeshEntity() { return static_cast<cMeshEntity*>(mpObject); }

	virtual bool Create(const tString& asName);

	bool CheckRayIntersect(iEdViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT=NULL);

	cBoundingVolume* GetPickBV(iEdViewport* apViewport) { return mpObject->GetBoundingVolume(); }

	void Update();
	void UpdateVisibility();

	void Draw(iEdViewport* apViewport, 
				cRendererCallbackFunctions* apFunctions,  
				bool abIsSelected,
				bool abIsActive, const cColor& aHighlightCol);
	void DrawProgram(iEdViewport* apViewport, 
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

class cEngineLoadedMesh : public iEngineMesh
{
public:
	cEngineLoadedMesh(iEdScnObject* apParent, const tString& asFilename);
};

//-----------------------------------------------------------------------

class cEngineGeneratedMesh : public iEngineMesh
{
public:
	cEngineGeneratedMesh(iEdScnObject* apParent, cMesh* apMesh);

	bool ReCreate(cMesh* apMesh);

	iVertexBuffer* GetVertexBuffer();
};

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// ICON ENTITY BASE
//////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

class iIconObject : public iEngineObject
{
public:
	iIconObject(iEdScnObject* apParent, const tString& asIconFile);
	~iIconObject();

	virtual bool Create(const tString& asName);
	
	cBoundingVolume* GetPickBV(iEdViewport* apViewport);

	virtual bool Check2DBoxIntersect(iEdViewport* apViewport, const cRect2l& aBox);
	virtual bool CheckRayIntersect(iEdViewport* apViewport, cVector3f* apPos, tVector3fVec* apTriangle, float* apT=NULL);

	void UpdateVisibility() { Update(); }

	void Draw(iEdViewport* apViewport, 
					  cRendererCallbackFunctions* apFunctions, 
					  bool abIsSelected,
					  bool abIsActive,
					  const cColor& aHighlightCol=cColor(1,1));

protected:
	cObjectIcon* mpIcon;
};

#endif //HPLEDITOR_ENGINE_ENTITY_H
