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

#ifndef ED_HPL_MAP_H
#define ED_HPL_MAP_H

//----------------------------------------------------------

#include "../Common/EdScnWorld.h"

#include "../LevelEditor/LevelEditorStaticObjectCombo.h"
#include "../LevelEditor/EdGroup.h"

//----------------------------------------------------------

class cTypeCompound;
class cLevelEditorStaticObjectCombo;

//----------------------------------------------------------

class cLevelEditorSessionData : public iEdSessionData
{
public:
	cLevelEditorSessionData(iEdWorld* apWorld);

	bool Load(cXmlElement*);
	bool Save(cXmlElement*);
};

//----------------------------------------------------------

class cMapObjExtension  : public iEdObjExtension
{
public:
	cMapObjExtension(iEdWorld*, iEdObject*);
	~cMapObjExtension();

	bool Save(cXmlElement*) { return true; }
	bool Load(cXmlElement*) { return true; }

	void OnPostDeployAll() {}

	void SetCompoundObject(cEdObjCompound*, bool);
	void SetGroup(cEdGroup*, bool);
	void SetCombo(cLevelEditorStaticObjectCombo*, bool);

	void Copy(iEdObjExtension*) {}

	cEdObjCompound* mpCompound;
	cEdGroup* mpGroup;
	cLevelEditorStaticObjectCombo* mpCombo;
};

//----------------------------------------------------------

class cHplMap : public iEdScnWorld
{
public:
	cHplMap(iEditor*);

	//////////////////////////////////////////
	// StaticObject Combos management
	int GetFreeComboID() { return ++mlComboID; }
	cLevelEditorStaticObjectCombo* CreateStaticObjectCombo(int alID);

	bool AddStaticObjectCombo(cLevelEditorStaticObjectCombo* apCombo);
	void RemoveStaticObjectCombo(int alID);
	void ClearStaticObjectCombos();
	const tStaticObjectComboList& GetStaticObjectCombos() { return mlstStaticObjectCombos; }

	cLevelEditorStaticObjectCombo* GetLastAddedCombo() { return mpLastAddedCombo; }

	cLevelEditorStaticObjectCombo* GetStaticObjectCombo(int alComboId);
	int GetStaticObjectComboNum();

	/////////////////////////////////////////////////
	// Group management
	int GetFreeGroupID() { return ++mlGroupID; }
	cEdGroup* AddGroup(int, bool abAddToList=true);
	void RemoveGroup(int);

	const tGroupList& GetGroups();
	cEdGroup* GetGroupByID(int);
	cEdGroup* GetGroup(int);
	int GetNumGroups();

	cEdGroup* GetNullGroup() { return mpNullGroup; }

	iEdAction* CreateAddComboAction();
	iEdAction* CreateRemoveComboAction(int);

	iEdAction* CreateAddGroupAction();
	iEdAction* CreateRemoveGroupAction(int);


	iEdObjExtension* CreateObjExtension(iEdObject* apObj) { return hplNew(cMapObjExtension,(this, apObj)); }

protected:
	iEdSessionData* CreateSessionData();

	bool LoadContentsFromXmlDoc(iXmlDocument*);
	bool SaveContentsToXmlDoc(iXmlDocument*);

	void OnInit();
	void OnReset();
	void OnUpdate();

	int mlComboID;
	tStaticObjectComboList mlstStaticObjectCombos;
	cLevelEditorStaticObjectCombo* mpLastAddedCombo;

	bool mbGroupsChanged;
	int mlGroupID;
	tGroupList mlstGroups;
	cEdGroup* mpNullGroup;

	cTypeCompound* mpTypeCompound;
};

//----------------------------------------------------------

#endif // ED_HPL_MAP_H
