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

#ifndef HPL_WIDGET_MENU_ITEM
#define HPL_WIDGET_MENU_ITEM

#include "gui/Widget.h"

#include "input/InputTypes.h"

namespace hpl {


	class iWidgetMenu;

	class cGuiSet;
	class cGuiSkin;



	//----------------------------

	class cWidgetMenuItem : public iWidget
	{
	public:
		cWidgetMenuItem(iWidgetMenu* apParent);
		virtual ~cWidgetMenuItem();

		cWidgetMenuItem* AddMenuItem(const tWString &asText, const tWString &asIconFilename = _W(""));
		void AddSeparator();
		void ClearMenuItems();

		bool IsParent() { return (mpChildMenu != NULL); }
		bool IsChecked()	{ return mbChecked; }
		void SetChecked(bool abX) { mbChecked = abX; }
		bool IsDefault()	{ return mbDefault; }
		void SetDefault(bool abX) { mbDefault = abX; }
		
		iWidgetMenu*	GetParentMenu()	{ return (iWidgetMenu*)mpParent; }
		iWidgetMenu*	GetChildMenu() { return mpChildMenu; }

		bool IsMenuOpen();

		bool IsSeparator() { return (msText==_W(""));}

		cGuiGlobalShortcut* AddShortcut(int alKeyModifiers, eKey aKey, eGuiMessage aMsg=eGuiMessage_ButtonPressed, 
						 bool abBypassVisibility=true, bool abBypassEnabled=true);

		const tWString& GetShortcutText() { return msShortcutText; }

		float GetTextLength();

	protected:

		/////////////////////////
		// Own functions
		void ShowSubMenu();
		/////////////////////////
		// Implemented functions
		void OnInit();
		void OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion);
		void OnUpdate(float afTimeStep);
		
		void OnChangeSize();


		bool OnMouseDown(const cGuiMessageData& aData);
		bool OnMouseUp(const cGuiMessageData& aData);
		bool OnMouseEnter(const cGuiMessageData& aData);
		bool OnMouseLeave(const cGuiMessageData& aData);

		bool OnMouseMove(const cGuiMessageData& aData);

		bool OnKeyPress(const cGuiMessageData& aData);

		bool OnLostFocus(const cGuiMessageData& aData);
		bool OnGotFocus(const cGuiMessageData& aData);

		void OnLoadGraphics();


		//////////////////////////
		// Data
		eWidgetType mItemType;

		bool	mbChecked;
		bool	mbDefault;

		float	mfTimer;
		float	mfOpenMenuTime;

		bool	mbPressed;

		iWidgetMenu*	mpChildMenu;

		cGuiGfxElement*	mpGfxSelection;
		cGuiGfxElement*	mpGfxSubMenuArrow;
		cGuiGfxElement* mpGfxChecked[2];

		cGuiGfxElement* mpGfxLine;

		float mfItemHPadding;
		float mfItemTextLeftPadding;
		float mfItemTextRightPadding;
		float mfItemVPadding;
		float mfItemSeparation;
		float mfItemArrowIconSize;
		float mfItemCheckIconSize;

		float mfSeparatorPadding;
		float mfSeparatorHeight;

		tWString msShortcutText;
	};

	
};

#endif	// HPL_WIDGET_MENU_ITEM
