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

#ifndef LUX_MAIN_MENU_PROFILE_H
#define LUX_MAIN_MENU_PROFILE_H

//----------------------------------------------

#include "LuxMainMenu.h"

//----------------------------------------------

class cLuxGameProfile
{
public:
	tWString msName;
	cDate mCreationDate;
};

//----------------------------------------------

class cLuxMainMenu_Profile : public iLuxMainMenuWindow
{
public:	
	cLuxMainMenu_Profile(cGuiSet *apGuiSet, cGuiSkin *apGuiSkin);
	~cLuxMainMenu_Profile();
	
	void CreateGui();

	void ExitPressed();
	
private:
	void OnSetActive(bool abX);

	void CreateMainGui();
	void CreateEnterNameGui();

	void AddProfilesInListBox();

	void SelectProfile(int alNum);
	
	////////////////////////
	// Properties
	tWString msDefaultProfileName;

	cVector2f mvWindowSize;
	float mfInformationWidth;
	cVector2f mvListFontSize;

	cVector2f mvEnterNameWindowSize;
	float mfEnterNameButtonLength;


	///////////////////////
	// Data and varaibles
	int mlLastPickedProfile;
	cWidgetListBox *mpListProfiles;

	cWidgetWindow *mpWindowEnterName;
	cWidgetTextBox *mpTextEnterName;

	cWidgetButton* mpSelectButton;
	cWidgetButton* mpCreateButton;

	std::vector<cLuxGameProfile*> mvProfiles;

	///////////////////////
	// Widget callbacks
	bool WindowCloses(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(WindowCloses);

	bool UIPressList(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(UIPressList);

	bool WindowUIPress(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(WindowUIPress); 

	bool ClickedExitPopup(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ClickedExitPopup);

	bool SelectedProfileChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(SelectedProfileChange);

	bool DoubleClickSelection(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(DoubleClickSelection);

	bool PressSelectProfile(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressSelectProfile);

	bool PressCreateProfile(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressCreateProfile);

	bool PressDeleteProfile(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressDeleteProfile);

	bool PressDeleteProfilePopupClose(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressDeleteProfilePopupClose);

	
	bool PressEnterNameCreate(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressEnterNameCreate);

	bool PressEnterNameCancel(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressEnterNameCancel);

	bool UIPressEnterNameCancel(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(UIPressEnterNameCancel);

	bool ProfileSelectionClickChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ProfileSelectionClickChange);

	bool LockProfileList(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(LockProfileList);
};

//----------------------------------------------


#endif // LUX_MAIN_MENU_PROFILE_H
