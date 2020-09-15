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

#include "gui/WidgetListBox.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"

#include "gui/WidgetSlider.h"

#include <algorithm>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cWidgetListBox::cWidgetListBox(cGuiSet *apSet, cGuiSkin *apSkin) : iWidgetListBoxBase(apSet, apSkin)
	{
	}

	//-----------------------------------------------------------------------

	cWidgetListBox::~cWidgetListBox()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cWidgetListBox::DrawItems(float afTimeStep, cGuiClipRegion* apClipRegion)
	{
		cVector3f vPosition = GetItemStartPos()+cVector3f(3,0,mfBackgroundZ+0.1f);
		for(int i=mlFirstItem; i < (int)mvItems.size(); ++i)
		{
			if(i-mlFirstItem > mlMaxItems) break;
			cWidgetItem* pItem = mvItems[i];
			if(pItem==NULL)
				continue;

			if(pItem->IsSelectable()==false)
			{
				mpSet->DrawGfx( mpGfxBackground, vPosition - cVector3f(3,0,0.01f), cVector2f(mvSize.x, mvDefaultFontSize.y), cColor(0.82f,0.81f,0.79f,1));
			}

			if(pItem->IsSelected())
			{
				mpSet->DrawGfx(	mpGfxSelection,vPosition - cVector3f(3,0,0.01f),
								cVector2f(mvSize.x,mvDefaultFontSize.y));
				DrawDefaultTextHighlight(pItem->GetText(),vPosition,eFontAlign_Left);
			}
			else
				DrawDefaultText(pItem->GetText(),vPosition,eFontAlign_Left);
			
			vPosition.y += mvDefaultFontSize.y +2;
		}
	}
	
	//-----------------------------------------------------------------------

}
