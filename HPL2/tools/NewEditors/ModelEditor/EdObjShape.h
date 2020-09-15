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

#ifndef ED_OBJ_SHAPE_H
#define ED_OBJ_SHAPE_H

#include "../Common/StdAfx.h"

using namespace hpl;

#include "../Common/EdScnObject.h"

//---------------------------------------------------------------

class iEdObjShape;
class cEdObjBody;

enum eEdShape
{
	eEdShape_Box,
	eEdShape_Cylinder,
	eEdShape_Sphere,
	eEdShape_Capsule,

	eEdShape_LastEnum
};

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

enum eShapeVec3f
{
	eShapeVec3f_Size = eScnVec3f_LastEnum,

	eShapeVec3f_LastEnum,
};

class cTypeShape : public iEdScnObjType
{
public:
	cTypeShape();
	~cTypeShape();

	bool Init();

	iEdObjectType* GetTypeByXmlElement(cXmlElement*);

	cMaterial* GetShapeMaterial(int, int);
	const tString& GetMaterialFile(int, int);

	iEdEditPane* CreateGroupEditPane(iEdScnObject*);

protected:
	iEdObjectData* CreateTypeSpecificData() { return NULL; }

	tString mvMaterialFiles[2][2];
	cMaterial* mvMaterials[2][2];
};

class iShapeSubType : public iEdScnObjType
{
public:
	iShapeSubType(const tWString& asName);
	~iShapeSubType();

	iEdObjectType* GetTypeByXmlElement(cXmlElement*);

protected:
};

class iEdObjShapeData : public iEdScnObjData
{
public:
	iEdObjShapeData(iEdObjectType*);

	void CopyFromObject(iEdObject*);
	//void CopyToObject(iEdObject* );

	bool LoadSpecificData(cXmlElement*);
	bool SaveSpecificData(cXmlElement*);
	
protected:
	iEdObjShape* mpShape;
};

//---------------------------------------------------------------

class iEdObjShape : public iEdScnObject
{
public:
	iEdObjShape(iEdObjectData*);
	~iEdObjShape();

	cMaterial* GetShapeMaterial(int, int);
	const tString& GetShapeMaterialFile(int);

	virtual eEdShape GetShapeType()=0;
	
	bool SetProperty(int, const tString&);
	bool GetProperty(int, tString&);

	bool SetProperty(int, const cVector3f&);
	bool GetProperty(int, cVector3f&);

	bool CheckPicked(cUIPickFilter*, cUIPickMethod*, iEdScnObject**);

	iEdEditPane* CreateEditPane();

	void SetSize(const cVector3f&);
	const cVector3f& GetSize() { return mvShapeSize; }
	
	void SetParentBody(cEdObjBody* apParentBody);
	cEdObjBody* GetParentBody() { return mpParentBody; }
	bool HasParentBody() { return GetParentBody()!=NULL; }

	static eEdShape StringToEditorBodyShape(const tString&);
	static tString EditorBodyShapeToString(eEdShape);

	void SetRelativeMatrix(const cMatrixf& amtxRelMatrix) { mpRelativeMatrix = (cMatrixf*)&amtxRelMatrix; }
	cMatrixf* GetRelativeMatrix() { return mpRelativeMatrix; }

	iEdAction* CreateActionCreateBody();
	static iEdAction* CreateActionCreateBodyFromShapes(iEdWorld* apWorld, const tScnObjList& alstShapes);

	bool SkipAddingToOutline(int alLevel, const tScnObjList& alstAddedChildren);

	static bool mbHideConnectedShapes;

	static int mlMeshResolution;

protected:
	void OnDraw(const cModuleDrawData&);

	bool OnUpdate(bool);

	virtual cMesh* CreateShape() { return NULL; }
	iEngineObject* CreateEngineObject();

	///////////////////////////
	// Data
	eEdShape mShapeType;
	iVertexBuffer* mpVBShape;
	cVector3f mvShapeSize;
	
	cEdObjBody* mpParentBody;
	cMatrixf* mpRelativeMatrix;
};

//---------------------------------------------------------------

//---------------------------------------------------------------

#endif // ED_OBJ_SHAPE_H
