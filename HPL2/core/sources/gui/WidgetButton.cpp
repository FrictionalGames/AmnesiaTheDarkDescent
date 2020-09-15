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

#include "gui/WidgetButton.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "system/String.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"

#include "graphics/FontData.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cWidgetButton::cWidgetButton(cGuiSet *apSet, cGuiSkin *apSkin) : iWidget(eWidgetType_Button,apSet, apSkin)
	{
		mbToggleable = false;
		mbPressed = false;
		mpImage = NULL;

		mfTimer = 0;
		mbRepeatActive = false;
		mfRepeatPeriod = 1;
		mlRepeatTimes = 0;
	}

	//-----------------------------------------------------------------------

	cWidgetButton::~cWidgetButton()
	{
		if(mpImage && mbDestroyImage)
		{
			mpGui->DestroyGfx(mpImage);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cWidgetButton::SetImage(cGuiGfxElement *apImage, bool abDestroyImage)
	{
		mpImage = apImage;
		mbDestroyImage = abDestroyImage;
	}

	//-----------------------------------------------------------------------

	void cWidgetButton::SetRepeatFreq(float afFreq)
	{
		mfRepeatPeriod = 1/afFreq;
	}

	void cWidgetButton::SetPressed(bool abX, bool abGenCallback)
	{
		if(mbToggleable)
		{
			mbPressed = abX;
			cGuiMessageData data;
			if(abGenCallback)
				ProcessMessage(eGuiMessage_ButtonPressed, data);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cWidgetButton::OnLoadGraphics()
	{
		/////////////////////////
		//Up
		mpGfxBackgroundUp = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpBackground);

		mvGfxBordersUp[0] = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderRight);
		mvGfxBordersUp[1] = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderLeft);
		mvGfxBordersUp[2] = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderUp);
		mvGfxBordersUp[3] = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpBorderDown);

		mvGfxCornersUp[0] = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerLU);
		mvGfxCornersUp[1] = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerRU);
		mvGfxCornersUp[2] = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerRD);
		mvGfxCornersUp[3] = mpSkin->GetGfx(eGuiSkinGfx_ButtonUpCornerLD);

		/////////////////////////
		//Down
		mpGfxBackgroundDown = mpSkin->GetGfx(eGuiSkinGfx_ButtonDownBackground);

		mvGfxBordersDown[0] = mpSkin->GetGfx(eGuiSkinGfx_ButtonDownBorderRight);
		mvGfxBordersDown[1] = mpSkin->GetGfx(eGuiSkinGfx_ButtonDownBorderLeft);
		mvGfxBordersDown[2] = mpSkin->GetGfx(eGuiSkinGfx_ButtonDownBorderUp);
		mvGfxBordersDown[3] = mpSkin->GetGfx(eGuiSkinGfx_ButtonDownBorderDown);

		mvGfxCornersDown[0] = mpSkin->GetGfx(eGuiSkinGfx_ButtonDownCornerLU);
		mvGfxCornersDown[1] = mpSkin->GetGfx(eGuiSkinGfx_ButtonDownCornerRU);
		mvGfxCornersDown[2] = mpSkin->GetGfx(eGuiSkinGfx_ButtonDownCornerRD);
		mvGfxCornersDown[3] = mpSkin->GetGfx(eGuiSkinGfx_ButtonDownCornerLD);
	}

	//-----------------------------------------------------------------------

	void cWidgetButton::OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion)
	{
		cVector3f vOffset = 0;
		if(mbPressed) vOffset = mpSkin->GetAttribute(eGuiSkinAttribute_ButtonPressedContentOffset);

		////////////////////////////////
		// Image
		if(mpImage)
		{
			cColor color = IsEnabled()?cColor(1):cColor(1,0.2f);
			mpSet->DrawGfx( mpImage,GetGlobalPosition()+
							cVector3f(	mvSize.x/2 - mpImage->GetImageSize().x/2, 
										mvSize.y/2 - mpImage->GetImageSize().y/2,0.2f) + vOffset,
							-1,
							color
							);
		}

		////////////////////////////////
		// Text
		if(IsEnabled())
		{
			//cColor col = this->HasFocus()?cColor(1,0,0,1):cColor(0,1);
			//cColor col = cColor(0,1);
			DrawDefaultText( msText,GetGlobalPosition()+
							cVector3f(mvSize.x/2, mvSize.y/2 - mvDefaultFontSize.y/2,0.5f)+vOffset,
							eFontAlign_Center, mDefaultFontColor);
		}
		else
		{
			DrawDefaultText( msText,GetGlobalPosition()+
							cVector3f(mvSize.x/2, mvSize.y/2 - mvDefaultFontSize.y/2,0.5f)+vOffset,
							eFontAlign_Center, cColor(0.5f,1));
			/*DrawSkinText( msText,eGuiSkinFont_Disabled, GetGlobalPosition()+
						cVector3f(mvSize.x/2, mvSize.y/2 - mvDefaultFontSize.y/2,0.5f)+vOffset,
						eFontAlign_Center);*/
		}
		

		////////////////////////////////
		// Borders and background
		if(mbPressed)
		{
			DrawBordersAndCorners(	mpGfxBackgroundDown, mvGfxBordersDown, mvGfxCornersDown, 
									GetGlobalPosition(), mvSize);
		}
		else
		{
			DrawBordersAndCorners(	mpGfxBackgroundUp, mvGfxBordersUp, mvGfxCornersUp, 
									GetGlobalPosition(), mvSize);
		}
	}

	//-----------------------------------------------------------------------

	void cWidgetButton::OnUpdate(float afTimeStep)
	{
		if(mbPressed && mbRepeatActive)
		{
			mfTimer+=afTimeStep;

			if(mfTimer>mfRepeatPeriod)
			{
				mfTimer=0;
				mlRepeatTimes++;

				ProcessMessage(eGuiMessage_ButtonPressed, mPressedData);
			}

		}
	}

	//-----------------------------------------------------------------------
	
	bool cWidgetButton::OnMouseMove(const cGuiMessageData& aData)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool cWidgetButton::OnMouseDown(const cGuiMessageData& aData)
	{
		if(IsEnabled() && aData.mlVal==eGuiMouseButton_Left)
		{
			if(mbToggleable)
			{
				mbPressed = !mbPressed;
				ProcessMessage(eGuiMessage_ButtonPressed,aData);
			}
			else
				mbPressed = true;

			mPressedData = aData;

			return true;
		}
		return false;
	}

	//-----------------------------------------------------------------------

	bool cWidgetButton::OnMouseUp(const cGuiMessageData& aData)
	{
		if(IsEnabled() && aData.mlVal==eGuiMouseButton_Left)
		{
			if(GetMouseIsOver()==false) return false;

			if(mbToggleable==false)
			{
				if(mbPressed && mlRepeatTimes==0) ProcessMessage(eGuiMessage_ButtonPressed,aData);
				mbPressed = false;
				mlRepeatTimes = 0;
			}

			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------

	bool cWidgetButton::OnMouseEnter(const cGuiMessageData& aData)
	{
		return false;
	}
	
	//-----------------------------------------------------------------------
	
	bool cWidgetButton::OnMouseLeave(const cGuiMessageData& aData)
	{
		if(mbToggleable==false)
			mbPressed = false;

		return false;
	}

	//-----------------------------------------------------------------------

	bool cWidgetButton::OnLostFocus(const cGuiMessageData& aData)
	{
		if(mbToggleable==false)
			mbPressed = false;

		return false;
	}

	//-----------------------------------------------------------------------

	bool cWidgetButton::OnUIButtonPress(const cGuiMessageData& aData)
	{
		if(HasFocus())
		{
			if(aData.mlVal==eUIButton_Primary)
				return OnMouseDown(cGuiMessageData(eGuiMouseButton_Left));
		}
		
		return false;
	}

	bool cWidgetButton::OnUIButtonRelease(const cGuiMessageData& aData)
	{
		if(HasFocus())
		{
			if(aData.mlVal==eUIButton_Primary)
			{
				// Lil hack: make widget think mouse is over.
				bool bPrevMouseOver = mbMouseIsOver;
				mbMouseIsOver = true;
				{
					OnMouseUp(cGuiMessageData(eGuiMouseButton_Left));
				}
				mbMouseIsOver = bPrevMouseOver;

				return true;
			}
		}

		return false;
	}

	//-----------------------------------------------------------------------

}
