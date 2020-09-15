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

#ifndef LUX_MAIN_MENU_KEY_CONFIG_H
#define LUX_MAIN_MENU_KEY_CONFIG_H

//----------------------------------------------

#include "LuxMainMenu.h"

//----------------------------------------------

class cLuxAction;
class cLuxInput;
class cLuxMainMenu_KeyConfig;

//----------------------------------------------

///////////////////////////////////////////////
// Helper wrapper for SubActions
///////////////////////////////////////////////

class cSubActionWrapper
{
public:
	cSubActionWrapper(eInputDeviceType aType, int alInputId) : 
	  mType(aType), mGamepadInputType(eGamepadInputType_LastEnum), mlInputId(alInputId), mfInputValue(1.0f) {}
	cSubActionWrapper(eGamepadInputType aType, int alInputId, float afInputValue) : 
	  mType(eInputDeviceType_Gamepad), mGamepadInputType(aType), mlInputId(alInputId), mfInputValue(afInputValue) {}

	eInputDeviceType mType;
	eGamepadInputType mGamepadInputType;
	int mlInputId;
	float mfInputValue;
};

//----------------------------------------------

///////////////////////////////////////////////
// Helper wrapper for input entries
///////////////////////////////////////////////

//----------------------------------------------

enum eInputMenuEntryPos
{
	eInputMenuEntryPos_Primary,
	eInputMenuEntryPos_Secondary,

	eInputMenuEntryPos_LastEnum
};

//----------------------------------------------

typedef std::vector<eInputMenuEntryPos> tMenuEntryPosVec;

//----------------------------------------------

class cLuxInputMenuEntry
{
public:
	cLuxInputMenuEntry(cLuxMainMenu_KeyConfig* apWindow,
					   cLuxAction* apAction,
					   iWidget* apParent,
					   const cVector3f& avPos);
	~cLuxInputMenuEntry();

	void RetrieveDefaultValue();
	void RetrieveInitialValue();

	void UpdateAction();
	void UpdateGamepadAction(cAction* apAction, const cSubActionWrapper& aSubAction);
	void UpdateEntry();

	void SetInputKeypress(eInputMenuEntryPos aPos, eKey aKey);
	void SetInputMouseButton(eInputMenuEntryPos aPos, eMouseButton aButton);
	void SetInputGamepadButton(eInputMenuEntryPos aPos, eGamepadButton aButton);
	void SetInputGamepadHat(eInputMenuEntryPos aPos, eGamepadHat aHat, eGamepadHatState aState);
	void SetInputGamepadAxis(eInputMenuEntryPos aPos, eGamepadAxis aAxis, eGamepadAxisRange aRange);

	eInputMenuEntryPos GetKeyPos(eKey aKey);
	eInputMenuEntryPos GetMouseButtonPos(eMouseButton aButton);
	eInputMenuEntryPos GetGamepadButtonPos(eGamepadButton aButton);
	eInputMenuEntryPos GetGamepadHatStatePos(eGamepadHat aHat, eGamepadHatState aState);
	eInputMenuEntryPos GetGamepadAxisPos(eGamepadAxis aAxis, eGamepadAxisRange aRange);

	bool HasKey(eKey aKey);
	bool HasMouseButton(eMouseButton aButton);
	bool HasGamepadButton(eGamepadButton aButton);
	bool HasGamepadHatState(eGamepadHat aHat, eGamepadHatState aState);
	bool HasGamepadAxis(eGamepadAxis aAxis, eGamepadAxisRange aRange);

	bool HasSubActions();
	tMenuEntryPosVec GetPosSharingSubAction(cLuxInputMenuEntry* apEntry);

	void SetColor(eInputMenuEntryPos aPos, const cColor& aCol);

	cWidgetLabel* mpLAction;
	std::vector<cWidgetLabel*> mvLKeyInputs;

	int mlActionId;
	std::vector<cSubActionWrapper> mvSubActions;
private:
	cLuxMainMenu_KeyConfig* mpWindow;
	std::vector<cColor> mvInputColors;
	bool mbChanged;

	///////////////////////////
	// Helper funcs
	tString GetStringFromSubAction(const cSubActionWrapper& aSubAction);
	tString GetStringFromGamepadSubAction(const cSubActionWrapper& aSubAction);

	cSubActionWrapper GetSubActionWrapperFromSubAction(iSubAction* apSubAction);
	cSubActionWrapper GetSubActionWrapperFromLuxInput(cLuxInput* apInput);
	
	cSubActionWrapper GetSubActionWrapperFromString(const tString& asX, int alValue);

	cSubActionWrapper GetSubActionWrapperFromStringVecKeyboard(const tStringVec& avX, int alValue);
	cSubActionWrapper GetSubActionWrapperFromStringVecMouse(const tStringVec& avX, int alValue);
	cSubActionWrapper GetSubActionWrapperFromStringVecGamepad(const tStringVec& avX, int alValue);

	eInputDeviceType GetInputDeviceTypeFromString(const tString& asX);

	eInputMenuEntryPos GetPosFromInput(iWidget* apWidget);

	void SaveSubAction(eInputMenuEntryPos aPos, eInputDeviceType aType, int alInputId);
	void SaveGamepadSubAction(eInputMenuEntryPos aPos, eGamepadInputType, int alInputId, float afValue);

	void SetWaiting(bool abX, eInputMenuEntryPos aCat);
	bool IsWaiting();

	///////////////////////////
	// Callbacks
	bool InputEntryMouseOver(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputEntryMouseOver);

	//bool InputEntryMouseMove(iWidget* apWidget, const cGuiMessageData& aData);
	//kGuiCallbackDeclarationEnd(InputEntryMouseMove);

	bool InputEntryMouseEnter(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputEntryMouseEnter);

	bool InputEntryMouseLeave(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputEntryMouseLeave);

	bool InputEntryChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputEntryChange);

	bool InputEntryClick(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputEntryClick);

	bool InputEntryKeyPress(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputEntryKeyPress);

	bool InputEntryUIButtonPress(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputEntryUIButtonPress);

	bool InputEntryUIArrowPress(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputEntryUIArrowPress);

	bool InputEntryGamepadButtonPress(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(InputEntryGamepadButtonPress);

	static bool mbGamepadButtonPress;
};

typedef std::vector<cLuxInputMenuEntry*> tInputEntryVec;
typedef tInputEntryVec::iterator tInputEntryVecIt;

//----------------------------------------------

class cLuxMainMenu_KeyConfig : public iLuxMainMenuWindow
{
	friend class cLuxInputMenuEntry;
public:
	cLuxMainMenu_KeyConfig(cGuiSet *apGuiSet, cGuiSkin *apGuiSkin);
	~cLuxMainMenu_KeyConfig();

	void CreateGui();

	void ExitPressed();

	iWidget* GetWaitingInput() { return mpWaitingInput; }
	void SetWaitingInput(iWidget* apInput);

	void CancelInputSetting() { mbInputSettingCancelled = true; }
private:
	void OnSetActive(bool abX);
	
	cLuxInputMenuEntry* CreateInputEntry(cLuxAction* apAction, iWidget* apParent, const cVector3f& avPos);

	void SetCurrentToolTipEntry(cLuxInputMenuEntry* apEntry);
	cLuxInputMenuEntry* GetCurrentToolTipEntry() { return mpCurrentTipEntry; }

	void ApplyChanges();
	void SetDefaultValues();
	void SetInitialValues();

	void CheckEmptyAndDuplicateInputs();
	////////////////////////
	// Properties
	cVector2f mvWindowSize;

	bool mbSettingInitialValues;

	bool mbInputSettingCancelled;

	iWidget* mpWaitingInput;
	bool mbInvalidInputs;

	////////////////////////
	// Layout
	cVector2f mvHeaderPositions;
	cVector2f mvEntrySize;
	float mfEntrySep;

	// Tip Label
	cWidgetLabel *mpLTip;
	bool mbTipFadeOut;
	bool mbTipFadeRestore;
	bool mbTipWidgetUpdated;
	//iWidget* mpCurrentTipWidget;
	cLuxInputMenuEntry* mpCurrentTipEntry;
	tWString msTip;

	bool mbWasWaitingOnInput;

	// Inputs
	std::vector<cWidgetDummy*> mvDKeyGroups;
	std::vector<tInputEntryVec> mvInputs;

	cWidgetComboBox* mpCBCategory;

	cWidgetButton* mpBDefaultKeys;
	cWidgetButton* mpBOK;
	cWidgetButton* mpBCancel;

	////////////////////////
	// Callbacks
	bool WindowOnUpdate(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(WindowOnUpdate);

	bool CategoryChange(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(CategoryChange);

	bool CategorySelector_OnUIButtonPress(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(CategorySelector_OnUIButtonPress);
	
	bool PressSetDefault(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressSetDefault);

	bool PressOK(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressOK);

	bool PressCancel(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(PressCancel);

	bool UIPressCancel(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(UIPressCancel);

	bool LoadDefaultsCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(LoadDefaultsCallback);

	bool ExitCallback(iWidget* apWidget, const cGuiMessageData& aData);
	kGuiCallbackDeclarationEnd(ExitCallback);
};

//----------------------------------------------

#endif // LUX_MAIN_MENU_KEY_CONFIG_H
