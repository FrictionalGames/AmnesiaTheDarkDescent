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

#include "gui/WidgetListBoxBase.h"

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

	iWidgetListBoxBase::iWidgetListBoxBase(cGuiSet *apSet, cGuiSkin *apSkin) : iWidget(eWidgetType_ListBox,apSet, apSkin)
	{
		mbClipsGraphics = true;

		mfBackgroundZ = -0.5;

		mlFirstItem = 0;
		mlMaxItems = 1;

		mlCursorPos = 0;
		mlLastCursorPos = 0;
		mlAnchorPos = -1;
		mlNumClicksOnSamePos = 0;

		mlNumBlankRows = 0;

		mfSliderWidth = mpSkin->GetAttribute(eGuiSkinAttribute_ListBoxSliderWidth).x;

		mpSlider = NULL;

		mbAllowMultiSelection = false;
		mbClearingSelection = false;

		mbLocked = false;
	}

	//-----------------------------------------------------------------------

	iWidgetListBoxBase::~iWidgetListBoxBase()
	{
		if(mpSet->IsDestroyingSet()==false)
		{
			mpSet->DestroyWidget(mpSlider);			
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	void iWidgetListBoxBase::SetSelectedItem(int alX, bool abMoveList, bool abGenerateCallback, eListBoxSelectType aType, 
												bool abClearPrevious)
	{
		if(abClearPrevious || mbAllowMultiSelection==false)
			ClearSelection();

		if(abMoveList)
			SetCursorPos(alX, true, true);

		cWidgetItem* pItem = GetItem(alX);
		if(mbAllowMultiSelection)
		{
			switch(aType)
			{

			case eListBoxSelectType_Select:
				{
					if(pItem)
						pItem->SetSelected(true);
				}
				break;
			case eListBoxSelectType_Toggle:
				{
					if(pItem)
						pItem->SetSelected(!pItem->IsSelected());
				}
				break;
			case eListBoxSelectType_FromLastCursor:
				SelectRange(mlLastCursorPos, alX);
				break;
			case eListBoxSelectType_FromAnchorPos:
				SelectRange(mlAnchorPos, alX);
				break;
			}
		}
		else
		{
			if(pItem)
				pItem->SetSelected(true);
		}

		if(abGenerateCallback)
			ProcessMessage(eGuiMessage_SelectionChange, cGuiMessageData(alX));
	}
	
	//-----------------------------------------------------------------------

	int iWidgetListBoxBase::GetSelectedItem()
	{
		if(mlstSelectedItems.empty())
			return -1;
		
		return mlstSelectedItems.back();
	}

	//-----------------------------------------------------------------------

	int iWidgetListBoxBase::GetMultiSelection(int alIdx)
	{
		int i=0;
		for(tIntListIt it=mlstSelectedItems.begin(); it!=mlstSelectedItems.end() || i>alIdx; ++it, ++i)
		{
			if(i==alIdx)
				return *it;
		}
		return -1;
	}

	//-----------------------------------------------------------------------

	int iWidgetListBoxBase::GetMultiSelectionNum()
	{
		return (int)mlstSelectedItems.size();
	}
	//-----------------------------------------------------------------------

	void iWidgetListBoxBase::ClearSelection()
	{
		mbClearingSelection = true;

		tIntListIt it = mlstSelectedItems.begin();
		for(;it!=mlstSelectedItems.end();++it)
		{
			mvItems[*it]->SetSelected(false);
		}
		mlstSelectedItems.clear();

		mbClearingSelection = false;
	}

	//-----------------------------------------------------------------------

	cVector3f iWidgetListBoxBase::GetItemStartPos()
	{
		return GetGlobalPosition() + cVector3f(0,2,0.01f);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void iWidgetListBoxBase::SelectRange(int alStart, int alEnd)
	{
		int add;

		if(alStart<alEnd)
			add = 1;
		else
			add = -1;

		cWidgetItem* pItem = NULL;
		for(int i=alStart;i!=alEnd;i+=add)
		{
			pItem = GetItem(i);
			if(pItem)
				pItem->SetSelected(true);
		}
		pItem = GetItem(alEnd);
		if(pItem)
			pItem->SetSelected(true);
	}

	//-----------------------------------------------------------------------
	
	void iWidgetListBoxBase::UpdateProperties()
	{
		mlNumItems = (int)mvItems.size();
		mlMaxItems = (int)(mvSize.y /(mvDefaultFontSize.y +2))-1;

        if(mpSlider == NULL)
			return;
		
		if((int)mvItems.size() > mlMaxItems)
		{
			mpSlider->SetBarValueSize(mlMaxItems);
			mpSlider->SetMaxValue((int)mvItems.size() - mlMaxItems);
		}
		else
		{
			mpSlider->SetMaxValue(0);
			mpSlider->SetBarValueSize(1);
		}

		mpSlider->SetButtonValueAdd(1);
		mpSlider->SetBarClickValueAdd(mlMaxItems-1);

		tIntListIt it = mlstSelectedItems.begin();
		for(;it!=mlstSelectedItems.end();)
		{
			int lIdx = *it;
			if(lIdx>=GetItemNum())
				it = mlstSelectedItems.erase(it);
			else
				++it;
		}

		mpSlider->SetValue(GetSelectedItem(), true);

		if(mlNumItems==0)
			SetCursorPos(0,true, true);
	}

	//-----------------------------------------------------------------------

	void iWidgetListBoxBase::OnChangeSize()
	{
		if(mpSlider)
		{
			mpSlider->SetSize(cVector2f(mfSliderWidth,mvSize.y));
			mpSlider->SetPosition(cVector3f(mvSize.x - mfSliderWidth, 0, 0.2f));
		}

		UpdateProperties();
	}

	//-----------------------------------------------------------------------

	void iWidgetListBoxBase::OnInit()
	{
		mpSlider = mpSet->CreateWidgetSlider(eWidgetSliderOrientation_Vertical,0,0,0,this);
		mpSlider->AddCallback(eGuiMessage_SliderMove,this,kGuiCallback(MoveSlider));

		OnChangeSize();
	}

	//-----------------------------------------------------------------------

	void iWidgetListBoxBase::AddItemToSelection(cWidgetItem* apItem)
	{
		mlstSelectedItems.push_back(apItem->GetIndex());
	}

	void iWidgetListBoxBase::RemoveItemFromSelection(cWidgetItem* apItem)
	{
		if(mbClearingSelection==false)
			mlstSelectedItems.remove(apItem->GetIndex());
	}

	//-----------------------------------------------------------------------

	bool iWidgetListBoxBase::MoveSlider(iWidget* apWidget, const cGuiMessageData& aData)
	{
		mlFirstItem = aData.mlVal;

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(iWidgetListBoxBase,MoveSlider)
	
	//-----------------------------------------------------------------------

	void iWidgetListBoxBase::OnLoadGraphics()
	{
		mpGfxBackground = mpSkin->GetGfx(eGuiSkinGfx_ListBoxBackground);

		mpGfxSelection = mpSkin->GetGfx(eGuiSkinGfx_TextBoxSelectedTextBack);

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

	void iWidgetListBoxBase::OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion)
	{
		////////////////////////////////
		// Background
		mpSet->DrawGfx(	mpGfxBackground,GetGlobalPosition() +cVector3f(0,0,mfBackgroundZ),
						mvSize);
		
		
		////////////////////////////////
		// Borders
		DrawBordersAndCorners(	NULL, mvGfxBorders, mvGfxCorners, 
								GetGlobalPosition() - 
									cVector3f(	mvGfxCorners[0]->GetActiveSize().x,
												mvGfxCorners[0]->GetActiveSize().y,0), 
									mvSize +	mvGfxCorners[0]->GetActiveSize() + 
												mvGfxCorners[2]->GetActiveSize());
	}

	//-----------------------------------------------------------------------

	void iWidgetListBoxBase::OnDrawAfterClip(float afTimeStep, cGuiClipRegion* apClipRegion)
	{
		cGuiClipRegion* pRegion = apClipRegion->CreateChild(GetGlobalPosition(), mvSize-cVector2f(mfSliderWidth,0));
		mpSet->SetCurrentClipRegion(pRegion);

		DrawItems(afTimeStep, pRegion);
		// Draw cursor
		if(HasFocus())
		{
			int lIndex = mlCursorPos-mlFirstItem;
			if(lIndex!=-1)
			{
				cVector3f vPosition = GetItemStartPos();
				
				cVector3f vUpperLeftCorner = vPosition + cVector3f(-1,lIndex*(mvDefaultFontSize.y+2),0);
				cVector2f vWidth = cVector2f(mvSize.x-mfSliderWidth,1);
				cVector2f vHeight = cVector2f(1, mvDefaultFontSize.y);

				mpSet->DrawGfx(cGui::mpGfxRect, vUpperLeftCorner, vWidth, cColor(0.3f, 0.4));
				mpSet->DrawGfx(cGui::mpGfxRect, vUpperLeftCorner+cVector3f(0,vHeight.y,0), vWidth, cColor(0.3f, 0.4));
				mpSet->DrawGfx(cGui::mpGfxRect, vUpperLeftCorner, vHeight, cColor(0.3f, 0.4));
				mpSet->DrawGfx(cGui::mpGfxRect, vUpperLeftCorner+cVector3f(vHeight.x,0,0), vHeight, cColor(0.3f, 0.4));
			}
		}

		mpSet->SetCurrentClipRegion(apClipRegion);
	}

	//-----------------------------------------------------------------------
	
	bool iWidgetListBoxBase::OnMouseMove(const cGuiMessageData& aData)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool iWidgetListBoxBase::OnMouseDown(const cGuiMessageData& aData)
	{
		int lSliderValue = mpSlider->GetValue();
		if(aData.mlVal == eGuiMouseButton_WheelUp)
		{
			mpSlider->SetValue(lSliderValue-5);
			return true;
		}

		if(aData.mlVal == eGuiMouseButton_WheelDown)
		{
			mpSlider->SetValue(lSliderValue+5);
			return true;
		}

		cVector3f vLocalPos = WorldToLocalPosition(aData.mvPos);

		int lSelection = (int)((vLocalPos.y - 2) / (mvDefaultFontSize.y+2)) - mlNumBlankRows;
		if(lSelection < 0) return false;

		lSelection = lSelection + mlFirstItem;

		bool bUsingCtrl = (aData.mKeyPress.mlModifier & eKeyModifier_Ctrl)!=0;
		bool bUsingShift = (aData.mKeyPress.mlModifier & eKeyModifier_Shift)!=0;
		bool bClearSelection = !bUsingCtrl;

		eListBoxSelectType type = bUsingShift? eListBoxSelectType_FromAnchorPos :
												(bUsingCtrl? eListBoxSelectType_Toggle : eListBoxSelectType_Select);
		
		SetCursorPos(lSelection,true,false);

		if(bUsingShift==false)
			SetAnchorPos(mlCursorPos);

		if(lSelection==mlLastCursorPos)
			++mlNumClicksOnSamePos;
		else
			mlNumClicksOnSamePos = 1;

		cWidgetItem* pItem = GetItem(mlCursorPos);
		if(GetMultiSelectionNum()==1 && pItem && pItem->IsSelected())
			;
		else
			SetSelectedItem(mlCursorPos, false, true, type, bClearSelection);

		return true;
	}

	//-----------------------------------------------------------------------

	bool iWidgetListBoxBase::OnMouseUp(const cGuiMessageData& aData)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool iWidgetListBoxBase::OnMouseDoubleClick(const cGuiMessageData& aData)
	{
		cVector3f vLocalPos = WorldToLocalPosition(aData.mvPos);

		// If vLocalPos is on the slider, ignore event
		if(vLocalPos.x > mvSize.x - mpSlider->GetSize().x)
			return false;

		if(mlCursorPos!=-1 && mlNumClicksOnSamePos>=2)
		{
			mlNumClicksOnSamePos = 0;
			ProcessMessage(eGuiMessage_SelectionDoubleClick, aData);
		}

		return true;
	}


	//-----------------------------------------------------------------------

	bool iWidgetListBoxBase::OnMouseEnter(const cGuiMessageData& aData)
	{
		return false;
	}
	
	//-----------------------------------------------------------------------
	
	bool iWidgetListBoxBase::OnMouseLeave(const cGuiMessageData& aData)
	{
		return false;
	}

	//-----------------------------------------------------------------------

	bool iWidgetListBoxBase::OnKeyPress(const cGuiMessageData& aData)
	{
		eKey key = aData.mKeyPress.mKey;
		int keyMod = aData.mKeyPress.mlModifier;
		eListBoxSelectType type = eListBoxSelectType_Select;
		bool bUsingCtrl = ((keyMod&eKeyModifier_Ctrl)!=0);
		bool bUsingShift = ((keyMod&eKeyModifier_Shift)!=0);

		if(key == eKey_Up)
		{
			SetCursorPos(mlCursorPos-1, true, true);
			if(bUsingCtrl==false && bUsingShift==false)
				SetAnchorPos(mlCursorPos);
			
			if(bUsingCtrl==false && bUsingShift==false)
			{
				SetSelectedItem(mlCursorPos, false, true, eListBoxSelectType_Select, true);
			}
			else if(bUsingShift)
			{
				SetSelectedItem(mlCursorPos, false, true, eListBoxSelectType_FromAnchorPos, bUsingCtrl==false);
			}

			return true;
		}
		else if(key == eKey_Down)
		{
			SetCursorPos(mlCursorPos+1, true, true);
			if(bUsingCtrl==false && bUsingShift==false)
				SetAnchorPos(mlCursorPos);
			
			if(bUsingCtrl==false && bUsingShift==false)
			{
				SetSelectedItem(mlCursorPos, false, true, eListBoxSelectType_Select, true);
			}
			else if(bUsingShift)
			{
				SetSelectedItem(mlCursorPos, false, true, eListBoxSelectType_FromAnchorPos, bUsingCtrl==false);
			}

			return true;
		}
		else if(key == eKey_Space)
		{
			eListBoxSelectType type;
			bool bClearSelection;

			if(bUsingCtrl)
			{
				type = eListBoxSelectType_Toggle;
				bClearSelection = false;
			}
			else
			{
				type = eListBoxSelectType_Select;
				bClearSelection = true;
			}

			SetSelectedItem(mlCursorPos, false, true, type, bClearSelection);

			return true;
		}
		
		return false;
	}

	//-----------------------------------------------------------------------

	bool iWidgetListBoxBase::OnUIArrowPress(const cGuiMessageData& aData)
	{
		if(mbLocked==false)
			return false;

		bool bUnlock = false;

		if(aData.mlVal==eUIArrow_Up)
		{
			SetCursorPos(mlCursorPos-1, true, true);
		}
		else if(aData.mlVal==eUIArrow_Down)
		{
			int lOldPos = mlCursorPos;

			SetCursorPos(mlCursorPos+1, true, true);

			bUnlock = lOldPos == mlCursorPos;
		}
		else
			return false;

		SetSelectedItem(mlCursorPos, true, true, eListBoxSelectType_Select);

		if(bUnlock)
		{
			ProcessMessage(eGuiMessage_CheckChange, aData);
			mbLocked = false;
		}

		return true;
	}

	bool iWidgetListBoxBase::OnUIArrowRelease(const cGuiMessageData& aData)
	{
		return false;
	}

	//-----------------------------------------------------------------------

	bool iWidgetListBoxBase::OnUIButtonPress(const cGuiMessageData& aData)
	{
		if(HasFocus())
		{
			if(aData.mlVal==eUIButton_Primary)
			{
				if(GetSelectedItem() == -1)
				{
					SetCursorPos(0, true,true);
					SetSelectedItem(mlCursorPos, true, true, eListBoxSelectType_Select);
				}
				if(mbLocked == true)
				{
				    if(ProcessMessage(eGuiMessage_CheckChange, aData))
					{
						mbLocked = false;
						return true;
					}
				}

				mbLocked=true;
			}
			if(aData.mlVal==eUIButton_Secondary)
			{
				if(mbLocked == false) return false;
				mbLocked=false;
			}
		
			return true;
		}

		return false;
	}

	bool iWidgetListBoxBase::OnUIButtonRelease(const cGuiMessageData& aData)
	{
		return HasFocus();
	}

	bool iWidgetListBoxBase::OnUIButtonDoublePress(const cGuiMessageData& aData)
	{
		if(HasFocus())
		{
			if(aData.mlVal==eUIButton_Primary)
			{
				mbLocked = false;
				return ProcessMessage(eGuiMessage_SelectionDoubleClick, aData);
			}

			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------

	void iWidgetListBoxBase::SetCursorPos(int alPos, bool abMoveList, bool abClamp)
	{
		if(abClamp)
		{
			if(alPos<0)
				alPos = 0;
			if(alPos>=mlNumItems)
				alPos = mlNumItems-1;
		}
		else
		{
			if(alPos<0 || alPos>=mlNumItems)
				alPos = -1;
		}

		mlLastCursorPos = mlCursorPos;
		mlCursorPos = alPos;

		if(abMoveList && mlCursorPos >= mlFirstItem + mlMaxItems)
		{
			while(mlCursorPos >= mlFirstItem + mlMaxItems)
			{
				mlFirstItem++;
			}
			mpSlider->SetValue(mlFirstItem);
		}
		if(abMoveList && mlCursorPos < mlFirstItem && mlCursorPos>=0)
		{
			while(mlCursorPos < mlFirstItem)
			{
				mlFirstItem--;
			}
			mpSlider->SetValue(mlCursorPos);
		}
	}

	//-----------------------------------------------------------------------


}
