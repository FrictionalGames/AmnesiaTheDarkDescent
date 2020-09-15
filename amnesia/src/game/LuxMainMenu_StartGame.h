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

#ifndef LUX_MAIN_MENU_START_GAME_H
#define LUX_MAIN_MENU_START_GAME_H

//----------------------------------------------

#include "LuxMainMenu.h"

//----------------------------------------------

class cLuxMainMenu_StartGame : public iLuxMainMenuWindow
{
public:
	cLuxMainMenu_StartGame(cGuiSet *apGuiSet, cGuiSkin *apGuiSkin);
	~cLuxMainMenu_StartGame();

	virtual void CreateGui();

	virtual void ExitPressed();

private:
	virtual void OnSetActive(bool abX);

	////////////////////////
	// Properties
	cVector2f mvWindowSize;

	////////////////////////
	// Layout
	cWidgetButton* mpNormalModeButton;
	cWidgetButton* mpHardModeButton;

	iWidget *mpStartButton;

	//Description
	cWidgetLabel* mpLDescription;

	////////////////////////
	// Callbacks
	bool WindowOnUpdate(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(WindowOnUpdate);

	bool PressStartGame(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressStartGame);

	bool PressCancel(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressCancel);

	bool UIPressStart(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(UIPressStart);

	bool UIPressCancel(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(UIPressCancel);

	bool ExitCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ExitCallback);

	//NORMAL
	bool PressNormalMode(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressNormalMode);

	bool ClickedStartGamePopup(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ClickedStartGamePopup);

	bool UIPressNormalMode(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(UIPressNormalMode);

	//HARDMODE
	bool PressHardMode(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressHardMode);

	bool UIPressHardmode(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(UIPressHardmode);


};

//----------------------------------------------

#endif // LUX_MAIN_MENU_START_GAME_H