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

#include "LuxMainMenu_StartGame.h"
#include "LuxBase.h"
#include "LuxInputHandler.h"
#include "LuxDebugHandler.h"

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxMainMenu_StartGame::cLuxMainMenu_StartGame(cGuiSet *apGuiSet, cGuiSkin *apGuiSkin) : iLuxMainMenuWindow(apGuiSet, apGuiSkin)
{
	mvWindowSize = cVector2f(400, 220);
#if MAC_OS || LINUX
	mpStartButton = 0;
#else
    mpStartButton = nullptr;
#endif
}

//-----------------------------------------------------------------------

cLuxMainMenu_StartGame::~cLuxMainMenu_StartGame()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMainMenu_StartGame::CreateGui()
{
	//////////////////////////
	//Window
	mpWindow = mpGuiSet->CreateWidgetWindow(eWidgetWindowButtonFlag_None,cVector3f(0,0,5),mvWindowSize, kTranslate("MainMenu", "Start Game"));
	mpWindow->AddCallback(eGuiMessage_OnUpdate, this, kGuiCallback(WindowOnUpdate));

	float fBorderSize = 15;
	cVector3f vPos = cVector3f(fBorderSize, 60+fBorderSize, 0.1f);

	//////////////////////////
	//Buttons
	float fButtonWidth = 120;
	float fButtonSepp = 3;

	std::vector<iWidget*> vButtons;

	vPos.x = (mvWindowSize.x ) - ((fButtonWidth * 2.0f) + fButtonSepp * 4);
	vPos.y = mpWindow->GetSize().y - 35;

	// Start Game
	cWidgetButton* pButton = mpGuiSet->CreateWidgetButton(vPos, cVector2f(fButtonWidth * 2.0 + fButtonSepp, 30), kTranslate("MainMenu", "Start Game"), mpWindow);
	pButton->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(PressStartGame));
	pButton->AddCallback(eGuiMessage_UIButtonPress,this, kGuiCallback(UIPressStart));
	mpStartButton = pButton;
	vButtons.push_back(mpStartButton);

	vPos.x = (vPos.x / 2.0f) - (fButtonWidth / 2.0f);

	// Cancel
	pButton = mpGuiSet->CreateWidgetButton(vPos, cVector2f(fButtonWidth, 30), kTranslate("Global", "Cancel"), mpWindow);
	pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(PressCancel));
	pButton->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(UIPressCancel));
	vButtons.push_back(pButton);

	// Normal
	float fButtonHeight = 30.0f;

	vPos.x = (mvWindowSize.x) - ((fButtonWidth * 2.0f) + fButtonSepp * 4);

	cVector3f vButtonPosition = cVector3f(
		(vPos.x / 2.0f) - (fButtonWidth / 2.0f), 
		(fBorderSize + (mvWindowSize.y / 4.0)) - (fButtonHeight / 2.0), 
		0.1f);
	vButtonPosition.z = 0.1f;

	// Normal mode
	mpNormalModeButton = mpGuiSet->CreateWidgetButton(vButtonPosition, cVector2f(fButtonWidth, 30), kTranslate("MainMenu","NormalMode"), mpWindow);// Translate This
	mpNormalModeButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(PressNormalMode));
	mpNormalModeButton->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(UIPressNormalMode));
	mpNormalModeButton->SetDefaultFontColor(cColor(232.0f / 255.0f, 201.0f / 255.0f, 28.0f / 255.0f, 1.0f));
	vButtons.push_back(mpNormalModeButton);

	vButtonPosition.y += 35 + fButtonSepp;

	// Hard mode
	mpHardModeButton = mpGuiSet->CreateWidgetButton(vButtonPosition, cVector2f(fButtonWidth, 30), kTranslate("MainMenu", "HardMode"), mpWindow);// Translate This
	mpHardModeButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(PressHardMode));
	mpHardModeButton->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(UIPressHardmode));
	mpHardModeButton->SetDefaultFontColor(cColor(1.f, 1.0f));

	vButtons.push_back(mpHardModeButton);

	vButtonPosition.y += 50 + fButtonSepp;

	////////////////////////////////////////
	// Set up focus navigation
	
	// 0 = Start Game
	// 1 = Cancel
	// 2 = Normal
	// 3 = Hardmode 

	vButtons[0]->SetFocusNavigation(eUIArrow_Up, vButtons[3]);
	vButtons[0]->SetFocusNavigation(eUIArrow_Left, vButtons[1]);

	vButtons[1]->SetFocusNavigation(eUIArrow_Up, vButtons[3]);
	vButtons[1]->SetFocusNavigation(eUIArrow_Right, vButtons[0]);

	vButtons[2]->SetFocusNavigation(eUIArrow_Down, vButtons[3]);
	
	vButtons[3]->SetFocusNavigation(eUIArrow_Up, vButtons[2]);
	vButtons[3]->SetFocusNavigation(eUIArrow_Down, vButtons[0]);


	vPos.x = 0;
	vPos.y = mpWindow->GetSize().y - 90 - 10 - 30;


	////////////////////////////////////////
	// Description

	cVector3f vDescriptionPos = cVector3f(
		mvWindowSize.x - ((fButtonWidth * 2.0f) + fButtonSepp * 4),
		30 + fBorderSize, 
		0.1f
	);

	cVector2f vDescriptionSize = cVector2f(mvWindowSize.x - (vDescriptionPos.x + fButtonSepp)  , mvWindowSize.y - (fBorderSize * 2) - 60);
	vDescriptionSize.x -= fButtonSepp * 3.0f;

	mpLDescription = mpGuiSet->CreateWidgetLabel(vDescriptionPos, vDescriptionSize, kTranslate("MainMenu", "NormalModeDescription"), mpWindow);
	mpLDescription->SetDefaultFontColor(cColor(1, 1));
	mpLDescription->SetWordWrap(true);
	mpLDescription->SetTextAlign(eFontAlign_Center);
	mpLDescription->SetClipActive(true);
	mpLDescription->SetScrollSpeedMul(4.0f);
	mpLDescription->SetDrawBackGround(true);
	mpLDescription->SetBackGroundColor(cColor(0, 0.5f));
}

//-----------------------------------------------------------------------

void cLuxMainMenu_StartGame::ExitPressed()
{
	if (mpGuiSet->PopUpIsActive()) return;

	gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_LastEnum);
}

//-----------------------------------------------------------------------

void cLuxMainMenu_StartGame::OnSetActive(bool abX)
{
	if (abX)
	{
		mpGuiSet->SetDefaultFocusNavWidget(mpStartButton);
		mpGuiSet->SetFocusedWidget(mpStartButton);
	}
}

//-----------------------------------------------------------------------

bool cLuxMainMenu_StartGame::WindowOnUpdate(iWidget* apWidget, const cGuiMessageData& aData)
{
	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_StartGame, WindowOnUpdate);

//-----------------------------------------------------------------------

bool cLuxMainMenu_StartGame::PressNormalMode(iWidget* apWidget, const cGuiMessageData& aData)
{
	mpNormalModeButton->SetDefaultFontColor(cColor(232.0f / 255.0f, 201.0f / 255.0f, 28.0f / 255.0f, 1.0f));
	mpHardModeButton->SetDefaultFontColor(cColor(1.f, 1.0f));
	mpLDescription->SetText(kTranslate("MainMenu", "NormalModeDescription"));

	gpBase->mbHardMode = false;

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_StartGame, PressNormalMode);

//-----------------------------------------------------------------------

bool cLuxMainMenu_StartGame::ClickedStartGamePopup(iWidget* apWidget, const cGuiMessageData& aData)
{
	bool bStartGame = aData.mlVal == 0 ? true : false;
	mpGuiSet->SetDrawFocus(false);

	if (bStartGame)
	{
		gpBase->SetCustomStory(NULL);
		gpBase->mpMainMenu->ExitMenu(eLuxMainMenuExit_StartGame);
		gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_LastEnum);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_StartGame, ClickedStartGamePopup);

//-----------------------------------------------------------------------

bool cLuxMainMenu_StartGame::PressStartGame(iWidget* apWidget, const cGuiMessageData& aData)
{
	mpGuiSet->SetDrawFocus(gpBase->mpInputHandler->IsGamepadPresent());

	tWString sDesciption = gpBase->mbHardMode == false ? kTranslate("MainMenu", "Start a new game?") : kTranslate("MainMenu", "HardModeStartNewGame");

	cGuiPopUpMessageBox *pPopUp = mpGuiSet->CreatePopUpMessageBox(_W(""), sDesciption,
		kTranslate("MainMenu", "Yes"), kTranslate("MainMenu", "No"),
		this,
		kGuiCallback(ClickedStartGamePopup));
	
	pPopUp->GetGuiSet()->SetDrawFocus(mpGuiSet->GetDrawFocus());
	pPopUp->SetKillOnEscapeKey(false);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_StartGame, PressStartGame);


//-----------------------------------------------------------------------

bool cLuxMainMenu_StartGame::PressCancel(iWidget* apWidget, const cGuiMessageData& aData)
{
	ExitCallback(NULL, cGuiMessageData(0));
	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_StartGame, PressCancel);

//-----------------------------------------------------------------------

bool cLuxMainMenu_StartGame::ExitCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	bool bOkPressed = aData.mlVal == 0 ? true : false;
	if (bOkPressed == false)
		return true;

	if (gpBase->mpCustomStory == NULL)
		gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_LastEnum);
	else
		gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_CustomStory);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_StartGame, ExitCallback);

//-----------------------------------------------------------------------

bool cLuxMainMenu_StartGame::UIPressStart(iWidget* apWidget, const cGuiMessageData& aData)
{
	switch(aData.mlVal)
	{
	case eUIButton_Primary:	return PressStartGame(apWidget, aData);
	case eUIButton_Secondary: return PressCancel(apWidget, aData);
	}

	return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_StartGame, UIPressStart);


bool cLuxMainMenu_StartGame::UIPressNormalMode(iWidget* apWidget, const cGuiMessageData& aData)
{
	switch (aData.mlVal)
	{
	case eUIButton_Primary:	return PressNormalMode(apWidget, aData);
	case eUIButton_Secondary: return PressCancel(apWidget, aData);
	}

	return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_StartGame, UIPressNormalMode);



bool cLuxMainMenu_StartGame::UIPressHardmode(iWidget* apWidget, const cGuiMessageData& aData)
{
	switch (aData.mlVal)
	{
	case eUIButton_Primary:	return PressHardMode(apWidget, aData);
	case eUIButton_Secondary: return PressCancel(apWidget, aData);
	}

	return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_StartGame, UIPressHardmode);

bool cLuxMainMenu_StartGame::UIPressCancel(iWidget* apWidget, const cGuiMessageData& aData)
{
	switch(aData.mlVal)
	{
	case eUIButton_Primary: return PressCancel(apWidget, aData);
	case eUIButton_Secondary: return PressCancel(apWidget, aData);
	}

	return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_StartGame, UIPressCancel);

//-----------------------------------------------------------------------

bool cLuxMainMenu_StartGame::PressHardMode(iWidget* apWidget, const cGuiMessageData& aData)
{
	mpNormalModeButton->SetDefaultFontColor(cColor(1.f, 1.0f));
	mpHardModeButton->SetDefaultFontColor(cColor(232.0f / 255.0f, 201.0f / 255.0f, 28.0f / 255.0f, 1.0f));
	mpLDescription->SetText(kTranslate("MainMenu", "HardModeDescription"));
	gpBase->mbHardMode = true;

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_StartGame, PressHardMode);

