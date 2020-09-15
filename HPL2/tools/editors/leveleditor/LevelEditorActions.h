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

#ifndef LEVEL_EDITOR_ACTIONS_H
#define LEVEL_EDITOR_ACTIONS_H

#include "../common/EditorAction.h"
#include "../common/EditorEditModeSelect.h"

#include "LevelEditorTypes.h"


//--------------------------------------------------------------------------------------

class cLevelEditor;
class cLevelEditorWorld;
class cLevelEditorStaticObjectCombo;

//--------------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------

class cLevelEditorActionSetSkyboxActive : public iEditorActionWorldModifier
{
public:
	cLevelEditorActionSetSkyboxActive(iEditorWorld* apWorld, bool abX);

	void DoModify();
	void UndoModify();

protected:
	void Apply(bool abX);

	bool mbOldValue;
	bool mbNewValue;
};

class cLevelEditorActionSetSkyboxTexture : public iEditorActionWorldModifier
{
public:
	cLevelEditorActionSetSkyboxTexture(iEditorWorld* apWorld, const tString& asX);

	void DoModify();
	void UndoModify();

protected:
	void Apply(const tString& asX);

	tString msOldValue;
	tString msNewValue;
};

class cLevelEditorActionSetSkyboxColor : public iEditorActionWorldModifier
{
public:
	cLevelEditorActionSetSkyboxColor(iEditorWorld* apWorld, const cColor& aX);

	void DoModify();
	void UndoModify();

protected:
	void Apply(const cColor& aX);

	cColor mOldValue;
	cColor mNewValue;
};

//--------------------------------------------------------------------------------------

enum eFogBoolProp
{
	eFogBoolProp_Active,
	eFogBoolProp_Culling,
};

enum eFogFloatProp
{
	eFogFloatProp_Start,
	eFogFloatProp_End,
	eFogFloatProp_FalloffExp
};

//--------------------------------------------------------------------------------------

class cLevelEditorActionSetFogBoolProperty : public iEditorActionWorldModifier
{
public:
	cLevelEditorActionSetFogBoolProperty(iEditorWorld* apWorld, eFogBoolProp aProp, bool abX);

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

class cLevelEditorActionSetFogFloatProperty : public iEditorActionWorldModifier
{
public:
	cLevelEditorActionSetFogFloatProperty(iEditorWorld* apWorld, eFogFloatProp aProp, float afX);

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

class cLevelEditorActionSetFogColor : public iEditorActionWorldModifier
{
public:
	cLevelEditorActionSetFogColor(iEditorWorld* apWorld, const cColor& aX);

	void DoModify();
	void UndoModify();

protected:
	void Apply(const cColor& aX);

	cColor mOldValue;
	cColor mNewValue;
};

//--------------------------------------------------------------------------------------

class cLevelEditorActionSetGlobalMaxDecalTris : public iEditorActionWorldModifier
{
public:
	cLevelEditorActionSetGlobalMaxDecalTris(iEditorWorld* apWorld, int alMaxTris);

	void DoModify();
	void UndoModify();

protected:
	void Apply(const int& aX);

	int mOldValue;
	int mNewValue;
};

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

class cLevelEditorActionAddStaticObjectCombo : public iEditorActionWorldModifier
{
public:
	cLevelEditorActionAddStaticObjectCombo(iEditorWorld* apWorld);

	void DoModify();
	void UndoModify();
private:
	int mlNewComboID;
};

//--------------------------------------------------------------------------------------

class cLevelEditorActionRemoveStaticObjectCombo : public iEditorActionWorldModifier
{
public:
	cLevelEditorActionRemoveStaticObjectCombo(iEditorWorld* apWorld, int alComboID);

	bool IsValidAction() { return mlComboID!=-1; }

	void DoModify();
	void UndoModify();
protected:
	int mlComboID;
	cColor mColor;
	tIntVec mvObjIds;
};

//--------------------------------------------------------------------------------------

class cLevelEditorActionAddObjectToCombo : public iEditorActionWorldModifier
{
public:
	cLevelEditorActionAddObjectToCombo(iEditorWorld* apWorld, cLevelEditorStaticObjectCombo* apCombo, cEntityWrapperStaticObject* apObj );

	bool IsValidAction() { return mlObjectID!=-1; }

	void DoModify();
	void UndoModify();
private:
	void Apply(int alComboToRemove, int alComboToAdd);
	
	int mlObjectID;
	int mlOldComboID;
	int mlNewComboID;
};

//--------------------------------------------------------------------------------------

class cLevelEditorActionRemoveObjectFromCombo : public iEditorActionWorldModifier
{
public:
	cLevelEditorActionRemoveObjectFromCombo(iEditorWorld* apWorld, cLevelEditorStaticObjectCombo* apCombo, cEntityWrapperStaticObject* apObj );

	bool IsValidAction() { return mlObjectID!=-1; }

	void DoModify();
	void UndoModify();
private:

	int mlComboID;
	int mlObjectID;
};

//--------------------------------------------------------------------------------------

class cLevelEditorActionComboSetColor : public iEditorActionWorldModifier
{
public:
	cLevelEditorActionComboSetColor(iEditorWorld* apWorld, int alComboID, const cColor& aCol);

	void DoModify();
	void UndoModify();
private:
	void Apply(const cColor& aX);

	int mlComboID;
	cColor mNewColor;
	cColor mOldColor;
};

//--------------------------------------------------------------------------------------

#endif // LEVEL_EDITOR_ACTIONS_H
