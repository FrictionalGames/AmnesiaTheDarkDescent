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

#include "gui/WidgetMenuItem.h"

#include "gui/Widget.h"
#include "gui/WidgetMenu.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"

#include "gui/GuiSet.h"
#include "gui/GuiSkin.h"

#include "graphics/FontData.h"

namespace hpl {

	//----------------------------------------------------------------

	//////////////////////////////////////////////////////////////////
	//	CONSTRUCTORS
	//////////////////////////////////////////////////////////////////

	//----------------------------------------------------------------

	cWidgetMenuItem::cWidgetMenuItem(iWidgetMenu* apParent) : iWidget(eWidgetType_MenuItem, apParent->GetSet(), apParent->GetSet()->GetSkin())
	{
		mbChecked = false;
		mItemType = apParent->GetType();

		mpChildMenu	= NULL;

		mfTimer = 0;
		mfOpenMenuTime = 0.5f;

		LoadGraphics();
	}

	cWidgetMenuItem::~cWidgetMenuItem()
	{
		if(mpSet->IsDestroyingSet()==false)
		{
			ClearMenuItems();
		}
	}

	//----------------------------------------------------------------

	//////////////////////////////////////////////////////////////////
	//	PUBLIC METHODS
	//////////////////////////////////////////////////////////////////

	//----------------------------------------------------------------

	cWidgetMenuItem*	cWidgetMenuItem::AddMenuItem( const tWString &asText, const tWString &asIconFilename )
	{
		//////////////////////////////////////////
		// Add a menu item. Create submenu if needed.
		if(mpChildMenu == NULL)
		{
			mpChildMenu = (iWidgetMenu*) mpSet->CreateWidgetContextMenu(0,0,_W(""), GetParentMenu());
			//mpChildMenu->SetEnabled(false);
			mpChildMenu->SetVisible(false);
			mpChildMenu->SetParentItem(this);
		}
		cWidgetMenuItem* pMenuItem = mpChildMenu->AddMenuItem(asText);

		return pMenuItem;
	}

	//----------------------------------------------------------------

	void cWidgetMenuItem::AddSeparator()
	{
		AddMenuItem(_W(""));
	}

	//----------------------------------------------------------------

	void cWidgetMenuItem::ClearMenuItems()
	{
		if(mpChildMenu)
		{
			mpSet->DestroyWidget(mpChildMenu);
			mpChildMenu=NULL;
		}
	}

	//----------------------------------------------------------------

	cGuiGlobalShortcut* cWidgetMenuItem::AddShortcut(int alKeyModifiers, eKey aKey, eGuiMessage aMsg, 
									  bool abBypassVisibility, bool abBypassEnabled)
	{
		cGuiGlobalShortcut* pShortcut = iWidget::AddShortcut(alKeyModifiers, aKey, eGuiMessage_ButtonPressed, true, false);

		if(msShortcutText!=_W(""))
			msShortcutText += _W(",");
		msShortcutText += cString::To16Char(mvShortcuts.back()->ToString());

		GetParentMenu()->UpdateMenuWidth(GetTextLength());

		return pShortcut;
	}

	//----------------------------------------------------------------

	float cWidgetMenuItem::GetTextLength()
	{
		const cVector2f& vFontSize = mpDefaultFont->mvSize;
		return mpDefaultFontType->GetLength(vFontSize, msText.c_str()) + 
			mpDefaultFontType->GetLength(vFontSize, msShortcutText.c_str());
	}

	//----------------------------------------------------------------

	//////////////////////////////////////////////////////////////////
	//	PROTECTED METHODS
	//////////////////////////////////////////////////////////////////

	//----------------------------------------------------------------

	bool cWidgetMenuItem::IsMenuOpen()
	{
		if(IsParent())
			return (mpChildMenu->IsVisible());

		return false;
	}

	//----------------------------------------------------------------

	void cWidgetMenuItem::ShowSubMenu()
	{
		if(mpChildMenu==NULL) return;

		///////////////////////////////////
		// Set up submenu new position
		cVector3f vChildPos = GetParentMenu()->GetGlobalPosition() + cVector3f(0,mvPosition.y,mvPosition.z);
		float fParentWidth = GetParentMenu()->GetSize().x;
		float fChildWidth = mpChildMenu->GetSize().x;

		////////////////////////////////
		// Show submenu according to item type
		switch(mItemType)
		{
		// If this Item belongs to a ContextMenu, pop submenu at one of the sides
		case eWidgetType_ContextMenu:
			
            if(vChildPos.x + fParentWidth + fChildWidth> mpSet->GetVirtualSize().x)
				vChildPos.x -= fChildWidth;
			else
				vChildPos.x += fParentWidth;

			break;
		// If this Item belongs to a MainMenu, pop submenu below
		case eWidgetType_MainMenu:
			vChildPos.x += mvPosition.x;
			vChildPos.y += mvSize.y;
			break;
		default:
			break;
		}
		//mpChildMenu->SetGlobalPosition(vChildPos);
		//mpChildMenu->SetEnabled(true);
		//mpChildMenu->SetVisible(true);
		mpSet->ShowContextMenu((cWidgetContextMenu*)mpChildMenu,vChildPos);		
	}
	//----------------------------------------------------------------

	void cWidgetMenuItem::OnInit()
	{
		float fY = 2*mfItemVPadding;
		if(IsSeparator())
			fY += mfSeparatorHeight;
		else
			fY += mpDefaultFont->mvSize.y;

		SetSize(cVector2f(GetParentMenu()->GetMenuItemWidth(),fY));
	}

	//----------------------------------------------------------------

	void cWidgetMenuItem::OnUpdate(float afTimeStep)
	{
		if(IsVisible() == false || (mpSet->GetAttentionWidget()!=NULL && IsConnectedTo(mpSet->GetAttentionWidget())==false))
			return;

		////////////////////////////////////////////
		// If context menu, increase timer
		if(GetMouseIsOver() && mItemType==eWidgetType_ContextMenu )
			mfTimer+=afTimeStep;
		/////////////////////////////////////////////
		// If item is highlighted, has submenu, and timer has right value, pop up submenu
		if(GetParentMenu()->GetHightlightedItem()==this && IsParent())
		{
			if(mfTimer>=mfOpenMenuTime && IsMenuOpen()==false)
			{
				ShowSubMenu();
			}
		}
	}

	//----------------------------------------------------------------

	void cWidgetMenuItem::OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion)
	{
		if(IsSeparator())
		{
			mpSet->DrawGfx(mpGfxLine, GetGlobalPosition() + cVector3f(mfSeparatorPadding,mvSize.y*0.5f-1,0.2f), 
								cVector2f(mvSize.x-2*mfSeparatorPadding, mfSeparatorHeight));
			return;
		}

		bool bHighlight = (GetParentMenu()->GetHightlightedItem()==this);
		bool bEnabled = IsEnabled();

		cColor col = bEnabled ? (bHighlight? mDefaultFontHighlightColor : mDefaultFontColor ) :
						mpSkin->GetFont(eGuiSkinFont_Disabled)->mColor;

		const cVector3f& vArrowOffset = GetParentMenu()->GetArrowOffset();
		const cVector3f& vCheckOffset = GetParentMenu()->GetCheckOffset();
		const cVector3f& vTextOffset = GetParentMenu()->GetTextOffset();
		
		//////////////////////////////////////////////
		// Draw the item according to the item type
		switch(mItemType)
		{
		// If this Item belongs to a ContextMenu
		case eWidgetType_ContextMenu:

			// Draw arrow icon if has submenu
			if(IsParent())
				mpSet->DrawGfx(mpGfxSubMenuArrow,GetGlobalPosition()+vArrowOffset);
			else
				DrawDefaultText(msShortcutText, GetGlobalPosition()+cVector3f(vArrowOffset.x,vTextOffset.y,vTextOffset.z), eFontAlign_Right, col);

			// Draw check icon if checked
			if(mbChecked)
				mpSet->DrawGfx(mpGfxChecked[mbEnabled], GetGlobalPosition() + vCheckOffset);

			break;
		// If this Item belongs to a MainMenu
		case eWidgetType_MainMenu:
			break;
		}

		////////////////////////////////////////////////////////
        // Draw item with highlight, this should happen when:
		//	- Pointer is over the item
		//	- this item's submenu is open and pointer is not over this item's menu			
		if(bHighlight)
		{
			// Draw Selection Bar
			mpSet->DrawGfx(mpGfxSelection, GetGlobalPosition()+cVector3f(0,0,0.1f), mvSize);					
		}
			
			
		DrawDefaultText(msText, GetGlobalPosition()+vTextOffset, eFontAlign_Left, col);
	}

	//----------------------------------------------------------------

	void cWidgetMenuItem::OnChangeSize()
	{
		/*
		SetArrowPos(cVector3f(mvSize.x - (mfItemHPadding + mfItemArrowIconSize),
												 mfItemArrowIconSize*0.5f,
												 0.2f));

		// Set text and check icon offsets, these should not change
		SetTextPos(cVector3f(	mfItemHPadding + mfItemCheckIconSize + mfItemTextLeftPadding,
								mfItemVPadding, 
								0.2f));
		SetCheckPos(cVector3f(	mfItemHPadding,
								mfItemVPadding,
								0.2f));
								*/
	}

	//----------------------------------------------------------------

	bool cWidgetMenuItem::OnMouseDown(const cGuiMessageData& aData)
	{
		if(IsSeparator()) return false;
		///////////////////////////
		// If clicked, skip timer
		mfTimer = mfOpenMenuTime;

		return true;
	}

	//----------------------------------------------------------------

	bool cWidgetMenuItem::OnMouseUp(const cGuiMessageData& aData)
	{
		if(IsSeparator()) return false;
		if(GetParentMenu()->GetHightlightedItem()!=this) return false;

		//////////////////////////////////////
		// If is "leaf" item, hide menu (completely if Context, all but topmost if Main)
		if(IsParent()==false)
			GetParentMenu()->GetTopMostMenu()->Hide();

		ProcessMessage(eGuiMessage_ButtonPressed, aData, true, true);

		return true;
	}

	//----------------------------------------------------------------

	bool cWidgetMenuItem::OnMouseEnter(const cGuiMessageData& aData)
	{
		//mfTimer = 0;

		return true;
	}

	//----------------------------------------------------------------

	bool cWidgetMenuItem::OnMouseMove(const cGuiMessageData& aData)
	{
		if(IsVisible()==false || 
			mpSet->GetDrawMouse()==false || 
			(mpSet->GetAttentionWidget()!=NULL && IsConnectedTo(mpSet->GetAttentionWidget())==false))
			return true;

		if(GetMouseIsOver())
			GetParentMenu()->SetHighlightedItem(this);
		return true;
	}

	//----------------------------------------------------------------

	bool cWidgetMenuItem::OnMouseLeave(const cGuiMessageData& aData)
	{
		if(mpSet->GetAttentionWidget()!=NULL && 
			IsConnectedTo(mpSet->GetAttentionWidget())==false) return false;

		if(IsMenuOpen()==false)
			GetParentMenu()->SetHighlightedItem(NULL);

		mfTimer = 0;

		return true;
	}

	//----------------------------------------------------------------
	
	bool cWidgetMenuItem::OnKeyPress(const cGuiMessageData& aData)
	{
		eKey key = aData.mKeyPress.mKey;

		if(IsParent())
			return mpChildMenu->ProcessMessage(eGuiMessage_KeyPress, aData);

		return false;
	}

	//----------------------------------------------------------------

	void cWidgetMenuItem::OnLoadGraphics()
	{
		mpGfxSelection = mpSkin->GetGfx(eGuiSkinGfx_TextBoxSelectedTextBack);
		mpGfxSubMenuArrow = mpSkin->GetGfx(eGuiSkinGfx_MenuItemArrow);

		mpGfxChecked[0] = mpSkin->GetGfx(eGuiSkinGfx_MenuDisabledChecked); 
		mpGfxChecked[1] = mpSkin->GetGfx(eGuiSkinGfx_MenuEnabledChecked);

		mpGfxLine = mpSkin->GetGfx(eGuiSkinGfx_MenuSeparatorLine);

		mfSeparatorPadding = mpSkin->GetAttribute(eGuiSkinAttribute_MenuSeparatorPadding).x;
		mfSeparatorHeight = mpSkin->GetAttribute(eGuiSkinAttribute_MenuSeparatorHeight).x;
		mfItemHPadding = mpSkin->GetAttribute(eGuiSkinAttribute_ContextMenuItemHPadding).x;
		mfItemVPadding = mpSkin->GetAttribute(eGuiSkinAttribute_ContextMenuItemVPadding).x;
		mfItemSeparation = mpSkin->GetAttribute(eGuiSkinAttribute_ContextMenuItemSeparation).x;
		mfItemArrowIconSize = mpSkin->GetAttribute(eGuiSkinAttribute_ContextMenuItemArrowSize).x;
		mfItemCheckIconSize = mpSkin->GetAttribute(eGuiSkinAttribute_ContextMenuItemCheckSize).x;
		mfItemTextLeftPadding = mpSkin->GetAttribute(eGuiSkinAttribute_ContextMenuItemTextLeftPadding).x;
		mfItemTextRightPadding = mpSkin->GetAttribute(eGuiSkinAttribute_ContextMenuItemTextRightPadding).x;

		SetDefaultFontSize(12.5f);
	}

	//----------------------------------------------------------------

	bool cWidgetMenuItem::OnLostFocus(const cGuiMessageData& aData)
	{
		////////////////////////////////////////
		// If loses focus, try to hide topmost menu
		//GetParentMenu()->GetTopMostMenu()->SetMustHide(true);
		return true;
	}

	//----------------------------------------------------------------

	bool cWidgetMenuItem::OnGotFocus(const cGuiMessageData& aData)
	{
		////////////////////////////////////////
		// If gets focus, keep topmost menu up
		//GetParentMenu()->GetTopMostMenu()->SetMustHide(false);
		return true;
	}

	//----------------------------------------------------------------

};
