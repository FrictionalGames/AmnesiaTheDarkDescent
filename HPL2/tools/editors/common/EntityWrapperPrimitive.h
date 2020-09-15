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

#ifndef HPLEDITOR_ENTITY_WRAPPER_PRIMITIVE_H
#define HPLEDITOR_ENTITY_WRAPPER_PRIMITIVE_H

#include "EntityWrapper.h"

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

class iEntityWrapperTypePrimitive : public iEntityWrapperType
{
public:
	iEntityWrapperTypePrimitive(const tString&, int);

	int GetPrimitiveType() { return mlSubType; }

protected:
	int mlSubType;
};

//------------------------------------------------------------------------

class iEntityWrapperPrimitive : public iEntityWrapper
{
public:
	iEntityWrapperPrimitive(iEntityWrapperData*);
	virtual ~iEntityWrapperPrimitive();

	int GetPrimitiveType() { return ((iEntityWrapperTypePrimitive*)mpType)->GetPrimitiveType(); }

	bool SetProperty(int, const tString&);
	bool SetProperty(int, const bool&);

	bool GetProperty(int, tString&);
	bool GetProperty(int, bool&);

	cEditorWindowEntityEditBox* CreateEditBox(cEditorEditModeSelect* apEditMode);

	void SetMaterial(const tString& asMaterial);
	tString& GetMaterial() { return msMaterial; }

	void SetCastShadows(bool abX);
	bool GetCastShadows() { return mbCastShadows; }

	void SetCollides(bool abX);
	bool GetCollides() { return mbCollides; }

	bool IsAffectedByDecal(bool abAffectsStaticObject, bool abAffectsPrimitive, bool abAffectsEntity);

protected:
	iEngineEntity* CreateSpecificEngineEntity();
	virtual cMesh* CreatePrimitiveMesh()=0;
	//eEditorPrimitiveType mPType;

	tString msMaterial;
	bool mbCastShadows;
	bool mbCollides;
};

//------------------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_WRAPPER_PRIMITIVE_H
