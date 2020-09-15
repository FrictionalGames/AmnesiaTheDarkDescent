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

#ifndef ED_WINDOW_ANIMATIONS_H
#define ED_WINDOW_ANIMATIONS_H

#include "../Common/EdWindowHandler.h"
#include "../ModelEditor/HPLEntity.h"

//-------------------------------------------------------------------------

class cEdWindowAnimations : public iEdPopUp
{
public:
	cEdWindowAnimations(iEditor* apEditor);

protected:
	void OnSetActive(bool abX);
	void OnCreateLayout();
	void OnUpdate();

	bool InputCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputCallback);
	bool WindowSpecificInputCallback(iEdInput* apInput);

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
	cEdInputText* mpInpAnimName;
	cEdInputFile* mpInpAnimFile;
	cEdInputNumber* mpInpAnimSpeed;
	cEdInputNumber* mpInpAnimSpecialEventTime;

	cWidgetButton* mpBAddEvent;
	cWidgetButton* mpBRemEvent;

	cEdInputEnum* mpInpAnimEvents;

	cWidgetLabel* mpLEvent;
	cWidgetListBox* mpListEvents;
	cWidgetGroup* mpGEvent;
	cEdInputNumber* mpInpEventTime;
	cEdInputEnum* mpInpEventType;
	cEdInputText* mpInpEventValue;

	tAnimWrapperList mlstTempAnimations;
};

//-------------------------------------------------------------------------

#endif // ED_WINDOW_ANIMATIONS_H
