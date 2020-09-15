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

#ifndef HPLEDITOR_ENTITY_WRAPPER_STATIC_OBJECT_H
#define HPLEDITOR_ENTITY_WRAPPER_STATIC_OBJECT_H

#include "../common/StdAfx.h"

using namespace hpl;

#include "EntityWrapper.h"

//---------------------------------------------------------------

#define StaticObjectPropIdStart 140

class cEditorWindowEntityEditBoxStaticObject;

enum eStaticObjectInt
{
	eStaticObjectInt_FileIndex = StaticObjectPropIdStart,

	eStaticObjectInt_LastEnum,
};

enum eStaticObjectBool
{
	eStaticObjectBool_Collides = StaticObjectPropIdStart,
	eStaticObjectBool_CastShadows,

	eStaticObjectBool_LastEnum,
};

enum eStaticObjectStr
{
	eStaticObjectStr_Filename = StaticObjectPropIdStart,

	eStaticObjectStr_LastEnum,
};

//---------------------------------------------------------------

class cEntityWrapperDataStaticObject : public iEntityWrapperData
{
public:
	cEntityWrapperDataStaticObject(iEntityWrapperType*);
	bool Load(cXmlElement* apElement);

	iEntityWrapper* CreateSpecificEntity();
};

//---------------------------------------------------------------

class cEntityWrapperTypeStaticObject : public iEntityWrapperType
{
public:
	cEntityWrapperTypeStaticObject() : iEntityWrapperType(eEditorEntityType_StaticObject, _W("Static Object"), "StaticObject")
	{
		GetPropBool(eObjBool_Active)->SetSaved(false);

		AddBool(eStaticObjectBool_Collides, "Collides");
		AddBool(eStaticObjectBool_CastShadows, "CastShadows");

		AddInt(eStaticObjectInt_FileIndex, "FileIndex", -1, ePropCopyStep_PreEnt);
		AddString(eStaticObjectStr_Filename, "Filename", "", ePropCopyStep_PreEnt, false);
	}

	iEntityWrapperData* CreateSpecificData() { return hplNew(cEntityWrapperDataStaticObject,(this)); }
};

//---------------------------------------------------------------

class cEntityWrapperStaticObject : public iEntityWrapper
{
public:
	cEntityWrapperStaticObject(iEntityWrapperData* apData);
	~cEntityWrapperStaticObject();

	bool GetProperty(int, int&);
	bool GetProperty(int, bool&);
	bool GetProperty(int, tString&);

	bool SetProperty(int, const int&);
	bool SetProperty(int, const bool&);
	bool SetProperty(int, const tString&);

	void SetCollides(bool abX) { mbCollides = abX; }
	bool GetCollides() { return mbCollides; }

	void SetCastShadows(bool abX);
	bool GetCastShadows() { return mbCastShadows; }

	void SetFileIndex(int alIdx) { mlFileIndex = alIdx; }
	int GetFileIndex() { return mlFileIndex; }

	void SetFilename(const tString& asFilename);

	cEditorWindowEntityEditBox* CreateEditBox(cEditorEditModeSelect* apEditMode);

	bool IsAffectedByDecal(bool abAffectsStaticObject, bool abAffectsPrimitive, bool abAffectsEntity);

protected:

	iEngineEntity* CreateSpecificEngineEntity();

	///////////////////////////
	// Data

	bool mbCollides;
	bool mbCastShadows;

	int mlFileIndex;
};

//---------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_WRAPPER_STATIC_OBJECT_H
