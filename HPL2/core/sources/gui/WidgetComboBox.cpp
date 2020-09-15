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

#include "gui/WidgetComboBox.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"
#include "gui/GuiGfxElement.h"

#include "gui/WidgetTextBox.h"
#include "gui/WidgetButton.h"
#include "gui/WidgetSlider.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cWidgetComboBox::cWidgetComboBox(cGuiSet *apSet, cGuiSkin *apSkin) : iWidget(eWidgetType_ComboBox,apSet, apSkin)
	{
		mfButtonWidth = mpSkin->GetAttribute(eGuiSkinAttribute_ComboBoxButtonWidth).x;
		mfSliderWidth = mpSkin->GetAttribute(eGuiSkinAttribute_ComboBoxSliderWidth).x;

		mpText = NULL;
		mpButton = NULL;
		mpSlider = NULL;

		mbMenuOpen = false;

		mbOpenByUIButton = false;

		//mbConnectedToChildren = false;

		mfMenuHeight = 0;

		mlSelectedItem = -1;
		mlMouseOverSelection =-1;

		mlFirstItem =0;
		mlMaxItems =12;
		mlItemsShown = 0;
	}

	//-----------------------------------------------------------------------

	cWidgetComboBox::~cWidgetComboBox()
	{
		if(mpSet->IsDestroyingSet()==false)
		{
			mpSet->DestroyWidget(mpText);
			mpSet->DestroyWidget(mpButton);
			mpSet->DestroyWidget(mpSlider);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void cWidgetComboBox::SetSelectedItem(int alX, bool abMoveList, bool abGenCallback)
	{
		if(alX >= GetItemNum() || 
			mlSelectedItem == alX) return;

		mlSelectedItem = alX;

		if(abMoveList && mlSelectedItem >= mlFirstItem + mlMaxItems)
		{
			while(mlSelectedItem >= mlFirstItem + mlMaxItems)
			{
				mlFirstItem++;
			}
			mpSlider->SetValue(mlFirstItem);
		}
		if(abMoveList && mlSelectedItem < mlFirstItem && mlSelectedItem >=0)
		{
			while(mlSelectedItem < mlFirstItem)
			{
				mlFirstItem--;
			}
			mpSlider->SetValue(mlSelectedItem);
		}
		
		if(mlSelectedItem >=0 && mlSelectedItem < GetItemNum())
			SetText(mvItems[mlSelectedItem]->GetText());
		else
			SetText(_W(""));

		if(abGenCallback)
			ProcessMessage(eGuiMessage_SelectionChange, cGuiMessageData(mlSelectedItem));
	}

	//-----------------------------------------------------------------------

	void cWidgetComboBox::SetCanEdit(bool abX)
	{
		mpText->SetCanEdit(abX);
	}

	bool cWidgetComboBox::GetCanEdit()
	{
		return mpText->GetCanEdit();
	}

	//-----------------------------------------------------------------------

	void cWidgetComboBox::SetMaxShownItems(int alX)
	{
		mlMaxItems = alX;

		UpdateProperties();
	}

	//-----------------------------------------------------------------------

	void cWidgetComboBox::SetDefaultFontSize(const cVector2f& avSize)
	{
		iWidget::SetDefaultFontSize(avSize);
		mpText->SetDefaultFontSize(avSize);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------

	void cWidgetComboBox::UpdateProperties()
	{
		if((int)mvItems.size() <= mlMaxItems)
			mlItemsShown = (int)mvItems.size();
		else
			mlItemsShown = mlMaxItems;

		mfMenuHeight = 2 + (mvDefaultFontSize.y+2)* (float)mlItemsShown + 2;

		if(mlSelectedItem>=(int)mvItems.size())
			SetSelectedItem((int)mvItems.size()-1);
		
		OnChangeSize();
	}
	
	//-----------------------------------------------------------------------

	void cWidgetComboBox::OpenMenu()
	{
		if(mvItems.empty()) return;

		if(mbMenuOpen) return;
		
		mpSet->PushAttentionWidget();
		mpSet->PushFocusedWidget();
		mpSet->PushDefaultFocusNavWidget();

		mpSet->SetAttentionWidget(this);
		mbMenuOpen = true;

		mlMouseOverSelection = mlSelectedItem;

		mvMenuPos = cVector3f(0, 
							  mvSize.y+mvGfxBorders[1]->GetActiveSize().y,
							  mpSet->GetPopUpZ()+1);

		if(GetGlobalPosition().y+mvMenuPos.y+mfMenuHeight>mpSet->GetVirtualSize().y)
		{
			float fDiff = GetGlobalPosition().y+mvMenuPos.y+mfMenuHeight-mpSet->GetVirtualSize().y;
			mvMenuPos.y-=fDiff;
		}
		
		if((int)mvItems.size() > mlMaxItems)
		{
			mpSlider->SetEnabled(true);
			mpSlider->SetVisible(true);

			mpSlider->SetPosition(mvMenuPos + cVector3f(mvSize.x - 20 - mvGfxBorders[1]->GetActiveSize().x,1,0.2f));
			mpSlider->SetSize(cVector2f(20,mfMenuHeight - mvGfxBorders[1]->GetActiveSize().y -
														mvGfxBorders[2]->GetActiveSize().y));

			//mpSlider->SetMaxValue(GetItemNum());
			//mpSlider->SetBarValueSize(mlMaxItems);

			mpSet->SetFocusedWidget(this);
		}
		else
		{
			mpSet->SetFocusedWidget(this);
		}

		// This sets clip active in a quite sneaky way, why that?
		//mbClipsGraphics = true;
	}

	//-----------------------------------------------------------------------

	void cWidgetComboBox::CloseMenu()
	{
		if(mbMenuOpen==false) return;

		mbMenuOpen = false;

		mpSet->PopAttentionWidget();
		mpSet->PopFocusedWidget();
		mpSet->PopDefaultFocusNavWidget();

		mpSlider->SetEnabled(false);
		mpSlider->SetVisible(false);

		mbClipsGraphics = false;
	}
	
	//-----------------------------------------------------------------------

	bool cWidgetComboBox::ButtonPress(iWidget* apWidget, const cGuiMessageData& aData)
	{
		if(aData.mlVal==eGuiMouseButton_Left)
		{
			if(mbMenuOpen)
			{
				cVector3f vLocalPos = WorldToLocalPosition(aData.mvPos);
				if(vLocalPos.x >= mvMenuPos.x && vLocalPos.y <= mvMenuPos.x + mvSize.x &&
					vLocalPos.y >= mvMenuPos.y && vLocalPos.y <= mvMenuPos.y + mfMenuHeight)
				{
					return OnMouseDown(aData) && OnMouseUp(aData);
				}
				else
					CloseMenu();
			}
			else
			{
				OpenMenu();
				mbClickedAfterOpen = false;
			}
			return true;
		}
		return false;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetComboBox,ButtonPress)

	//-----------------------------------------------------------------------

	bool cWidgetComboBox::SliderMove(iWidget* apWidget, const cGuiMessageData& aData)
	{
		mlFirstItem = aData.mlVal;

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetComboBox,SliderMove)

	bool cWidgetComboBox::SliderLostFocus(iWidget* apWidget, const cGuiMessageData& aData)
	{
		if(mbMenuOpen && GetMouseIsOver()==false)
		{
			CloseMenu();
		}

		return false;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetComboBox,SliderLostFocus)

	//-----------------------------------------------------------------------

	void cWidgetComboBox::OnInit()
	{
		mpText = mpSet->CreateWidgetTextBox(0,mvSize,_W(""),this);
		mpText->SetText(msText);
		mpText->SetCanEdit(false);
		
		mpButton = mpSet->CreateWidgetButton(0,0,_W(""),this);
		mpButton->SetImage(mpSkin->GetGfx(eGuiSkinGfx_ComboBoxButtonIcon), false);
		mpButton->AddCallback(eGuiMessage_MouseDown, this, kGuiCallback(ButtonPress));

		mpSlider = mpSet->CreateWidgetSlider(eWidgetSliderOrientation_Vertical,0,0,0,this);
		mpSlider->AddCallback(eGuiMessage_SliderMove,this,kGuiCallback(SliderMove));
		mpSlider->AddCallback(eGuiMessage_LostFocus,this,kGuiCallback(SliderLostFocus));
		mpSlider->AddCallback(eGuiMessage_UIArrowPress, this, kGuiCallback(SliderOnUIArrowPress));
		mpSlider->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(SliderOnUIButtonPress));
		mpSlider->AddCallback(eGuiMessage_UIButtonRelease, this, kGuiCallback(SliderOnUIButtonRelease));
		mpSlider->SetEnabled(false);
		mpSlider->SetVisible(false);



		AddCallback(eGuiMessage_OnDraw,this, kGuiCallback(DrawText));
	
		OnChangeSize();
	}

	//-----------------------------------------------------------------------

	void cWidgetComboBox::OnChangeSize()
	{
		if(mpText && mpButton && mpSlider)
		{
			mpText->SetSize(mvSize);
			mvSize = mpText->GetSize();	

			cVector2f vBackSize = mpText->GetBackgroundSize();
			
			mpButton->SetSize(cVector2f(mfButtonWidth,vBackSize.y));
			mpButton->SetPosition(cVector3f( mvSize.x - (mvSize.x - vBackSize.x)/2 - mfButtonWidth,
											(mvSize.y - vBackSize.y)/2, 0.3f));

			mpSlider->SetBarValueSize(mlMaxItems);
			mpSlider->SetMaxValue((int)mvItems.size() - mlMaxItems);
		}
	}

	//-----------------------------------------------------------------------

	void cWidgetComboBox::OnChangeText()
	{
		if(mpText) mpText->SetText(msText);
	}

	//-----------------------------------------------------------------------

	void cWidgetComboBox::OnLoadGraphics()
	{
		mpGfxBackground = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxBackground);

		mpGfxSelection = mpSkin->GetGfx(eGuiSkinGfx_TextBoxSelectedTextBack);

		mvGfxBorders[0] = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxBorderRight);
		mvGfxBorders[1] = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxBorderLeft);
		mvGfxBorders[2] = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxBorderUp);
		mvGfxBorders[3] = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxBorderDown);

		mvGfxCorners[0] = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxCornerLU);
		mvGfxCorners[1] = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxCornerRU);
		mvGfxCorners[2] = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxCornerRD);
		mvGfxCorners[3] = mpSkin->GetGfx(eGuiSkinGfx_ComboBoxCornerLD);
	}

	//-----------------------------------------------------------------------

	void cWidgetComboBox::OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion)
	{
		if(mbMenuOpen)
		{
			///////////////////////////////
			// Set up base as clip region
			cGuiClipRegion *pPrevClipRegion = mpSet->GetCurrentClipRegion();
			mpSet->SetCurrentClipRegion(mpSet->GetBaseClipRegion());

			////////////////////////////
			//Draw background
			mpSet->DrawGfx(mpGfxBackground,GetGlobalPosition() + mvMenuPos,
							cVector2f(mvSize.x, mfMenuHeight));

			////////////////////////////////
			// Draw borders
			DrawBordersAndCorners(	NULL, mvGfxBorders, mvGfxCorners, 
									GetGlobalPosition() + mvMenuPos + cVector3f(0,0,0.2f),
									cVector2f(mvSize.x, mfMenuHeight));

			///////////////////////////////
			// Reset clip region
			mpSet->SetCurrentClipRegion(pPrevClipRegion);
		}
	}

	//-----------------------------------------------------------------------

	bool cWidgetComboBox::DrawText(iWidget* apWidget, const cGuiMessageData& aData)
	{
		if(mbMenuOpen==false) return false;

		///////////////////////////////
		// Set up base as clip region
		cGuiClipRegion *pPrevClipRegion = mpSet->GetCurrentClipRegion();
		mpSet->SetCurrentClipRegion(mpSet->GetBaseClipRegion());

		///////////////////////////////
		// Draw text entries
		cVector3f vPos = GetGlobalPosition() + 
			mvMenuPos + cVector3f(3,2,0.2f);
		
		for(int i=mlFirstItem; i<(int)mvItems.size(); ++i)
		{
			if(i-mlFirstItem >= mlMaxItems) break;

			if(i == mlMouseOverSelection)
			{
				mpSet->DrawGfx(mpGfxSelection,vPos - cVector3f(3,0,0.1f),
								cVector2f(mvSize.x,mvDefaultFontSize.y));
				DrawDefaultTextHighlight(mvItems[i]->GetText(),vPos,eFontAlign_Left);
			}
			else
				DrawDefaultText(mvItems[i]->GetText(),vPos,eFontAlign_Left);
			vPos.y += mvDefaultFontSize.y +2;
		}

		///////////////////////////////
		// Reset clip region
		mpSet->SetCurrentClipRegion(pPrevClipRegion);


		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetComboBox,DrawText)
	
	//-----------------------------------------------------------------------
	
	bool cWidgetComboBox::OnMouseMove(const cGuiMessageData& aData)
	{
		if(mbMenuOpen==false) return false;

		if(GetMouseIsOver()==false) return false;

		cVector3f vLocalPos = WorldToLocalPosition(aData.mvPos);

		if(vLocalPos.y <= mvMenuPos.y) return false;
		if(mpSlider->IsEnabled() && vLocalPos.x >= mvSize.x - 20) return false;

		float fToTextStart = 2 + mvMenuPos.y + mvGfxCorners[0]->GetActiveSize().y;
		int lSelection = (int)(( vLocalPos.y - fToTextStart) / (mvDefaultFontSize.y+2));
		if(lSelection <0) lSelection =0;

		lSelection = lSelection + mlFirstItem;

		if(lSelection >= (int)mvItems.size()) lSelection = (int)mvItems.size()-1;

		mlMouseOverSelection = lSelection;

		return true;
	}

	//-----------------------------------------------------------------------

	bool cWidgetComboBox::OnMouseDown(const cGuiMessageData& aData)
	{
		if(mbMenuOpen == false) return false;

		mbClickedAfterOpen = true;

		int lSliderValue = mpSlider->GetValue();
		int lValueAdd = mpSlider->GetButtonValueAdd();

		if(aData.mlVal & eGuiMouseButton_WheelUp)
			mpSlider->SetValue(lSliderValue-lValueAdd);
		else if(aData.mlVal & eGuiMouseButton_WheelDown)
			mpSlider->SetValue(lSliderValue+lValueAdd);
		else
		{
			cVector3f vLocal = WorldToLocalPosition(aData.mvPos);
	        
			if(vLocal.y < mvMenuPos.y || vLocal.y > mvMenuPos.y + mfMenuHeight)
			{
				CloseMenu();
			}
		}
	
		return true;
	}

	//-----------------------------------------------------------------------

	bool cWidgetComboBox::OnMouseUp(const cGuiMessageData& aData)
	{
		if(mbMenuOpen == false || mbClickedAfterOpen == false) return false;
		if((aData.mlVal&eGuiMouseButton_WheelUp) || (aData.mlVal & eGuiMouseButton_WheelDown))
            return false;

		cVector3f vLocal = WorldToLocalPosition(aData.mvPos);

		if(vLocal.y > mvMenuPos.y)
		{
			SetSelectedItem(mlMouseOverSelection);
			CloseMenu();
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool cWidgetComboBox::OnMouseEnter(const cGuiMessageData& aData)
	{
		return false;
	}
	
	//-----------------------------------------------------------------------
	
	bool cWidgetComboBox::OnMouseLeave(const cGuiMessageData& aData)
	{
		return false;
	}

	//-----------------------------------------------------------------------

	bool cWidgetComboBox::OnUIArrowPress(const cGuiMessageData& aData)
	{
		if(mbMenuOpen)
		{
			if(aData.mlVal==eUIArrow_Down)
			{
				if(mlMouseOverSelection+1 <= GetItemNum()-1) mlMouseOverSelection++;
			}
			else if(aData.mlVal==eUIArrow_Up)
			{
				if(mlMouseOverSelection-1 >= 0)	mlMouseOverSelection--;
			}
		
			int lVal = mpSlider->GetValue() / mpSlider->GetButtonValueAdd();

			if(mlMouseOverSelection < lVal)
				mpSlider->SetValue(int((mlMouseOverSelection) * mpSlider->GetButtonValueAdd()));
			
			if(mlMouseOverSelection > lVal + 11)
				mpSlider->SetValue(int((mlMouseOverSelection - 11) * mpSlider->GetButtonValueAdd()));
			return true;
		}

		return false;
	}

	bool cWidgetComboBox::OnUIButtonPress(const cGuiMessageData& aData)
	{
		if(HasFocus())
		{
			if(aData.mlVal&eUIButton_Secondary)
			{
				if(mbMenuOpen)
				{
					mbOpenByUIButton = false;
					CloseMenu();
					
					return true;
				}
				else
					return false;
			}
			else if((aData.mlVal&eUIButton_Primary)==0)
			{
				return false;
			}

			if(mbOpenByUIButton==false)
			{
				if(mbMenuOpen==false)
				{
					mbOpenByUIButton = true;
					OpenMenu();
				}
				else
				{
					SetSelectedItem(mlMouseOverSelection, true, true);
					CloseMenu();
				}
			}
			else
			{
				if(mbMenuOpen)
				{
					SetSelectedItem(mlMouseOverSelection, true, true);
					CloseMenu();
					mbOpenByUIButton = false;
				}
			}
		
			return aData.mlVal==eUIButton_Primary;
		}

		return false;
	}

	bool cWidgetComboBox::OnUIButtonRelease(const cGuiMessageData& aData)
	{
		if(HasFocus())
			return aData.mlVal==eUIButton_Primary;

		return false;
	}

	//-----------------------------------------------------------------------

	bool cWidgetComboBox::OnLostFocus(const cGuiMessageData& aData)
	{	
		if(mbMenuOpen && mbOpenByUIButton==false)
		{
			cVector3f vLocal = WorldToLocalPosition(aData.mvPos);
			if(vLocal.x < mvMenuPos.x || vLocal.x > mvMenuPos.x + mvSize.x
				|| vLocal.y < mvMenuPos.y || vLocal.y > mvMenuPos.y + mfMenuHeight)
				CloseMenu();
		}
	
		return false;
	}

	//-----------------------------------------------------------------------

	bool cWidgetComboBox::PointIsInside(const cVector2f& avPoint, bool abOnlyClipped)
	{
		if(mbMenuOpen==false && CheckPointInsideClippingParent(avPoint)==false)
			return false;

		if(abOnlyClipped && mbClipsGraphics==false) return true;

		cVector3f vMousePos = WorldToLocalPosition(cVector3f(avPoint));
		bool bInside = false;
		
		if(	vMousePos.x < 0 || vMousePos.x > mvSize.x || 
			vMousePos.y < 0 || vMousePos.y > mvSize.y)
		{
			bInside = false;
		}
		else
		{
			bInside = true;	
		}

		if(bInside==false)
		{
			if(mbMenuOpen)
			{
				if(vMousePos.x >= mvMenuPos.x && vMousePos.x <= mvMenuPos.x + mvSize.x &&
					vMousePos.y >= mvMenuPos.y && vMousePos.y <= mvMenuPos.y + mfMenuHeight)
					bInside = true;
			}
		}

		return bInside;
	}

	//-----------------------------------------------------------------------

	bool cWidgetComboBox::SliderOnUIArrowPress(iWidget* apWidget, const cGuiMessageData& aData)
	{
		if(aData.mlVal==eUIArrow_Up)
		{
			mlMouseOverSelection--;
			if(mlMouseOverSelection<0)
			{
				mlMouseOverSelection = GetItemNum()-1;

				mlFirstItem = mlMouseOverSelection-mlItemsShown+1;
			}

			if(mlMouseOverSelection<mlFirstItem)
				mlFirstItem = mlMouseOverSelection;
		}
		else if(aData.mlVal==eUIArrow_Down)
		{
			mlMouseOverSelection++;
			if(mlMouseOverSelection>GetItemNum()-1)
			{
				mlMouseOverSelection = 0;

				mlFirstItem = 0;
			}

			if(mlMouseOverSelection>mlFirstItem+mlItemsShown-1)
				mlFirstItem = mlMouseOverSelection-mlItemsShown+1;
		}
		else
			return false;

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetComboBox, SliderOnUIArrowPress);

	bool cWidgetComboBox::SliderOnUIButtonPress(iWidget* apWidget, const cGuiMessageData& aData)
	{
		if(aData.mlVal==eUIButton_Primary)
		{
			SetSelectedItem(mlMouseOverSelection);
			CloseMenu();

			mbOpenByUIButton = false;
			mpSet->SetFocusedWidget(this);
		}

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetComboBox, SliderOnUIButtonPress);

	bool cWidgetComboBox::SliderOnUIButtonRelease(iWidget* apWidget, const cGuiMessageData& aData)
	{
		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cWidgetComboBox, SliderOnUIButtonRelease);

	//-----------------------------------------------------------------------


}
