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

#ifndef HPLEDITOR_EDITOR_ACTION_SELECTION_H
#define HPLEDITOR_EDITOR_ACTION_SELECTION_H

#include "../common/StdAfx.h"
using namespace hpl;

#include "EditorAction.h"

//--------------------------------------------------------------

class iEditorWorld;
class cEditorSelection;

class cEditorEditModeSelect;

//--------------------------------------------------------------

class cEditorActionSelectionTranslate : public iEditorActionWorldModifier
{
public:
	cEditorActionSelectionTranslate(cEditorEditModeSelect* apEditMode,
									const cVector3f& avTranslate,
									bool abUseSnap);

	void DoModify();

	void UndoModify();

protected:
	cEditorEditModeSelect* mpEditMode;

	cEditorSelection* mpSelection;

	cVector3f mvTranslate;
	tVector3fVec mvEntityOldPos;

	bool mbUseSnap;
};

//--------------------------------------------------------------

class cEditorActionSelectionRotate : public iEditorActionWorldModifier
{
public:
	cEditorActionSelectionRotate(cEditorEditModeSelect* apEditMode,
									iEditorWorld* apEditorWorld,
									const cVector3f& avRotate,
									bool abUseSnap,
									bool abRelativeTransform = true);

	void DoModify();

	void UndoModify();

protected:
	cEditorEditModeSelect* mpEditMode;

	cEditorSelection* mpSelection;

	cVector3f mvOldRotation;

	cVector3f mvRotate;
	tVector3fVec mvEntityOldRot;

	bool mbUseSnap;
	bool mbRelativeTransform;
};

//--------------------------------------------------------------

class cEditorActionSelectionScale : public iEditorActionWorldModifier
{
public:
	cEditorActionSelectionScale(cEditorEditModeSelect* apEditMode,
								iEditorWorld* apEditorWorld,
								const cVector3f& avScale,
								bool abUseSnap);

	void DoModify();

	void UndoModify();

protected:
	cEditorEditModeSelect* mpEditMode;

	cEditorSelection* mpSelection;

	cVector3f mvScale;
	tVector3fVec mvEntityOldScale;

	bool mbUseSnap;
};

//--------------------------------------------------------------

#endif // HPLEDITOR_EDITOR_ACTION_SELECTION_TRANSLATE_H
