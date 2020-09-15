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

#ifndef HPLEDITOR_ENTITY_WRAPPER_SUB_MESH_H
#define HPLEDITOR_ENTITY_WRAPPER_SUB_MESH_H


#include "EntityWrapper.h"
#include "EngineEntity.h"

//---------------------------------------------------------------

class cEntityWrapperBody;

class cEntityWrapperSubMesh;

typedef std::vector<cEntityWrapperSubMesh*> tSubMeshWrapperVec;
typedef tSubMeshWrapperVec::iterator tSubMeshWrapperVecIt;

//---------------------------------------------------------------

class cEngineEntitySubMesh : public iEngineEntity
{
public:
	cEngineEntitySubMesh(iEntityWrapper*);

	bool Create(const tString&);

	bool CheckRayIntersect(cEditorWindowViewport*, cVector3f*, tVector3fVec*, float*);

	void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, 
				bool abIsSelected, bool abIsActive, const cColor& aHighlightCol);

	cBoundingVolume* GetPickBV(cEditorWindowViewport* apViewport);
	void UpdateVisibility(){}
protected:
};

//---------------------------------------------------------------

enum eSubMeshInt
{
	eSubMeshInt_SubMeshID = eObjInt_LastEnum,

	eSubMeshInt_LastEnum,
};

enum eSubMeshStr
{
	eSubMeshStr_Material = eObjStr_LastEnum,

	eSubMeshStr_LastEnum,
};


//---------------------------------------------------------------

class cEntityWrapperTypeSubMesh : public iEntityWrapperType
{
	friend class cEntityWrapperSubMesh;
public:
	cEntityWrapperTypeSubMesh();

	cMeshEntity* GetMesh() { return mpMesh; }
	bool SetMesh(const tString& asFilename, bool abDeleteData,
											tEntityDataVec& avSubMeshData, const tIntList& alstSubMeshIDs, 
											tEntityDataVec& avBoneData, const tIntList& alstBoneIDs);
	void ClearMesh();

	tString GetMeshFilename();
	tIntList GetSubMeshIDs();
protected:
	void AddSubMesh(cEntityWrapperSubMesh*);
	void RemoveSubMesh(cEntityWrapperSubMesh*);
	void ClearSubMeshes(bool);

	iEntityWrapperData* CreateSpecificData();

	cMeshEntity* mpMesh;
	tSubMeshWrapperVec mvSubMeshes;
};

//---------------------------------------------------------------

class cEntityWrapperDataSubMesh : public iEntityWrapperData
{
public:
	cEntityWrapperDataSubMesh(iEntityWrapperType*);

protected:
	iEntityWrapper* CreateSpecificEntity();
};

//---------------------------------------------------------------

class cEntityWrapperSubMesh : public iEntityWrapper
{
public:
	cEntityWrapperSubMesh(iEntityWrapperData*);
	~cEntityWrapperSubMesh();

	bool SetProperty(int, const int&);
	bool SetProperty(int, const tString&);
	bool GetProperty(int, int&);
	bool GetProperty(int, tString&);

	/////////////////////////////////////////////////
	// iEntityWrapper methods

    // Edition Helpers
	cEditorWindowEntityEditBox* CreateEditBox(cEditorEditModeSelect* apEditMode);

	int GetSubMeshID() { return mlSubMeshID; }
	const tString& GetMeshFilename() { return msMeshFilename; }

	void SetMaterialFile(const tString& asFile);
	const tString& GetMaterialFile() { return msMatFile; }

	const cVector3f& GetLocalTranslation() { return mvLocalTranslation; }
	const cVector3f& GetLocalRotation() { return mvLocalRotation; }
	const cVector3f& GetLocalScale() { return mvLocalScale; }

protected:
	void OnAddToWorld();

	iEngineEntity* CreateSpecificEngineEntity();
	
	///////////////////////////
	// Data
	int mlSubMeshID;
	tString msMeshFilename;
	tString msMatFile;

	//tString msSubMeshName;
	cVector3f mvLocalTranslation;
	cVector3f mvLocalRotation;
	cVector3f mvLocalScale;
};

//---------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_WRAPPER_SUB_MESH_H
