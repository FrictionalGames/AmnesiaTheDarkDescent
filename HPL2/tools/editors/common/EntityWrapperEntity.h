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

#ifndef HPLEDITOR_ENTITY_WRAPPER_ENTITY_H
#define HPLEDITOR_ENTITY_WRAPPER_ENTITY_H

#include "../common/StdAfx.h"

using namespace hpl;

#include "EntityWrapper.h"

//---------------------------------------------------------------

class cEditorWindowEntityEditBoxEntity;

//---------------------------------------------------------------

#define EntityPropIdStart 40

enum eEntityInt
{
	eEntityInt_FileIndex = EntityPropIdStart,

	eEntityInt_LastEnum,
};

enum eEntityStr
{
	eEntityStr_Filename = EntityPropIdStart,

	eEntityStr_LastEnum,
};

//---------------------------------------------------------------

class cEntityWrapperTypeEntity : public iEntityWrapperTypeUserDefinedEntity
{
public:
	cEntityWrapperTypeEntity(cEditorUserClassSubType*);

	tString ToString();

	bool IsAppropriateType(cXmlElement*);
	bool IsAppropriateDefaultType(cXmlElement*);

	const tString& GetUserTypeName();
	const tString& GetUserSubTypeName();

	iEditorVar* GetAffectsShadowsVar();
	iEditorVar* GetAffectsLightVar();
	iEditorVar* GetAffectsParticlesVar();

	iEditorVar* GetLinkedEditorSetupVar(const tWString&, eVariableType);

protected:
 	iEntityWrapperData* CreateSpecificData();

	/////////////////////////
	// Data for making checks for appropriate type faster and nicer
	static tString msLastCheckedFile;
	static tString msLastCheckedType;
	static tString msLastCheckedSubType;
};

//---------------------------------------------------------------

class cEntityWrapperDataEntity : public iEntityWrapperDataUserDefinedEntity
{
public:
	cEntityWrapperDataEntity(iEntityWrapperType*);
	~cEntityWrapperDataEntity();

	bool Load(cXmlElement* apElement);

protected:
	iEntityWrapper* CreateSpecificEntity();
};

//---------------------------------------------------------------

class cEntityWrapperEntity : public iEntityWrapperUserDefinedEntity
{
public:
	cEntityWrapperEntity(iEntityWrapperData* apData);
	~cEntityWrapperEntity();

	bool GetProperty(int, int&);
	bool GetProperty(int, tString&);

	bool SetProperty(int, const int&);
	bool SetProperty(int, const tString&);

	void SetFileIndex(int alIdx) { mlFileIndex = alIdx; }
	int GetFileIndex() { return mlFileIndex; }

	void SetFilename(const tString& asFilename);

	bool GetTypeChanged() { return mbTypeChanged; }
	void SetTypeChanged(bool abX) { mbTypeChanged = abX; }

	cEditorWindowEntityEditBox* CreateEditBox(cEditorEditModeSelect* apEditMode);

	bool IsAffectedByDecal(bool abAffectsStaticObject, bool abAffectsPrimitive, bool abAffectsEntity);

	void UpdateEntity();

	iEditorAction* CreateSetPropertyActionString(int alPropID, const tString& asX);
protected:
	iEngineEntity* CreateSpecificEngineEntity();

	void OnSetActive(bool abX);
	void OnSetVar(const tWString& asName, const tWString& asValue);

	bool SetEntityType(iEntityWrapperType* apType);
	///////////////////////////
	// Data
	std::vector<iLight*> mvLights;
	std::vector<cMatrixf> mvLightLocalTransforms;

	int mlFileIndex;
	bool mbAffectedByDecal;

	bool mbTypeChanged;
};

//---------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_WRAPPER_ENTITY_H
