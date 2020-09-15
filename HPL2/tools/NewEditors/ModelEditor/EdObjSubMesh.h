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

#ifndef ED_OBJ_SUB_MESH_H
#define ED_OBJ_SUB_MESH_H

#include "../Common/EdScnObject.h"
#include "../Common/EdEngineObject.h"

//---------------------------------------------------------------

class cEdObjSubMesh;

typedef std::vector<cEdObjSubMesh*> tEdSubMeshVec;
typedef tEdSubMeshVec::iterator		tEdSubMeshVecIt;

//---------------------------------------------------------------

class cEngineSubMesh : public iEngineObject
{
public:
	cEngineSubMesh(iEdScnObject*);

	bool Create(const tString&);

	bool CheckRayIntersect(iEdViewport*, cVector3f*, tVector3fVec*, float*);

	void Draw(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions, 
				bool abIsSelected, bool abIsActive, const cColor& aHighlightCol);

	cBoundingVolume* GetPickBV(iEdViewport* apViewport);
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

class cTypeSubMesh : public iEdScnObjType
{
	friend class cEdObjSubMesh;
public:
	cTypeSubMesh();

	cMeshEntity* GetMesh() { return mpMesh; }
	bool SetMesh(const tString& asFilename, bool abDeleteData,
					tEdObjDataVec& avSubMeshData, const tIntList& alstSubMeshIDs, 
					tEdObjDataVec& avBoneData, const tIntList& alstBoneIDs);
	void ClearMesh();

	tString GetMeshFilename();
	
protected:
	iEdObjectData* CreateTypeSpecificData();

	iEdObjectData* GetDataForName(const tWString&, const tEdObjDataVec&);

	cMeshEntity* mpMesh;
};

//---------------------------------------------------------------

class cEdObjSubMeshData : public iEdScnObjData
{
public:
	cEdObjSubMeshData(iEdObjectType*);

protected:
	iEdObject* CreateTypeSpecificObject();
};

//---------------------------------------------------------------

class cEdObjSubMesh : public iEdScnObject
{
public:
	cEdObjSubMesh(iEdObjectData*);
	~cEdObjSubMesh();

	bool SetProperty(int, const int);
	bool SetProperty(int, const tString&);

	bool GetProperty(int, int&);
	bool GetProperty(int, tString&);

	// Edition Helpers
	iEdEditPane* CreateEditPane();

	int GetSubMeshID() { return mlSubMeshID; }
	const tString& GetMeshFilename() { return msMeshFilename; }

	void SetMaterialFile(const tString& asFile);
	const tString& GetMaterialFile() { return msMatFile; }

	const cVector3f& GetLocalTranslation() { return mvLocalTranslation; }
	const cVector3f& GetLocalRotation() { return mvLocalRotation; }
	const cVector3f& GetLocalScale() { return mvLocalScale; }

protected:
	void OnAddToWorld();

	iEngineObject* CreateEngineObject();
	
	///////////////////////////
	// Data
	int mlSubMeshID;
	tString msMeshFilename;
	tString msMatFile;

	cVector3f mvLocalTranslation;
	cVector3f mvLocalRotation;
	cVector3f mvLocalScale;
};

//---------------------------------------------------------------

#endif // ED_OBJ_SUB_MESH_H
