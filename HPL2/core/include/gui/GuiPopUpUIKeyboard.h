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

#ifndef HPL_GUI_POP_UP_UI_KEYBOARD_H
#define HPL_GUI_POP_UP_UI_KEYBOARD_H

#include "gui/GuiPopUp.h"

namespace hpl {

	class cGuiSet;
	class cGuiSkin;

	class cWidgetWindow;
	class cWidgetButton;
	class cWidgetTextBox;
	class cWidgetFrame;
	class cWidgetSlider;
	class cWidgetLabel;

	//---------------------------------------------------------------

	class cUIKey
	{
	public:
		cUIKey(eKey aKey, int alUnicodeLower, int alUnicodeUpper) : mKey(aKey) 
		{ mvUnicode[0]=alUnicodeLower; mvUnicode[1]=alUnicodeUpper; }

		eKey mKey;
		int mvUnicode[2];
	};

	//---------------------------------------------------------------

	typedef std::list<cUIKey>		tUIKeyList;
	typedef tUIKeyList::iterator	tUIKeyListIt;

	//---------------------------------------------------------------

	class cGuiPopUpUIKeyboard : public iGuiPopUp
	{
	public:
		cGuiPopUpUIKeyboard(cWidgetTextBox* apTargetTextBox, void *apCallbackObject, tGuiCallbackFunc apCallback);
		virtual ~cGuiPopUpUIKeyboard();		

	protected:
		/////////////////////////////
		// Own functions
		void SetUpKey(iWidget* apKeyWidget, eKey aKey, int alUnicode, bool abShift=true);

		bool Key_OnPress(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(Key_OnPress);

		bool Enter_OnPress(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(Enter_OnPress);

		bool Cancel_OnPress(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(Cancel_OnPress);

		bool Button_Pressed(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(Button_Pressed);

		bool WindowClose(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(WindowClose);

		bool Window_OnUIButtonPress(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(Window_OnUIButtonPress);

		bool Window_OnUIButtonRelease(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(Window_OnUIButtonRelease);
		
		void Init();

		void ClosePopUp();

		void SetShiftActive(bool abX);

		//////////////////////////////
		// Data
		cVector3f mvPos;

		cWidgetTextBox* mpTargetTextBox;
		tWString msBackUpText;

		tWidgetList mlstKeyWidgets;		
		tUIKeyList mlstKeys;

		bool mbShift;
	};
};

#endif // HPL_GUI_POP_UP_UI_KEYBOARD_H
