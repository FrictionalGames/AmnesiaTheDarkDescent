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

#ifndef HPL_WIDGET_COMBO_BOX_H
#define HPL_WIDGET_COMBO_BOX_H

#include "gui/Widget.h"
#include "gui/WidgetSlider.h"
#include "gui/WidgetBaseClasses.h"


namespace hpl {

	class cGuiSkinFont;
	
	class cWidgetTextBox;
	class cWidgetButton;
	class cWidgetSlider;

	class cWidgetComboBox : public iWidget, public iWidgetItemContainer
	{
	public:
		cWidgetComboBox(cGuiSet *apSet, cGuiSkin *apSkin);
		virtual ~cWidgetComboBox();

		void SetSelectedItem(int alX,bool abMoveList=false, bool abGenCallback=true);
		int GetSelectedItem(){ return mlSelectedItem;}

		void SetCanEdit(bool abX);
		bool GetCanEdit();

		void SetMaxShownItems(int alX);
		int GetMaxShownItems(){ return mlMaxItems;}

		void SetDefaultFontSize(const cVector2f& avSize);
		
	protected:
		/////////////////////////
		// Own functions
		void UpdateProperties();

		void OpenMenu();
		void CloseMenu();

		bool ButtonPress(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(ButtonPress);

		bool DrawText(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(DrawText);	

		bool SliderMove(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(SliderMove);

		bool SliderLostFocus(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(SliderLostFocus);

		/////////////////////////
		// Implemented functions
		void OnLoadGraphics();
		void OnChangeSize();
		void OnChangeText();
		void OnInit();
		
		void OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion);

		bool OnMouseMove(const cGuiMessageData& aData);
		bool OnMouseDown(const cGuiMessageData& aData);
		bool OnMouseUp(const cGuiMessageData& aData);
		bool OnMouseEnter(const cGuiMessageData& aData);
		bool OnMouseLeave(const cGuiMessageData& aData);

		bool OnUIArrowPress(const cGuiMessageData& aData);

		bool OnUIButtonPress(const cGuiMessageData& aData);
		bool OnUIButtonRelease(const cGuiMessageData& aData);
		
		bool OnLostFocus(const cGuiMessageData& aData);

		bool PointIsInside(const cVector2f& avPoint, bool abOnlyClipped);

		bool SliderOnUIArrowPress(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(SliderOnUIArrowPress);

		bool SliderOnUIButtonPress(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(SliderOnUIButtonPress);

		bool SliderOnUIButtonRelease(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(SliderOnUIButtonRelease);

		/////////////////////////
		// Data
		
		cWidgetTextBox *mpText;
		cWidgetButton *mpButton;
		cWidgetSlider *mpSlider;

		bool mbMenuOpen;
		bool mbClickedAfterOpen;
		float mfMenuHeight;
		cVector3f mvMenuPos;

		float mfButtonWidth;
		float mfSliderWidth;

		int mlMouseOverSelection;
		int mlSelectedItem;
		int mlFirstItem;
		int mlMaxItems;
		int mlItemsShown;

		cGuiGfxElement *mpGfxBackground;

		cGuiGfxElement *mpGfxSelection;
		
		cGuiGfxElement *mvGfxBorders[4];
		cGuiGfxElement *mvGfxCorners[4];
		
		bool mbOpenByUIButton;
	};

};
#endif // HPL_WIDGET_COMBO_BOX_H
