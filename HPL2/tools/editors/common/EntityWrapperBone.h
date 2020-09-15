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

#ifndef HPLEDITOR_ENTITY_WRAPPER_BONE_H
#define HPLEDITOR_ENTITY_WRAPPER_BONE_H

#include "EntityWrapper.h"

//------------------------------------------------------------

class cEntityWrapperBone;

typedef std::vector<cEntityWrapperBone*> tBoneWrapperVec;
typedef tBoneWrapperVec::iterator		 tBoneWrapperVecIt;

//------------------------------------------------------------

enum eBoneInt
{
	eBoneInt_ParentBoneID = eObjInt_LastEnum,

	eBoneInt_LastEnum,
};

class cEntityWrapperTypeBone : public iEntityWrapperType
{
	friend class cEntityWrapperBone;
public:
	cEntityWrapperTypeBone();

	tIntList GetBoneIDs();

	void ClearBones(bool);

protected:
	void AddBone(cEntityWrapperBone*);
	void RemoveBone(cEntityWrapperBone*);

	iEntityWrapperData* CreateSpecificData();

	tBoneWrapperVec mvBones;
};

class cEntityWrapperDataBone : public iEntityWrapperData
{
public:
	cEntityWrapperDataBone(iEntityWrapperType*);

	//void CopyToEntity(iEntityWrapper* apEntity);
	//void Load(cXmlElement* apElement);

protected:
	iEntityWrapper* CreateSpecificEntity();

	int mlParentBoneID;
};

//------------------------------------------------------------

typedef std::vector<cEntityWrapperBone*> tBoneWrapperVec;
typedef tBoneWrapperVec::iterator		 tBoneWrapperVecIt;

//------------------------------------------------------------

class cEntityWrapperBone : public iEntityWrapper
{
public:
	cEntityWrapperBone(iEntityWrapperData*);
	~cEntityWrapperBone();

	bool SetProperty(int, const int&);
	bool GetProperty(int, int&);

	void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, 
				iEditorEditMode* apEditMode, bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol);
	
	bool CheckRayIntersect(cEditorWindowViewport*, cVector3f* , tVector3fVec*, float*);
	//bool CheckRayBoundingVolumeIntersection(const cVector3f& avRayStart, const cVector3f& avRayEnd, cVector3f* apIntersection);
	//bool CheckRayAccurateIntersection(const cVector3f& avRayStart, const cVector3f& avRayEnd, cVector3f* apIntersection, tVector3fVec* apTriangle=NULL);

	void OnSetSelected(bool abX);

	cEditorWindowEntityEditBox* CreateEditBox(cEditorEditModeSelect* apEditMode);

	void DrawBone(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEditorEditMode* apEditMode, bool abIsSelected);

	void SetParentBone(cEntityWrapperBone* apBone);
	void AddChildBone(cEntityWrapperBone* apBone);
	void RemoveChildBone(cEntityWrapperBone* apBone);

	tBoneWrapperVec& GetChildBones() { return mvChildBones; }

	void CreateLinkToParent();

	iVertexBuffer* GetBoneVB() { return mpVBBone; }
	void SetBoneVBColor(const cColor& aCol);

protected:
	void OnAddToWorld();

	cEntityWrapperBone* mpParentBone;
	tBoneWrapperVec mvChildBones;

	
	iVertexBuffer* mpVBBone;
};

//------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_WRAPPER_BONE_H
