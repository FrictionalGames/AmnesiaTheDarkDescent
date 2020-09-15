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

#ifndef ED_OBJ_BONE_H
#define ED_OBJ_BONE_H

#include "../Common/EdScnObject.h"

//------------------------------------------------------------

class cEdObjBone;

typedef std::vector<cEdObjBone*>	tEdBoneVec;
typedef tEdBoneVec::iterator		tEdBoneVecIt;

//------------------------------------------------------------

enum eBoneInt
{
	eBoneInt_ParentBoneID = eObjInt_LastEnum,

	eBoneInt_LastEnum,
};

//------------------------------------------------------------

class cTypeBone : public iEdScnObjType
{
	friend class cEdObjBone;
public:
	cTypeBone();

	tIntList GetBoneIDs();

	bool AllowNullEngineObject() { return true; }

protected:
	iEdObjectData* CreateTypeSpecificData();
};

class cEdObjBoneData : public iEdScnObjData
{
public:
	cEdObjBoneData(iEdObjectType*);

	//void CopyToObject(iEdObject*);
	//bool LoadSpecificData(cXmlElement* apElement);

protected:
	iEdObject* CreateTypeSpecificObject();

	int mlParentBoneID;
};

//------------------------------------------------------------

class cEdObjBone : public iEdScnObject
{
public:
	cEdObjBone(iEdObjectData*);
	~cEdObjBone();

	bool SetProperty(int, const int);
	bool GetProperty(int, int&);

	void OnDraw(const cModuleDrawData&);	
	void DrawBone(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions, const cColor& aCol);
	
	bool CheckRayIntersect(iEdViewport*, cVector3f*, tVector3fVec*, float* apT=NULL);

	iEdEditPane* CreateEditPane();

	void SetParentBone(cEdObjBone* apBone);
	void AddChildBone(cEdObjBone* apBone);
	void RemoveChildBone(cEdObjBone* apBone);

	tEdBoneVec& GetChildBones() { return mvChildBones; }

	void CreateLinkToParent();

	iVertexBuffer* GetBoneVB() { return mpVBBone; }

protected:
	cObjectIcon* CreateIcon();

	void OnAddToWorld();

	cEdObjBone* mpParentBone;
	tEdBoneVec mvChildBones;
	
	iVertexBuffer* mpVBBone;
};

//------------------------------------------------------------

#endif // ED_OBJ_BONE_H
