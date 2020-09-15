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

#ifndef ED_OBJ_PRIMITIVE_H
#define ED_OBJ_PRIMITIVE_H

#include "../Common/EdScnObject.h"

//------------------------------------------------------------------------

#define PrimitivePropIdStart 110

enum ePrimitiveStr
{
	ePrimitiveStr_Material = PrimitivePropIdStart,

	ePrimitiveStr_LastEnum
};

enum ePrimitiveBool
{
	ePrimitiveBool_CastShadows = PrimitivePropIdStart,
	ePrimitiveBool_Collides,

	ePrimitiveBool_LastEnum,
};

//------------------------------------------------------------------------

class iPrimitiveMeshCreator
{
public:
	iPrimitiveMeshCreator();

	virtual void OnViewportMouseDown(const cViewportClick& aData){}
	virtual void OnViewportMouseUp(const cViewportClick& aData){}

	virtual void Draw(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions)=0;

	virtual iEdObjectData* CreateData();
	virtual void SetUpData(iEdWindow*, iEdObjectData*);
    
	virtual void Reset(){}

	virtual bool IsDoneCreating()=0;
protected:
};

//------------------------------------------------------------------------

class cTypePrimitive : public iEdScnObjType
{
public:
	cTypePrimitive();

	bool Init();

protected:
	iEdObjectData* CreateTypeSpecificData() { return NULL; }
};

//------------------------------------------------------------------------

class iPrimitiveSubType : public iEdScnObjType
{
public:
	iPrimitiveSubType(const tWString&, const tString&, iPrimitiveMeshCreator*);

	iPrimitiveMeshCreator* mpCreator;
};

//------------------------------------------------------------------------

class iEdScnObjPrimitive : public iEdScnObject
{
public:
	iEdScnObjPrimitive(iEdObjectData*);
	virtual ~iEdScnObjPrimitive();

	bool SetProperty(int, const tString&);
	bool SetProperty(int, const bool);

	bool GetProperty(int, tString&);
	bool GetProperty(int, bool&);

	iEdEditPane* CreateEditPane();

	void SetMaterial(const tString& asMaterial);
	tString& GetMaterial() { return msMaterial; }

	void SetCastShadows(bool abX);
	bool GetCastShadows() { return mbCastShadows; }

	void SetCollides(bool abX);
	bool GetCollides() { return mbCollides; }

	bool IsAffectedByDecal(bool abAffectsStaticObject, bool abAffectsPrimitive, bool abAffectsEntity);

protected:
	iEngineObject* CreateEngineObject();
	virtual cMesh* CreatePrimitiveMesh()=0;
	//eEdPrimitiveType mPType;

	tString msMaterial;
	bool mbCastShadows;
	bool mbCollides;
};

//------------------------------------------------------------------------

#endif // ED_OBJ_PRIMITIVE_H
