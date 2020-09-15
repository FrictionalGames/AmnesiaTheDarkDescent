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

#ifndef LUX_MAIN_MENU_LOAD_GAME_H
#define LUX_MAIN_MENU_LOAD_GAME_H

//----------------------------------------------

#include "LuxMainMenu.h"

//----------------------------------------------

class cLuxMainMenu_LoadGame : public iLuxMainMenuWindow
{
	friend class cLuxInputMenuEntry;
public:
	cLuxMainMenu_LoadGame(cGuiSet *apGuiSet, cGuiSkin *apGuiSkin);
	~cLuxMainMenu_LoadGame();

	void CreateGui();

	void ExitPressed();

private:
	void OnSetActive(bool abX);

	void PopulateSavedGameList();

	////////////////////////
	// Properties
	cVector2f mvWindowSize;

	tWStringVec mvSavedGameFileNames;

	////////////////////////
	// Layout
	cWidgetListBox* mpLBSavedGames;
	cWidgetImage* mpISavedGameSnapShot;
	iWidget *mpLoadButton;

	////////////////////////
	// Callbacks
	void LoadGame(int alIdx);

	bool WindowOnUpdate(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(WindowOnUpdate);

	bool PressOK(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressOK);

	bool PressCancel(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressCancel);

	bool UIPressList(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(UIPressList);

	bool UIPress(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(UIPress);

	bool UIPressCancel(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(UIPressCancel);

	bool LoadGameCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(LoadGameCallback);

	bool ExitCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ExitCallback);

	bool LoadSelectionClickChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(LoadSelectionClickChange);

	bool LockLoadList(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(LockLoadList);
};

//----------------------------------------------

#endif // LUX_MAIN_MENU_LOAD_GAME_H
