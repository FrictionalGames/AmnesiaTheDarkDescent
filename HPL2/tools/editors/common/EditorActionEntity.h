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

#ifndef HPLEDITOR_EDITOR_ACTION_ENTITY_H
#define HPLEDITOR_EDITOR_ACTION_ENTITY_H


#include "../common/StdAfx.h"
using namespace hpl;

#include "EditorAction.h"
#include "EditorTypes.h"

//--------------------------------------------------------------------

class cEditorEditModeSelect;
class iEditorWorld;
class cEditorGrid;
class cEditorSelection;

enum eEditorEntityType;

//--------------------------------------------------------------------

enum eEntityBoolProperty
{
	eEntityBoolProperty_Active,
	eEntityBoolProperty_Visible,

	eEntityBoolProperty_LastEnum
};

enum eEntityStringProperty
{
	eEntityStringProperty_Name,
	eEntityStringProperty_Tag,

	eEntityStringProperty_LastEnum
};

enum eSelectActionType
{
	eSelectActionType_Clear,
	eSelectActionType_Select,
	eSelectActionType_Deselect,
	eSelectActionType_Toggle,
};

//--------------------------------------------------------------------

//--------------------------------------------------------------------

////////////////////////////////////////////////////////////
// ENTITY CREATE ACTION
////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

class cEditorActionObjectCreate : public iEditorActionWorldModifier
{
public:
	cEditorActionObjectCreate(iEditorWorld* apEditorWorld, iEntityWrapperData* apData);
	~cEditorActionObjectCreate();

	void DoModify();
	void UndoModify();
protected:
	//////////////////////////////////
	// Data
	iEntityWrapperData* mpNewObjectData;
};

//--------------------------------------------------------------------

////////////////////////////////////////////////////////////
// ENTITY DELETE ACTION
////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

class cEditorActionEntityDelete : public iEditorActionWorldModifier
{
public:
	cEditorActionEntityDelete(iEditorWorld* apEditorWorld, cEditorSelection* apSelection, const tIntList& alstSelectedIDs);
	~cEditorActionEntityDelete();

	void DoModify();
	void UndoModify();
protected:
	//////////////////////////////////
	// Data
	cEditorSelection* mpSelection;

	tIntVec mvNewEntityIDs;
	tEntityDataVec mvEntityData;
};

//--------------------------------------------------------------------

////////////////////////////////////////////////////////////
// ENTITY CLONE ACTION
////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

class cEditorActionEntityClone : public iEditorActionWorldModifier
{
public:
	cEditorActionEntityClone(iEditorWorld* apEditorWorld, cEditorSelection* apSelection, const tIntList& alstSelectedIDs);

	void DoModify();
	void UndoModify();
protected:
	cEditorSelection* mpSelection;

	std::vector<tIntVec> mvClonedEntityIDs;
	tStringVec mvClonedEntityNames;
	tIntVec mvEntityIDs;
};

//--------------------------------------------------------------------

////////////////////////////////////////////////////////////
// ENTITY SELECT ACTION
////////////////////////////////////////////////////////////

//--------------------------------------------------------------

class cEditorActionEntitySelect : public iEditorAction
{
public:
	cEditorActionEntitySelect(cEditorEditModeSelect* apEditMode, const tIntList& alstEntityIDs, eSelectActionType aType);

	/////////////////////////////////////////
	// iEditorAction implementation
	void Do();
	void Undo();

protected:
	/////////////////////////////////////////
	// Data
	eSelectActionType mType;

	cEditorEditModeSelect* mpEditMode;
	iEditorWorld* mpEditorWorld;
	cEditorSelection* mpSelection;

	tIntList mlstNewSelectedEntityIDs;
	tIntList mlstOldSelectedEntityIDs;
};

//--------------------------------------------------------------

////////////////////////////////////////////////////////////
// ENTITY TRANSLATE ACTION
////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

class cEditorActionEntityTranslate : public iEditorActionWorldModifier
{
public:
	cEditorActionEntityTranslate(iEditorWorld* apEditorWorld, tIntList& alstEntityIDs, const cVector3f& avTranslate, bool abUseSnap, bool abRelativeTransform);

	void Do();
	void Undo();

protected:
	void Apply(tVector3fList& alstParam);

	tIntList mlstEntityIDs;

	tVector3fList mlstOldTranslations;
	tVector3fList mlstNewTranslations;
};

//--------------------------------------------------------------------

////////////////////////////////////////////////////////////
// ENTITY ROTATE ACTION
////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

class cEditorActionEntityRotate : public iEditorActionWorldModifier
{
public:
	cEditorActionEntityRotate(iEditorWorld* apEditorWorld, tIntList& alstEntityIDs, const cVector3f& avRotate, bool abRelativeTransform);

	void Do();
	void Undo();

protected:
	void Apply(tVector3fList& alstParam);

	tIntList mlstEntityIDs;

	tVector3fList mlstOldRotations;
	tVector3fList mlstNewRotations;
};

//--------------------------------------------------------------------

////////////////////////////////////////////////////////////
// ENTITY SCALE ACTION
////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

class cEditorActionEntityScale : public iEditorActionWorldModifier
{
public:
	cEditorActionEntityScale(iEditorWorld* apEditorWorld, tIntList& alstEntityIDs, const cVector3f& avScale, bool abRelativeTransform);

	void Do();
	void Undo();

protected:
	void Apply(tVector3fList& alstParam);

	tIntList mlstEntityIDs;

	tVector3fList mlstOldScale;
	tVector3fList mlstNewScale;
};

//--------------------------------------------------------------------

////////////////////////////////////////////////////////////
// ENTITY ATTACH CHILDREN ACTION
////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

class cEditorActionEntityAttachChildren : public iEditorActionWorldModifier
{
public:
	cEditorActionEntityAttachChildren(iEditorWorld* apEditorWorld, int alID, const tIntList& alstChildIDs);

	void DoModify();
	void UndoModify();
protected:
	int mlID;
	tIntList mlstOldChildIDs;
	tIntList mlstChildIDs;
	tIntList mlstChildOldParentIDs;
};

//--------------------------------------------------------------------

////////////////////////////////////////////////////////////
// ENTITY DETACH CHILDREN ACTION
////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

class cEditorActionEntityDetachChildren : public iEditorActionWorldModifier
{
public:
	cEditorActionEntityDetachChildren(iEditorWorld* apEditorWorld, int alID, tIntList& alstChildIDs);

	void DoModify();
	void UndoModify();
protected:
	int mlID;
	tIntList mlstChildIDs;
};

//--------------------------------------------------------------------

#endif // HPLEDITOR_EDITOR_ACTION_ENTITY_H
