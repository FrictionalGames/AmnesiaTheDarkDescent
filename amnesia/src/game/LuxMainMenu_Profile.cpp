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

#include "LuxMainMenu_Profile.h"

#include "LuxMap.h"
#include "LuxMapHandler.h"
#include "LuxInputHandler.h"

#include <algorithm>


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxMainMenu_Profile::cLuxMainMenu_Profile(cGuiSet *apGuiSet, cGuiSkin *apGuiSkin) : iLuxMainMenuWindow(apGuiSet, apGuiSkin)
{
	mlLastPickedProfile = -1;
	msDefaultProfileName = kTranslate("MainMenu","New Player");

	mvWindowSize = gpBase->mpMenuCfg->GetVector2f("Profiles","WindowSize",0);
	mfInformationWidth = gpBase->mpMenuCfg->GetFloat("Profiles","InformationWidth",0);
	mvListFontSize = gpBase->mpMenuCfg->GetVector2f("Profiles","ListFontSize",0);

	mvEnterNameWindowSize = gpBase->mpMenuCfg->GetVector2f("Profiles","EnterNameWindowSize",0);
	mfEnterNameButtonLength = gpBase->mpMenuCfg->GetFloat("Profiles","EnterNameButtonLength",0);
}

//-----------------------------------------------------------------------

cLuxMainMenu_Profile::~cLuxMainMenu_Profile()
{
	STLDeleteAll(mvProfiles);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------

void cLuxMainMenu_Profile::CreateGui()
{
	CreateMainGui();
	CreateEnterNameGui();
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Profile::ExitPressed()
{
	if(mpGuiSet->PopUpIsActive()) return;

    //TODO: If game is active, then return to game!
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMainMenu_Profile::OnSetActive(bool abX)
{
	if(abX)
	{
		mpGuiSet->SetDefaultFocusNavWidget(mpListProfiles);
		mpGuiSet->SetFocusedWidget(mpListProfiles);
		
		////////////////////////////////////////////////////////////
		// If no profiles found, start the Create Profile window
		if(mpListProfiles->GetItemNum()==0)
			PressCreateProfile(NULL, cGuiMessageData());
		else
		{
			mpListProfiles->SetIsLocked(true);
		}
	}
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Profile::CreateMainGui()
{
	cWidgetButton *pButton;

	float fWindowHeaderSize = 40;
	float fBorderSize = 35;

	cVector3f vPos(fBorderSize, fWindowHeaderSize+fBorderSize,1);

	//////////////////////////
	//Window
	//TODO: If a game is running, then add close button.
	mpWindow = mpGuiSet->CreateWidgetWindow(eWidgetWindowButtonFlag_ButtonClose,cVector3f(0,0,5),mvWindowSize,kTranslate("MainMenu","Profiles"));
	mpWindow->SetCloseButtonDisablesWindow(false);
	mpWindow->AddCallback(eGuiMessage_WindowClose,this, kGuiCallback(WindowCloses));
	mpWindow->AddCallback(eGuiMessage_UIButtonPress,this,kGuiCallback(WindowUIPress));
	mpWindow->SetGlobalUIInputListener(true);


	//////////////////////////
	//List box
	mpListProfiles = mpGuiSet->CreateWidgetListBox(vPos,cVector2f(mvWindowSize.x - mfInformationWidth - fBorderSize*2,
																mvWindowSize.y-vPos.y-40-fBorderSize*2),
													mpWindow);
	mpListProfiles->AddCallback(eGuiMessage_SelectionChange,this, kGuiCallback(SelectedProfileChange));
	mpListProfiles->AddCallback(eGuiMessage_SelectionDoubleClick,this, kGuiCallback(DoubleClickSelection));
	mpListProfiles->AddCallback(eGuiMessage_CheckChange,this,kGuiCallback(ProfileSelectionClickChange));
	mpListProfiles->AddCallback(eGuiMessage_GetUINavFocus, this, kGuiCallback(LockProfileList));
	mpListProfiles->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(UIPressList));
	mpListProfiles->SetDefaultFontSize(mvListFontSize);
	vPos.y += mpListProfiles->GetSize().y + fBorderSize;

	AddProfilesInListBox();

	if(mpListProfiles->GetItemNum()>0)
	{
		mpListProfiles->SetSelectedItem(mlLastPickedProfile);
		if(mpListProfiles->GetSelectedItem()==-1)
			mpListProfiles->SetSelectedItem(0);
	}


	//////////////////////////
	//Buttons
	std::vector<iWidget*> vButtons;
	float fButtonSepp = 3.0f;
	float fButtonLength = mpListProfiles->GetSize().x / 3.0f- fButtonSepp;
	//Select
	pButton = mpGuiSet->CreateWidgetButton(vPos,cVector2f(fButtonLength,30),kTranslate("MainMenu","Select"),mpWindow);
	pButton->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(PressSelectProfile));
	mpSelectButton = pButton;
	vPos.x += fButtonLength + fButtonSepp;

	vButtons.push_back(pButton);

	//Create
	pButton = mpGuiSet->CreateWidgetButton(vPos,cVector2f(fButtonLength,30),kTranslate("MainMenu","Create"),mpWindow);
	pButton->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(PressCreateProfile));

	vPos.x += fButtonLength + fButtonSepp;

	vButtons.push_back(pButton);

	//Delete
	pButton = mpGuiSet->CreateWidgetButton(vPos,cVector2f(fButtonLength,30),kTranslate("MainMenu","Delete"),mpWindow);
	pButton->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(PressDeleteProfile));
	vPos.x += fButtonLength + fButtonSepp;

	vButtons.push_back(pButton);

	//////////////////////////
	//Label
	/*vPos.x = fBorderSize*2 + mpListProfiles->GetSize().x;
	vPos.y = fWindowHeaderSize+fBorderSize;
	pLabel = mpGuiSet->CreateWidgetLabel(vPos,0,kTranslate("MainMenu","Information:"),mpWindow);*/

	/////////////////////////////////////////////
	// Set focus navigation
	mpListProfiles->SetFocusNavigation(eUIArrow_Down, vButtons[0]);
	for(size_t i=0; i<vButtons.size(); ++i)
	{
		int lPrev = i-1;
		int lNext = i+1;
		iWidget* pWidget = vButtons[i];

		pWidget->SetFocusNavigation(eUIArrow_Up, mpListProfiles);
		if(lPrev>=0)
			pWidget->SetFocusNavigation(eUIArrow_Left, vButtons[lPrev]);
		if(lNext<vButtons.size())
			pWidget->SetFocusNavigation(eUIArrow_Right, vButtons[lNext]);
	}
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Profile::CreateEnterNameGui()
{
	cWidgetButton *pButton;

	float fBorderSize = 5;

	cVector3f vPos(fBorderSize, 25+fBorderSize,1);

	//////////////////////////
	//Window
	vPos.y += 5;
	mpWindowEnterName = mpGuiSet->CreateWidgetWindow(eWidgetWindowButtonFlag_ButtonClose,cVector3f(0,0,10),mvEnterNameWindowSize,
													kTranslate("MainMenu","Create Profile"));
	//mpWindowEnterName->AddCallback(eGuiMessage_WindowClose,this, kGuiCallback(WindowCloses));

	//////////////////////////
	//Label
	cWidgetLabel* pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("MainMenu", "Enter name of Profile"), mpWindowEnterName);
    pLabel->SetAutogenerateSize(true);
	vPos.y += pLabel->GetSize().y + fBorderSize;
	//////////////////////////
	//Text Box
	tWString sIllegalChars = _W("*/\\:<>|\"?");
	mpTextEnterName = mpGuiSet->CreateWidgetTextBox(vPos,cVector2f(mvEnterNameWindowSize.x-fBorderSize*2, 20), _W(""),mpWindowEnterName);
	mpTextEnterName->SetForceCallBackOnEnter(true);
	mpTextEnterName->SetCallbackOnLostFocus(false);
	mpTextEnterName->SetIllegalChars(sIllegalChars);
	mpTextEnterName->AddCallback(eGuiMessage_TextBoxEnter, this, kGuiCallback(PressEnterNameCreate));
	

	mpTextEnterName->SetMaxTextLength(20);
	vPos.y += mpTextEnterName->GetSize().y + fBorderSize;

	//////////////////////////
	//Buttons
	float fButtonSepp = mvEnterNameWindowSize.x - mfEnterNameButtonLength*2 - fBorderSize*2;
    
	std::vector<iWidget*> vButtons;
	//Create
    //vPos.x += mpTextEnterName->GetSize().x - fButtonLength*2 - fButtonSepp; //right align buttons!
	vPos.y =  mvEnterNameWindowSize.y - 30 - fBorderSize; 
	pButton = mpGuiSet->CreateWidgetButton(vPos,cVector2f(mfEnterNameButtonLength, 30),kTranslate("MainMenu","Create"),mpWindowEnterName);
	pButton->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(PressEnterNameCreate));
	mpCreateButton = pButton;
	mpTextEnterName->SetUserData(mpCreateButton);
	vButtons.push_back(pButton);

	//Cancel
	vPos.x += mfEnterNameButtonLength + fButtonSepp;
	pButton = mpGuiSet->CreateWidgetButton(vPos,cVector2f(mfEnterNameButtonLength, 30),kTranslate("MainMenu","Cancel"),mpWindowEnterName);
	pButton->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(PressEnterNameCancel));
	pButton->AddCallback(eGuiMessage_UIButtonPress,this, kGuiCallback(UIPressEnterNameCancel));
	pButton->SetGlobalUIInputListener(true);
	vButtons.push_back(pButton);

	//////////////////////////
	//Final set up
	mpWindowEnterName->SetVisible(false);
	mpWindowEnterName->SetEnabled(false);
	mpWindowEnterName->CenterGlobalPositionInSet();
	mpWindowEnterName->SetStatic(true);


	/////////////////////////////////////
	// Set up focus navigation
	mpTextEnterName->SetFocusNavigation(eUIArrow_Down, vButtons[0]);
	vButtons[0]->SetFocusNavigation(eUIArrow_Right, vButtons[1]);
	vButtons[1]->SetFocusNavigation(eUIArrow_Left, vButtons[0]);
	for(size_t i=0; i<vButtons.size(); ++i)
		vButtons[i]->SetFocusNavigation(eUIArrow_Up, mpTextEnterName);
}

//-----------------------------------------------------------------------

static bool ProfileSort(cLuxGameProfile* apProfileA,const cLuxGameProfile* apProfileB)
{
	return apProfileA->mCreationDate < apProfileB->mCreationDate;
}

void cLuxMainMenu_Profile::AddProfilesInListBox()
{
	mpListProfiles->ClearItems();
	STLDeleteAll(mvProfiles);

	tWStringList lstFolders;

	cPlatform::FindFoldersInDir(lstFolders,gpBase->msBaseSavePath,false,false);

	//Add the profile combos to list
	tWStringListIt it = lstFolders.begin();
	for(; it != lstFolders.end(); ++it)
	{
		cLuxGameProfile* pProfile = hplNew(cLuxGameProfile, ());
		pProfile->msName = *it;
		pProfile->mCreationDate = cPlatform::FileCreationDate(gpBase->msBaseSavePath+pProfile->msName);
		//Log("%s: %d:%d:%d %d/%d-%d\n",cString::To8Char(combo.msFolder).c_str(), 
		//							combo.mCreationDate.hours,combo.mCreationDate.minutes,combo.mCreationDate.seconds,
		//							combo.mCreationDate.month_day,combo.mCreationDate.month,combo.mCreationDate.year);

		mvProfiles.push_back(pProfile);
	}

	//Sort by date and add
	std::sort(mvProfiles.begin(), mvProfiles.end(),ProfileSort);		
	for(size_t i=0; i<mvProfiles.size(); ++i)
	{
		cLuxGameProfile *pProfile = mvProfiles[i];

		mpListProfiles->AddItem(pProfile->msName);
	}
	

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CALLBACKS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxMainMenu_Profile::WindowCloses(iWidget* apWidget, const cGuiMessageData& aData)
{
	/////////
	//User config is loaded just return to menu
	if(gpBase->mpUserConfig)
	{
		gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_LastEnum);
		gpBase->mpMainMenu->SetTopMenuAlpha(0.01f);

		mpWindow->SetEnabled(false);
		mpWindow->SetVisible(false);
		if(mpGuiSet->GetAttentionWidget() == mpWindow) mpGuiSet->SetAttentionWidget(NULL);
	}
	/////////
	//User config not loaded, pop up quit game message box
	else
	{
		cGuiPopUpMessageBox* pPopUp =  mpGuiSet->CreatePopUpMessageBox(_W(""),kTranslate("MainMenu", "Sure you want to quit?"),
										kTranslate("MainMenu", "Yes"), kTranslate("MainMenu", "No"),
										this,
										kGuiCallback(ClickedExitPopup));
		pPopUp->GetGuiSet()->SetDrawFocus(mpGuiSet->GetDrawFocus());
	}
	
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Profile, WindowCloses);
//-----------------------------------------------------------------------

bool cLuxMainMenu_Profile::UIPressList(iWidget* apWidget, const cGuiMessageData& aData)
{
	switch(aData.mlVal)
	{
	case eUIButton_Secondary: return WindowCloses(apWidget, aData);
	}

	return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Profile, UIPressList);

bool cLuxMainMenu_Profile::WindowUIPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal == eUIButton_Secondary)
	{
		if(mpListProfiles->GetIsLocked()) return false;
		if(mpTextEnterName->IsVisible()) return false;

		return WindowCloses(apWidget, aData);
	}

	return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Profile, WindowUIPress);

bool cLuxMainMenu_Profile::ClickedExitPopup(iWidget* apWidget, const cGuiMessageData& aData)
{
	bool bExit = aData.mlVal ==0 ? true : false;

	if(bExit)
	{
		gpBase->mpEngine->Exit();
	}
	else
	{
		mpListProfiles->SetIsLocked(true);
	}

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Profile, ClickedExitPopup);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Profile::SelectedProfileChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	//TODO: Update information!

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Profile, SelectedProfileChange);

//-----------------------------------------------------------------------

void cLuxMainMenu_Profile::SelectProfile(int alNum)
{
	if(alNum < 0) return;

	mlLastPickedProfile = alNum;

	cLuxGameProfile *pProfile = mvProfiles[alNum];

	//TODO: Make sure to clear stuff the other profile might have set
	//		Especially language resources!

	gpBase->SetProfile(pProfile->msName);
	
	gpBase->InitUserConfig();

	gpBase->mpMainMenu->RecreateGui();
	gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_LastEnum);
	gpBase->mpMainMenu->SetTopMenuAlpha(0.01f);
}

bool cLuxMainMenu_Profile::DoubleClickSelection(iWidget* apWidget, const cGuiMessageData& aData)
{
	SelectProfile(mpListProfiles->GetSelectedItem());
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Profile, DoubleClickSelection);

bool cLuxMainMenu_Profile::PressSelectProfile(iWidget* apWidget, const cGuiMessageData& aData)
{
	SelectProfile(mpListProfiles->GetSelectedItem());
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Profile, PressSelectProfile);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Profile::PressCreateProfile(iWidget* apWidget, const cGuiMessageData& aData)
{
	mpWindowEnterName->SetVisible(true);
	mpWindowEnterName->SetEnabled(true);
	mpWindowEnterName->CenterGlobalPositionInSet();

	mpGuiSet->PushAttentionWidget();
	mpGuiSet->PushDefaultFocusNavWidget();
	mpGuiSet->PushFocusedWidget();

	mpGuiSet->SetAttentionWidget(mpWindowEnterName);
	
	mpTextEnterName->SetText(msDefaultProfileName);
	mpTextEnterName->SetSelectedText();
	
	mpGuiSet->SetFocusedWidget(mpTextEnterName);
	mpGuiSet->SetDefaultFocusNavWidget(mpTextEnterName);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Profile, PressCreateProfile);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Profile::PressDeleteProfile(iWidget* apWidget, const cGuiMessageData& aData)
{
	cGuiPopUpMessageBox* pPopUp;

	if(mpListProfiles->GetSelectedItem()<0)
	{
		pPopUp = mpGuiSet->CreatePopUpMessageBox(_W(""),kTranslate("MainMenu","No profile to be deleted!"), kTranslate("MainMenu", "OK"),_W(""),NULL,NULL);
		pPopUp->GetGuiSet()->SetDrawFocus(mpGuiSet->GetDrawFocus());
		return true;
	}

	pPopUp = mpGuiSet->CreatePopUpMessageBox(_W(""),kTranslate("MainMenu","Sure you want to delete profile"),
									kTranslate("MainMenu","Yes"),kTranslate("MainMenu","No"),this,kGuiCallback(PressDeleteProfilePopupClose));
	pPopUp->GetGuiSet()->SetDrawFocus(mpGuiSet->GetDrawFocus());
	
	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Profile, PressDeleteProfile);


bool cLuxMainMenu_Profile::PressDeleteProfilePopupClose(iWidget* apWidget, const cGuiMessageData& aData)
{
	bool bDeleteProfile = aData.mlVal ==0 ? true : false;
	if(bDeleteProfile==false) return true;

	tWString sFolder = mpListProfiles->GetItem(mpListProfiles->GetSelectedItem())->GetText();

	///////////////////////////////////////////////////////////////
	// Check if profile is in use
	if(gpBase->mpUserConfig)
	{
		tWString sCurrentProfile = gpBase->mpUserConfig->GetFileLocation();
		sCurrentProfile = cString::SubW(sCurrentProfile, (int)gpBase->msBaseSavePath.length());

		tWString sSeparators = _W("/\\");
		tWStringVec vCurrentProfilePath;
		cString::GetStringVecW(sCurrentProfile, vCurrentProfilePath, &sSeparators);

		////////////////////////////////////////////////
		// If folders are the same, profile is in use!
		if(vCurrentProfilePath[0]==sFolder)
		{
			// Set a blank profile and config
			gpBase->SetProfile(_W(""));
	
			gpBase->InitUserConfig();
		}
	}

	int lNewSelection = mpListProfiles->GetSelectedItem()-1;

	if(cPlatform::RemoveFolder(gpBase->msBaseSavePath+sFolder, true,true)==false)
	{
		cGuiPopUpMessageBox* pPopUp =  mpGuiSet->CreatePopUpMessageBox(_W(""),kTranslate("MainMenu","Could not remove profile"), kTranslate("MainMenu", "OK"),_W(""),NULL,NULL);
		pPopUp->GetGuiSet()->SetDrawFocus(mpGuiSet->GetDrawFocus());
		return true;
	}

	AddProfilesInListBox();

	mpListProfiles->SetSelectedItem(lNewSelection);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Profile, PressDeleteProfilePopupClose);

//-----------------------------------------------------------------------

//Encoded string 'http://steamcommunity.com/as/da/rkdesce/nt/?k='
static unsigned char gvEncStr[54] = {0x2B, 0x15, 0x7, 0x12, 0x5B, 0x47, 0x6C, 0x30, 0x37, 0x26, 0x22, 0x2E, 0x20, 0x2C, 0x2E, 0x2E, 0x36, 0x2D, 0x2A, 0x37, 0x3A, 0x6D, 0x20, 0x2C, 0x2E, 0x6C, 0x22, 0x30, 0x6C, 0x21, 0x2C, 0x36, 0x2D, 0x20, 0x26, 0x6C, 0x27, 0x22, 0x6C, 0x31, 0x28, 0x27, 0x26, 0x30, 0x20, 0x26, 0x6C, 0x2D, 0x37, 0x6C, 0x7C, 0x28, 0x7E, 0};
#define kCRCKey			0xDD4B3114
#define kKeyBufferCRC		0xA9DA472C

static tString DecryptCryptoStringCode(const tString &asEncStr, cBinaryBuffer &aKeyBuffer)
{
	size_t lBuffPos=0;
	tString sOutStr;

	aKeyBuffer.SetPos(0);
	for(size_t i=0; i<asEncStr.size(); ++i)
	{
		sOutStr +=  asEncStr[i] ^ aKeyBuffer.GetChar();
		if(++lBuffPos >= aKeyBuffer.GetSize()) aKeyBuffer.SetPos(0);
	}

	return sOutStr;
}

bool cLuxMainMenu_Profile::PressEnterNameCreate(iWidget* apWidget, const cGuiMessageData& aData)
{
	/////////////////////////
	// Check if the name already exists
	tWString sProfileName = mpTextEnterName->GetText();

	
	while(sProfileName.size() && sProfileName[0] == ' ') sProfileName = sProfileName.substr(1);
	while(sProfileName.size() && sProfileName[sProfileName.size()-1] == ' ') sProfileName = sProfileName.substr(0, sProfileName.size()-1);
	
	if(sProfileName.empty())
	{
		cGuiPopUpMessageBox* pPopUp = mpGuiSet->CreatePopUpMessageBox(_W(""),kTranslate("MainMenu","The profile name already exists"), kTranslate("MainMenu","OK"),_W(""),NULL,NULL);
		pPopUp->GetGuiSet()->SetDrawFocus(mpGuiSet->GetDrawFocus());
		return true;
	}

	if(gpBase->CreateProfile(sProfileName)==false)
	{
		cGuiPopUpMessageBox* pPopUp = mpGuiSet->CreatePopUpMessageBox(_W(""),kTranslate("MainMenu","The profile name already exists"), kTranslate("MainMenu", "OK"),_W(""),NULL,NULL);
		pPopUp->GetGuiSet()->SetDrawFocus(mpGuiSet->GetDrawFocus());
		return true;
	}

	/////////////////////////
	// Close window and update list.
	mpWindowEnterName->SetVisible(false);
	mpWindowEnterName->SetEnabled(false);

	mpGuiSet->PopAttentionWidget();
	mpGuiSet->PopDefaultFocusNavWidget();
	mpGuiSet->PopFocusedWidget();
	
	AddProfilesInListBox();

    mpListProfiles->SetSelectedItem(mpListProfiles->GetItemNum()-1);

	mpGuiSet->SetFocusedWidget(mpSelectButton);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Profile, PressEnterNameCreate);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Profile::PressEnterNameCancel(iWidget* apWidget, const cGuiMessageData& aData)
{
	mpWindowEnterName->SetVisible(false);
	mpWindowEnterName->SetEnabled(false);

	mpGuiSet->PopAttentionWidget();
	mpGuiSet->PopDefaultFocusNavWidget();
	mpGuiSet->PopFocusedWidget();

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Profile, PressEnterNameCancel);

//----------------------------------------------------------------------

bool cLuxMainMenu_Profile::UIPressEnterNameCancel(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal == eUIButton_Secondary)
	{
		mpWindowEnterName->SetVisible(false);
		mpWindowEnterName->SetEnabled(false);

		mpGuiSet->PopAttentionWidget();
		mpGuiSet->PopDefaultFocusNavWidget();
		mpGuiSet->PopFocusedWidget();

		return true;
	}

	return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Profile, UIPressEnterNameCancel);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Profile::ProfileSelectionClickChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	mpGuiSet->SetFocusedWidget(mpSelectButton);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Profile, ProfileSelectionClickChange);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Profile::LockProfileList(iWidget* apWidget, const cGuiMessageData& aData)
{
	mpListProfiles->SetIsLocked(true);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Profile, LockProfileList);

//----------------------------------------------------------------------
