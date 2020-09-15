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

#include "gui/WidgetFrame.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"
#include "gui/WidgetSlider.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cWidgetFrame::cWidgetFrame(cGuiSet *apSet, cGuiSkin *apSkin, bool abHScrollBar, bool abVScrollBar) : iWidget(eWidgetType_Frame,apSet, apSkin)
	{
		mbClipsGraphics = true;

		mbHScrollBar = abHScrollBar;
		mbVScrollBar = abVScrollBar;

		mbDrawFrame = false;
		
		mbDrawBackground = false;
		mfBackgroundZ = -0.5;
		mBackGroundColor = cColor(1,1);

		mpHSlider = NULL;
		mpVSlider = NULL;
		mvMinWidgetCoord = 999999.9f;
		mvMaxWidgetCoord = -999999.9f;
		if(abHScrollBar)
		{
			mpHSlider = mpSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal,0,cVector2f(0,18),0, this);
			mpHSlider->SetAffectedByScroll(false);
			mpHSlider->AddCallback(eGuiMessage_SliderMove, this, kGuiCallback(OnSliderMove));
			mpHSlider->SetVisible(false);
			mpHSlider->SetEnabled(false);
		}
		if(abVScrollBar)
		{
			mpVSlider = mpSet->CreateWidgetSlider(eWidgetSliderOrientation_Vertical,0,cVector2f(18,0),0, this);
			mpVSlider->SetAffectedByScroll(false);
			mpVSlider->AddCallback(eGuiMessage_SliderMove, this, kGuiCallback(OnSliderMove));
			mpVSlider->SetVisible(false);
			mpVSlider->SetEnabled(false);
		}

		mvAutoScrollMargin = 0;

		mbScrollBarsNeedUpdate = true;
		mbScrollUpdated = true;
	}

	//-----------------------------------------------------------------------

	cWidgetFrame::~cWidgetFrame()
	{
		if(mpSet->IsDestroyingSet()==false)
		{
			if(mpHSlider)
				mpSet->DestroyWidget(mpHSlider);
			if(mpVSlider)
				mpSet->DestroyWidget(mpVSlider);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cWidgetFrame::OnAttachChild(iWidget* apChild)
	{
		mbScrollBarsNeedUpdate = true;
	}

	//-----------------------------------------------------------------------

	void cWidgetFrame::OnRemoveChild(iWidget* apChild)
	{
		mbScrollBarsNeedUpdate = true;
	}

	//-----------------------------------------------------------------------

	void cWidgetFrame::ScrollToPosition(const cVector2f& avPos)
	{
		if(mpHSlider)
			mpHSlider->SetValue(cMath::RoundToInt(avPos.x));
		if(mpVSlider)
			mpVSlider->SetValue(cMath::RoundToInt(avPos.y));
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cWidgetFrame::OnSliderMove(iWidget* apWidget, const cGuiMessageData& aData)
	{
		if(apWidget==mpHSlider || apWidget==mpVSlider)
			mbScrollUpdated = true;
		
		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetFrame,OnSliderMove);

	//-----------------------------------------------------------------------

	void cWidgetFrame::OnLoadGraphics()
	{
		mpGfxBackground = mpSkin->GetGfx(eGuiSkinGfx_FrameBackground);

		mvGfxBorders[0] = mpSkin->GetGfx(eGuiSkinGfx_FrameBorderRight);
		mvGfxBorders[1] = mpSkin->GetGfx(eGuiSkinGfx_FrameBorderLeft);
		mvGfxBorders[2] = mpSkin->GetGfx(eGuiSkinGfx_FrameBorderUp);
		mvGfxBorders[3] = mpSkin->GetGfx(eGuiSkinGfx_FrameBorderDown);

		mvGfxCorners[0] = mpSkin->GetGfx(eGuiSkinGfx_FrameCornerLU);
		mvGfxCorners[1] = mpSkin->GetGfx(eGuiSkinGfx_FrameCornerRU);
		mvGfxCorners[2] = mpSkin->GetGfx(eGuiSkinGfx_FrameCornerRD);
		mvGfxCorners[3] = mpSkin->GetGfx(eGuiSkinGfx_FrameCornerLD);
	}

	//-----------------------------------------------------------------------

	void cWidgetFrame::OnInit()
	{
	}

	//-----------------------------------------------------------------------

	void cWidgetFrame::OnChangeSize()
	{
		if(mpHSlider)
		{
			float fSliderSize = mvSize.x;
			if(mpVSlider&&mpVSlider->IsVisible())
				fSliderSize-=18;
			mpHSlider->SetSize(cVector2f(fSliderSize,18));
			mpHSlider->SetPosition(cVector3f(0,mvSize.y-18,0.7f));
		}
		if(mpVSlider)
		{
			float fSliderSize = mvSize.y;
			if(mpHSlider&&mpHSlider->IsVisible())
				fSliderSize-=18;
			mpVSlider->SetSize(cVector2f(18,fSliderSize));
			mpVSlider->SetPosition(cVector3f(mvSize.x-18,0,0.7f));
		}

		mbScrollBarsNeedUpdate = true;
	}

	//-----------------------------------------------------------------------

	void cWidgetFrame::OnChildUpdate(iWidget* apChild)
	{
		mbScrollBarsNeedUpdate = true;
	}

	//-----------------------------------------------------------------------

	void cWidgetFrame::OnUpdate(float afTimeStep)
	{
		bool bHoriActive = false;
		bool bVertActive = false;
		cVector3f vOffset = GetScrollAmount();

		if(mbScrollBarsNeedUpdate && (mpHSlider || mpVSlider) && IsVisible())
		{
			mbScrollBarsNeedUpdate=false;

			cVector2f vSafeFrame = cVector2f(vOffset.x,vOffset.y) + mvSize-mvAutoScrollMargin;
			tWidgetListIt it = mlstChildren.begin();

			cVector3f vMaxCoord = -999999;

			//////////////////////////////////////////////////////
			// Iterate children
			for(;it!=mlstChildren.end();++it)
			{
				iWidget* pChild = *it;
				if(pChild==mpHSlider || pChild==mpVSlider)
					continue;

				if(pChild->IsVisible())
				{
					cVector3f vPos = pChild->GetLocalPosition();
					cVector3f vLowerRightCorner = vPos + cVector3f(pChild->GetSize());

					if(vPos.x<vOffset.x || vLowerRightCorner.x>vSafeFrame.x)
						bHoriActive = true;
					if(vPos.y<vOffset.y || vLowerRightCorner.y>vSafeFrame.y)
					{
						bVertActive = true;
					}

					if(vMaxCoord.x<vLowerRightCorner.x)
						vMaxCoord.x = vLowerRightCorner.x;
					if(vMaxCoord.y<vLowerRightCorner.y)
					{
						vMaxCoord.y = vLowerRightCorner.y;
					}
				}
			}

			if(vMaxCoord.x!=mvMaxWidgetCoord.x)
				mvMaxWidgetCoord.x = vMaxCoord.x;
			if(vMaxCoord.y!=mvMaxWidgetCoord.y)
				mvMaxWidgetCoord.y = vMaxCoord.y;

			cVector3f vLimitOffset = vOffset;
			if(mpHSlider)
			{
				float fEndCoord = mvMaxWidgetCoord.x-mvSize.x;
				float fBarSize = mvSize.y*fEndCoord/mvMaxWidgetCoord.y;
				if(bVertActive && mpVSlider)
					fEndCoord += mpVSlider->GetSize().x;

				mpHSlider->SetMaxValue(cMath::RoundToInt(fEndCoord)+1);
				mpHSlider->SetBarValueSize(cMath::RoundToInt(fBarSize));

				mpHSlider->SetButtonValueAdd(cMath::RoundToInt(mvSize.x*0.1f));
				mpHSlider->SetBarClickValueAdd(mpHSlider->GetBarValueSize());

				if(fEndCoord>0 && vLimitOffset.x>fEndCoord)
					vLimitOffset.x = fEndCoord;

				if(bHoriActive==false)
					mpHSlider->SetValue(0);
				else
				{
					int lOffset = cMath::RoundToInt(vOffset.x);
					int lLimitOffset = cMath::RoundToInt(vLimitOffset.x);
					if(lOffset>lLimitOffset)
						mpHSlider->SetValue(lLimitOffset);
				}

				mpHSlider->SetEnabled(bHoriActive);
				mpHSlider->SetVisible(bHoriActive);
			}
			if(mpVSlider)
			{
				float fEndCoord = mvMaxWidgetCoord.y-mvSize.y;
				float fBarSize = mvSize.y*fEndCoord/mvMaxWidgetCoord.y;
				if(bHoriActive && mpHSlider)
					fEndCoord += mpHSlider->GetSize().y;

				mpVSlider->SetMaxValue(cMath::RoundToInt(fEndCoord)+1);
				mpVSlider->SetBarValueSize(cMath::RoundToInt(fBarSize));

				mpVSlider->SetButtonValueAdd(cMath::RoundToInt(mvSize.y*0.1f));
				mpVSlider->SetBarClickValueAdd(mpVSlider->GetBarValueSize());

				if(fEndCoord>0)
				{
					if(vLimitOffset.y>fEndCoord) vLimitOffset.y = fEndCoord;
				}
				else
				{
					bVertActive = false;
				}

				if(bVertActive==false)
					mpVSlider->SetValue(0);
				else
				{
					int lOffset = cMath::RoundToInt(vOffset.y);
					int lLimitOffset = cMath::RoundToInt(vLimitOffset.y);
					if(lOffset>lLimitOffset)
						mpVSlider->SetValue(lLimitOffset);
				}

				mpVSlider->SetEnabled(bVertActive);
				mpVSlider->SetVisible(bVertActive);
			}

			//OnChangeSize();
		}

		if(mbScrollUpdated)
		{
			mbScrollUpdated = false;
			if(mpHSlider) vOffset.x = (float)mpHSlider->GetValue();
			if(mpVSlider) vOffset.y = (float)mpVSlider->GetValue();
		}

		SetScrollAmount(vOffset);
	}

	//-----------------------------------------------------------------------

	void cWidgetFrame::OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion)
	{
		////////////////////////////////
		// Borders
		if(mbDrawFrame)
		{
			//Background must be drawn first!
			DrawBordersAndCorners(	NULL, mvGfxBorders, mvGfxCorners, 
									GetGlobalPosition() - 
										cVector3f(	mvGfxCorners[0]->GetActiveSize().x,
													mvGfxCorners[0]->GetActiveSize().y,0), 
									mvSize +	mvGfxCorners[0]->GetActiveSize() + 
												mvGfxCorners[2]->GetActiveSize());
		}
	}

	//-----------------------------------------------------------------------

	void cWidgetFrame::OnDrawAfterClip(float afTimeStep, cGuiClipRegion *apClipRegion)
	{
		////////////////////////////////
		// Background
		if(mbDrawBackground)
		{
			mpSet->DrawGfx(	mpGfxBackground,GetGlobalPosition() +cVector3f(0,0,mfBackgroundZ),
				mvSize,mBackGroundColor);
		}
	}

	//-----------------------------------------------------------------------
	
	bool cWidgetFrame::OnMouseMove(const cGuiMessageData& aData)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool cWidgetFrame::OnMouseDown(const cGuiMessageData& aData)
	{
		if(aData.mlVal&eGuiMouseButton_Left ||
		   aData.mlVal&eGuiMouseButton_Right)
			return true;

		if(mpVSlider==NULL || mpVSlider->IsVisible()==false)
			return false;

		int lValue = mpVSlider->GetValue();
		int lAdd = mpVSlider->GetButtonValueAdd()*3;

		if(aData.mlVal & eGuiMouseButton_WheelUp)
			lValue-=lAdd;
		else if(aData.mlVal & eGuiMouseButton_WheelDown)
			lValue+=lAdd;

		if(lValue!=mpVSlider->GetValue())
			mpVSlider->SetValue(lValue);

		return true;
	}

	//-----------------------------------------------------------------------

	bool cWidgetFrame::OnMouseUp(const cGuiMessageData& aData)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool cWidgetFrame::OnMouseEnter(const cGuiMessageData& aData)
	{
		return false;
	}
	
	//-----------------------------------------------------------------------
	
	bool cWidgetFrame::OnMouseLeave(const cGuiMessageData& aData)
	{
		return false;
	}

	//-----------------------------------------------------------------------


}
