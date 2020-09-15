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

#ifndef HPL_WIDGET_CHECK_BOX_H
#define HPL_WIDGET_CHECK_BOX_H

#include "gui/Widget.h"

namespace hpl {

	class cGuiSkinFont;
	class cWidgetLabel;

	class cWidgetCheckBox : public iWidget
	{
	public:
		cWidgetCheckBox(cGuiSet *apSet, cGuiSkin *apSkin);
		virtual ~cWidgetCheckBox();

		void SetChecked(bool abX, bool abGenCallback=true);
		bool IsChecked(){return mbChecked;}

		void SetDefaultFontType(iFontData *apFont);
		void SetDefaultFontColor(const cColor& aColor);
		void SetDefaultFontSize(const cVector2f& avSize);
	
	protected:
		/////////////////////////
		// Own functions
		bool Label_MouseDown(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(Label_MouseDown);
		bool Label_MouseUp(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(Label_MouseUp);
        
		void UpdateLabel();

		/////////////////////////
		// Implemented functions
		void OnInit();
		void OnLoadGraphics();
		void OnChangeSize();
		void OnChangeText();
		
		void OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion);

		bool OnMouseMove(const cGuiMessageData& aData);
		bool OnMouseDown(const cGuiMessageData& aData);
		bool OnMouseUp(const cGuiMessageData& aData);
		bool OnMouseEnter(const cGuiMessageData& aData);
		bool OnMouseLeave(const cGuiMessageData& aData);

		bool OnUIButtonPress(const cGuiMessageData& aData);
		bool OnUIButtonRelease(const cGuiMessageData& aData);

		/////////////////////////
		// Data
		bool mbChecked;
		bool mbPressed;

		cGuiGfxElement *mvGfxBox[2][2];

		cVector2f mvBoxSize;

		cWidgetLabel	*mpLabel;
		
	};

};
#endif // HPL_WIDGET_CHECK_BOX_H
