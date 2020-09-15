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

#include "LuxMainMenu_KeyConfig.h"

#include "LuxInputHandler.h"

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxInputMenuEntry::mbGamepadButtonPress = false;

cLuxInputMenuEntry::cLuxInputMenuEntry(cLuxMainMenu_KeyConfig* apWindow, cLuxAction* apAction, iWidget* apParent, const cVector3f& avPos)
{
	mpWindow = apWindow;
	mlActionId = apAction->mlId;

	mbChanged = false;

	mpLAction = mpWindow->mpGuiSet->CreateWidgetLabel(avPos, -1, kTranslate("Actions", apAction->msName), apParent);
	mpLAction->AddCallback(eGuiMessage_MouseEnter, this, kGuiCallback(InputEntryMouseEnter));
	mpLAction->AddCallback(eGuiMessage_MouseLeave, this, kGuiCallback(InputEntryMouseLeave));
	mpLAction->AddCallback(eGuiMessage_GetUINavFocus, this, kGuiCallback(InputEntryMouseEnter));
	mpLAction->AddCallback(eGuiMessage_LoseUINavFocus, this, kGuiCallback(InputEntryMouseLeave));
	//mpLAction->AddCallback(eGuiMessage_GetUINavFocus, this, kGuiCallback(InputEntryMouseMove));
	//mpLAction->AddCallback(eGuiMessage_MouseMove, this, kGuiCallback(InputEntryMouseMove));
	mpLAction->AddCallback(eGuiMessage_TextChange, this, kGuiCallback(InputEntryChange));
	//mpLAction->AddCallback(eGuiMessage_MouseDown, this, kGuiCallback(InputEntryClick));
	//mpLAction->AddCallback(eGuiMessage_KeyPress, this, kGuiCallback(InputEntryKeyPress));

	if((apAction->msName == "LeanLeft"
	|| apAction->msName == "LeanRight") == false)
	{
		mpLAction->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(InputEntryUIButtonPress));
	}

	mpLAction->AddCallback(eGuiMessage_UIArrowPress, this, kGuiCallback(InputEntryUIArrowPress));

	mpLAction->SetUserData(this);
	
	///////////////////////////////////////////
	// Set up primary and secondary Key boxes
	for(int i=0; i<eInputMenuEntryPos_LastEnum; ++i)
	{
		cWidgetLabel* pLabel = mpWindow->mpGuiSet->CreateWidgetLabel(cVector3f(mpWindow->mvHeaderPositions.v[i],0,0), mpWindow->mvEntrySize,_W(""), mpLAction);
		pLabel->SetDefaultFontColor(cColor(1,1));

		pLabel->SetTextAlign(eFontAlign_Center);

		if(i == 0)
		{
			////////////
			// It should only be possible to bind mouse to primary
			pLabel->AddCallback(eGuiMessage_MouseEnter, this, kGuiCallback(InputEntryMouseOver));
			pLabel->AddCallback(eGuiMessage_MouseLeave, this, kGuiCallback(InputEntryMouseOver));
			pLabel->AddCallback(eGuiMessage_MouseEnter, this, kGuiCallback(InputEntryMouseEnter));
			pLabel->AddCallback(eGuiMessage_MouseLeave, this, kGuiCallback(InputEntryMouseLeave));
			pLabel->AddCallback(eGuiMessage_MouseDown, this, kGuiCallback(InputEntryClick));
		}
		//pLabel->AddCallback(eGuiMessage_MouseMove, this, kGuiCallback(InputEntryMouseMove));
		pLabel->AddCallback(eGuiMessage_TextChange, this, kGuiCallback(InputEntryChange));
		pLabel->AddCallback(eGuiMessage_KeyPress, this, kGuiCallback(InputEntryKeyPress));
		
		if((apAction->msName == "LeanLeft"
		|| apAction->msName == "LeanRight") == false)
		{
			//////////////
			// Not possible to bind lean to gamepad
			pLabel->AddCallback(eGuiMessage_GamepadInput, this, kGuiCallback(InputEntryGamepadButtonPress));
		}
		pLabel->SetUserData(this);
		pLabel->SetUserValue(i);

		// Not drawing bg anymore
		//pLabel->SetDrawBackGround(true);

		mvLKeyInputs.push_back(pLabel);
		mvInputColors.push_back(cColor(1,1));
		mvSubActions.push_back(cSubActionWrapper(eInputDeviceType_LastEnum, -1));
	}
}

cLuxInputMenuEntry::~cLuxInputMenuEntry()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxInputMenuEntry::RetrieveDefaultValue()
{
	tLuxInputVec vDefaultInputs = gpBase->mpInputHandler->GetDefaultInputsByActionId(mlActionId);
	
	size_t i=0;
	for(;i<vDefaultInputs.size();++i)
	{
		if(i>=eInputMenuEntryPos_LastEnum)
			break;

		cLuxInput* pInput = vDefaultInputs[i];

		mvSubActions[i] = GetSubActionWrapperFromLuxInput(pInput);
		//mvSubActions[i].mType = GetInputDeviceTypeFromString(pInput->msInputType);
		//mvSubActions[i].mlInputId = pInput->mlValue;
	}
#ifdef __APPLE__
	// Heinous Kludge to get a default Mac keyboard shortcut without relying on different config files.
	if (i < eInputMenuEntryPos_LastEnum && mlActionId == eLuxAction_Attack) {
		mvSubActions[i].mType = eInputDeviceType_Keyboard;
		mvSubActions[i].mlInputId = eKey_LeftMeta;
		++i;
	}
#endif
	for(;i<eInputMenuEntryPos_LastEnum;++i)
	{
		mvSubActions[i].mType = eInputDeviceType_LastEnum;
		mvSubActions[i].mlInputId = -1;
	}

	///////////////////////////////////////////////////////////
	// Set changed to true, as we might have changed stuff
	mbChanged = true;
}

//-----------------------------------------------------------------------

void cLuxInputMenuEntry::RetrieveInitialValue()
{
	cInput* pInput = gpBase->mpEngine->GetInput();
	cAction* pAction = pInput->GetAction(mlActionId);

	int i=0;
	for(;i<pAction->GetSubActionNum();++i)
	{
		if(i>=eInputMenuEntryPos_LastEnum)
			break;

		iSubAction* pSubAction = pAction->GetSubAction(i);
		
		mvSubActions[i] = GetSubActionWrapperFromSubAction(pSubAction);
	}

	for(;i<eInputMenuEntryPos_LastEnum;++i)
	{
		mvSubActions[i].mType = eInputDeviceType_LastEnum;
		mvSubActions[i].mlInputId = -1;
	}
}

//-----------------------------------------------------------------------

void cLuxInputMenuEntry::UpdateAction()
{
	if(mbChanged==false)
		return;

	cInput* pInp = gpBase->mpEngine->GetInput();
	cAction* pAction = pInp->GetAction(mlActionId);

	pAction->ClearSubActions();
	for(size_t i=0;i<mvSubActions.size();++i)
	{
		const cSubActionWrapper& subAction = mvSubActions[i];
		int lId = subAction.mlInputId;
		switch(subAction.mType)
		{
		case eInputDeviceType_Keyboard:
			pAction->AddKey((eKey)lId);
			break;
		case eInputDeviceType_Mouse:
			pAction->AddMouseButton((eMouseButton)lId);
			break;
		case eInputDeviceType_Gamepad:
			UpdateGamepadAction(pAction, subAction);
			break;
		}
	}
	mbChanged = false;
}

void cLuxInputMenuEntry::UpdateGamepadAction(cAction* pAction, const cSubActionWrapper& aSubAction)
{
	cInput* pInp = gpBase->mpEngine->GetInput();
	switch(aSubAction.mGamepadInputType)
	{
		case eGamepadInputType_Button:
			pAction->AddGamepadButton(0, (eGamepadButton)aSubAction.mlInputId);
			break;
		case eGamepadInputType_Hat:
			{
				int lState = (int)aSubAction.mfInputValue;
				pAction->AddGamepadHat(0, (eGamepadHat)aSubAction.mlInputId, (eGamepadHatState)lState);
			}
			break;
		case eGamepadInputType_Axis:
			{
				int lRange = (int)aSubAction.mfInputValue;
				pAction->AddGamepadAxis(0, (eGamepadAxis)aSubAction.mlInputId, (eGamepadAxisRange)lRange, 0.25f);
			}
			break;
	}
}

//-----------------------------------------------------------------------

void cLuxInputMenuEntry::UpdateEntry()
{
	mpWindow->CheckEmptyAndDuplicateInputs();

	tWStringVec vInputStrings;
	vInputStrings.resize(eInputMenuEntryPos_LastEnum);

	
	for(int i=0;i<eInputMenuEntryPos_LastEnum;++i)
	{
		const cSubActionWrapper& subAction = mvSubActions[i];
		cWidgetLabel* pInput = mvLKeyInputs[i];

		//pInput->SetText(kTranslate("ButtonNames", GetStringFromSubAction(subAction)));
		pInput->SetText(kTranslate("ButtonNames", GetStringFromSubAction(subAction)));
	}
}

//-----------------------------------------------------------------------

void cLuxInputMenuEntry::SetInputKeypress(eInputMenuEntryPos aPos, eKey aKey)
{
	if(HasKey(aKey))
		return;

	mvSubActions[aPos] = cSubActionWrapper(eInputDeviceType_Keyboard, aKey);
}

void cLuxInputMenuEntry::SetInputMouseButton(eInputMenuEntryPos aPos, eMouseButton aButton)
{
	if(HasMouseButton(aButton))
		return;

	mvSubActions[aPos] = cSubActionWrapper(eInputDeviceType_Mouse, aButton);
}

void cLuxInputMenuEntry::SetInputGamepadButton(eInputMenuEntryPos aPos, eGamepadButton aButton)
{
	if(HasGamepadButton(aButton))
		return;

	mvSubActions[aPos] = cSubActionWrapper(eGamepadInputType_Button, aButton, 1.0f);
}

void cLuxInputMenuEntry::SetInputGamepadHat(eInputMenuEntryPos aPos, eGamepadHat aHat, eGamepadHatState aState)
{
	if(HasGamepadHatState(aHat, aState))
		return;

	mvSubActions[aPos] = cSubActionWrapper(eGamepadInputType_Hat, aHat, (float)aState);
}

void cLuxInputMenuEntry::SetInputGamepadAxis(eInputMenuEntryPos aPos, eGamepadAxis aAxis, eGamepadAxisRange aRange)
{
	if(HasGamepadAxis(aAxis, aRange))
		return;

	mvSubActions[aPos] = cSubActionWrapper(eGamepadInputType_Axis, aAxis, (float)aRange);
}

//-----------------------------------------------------------------------

eInputMenuEntryPos cLuxInputMenuEntry::GetKeyPos(eKey aKey)
{
	for(size_t i=0;i<mvSubActions.size();++i)
	{
		const cSubActionWrapper& subAction = mvSubActions[i];
		if(subAction.mType==eInputDeviceType_Keyboard &&
			subAction.mlInputId==aKey)
			return (eInputMenuEntryPos)i;
	}

	return eInputMenuEntryPos_LastEnum;
}

eInputMenuEntryPos cLuxInputMenuEntry::GetMouseButtonPos(eMouseButton aButton)
{
	for(size_t i=0;i<mvSubActions.size();++i)
	{
		const cSubActionWrapper& subAction = mvSubActions[i];
		if(subAction.mType==eInputDeviceType_Mouse &&
			subAction.mlInputId==aButton)
			return (eInputMenuEntryPos)i;
	}

	return eInputMenuEntryPos_LastEnum;
}

eInputMenuEntryPos cLuxInputMenuEntry::GetGamepadButtonPos(eGamepadButton aButton)
{
	for(size_t i=0;i<mvSubActions.size();++i)
	{
		const cSubActionWrapper& subAction = mvSubActions[i];
		if(subAction.mGamepadInputType==eGamepadInputType_Button &&
			subAction.mlInputId==aButton)
			return (eInputMenuEntryPos)i;
	}

	return eInputMenuEntryPos_LastEnum;
}

eInputMenuEntryPos cLuxInputMenuEntry::GetGamepadHatStatePos(eGamepadHat aHat, eGamepadHatState aState)
{
	for(size_t i=0;i<mvSubActions.size();++i)
	{
		const cSubActionWrapper& subAction = mvSubActions[i];
		if(subAction.mGamepadInputType==eGamepadInputType_Hat &&
			subAction.mlInputId==aHat && subAction.mfInputValue==aState)
			return (eInputMenuEntryPos)i;
	}

	return eInputMenuEntryPos_LastEnum;
}

eInputMenuEntryPos cLuxInputMenuEntry::GetGamepadAxisPos(eGamepadAxis aAxis, eGamepadAxisRange aRange)
{
	for(size_t i=0;i<mvSubActions.size();++i)
	{
		const cSubActionWrapper& subAction = mvSubActions[i];
		if(subAction.mGamepadInputType==eGamepadInputType_Axis &&
			subAction.mlInputId==aAxis && subAction.mfInputValue==aRange)
			return (eInputMenuEntryPos)i;
	}

	return eInputMenuEntryPos_LastEnum;
}

//-----------------------------------------------------------------------

bool cLuxInputMenuEntry::HasKey(eKey aKey)
{
	return GetKeyPos(aKey)!=eInputMenuEntryPos_LastEnum;
}

bool cLuxInputMenuEntry::HasMouseButton(eMouseButton aButton)
{
	return GetMouseButtonPos(aButton)!=eInputMenuEntryPos_LastEnum;
}

bool cLuxInputMenuEntry::HasGamepadButton(eGamepadButton aButton)
{
	return GetGamepadButtonPos(aButton)!=eInputMenuEntryPos_LastEnum;
}

bool cLuxInputMenuEntry::HasGamepadHatState(eGamepadHat aHat, eGamepadHatState aState)
{
	return GetGamepadHatStatePos(aHat,aState)!=eInputMenuEntryPos_LastEnum;
}

bool cLuxInputMenuEntry::HasGamepadAxis(eGamepadAxis aAxis, eGamepadAxisRange aRange)
{
	return GetGamepadAxisPos(aAxis,aRange)!=eInputMenuEntryPos_LastEnum;
}

//-----------------------------------------------------------------------

bool cLuxInputMenuEntry::HasSubActions()
{
	for(int i=0;i<eInputMenuEntryPos_LastEnum;++i)
	{
		if(mvSubActions[i].mType!=eInputDeviceType_LastEnum)
			return true;
	}

	return false;
}

//-----------------------------------------------------------------------

tMenuEntryPosVec cLuxInputMenuEntry::GetPosSharingSubAction(cLuxInputMenuEntry* apEntry)
{
	tMenuEntryPosVec vEntryPositions;
	for(size_t i=0;i<mvSubActions.size();++i)
	{
		const cSubActionWrapper& subAction = mvSubActions[i];
		switch(subAction.mType)
		{
		case eInputDeviceType_Keyboard:
			if(apEntry->HasKey((eKey)subAction.mlInputId))
				vEntryPositions.push_back((eInputMenuEntryPos)i);
			break;
		case eInputDeviceType_Mouse:
			if(apEntry->HasMouseButton((eMouseButton)subAction.mlInputId))
				vEntryPositions.push_back((eInputMenuEntryPos)i);
			break;
		case eInputDeviceType_Gamepad:
			switch(subAction.mGamepadInputType)
			{
			case eGamepadInputType_Button:
				if(apEntry->HasGamepadButton((eGamepadButton)subAction.mlInputId))
					vEntryPositions.push_back((eInputMenuEntryPos)i);
				break;
			case eGamepadInputType_Hat:
				if(apEntry->HasGamepadHatState((eGamepadHat)subAction.mlInputId, eGamepadHatState((int)subAction.mfInputValue)))
					vEntryPositions.push_back((eInputMenuEntryPos)i);
				break;
			case eGamepadInputType_Axis:
				if(apEntry->HasGamepadAxis((eGamepadAxis)subAction.mlInputId, eGamepadAxisRange((int)subAction.mfInputValue)))
					vEntryPositions.push_back((eInputMenuEntryPos)i);
				break;
			}
			break;
		}
	}

	return vEntryPositions;
}

//-----------------------------------------------------------------------

void cLuxInputMenuEntry::SetColor(eInputMenuEntryPos aPos, const cColor& aCol)
{
	if(aPos==eInputMenuEntryPos_LastEnum)
		return;

	mvInputColors[aPos] = aCol;
	mvLKeyInputs[aPos]->SetDefaultFontColor(aCol);
	//mvLKeyInputs[aPos]->SetBackGroundColor(aCol);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

tString cLuxInputMenuEntry::GetStringFromSubAction(const cSubActionWrapper& aSubAction)
{
    cInput* pInput = gpBase->mpEngine->GetInput();
	switch(aSubAction.mType)
	{
	case eInputDeviceType_Keyboard:
		return pInput->GetKeyboard()->KeyToString((eKey)aSubAction.mlInputId);
	case eInputDeviceType_Mouse:
		return pInput->GetMouse()->ButtonToString((eMouseButton)aSubAction.mlInputId);
	case eInputDeviceType_Gamepad:
		return GetStringFromGamepadSubAction(aSubAction);
	}

	return "None";
}

tString cLuxInputMenuEntry::GetStringFromGamepadSubAction(const cSubActionWrapper& aSubAction)
{
	switch(aSubAction.mGamepadInputType)
	{
	case eGamepadInputType_Button:
		return iGamepad::ButtonToString((eGamepadButton)aSubAction.mlInputId);
	case eGamepadInputType_Hat:
		{
			int lState = (int)aSubAction.mfInputValue;
			return iGamepad::HatToString((eGamepadHat)aSubAction.mlInputId) + "." + iGamepad::HatStateToString((eGamepadHatState)lState);
		}
	case eGamepadInputType_Axis:
		{
			int lRange = (int)aSubAction.mfInputValue;
			return iGamepad::AxisToString((eGamepadAxis)aSubAction.mlInputId) + "." + iGamepad::AxisRangeToString((eGamepadAxisRange)lRange);
		}
	}

	return "None";
}

//-----------------------------------------------------------------------

cSubActionWrapper cLuxInputMenuEntry::GetSubActionWrapperFromSubAction(iSubAction* apSubAction)
{
	return GetSubActionWrapperFromString(apSubAction->GetInputType()+"."+apSubAction->GetInputName(),-1);
}

cSubActionWrapper cLuxInputMenuEntry::GetSubActionWrapperFromLuxInput(cLuxInput* apInput)
{
	return GetSubActionWrapperFromString(apInput->msInputType, apInput->mlValue);
}

cSubActionWrapper cLuxInputMenuEntry::GetSubActionWrapperFromString(const tString& asX, int alValue)
{
	tString sSep = ".";
	tStringVec vInputParts;
	cString::GetStringVec(asX, vInputParts, &sSep);

	tString sLowerCaseDev = cString::ToLowerCase(vInputParts[0]);

	if(sLowerCaseDev=="keyboard")
		return GetSubActionWrapperFromStringVecKeyboard(vInputParts, alValue);
	else if(sLowerCaseDev=="mousebutton" || sLowerCaseDev=="mouse")
		return GetSubActionWrapperFromStringVecMouse(vInputParts, alValue);
	else if(cString::GetFirstStringPos(sLowerCaseDev, "gamepad")!=-1)
		return GetSubActionWrapperFromStringVecGamepad(vInputParts, alValue);

	return cSubActionWrapper(eInputDeviceType_LastEnum, -1);

}

cSubActionWrapper cLuxInputMenuEntry::GetSubActionWrapperFromStringVecKeyboard(const tStringVec& avX, int alValue)
{
	iKeyboard* pKB = gpBase->mpEngine->GetInput()->GetKeyboard();
	
	eKey key;
	if(alValue!=-1)
		key = (eKey)alValue;
	else
		key = pKB->StringToKey(avX[1]);
	
	return cSubActionWrapper(eInputDeviceType_Keyboard, key);
}

cSubActionWrapper cLuxInputMenuEntry::GetSubActionWrapperFromStringVecMouse(const tStringVec& avX, int alValue)
{
	iMouse* pMouse = gpBase->mpEngine->GetInput()->GetMouse();

	eMouseButton button;
	if(alValue!=-1)
		button = (eMouseButton)alValue;
	else
		button = pMouse->StringToButton(avX[1]);
	
	return cSubActionWrapper(eInputDeviceType_Mouse, button);
}

cSubActionWrapper cLuxInputMenuEntry::GetSubActionWrapperFromStringVecGamepad(const tStringVec& avInputParts, int alValue)
{
	/*
	tString sInputName = apSubAction->GetInputName();
	tString sSep = ".";
	tStringVec vGamepadInputVec;
	cString::GetStringVec(sInputName, vGamepadInputVec, &sSep);
	*/

	//if(alValue==-1 && avInputParts.size()==3 || alValue!=-1 && avInputParts.size()==2)
	if(avInputParts[0]=="GamepadButton")
	{
		eGamepadButton button;
		if(alValue!=-1)
			button = (eGamepadButton)alValue;
		else
			button = iGamepad::StringToButton(avInputParts[1]);

		return cSubActionWrapper(eGamepadInputType_Button, button, 1.0f);
	}
	else
	{
		eGamepadHat hat = iGamepad::StringToHat(avInputParts[1]);
		eGamepadAxis axis = iGamepad::StringToAxis(avInputParts[1]);

		if(hat!=eGamepadHat_LastEnum)
		{
			eGamepadHatState state;
			if(alValue!=-1)
				state = (eGamepadHatState)alValue;
			else
				state = iGamepad::StringToHatState(avInputParts[2]);

			return cSubActionWrapper(eGamepadInputType_Hat, hat, (float)state);
		}
		else if(axis!=eGamepadAxis_LastEnum)
		{
			eGamepadAxisRange range;
			if(alValue!=-1)
				range = (eGamepadAxisRange)alValue;
			else
				range = iGamepad::StringToAxisRange(avInputParts[2]);

			return cSubActionWrapper(eGamepadInputType_Axis, axis, (float)range);
		}
	}


	return cSubActionWrapper(eInputDeviceType_Gamepad, -1);
}


//-----------------------------------------------------------------------

eInputDeviceType cLuxInputMenuEntry::GetInputDeviceTypeFromString(const tString& asX)
{
	tString sLowerCaseDev = cString::ToLowerCase(asX);
	if(sLowerCaseDev=="keyboard")
		return eInputDeviceType_Keyboard;
	else if(sLowerCaseDev=="mousebutton" || sLowerCaseDev=="mouse")
		return eInputDeviceType_Mouse;
	else if(cString::GetFirstStringPos(sLowerCaseDev, "gamepad")!=-1)
		return eInputDeviceType_Gamepad;

	return eInputDeviceType_LastEnum;
}

//-----------------------------------------------------------------------

eInputMenuEntryPos cLuxInputMenuEntry::GetPosFromInput(iWidget* apWidget)
{
	if(apWidget==NULL) return eInputMenuEntryPos_LastEnum;

	return (eInputMenuEntryPos)apWidget->GetUserValue();
}

//-----------------------------------------------------------------------

void cLuxInputMenuEntry::SaveSubAction(eInputMenuEntryPos aPos, eInputDeviceType aType, int alInputId)
{
	if(aPos==eInputMenuEntryPos_LastEnum)
		return;

	switch(aType)
	{
	case eInputDeviceType_Keyboard:
		SetInputKeypress(aPos, (eKey)alInputId);
		break;
	case eInputDeviceType_Mouse:
		SetInputMouseButton(aPos, (eMouseButton)alInputId);
		break;
	}

	UpdateEntry();
	SetWaiting(false, aPos);
}

void cLuxInputMenuEntry::SaveGamepadSubAction(eInputMenuEntryPos aPos, eGamepadInputType aType, int alInputId, float afValue)
{
	if(aPos==eInputMenuEntryPos_LastEnum)
		return;

	switch(aType)
	{
	case eGamepadInputType_Button:
		SetInputGamepadButton(aPos, (eGamepadButton)alInputId);
		break;
	case eGamepadInputType_Hat:
		{
			int lState = (int)afValue;
			SetInputGamepadHat(aPos, (eGamepadHat)alInputId, (eGamepadHatState)lState);
			break;
		}
	case eGamepadInputType_Axis:
		{
			SetInputGamepadAxis(aPos, (eGamepadAxis)alInputId, iGamepad::GetAxisRangeFromValue(afValue));
			break;
		}
	}

	UpdateEntry();
	SetWaiting(false, aPos);
}

//-----------------------------------------------------------------------

void cLuxInputMenuEntry::SetWaiting(bool abX, eInputMenuEntryPos aPos)
{
	if(aPos==eInputMenuEntryPos_LastEnum)
		return;

	cWidgetLabel* pWaitingInput = abX ? mvLKeyInputs[aPos]: NULL;
	
	mpWindow->SetWaitingInput(pWaitingInput);

	if(pWaitingInput)
		pWaitingInput->SetText(_W("..."));
}

bool cLuxInputMenuEntry::IsWaiting()
{
	if(GetPosFromInput(mpWindow->GetWaitingInput())!=eInputMenuEntryPos_LastEnum)
		return true;

	return false;
}

//-----------------------------------------------------------------------

bool cLuxInputMenuEntry::InputEntryMouseOver(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(apWidget->GetSet()->PopUpIsActive())
		return true;

	cColor color = apWidget->GetMouseIsOver()? cColor(1,1,0,1) : mvInputColors[GetPosFromInput(apWidget)];

	cWidgetLabel* pLabel = (cWidgetLabel*)apWidget;
	pLabel->SetDefaultFontColor(color);

	//This worked when bg was visible
	//pLabel->SetBackGroundColor(color);

	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxInputMenuEntry, InputEntryMouseOver);

//-----------------------------------------------------------------------

bool cLuxInputMenuEntry::InputEntryMouseEnter(iWidget* apWidget, const cGuiMessageData& aData)
{
	cLuxInputMenuEntry* pEntry = static_cast<cLuxInputMenuEntry*>(apWidget->GetUserData());
	mpWindow->SetCurrentToolTipEntry(this);

	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxInputMenuEntry, InputEntryMouseEnter);


//-----------------------------------------------------------------------

bool cLuxInputMenuEntry::InputEntryMouseLeave(iWidget* apWidget, const cGuiMessageData& aData)
{
	cLuxInputMenuEntry* pEntry = static_cast<cLuxInputMenuEntry*>(apWidget->GetUserData());

	if(pEntry==mpWindow->GetCurrentToolTipEntry())
		mpWindow->SetCurrentToolTipEntry(NULL);
	
	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxInputMenuEntry, InputEntryMouseLeave);


//-----------------------------------------------------------------------

/*
bool cLuxInputMenuEntry::InputEntryMouseMove(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(apWidget->GetMouseIsOver()==false)
		return true;

	cInput* pInput = gpBase->mpEngine->GetInput();
	cAction* pAction = pInput->GetAction(mlActionId);
	mpWindow->SetCurrentToolTipText(kTranslate("Actions", pAction->GetName() + "Tooltip"));

	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxInputMenuEntry, InputEntryMouseMove);
*/

//-----------------------------------------------------------------------

bool cLuxInputMenuEntry::InputEntryChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	mbChanged = true;
	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxInputMenuEntry, InputEntryChange);

//-----------------------------------------------------------------------

bool cLuxInputMenuEntry::InputEntryClick(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mpWindow->GetWaitingInput()==NULL)
	{
		if(aData.mlVal==eGuiMouseButton_Left)
		{
			SetWaiting(true, GetPosFromInput(apWidget));
		}
	}
	else
	{
		SaveSubAction(GetPosFromInput(apWidget),
					  eInputDeviceType_Mouse, 
					  gpBase->mpEngine->GetGui()->TranslateGuiMouseToMouse((eGuiMouseButton)aData.mlVal));
	}
	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxInputMenuEntry, InputEntryClick);

//-----------------------------------------------------------------------

bool cLuxInputMenuEntry::InputEntryKeyPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(IsWaiting())
	{
		if(aData.mKeyPress.mKey!=eKey_Escape)
		{
			SaveSubAction(GetPosFromInput(apWidget),
					  eInputDeviceType_Keyboard,
					  aData.mKeyPress.mKey);
		}
		else
		{
			UpdateEntry();
			SetWaiting(false, GetPosFromInput(apWidget));

			mpWindow->CancelInputSetting();
		}
	}

	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxInputMenuEntry, InputEntryKeyPress);

//-----------------------------------------------------------------------

bool cLuxInputMenuEntry::InputEntryUIArrowPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mbGamepadButtonPress)
	{
		mbGamepadButtonPress = false;
		return true;
	}

	return false; 
}
kGuiCallbackDeclaredFuncEnd(cLuxInputMenuEntry, InputEntryUIArrowPress);

bool cLuxInputMenuEntry::InputEntryUIButtonPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(aData.mlVal!=eUIButton_Primary)
		return false;

	if(mbGamepadButtonPress)
	{
		mbGamepadButtonPress = false;
		return true;
	}

	if(mpWindow->GetWaitingInput()==NULL)
	{
		if(aData.mlVal==eUIButton_Primary)
		{
			SetWaiting(true, eInputMenuEntryPos_Secondary);
		}
	}
	
	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxInputMenuEntry, InputEntryUIButtonPress);

//-----------------------------------------------------------------------


bool cLuxInputMenuEntry::InputEntryGamepadButtonPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(IsWaiting())
	{
		if(aData.mGamepadInputData.mfInputValue==0.0f) return false;
#if USE_SDL2
		if(aData.mGamepadInputData.mInputType == eGamepadInputType_Button && aData.mGamepadInputData.mlInputId == eGamepadButton_B)
#else
		if(aData.mGamepadInputData.mInputType == eGamepadInputType_Button && aData.mGamepadInputData.mlInputId == eGamepadButton_1)
#endif
		{
			//////////
			// Cancel when pressing B
			UpdateEntry();
			SetWaiting(false, GetPosFromInput(apWidget));

			mpWindow->CancelInputSetting();

			return true;
		}

		mbGamepadButtonPress = true;

		
		SaveGamepadSubAction(GetPosFromInput(apWidget), 
								aData.mGamepadInputData.mInputType, 
								aData.mGamepadInputData.mlInputId,
								aData.mGamepadInputData.mfInputValue);
	}

	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxInputMenuEntry, InputEntryGamepadButtonPress);

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxMainMenu_KeyConfig::cLuxMainMenu_KeyConfig(cGuiSet *apGuiSet, cGuiSkin *apGuiSkin) : iLuxMainMenuWindow(apGuiSet, apGuiSkin)
{
	mvWindowSize = cVector2f(620,460);
	mpWaitingInput = NULL;

	mpCurrentTipEntry = NULL;
	mbTipFadeOut = false;
	mbTipFadeRestore = false;
}

//-----------------------------------------------------------------------

cLuxMainMenu_KeyConfig::~cLuxMainMenu_KeyConfig()
{
	for(size_t i=0; i<mvInputs.size(); ++i)
		STLDeleteAll(mvInputs[i]);
	mvInputs.clear();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMainMenu_KeyConfig::CreateGui()
{
	mbSettingInitialValues = false;

	//////////////////////////
	//Window
	mpWindow = mpGuiSet->CreateWidgetWindow(eWidgetWindowButtonFlag_None,cVector3f(0,0,5),mvWindowSize,kTranslate("KeyConfig","KeyConfigTitle"));
	mpWindow->AddCallback(eGuiMessage_OnUpdate, this, kGuiCallback(WindowOnUpdate));

	float fHeaderSize = 70;
	float fBorderSize = 15;
	float fHeaderFontMul = 1.25f;
	mvEntrySize = cVector2f(140.0f, 16.0f);
	mfEntrySep = 10.0f;
	mvHeaderPositions.x = 240.0f;
	mvHeaderPositions.y = mvHeaderPositions.x +mvEntrySize.x+mfEntrySep;

	//////////////////////////
	//Category selector
	cVector3f vPos = cVector3f(fBorderSize*2,fHeaderSize+fBorderSize,0.1f);
	cWidgetDummy* pGroup = mpGuiSet->CreateWidgetDummy(vPos, mpWindow);
	cWidgetLabel* pLabel = mpGuiSet->CreateWidgetLabel(0, -1, kTranslate("KeyConfig","Category"), pGroup);
	pLabel->SetDefaultFontSize(pLabel->GetDefaultFontSize()*fHeaderFontMul);
	pLabel->SetAutogenerateSize(true);

	mpCBCategory = mpGuiSet->CreateWidgetComboBox(cVector3f(pLabel->GetSize().x + 10, 0 ,0), cVector2f(175,25), _W(""), pGroup);
	mpCBCategory->AddItem(kTranslate("KeyConfig","Movement"));
	mpCBCategory->AddItem(kTranslate("KeyConfig","Actions"));
	mpCBCategory->AddItem(kTranslate("KeyConfig","Misc"));
	mpCBCategory->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(CategoryChange));
	mpCBCategory->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(CategorySelector_OnUIButtonPress));
	mpCBCategory->SetGlobalUIInputListener(true);

	/////////////////////////////////////////////////////
	// Input panes
	vPos = pGroup->GetLocalPosition() + cVector3f(fBorderSize, pGroup->GetSize().y + 5.0f, 0);

	pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("KeyConfig", "HeaderAction"), mpWindow);
	pLabel->SetDefaultFontSize(pLabel->GetDefaultFontSize()*fHeaderFontMul);

	pLabel = mpGuiSet->CreateWidgetLabel(vPos + cVector3f(mvHeaderPositions.x,0,0), mvEntrySize, kTranslate("KeyConfig", "HeaderPrimary"), mpWindow);
	pLabel->SetDefaultFontSize(pLabel->GetDefaultFontSize()*fHeaderFontMul);
	pLabel->SetTextAlign(eFontAlign_Center);

	pLabel = mpGuiSet->CreateWidgetLabel(vPos + cVector3f(mvHeaderPositions.y,0,0), mvEntrySize, kTranslate("KeyConfig", "HeaderSecondary"), mpWindow);
	pLabel->SetDefaultFontSize(pLabel->GetDefaultFontSize()*fHeaderFontMul);
	pLabel->SetTextAlign(eFontAlign_Center);

	vPos.y += pLabel->GetSize().y+10;

	/////////////////////////
	//Key groups
	mvDKeyGroups.resize(eLuxActionCategory_LastEnum);
	for(size_t i=0; i<mvInputs.size(); ++i)
		STLDeleteAll(mvInputs[i]);
	mvInputs.clear();

	//For each category
	for(int cat = eLuxActionCategory_Movement; cat!= eLuxActionCategory_LastEnum; ++cat)
	{
		tInputEntryVec vInputs;

		cWidgetDummy* pGroup = mpGuiSet->CreateWidgetDummy(vPos, mpWindow);
		tLuxActionVec vActions = gpBase->mpInputHandler->GetActionsByCategory((eLuxActionCategory)cat);

		cVector3f vPosInGroup = cVector3f(0,0,0.1f);

		// Iterate every action in category
		for(size_t i=0;i<vActions.size();++i)
		{
			cLuxAction* pLuxAction = vActions[i];
			if(pLuxAction->mbConfigurable==false)
				continue;

			cLuxInputMenuEntry* pEntry = CreateInputEntry(pLuxAction, pGroup, vPosInGroup);
			vInputs.push_back(pEntry);

			vPosInGroup.y += pEntry->mvLKeyInputs[0]->GetSize().y + 10;
		}

		// Set up focus navigation
		for(size_t i=0;i<vInputs.size();++i)
		{
			cLuxInputMenuEntry* pEntry = vInputs[i];
			cLuxInputMenuEntry* pPrevEntry = NULL;
			cLuxInputMenuEntry* pNextEntry = NULL;

			int lPrev = i-1;
			int lNext = i+1;

			if(lPrev>=0)
				pPrevEntry = vInputs[lPrev];
			if(lNext<(int)vInputs.size())
				pNextEntry = vInputs[lNext];

			if(pPrevEntry)
			{
				pEntry->mpLAction->SetFocusNavigation(eUIArrow_Up, pPrevEntry->mpLAction);
			}

			if(pNextEntry)
			{
				pEntry->mpLAction->SetFocusNavigation(eUIArrow_Down, pNextEntry->mpLAction);
			}
		}

		mvDKeyGroups[cat] = pGroup;
		mvInputs.push_back(vInputs);
	}

	//////////////////////////
	//Buttons
	float fButtonWidth = 80;
	float fButtonSepp = 3;
	vPos.x = 30;
	vPos.y = mpWindow->GetSize().y - 25 - 10;

	//Tip Label
	mpLTip = mpGuiSet->CreateWidgetLabel(vPos + cVector3f(10,0,0), cVector2f(400,30), _W(""), mpWindow);
	mpLTip->SetDefaultFontColor(cColor(1,1));
	mpLTip->SetWordWrap(true);
	mpLTip->SetClipActive(true);
	mpLTip->SetScrollSpeedMul(4.0f);
	mpLTip->SetDrawBackGround(true);
	mpLTip->SetBackGroundColor(cColor(0, 0.5f));

	//Set default keys
	cWidgetButton* pButton = mpGuiSet->CreateWidgetButton(vPos + cVector3f(10,-35,0), cVector2f(fButtonWidth,25), kTranslate("KeyConfig","SetDefault"), mpWindow);
	pButton->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(PressSetDefault));
	float fSetDefaultsWidth = pButton->GetDefaultFontType()->GetLength(pButton->GetDefaultFontSize(), pButton->GetText().c_str());
	pButton->SetSize(cVector2f(fSetDefaultsWidth+20, pButton->GetSize().y));
	mpBDefaultKeys = pButton;


	//Save changes
	vPos.x = mpWindow->GetSize().x - fButtonWidth*2-fButtonSepp-5;
	mpBOK = mpGuiSet->CreateWidgetButton(vPos,cVector2f(fButtonWidth,30),kTranslate("Global","OK"),mpWindow);
	mpBOK->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(PressOK));

	//Cancel
	vPos.x += fButtonWidth + fButtonSepp;
	mpBCancel = mpGuiSet->CreateWidgetButton(vPos,cVector2f(fButtonWidth,30),kTranslate("Global","Cancel"),mpWindow);
	mpBCancel->AddCallback(eGuiMessage_ButtonPressed,this, kGuiCallback(PressCancel));
	mpBCancel->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(UIPressCancel));
	mpBCancel->SetGlobalUIInputListener(true);

	mpCBCategory->SetFocusNavigation(eUIArrow_Down, mvInputs[0].front()->mpLAction);

	////////////////////////////////////////////////////////
	// Last focus nav settings
	// Set focus nav linking to DefaultKeys button
	for(size_t cat=0; cat<mvInputs.size(); ++cat)
	{
		iWidget* pWidget = mvInputs[cat].back()->mpLAction;

		pWidget->SetFocusNavigation(eUIArrow_Down, mpBDefaultKeys);

		pWidget = mvInputs[cat].front()->mpLAction;

		pWidget->SetFocusNavigation(eUIArrow_Up, mpCBCategory);
	}
	
	mpBDefaultKeys->SetFocusNavigation(eUIArrow_Up, mpCBCategory);
	mpBDefaultKeys->SetFocusNavigation(eUIArrow_Down, mpBOK);
	mpBOK->SetFocusNavigation(eUIArrow_Up, mpBDefaultKeys);
	mpBOK->SetFocusNavigation(eUIArrow_Right, mpBCancel);
	mpBCancel->SetFocusNavigation(eUIArrow_Up, mpBDefaultKeys);
	mpBCancel->SetFocusNavigation(eUIArrow_Left, mpBOK);
}

//-----------------------------------------------------------------------

void cLuxMainMenu_KeyConfig::ExitPressed()
{
    if(mpGuiSet->PopUpIsActive()) return;
	if(mbInputSettingCancelled)
	{
		mbInputSettingCancelled = false;
		return;
	}

	gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_Options);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMainMenu_KeyConfig::OnSetActive(bool abX)
{
	if(abX)
	{
		mpGuiSet->PushAttentionWidget();
		mpGuiSet->PushFocusedWidget();
		mpGuiSet->PushDefaultFocusNavWidget();

		SetInitialValues();

		mpCBCategory->SetSelectedItem(-1);
		mpCBCategory->SetSelectedItem(0);
	}
	else
	{
		//mpGuiSet->SetDrawMouse(true);
#ifdef USE_GAMEPAD
		if(gpBase->mpInputHandler->IsGamepadPresent() == false)
			mpGuiSet->SetDrawMouse(true);
#endif
	}

	mpGuiSet->SetAttentionWidget(abX? mpWindow : NULL);
}

//-----------------------------------------------------------------------

cLuxInputMenuEntry* cLuxMainMenu_KeyConfig::CreateInputEntry(cLuxAction* apAction, iWidget* apParent, const cVector3f& avPos)
{
	cLuxInputMenuEntry* pEntry = hplNew(cLuxInputMenuEntry,(this, apAction, apParent, avPos));

	return pEntry;
}

//-----------------------------------------------------------------------

void cLuxMainMenu_KeyConfig::SetCurrentToolTipEntry(cLuxInputMenuEntry* apEntry)
{
	if(apEntry==mpCurrentTipEntry)
	{
		return;
	}

	mpCurrentTipEntry = apEntry;

	tWString sTip;

	if(mpCurrentTipEntry)
	{
		cInput* pInput = gpBase->mpEngine->GetInput();
		cAction* pAction = pInput->GetAction(apEntry->mlActionId);
		sTip = kTranslate("Actions", pAction->GetName() + "Tooltip");

		mpLTip->SetText(sTip);
	}

	if(sTip.empty())
	{
		mbTipFadeOut = true;
		mbTipFadeRestore = false;
	}
	else
	{
		mbTipFadeOut = false;
		mbTipFadeRestore = true;
	}

	//mpLTip->SetText(sTip);
}

//-----------------------------------------------------------------------

void cLuxMainMenu_KeyConfig::ApplyChanges()
{
	cInput* pInput = gpBase->mpEngine->GetInput();
	for(size_t i=0; i<mvInputs.size(); ++i)
	{
		tInputEntryVecIt it = mvInputs[i].begin();
		tString sSeparator = ",";
		for(;it!=mvInputs[i].end();++it)
		{
			cLuxInputMenuEntry* pEntry = *it;
			pEntry->UpdateAction();
		}
	}
}

//-----------------------------------------------------------------------

void cLuxMainMenu_KeyConfig::SetDefaultValues()
{
	mbSettingInitialValues = true;

	for(size_t i=0; i<mvInputs.size(); ++i)
	{
		
		tInputEntryVecIt it = mvInputs[i].begin();
		for(;it!=mvInputs[i].end();++it)
		{
			cLuxInputMenuEntry* pEntry = *it;
			pEntry->RetrieveDefaultValue();
			pEntry->UpdateEntry();
		}

	}

	mbSettingInitialValues = false;

	CheckEmptyAndDuplicateInputs();
}

//-----------------------------------------------------------------------

void cLuxMainMenu_KeyConfig::SetInitialValues()
{
	mbSettingInitialValues = true;

	for(size_t i=0; i<mvInputs.size(); ++i)
	{
		tInputEntryVecIt it = mvInputs[i].begin();
		for(;it!=mvInputs[i].end();++it)
		{
			cLuxInputMenuEntry* pEntry = *it;
			pEntry->RetrieveInitialValue();
			pEntry->UpdateEntry();
		}
	}

	mbSettingInitialValues = false;

	CheckEmptyAndDuplicateInputs();
}

//-----------------------------------------------------------------------

void cLuxMainMenu_KeyConfig::SetWaitingInput(iWidget *apWidget)
{
	if(apWidget!=NULL && mpWaitingInput)
		return;

	mpWaitingInput = apWidget;
	mbWasWaitingOnInput = true;

	if(apWidget)
	{
		mpGuiSet->PushAttentionWidget();
		mpGuiSet->PushFocusedWidget();

		mpGuiSet->SetAttentionWidget(apWidget);
	}
	else
	{
		mpGuiSet->PopAttentionWidget();
		mpGuiSet->PopFocusedWidget();
	}
	
	mpGuiSet->SetMouseMovementEnabled(apWidget==NULL);
}

//-----------------------------------------------------------------------

void cLuxMainMenu_KeyConfig::CheckEmptyAndDuplicateInputs()
{
	if(mbSettingInitialValues)
		return;

	//////////////////////////////////////////
	// Brute force marking... works for now
	cColor red = cColor(1,0,0,1);
	cColor white = cColor(1,1);

	
	// Reset marking
	mbInvalidInputs = false;
	for(size_t cat=0; cat<mvInputs.size(); ++cat)
	{
		const tInputEntryVec& vInputs = mvInputs[cat];
		for(size_t i=0;i<vInputs.size();++i)
		{
			cLuxInputMenuEntry* pEntry = vInputs[i];
			pEntry->SetColor(eInputMenuEntryPos_Primary, white);
			pEntry->SetColor(eInputMenuEntryPos_Secondary, white);
		}
	}

	// Update
	for(size_t cat=0; cat<mvInputs.size(); ++cat)
	{
		const tInputEntryVec& vInputs = mvInputs[cat];
		for(size_t i=0;i<vInputs.size();++i)
		{
			cLuxInputMenuEntry* pEntry1 = vInputs[i];
		
			for(size_t j=i+1;j<vInputs.size();++j)
			{
				cLuxInputMenuEntry* pEntry2 = vInputs[j];
				if(pEntry2->HasSubActions()==false)
				{
					mbInvalidInputs = true;
					pEntry2->SetColor(eInputMenuEntryPos_Primary, red);
					pEntry2->SetColor(eInputMenuEntryPos_Secondary, red);
					continue;
				}

				tMenuEntryPosVec vPositions = pEntry1->GetPosSharingSubAction(pEntry2);
				if(vPositions.empty()==false)
				{
					mbInvalidInputs = true;

					for(size_t k=0;k<vPositions.size();++k)
						pEntry1->SetColor(vPositions[k], red);

					vPositions = pEntry2->GetPosSharingSubAction(pEntry1);
					for(size_t k=0;k<vPositions.size();++k)
						pEntry2->SetColor(vPositions[k], red);
				
				}
			}
		}
	}
}

//-----------------------------------------------------------------------

bool cLuxMainMenu_KeyConfig::WindowOnUpdate(iWidget* apWidget, const cGuiMessageData& aData)
{
	//mpGuiSet->SetDrawMouse(mpWaitingInput==NULL);
	//gpBase->SetDrawOnLiveCursor(mpWaitingInput==NULL);
	
	if(mpWaitingInput == NULL)
		mbWasWaitingOnInput = false;

	///////////////////////////////////////////////////
	// If there is a popup active, dont update tips
	if(mpGuiSet->PopUpIsActive())
		return true;

	////////////////////////////////////////////////////
	// Update Tip label fade
	cColor labelCol = mpLTip->GetDefaultFontColor();
	if(mbTipFadeOut)
	{
		labelCol.a -= aData.mfVal*0.8f;
		if(labelCol.a<=0.0f)
		{
			labelCol.a = 0.0f;
			mbTipFadeOut=false;
		}

		mpLTip->SetDefaultFontColor(labelCol);		
	}

	if(mbTipFadeRestore)
	{
		labelCol.a += aData.mfVal*3;
		if(labelCol.a>=1.0f)
		{
			labelCol.a = 1.0f;
			mbTipFadeRestore = false;
		}

		mpLTip->SetDefaultFontColor(labelCol);
	}

	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_KeyConfig, WindowOnUpdate);

//-----------------------------------------------------------------------

bool cLuxMainMenu_KeyConfig::CategoryChange(iWidget* apWidget, const cGuiMessageData& aData)
{
	for(int i=eLuxActionCategory_Movement;i!=eLuxActionCategory_LastEnum;++i)
	{
		bool bActive = (aData.mlVal==i-1);
        mvDKeyGroups[i]->SetVisible(bActive);
		mvDKeyGroups[i]->SetEnabled(bActive);

		if(bActive)
		{
			iWidget* pFocus = mvInputs[i-1].front()->mpLAction;
			iWidget* pTailWidget = mvInputs[i-1].back()->mpLAction;

			mpGuiSet->SetDefaultFocusNavWidget(pFocus);
			mpGuiSet->SetFocusedWidget(pFocus);

			mpCBCategory->SetFocusNavigation(eUIArrow_Down, pFocus);
			mpBDefaultKeys->SetFocusNavigation(eUIArrow_Up, pTailWidget);
		}
	}
	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_KeyConfig, CategoryChange);

//-----------------------------------------------------------------------

bool cLuxMainMenu_KeyConfig::CategorySelector_OnUIButtonPress(iWidget* apWidget, const cGuiMessageData& aData)
{
	iWidget* pWidget = GetWaitingInput();

	if(mbWasWaitingOnInput)
		return false;

	int lCat = mpCBCategory->GetSelectedItem();
	
	if(aData.mlVal==eUIButton_PrevPage)
	{
		int lPrevCat = lCat-1;
		if(lPrevCat>=0)
			lCat = lPrevCat;
	}
	else if(aData.mlVal==eUIButton_NextPage)
	{
		int lNextCat = lCat+1;
		if(lNextCat<mpCBCategory->GetItemNum())
			lCat = lNextCat;
	}
	else
		return false;

	mpCBCategory->SetSelectedItem(lCat);

	return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_KeyConfig, CategorySelector_OnUIButtonPress);

//-----------------------------------------------------------------------

bool cLuxMainMenu_KeyConfig::PressSetDefault(iWidget* apWidget, const cGuiMessageData& aData)
{
	mpGuiSet->CreatePopUpMessageBox(kTranslate("Global", "Warning"), 
											kTranslate("KeyConfig", "LoadDefaultsMessage"), 
											kTranslate("Global", "OK"), 
											kTranslate("Global", "Cancel"), 
											this, kGuiCallback(LoadDefaultsCallback));
	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_KeyConfig, PressSetDefault);

//-----------------------------------------------------------------------

bool cLuxMainMenu_KeyConfig::PressOK(iWidget* apWidget, const cGuiMessageData& aData)
{
	if(mbInvalidInputs)
	{
		mpGuiSet->CreatePopUpMessageBox(kTranslate("Global", "Warning"), 
										kTranslate("KeyConfig", "InvalidInputMessage"), 
										kTranslate("Global", "OK"), 
										kTranslate("Global", "Cancel"), 
										this, kGuiCallback(ExitCallback));
	}
	else
	{
		ExitCallback(NULL, cGuiMessageData(0));
	}

	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_KeyConfig, PressOK);

//-----------------------------------------------------------------------

bool cLuxMainMenu_KeyConfig::PressCancel(iWidget* apWidget, const cGuiMessageData& aData)
{
	gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_Options);

	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_KeyConfig, PressCancel);

//-----------------------------------------------------------------------

bool cLuxMainMenu_KeyConfig::UIPressCancel(iWidget* apWidget, const cGuiMessageData& aData)
{
	///////////////
	// Check if this windows is or has been waiting for input his frame
	if(mbWasWaitingOnInput)
		return false;

	if(aData.mlVal == eUIButton_Secondary)
	{
		gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_Options);
		return true;
	}

	return false; 
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_KeyConfig, UIPressCancel);

//-----------------------------------------------------------------------

bool cLuxMainMenu_KeyConfig::LoadDefaultsCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	bool bOkPressed = aData.mlVal==0? true : false;
	if(bOkPressed)
		SetDefaultValues();
	
	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_KeyConfig, LoadDefaultsCallback);

//-----------------------------------------------------------------------

bool cLuxMainMenu_KeyConfig::ExitCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
	bool bOkPressed = aData.mlVal==0? true : false;
	if(bOkPressed)
	{
		ApplyChanges();

		gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_Options);
	}

	return true; 
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_KeyConfig, ExitCallback);

//-----------------------------------------------------------------------
