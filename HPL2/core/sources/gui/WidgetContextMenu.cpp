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

#include "gui/WidgetContextMenu.h"
#include "gui/GuiSet.h"

#include "gui/WidgetMenuItem.h"
#include "gui/GuiGfxElement.h"

#include "graphics/FontData.h"

#include "system/LowLevelSystem.h"



namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	cWidgetContextMenu::cWidgetContextMenu(cGuiSet *apSet, cGuiSkin *apSkin) : iWidgetMenu(eWidgetType_ContextMenu,apSet,apSkin)
	{
		mfTimer = 0;
		mfHideTime = 0;
	}

	cWidgetContextMenu::~cWidgetContextMenu()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cWidgetContextMenu::AddSeparator()
	{
		AddMenuItem(_W(""));
	}

	//-----------------------------------------------------------------------

	void cWidgetContextMenu::Hide()
	{
		//////////////////////////////////////////
		// This does the following:
		// - Closes any submenu that is being shown at the moment
		// - Resets the selection
		// - If this is the topmost menu, clear the AttentionWidget
		// - And hides the menu

		if(IsSubmenuOpen()) 
			mpHighlightedItem->GetChildMenu()->Hide();

        SetHighlightedItem(NULL);

		if(GetParentMenu())
			GetParentMenu()->OnChildMenuHide();
		else
		{
			mpSet->SetAttentionWidget(mpPrevAttention);
			mpSet->DecContextMenuZ();
		}

		//SetEnabled(false);
		SetVisible(false);

		// Reset Timer
		mfTimer = 0;
	}


	//-----------------------------------------------------------------------

	void cWidgetContextMenu::SavePrevAttentionWidget()
	{
		mpPrevAttention = mpSet->GetAttentionWidget();
	}

	//-----------------------------------------------------------------------

	float cWidgetContextMenu::GetMenuItemWidth()
	{
		float fLeftBorderWidth = mvGfxBorders[0]->GetImageSize().x;
		float fRightBorderWidth = mvGfxBorders[1]->GetImageSize().x;
		
		return mvSize.x - (fLeftBorderWidth+fRightBorderWidth);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cWidgetContextMenu::UpdateMenuItemsPos(cWidgetMenuItem* apNewItem)
	{
		float fUpperBorderHeight = mvGfxBorders[2]->GetImageSize().y;
		float fLeftBorderWidth = mvGfxBorders[0]->GetImageSize().x;

		float fMenuItemY = mvSize.y;
		if(mvMenuItems.size()>1)
			fMenuItemY -= fUpperBorderHeight;
		else
			fMenuItemY += fUpperBorderHeight;

		apNewItem->SetPosition(cVector3f(fLeftBorderWidth, fMenuItemY, 0.1f));
	}

	//-----------------------------------------------------------------------

	void cWidgetContextMenu::UpdateMenuItemsSize(cWidgetMenuItem* apNewItem)
	{
		float fTextLength = apNewItem->GetTextLength();

		UpdateMenuWidth(fTextLength);

		///////////////////////////////////////////////////////////////////////
		// Update actual ContextMenu height
		float fSeparation = 0;
		size_t lNumItems = mvMenuItems.size();

		if(lNumItems>1)
		{
			cWidgetMenuItem *pPreviousItem = mvMenuItems[lNumItems-2];
			fSeparation = (pPreviousItem->IsSeparator()?0:(apNewItem->IsSeparator()?0:mfItemSeparation));
		}
		else
		{
			SetSize(cVector2f(mvSize.x,mvGfxBorders[2]->GetImageSize().y + mvGfxBorders[3]->GetImageSize().y));
		}


		SetSize(cVector2f(mvSize.x, mvSize.y+apNewItem->GetSize().y+fSeparation) );
	}

	//-----------------------------------------------------------------------

	void cWidgetContextMenu::UpdateMenuWidth(float afItemTextWidth)
	{
		float fItemWidth = GetMenuItemWidth();

		// This computes the width of all non text items
		float fNonTextWidth =	mfItemHPadding + mfItemCheckIconSize + mfItemTextLeftPadding +  
								mfItemTextRightPadding + mfItemArrowIconSize + mfItemHPadding;

		/////////////////////////////////////////////////////////////////
		// Update MenuItem now. This also sets up check and arrow icons and text offset

		// Check if all items need to be updated with new Width (if text length is greater than the reserved width...)
		if(afItemTextWidth > fItemWidth-fNonTextWidth) 
		{
			fItemWidth = afItemTextWidth + fNonTextWidth;
			
			for(tWidgetMenuItemVectorIt it = mvMenuItems.begin(); it != mvMenuItems.end(); ++it)
			{
				cWidgetMenuItem* pItem = *it;

               	pItem->SetSize(cVector2f(fItemWidth, pItem->GetSize().y) );
			}
		}

		float fMenuWidth = fItemWidth + mvGfxBorders[0]->GetImageSize().x + mvGfxBorders[1]->GetImageSize().x;
		SetSize(cVector2f(fMenuWidth, mvSize.y));

		SetArrowPos(cVector3f(fItemWidth - (mfItemHPadding + mfItemArrowIconSize),
												 mfItemArrowIconSize*0.5f,
												 0.2f));

		// Set text and check icon offsets, these should not change
		SetTextPos(cVector3f(	mfItemHPadding + mfItemCheckIconSize + mfItemTextLeftPadding,
								mfItemVPadding, 
								0.2f));
		SetCheckPos(cVector3f(	mfItemHPadding,
								mfItemVPadding,
								0.2f));
	}

	//-----------------------------------------------------------------------

	void cWidgetContextMenu::OnChildMenuHide()
	{
		mpSet->SetFocusedWidget(this);
	}

	//-----------------------------------------------------------------------
    
	void cWidgetContextMenu::OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion)
	{
		////////////////////////////////
		// Background and borders
		DrawBordersAndCorners(	 mpGfxBackground, mvGfxBorders, mvGfxCorners, 
								GetGlobalPosition(),
								mvSize);

	}

	//-----------------------------------------------------------------------

	void cWidgetContextMenu::OnUpdate(float afTimeStep)
	{
		////////////////////////////////////////////////
		// Checks if should hide (ie if parent menu highlights some item other 
		// than this menu's parent item, or if SetMustHide(true) has been called)

		if(IsVisible() && mpParentItem!=NULL && GetParentMenu()->GetHightlightedItem()!=mpParentItem)
		{
			mfTimer+=afTimeStep;
			if(mfTimer>=mfHideTime)
			{
				Hide();
			}
		}

		if(mbMustHide)
		{
			Hide();
			mbMustHide=false;
		}
	}

	//-----------------------------------------------------------------------

	void cWidgetContextMenu::OnInit()
	{
		SetVisible(false);
		//SetEnabled(false);
	}

	//-----------------------------------------------------------------------

	bool cWidgetContextMenu::OnMouseEnter(const cGuiMessageData& aData)
	{
     	return true;
	}

	//-----------------------------------------------------------------------

	bool cWidgetContextMenu::OnMouseLeave(const cGuiMessageData& aData)
	{
		if(IsSubmenuOpen()==false)
			SetHighlightedItem(NULL);
        return true;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////
	// These should work as follows (needs Oldwidget->OnLostFocus, NewWidget->OnGotFocus order to work):
	// - If a Context menu or submenu loses focus, try to hide topmost (would hide all submenus)
	// - If next focused widget belongs to the Context menu, keep it enabled ( by revoking last OnLostFocus action )
	// - If next focused widget doesnt belong to the menu, this will be closed

	bool cWidgetContextMenu::OnLostFocus(const cGuiMessageData& aData)
	{
		iWidget* pFocus = mpSet->GetFocusedWidget();
		if(pFocus==NULL || IsConnectedTo(pFocus, false)==false && pFocus->IsConnectedTo(this, false)==false)
			GetTopMostMenu()->SetMustHide(true);

		return true;
	}

	bool cWidgetContextMenu::OnGotFocus(const cGuiMessageData& aData)
	{
		mfTimer=0;

		return true;
	}

	//-----------------------------------------------------------------------

	void	cWidgetContextMenu::OnLoadGraphics()
	{
		mpGfxBackground = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpBackground);

		mvGfxBorders[0] = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxBorderRight);
		mvGfxBorders[1] = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxBorderLeft);
		mvGfxBorders[2] = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxBorderUp);
		mvGfxBorders[3] = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxBorderDown);

		mvGfxCorners[0] = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxCornerLU);
		mvGfxCorners[1] = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxCornerRU);
		mvGfxCorners[2] = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxCornerRD);
		mvGfxCorners[3] = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxCornerLD);

		mfItemHPadding = mpSkin->GetAttribute(eGuiSkinAttribute_ContextMenuItemHPadding).x;
		mfItemVPadding = mpSkin->GetAttribute(eGuiSkinAttribute_ContextMenuItemVPadding).x;
		mfItemSeparation = mpSkin->GetAttribute(eGuiSkinAttribute_ContextMenuItemSeparation).x;
		mfItemArrowIconSize = mpSkin->GetAttribute(eGuiSkinAttribute_ContextMenuItemArrowSize).x;
		mfItemCheckIconSize = mpSkin->GetAttribute(eGuiSkinAttribute_ContextMenuItemCheckSize).x;
		mfItemTextLeftPadding = mpSkin->GetAttribute(eGuiSkinAttribute_ContextMenuItemTextLeftPadding).x;
		mfItemTextRightPadding = mpSkin->GetAttribute(eGuiSkinAttribute_ContextMenuItemTextRightPadding).x;
	}

	//-----------------------------------------------------------------------

	bool cWidgetContextMenu::OnKeyPress(const cGuiMessageData& aData)
	{
		if(aData.mKeyPress.mKey==eKey_Escape)
		{
			Hide();
			return true;
		}

		for(int i=0;i<(int)mvMenuItems.size();++i)
		{
			if(mvMenuItems[i]->ProcessMessage(eGuiMessage_KeyPress, aData))
				break;
		}
		return true;
	}

	//-----------------------------------------------------------------------

};
