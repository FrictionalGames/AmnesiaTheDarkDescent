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

#include "gui/WidgetWindow.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"

#include "gui/WidgetButton.h"

#include "resources/Resources.h"
#include "resources/FontManager.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cWidgetWindow::cWidgetWindow(cGuiSet *apSet, cGuiSkin *apSkin, tWidgetWindowButtonFlag alFlags) : iWidget(eWidgetType_Window,apSet, apSkin)
	{
		mpPrevAttention = NULL;
		mvRelMousePos =0;
		mbMoving = false;
		mbStatic = false;
		mbFocused = false;
		mbCloseButtonDisablesWindow = true;
		mbCloseOnEscapeKey = false;
		mlFlags = alFlags;

		mbDrawLabel = true;

		SetGlobalKeyPressListener(true);

		for(int i=0; i<1; ++i) mvButtons[i] = NULL;

		LoadGraphics();
	}

	//-----------------------------------------------------------------------

	cWidgetWindow::~cWidgetWindow()
	{
		mpSet->RemoveWindow(this);

		if(mpSet->IsDestroyingSet()==false)
		{
			for(int i=0; i<1; ++i) if(mvButtons[i]) mpSet->DestroyWidget(mvButtons[i]);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cWidgetWindow::SetStatic(bool abX)
	{
		mbStatic = abX;

		if(mbStatic==false)
		{
			mbMoving = false;
		}
	}

	//-----------------------------------------------------------------------
	
	void cWidgetWindow::SetDrawLabel(bool abX)
	{
		mbDrawLabel = abX;
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////


	//-----------------------------------------------------------------------

	bool cWidgetWindow::ButtonPressed(iWidget* apWidget, const cGuiMessageData& aData)
	{	
		int i;
		for(i=0; i<1; ++i) if(mvButtons[i] == apWidget) break;


		if(i == 0)			// Close Button
		{
			ProcessMessage(eGuiMessage_WindowClose, cGuiMessageData());
			
			if(mbCloseButtonDisablesWindow)
			{
				SetEnabled(false);
				SetVisible(false);
				if(mpSet->GetAttentionWidget() == this) mpSet->SetAttentionWidget(NULL);
			}
		}
		
		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetWindow,ButtonPressed)

	//-----------------------------------------------------------------------

	void cWidgetWindow::OnInit()
	{
		if(mlFlags & eWidgetWindowButtonFlag_ButtonClose)
		{
			mvButtons[0] = mpSet->CreateWidgetButton(0,0,_W(""), this);
			mvButtons[0]->SetImage(mpGfxButtonCross,false);
			mvButtons[0]->SetSize(cVector2f(mfButtonSize));
			mvButtons[0]->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(ButtonPressed));

			OnChangeSize();
		}
	}

	//-----------------------------------------------------------------------

	void cWidgetWindow::OnLoadGraphics()
	{
		mpGfxBackground = mpSkin->GetGfx(eGuiSkinGfx_WindowBackground);

		mpGfxLabel = mpSkin->GetGfx(eGuiSkinGfx_WindowLabel);

		mvGfxBorders[0] = mpSkin->GetGfx(eGuiSkinGfx_WindowBorderRight);
		mvGfxBorders[1] = mpSkin->GetGfx(eGuiSkinGfx_WindowBorderLeft);
		mvGfxBorders[2] = mpSkin->GetGfx(eGuiSkinGfx_WindowBorderUp);
		mvGfxBorders[3] = mpSkin->GetGfx(eGuiSkinGfx_WindowBorderDown);

		mvGfxCorners[0] = mpSkin->GetGfx(eGuiSkinGfx_WindowCornerLU);
		mvGfxCorners[1] = mpSkin->GetGfx(eGuiSkinGfx_WindowCornerRU);
		mvGfxCorners[2] = mpSkin->GetGfx(eGuiSkinGfx_WindowCornerRD);
		mvGfxCorners[3] = mpSkin->GetGfx(eGuiSkinGfx_WindowCornerLD);

		mpLabelFont = mpSkin->GetFont(eGuiSkinFont_WindowLabel);

		mvLabelTextOffset = mpSkin->GetAttribute(eGuiSkinAttribute_WindowLabelTextOffset);

		mpGfxButtonCross = mpSkin->GetGfx(eGuiSkinGfx_WindowButtonCross);

		mfButtonSize = mpSkin->GetAttribute(eGuiSkinAttribute_WindowButtonSize).x;

		mbDrawBordersOutside = (mpSkin->GetAttribute(eGuiSkinAttribute_WindowBordersOutside).x==1);
	}

	//-----------------------------------------------------------------------

	void cWidgetWindow::OnAttachChild(iWidget* apChild)
	{
		if(apChild->GetType()==eWidgetType_MainMenu)
		{
			////////////////////////////////
			// Calc label size
			cVector2f vLabelSize;
			vLabelSize.x = mvSize.x - (mvGfxCorners[0]->GetActiveSize().x + mvGfxCorners[1]->GetActiveSize().x);
			vLabelSize.y = mpLabelFont->mvSize.y + mvLabelTextOffset.y*2;

			apChild->SetPosition(cVector3f(0,vLabelSize.y+3,0.1f));
		}
	}

	//-----------------------------------------------------------------------

	void cWidgetWindow::OnChangeSize()
	{
		if(mvButtons[0])
		{
			mvButtons[0]->SetPosition(cVector3f(mvSize.x - mfButtonSize - 5 ,5,0.2f));
		}
	}

	//-----------------------------------------------------------------------

	void cWidgetWindow::OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion)
	{
		////////////////////////////////
		// Calc label size
		cVector2f vLabelSize;
		vLabelSize.x = mvSize.x - (mvGfxCorners[0]->GetActiveSize().x + mvGfxCorners[1]->GetActiveSize().x);
		vLabelSize.y = mpLabelFont->mvSize.y + mvLabelTextOffset.y*2;

		////////////////////////////////
		// Label
		if(mbDrawLabel)
		{
			cColor col = mbFocused? cColor(1,1) : cColor(0.6f,0.3f);
			mpSet->DrawGfx(	mpGfxLabel,GetGlobalPosition() + 
							cVector3f(mvGfxCorners[0]->GetActiveSize().x,mvGfxCorners[0]->GetActiveSize().y,0.2f),
							vLabelSize, col);
		}

		////////////////////////////////
		// Label text
		DrawSkinText(	msText,eGuiSkinFont_WindowLabel,GetGlobalPosition()+
						cVector3f(	mvGfxCorners[0]->GetActiveSize().x+mvLabelTextOffset.x,
									mvGfxCorners[0]->GetActiveSize().y+mvLabelTextOffset.y,0.4f));

		////////////////////////////////
		// Borders and background
		DrawBordersAndCorners(	mpGfxBackground, mvGfxBorders, mvGfxCorners, GetGlobalPosition(), mvSize, mbDrawBordersOutside);
	}

	//-----------------------------------------------------------------------
	
	bool cWidgetWindow::OnMouseMove(const cGuiMessageData& aData)
	{
		if(mbMoving) SetGlobalPosition(mvRelMousePos + cVector3f(aData.mvPos.x, aData.mvPos.y,0));
		return true;
	}

	//-----------------------------------------------------------------------

	bool cWidgetWindow::OnMouseDown(const cGuiMessageData& aData)
	{
		if(mbStatic) return false;

		////////////////////////////////
		// Calculate label rectangle
		cRect2f labelRect;
		labelRect.w = mvSize.x - (mvGfxCorners[0]->GetActiveSize().x + mvGfxCorners[1]->GetActiveSize().x);
		labelRect.h = mpLabelFont->mvSize.y + mvLabelTextOffset.y*2;
		labelRect.x = GetGlobalPosition().x + mvGfxCorners[0]->GetActiveSize().x;
		labelRect.y = GetGlobalPosition().y + mvGfxCorners[0]->GetActiveSize().y;
		
		////////////////////////////////
		// Check for collision
		if(cMath::CheckPointInRectIntersection(aData.mvPos,labelRect) && aData.mlVal & eGuiMouseButton_Left)
		{
			mbMoving = true;
			mpPrevAttention = mpSet->GetAttentionWidget();
			mpSet->SetAttentionWidget(this);
			mvRelMousePos = GetPosRelativeToMouse(aData);
			mvRelMousePos.z = GetGlobalPosition().z;
		}
		
		return true;
	}

	//-----------------------------------------------------------------------

	bool cWidgetWindow::OnMouseUp(const cGuiMessageData& aData)
	{
		if(aData.mlVal & eGuiMouseButton_Left)
		{
			if(mbMoving)
			{
				mpSet->SetAttentionWidget(mpPrevAttention);
				mpSet->SetFocusedWidget(this);
				mbMoving = false;
			}
		}
		
		return true;
	}

	//-----------------------------------------------------------------------

	bool cWidgetWindow::OnMouseEnter(const cGuiMessageData& aData)
	{
		return false;
	}
	
	//-----------------------------------------------------------------------
	
	bool cWidgetWindow::OnMouseLeave(const cGuiMessageData& aData)
	{
		cVector3f vLastGlobal = GetGlobalPosition();
		
		if(mbMoving)
		{
			SetGlobalPosition(mvRelMousePos + cVector3f(aData.mvPos.x, aData.mvPos.y,0));

			//Check so that mouse is not outside of clip area.
			if(PointIsInside(aData.mvPos, false)==false)
			{
				SetGlobalPosition(vLastGlobal);
				mbMoving = false;
			}
		}
		
		return false;
	}

	//-----------------------------------------------------------------------

	bool cWidgetWindow::OnKeyPress(const cGuiMessageData& aData)
	{
		if(mbFocused==false || mbCloseOnEscapeKey==false)
			return false;

		if(aData.mKeyPress.mKey==eKey_Escape && aData.mKeyPress.mlModifier==eKeyModifier_None)
			return ProcessMessage(eGuiMessage_WindowClose, aData);

		return false;
	}

	//-----------------------------------------------------------------------

	bool cWidgetWindow::OnGotFocus(const cGuiMessageData& aData)
	{
		OnChildGotFocus(NULL, aData);
		return true;
	}

	//-----------------------------------------------------------------------

	bool cWidgetWindow::OnLostFocus(const cGuiMessageData& aData)
	{
		if(mbMoving)
			return true;

		OnChildLostFocus(NULL, aData);

		return true;
	}

	//-----------------------------------------------------------------------

	void cWidgetWindow::OnChildGotFocus(iWidget* apChild, const cGuiMessageData& aData)
	{
		mpSet->SetWindowOnTop(this);

		mbFocused = true;
	}

	//-----------------------------------------------------------------------

	void cWidgetWindow::OnChildLostFocus(iWidget* apChild, const cGuiMessageData& aData)
	{
		iWidget* pWidget = mpSet->GetFocusedWidget();
		if(pWidget==NULL || IsConnectedTo(pWidget,false)==false && pWidget->IsConnectedTo(this, false)==false)
			mbFocused = false;
	}

	//-----------------------------------------------------------------------


}
