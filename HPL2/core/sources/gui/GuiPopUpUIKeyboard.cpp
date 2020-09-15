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

#include "gui/GuiPopUpUIKeyboard.h"

#include "gui/GuiTypes.h"

#include "system/LowLevelSystem.h"
#include "system/Platform.h"
#include "system/String.h"

#include "math/Math.h"

#include "graphics/FontData.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"

#include "gui/WidgetTextBox.h"
#include "gui/WidgetWindow.h"
#include "gui/WidgetButton.h"
#include "gui/WidgetFrame.h"
#include "gui/WidgetSlider.h"
#include "gui/WidgetLabel.h"

namespace hpl {

	/////////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	/////////////////////////////////////////////////////////////////////////////////

	static int glUnicodeUpperCaseOffset = _W('A')-_W('a');

	//-------------------------------------------------------------------------------

	cGuiPopUpUIKeyboard::cGuiPopUpUIKeyboard(cWidgetTextBox* apTarget, 
												void *apCallbackObject, tGuiCallbackFunc apCallback) : iGuiPopUp(apTarget->GetSet(), true, 0)
	{
		//mvPos = avPos;
		//mpCallbackObject = apCallbackObject;
		//mpCallback = apCallback;

		mpTargetTextBox = apTarget;
		msBackUpText = mpTargetTextBox->GetText();

		mpTargetTextBox->OnUIKeyboardOpen();

		mbShift = false;

		SetNextFocusWidget(static_cast<iWidget*>(apTarget->GetUserData()));
		
		Init();
	}

    //-------------------------------------------------------------------------------

	cGuiPopUpUIKeyboard::~cGuiPopUpUIKeyboard()
	{
		mpTargetTextBox->OnUIKeyboardClose();
	}

	//-------------------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	/////////////////////////////////////////////////////////////////////////////////


	//-------------------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	/////////////////////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------------------

	void cGuiPopUpUIKeyboard::SetUpKey(iWidget* apKeyWidget, eKey aKey, int alUnicode, bool abShift)
	{
		tGuiCallbackFunc pCallback;

		if(aKey==eKey_Return)
		{
			pCallback = kGuiCallback(Enter_OnPress);
		}
		else if(aKey==eKey_None)
		{
			pCallback = kGuiCallback(Cancel_OnPress);
		}
		else
		{
			pCallback = kGuiCallback(Key_OnPress);

			mlstKeys.push_back( cUIKey(aKey, alUnicode, abShift?alUnicode+glUnicodeUpperCaseOffset:alUnicode));
			apKeyWidget->SetUserData(&mlstKeys.back());
		}

		apKeyWidget->AddCallback(eGuiMessage_ButtonPressed, this, pCallback);

		mlstKeyWidgets.push_back(apKeyWidget);
	}

	//-------------------------------------------------------------------------------

	bool cGuiPopUpUIKeyboard::Key_OnPress(iWidget* apWidget, const cGuiMessageData &aData)
	{
		cUIKey* pKey = static_cast<cUIKey*>(apWidget->GetUserData());

		cKeyPress key(pKey->mKey, pKey->mvUnicode[mbShift], 0);
		
		return mpTargetTextBox->ProcessMessage(eGuiMessage_KeyPress, cGuiMessageData(key), true, true);
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpUIKeyboard, Key_OnPress);

	//-------------------------------------------------------------------------------

	bool cGuiPopUpUIKeyboard::Enter_OnPress(iWidget* apWidget, const cGuiMessageData &aData)
	{
		SelfDestruct();
		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpUIKeyboard, Enter_OnPress);

	//-------------------------------------------------------------------------------

	bool cGuiPopUpUIKeyboard::Cancel_OnPress(iWidget* apWidget, const cGuiMessageData &aData)
	{
		mpTargetTextBox->SetText(msBackUpText);
		
		SelfDestruct();

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpUIKeyboard, Cancel_OnPress);

	//-------------------------------------------------------------------------------

	bool cGuiPopUpUIKeyboard::Button_Pressed(iWidget* apWidget, const cGuiMessageData &aData)
	{
		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpUIKeyboard, Button_Pressed);

	//-------------------------------------------------------------------------------

	bool cGuiPopUpUIKeyboard::WindowClose(iWidget* apWidget, const cGuiMessageData& aData)
	{
		SelfDestruct();
		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpUIKeyboard, WindowClose);


	//-------------------------------------------------------------------------------

	bool cGuiPopUpUIKeyboard::Window_OnUIButtonPress(iWidget* apWidget, const cGuiMessageData& aData)
	{
		if(aData.mlVal==eUIButton_Secondary)
		{
			return Cancel_OnPress(apWidget, aData);
		}
		else if(aData.mlVal==eUIButton_Delete)
		{
			return mpTargetTextBox->ProcessMessage(eGuiMessage_KeyPress, cGuiMessageData(cKeyPress(eKey_BackSpace, 0, 0)));
		}
		else if(aData.mlVal==eUIButton_Clear)
		{
			mpTargetTextBox->SetText(_W(""));
			return true;
		}
		else if(aData.mlVal==eUIButton_PrevPage)
		{
			SetShiftActive(true);
			return true;
		}

		return false;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpUIKeyboard, Window_OnUIButtonPress);


	//-------------------------------------------------------------------------------

	bool cGuiPopUpUIKeyboard::Window_OnUIButtonRelease(iWidget* apWidget, const cGuiMessageData& aData)
	{
		if((aData.mlVal&eUIButton_PrevPage)!=0)
		{
			SetShiftActive(false);
			return true;
		}

		return false;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpUIKeyboard, Window_OnUIButtonRelease);


	//-------------------------------------------------------------------------------


	/** Builds the window and sets up stuff
	 *
	 */
	void cGuiPopUpUIKeyboard::Init()
	{
		/////////////////////////////////
		// Init widgets

		// Main window
		mpWindow->SetText(_W(""));
		mpWindow->SetStatic(true);
		mpWindow->SetDrawLabel(false);
		mpWindow->SetGlobalUIInputListener(true);
		mpWindow->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(Window_OnUIButtonPress));
		mpWindow->AddCallback(eGuiMessage_UIButtonRelease, this, kGuiCallback(Window_OnUIButtonRelease));

		float fKeySize = 32;
		cWidgetButton* pKey = NULL;
		
		// Adds numeric keys
		for(int i=eKey_0; i<=eKey_9; ++i)
		{
			wchar_t lUnicode = _W('0') + i-eKey_0;
			pKey = mpSet->CreateWidgetButton(0, fKeySize, tWString(1,lUnicode), mpWindow);
			SetUpKey(pKey, (eKey)i, lUnicode, false);
		}

		// Adds alphabetical keys
		for(int i=eKey_A; i<=eKey_Z; ++i)
		{
			wchar_t lUnicode = _W('a') + i-eKey_A;
			pKey = mpSet->CreateWidgetButton(0, fKeySize, tWString(1,lUnicode), mpWindow);
			SetUpKey(pKey, (eKey)i, lUnicode);
		}

		////////////////////////////////////////
		// Add enter, cancel and space keys
		pKey = mpSet->CreateWidgetButton(0, cVector2f(fKeySize*2+1, fKeySize), _W("Enter"), mpWindow);
		SetUpKey(pKey, eKey_Return, 0, false);

		pKey = mpSet->CreateWidgetButton(0, cVector2f(fKeySize*2+1, fKeySize), _W("Cancel"), mpWindow);
		SetUpKey(pKey, eKey_None, 0, false);

		pKey = mpSet->CreateWidgetButton(0, cVector2f((fKeySize+1)*10-1, fKeySize), _W("Space"), mpWindow);
		SetUpKey(pKey, eKey_Space, _W(' '), false);


		////////////////////////////////////////
		// Now arrange the keys in a nice grid
		std::vector<tWidgetVec> vKeyMatrix;
		
		vKeyMatrix.push_back(tWidgetVec());
		tWidgetVec* pWidgetRow = &vKeyMatrix.back();

		cVector3f vPos = cVector3f(0,0,2);
		
		tWidgetListIt it = mlstKeyWidgets.begin();
		for(;it!=mlstKeyWidgets.end(); ++it)
		{
			iWidget* pKey = *it;

			if(vPos.x+pKey->GetSize().x > (fKeySize+1)*10-1)
			{
				vKeyMatrix.push_back(tWidgetVec());
				pWidgetRow = &vKeyMatrix.back();
				
				vPos.x = 0;
				vPos.y += pKey->GetSize().y+1;
			}
			
			pKey->SetPosition(vPos);
			pWidgetRow->push_back(pKey);

			vPos.x += pKey->GetSize().x+1;
		}

		//////////////////////////////////////////////////////////
		// Position window right below the textbox
		cVector2f vWindowSize = cVector2f((fKeySize+1)*10, vPos.y+fKeySize+1);

		mpWindow->SetSize(vWindowSize);
		cVector3f vGlobalPos = mpTargetTextBox->GetGlobalPosition();

		// This 8 is a magic number since I cannot access the activesize for the window border without hacking a bit.
		vGlobalPos.y += mpTargetTextBox->GetSize().y+8; 
		vGlobalPos.x += mpTargetTextBox->GetSize().x*0.5f;
		vGlobalPos.x -= vWindowSize.x*0.5f;
		mpWindow->SetGlobalPosition(vGlobalPos);

		mpSet->PositionWidgetInsideBounds(mpWindow);

		

		/////////////////////////////////////////////////////////////
		// Set up focus navigation
		for(size_t i=0; i<vKeyMatrix.size(); ++i)
		{
			int lVPrev = i-1;
			int lVNext = i+1;

			const tWidgetVec vRow = vKeyMatrix[i];
			for(size_t j=0; j<vRow.size(); ++j)
			{
				iWidget* pWidget = vRow[j];

				int lHPrev = j-1;
				int lHNext = j+1;

				iWidget* pUpper = NULL;
				iWidget* pRight = NULL;
				iWidget* pLower = NULL;
				iWidget* pLeft = NULL;

				if(lHPrev>=0)
					pLeft = vRow[lHPrev];
				else
					pLeft = vRow[vRow.size() - 1];
				if(lHNext<(int)vRow.size())
					pRight = vRow[lHNext];
				else
					pRight = vRow[0];

				if(lVPrev>=0)
				{
					const tWidgetVec& vPrevRow = vKeyMatrix[lVPrev];
					if(j<(int)vPrevRow.size())
						pUpper = vPrevRow[j];
					else
						pUpper = vPrevRow.back();
				}
				else
				{
					const tWidgetVec& vPrevRow = vKeyMatrix[vKeyMatrix.size() - 1];
					if(j<(int)vPrevRow.size())
						pUpper = vPrevRow[j];
					else
						pUpper = vPrevRow.back();
				}

				if(lVNext<(int)vKeyMatrix.size())
				{
					const tWidgetVec& vNextRow = vKeyMatrix[lVNext];
					if(j<(int)vNextRow.size()) 
						pLower = vNextRow[j];
					else
						pLower = vNextRow.back();
				}
				else
				{
					const tWidgetVec& vNextRow = vKeyMatrix[0];
					if(j<(int)vNextRow.size()) 
						pLower = vNextRow[j];
					else
						pLower = vNextRow.back();
				}

				pWidget->SetFocusNavigation(eUIArrow_Up, pUpper);
				pWidget->SetFocusNavigation(eUIArrow_Right, pRight);
				pWidget->SetFocusNavigation(eUIArrow_Down, pLower);
				pWidget->SetFocusNavigation(eUIArrow_Left, pLeft);
			}
		}
		mpSet->SetDefaultFocusNavWidget(vKeyMatrix[0][0]);
		mpSet->SetFocusedWidget(vKeyMatrix[0][0]);
	}

	//-------------------------------------------------------------------------------
	
	void cGuiPopUpUIKeyboard::ClosePopUp()
	{
		SelfDestruct();
	}

	//-------------------------------------------------------------------------------

	void cGuiPopUpUIKeyboard::SetShiftActive(bool abX)
	{
		if(mbShift==abX) return;

		mbShift = abX;
		tWidgetListIt it = mlstKeyWidgets.begin();
		for(;it!=mlstKeyWidgets.end(); ++it)
		{
			iWidget* pKeyWidget = *it;
			cUIKey* pKey = static_cast<cUIKey*>(pKeyWidget->GetUserData());

			if(pKey) pKeyWidget->SetText(tWString(1, pKey->mvUnicode[mbShift]));
		}
	}

	//-------------------------------------------------------------------------------

};




