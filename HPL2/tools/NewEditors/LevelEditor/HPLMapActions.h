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

#ifndef ED_HPL_MAP_ACTIONS_H
#define ED_HPL_MAP_ACTIONS_H

#include "../Common/EdAction.h"
#include "../LevelEditor/HPLMap.h"

//--------------------------------------------------------------------------------------

class cHplMap;
class cLevelEditorStaticObjectCombo;

//--------------------------------------------------------------------------------------

class cEdActionMapAddGroup : public iEdActionWorldModifier
{
public:
	cEdActionMapAddGroup(cHplMap* apMap);

	bool Init() { return true; }

	void DoModify();
	void UndoModify();

protected:
	unsigned int mlID;
	tString msName;
};

//--------------------------------------------------------------------------------------

class cEdActionGroupAddObjects : public iEdActionWorldModifier
{
public:
	cEdActionGroupAddObjects(cEdGroup*, const tScnObjList&);

	bool Init() { return true; }

	void DoModify();
	void UndoModify();

protected:
	unsigned int mlID;

	tIntVec mvObjectIDs;
	tIntVec mvObjectOldGroups;
};

//--------------------------------------------------------------------------------------

class cEdActionMapDeleteGroup : public iEdActionWorldModifier
{
public:
	cEdActionMapDeleteGroup(cHplMap* apMap, unsigned int alID);

	bool Init() { return true; }

	void DoModify();
	void UndoModify();

protected:
	unsigned int mlID;
	tString msName;
	bool mbVisible;

	tIntVec mvObjectIDs;
};

//--------------------------------------------------------------------------------------

class cEdActionGroupRemoveObjects : public iEdActionWorldModifier
{
public:
	cEdActionGroupRemoveObjects(cEdGroup* apGroup, const tScnObjList&);

	bool Init() { return true; }

	void DoModify();
	void UndoModify();

protected:
	unsigned int mlID;

	tIntVec mvObjectIDs;
};

//--------------------------------------------------------------------------------------

class cEdActionGroupSetName : public iEdActionWorldModifier
{
public:
	cEdActionGroupSetName(cEdGroup* apGroup, const tString& asName);

	bool Init() { return true; }

	void DoModify();
	void UndoModify();
protected:
	unsigned int mlID;

	tString msOldName;
	tString msNewName;
};

//--------------------------------------------------------------------------------------

class cEdActionGroupSetVisible : public iEdActionWorldModifier
{
public:
	cEdActionGroupSetVisible(cEdGroup* apGroup, bool abNewValue);

	bool Init() { return true; }

	void DoModify();
	void UndoModify();
protected:
	unsigned int mlID;

	bool mbNewValue;
	bool mbOldValue;
};


//--------------------------------------------------------------------------------------

/*
class cLevelEditorActionSetGlobalMaxDecalTris : public iEdActionWorldModifier
{
public:
	cLevelEditorActionSetGlobalMaxDecalTris(iEdWorld* apWorld, int alMaxTris);

	void DoModify();
	void UndoModify();

protected:
	void Apply(const int& aX);

	int mOldValue;
	int mNewValue;
};
*/

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

class cEdActionMapAddStaticObjectCombo : public iEdActionWorldModifier
{
public:
	cEdActionMapAddStaticObjectCombo(iEdWorld* apWorld);

	bool Init() { return true; }

	void DoModify();
	void UndoModify();
private:
	int mlNewComboID;
};

//--------------------------------------------------------------------------------------

class cEdActionMapRemoveStaticObjectCombo : public iEdActionWorldModifier
{	
public:
	cEdActionMapRemoveStaticObjectCombo(iEdWorld* apWorld, int alComboID);

	bool Init() { return true; }

	void DoModify();
	void UndoModify();
protected:
	int mlComboID;
	cColor mColor;
	tIntVec mvObjIds;
};

//--------------------------------------------------------------------------------------

class cEdActionComboAddObject : public iEdActionWorldModifier
{
public:
	cEdActionComboAddObject(cLevelEditorStaticObjectCombo* apCombo, iEdScnObject* apObj );

	bool Init() { return mlObjectID!=-1; }

	void DoModify();
	void UndoModify();
private:
	void Apply(int alComboToRemove, int alComboToAdd);
	
	int mlObjectID;
	int mlOldComboID;
	int mlNewComboID;
};

//--------------------------------------------------------------------------------------

class cEdActionComboRemoveObject : public iEdActionWorldModifier
{
public:
	cEdActionComboRemoveObject(cLevelEditorStaticObjectCombo* apCombo, iEdScnObject* apObj);

	bool Init() { return mlObjectID!=-1; }
	
	void DoModify();
	void UndoModify();
private:

	int mlComboID;
	int mlObjectID;
};

//--------------------------------------------------------------------------------------

class cEdActionComboSetColor : public iEdActionWorldModifier
{
public:
	cEdActionComboSetColor(cLevelEditorStaticObjectCombo* apCombo, const cColor& aCol);

	bool Init() { return true; }

	void DoModify();
	void UndoModify();
private:
	void Apply(const cColor& aX);

	int mlComboID;
	cColor mNewColor;
	cColor mOldColor;
};

//--------------------------------------------------------------------------------------

#endif // ED_HPL_MAP_ACTIONS_H
