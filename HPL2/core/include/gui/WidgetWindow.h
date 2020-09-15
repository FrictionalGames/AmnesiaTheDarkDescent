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

#ifndef HPL_WIDGET_WINDOW_H
#define HPL_WIDGET_WINDOW_H

#include <list>
#include <vector>

#include "gui/Widget.h"

namespace hpl {

	class cGuiSkinFont;
	class cWidgetButton;


	class cWidgetWindow : public iWidget
	{
	public:
		cWidgetWindow(cGuiSet *apSet, cGuiSkin *apSkin, tWidgetWindowButtonFlag alFlags = 0);
		virtual ~cWidgetWindow();

		void SetStatic(bool abX);
		bool GetStatic(){return mbStatic;}

		void SetDrawLabel(bool abX);
		
		void SetCloseButtonDisablesWindow(bool abX){mbCloseButtonDisablesWindow = abX;}
		bool GetCloseButtonDisablesWindow(){ return mbCloseButtonDisablesWindow;}

		void SetEscapeKeyClosesWindow(bool abX) { mbCloseOnEscapeKey = abX; }
		bool GetEscapeKeyClosesWindow() { return mbCloseOnEscapeKey; }
		
	protected:

		/////////////////////////
		// Window Button Callbacks
		bool ButtonPressed(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(ButtonPressed);

		/////////////////////////
		// Implemented functions
		void OnInit();
		void OnLoadGraphics();
		void OnAttachChild(iWidget *apChild);

		void OnChangeSize();
		
		void OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion);

		bool OnMouseMove(const cGuiMessageData& aData);
		bool OnMouseDown(const cGuiMessageData& aData);
		bool OnMouseUp(const cGuiMessageData& aData);
		bool OnMouseEnter(const cGuiMessageData& aData);
		bool OnMouseLeave(const cGuiMessageData& aData);

		bool OnKeyPress(const cGuiMessageData& aData);

		bool OnGotFocus(const cGuiMessageData& aData);
		bool OnLostFocus(const cGuiMessageData& aData);

		void OnChildGotFocus(iWidget* apChild, const cGuiMessageData& aData);
		void OnChildLostFocus(iWidget* apChild, const cGuiMessageData& aData);

		/////////////////////////
		// Data
		cGuiSkinFont *mpLabelFont;

		cWidgetButton* mvButtons[1];

		cGuiGfxElement *mpGfxBackground;
		cGuiGfxElement *mpGfxLabel;

		cGuiGfxElement *mvGfxBorders[4];
		cGuiGfxElement *mvGfxCorners[4];

		cGuiGfxElement *mpGfxButtonCross;

		cVector3f mvLabelTextOffset;

		tWidgetWindowButtonFlag mlFlags;

		bool mbStatic;
		bool mbCloseButtonDisablesWindow;
		bool mbCloseOnEscapeKey;

		bool mbDrawLabel;
		bool mbFocused;

		cVector3f mvRelMousePos;
		bool mbMoving;
		iWidget* mpPrevAttention;
		cWidgetWindow* mpPrevTopMostWindow;

		float mfButtonSize;

		bool mbDrawBordersOutside;
	};

};
#endif // HPL_WIDGET_H
