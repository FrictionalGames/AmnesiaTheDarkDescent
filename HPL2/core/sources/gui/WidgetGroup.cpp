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

#include "gui/WidgetGroup.h"

#include "gui/GuiTypes.h"

#include "gui/Gui.h"
#include "gui/GuiSet.h"
#include "gui/GuiSkin.h"
#include "gui/GuiGfxElement.h"

#include "gui/WidgetLabel.h"

#include "graphics/FontData.h"



namespace hpl {
	
	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cWidgetGroup::cWidgetGroup(cGuiSet* apSet, cGuiSkin *apSkin) : iWidget(eWidgetType_Group, apSet, apSkin)
	{
		mpHeader = NULL;

		LoadGraphics();
	}

	//-----------------------------------------------------------------------
	
	cWidgetGroup::~cWidgetGroup()
	{
		if(mpSet->IsDestroyingSet()==false)
		{
			mpSet->DestroyWidget(mpHeader);
		}
	}

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cWidgetGroup::SetDefaultFontSize(const cVector2f& avSize)
	{
		iWidget::SetDefaultFontSize(avSize);
		mpHeader->SetDefaultFontSize(avSize);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cWidgetGroup::OnInit()
	{
		mpHeader = mpSet->CreateWidgetLabel(cVector3f(mfHeaderOffset,-mpDefaultFont->mvSize.y*0.5f,0.1f),0, msText, this);
		mpHeader->SetTextAlign(eFontAlign_Left);
		mpHeader->SetAutogenerateSize(true);

		OnChangeText();
	}

	//-----------------------------------------------------------------------

	void cWidgetGroup::OnLoadGraphics()
	{
		mpGfxBackground = mpSkin->GetGfx(eGuiSkinGfx_GroupBackground);

		mvGfxBorders[0] = mpSkin->GetGfx(eGuiSkinGfx_GroupBorderRight);
		mvGfxBorders[1] = mpSkin->GetGfx(eGuiSkinGfx_GroupBorderLeft);
		mvGfxBorders[2] = mpSkin->GetGfx(eGuiSkinGfx_GroupBorderUp);
		mvGfxBorders[3] = mpSkin->GetGfx(eGuiSkinGfx_GroupBorderDown);

		mvGfxCorners[0] = mpSkin->GetGfx(eGuiSkinGfx_GroupCornerLU);
		mvGfxCorners[1] = mpSkin->GetGfx(eGuiSkinGfx_GroupCornerRU);
		mvGfxCorners[2] = mpSkin->GetGfx(eGuiSkinGfx_GroupCornerRD);
		mvGfxCorners[3] = mpSkin->GetGfx(eGuiSkinGfx_GroupCornerLD);

		mfHeaderOffset = mpSkin->GetAttribute(eGuiSkinAttribute_GroupHeaderOffset).x;
		mbHeaderBreaksUpperBorder = (mpSkin->GetAttribute(eGuiSkinAttribute_GroupHeaderOverUpperBorder).x==0);
	}

	//-----------------------------------------------------------------------

	void cWidgetGroup::OnChangeText()
	{
		if(mpHeader) 
			mpHeader->SetText(msText);
	}

	//-----------------------------------------------------------------------

	void cWidgetGroup::OnDraw(float afTimeStep, cGuiClipRegion* apClipRegion)
	{
		const cVector3f& vPos = GetGlobalPosition();
		if(mbHeaderBreaksUpperBorder)
		{
			const cVector3f& vHeaderPos = mpHeader->GetLocalPosition();
			cVector3f vPostHeaderPos = cVector3f(vHeaderPos.x + mpHeader->GetSize().x,0,0);

			float fPreHeaderWidth = vHeaderPos.x;
			float fPostHeaderWidth = mvSize.x - fPreHeaderWidth - mpHeader->GetSize().x;

			mpSet->SetDrawOffset(vPos);

			cVector3f vOffset;
			cVector2f vSize;

			vOffset.z = 0;

			///////////////////////
			// Background
			if(mpGfxBackground)
			{
				vOffset.x = mvGfxCorners[0]->GetActiveSize().x;
				vOffset.y = mvGfxCorners[0]->GetActiveSize().y;

				vSize = mvSize - mvGfxCorners[2]->GetActiveSize()-mvGfxCorners[0]->GetActiveSize();
				
				mpSet->DrawGfx(mpGfxBackground,vOffset,
								vSize,
								cColor(1,1));
			}

			{
				///////////////////////
				// Borders
				//Right
				mpSet->DrawGfx(	mvGfxBorders[0],
					cVector3f(	mvSize.x - mvGfxBorders[0]->GetActiveSize().x,
					mvGfxCorners[1]->GetActiveSize().y,0),
					cVector2f(	mvGfxBorders[0]->GetImageSize().x, 
					mvSize.y - (mvGfxCorners[2]->GetActiveSize().y +
					mvGfxCorners[1]->GetActiveSize().y)));
				//Left
				mpSet->DrawGfx(	mvGfxBorders[1],
					cVector3f(	0,mvGfxCorners[0]->GetActiveSize().y,0),
					cVector2f(	mvGfxBorders[1]->GetImageSize().x, 
					mvSize.y - (mvGfxCorners[3]->GetActiveSize().y + 
					mvGfxCorners[0]->GetActiveSize().y)));

				//Up
				mpSet->DrawGfx(mvGfxBorders[2], 
								cVector3f(mvGfxCorners[0]->GetActiveSize().x,0,0), 
								cVector2f(fPreHeaderWidth-4, mvGfxBorders[2]->GetActiveSize().y));
				mpSet->DrawGfx(mvGfxBorders[2], 
								vPostHeaderPos+cVector3f(4,0,0) , 
								cVector2f(fPostHeaderWidth-mvGfxCorners[1]->GetActiveSize().x-4, mvGfxBorders[2]->GetActiveSize().y));

				//Down
				mpSet->DrawGfx(	mvGfxBorders[3],
					cVector3f(	mvGfxCorners[3]->GetActiveSize().x, 
					mvSize.y - mvGfxCorners[3]->GetActiveSize().y,0),
					cVector2f(	mvSize.x - (mvGfxCorners[2]->GetActiveSize().x+
					mvGfxCorners[3]->GetActiveSize().x), 
					mvGfxBorders[3]->GetImageSize().y));


				///////////////////////
				// Corners
				//Left Up
				mpSet->DrawGfx(mvGfxCorners[0], cVector3f(0,0,0));
				//Right Up
				mpSet->DrawGfx(mvGfxCorners[1], cVector3f(	mvSize.x - mvGfxCorners[1]->GetActiveSize().x,0,0));

				//Right Down
				mpSet->DrawGfx(mvGfxCorners[2], cVector3f(	mvSize.x -  mvGfxCorners[2]->GetActiveSize().x,
					mvSize.y -  mvGfxCorners[2]->GetActiveSize().y,0));
				//Left Down
				mpSet->DrawGfx(mvGfxCorners[3], cVector3f(	0,mvSize.y - mvGfxCorners[3]->GetActiveSize().y,0));
			}

			mpSet->SetDrawOffset(0);
		}
		else
		{
			DrawBordersAndCorners(mpGfxBackground, mvGfxBorders, mvGfxCorners, vPos, mvSize);
		}
	}

	//-----------------------------------------------------------------------

	void cWidgetGroup::OnDrawAfterClip(float afTimeStep, cGuiClipRegion* apClipRegion)
	{
	}

	//-----------------------------------------------------------------------

};
