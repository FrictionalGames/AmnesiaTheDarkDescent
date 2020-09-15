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

#include "gui/WidgetMultiPropertyListBox.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"

#include "gui/WidgetLabel.h"

#include "graphics/FontData.h"


#include "gui/GuiSet.h"
#include "gui/GuiSkin.h"


namespace hpl {

	/////////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	/////////////////////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------------------

	cWidgetMultiPropertyListBox::cWidgetMultiPropertyListBox(cGuiSet* apSet, cGuiSkin* apSkin) : iWidgetListBoxBase(apSet,apSkin)
	{
		mbClipsGraphics = true;
		mlNumBlankRows = 1;
	}

	cWidgetMultiPropertyListBox::~cWidgetMultiPropertyListBox()
	{
		if(mpSet->IsDestroyingSet()==false)
		{
			for(size_t i=0;i<mvColumns.size();++i)
			{
				mpSet->DestroyWidget(mvColumns[i]);
			}
		}
		mvColumns.clear();
	}

	//-------------------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	/////////////////////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------------------

	/**	Adds a new column
	 *
	 * \param asName 
	 * \param alIndex 
	 */
	void cWidgetMultiPropertyListBox::AddColumn(const tString& asName, const int alIndex, eFontAlign aAlign)
	{
		mvSubLists.push_back(cSubList(asName,alIndex, aAlign));

		cWidgetLabel* pLabel = mpSet->CreateWidgetLabel( cVector3f(0,0,mfBackgroundZ+0.5f),
														 -1, 
														 cString::To16Char(asName), 
														 this);
		pLabel->SetDefaultFontSize(mvDefaultFontSize);
		pLabel->SetBackGroundColor(cColor(0.82f,0.81f,0.79f,1));
		mvColumns.push_back(pLabel);

		SetColumnWidth((int)mvColumns.size()-1, 0);
        
		UpdateColumns();
	}

	//-------------------------------------------------------------------------------

	/** Sets column width, and updates the others positions
	 *
	 * \param alIdx 
	 * \param afWidth 
	 */
	void cWidgetMultiPropertyListBox::SetColumnWidth(const int alIdx, float afWidth)
	{
		if(alIdx>=0 && alIdx<(int)mvColumns.size())
		{
			cWidgetLabel* pLabel = mvColumns[alIdx];
			cVector2f vSize = pLabel->GetSize();

			float fTextWidth = mpDefaultFontType->GetLength(mvDefaultFontSize, pLabel->GetText().c_str());

			if(afWidth < fTextWidth)
				vSize.x = fTextWidth;
			else
				vSize.x = afWidth;

			mvColumns[alIdx]->SetSize(vSize);

			UpdateColumns();
		}
	}

	cVector3f cWidgetMultiPropertyListBox::GetItemStartPos()
	{
		return GetGlobalPosition() + cVector3f(0, mlNumBlankRows * (mvDefaultFontSize.y+2), mfBackgroundZ+0.02f);
	}

	//-------------------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	/////////////////////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------------------

	void cWidgetMultiPropertyListBox::UpdateColumns()
	{
		cVector3f vPos = cVector3f(0,0,0.02f);
		for(size_t i=0;i<mvColumns.size();++i)
		{
			mvColumns[i]->SetPosition(vPos);
			vPos.x += mvColumns[i]->GetSize().x;
		}
	}

	//-------------------------------------------------------------------------------

	void cWidgetMultiPropertyListBox::DrawItems(float afTimeStep, cGuiClipRegion* apClipRegion)
	{
		cVector3f vPosition = GetGlobalPosition();
		vPosition.z += mfBackgroundZ+0.01f;

		bool bSelected = false;

		///////////////////////////////////////
		// Draw Header background
		mpSet->DrawGfx(mpGfxBackground, vPosition, cVector2f(mvSize.x, mvDefaultFontSize.y+2), cColor(0.82f,0.81f,0.79f,1));

		/////////////////////////////
		// Sets up column clipping
		std::vector<cGuiClipRegion*> pRegion;
		for(size_t i=0; i<mvColumns.size();++i)
		{
			cWidgetLabel* pCol = mvColumns[i];
			cVector3f vPos = pCol->GetGlobalPosition();
			cVector2f vSize = cVector2f(pCol->GetSize().x, mvSize.y);
			
			pRegion.push_back( apClipRegion->CreateChild(vPos, vSize) );
		}

		/////////////////////////////
		// Draws items
		vPosition = GetItemStartPos();
		for(size_t i=mlFirstItem; i<mvItems.size(); ++i)
		{
			if(i-mlFirstItem > mlMaxItems) break;

			cWidgetItem* pItem = mvItems[i];

			bSelected = pItem->IsSelected();

			vPosition.x = GetGlobalPosition().x;

			// Draw Highlight
			if(bSelected)
				mpSet->DrawGfx(	mpGfxSelection,vPosition - cVector3f(0,0,0.01f),
								cVector2f(mvSize.x - mfSliderWidth,mvDefaultFontSize.y+2));

			// One property per column
			for(size_t j=0; j<mvSubLists.size(); ++j)
			{
				const cSubList& sublist = mvSubLists[j];
				cWidgetItemProperty* pProp = pItem->GetProperty(sublist.mlIndex);

				vPosition.x = mvColumns[j]->GetGlobalPosition().x;
				if(sublist.mAlign==eFontAlign_Right)
					vPosition.x+= mvColumns[j]->GetSize().x-10;

				if(pProp==NULL)
					continue;

				mpSet->SetCurrentClipRegion(pRegion[j]);

				////////////////////////////////////////////
				// Draw property according to type (image or string)
				switch(pProp->GetType())
				{
				case eItemPropertyType_String:
					if(bSelected)
                        DrawDefaultTextHighlight(pProp->GetText(), vPosition, sublist.mAlign);
					else
						DrawDefaultText(pProp->GetText(), vPosition, sublist.mAlign);
					break;
				case eItemPropertyType_Image:
					if(pProp->GetGfx()) mpSet->DrawGfx(pProp->GetGfx(), vPosition, mfIconSize);
					break;
				}

				mpSet->SetCurrentClipRegion(apClipRegion);
			}
			vPosition.y += mvDefaultFontSize.y+2;
		}


	}

	//-------------------------------------------------------------------------------

	void cWidgetMultiPropertyListBox::OnLoadGraphics()
	{
		iWidgetListBoxBase::OnLoadGraphics();

		mfIconSize = 15;
	}

	//-------------------------------------------------------------------------------

};


