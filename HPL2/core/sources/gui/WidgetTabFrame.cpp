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

#include "gui/WidgetTabFrame.h"

#include "gui/GuiTypes.h"

#include "gui/Widget.h"
#include "gui/WidgetFrame.h"
#include "gui/WidgetButton.h"
#include "gui/GuiGfxElement.h"

#include "gui/GuiSet.h"
#include "gui/GuiSkin.h"

#include "graphics/FontData.h"

#include "math/Math.h"



namespace hpl
{

	//////////////////////////////////////////////////////////////////
	// Tab Label
	//////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------
	//-------------------------------------------------------------------
	//-------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////


	//-------------------------------------------------------------------

	cWidgetTabLabel::cWidgetTabLabel(cGuiSet* apSet, cGuiSkin* apSkin) : iWidget(eWidgetType_Label, apSet, apSkin)
	{
		mpGfxIcon = NULL;

		LoadGraphics();
	}

	//-------------------------------------------------------------------

	cWidgetTabLabel::~cWidgetTabLabel()
	{
	}

	//-------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////

	
	bool cWidgetTabLabel::OnMouseDown(const cGuiMessageData& aData)
	{
		cWidgetTabFrame* pFrame = mpParentTab->mpParentTabFrame;
		
		while (mvPosition.x + mvSize.x > pFrame->mvSize.x)
		{
			pFrame->ArrowButton_Pressed(pFrame->mvArrowButtons[1], cGuiMessageData());
		}

		mpParentTab->GetParentTabFrame()->SetTabOnTop(mpParentTab);

		return true;
	}

	//-------------------------------------------------------------------

	void cWidgetTabLabel::OnChangeText()
	{
		cWidgetTabFrame* pTabFrame = mpParentTab->mpParentTabFrame;
		float fBordersWidth = pTabFrame->mvGfxTabLabelBorders[0]->GetImageSize().x + 
								pTabFrame->mvGfxTabLabelBorders[1]->GetImageSize().x;
		float fNewWidth = mpDefaultFontType->GetLength(mpDefaultFont->mvSize, msText.c_str()) + 
								pTabFrame->mfTextLeftPadding + 
								pTabFrame->mfTextRightPadding + 
								fBordersWidth;

		if(mpGfxIcon!=NULL)
		{
			fNewWidth += mpGfxIcon->GetImageSize().x + 2*pTabFrame->mfIconPadding;
		}

		SetSize(cVector2f(fNewWidth,mvSize.y));
	}

	//-------------------------------------------------------------------

	bool cWidgetTabLabel::OnGetUINavFocus(const cGuiMessageData& aData)
	{
		mpParentTab->GetParentTabFrame()->SetTabOnTop(this->mpParentTab);

		return true;
	}

	//-------------------------------------------------------------------

	void cWidgetTabLabel::OnDraw(float afTimeStep, cGuiClipRegion* apClipRegion)
	{
		cWidgetTabFrame* pTabFrame = mpParentTab->mpParentTabFrame;
		cVector3f vTextPos = GetGlobalPosition() + cVector3f(pTabFrame->mfTextLeftPadding,2,0.1f);
		
		cGuiGfxElement* pBG;

		if(mpParentTab->IsOnTop())
		{
			pBG = pTabFrame->mpGfxTabLabelBGActive;
		}
		else
		{
			pBG = pTabFrame->mpGfxTabLabelBG;
		}

		
		if(mpGfxIcon!=NULL)
		{
			vTextPos.x += 2*pTabFrame->mfIconPadding + mpGfxIcon->GetImageSize().x;
		}

		DrawDefaultText(msText, vTextPos, eFontAlign_Left);
		DrawBordersAndCorners(pBG, 
								pTabFrame->mvGfxTabLabelBorders, 
								pTabFrame->mvGfxTabLabelCorners, GetGlobalPosition(), mvSize);
	}

	//-------------------------------------------------------------------

	void cWidgetTabLabel::OnLoadGraphics()
	{
		// TODO : Add nice gfx here
		
		// Debug
		//mpGfxIcon = mvGfxCorners[0];
	}

	//-------------------------------------------------------------------
	//-------------------------------------------------------------------
	//-------------------------------------------------------------------
	
	//////////////////////////////////////////////////////////////////
	// Tab
	//////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------
	//-------------------------------------------------------------------
	//-------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////


	cWidgetTab::cWidgetTab(cWidgetTabFrame* apParent) : iWidget(eWidgetType_Tab, apParent->GetSet(), apParent->GetSet()->GetSkin())
	{
		LoadGraphics();

		mpTabLabel = NULL;
		mpFrame = NULL;

		mpParentTabFrame = apParent;
		mlIndex = mpParentTabFrame->GetTabNum();
	}

	//-------------------------------------------------------------------

	cWidgetTab::~cWidgetTab()
	{
		if(mpSet->IsDestroyingSet()==false)
		{
			mpSet->DestroyWidget(mpTabLabel);
			mpSet->DestroyWidget(mpFrame);
		}
	}



	//-------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------

	void cWidgetTab::AttachChild(iWidget* apChild)
	{
		if(apChild==mpTabLabel || apChild==mpFrame)
			iWidget::AttachChild(apChild);
		else
            mpFrame->AttachChild(apChild);
	}

	//-------------------------------------------------------------------

	void cWidgetTab::RemoveChild(iWidget* apChild)
	{
		if(apChild==mpTabLabel || apChild==mpFrame)
			iWidget::RemoveChild(apChild);
		else
			mpFrame->RemoveChild(apChild);
	}

	//-------------------------------------------------------------------

	void cWidgetTab::SetLabelPosX(float afPosX)
	{
		cVector3f vNewPos = mpTabLabel->GetLocalPosition();
		vNewPos.x = afPosX;
		mpTabLabel->SetPosition(vNewPos);
	}

	//-------------------------------------------------------------------

	void cWidgetTab::SetIcon(cGuiGfxElement* apIcon)
	{
		mpTabLabel->mpGfxIcon = apIcon;
		mpTabLabel->OnChangeText();
	}

	//-------------------------------------------------------------------

	void cWidgetTab::SetOnTop(bool abX)
	{
		mbOnTop = abX;
		float fOffset = abX ? mpParentTabFrame->mfTabActiveOffset : mpParentTabFrame->mfTabInactiveOffset;
		cVector3f vPos = mpTabLabel->GetLocalPosition();
		vPos.y = -fOffset;
		
		mpTabLabel->SetPosition(vPos);
		mpFrame->SetEnabled(abX);
		mpFrame->SetVisible(abX);
	}

	//-------------------------------------------------------------------

	bool cWidgetTab::PointIsInside(const cVector2f& avPoint, bool abOnlyClipped)
	{
		if(CheckPointInsideClippingParent(avPoint)==false)
			return false;

		if(abOnlyClipped && mbClipsGraphics==false) return true;

		bool bLabelVisible = mpTabLabel->IsVisible();
		cVector3f vLabelGlobalPos = mpTabLabel->GetGlobalPosition();
		cVector2f vLabelSize = mpTabLabel->GetSize();

		cVector3f vFrameGlobalPos = mpFrame->GetGlobalPosition();
		cVector2f vFrameSize = mpFrame->GetSize();

		if( avPoint.x < vFrameGlobalPos.x || avPoint.x > vFrameGlobalPos.x + vFrameSize.x || 
			avPoint.y < vFrameGlobalPos.y || avPoint.y > vFrameGlobalPos.y + vFrameSize.y)
		{
			if( bLabelVisible==false || 
				avPoint.x < vLabelGlobalPos.x || avPoint.x > vLabelGlobalPos.x + vLabelSize.x || 
				avPoint.y < vLabelGlobalPos.y || avPoint.y > vLabelGlobalPos.y + vLabelSize.y) 
				return false;
			else
				return true;
		}
		else
		{
			return true;	
		}
	}

	//-------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------

	void cWidgetTab::OnChangeText()
	{
		if(mpTabLabel!=NULL) mpTabLabel->SetText(msText);
	}

	//-------------------------------------------------------------------

	void cWidgetTab::OnChangeSize()
	{
		if(mpTabLabel == NULL || mpFrame == NULL) return;

		float fTextHeight = mpDefaultFont->mvSize.y + 8;

		mpTabLabel->SetPosition(cVector3f(0,mpParentTabFrame->mfTabActiveOffset, 0.1f));
		
		mpFrame->SetPosition(cVector3f(0,fTextHeight-4 + mpParentTabFrame->mfTabActiveOffset, 0.1f));
		mpFrame->SetSize(mvSize - cVector2f(0, fTextHeight + mpParentTabFrame->mfTabActiveOffset));
	}

	//-------------------------------------------------------------------

	void cWidgetTab::OnInit()
	{
		float fLabelHeight = mpDefaultFont->mvSize.y+8;
		mpTabLabel = mpSet->CreateWidgetTabLabel(0,
												 cVector2f(0, fLabelHeight),
												 msText,
												 NULL);
		mpTabLabel->mpParentTab = this;

		//float fFrameOffset = cMath::Max(mpParentTabFrame->mfTabActiveOffset, mpParentTabFrame->mfTabInactiveOffset);
		float fFrameOffset = mpParentTabFrame->mfTabActiveOffset;
		cVector2f vCornerSize = cVector2f(mpParentTabFrame->mvGfxTabCorners[0]->GetActiveSize().x + 
										  mpParentTabFrame->mvGfxTabCorners[1]->GetActiveSize().x, 
										  mpParentTabFrame->mvGfxTabCorners[0]->GetActiveSize().y +
										  mpParentTabFrame->mvGfxTabCorners[2]->GetActiveSize().y);
								
								
								

		mpFrame = mpSet->CreateWidgetFrame(0,
											mvSize-cVector2f(vCornerSize.x,fLabelHeight+fFrameOffset+vCornerSize.y),
											false,
											NULL,
											mpParentTabFrame->mbHoriScrollEnabled, mpParentTabFrame->mbVertScrollEnabled);
		mpFrame->SetClipActive(true);

		AttachChild(mpTabLabel);
		AttachChild(mpFrame);
		mpTabLabel->SetPosition(cVector3f(0, 0, 0.1f));

		cVector3f vPos = cVector3f(mpParentTabFrame->mvGfxTabCorners[0]->GetActiveSize().x, mpParentTabFrame->mvGfxTabCorners[0]->GetActiveSize().y, 0);
		mpFrame->SetPosition(cVector3f(vPos.x,vPos.y + fLabelHeight+fFrameOffset-mpParentTabFrame->mvGfxTabCorners[0]->GetActiveSize().y,0));

		SetOnTop(false);
	}

	//-------------------------------------------------------------------

	void cWidgetTab::OnDraw(float afTimeStep, cGuiClipRegion* apClipRegion)
	{
		DrawBordersAndCorners(mpParentTabFrame->mpGfxTabBG, 
								mpParentTabFrame->mvGfxTabBorders, 
								mpParentTabFrame->mvGfxTabCorners, 
								mpFrame->GetGlobalPosition(), mpFrame->GetSize(), true);
	}

	//-------------------------------------------------------------------

	void cWidgetTab::OnLoadGraphics()
	{
	}

	//-------------------------------------------------------------------
	//-------------------------------------------------------------------
	//-------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////
	// Tab Frame
	//////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------
	//-------------------------------------------------------------------
	//-------------------------------------------------------------------

	//-------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------

	cWidgetTabFrame::cWidgetTabFrame(cGuiSet* apSet, cGuiSkin* apSkin) : iWidget(eWidgetType_TabFrame, apSet, apSkin)
	{
		mfBackgroundZ = 0;
		mfForegroundZ = 0.9f;

		mlFirstVisibleTab = 0;
		mlLastVisibleTab = 0;

		mpTopTab = NULL;

		mvArrowButtons[0] =NULL;
		mvArrowButtons[1] =NULL;

		mbHoriScrollEnabled = true;
		mbVertScrollEnabled = true;

		LoadGraphics();

		SetClipActive(true);
	}

	//-------------------------------------------------------------------

	cWidgetTabFrame::~cWidgetTabFrame()
	{
		if(mpSet->IsDestroyingSet()==false)
		{
			for(int i=0;i<2;++i)
				mpSet->DestroyWidget(mvArrowButtons[i]);
			ClearTabs();
		}
	}

	//-------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------

	cWidgetTab* cWidgetTabFrame::AddTab(const tWString& asTabCaption)
	{
		////////////////////////////////////////////////////////////////////
		// Create new tab, if first tab created, set as topmost
		// If not, check if fits in frame, and show scroll buttons if needed.
		cWidgetTab* pTab = mpSet->CreateWidgetTab(mvPosition,mvSize,_W(""), this);

		pTab->SetText(asTabCaption);
		pTab->SetPosition(cVector3f(0,0,mfBackgroundZ));
				
		if(mvTabs.empty())
			SetTabOnTop(pTab);
		
		mvTabs.push_back(pTab);

		UpdateTabVisibility();

		return pTab;
	}

	//-------------------------------------------------------------------

	void cWidgetTabFrame::ClearTabs()
	{
		for(tWidgetTabVectorIt it = mvTabs.begin(); it != mvTabs.end(); ++it)
			mpSet->DestroyWidget(*it);

		mvTabs.clear();
	}

	//-------------------------------------------------------------------

	int cWidgetTabFrame::GetTabNum()
	{
		return (int)mvTabs.size();
	}

	//-------------------------------------------------------------------

	cWidgetTab* cWidgetTabFrame::GetTab(int alIdx)
	{
		return mvTabs[alIdx];
	}

	//-------------------------------------------------------------------

	cWidgetTab* cWidgetTabFrame::GetTab(const tWString& asTabCaption)
	{
		for(int i=0;i<GetTabNum(); ++i)
		{
			cWidgetTab* pTab = GetTab(i);
			if(pTab->GetText()==asTabCaption)
				return pTab;
		}

		return NULL;
	}

	//-------------------------------------------------------------------

	//-------------------------------------------------------------------

	int cWidgetTabFrame::GetTabOnTopIndex()
	{
		int lIndex = -1;
		for(int i=0;i<(int)mvTabs.size();++i)
		{
			if(mpTopTab==mvTabs[i])
			{
				lIndex = i;
				break;
			}
		}

		return lIndex;
	}

	//-------------------------------------------------------------------

	void cWidgetTabFrame::SetTabOnTop(cWidgetTab* apTab)
	{
		/////////////////////////////////////////////
		// Move current top tab to the background, 
		// and then move apTab to top
		if(apTab == mpTopTab) return;

		cVector3f vNewPos;

		if(mpTopTab!=NULL)
		{
			vNewPos = mpTopTab->GetLocalPosition();
			vNewPos.z = mfBackgroundZ;
			mpTopTab->SetPosition(vNewPos);
			mpTopTab->SetOnTop(false);
		}

		mpTopTab = apTab;

		vNewPos = mpTopTab->GetLocalPosition();
		vNewPos.z = mfForegroundZ;
		mpTopTab->SetPosition(vNewPos);
		mpTopTab->SetOnTop(true);

		if(mvTabs.empty()==false)
			ProcessMessage(eGuiMessage_SelectionChange, cGuiMessageData(mpTopTab->GetIndex()));
	}

	//-------------------------------------------------------------------

	void cWidgetTabFrame::SetTabOnTopByIndex(int alIdx)
	{
		if(alIdx<0 || alIdx>=(int)mvTabs.size())
			return;

		SetTabOnTop(mvTabs[alIdx]);
	}

	//-------------------------------------------------------------------

	void cWidgetTabFrame::SetHorizontalScrollEnabled(bool abX)
	{
		if(mvTabs.empty())
			mbHoriScrollEnabled = abX;
	}

	//-------------------------------------------------------------------

	void cWidgetTabFrame::SetVerticalScrollEnabled(bool abX)
	{
		if(mvTabs.empty())
			mbVertScrollEnabled = abX;
	}

	//-------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------

	bool cWidgetTabFrame::OnUIButtonPress(const cGuiMessageData& aData)
	{
		int lTabOnTopIndex = GetTabOnTopIndex();

		if(aData.mlVal & eUIButton_PrevPage)
		{
			SetTabOnTopByIndex(--lTabOnTopIndex);
		}
		else if(aData.mlVal & eUIButton_NextPage)
		{
			SetTabOnTopByIndex(++lTabOnTopIndex);
		}
		else
			return false;

		return true;
	}

	//-------------------------------------------------------------------

	bool cWidgetTabFrame::ArrowButton_Pressed(iWidget* apWidget, const cGuiMessageData& aData)
	{
		///////////////////////////
		// Left Arrow Button
		if(apWidget==mvArrowButtons[0])
		{
			if(mlFirstVisibleTab>0)
				mlFirstVisibleTab--;
		}
		///////////////////////////
		// Right Arrow Button
		if(apWidget==mvArrowButtons[1])
		{
			if(mlLastVisibleTab < (int) mvTabs.size()-1)
				mlFirstVisibleTab++;
		}
		
		UpdateTabVisibility();
		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetTabFrame, ArrowButton_Pressed);

	//-------------------------------------------------------------------

	void cWidgetTabFrame::UpdateTabVisibility()
	{
		float fPosX = mfFirstTabOffset;
		
		///////////////////////////////
		// Hide tabs at the left of the first visible
		for(int i=0; i<mlFirstVisibleTab; ++i)
		{
			cWidgetTab* pTab = mvTabs[i];

			pTab->mpTabLabel->SetVisible(false);
			pTab->mpTabLabel->SetEnabled(false);
		}

		////////////////////////////////
		// Check how many tabs are visible
		for(int i=mlFirstVisibleTab; i<(int) mvTabs.size(); ++i)
		{
			cWidgetTab* pTab = mvTabs[i];

			pTab->mpTabLabel->SetVisible(true);
			pTab->mpTabLabel->SetEnabled(true);
			pTab->SetLabelPosX(fPosX);
				

			if(fPosX+pTab->mpTabLabel->GetSize().x < mvSize.x-(mfButtonSize*2+2))
                mlLastVisibleTab = i;

			fPosX += pTab->mpTabLabel->GetSize().x + mfTabSeparation;
		}

		////////////////////////////////
		// Update arrow buttons
		mvArrowButtons[0]->SetEnabled(mlFirstVisibleTab>0);
		mvArrowButtons[1]->SetEnabled(mlLastVisibleTab<(int)mvTabs.size()-1);

		bool bArrowsShown = (mlLastVisibleTab-mlFirstVisibleTab<(int)mvTabs.size()-1);
		mvArrowButtons[0]->SetVisible(bArrowsShown);
		mvArrowButtons[1]->SetVisible(bArrowsShown);
	}

	//-------------------------------------------------------------------

	void cWidgetTabFrame::SetScrollButtonsHidden(bool abX)
	{
		for(int i=0; i<2; ++i)
		{
			cWidgetButton* pArrow = mvArrowButtons[i];

			pArrow->SetEnabled(!abX);
			pArrow->SetVisible(!abX);
		}

	}
    
	//-------------------------------------------------------------------

	void cWidgetTabFrame::OnInit()
	{
		////////////////////////////////
		// Init Scroll buttons

		for(int i = 0; i < 2; ++i)
		{
			mvArrowButtons[i] = mpSet->CreateWidgetButton(0,0,_W(""),this);
			mvArrowButtons[i]->SetImage(mvGfxArrows[i],false);
			mvArrowButtons[i]->SetSize(mfButtonSize);
			mvArrowButtons[i]->AddCallback(eGuiMessage_ButtonPressed,this,kGuiCallback(ArrowButton_Pressed));	
		}

		OnChangeSize();
	}

	//-------------------------------------------------------------------

	void cWidgetTabFrame::OnChangeSize()
	{
		if(mvArrowButtons[0]==NULL || mvArrowButtons[1]==NULL) return;
		for(int i = 0; i<2; ++i)
			mvArrowButtons[i]->SetPosition(cVector3f(mvSize.x - (mfButtonSize+2)*(2-i), 0, mfForegroundZ+0.5f));

		for(int i=0; i<(int)mvTabs.size(); ++i)
			mvTabs[i]->SetSize(mvSize);

		UpdateTabVisibility();
	}

	//-------------------------------------------------------------------

	void cWidgetTabFrame::OnLoadGraphics()
	{
		//////////////////////////////////////////////
		// Attributes
		mfButtonSize = mpSkin->GetAttribute(eGuiSkinAttribute_TabFrameButtonSize).x;
		mfFirstTabOffset = mpSkin->GetAttribute(eGuiSkinAttribute_FirstTabOffset).x;
		mfTabSeparation = mpSkin->GetAttribute(eGuiSkinAttribute_TabSeparation).x;

		mfTabInactiveOffset = mpSkin->GetAttribute(eGuiSkinAttribute_TabFrameLabelInactiveOffset).x;
		mfTabActiveOffset = mpSkin->GetAttribute(eGuiSkinAttribute_TabFrameLabelActiveOffset).x;

		mfIconPadding = mpSkin->GetAttribute(eGuiSkinAttribute_TabFrameIconPadding).x;
		mfTextLeftPadding = mpSkin->GetAttribute(eGuiSkinAttribute_TabFrameTextRightPadding).x;
		mfTextRightPadding = mpSkin->GetAttribute(eGuiSkinAttribute_TabFrameTextLeftPadding).x;

		//////////////////////////////////////////////
		// Tab scroll buttons gfx
		mvGfxArrows[0] = mpSkin->GetGfx(eGuiSkinGfx_TabFrameArrowLeft);
		mvGfxArrows[1] = mpSkin->GetGfx(eGuiSkinGfx_TabFrameArrowRight);

		//////////////////////////////////////////////
		// Tab label gfx
		mvGfxTabLabelBorders[0] = mpSkin->GetGfx(eGuiSkinGfx_TabLabelBorderRight);
		mvGfxTabLabelBorders[1] = mpSkin->GetGfx(eGuiSkinGfx_TabLabelBorderLeft);
		mvGfxTabLabelBorders[2] = mpSkin->GetGfx(eGuiSkinGfx_TabLabelBorderUp);
		mvGfxTabLabelBorders[3] = mpSkin->GetGfx(eGuiSkinGfx_TabLabelBorderDown);

		mvGfxTabLabelCorners[0] = mpSkin->GetGfx(eGuiSkinGfx_TabLabelCornerLU);
		mvGfxTabLabelCorners[1] = mpSkin->GetGfx(eGuiSkinGfx_TabLabelCornerRU);
		mvGfxTabLabelCorners[2] = mpSkin->GetGfx(eGuiSkinGfx_TabLabelCornerRD);
		mvGfxTabLabelCorners[3] = mpSkin->GetGfx(eGuiSkinGfx_TabLabelCornerLD);
		
		mpGfxTabLabelBG = mpSkin->GetGfx(eGuiSkinGfx_TabLabelBackground);
		mpGfxTabLabelBGActive = mpSkin->GetGfx(eGuiSkinGfx_TabLabelBackgroundActive);
		
		//////////////////////////////////////////////
		// Tab Frame gfx
		mvGfxTabBorders[0] = mpSkin->GetGfx(eGuiSkinGfx_TabBorderRight);
		mvGfxTabBorders[1] = mpSkin->GetGfx(eGuiSkinGfx_TabBorderLeft);
		mvGfxTabBorders[2] = mpSkin->GetGfx(eGuiSkinGfx_TabBorderUp);
		mvGfxTabBorders[3] = mpSkin->GetGfx(eGuiSkinGfx_TabBorderDown);

		mvGfxTabCorners[0] = mpSkin->GetGfx(eGuiSkinGfx_TabCornerLU);
		mvGfxTabCorners[1] = mpSkin->GetGfx(eGuiSkinGfx_TabCornerRU);
		mvGfxTabCorners[2] = mpSkin->GetGfx(eGuiSkinGfx_TabCornerRD);
		mvGfxTabCorners[3] = mpSkin->GetGfx(eGuiSkinGfx_TabCornerLD);
		
		mpGfxTabBG = mpSkin->GetGfx(eGuiSkinGfx_TabBackground);
	}

	//-------------------------------------------------------------------

};

