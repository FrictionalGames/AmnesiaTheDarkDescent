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

#include "gui/WidgetLabel.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"

#include "graphics/FontData.h"

#include "math/Math.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cWidgetLabel::cWidgetLabel(cGuiSet *apSet, cGuiSkin *apSkin) : iWidget(eWidgetType_Label,apSet, apSkin)
	{
		mfScrollTimer = 0.0f;
		mfWaitToScrollTime = 1.0f;
		mfScrollSpeedMul = 1.0f;

		mbWordWrap = false;
		mfWordWrapOffset = 0.0f;
		mfWordWrapRowsHeight = 0.0f;
		mbScrollingDown = true;

		mTextAlign = eFontAlign_Left;

		mlMaxCharacters = -1;

		mbAutogenerateSize = false;

		mbDrawBackGround = false;
		mBackGroundColor = cColor(1,1);

		LoadGraphics();
	}

	//-----------------------------------------------------------------------

	cWidgetLabel::~cWidgetLabel()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cWidgetLabel::SetMaxTextLength(int alLength)
	{
		if(mlMaxCharacters == alLength) return;

		mlMaxCharacters = alLength;
	}

	//-----------------------------------------------------------------------

	void cWidgetLabel::SetAutogenerateSize(bool abX)
	{
		if(mbAutogenerateSize == abX) return;

		mbAutogenerateSize = abX;
		
		OnChangeText();
	}

	//-----------------------------------------------------------------------

	void cWidgetLabel::SetDefaultFontSize(const cVector2f& avSize)
	{
		iWidget::SetDefaultFontSize(avSize);

		OnChangeText();
	}

	//-----------------------------------------------------------------------



	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cWidgetLabel::DrawText(float afTimeStep, cGuiClipRegion *apClipRegion)
	{
		////////////////////////////////
		// Texts
		cVector3f vOffset = 0;
		if(mTextAlign == eFontAlign_Center)		vOffset.x += mvSize.x/2;
		else if(mTextAlign == eFontAlign_Right)	vOffset.x += mvSize.x;
		
		if(mbWordWrap)
		{
			int lChars =0;
			bool bEnabled = IsEnabled();
			float fHeight = mvDefaultFontSize.y+2;
			tWStringVec vRows;
			mpDefaultFontType->GetWordWrapRows(mvSize.x,fHeight,
												mvDefaultFontSize, msText,
												&vRows);

			mfWordWrapRowsHeight = (fHeight-1) * (int)vRows.size();

			for(size_t i=0; i< vRows.size(); ++i)
			{
				bool bBreak = false;
				if(mlMaxCharacters>=0)
				{
					if(lChars + (int)vRows[i].length() > mlMaxCharacters)
					{
						vRows[i] = cString::SubW(vRows[i],0, mlMaxCharacters - lChars);
						bBreak = true;
					}
					lChars += (int)vRows[i].length();
				}

				if(bEnabled)
					DrawDefaultText(vRows[i], GetGlobalPosition()+vOffset-cVector3f(0,mfWordWrapOffset,0),mTextAlign);
				else {
					DrawDefaultText(vRows[i], GetGlobalPosition()+vOffset-cVector3f(0,mfWordWrapOffset,0),mTextAlign, cColor(0.5f, mDefaultFontColor.a));
					//DrawSkinText(vRows[i],eGuiSkinFont_Disabled,GetGlobalPosition()+vOffset,mTextAlign);
				}
				vOffset.y += fHeight;

				if(bBreak) break;
			}
		}
		else
		{
			if(mlMaxCharacters>=0 && (int)msText.length() > mlMaxCharacters)
			{
				if(IsEnabled())
					DrawDefaultText(cString::SubW(msText,0,mlMaxCharacters), GetGlobalPosition()+vOffset,mTextAlign);
				else {
					DrawDefaultText(cString::SubW(msText,0,mlMaxCharacters), GetGlobalPosition()+vOffset,mTextAlign, cColor(0.5f, mDefaultFontColor.a));
					//DrawSkinText(cString::SubW(msText,0,mlMaxCharacters),eGuiSkinFont_Disabled,GetGlobalPosition()+vOffset,mTextAlign);
				}
			}
			else
			{
				if(IsEnabled())
					DrawDefaultText(msText, GetGlobalPosition()+vOffset,mTextAlign);
				else {
					DrawDefaultText(msText, GetGlobalPosition()+vOffset, mTextAlign, cColor(0.5f, mDefaultFontColor.a));
					//DrawSkinText(msText,eGuiSkinFont_Disabled,GetGlobalPosition()+vOffset,mTextAlign);
				}
			}
		}
	}

	//-----------------------------------------------------------------------

	void cWidgetLabel::OnLoadGraphics()
	{
		mpGfxBackground = mpSkin->GetGfx(eGuiSkinGfx_FrameBackground);
	}

	//-----------------------------------------------------------------------

	void cWidgetLabel::OnUpdate(float afTimeStep)
	{
		if(mbWordWrap==false)
			return;

		////////////////////////////////////
		// Do a little wait before starting to scroll
		mfScrollTimer -= afTimeStep;
		if(mfScrollTimer >= 0)
			return;

		float fAdvance = afTimeStep * mfScrollSpeedMul;

		// Scroll down
		if(mbScrollingDown)
		{
			if(mfWordWrapOffset+mvSize.y+fAdvance > mfWordWrapRowsHeight)
				mbScrollingDown = false;
			else
				mfWordWrapOffset += fAdvance;
		}
		// Scroll up
		else
		{
			if(mfWordWrapOffset-fAdvance < 0)
				mbScrollingDown = true;
			else
				mfWordWrapOffset -= fAdvance;
		}
	}

	//-----------------------------------------------------------------------

	void cWidgetLabel::OnChangeText()
	{
		mfScrollTimer = mfWaitToScrollTime;
		mfWordWrapOffset = 0;
		mbScrollingDown = true;

		if(mbAutogenerateSize==false) return;

		float fLength = mpDefaultFontType->GetLength(mvDefaultFontSize, msText.c_str());

        SetSize(cVector2f(fLength, mvDefaultFontSize.y));
	}

	//-----------------------------------------------------------------------

	void cWidgetLabel::OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion)
	{
		////////////////////////////////
		// Background
		if(mbDrawBackGround)
		{
			mpSet->DrawGfx(	mpGfxBackground,GetGlobalPosition() - cVector3f(2,2,0.001f),
				mvSize+4,mBackGroundColor);
		}

		if(mbClipsGraphics)
			return;

		DrawText(afTimeStep, apClipRegion);
	}

	//-----------------------------------------------------------------------

	void cWidgetLabel::OnDrawAfterClip(float afTimeStep, cGuiClipRegion* apClipRegion)
	{
		DrawText(afTimeStep, apClipRegion);
	}

	//-----------------------------------------------------------------------
	
	bool cWidgetLabel::OnMouseMove(const cGuiMessageData& aData)
	{
		return false;
	}

	//-----------------------------------------------------------------------

	bool cWidgetLabel::OnMouseDown(const cGuiMessageData& aData)
	{
		return false;
	}

	//-----------------------------------------------------------------------

	bool cWidgetLabel::OnMouseUp(const cGuiMessageData& aData)
	{
		return false;
	}

	//-----------------------------------------------------------------------

	bool cWidgetLabel::OnMouseEnter(const cGuiMessageData& aData)
	{
		return false;
	}
	
	//-----------------------------------------------------------------------
	
	bool cWidgetLabel::OnMouseLeave(const cGuiMessageData& aData)
	{
		return false;
	}

	//-----------------------------------------------------------------------


}
