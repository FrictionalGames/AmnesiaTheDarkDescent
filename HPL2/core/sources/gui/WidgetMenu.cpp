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

#include "gui/WidgetMenu.h"

#include "gui/Widget.h"

#include "system/LowLevelSystem.h"

#include "gui/WidgetMenuItem.h"



#include "gui/GuiSet.h"
#include "gui/GuiSkin.h"



namespace hpl
{
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iWidgetMenu::iWidgetMenu(eWidgetType aeMenuType, cGuiSet* apSet, cGuiSkin* apSkin) : iWidget(aeMenuType, apSet, apSkin)
	{
		mpParentItem = NULL;
		mpHighlightedItem = NULL;
		mpPrevAttention = NULL;

		mbMustHide = false;
	}

	//-----------------------------------------------------------------------

	iWidgetMenu::~iWidgetMenu()
	{
		if(mpSet->IsDestroyingSet()==false)
		{
			ClearMenuItems();
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	/**
	 * AddMenuItem() : adds a new MenuItem, updating previous items positions and sizes if needed, and returns it.
	 * \param &asText 
	 * \return cWidgetMenuItem*
	 */
	cWidgetMenuItem* iWidgetMenu::AddMenuItem(const tWString &asText)
	{
		cWidgetMenuItem* pItem = mpSet->CreateWidgetMenuItem(0,0,asText,this);
		mvMenuItems.push_back(pItem);

		UpdateMenuItemsPos(pItem);
		UpdateMenuItemsSize(pItem);

		return pItem;
	}

    //-----------------------------------------------------------------------

	/**
	 * ClearMenuItems() : clears MenuItem vector
	 */
	void iWidgetMenu::ClearMenuItems()
	{
		for (tWidgetMenuItemVectorIt it= mvMenuItems.begin(); it != mvMenuItems.end(); ++it)
			mpSet->DestroyWidget(*it);

		mvMenuItems.clear();
	}

	//-----------------------------------------------------------------------

	/**
	 * SetParentItem(cWidgetMenuItem*) : Sets parent MenuItem
	 * \param apParentItem 
	 */
	void iWidgetMenu::SetParentItem( cWidgetMenuItem* apParentItem )
	{
		mpParentItem = apParentItem;
	}

	//-----------------------------------------------------------------------

	/**
	 * GetParentMenu() : returns the parent Menu, NULL if topmost.
	 * \return 
	 */
	iWidgetMenu* iWidgetMenu::GetParentMenu()
	{
		if(mpParentItem==NULL)
			return NULL;
		
		return mpParentItem->GetParentMenu();
	}

	//-----------------------------------------------------------------------

	/**
	 * SetMustHide(bool) : sets if the Menu should hide
	 * \param abX 
	 */
	void iWidgetMenu::SetMustHide(bool abX)
	{
		mbMustHide = abX;
	}

	//-----------------------------------------------------------------------

	/**
	 * IsSubMenuOpen() : returns true if any of this Menu's submenues is being shown at the moment
	 * \return 
	 */
	bool iWidgetMenu::IsSubmenuOpen()
	{
		if(mpHighlightedItem!=NULL)
			return mpHighlightedItem->IsMenuOpen();
		
		return false;
	}

	//-----------------------------------------------------------------------

	/**
	 * GetTopMostMenu() : returns the topmost Menu in the hierarchy
	 * \return 
	 */
	iWidgetMenu* iWidgetMenu::GetTopMostMenu()
	{
		if(mpParentItem!=NULL)
			return GetParentMenu()->GetTopMostMenu();
		else
			return this;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------


	//-----------------------------------------------------------------------


};


