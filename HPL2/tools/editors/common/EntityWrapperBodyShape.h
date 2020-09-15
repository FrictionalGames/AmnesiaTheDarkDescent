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

#ifndef HPLEDITOR_ENTITY_WRAPPER_BODY_SHAPE_H
#define HPLEDITOR_ENTITY_WRAPPER_BODY_SHAPE_H

#include "../common/StdAfx.h"

using namespace hpl;

#include "EntityWrapper.h"

//---------------------------------------------------------------

class cEntityWrapperBody;

//---------------------------------------------------------------

enum eShapeInt
{
	eShapeInt_ParentBodyID = eObjStr_LastEnum,

	eShapeInt_LastEnum,
};

enum eShapeStr
{
	eShapeStr_ShapeType = eObjStr_LastEnum,

	eShapeStr_LastEnum,
};

class cEntityWrapperTypeBodyShape : public iEntityWrapperType
{
public:
	cEntityWrapperTypeBodyShape();
	~cEntityWrapperTypeBodyShape();

	cMaterial* GetShapeMaterial(int, int);
	const tString& GetMaterialFile(int, int);

protected:
	void OnSetWorld(iEditorWorld*);
	iEntityWrapperData* CreateSpecificData();

	tString mvMaterialFiles[2][2];
	cMaterial* mvMaterials[2][2];
};

class cEntityWrapperDataBodyShape : public iEntityWrapperData
{
public:
	cEntityWrapperDataBodyShape(iEntityWrapperType*);

	void CopyFromEntity(iEntityWrapper* apEntity);
	//void CopyToEntity(iEntityWrapper* apEntity);

	bool SaveSpecific(cXmlElement*);
	
protected:
	iEntityWrapper* CreateSpecificEntity();
	cEntityWrapperBodyShape* mpShape;

	/*
	eEditorBodyShape mBodyShapeType;
	int mlParentBodyID;
	
	cVector3f mvTranslationRelativeToBody;
	cVector3f mvRotationRelativeToBody;
	cVector3f mvScaleRelativeToBody;*/
};

//---------------------------------------------------------------

class cEntityWrapperBodyShape : public iEntityWrapper
{
public:
	cEntityWrapperBodyShape(iEntityWrapperData*);
	~cEntityWrapperBodyShape();
	
	bool SetProperty(int, const tString&);
	bool GetProperty(int, tString&);

	bool EntitySpecificFilterCheck(bool abPassAll, bool abPassType);
	//bool SpecificFilterCheck(const tBoolVec& avFilters);

	void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions,iEditorEditMode* apEditMode, 
				bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol);

	cEditorWindowEntityEditBox* CreateEditBox(cEditorEditModeSelect* apEditMode);
	cEditorWindowEntityEditBoxGroup* CreateGroupAllSameTypeEditBox(cEditorEditModeSelect* apEditMode, tEntityWrapperList& alstEntities);

	void SetParentBody(cEntityWrapperBody* apParentBody);
	cEntityWrapperBody* GetParentBody() { return mpParentBody; }
	bool HasParentBody() { return GetParentBody()!=NULL; }

	//void UpdateTransformRelativeToBodyCenter();

	eEditorBodyShape GetShapeType() { return mShapeType; }

	//void SetAbsPosition(const cVector3f& avPosition);
	//void SetAbsRotation(const cVector3f& avRotation);
	void SetAbsScale(const cVector3f& avScale, int alAxis=-1);

	static eEditorBodyShape StringToEditorBodyShape(const tString&);
	static tString EditorBodyShapeToString(eEditorBodyShape);

	//void UpdateMatrix();
	void UpdateEntity();

	void SetRelativeMatrix(const cMatrixf& amtxRelMatrix) { mpRelativeMatrix = (cMatrixf*)&amtxRelMatrix; }
	cMatrixf* GetRelativeMatrix() { return mpRelativeMatrix; }

	iEditorAction* CreateActionCreateBody();
	static iEditorAction* CreateActionCreateBodyFromShapes(iEditorWorld* apWorld, const tIntList& alstShapeIDs);
protected:
	void OnSetSelected(bool abX);

	cMesh* CreateShape();
	iEngineEntity* CreateSpecificEngineEntity();

	///////////////////////////
	// Data
	eEditorBodyShape mShapeType;
	iVertexBuffer* mpVBShape;
	
	cEntityWrapperBody* mpParentBody;
	cMatrixf* mpRelativeMatrix;

	//cVector3f mvTranslationRelativeToBody;
	//cVector3f mvRotationRelativeToBody;
	//cVector3f mvScaleRelativeToBody;

	//cMatrixf mmtxRelativeWorldMatrix;
};

//---------------------------------------------------------------

//---------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_WRAPPER_STATIC_BODY_SHAPE_H
