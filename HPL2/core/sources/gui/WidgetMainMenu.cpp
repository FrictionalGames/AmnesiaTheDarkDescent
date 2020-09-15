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

#include "gui/WidgetMainMenu.h"
#include "gui/GuiSet.h"

#include "gui/WidgetMenuItem.h"

#include "graphics/FontData.h"

#include "system/LowLevelSystem.h"



namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	cWidgetMainMenu::cWidgetMainMenu(cGuiSet *apSet, cGuiSkin *apSkin) : iWidgetMenu(eWidgetType_MainMenu,apSet,apSkin)
	{
		LoadGraphics();
	}

	cWidgetMainMenu::~cWidgetMainMenu()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cWidgetMainMenu::UpdateMenuItemsPos(cWidgetMenuItem* apNewItem)
	{
		cVector3f vPos = cVector3f(3,mfItemVPadding,1);
		
		size_t lNumItems = mvMenuItems.size();

		/////////////////////////////////////
		// If not the first item added, get previous one lentgh and pos,
		// and set it as new item's position.
		if(lNumItems > 1)
		{
			vPos.x += mvMenuItems[lNumItems-2]->GetLocalPosition().x;
			vPos.x += mvMenuItems[lNumItems-2]->GetSize().x;
			vPos.x += mfItemSeparation;
		}

		apNewItem->SetPosition(vPos);
	}

	//-----------------------------------------------------------------------

	void cWidgetMainMenu::UpdateMenuItemsSize(cWidgetMenuItem* apNewItem)
	{
		//////////////////////////////////////
		// Set item text length as items size 
		float fLength = apNewItem->GetTextLength();

        apNewItem->SetSize(cVector2f(fLength + 2*mfItemTextPadding, mpDefaultFont->mvSize.y + 2*mfItemVPadding));

		// Also set text offset
		SetTextPos(cVector3f(mfItemTextPadding,mfItemVPadding,0.2f));
	}

	//-----------------------------------------------------------------------

	void cWidgetMainMenu::OnChildMenuHide()
	{
		mpSet->SetAttentionWidget(mpPrevAttention);
	}
    
	//-----------------------------------------------------------------------

	void cWidgetMainMenu::Hide()
	{
		///////////////////////////////////////////////////
		// In MainMenu, topmost menu should never be hidden, so hide submenues only
		if(IsSubmenuOpen())
		{
			mpHighlightedItem->GetChildMenu()->Hide();
			mpHighlightedItem = NULL;
		}

		mbMustHide = false;
	}

	//-----------------------------------------------------------------------

	void cWidgetMainMenu::OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion)
	{
		////////////////////////////////
		// Draw background 
		mpSet->DrawGfx( mpGfxBackground,GetGlobalPosition(),mvSize-cVector2f(0,1));
		//mpSet->DrawGfx( mpGfxBackground, GetGlobalPosition()+cVector3f(0,mvSize.y-1,0.05f), cVector2f(mvSize.x,1), cColor(0.5f,1));

	}

	//-----------------------------------------------------------------------

	void cWidgetMainMenu::OnUpdate(float afTimeStep)
	{
		if(mbMustHide)
		{
			Hide();
			mbMustHide = false;
		}
	}

	//-----------------------------------------------------------------------

	void cWidgetMainMenu::OnInit()
	{
		float fWidth;
		float fHeight = mpDefaultFont->mvSize.y + 4 + 4;
		float fXOffset = 0;

		/////////////////////////////////////////////////////////////////////
		// If has parent, use parent width. If not, use screen width (temp)

		eWidgetType type = mpParent->GetType();
		switch(type)
		{
		case eWidgetType_Root:
			fWidth = mpSet->GetVirtualSize().x;
			break;
		case eWidgetType_Window:
			fWidth = mpParent->GetSize().x - 2 - 2;
			fXOffset = 2;
			break;
		default:
			fWidth = mpParent->GetSize().x;
			break;
		}

		SetSize(cVector2f(fWidth, fHeight));

		fHeight=0;
		mpParent->SetChildrenOffset(cVector3f(0,fHeight,0));
		SetPosition(GetLocalPosition()+cVector3f(fXOffset,-fHeight,1));
	}

	//-----------------------------------------------------------------------

	void cWidgetMainMenu::OnLoadGraphics()
	{
		mpGfxBackground = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpBackground);

		mfItemTextPadding = mpSkin->GetAttribute(eGuiSkinAttribute_MainMenuItemTextPadding).x;
		mfItemVPadding = mpSkin->GetAttribute(eGuiSkinAttribute_MainMenuItemVPadding).x;
		mfItemSeparation = mpSkin->GetAttribute(eGuiSkinAttribute_MainMenuItemSeparation).x;
	}

	//-----------------------------------------------------------------------

};
