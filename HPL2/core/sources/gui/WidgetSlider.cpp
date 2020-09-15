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

#include "gui/WidgetSlider.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "system/String.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"

#include "gui/WidgetButton.h"

#include "graphics/FontData.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cWidgetSlider::cWidgetSlider(cGuiSet *apSet, cGuiSkin *apSkin, 
								eWidgetSliderOrientation aOrientation) 
					: iWidget(eWidgetType_Slider,apSet, apSkin)
	{
		mOrientation = aOrientation;

		mbPressed = false;

		mlValue =0;
		mlMaxValue =10;
		mlButtonValueAdd =1;
		mlBarClickValueAdd = mlButtonValueAdd*10;
		mlBarValueSize = 1;

		for(int i=0; i<2; ++i) mvButtons[i] = NULL;

		mfButtonSize = mpSkin->GetAttribute(eGuiSkinAttribute_SliderButtonSize).x;

		LoadGraphics();
	}

	//-----------------------------------------------------------------------

	cWidgetSlider::~cWidgetSlider()
	{
		if(mpSet->IsDestroyingSet()==false)
		{
			for(int i=0; i<2; ++i) mpSet->DestroyWidget(mvButtons[i]);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cWidgetSlider::SetValue(int alValue, bool abGenCallback)
	{ 
		if(alValue<0) alValue = 0;
		if(alValue>mlMaxValue) alValue = mlMaxValue;

		if(mlValue == alValue) return;
		mlValue = alValue;

		if(abGenCallback) 
		{
			ProcessMessage(eGuiMessage_SliderMove, cGuiMessageData(mlValue));
			if(mbPressed==false)
				ProcessMessage(eGuiMessage_SliderRelease, cGuiMessageData(mlValue));
		}
		UpdateBarProperties();
	}

	void cWidgetSlider::SetMaxValue(int alMax)
	{ 
		if(mlMaxValue == alMax) return;

		mlMaxValue = alMax;
		if(mlMaxValue <0)mlMaxValue=0;

		if(mlBarValueSize > mlMaxValue) mlBarValueSize = mlMaxValue;

		UpdateBarProperties();
	}

	void cWidgetSlider::SetButtonValueAdd(int alAdd)
	{ 
		mlButtonValueAdd = alAdd;
	}

	void cWidgetSlider::SetBarClickValueAdd(int alAdd)
	{
		mlBarClickValueAdd = alAdd;
	}

	void cWidgetSlider::SetBarValueSize(int alSize)
	{ 
		if(mlBarValueSize == alSize) return;

		mlBarValueSize  = alSize;
		if(mlBarValueSize > mlMaxValue+1) mlBarValueSize = mlMaxValue+1;

		UpdateBarProperties();
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	void cWidgetSlider::UpdateBarProperties()
	{
		//////////////////////////
		// Vertical
		if(mOrientation == eWidgetSliderOrientation_Vertical)
		{
			mfSliderSize = mvSize.y-mfButtonSize*2;

			mvBarSize = cVector2f(mvSize.x, 
				((float)mlBarValueSize / (float)(mlMaxValue+1))*(mfSliderSize));

			float fMinSize = mvGfxCorners[0]->GetActiveSize().y + mvGfxCorners[3]->GetActiveSize().y + 2;
			if(mvBarSize.y < fMinSize) mvBarSize.y = fMinSize;

			//The set taken for each value
			if(mlMaxValue > 0)
				mfValueStep =	(1 / (float)(mlMaxValue)) * (mfSliderSize - mvBarSize.y);
			else
				mfValueStep = 0;

			mvBarPos = cVector3f(0, mfButtonSize + mfValueStep*(float)mlValue,0.2f);
		}
		//////////////////////////
		// Horizontal
		else
		{
			mfSliderSize = mvSize.x-mfButtonSize*2;

			mvBarSize = cVector2f(((float)mlBarValueSize / (float)(mlMaxValue+1))*(mfSliderSize),
									mvSize.y);

			float fMinSize = mvGfxCorners[0]->GetActiveSize().x + mvGfxCorners[3]->GetActiveSize().x + 2;
			if(mvBarSize.x < fMinSize) mvBarSize.x = fMinSize;

			//The set taken for each value
			if(mlMaxValue > 0)
				mfValueStep =	(1 / (float)(mlMaxValue)) * (mfSliderSize - mvBarSize.x);
			else
				mfValueStep =0;

			mvBarPos = cVector3f(mfButtonSize + mfValueStep*(float)mlValue,0,0.2f);
		}
		
		mBarRect.x = GetGlobalPosition().x + mvBarPos.x;
		mBarRect.y = GetGlobalPosition().y + mvBarPos.y;
		mBarRect.w = mvBarSize.x;
		mBarRect.h = mvBarSize.y;

		
		mfMaxPos = mfButtonSize + mfValueStep * (float)(mlMaxValue);
		mfMinPos = mfButtonSize;
	}
	
	//-----------------------------------------------------------------------

	bool cWidgetSlider::ArrowButtonDown(iWidget* apWidget, const cGuiMessageData& aData)
	{	
		int i;
		for(i=0; i<2; ++i) if(mvButtons[i] == apWidget) break;

		if(i==0) SetValue(mlValue-mlButtonValueAdd);
		if(i==1) SetValue(mlValue+mlButtonValueAdd);
		
		//Call callbacks.
		//ProcessMessage(eGuiMessage_SliderMove, cGuiMessageData(mlValue));
		
		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetSlider,ArrowButtonDown)

	//-----------------------------------------------------------------------

	void cWidgetSlider::OnInit()
	{
		mvButtons[0] = mpSet->CreateWidgetButton(0,0,_W(""), this);
		mvButtons[1] = mpSet->CreateWidgetButton(0,0,_W(""), this);

		for(int i=0; i<2;i++)
		{
			mvButtons[i]->SetImage(mvGfxArrow[i],false);
			
			mvButtons[i]->AddCallback(eGuiMessage_MouseDown,this,kGuiCallback(ArrowButtonDown));
		}

		OnChangeSize();
	}
	
	//-----------------------------------------------------------------------

	void cWidgetSlider::OnLoadGraphics()
	{
		/////////////////////////
		//Background, corners and borders
		mpGfxButtonBackground = mpSkin->GetGfx(eGuiSkinGfx_SliderHandleBackground);

		mvGfxBorders[0] = mpSkin->GetGfx(eGuiSkinGfx_SliderHandleBorderRight);
		mvGfxBorders[1] = mpSkin->GetGfx(eGuiSkinGfx_SliderHandleBorderLeft);
		mvGfxBorders[2] = mpSkin->GetGfx(eGuiSkinGfx_SliderHandleBorderUp);
		mvGfxBorders[3] = mpSkin->GetGfx(eGuiSkinGfx_SliderHandleBorderDown);

		mvGfxCorners[0] = mpSkin->GetGfx(eGuiSkinGfx_SliderHandleCornerLU);
		mvGfxCorners[1] = mpSkin->GetGfx(eGuiSkinGfx_SliderHandleCornerRU);
		mvGfxCorners[2] = mpSkin->GetGfx(eGuiSkinGfx_SliderHandleCornerRD);
		mvGfxCorners[3] = mpSkin->GetGfx(eGuiSkinGfx_SliderHandleCornerLD);
		
		//////////////////////////
		// Vertical
		if(mOrientation == eWidgetSliderOrientation_Vertical)
		{
			mvGfxArrow[0] = mpSkin->GetGfx(eGuiSkinGfx_SliderVertArrowUp);
			mvGfxArrow[1] = mpSkin->GetGfx(eGuiSkinGfx_SliderVertArrowDown);
			mpGfxBackground = mpSkin->GetGfx(eGuiSkinGfx_SliderVertBackground);
		}
		//////////////////////////
		// Horizontal
		else
		{
			mvGfxArrow[0] = mpSkin->GetGfx(eGuiSkinGfx_SliderHoriArrowUp);
			mvGfxArrow[1] = mpSkin->GetGfx(eGuiSkinGfx_SliderHoriArrowDown);
			mpGfxBackground = mpSkin->GetGfx(eGuiSkinGfx_SliderHoriBackground);
		}

		mfButtonSize = mpSkin->GetAttribute(eGuiSkinAttribute_SliderButtonSize).x;
	}

	//-----------------------------------------------------------------------

	void cWidgetSlider::OnChangeSize()
	{
		if(mvButtons[0] == NULL || mvButtons[1] == NULL) return;
		
		//////////////////////////
		// Vertical
		if(mOrientation == eWidgetSliderOrientation_Vertical)
		{
			mvButtons[0]->SetPosition(cVector3f(0,0,0.2f));
			mvButtons[0]->SetSize(cVector2f(mvSize.x, mfButtonSize));
			
			mvButtons[1]->SetPosition(cVector3f(0,mvSize.y-mfButtonSize,0.2f));
			mvButtons[1]->SetSize(cVector2f(mvSize.x, mfButtonSize));
		}
		//////////////////////////
		// Horizontal
		else
		{
			mvButtons[0]->SetPosition(cVector3f(0,0,0.2f));
			mvButtons[0]->SetSize(cVector2f(mfButtonSize,mvSize.y));

			mvButtons[1]->SetPosition(cVector3f(mvSize.x-mfButtonSize,0,0.2f));
			mvButtons[1]->SetSize(cVector2f(mfButtonSize, mvSize.y));
		}

		UpdateBarProperties();
	}
	
	//-----------------------------------------------------------------------

	void cWidgetSlider::OnChangePosition()
	{
		UpdateBarProperties();
	}

	//-----------------------------------------------------------------------

	void cWidgetSlider::OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion)
	{	
		////////////////////////////////
		// Background

		// Vertical
		if(mOrientation == eWidgetSliderOrientation_Vertical)
		{
			mpSet->DrawGfx(mpGfxBackground, GetGlobalPosition() + 
							cVector3f(0,mfButtonSize,0.1f),
							cVector2f(mvSize.x,mvSize.y - mfButtonSize*2));
		}
		// Horizontal
		else
		{
			mpSet->DrawGfx(mpGfxBackground, GetGlobalPosition() + 
				cVector3f(mfButtonSize,0,0.1f),
				cVector2f(mvSize.x - mfButtonSize*2,mvSize.y));
		}

		////////////////////////////////
		// Borders and button background
		DrawBordersAndCorners(	mpGfxButtonBackground, mvGfxBorders, mvGfxCorners, 
								GetGlobalPosition() + mvBarPos, mvBarSize);
	}

	//-----------------------------------------------------------------------
	
	bool cWidgetSlider::OnMouseMove(const cGuiMessageData& aData)
	{
		if(mbPressed)
		{
			int lVal;
			// Vertical
			if(mOrientation == eWidgetSliderOrientation_Vertical)
			{
				mvBarPos.y = WorldToLocalPosition(aData.mvPos).y + mvRelMousePos.y;

				if(mvBarPos.y > mfMaxPos) mvBarPos.y = mfMaxPos;
				if(mvBarPos.y < mfMinPos) mvBarPos.y = mfMinPos;

				mBarRect.y = GetGlobalPosition().y + mvBarPos.y;
				lVal = (int)((mvBarPos.y-mfButtonSize) / mfValueStep + 0.5f);
			}
			// Horizontal
			else
			{
				mvBarPos.x = WorldToLocalPosition(aData.mvPos).x + mvRelMousePos.x;

				if(mvBarPos.x > mfMaxPos) mvBarPos.x = mfMaxPos;
				if(mvBarPos.x < mfMinPos) mvBarPos.x = mfMinPos;

				mBarRect.x = GetGlobalPosition().x + mvBarPos.x;
				lVal = (int)((mvBarPos.x-mfButtonSize) / mfValueStep + 0.5f);
			}
			
			
			if(lVal>mlMaxValue)lVal = mlMaxValue;
			if(lVal<0)lVal = 0;
			if(lVal != mlValue)
			{
				mlValue = lVal;
				ProcessMessage(eGuiMessage_SliderMove, cGuiMessageData(mlValue));
			}
		}
		
		/*if(mbPressed && cMath::PointBoxCollision(aData.mvPos,mBarRect)==false)
		{
			mbPressed = false;
		}*/

		return true;
	}

	//-----------------------------------------------------------------------

	bool cWidgetSlider::OnMouseDown(const cGuiMessageData& aData)
	{
		if((aData.mlVal & eGuiMouseButton_WheelUp) || (aData.mlVal & eGuiMouseButton_WheelDown))
		{
			return false;
		}

		if(cMath::CheckPointInRectIntersection(aData.mvPos,mBarRect))
		{
			mbPressed = true;

			cVector3f vRel = mvBarPos - WorldToLocalPosition(aData.mvPos);
			mvRelMousePos.x = vRel.x;
			mvRelMousePos.y = vRel.y;
		}
		else
		{
			int lVal;
			float fValue =  mlBarClickValueAdd*mfValueStep;

			if(mOrientation == eWidgetSliderOrientation_Vertical)
			{
				if(WorldToLocalPosition(aData.mvPos.y).y < mvBarPos.y+mBarRect.h)
				{
					fValue*=-1;
				}

				mvBarPos.y += fValue;
				if(mvBarPos.y > mfMaxPos) mvBarPos.y = mfMaxPos;
				if(mvBarPos.y < mfMinPos) mvBarPos.y = mfMinPos;

				mBarRect.y = GetGlobalPosition().y + mvBarPos.y;
				lVal = (int)((mvBarPos.y-mfButtonSize) / mfValueStep + 0.5f);
			}
			else
			{
                if(WorldToLocalPosition(aData.mvPos.x).x < mvBarPos.x+mBarRect.w)
				{
					fValue*=-1;
				}
				
				mvBarPos.x += fValue;
				if(mvBarPos.x > mfMaxPos) mvBarPos.x = mfMaxPos;
				if(mvBarPos.x < mfMinPos) mvBarPos.x = mfMinPos;

				mBarRect.x = GetGlobalPosition().x + mvBarPos.x;
				lVal = (int)((mvBarPos.x-mfButtonSize) / mfValueStep + 0.5f);
			}
			if(lVal>mlMaxValue)lVal = mlMaxValue;
			if(lVal<0)lVal = 0;
			if(lVal != mlValue)
			{
				mlValue = lVal;
				ProcessMessage(eGuiMessage_SliderMove, cGuiMessageData(mlValue));
			}
			//mvRelMousePos.x = -mBarRect.w*0.5f;
			//mvRelMousePos.y = -mBarRect.h*0.5f;
		}
		
		return true;
	}

	//-----------------------------------------------------------------------

	bool cWidgetSlider::OnMouseUp(const cGuiMessageData& aData)
	{
		if(mbPressed)
		{
			mbPressed = false;
			UpdateBarProperties();
		}

		ProcessMessage(eGuiMessage_SliderRelease, cGuiMessageData(mlValue));
		
		return true;
	}

	//-----------------------------------------------------------------------

	bool cWidgetSlider::OnMouseEnter(const cGuiMessageData& aData)
	{
		return false;
	}
	
	//-----------------------------------------------------------------------
	
	bool cWidgetSlider::OnMouseLeave(const cGuiMessageData& aData)
	{
		return false;
	}

	bool cWidgetSlider::OnMouseDoubleClick(const cGuiMessageData& aData)
	{
		return false;
	}

	bool cWidgetSlider::OnLostFocus(const cGuiMessageData& aData)
	{
		if(mbPressed)
		{
			mbPressed = false;
			UpdateBarProperties();
		}

		return false;
	}

	//-----------------------------------------------------------------------

	bool cWidgetSlider::OnUIArrowPress(const cGuiMessageData& aData)
	{
		bool bRet = false;
		if(mOrientation==eWidgetSliderOrientation_Horizontal)
		{
			if(aData.mlVal==eUIArrow_Left)
				bRet = ArrowButtonDown(mvButtons[0], aData);

			if(aData.mlVal==eUIArrow_Right)
				bRet = ArrowButtonDown(mvButtons[1], aData);
		}
		else
		{
			if(aData.mlVal==eUIArrow_Up)
				bRet = ArrowButtonDown(mvButtons[0], aData);

			if(aData.mlVal==eUIArrow_Down)
				bRet = ArrowButtonDown(mvButtons[1], aData);
		}

		return bRet;
	}

	bool cWidgetSlider::OnUIArrowRelease(const cGuiMessageData& aData)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool cWidgetSlider::OnUIButtonPress(const cGuiMessageData& aData)
	{
		return false;
	}

	bool cWidgetSlider::OnUIButtonRelease(const cGuiMessageData& aData)
	{
		return false;
	}

	//-----------------------------------------------------------------------


}
