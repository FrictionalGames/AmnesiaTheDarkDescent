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

#ifndef HPL_GUI_POP_UP_COLOR_PICKER_H
#define HPL_GUI_POP_UP_COLOR_PICKER_H

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

	//---------------------------------------------------------------

	class cGuiPopUpColorPicker : public iGuiPopUp
	{
	public:
		cGuiPopUpColorPicker(cGuiSet* apSet, cGuiSkin* apSkin,const cVector3f& avPos,cColor* apDestColor, void *apCallbackObject, tGuiCallbackFunc apCallback);
		virtual ~cGuiPopUpColorPicker();		

	protected:
		/////////////////////////////
		// Own functions
		bool Button_Pressed(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(Button_Pressed);

		bool Slider_OnMove(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(Slider_OnMove);

		bool Input_OnTextBoxEnter(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(Input_OnTextBoxEnter);

		bool WindowClose(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(WindowClose);

		void Init();

		void ClosePopUp();

		void UpdateSliders();
		void UpdateInputs();		

		//////////////////////////////
		// Data
		cVector3f mvPos;

		cColor mOldColor;
		cColor mColor;
		cColor* mpDestColor;

		cWidgetFrame* mpFrameCurrentColor;
		cWidgetFrame* mpFramePreviousColor;

		cWidgetLabel* mvLabelColorComponent[4];
		cWidgetSlider* mvSliderColorComponent[4];
		cWidgetTextBox* mvInputColorComponent[4];

		cWidgetButton* mvButtons[3];

		void * mpCallbackObject;
		tGuiCallbackFunc mpCallback;	
	};
};

#endif // HPL_GUI_POP_UP_COLOR_PICKER_H
