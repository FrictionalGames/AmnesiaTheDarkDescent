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

#ifndef MODELEDITOR_EDITOR_WINDOW_ANIMATIONS_H
#define MODELEDITOR_EDITOR_WINDOW_ANIMATIONS_H

#include "../common/EditorWindow.h"
#include "ModelEditor.h"

class cModelEditor;
class cAnimationWrapper;

//-------------------------------------------------------------------------

class cModelEditorWindowAnimations : public iEditorWindowPopUp
{
public:
	cModelEditorWindowAnimations(cModelEditor* apEditor);

protected:
	void OnSetActive(bool abX);
	void OnInitLayout();
	void OnUpdate(float afTimeStep);

	bool InputCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputCallback);
	bool WindowSpecificInputCallback(iEditorInput* apInput);

	cModelEditor* mpEditor;

	bool CheckAnimNameAvailable(const tString& asName);

	void UpdateAnimInputs();
	void UpdateEventInputs();

	/////////////
	// Layout
	cWidgetButton* mpBOK;
	cWidgetButton* mpBCancel;

	cWidgetButton* mpBAddAnimation;
	cWidgetButton* mpBRemAnimation;

	cWidgetListBox* mpListAnimations;
	cEditorInputText* mpInpAnimName;
	cEditorInputFile* mpInpAnimFile;
	cEditorInputNumber* mpInpAnimSpeed;
	cEditorInputNumber* mpInpAnimSpecialEventTime;

	cWidgetButton* mpBAddEvent;
	cWidgetButton* mpBRemEvent;

	cEditorInputEnum* mpInpAnimEvents;

	cWidgetLabel* mpLEvent;
	cWidgetListBox* mpListEvents;
	cWidgetGroup* mpGEvent;
	cEditorInputNumber* mpInpEventTime;
	cEditorInputEnum* mpInpEventType;
	cEditorInputText* mpInpEventValue;

	std::vector<cAnimationWrapper> mvTempAnimations;
};

//-------------------------------------------------------------------------

#endif // MODELEDITOR_EDITOR_WINDOW_ANIMATIONS_H
