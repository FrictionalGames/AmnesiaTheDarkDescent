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

#ifndef ED_SCN_WORLD_ACTIONS_H
#define ED_SCN_WORLD_ACTIONS_H

#include "../Common/EdAction.h"
#include "../LevelEditor/HPLMap.h"
//#include "../common/EditorEditModeSelect.h"

//#include "LevelEditorTypes.h"


//--------------------------------------------------------------------------------------

class cLevelEditor;
class iEdWorld;
class cLevelEditorStaticObjectCombo;

//--------------------------------------------------------------------------------------

/*
class cLevelEditorActionGroupAdd : public iEditorAction
{
public:
	cLevelEditorActionGroupAdd(iEditorBase* apEditor, unsigned int alID, const tString& asName);

	void Do();
	void Undo();

protected:
	iEditorBase* mpEditor;
	unsigned int mlID;
	tString msName;

	tIntVec mvEntityIDs;
	tUIntVec mvEntityGroups;
};

//--------------------------------------------------------------------------------------

class cLevelEditorActionGroupAddSelected : public iEditorAction
{
public:
	cLevelEditorActionGroupAddSelected(iEditorBase* apEditor, unsigned int alID);

	void Do();
	void Undo();

protected:
	iEditorBase* mpEditor;
	unsigned int mlID;

	tIntVec mvEntityIDs;
	tUIntVec mvEntityGroups;
};

//--------------------------------------------------------------------------------------

class cLevelEditorActionGroupDelete : public iEditorAction
{
public:
	cLevelEditorActionGroupDelete(iEditorBase* apEditor, unsigned int alID);

	void Do();
	void Undo();

protected:
	iEditorBase* mpEditor;
	unsigned int mlID;
	tString msName;
	bool mbVis;

	tIntVec mvEntityIDs;
	tUIntVec mvEntityGroups;
};

//--------------------------------------------------------------------------------------

class cLevelEditorActionGroupDeselectEntities : public iEditorAction
{
public:
	cLevelEditorActionGroupDeselectEntities(iEditorBase* apEditor, unsigned int alID);

	void Do();
	void Undo();

protected:
	iEditorBase* mpEditor;
	cEditorEditModeSelect* mpEditMode;
	unsigned int mlID;

	tIntVec mvEntityIDs;
};

//--------------------------------------------------------------------------------------

class cLevelEditorActionGroupRemoveSelected : public iEditorAction
{
public:
	cLevelEditorActionGroupRemoveSelected(iEditorBase* apEditor, unsigned int alID);

	void Do();
	void Undo();

protected:
	iEditorBase* mpEditor;
	unsigned int mlID;

	tIntVec mvEntityIDs;
};

//--------------------------------------------------------------------------------------

class cLevelEditorActionGroupSelectEntities : public iEditorAction
{
public:
	cLevelEditorActionGroupSelectEntities(iEditorBase* apEditor, unsigned int alID);

	void Do();
	void Undo();

protected:
	iEditorBase* mpEditor;
	cEditorEditModeSelect* mpEditMode;
	unsigned int mlID;

	tIntVec mvEntityIDs;
};

//--------------------------------------------------------------------------------------

class cLevelEditorActionGroupSetName : public iEditorAction
{
public:
	cLevelEditorActionGroupSetName(iEditorBase* apEditor, unsigned int alID, const tString& asName);

	void Do();
	void Undo();
protected:
	iEditorBase* mpEditor;

	unsigned int mlID;
	tString msOldName;
	tString msNewName;
};

//--------------------------------------------------------------------------------------

class cLevelEditorActionGroupSetVisibility : public iEditorAction
{
public:
	cLevelEditorActionGroupSetVisibility(iEditorBase* apEditor, unsigned int alID, bool abNewValue);

	void Do();
	void Undo();
protected:
	iEditorBase* mpEditor;

	unsigned int mlID;
	bool mbNewValue;
	bool mbOldValue;
};
*/

//--------------------------------------------------------------------------------------

class cEdActionWorldSetSkyboxActive : public iEdActionWorldModifier
{
public:
	cEdActionWorldSetSkyboxActive(iEdWorld* apWorld, bool abX);

	bool Init() { return true; }

	void DoModify();
	void UndoModify();

protected:
	void Apply(bool abX);

	bool mbOldValue;
	bool mbNewValue;
};

class cEdActionWorldSetSkyboxTexture : public iEdActionWorldModifier
{
public:
	cEdActionWorldSetSkyboxTexture(iEdWorld* apWorld, const tString& asX);

	bool Init() { return true; }

	void DoModify();
	void UndoModify();

protected:
	void Apply(const tString& asX);

	tString msOldValue;
	tString msNewValue;
};

class cEdActionWorldSetSkyboxColor : public iEdActionWorldModifier
{
public:
	cEdActionWorldSetSkyboxColor(iEdWorld* apWorld, const cColor& aX);

	bool Init() { return true; }

	void DoModify();
	void UndoModify();

protected:
	void Apply(const cColor& aX);

	cColor mOldValue;
	cColor mNewValue;
};

//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------

class cEdActionWorldSetFogBool : public iEdActionWorldModifier
{
public:
	cEdActionWorldSetFogBool(iEdWorld* apWorld, eFogBoolProp aProp, bool abX);

	bool Init() { return true; }

	void DoModify();
	void UndoModify();

protected:
	void Apply(bool abX);
	bool GetOldValue();

	eFogBoolProp mProp;

	bool mbOldValue;
	bool mbNewValue;
};

//--------------------------------------------------------------------------------------

class cEdActionWorldSetFogFloat : public iEdActionWorldModifier
{
public:
	cEdActionWorldSetFogFloat(iEdWorld* apWorld, eFogFloatProp aProp, float afX);

	bool Init() { return true; }

	void DoModify();
	void UndoModify();

protected:
	void Apply(float afX);
	float GetOldValue();

	eFogFloatProp mProp;

	float mfOldValue;
	float mfNewValue;
};

//--------------------------------------------------------------------------------------

class cEdActionWorldSetFogColor : public iEdActionWorldModifier
{
public:
	cEdActionWorldSetFogColor(iEdWorld* apWorld, const cColor& aX);

	bool Init() { return true; }

	void DoModify();
	void UndoModify();

protected:
	void Apply(const cColor& aX);

	cColor mOldValue;
	cColor mNewValue;
};

//--------------------------------------------------------------------------------------



#endif // ED_SCN_WORLD_ACTIONS_H
