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

#include "LuxInputHandler.h"

#include "LuxPlayer.h"
#include "LuxPlayerState.h"
#include "LuxPlayerHelpers.h"

#include "LuxPreMenu.h"
#include "LuxMainMenu.h"
#include "LuxCredits.h"
#include "LuxDemoEnd.h"

#include "LuxInventory.h"
#include "LuxJournal.h"
#include "LuxMessageHandler.h"
#include "LuxMapHandler.h"
#include "LuxSaveHandler.h"
#include "LuxEffectHandler.h"
#include "LuxConfigHandler.h"
#include "LuxLoadScreenHandler.h"

#include "LuxDebugHandler.h"

//////////////////////////////////////////////////////////////////////////
// ACTION LISTS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

static cLuxAction gvLuxActions[] =
{
	cLuxAction("Exit",eLuxAction_Exit,				false, eLuxActionCategory_System),
	cLuxAction("ExitDirect",eLuxAction_ExitDirect,	false, eLuxActionCategory_System),
	cLuxAction("ScreenShot",eLuxAction_ScreenShot,	false, eLuxActionCategory_System),
	cLuxAction("PrintInfo",eLuxAction_PrintInfo,	false, eLuxActionCategory_System),

	cLuxAction("LeftClick",eLuxAction_LeftClick,	false, eLuxActionCategory_System),
	cLuxAction("MiddleClick",eLuxAction_MiddleClick,false, eLuxActionCategory_System),
	cLuxAction("RightClick",eLuxAction_RightClick,	false, eLuxActionCategory_System),
	cLuxAction("ScrollUp",eLuxAction_ScrollUp,	false, eLuxActionCategory_System),
	cLuxAction("ScrollDown",eLuxAction_ScrollDown,	false, eLuxActionCategory_System),
	cLuxAction("MouseButton6",eLuxAction_MouseButton6Click,	false, eLuxActionCategory_System),
	cLuxAction("MouseButton7",eLuxAction_MouseButton7Click,	false, eLuxActionCategory_System),
	cLuxAction("MouseButton8",eLuxAction_MouseButton8Click,	false, eLuxActionCategory_System),
	cLuxAction("MouseButton9",eLuxAction_MouseButton9Click,	false, eLuxActionCategory_System),

	cLuxAction("UIArrowUp", eLuxAction_UIArrowUp,		false, eLuxActionCategory_System),
	cLuxAction("UIArrowLeft", eLuxAction_UIArrowLeft,		false, eLuxActionCategory_System),
	cLuxAction("UIArrowDown", eLuxAction_UIArrowDown,	false, eLuxActionCategory_System),
	cLuxAction("UIArrowRight", eLuxAction_UIArrowRight,		false, eLuxActionCategory_System),

	cLuxAction("UIPrimary", eLuxAction_UIPrimary,		false, eLuxActionCategory_System),
	cLuxAction("UISecondary", eLuxAction_UISecondary,		false, eLuxActionCategory_System),
	cLuxAction("UIPrevPage", eLuxAction_UIPrevPage,		false, eLuxActionCategory_System),
	cLuxAction("UINextPage", eLuxAction_UINextPage,		false, eLuxActionCategory_System),
	cLuxAction("UIDelete", eLuxAction_UIDelete,		false, eLuxActionCategory_System),
	cLuxAction("UIClear", eLuxAction_UIClear,		false, eLuxActionCategory_System),

	cLuxAction("OpenDebug",eLuxAction_OpenDebug,	false, eLuxActionCategory_System),
	cLuxAction("ReloadMap",eLuxAction_ReloadMap,	false, eLuxActionCategory_System),
	cLuxAction("QuickSave",eLuxAction_QuickSave,	false, eLuxActionCategory_System),
	cLuxAction("QuickLoad",eLuxAction_QuickLoad,	false, eLuxActionCategory_System),
	cLuxAction("FastForward",eLuxAction_FastForward,	false, eLuxActionCategory_System),

	cLuxAction("Inventory",eLuxAction_Inventory,	true, eLuxActionCategory_Misc),
	cLuxAction("Journal",eLuxAction_Journal,		true, eLuxActionCategory_Misc),
	cLuxAction("QuestLog",eLuxAction_QuestLog,		true, eLuxActionCategory_Misc),
	cLuxAction("RecentText", eLuxAction_RecentText, true, eLuxActionCategory_Misc),
	cLuxAction("CrosshairToggle", eLuxAction_CrosshairToggle, true, eLuxActionCategory_Misc),
	
	cLuxAction("Forward",eLuxAction_Forward,	true, eLuxActionCategory_Movement),
	cLuxAction("Backward",eLuxAction_Backward,	true, eLuxActionCategory_Movement),
	cLuxAction("Right",eLuxAction_Right,		true, eLuxActionCategory_Movement),
	cLuxAction("Left",eLuxAction_Left,			true, eLuxActionCategory_Movement),

	cLuxAction("LeanRight",eLuxAction_LeanRight,true, eLuxActionCategory_Movement),
	cLuxAction("LeanLeft",eLuxAction_LeanLeft,	true, eLuxActionCategory_Movement),

#ifdef USE_GAMEPAD
	cLuxAction("Lean",eLuxAction_Lean,	true, eLuxActionCategory_Movement),
	cLuxAction("ZoomOut", eLuxAction_ZoomOut, false, eLuxActionCategory_Action),
	cLuxAction("ZoomIn", eLuxAction_ZoomIn, false, eLuxActionCategory_Action),
#endif

	cLuxAction("Attack",eLuxAction_Attack,		true, eLuxActionCategory_Action),
	cLuxAction("Interact",eLuxAction_Interact,	true, eLuxActionCategory_Action),
	cLuxAction("Ignite",eLuxAction_Ignite,		false, eLuxActionCategory_Action),
	cLuxAction("Rotate",eLuxAction_Rotate,		true, eLuxActionCategory_Action),
	cLuxAction("Holster",eLuxAction_Holster,	false, eLuxActionCategory_Action),
	cLuxAction("Lantern",eLuxAction_Lantern,	true, eLuxActionCategory_Action),

	cLuxAction("Run",eLuxAction_Run,			true, eLuxActionCategory_Movement),
	cLuxAction("Crouch",eLuxAction_Crouch,		true, eLuxActionCategory_Movement),
	cLuxAction("Jump",eLuxAction_Jump,			true, eLuxActionCategory_Movement),


	cLuxAction()
};

//-----------------------------------------------------------------------

// This is a list of all inputs, note that there can be several inputs per action!! (and this is the reason for having two arrays!!)
// Also note though that the current save/load of actions assume that there are no duplicates, and hence DO NOT add duplicates here
//until the key config load is updated!! (Remove this message then).
static cLuxInput gvLuxInputs[] =
{
	cLuxInput("Keyboard", eKey_Escape, eLuxAction_Exit),
	cLuxInput("Keyboard", eKey_F12, eLuxAction_ExitDirect),
	cLuxInput("Keyboard", eKey_F8, eLuxAction_ScreenShot),
	cLuxInput("Keyboard", eKey_P, eLuxAction_PrintInfo),

	cLuxInput("MouseButton", eMouseButton_Left, eLuxAction_LeftClick),
	cLuxInput("MouseButton", eMouseButton_Middle, eLuxAction_MiddleClick),
	cLuxInput("MouseButton", eMouseButton_Right, eLuxAction_RightClick),
	cLuxInput("MouseButton", eMouseButton_WheelUp, eLuxAction_ScrollUp),
	cLuxInput("MouseButton", eMouseButton_WheelDown, eLuxAction_ScrollDown),
	cLuxInput("MouseButton", eMouseButton_Button6, eLuxAction_MouseButton6Click),
	cLuxInput("MouseButton", eMouseButton_Button7, eLuxAction_MouseButton7Click),
	cLuxInput("MouseButton", eMouseButton_Button8, eLuxAction_MouseButton8Click),
	cLuxInput("MouseButton", eMouseButton_Button9, eLuxAction_MouseButton9Click),

#ifdef USE_GAMEPAD
#if USE_SDL2
	cLuxInput("GamepadButton", eGamepadButton_DpadUp, eLuxAction_UIArrowUp),
	cLuxInput("GamepadButton", eGamepadButton_DpadDown, eLuxAction_UIArrowDown),
	cLuxInput("GamepadButton", eGamepadButton_DpadLeft, eLuxAction_UIArrowLeft),
	cLuxInput("GamepadButton", eGamepadButton_DpadRight, eLuxAction_UIArrowRight),

	cLuxInput("GamepadAxis.Axis LeftY", eGamepadAxisRange_Negative, eLuxAction_UIArrowUp),
	cLuxInput("GamepadAxis.Axis LeftX", eGamepadAxisRange_Positive, eLuxAction_UIArrowRight),
	cLuxInput("GamepadAxis.Axis LeftY", eGamepadAxisRange_Positive, eLuxAction_UIArrowDown),
	cLuxInput("GamepadAxis.Axis LeftX", eGamepadAxisRange_Negative, eLuxAction_UIArrowLeft),

	cLuxInput("GamepadButton", eGamepadButton_A, eLuxAction_UIPrimary),
	cLuxInput("GamepadButton", eGamepadButton_B, eLuxAction_UISecondary),
	cLuxInput("GamepadButton", eGamepadButton_LeftShoulder, eLuxAction_UIPrevPage),
	cLuxInput("GamepadButton", eGamepadButton_RightShoulder, eLuxAction_UINextPage),
	cLuxInput("GamepadAxis.Axis LeftTrigger", eGamepadAxisRange_Positive, eLuxAction_UIPrevPage),
	cLuxInput("GamepadAxis.Axis RightTrigger", eGamepadAxisRange_Positive, eLuxAction_UINextPage),

	cLuxInput("GamepadButton", eGamepadButton_X, eLuxAction_UIDelete),
	cLuxInput("GamepadButton", eGamepadButton_Y, eLuxAction_UIClear),
#else
	cLuxInput("GamepadHat.Hat 0", eGamepadHatState_Up, eLuxAction_UIArrowUp),
	cLuxInput("GamepadHat.Hat 0", eGamepadHatState_Down, eLuxAction_UIArrowDown),
	cLuxInput("GamepadHat.Hat 0", eGamepadHatState_Left, eLuxAction_UIArrowLeft),
	cLuxInput("GamepadHat.Hat 0", eGamepadHatState_Right, eLuxAction_UIArrowRight),

	cLuxInput("GamepadAxis.Axis 1", eGamepadAxisRange_Negative, eLuxAction_UIArrowUp),
	cLuxInput("GamepadAxis.Axis 0", eGamepadAxisRange_Positive, eLuxAction_UIArrowRight),
	cLuxInput("GamepadAxis.Axis 1", eGamepadAxisRange_Positive, eLuxAction_UIArrowDown),
	cLuxInput("GamepadAxis.Axis 0", eGamepadAxisRange_Negative, eLuxAction_UIArrowLeft),

	cLuxInput("GamepadButton", eGamepadButton_0, eLuxAction_UIPrimary),
	cLuxInput("GamepadButton", eGamepadButton_1, eLuxAction_UISecondary),
	cLuxInput("GamepadButton", eGamepadButton_4, eLuxAction_UIPrevPage),
	cLuxInput("GamepadButton", eGamepadButton_5, eLuxAction_UINextPage),
	cLuxInput("GamepadAxis.Axis 2", eGamepadAxisRange_Positive, eLuxAction_UIPrevPage),
	cLuxInput("GamepadAxis.Axis 2", eGamepadAxisRange_Negative, eLuxAction_UINextPage),

	cLuxInput("GamepadButton", eGamepadButton_2, eLuxAction_UIDelete),
	cLuxInput("GamepadButton", eGamepadButton_3, eLuxAction_UIClear),
#endif
#endif
	
	cLuxInput("Keyboard", eKey_Return, eLuxAction_UIPrimary),

	cLuxInput("Keyboard", eKey_F1, eLuxAction_OpenDebug),
	cLuxInput("Keyboard", eKey_F2, eLuxAction_ReloadMap),
	cLuxInput("Keyboard", eKey_F4, eLuxAction_QuickSave),
	cLuxInput("Keyboard", eKey_F5, eLuxAction_QuickLoad),
	cLuxInput("Keyboard", eKey_F3, eLuxAction_FastForward),
	
	cLuxInput("Keyboard", eKey_Tab, eLuxAction_Inventory),
	cLuxInput("Keyboard", eKey_J, eLuxAction_Journal),
	cLuxInput("Keyboard", eKey_M, eLuxAction_QuestLog),
	cLuxInput("Keyboard", eKey_N, eLuxAction_RecentText),
	cLuxInput("Keyboard", eKey_X, eLuxAction_CrosshairToggle),

	cLuxInput("Keyboard", eKey_W, eLuxAction_Forward),
	cLuxInput("Keyboard", eKey_S, eLuxAction_Backward),
	cLuxInput("Keyboard", eKey_D, eLuxAction_Right),
	cLuxInput("Keyboard", eKey_A, eLuxAction_Left),

	cLuxInput("Keyboard", eKey_E, eLuxAction_LeanRight),
	cLuxInput("Keyboard", eKey_Q, eLuxAction_LeanLeft),

	cLuxInput("Keyboard", eKey_LeftAlt, eLuxAction_Lean),

	cLuxInput("MouseButton", eMouseButton_Right, eLuxAction_Attack),
	cLuxInput("MouseButton", eMouseButton_Left, eLuxAction_Interact),
	cLuxInput("Keyboard", eKey_C, eLuxAction_Ignite),
	cLuxInput("Keyboard", eKey_R, eLuxAction_Rotate),
	cLuxInput("Keyboard", eKey_H, eLuxAction_Holster),
	cLuxInput("Keyboard", eKey_F, eLuxAction_Lantern),
	
	cLuxInput("Keyboard", eKey_LeftShift, eLuxAction_Run),
	cLuxInput("Keyboard", eKey_LeftCtrl, eLuxAction_Crouch),
	cLuxInput("Keyboard", eKey_Space, eLuxAction_Jump),

	///////////////////////////////////////////////////////////////////////
	// Gamepad layout
	// ----------Buttons---------
	// 0 --> A
	// 1 --> B
	// 2 --> X
	// 3 --> Y
	// 4 --> LB
	// 5 --> RB
	// 6 --> Back
	// 7 --> Start
	// 8 --> LA
	// 9 --> RA
	// ------------Hat-------------
	// Up --> DPAD-Up
	// Down --> DPAD-Down
	// Left --> DPAD-Left
	// Right --> DPAD-Right
	// ------------Axis-------------
	// 0 --> (-) Left, (+) Right
	// 1 --> (-) Forward, (+) Back
	// 2 --> (-) Look-Down, (+) Look-Up
	// 3 --> (-) Look-Left, (+) Look-Right
	// 4 --> (-) LTrigger, (+) RTrigger

#ifdef USE_GAMEPAD
#if USE_SDL2
	cLuxInput("GamepadAxis.Axis LeftY", eGamepadAxisRange_Negative, eLuxAction_Forward),
	cLuxInput("GamepadAxis.Axis LeftX", eGamepadAxisRange_Positive, eLuxAction_Right),
	cLuxInput("GamepadAxis.Axis LeftY", eGamepadAxisRange_Positive, eLuxAction_Backward),
	cLuxInput("GamepadAxis.Axis LeftX", eGamepadAxisRange_Negative, eLuxAction_Left),

	cLuxInput("GamepadButton", eGamepadButton_A, eLuxAction_Jump),
	cLuxInput("GamepadButton", eGamepadButton_B, eLuxAction_Crouch),
	cLuxInput("GamepadButton", eGamepadButton_X, eLuxAction_Lantern),
	cLuxInput("GamepadButton", eGamepadButton_Y, eLuxAction_Journal),
	cLuxInput("GamepadButton", eGamepadButton_DpadRight, eLuxAction_QuestLog),
	cLuxInput("GamepadButton", eGamepadButton_DpadLeft, eLuxAction_RecentText),
	cLuxInput("GamepadButton", eGamepadButton_Back, eLuxAction_Inventory),
	cLuxInput("GamepadButton", eGamepadButton_LeftShoulder, eLuxAction_Attack),
	cLuxInput("GamepadButton", eGamepadButton_RightShoulder, eLuxAction_Interact),
	cLuxInput("GamepadButton", eGamepadButton_Start, eLuxAction_Exit),
	cLuxInput("GamepadButton", eGamepadButton_LeftStick, eLuxAction_CrosshairToggle),
	cLuxInput("GamepadButton", eGamepadButton_RightStick, eLuxAction_Rotate),
	cLuxInput("GamepadAxis.Axis LeftTrigger", eGamepadAxisRange_Positive, eLuxAction_Run),
	cLuxInput("GamepadAxis.Axis RightTrigger", eGamepadAxisRange_Positive, eLuxAction_Lean),

	cLuxInput("GamepadButton", eGamepadButton_DpadUp, eLuxAction_ZoomOut),
	cLuxInput("GamepadButton", eGamepadButton_DpadDown, eLuxAction_ZoomIn),
#else
	cLuxInput("GamepadAxis.Axis 1", eGamepadAxisRange_Negative, eLuxAction_Forward),
	cLuxInput("GamepadAxis.Axis 0", eGamepadAxisRange_Positive, eLuxAction_Right),
	cLuxInput("GamepadAxis.Axis 1", eGamepadAxisRange_Positive, eLuxAction_Backward),
	cLuxInput("GamepadAxis.Axis 0", eGamepadAxisRange_Negative, eLuxAction_Left),

	cLuxInput("GamepadButton", eGamepadButton_0, eLuxAction_Jump),
	cLuxInput("GamepadButton", eGamepadButton_1, eLuxAction_Crouch),
	cLuxInput("GamepadButton", eGamepadButton_2, eLuxAction_Lantern),
	cLuxInput("GamepadButton", eGamepadButton_3, eLuxAction_Journal),
	cLuxInput("GamepadHat.Hat 0", eGamepadHatState_Right, eLuxAction_QuestLog),
	cLuxInput("GamepadHat.Hat 0", eGamepadHatState_Left, eLuxAction_RecentText),
	cLuxInput("GamepadButton", eGamepadButton_6, eLuxAction_Inventory),
	cLuxInput("GamepadButton", eGamepadButton_4, eLuxAction_Attack),
	cLuxInput("GamepadButton", eGamepadButton_5, eLuxAction_Interact),
	cLuxInput("GamepadButton", eGamepadButton_7, eLuxAction_Exit),
	cLuxInput("GamepadButton", eGamepadButton_8, eLuxAction_CrosshairToggle),
	cLuxInput("GamepadButton", eGamepadButton_9, eLuxAction_Rotate),
	cLuxInput("GamepadAxis.Axis 2", eGamepadAxisRange_Positive, eLuxAction_Run),
	cLuxInput("GamepadAxis.Axis 2", eGamepadAxisRange_Negative, eLuxAction_Lean),
	
	cLuxInput("GamepadHat.Hat 0", eGamepadHatState_Up, eLuxAction_ZoomOut),
	cLuxInput("GamepadHat.Hat 0", eGamepadHatState_Down, eLuxAction_ZoomIn),
#endif
#endif

	cLuxInput()
};

//-----------------------------------------------------------------------

// These are used for the Load/SaveUserConfig, to keep them in one place

static tString gvLuxInputPos[] =
{
	"Primary",
	"Secondary",

	""
};

static tString gsLuxInputStringSeparator = ".";

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxInputHandler::cLuxInputHandler() : iLuxUpdateable("LuxInputHandler")
{
	////////////////////////////////////
	// Get needed engine modules
	mpInput = gpBase->mpEngine->GetInput();
	mpGraphics = gpBase->mpEngine->GetGraphics();

	////////////////////////////////////
	// Game settings init
	mlMaxSmoothMousePos = gpBase->mpGameCfg->GetInt("Input","MaxSmoothMousePos",0);
	mfPrevSmoothMousePosMul = gpBase->mpGameCfg->GetFloat("Input","PrevSmoothMousePosMul",0);

#ifdef USE_GAMEPAD
	////////////////////////////////////
	// Set up gamepad
	SetUpGamepad();
#endif

	////////////////////////////////////
	// Create the actions
	CreateActions();

	////////////////////////////////////
	// Variable init
	mState = eLuxInputState_Game;
	mfMouseActiveAt = -1;
}

//-----------------------------------------------------------------------

cLuxInputHandler::~cLuxInputHandler()
{

}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxInputHandler::LoadUserConfig()
{
	mbInvertMouse = gpBase->mpUserConfig->GetBool("Input", "InvertMouse", false);
	mbSmoothMouse = gpBase->mpUserConfig->GetBool("Input", "SmoothMouse", true);

	mfMouseSensitivity = gpBase->mpUserConfig->GetFloat("Input", "MouseSensitivity", 1.0f);

#ifdef USE_GAMEPAD
	mfGamepadWalkSensitivity = gpBase->mpUserConfig->GetFloat("Input", "GamepadWalkSensitivity", 1.0f);
	mfGamepadLookSensitivity = gpBase->mpUserConfig->GetFloat("Input", "GamepadLookSensitivity", 1.5f);

	mbGamepadLookInvert = gpBase->mpUserConfig->GetBool("Input", "InvertGamepadLook", false);
#endif


	tString sSep = ".";
	///////////////////////////////////////////////////////////
	// Load user key config, clear configurable keys first
	for(int i=0;gvLuxActions[i].msName!="";++i)
	{
		cLuxAction* pLuxAction = &gvLuxActions[i];
		if(pLuxAction->mbConfigurable==false)
			continue;

		cAction* pAction = mpInput->GetAction(pLuxAction->mlId);
		pAction->ClearSubActions();
		
		////////////////////////////////////////////////////////////////
		// Get user key for primary and secondary
		bool bHasUserDefinedInputs = false;

		for(size_t j=0; gvLuxInputPos[j]!=""; ++j)
		{
			tString sInput = gpBase->mpUserKeyConfig->GetString(pAction->GetName(), gvLuxInputPos[j], "");
			if(sInput.empty())
				continue;

			bHasUserDefinedInputs = CreateSubActionFromInputString(pAction, sInput) || 
									bHasUserDefinedInputs;
		}

		// If no valid inputs were loaded, load default
		if(bHasUserDefinedInputs==false)
		{
			tLuxInputVec vDefaultInputs = GetDefaultInputsByActionId(pLuxAction->mlId);

			for(size_t i=0; i<vDefaultInputs.size(); ++i)
			{
				cLuxInput* pDefaultInput = vDefaultInputs[i];
				tStringVec vInputParts;
				cString::GetStringVec(pDefaultInput->msInputType, vInputParts, &sSep);
				CreateSubAction(pAction, vInputParts, pDefaultInput->mlValue);

#if 0 && defined(__APPLE__)
				// Heinous Kludge to get a default Mac keyboard shortcut without relying on different config files.
				if (pDefaultInput->mlActionId == eLuxAction_Attack) {
					CreateSubAction(pAction, "Keyboard", eKey_LeftMeta);
				}
#endif
			}
		}
	}
}

//-----------------------------------------------------------------------

void cLuxInputHandler::SaveUserConfig()
{
	gpBase->mpUserConfig->SetBool("Input", "InvertMouse", mbInvertMouse);
	gpBase->mpUserConfig->SetBool("Input", "SmoothMouse", mbSmoothMouse);

	gpBase->mpUserConfig->SetFloat("Input", "MouseSensitivity", mfMouseSensitivity);

#ifdef USE_GAMEPAD
	gpBase->mpUserConfig->SetBool("Input", "InvertGamepadLook", mbGamepadLookInvert);
	
	gpBase->mpUserConfig->SetFloat("Input", "GamepadWalkSensitivity", mfGamepadWalkSensitivity);
	gpBase->mpUserConfig->SetFloat("Input", "GamepadLookSensitivity", mfGamepadLookSensitivity);
#endif

	//////////////////////////////////////////////////
	// Save key config
	tStringVec vInputStringFields;
	for(int i=0;gvLuxInputPos[i]!="";++i)
		vInputStringFields.push_back(gvLuxInputPos[i]);

	// Go through all actions and save configurable ones
	for(int i=0;gvLuxActions[i].msName!="";++i)
	{
		cLuxAction* pLuxAction = &gvLuxActions[i];
		if(pLuxAction->mbConfigurable==false)
			continue;

		cAction* pAction = mpInput->GetAction(pLuxAction->mlId);
		size_t j=0;
		////////////////////////////////////////////
		// Save sub actions in action
		for(;j<pAction->GetSubActionNum() && j<vInputStringFields.size();++j)
		{
			iSubAction* pSubAction = pAction->GetSubAction(j);
			const tString& sInputPos = vInputStringFields[j];
			
			tString sInputValue = pSubAction->GetInputType() + gsLuxInputStringSeparator + pSubAction->GetInputName();
			
			gpBase->mpUserKeyConfig->SetString(pAction->GetName(), sInputPos, sInputValue);
		}
		///////////////////////////////////////////////////////////////////
		// Fill up with empty sub actions to fill up
		for(;j<vInputStringFields.size();++j)
			gpBase->mpUserKeyConfig->SetString(pAction->GetName(), vInputStringFields[j], "");
	}
}

//-----------------------------------------------------------------------

void cLuxInputHandler::OnStart()
{
	mpPlayer = gpBase->mpPlayer;
}

//-----------------------------------------------------------------------

void cLuxInputHandler::Update(float afTimeStep)
{
	///////////////////////////////////
	// Update input for current state
	UpdateGlobalInput();

	switch(mState)
	{
	//Game
	case eLuxInputState_Game: UpdateGameInput(); break;
	//Main Menu
	case eLuxInputState_MainMenu: UpdateMainMenuInput(); break;
	//Pre Menu
	case eLuxInputState_PreMenu: UpdatePreMenuInput(); break;
	//Inventory
	case eLuxInputState_Inventory: UpdateInventoryInput(); break;
	//Journal
	case eLuxInputState_Journal: UpdateJournalInput(); break;
	//Debug
	case eLuxInputState_Debug: UpdateDebugInput(); break;
	//Credits
	case eLuxInputState_Credits: UpdateCreditsInput(); break;
	//Demo End
	case eLuxInputState_DemoEnd: UpdateDemoEndInput(); break;
	//Load Screen
	case eLuxInputState_LoadScreen: UpdateLoadScreenInput(); break;
	}
}

//-----------------------------------------------------------------------

void cLuxInputHandler::Reset()
{

}

//-----------------------------------------------------------------------

void cLuxInputHandler::OnPostRender(float afFrameTime)
{
	//Turn of logging so it only happens one frame!
	gpBase->mpMapHandler->GetViewport()->GetRenderSettings()->mbLog = false;
}

//-----------------------------------------------------------------------

tWString gsInvalidActionString = _W("InvalidAction");

tWString cLuxInputHandler::GetInputName(const tString& asActionName)
{
	cAction *pAction = mpInput->GetAction(asActionName);
	if(pAction == NULL) return gsInvalidActionString;

    iSubAction *pSubAction = pAction->GetSubAction(0);

	return cString::To16Char(pSubAction->GetInputName());
}

//-----------------------------------------------------------------------

void cLuxInputHandler::ChangeState(eLuxInputState aState)
{
	mState = aState;
	

	//This rests all actions so changing states does not give unwanted clicks!
	mpInput->ResetActionsToCurrentState();

	//Reset some stuff
	mlstSmoothMousePos.clear();	//Every state needs new smoothing!

	mvLastAbsMousePos = mpInput->GetMouse()->GetAbsPosition();
}

//-----------------------------------------------------------------------

void cLuxInputHandler::SetMouseSensitivity(float afX)
{
	if(mfMouseSensitivity==afX)
		return;

	mfMouseSensitivity = afX;
}

//-----------------------------------------------------------------------

#ifdef USE_GAMEPAD
void cLuxInputHandler::SetGamepadLookSensitivity(float afX)
{
	if(mfGamepadLookSensitivity==afX)
		return;

	mfGamepadLookSensitivity = afX;
}
#endif

//-----------------------------------------------------------------------

cLuxAction* cLuxInputHandler::GetActionByName(const tString& asName)
{
	for(int i=0; gvLuxActions[i].msName!=""; ++i)
	{
		cLuxAction* pAction = &gvLuxActions[i];

		if(pAction->msName == asName) return pAction;
	}

	return NULL;
}

//-----------------------------------------------------------------------

cLuxAction* cLuxInputHandler::GetActionById(int alId)
{
	for(int i=0; gvLuxActions[i].msName!=""; ++i)
	{
		cLuxAction* pAction = &gvLuxActions[i];

		if(pAction->mlId==alId)
			return pAction;
	}

	return NULL;
}

//-----------------------------------------------------------------------

tLuxActionVec cLuxInputHandler::GetActionsByCategory(eLuxActionCategory aCat)
{
	bool bAddAll = (aCat==eLuxActionCategory_LastEnum);

	tLuxActionVec vActions;
	for(int i=0; gvLuxActions[i].msName!=""; ++i)
	{
		cLuxAction* pAction = &gvLuxActions[i];

		if(bAddAll || pAction->mCat==aCat)
			vActions.push_back(pAction);
	}

	return vActions;
}

//-----------------------------------------------------------------------

tLuxInputVec cLuxInputHandler::GetDefaultInputsByActionId(int alId)
{
	tLuxInputVec vInputs;
	for(int i=0; gvLuxInputs[i].msInputType!=""; ++i)
	{
		cLuxInput* pInput = &gvLuxInputs[i];

		if(pInput->mlActionId==alId)
			vInputs.push_back(pInput);
	}

	return vInputs;
}

//-----------------------------------------------------------------------

void cLuxInputHandler::ResetSmoothMousePos()
{
	mlstSmoothMousePos.clear();
}

//-----------------------------------------------------------------------

cVector2f cLuxInputHandler::GetSmoothMousePos(const cVector2f& avRelPosMouse)
{
	mlstSmoothMousePos.push_front(avRelPosMouse);
	if((int)mlstSmoothMousePos.size() > mlMaxSmoothMousePos) mlstSmoothMousePos.pop_back();

	float fWeight = 1.0f;
	float fWeightSum =0;
	cVector2f vPosSum=0;
	for(tVector2fListIt it = mlstSmoothMousePos.begin(); it != mlstSmoothMousePos.end(); ++it)
	{
		vPosSum += *it * fWeight;
		fWeightSum += fWeight;
		fWeight *= mfPrevSmoothMousePosMul;	//decrease influence of next pos.
	}

	return vPosSum / fWeightSum;
}

//-----------------------------------------------------------------------

#ifdef USE_GAMEPAD
bool cLuxInputHandler::IsGamepadPresent()
{
	return mpPad!=NULL;
}
#endif

//-----------------------------------------------------------------------

#ifdef USE_GAMEPAD
void cLuxInputHandler::AppDeviceWasPlugged()
{
	SetUpGamepad();
}
#endif

#ifdef USE_GAMEPAD
void cLuxInputHandler::AppDeviceWasRemoved()
{
	SetUpGamepad();
}
#endif

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxInputHandler::UpdateGlobalInput()
{
	/////////////////
	// ScreenShot
	if(mpInput->BecameTriggerd(eLuxAction_ScreenShot))
	{
		tWString sFileName = _W("");
		tWString sBaseName = _W("Screen_");
#ifndef WIN32
		tWString sScreenShotDir = cPlatform::GetSystemSpecialPath(eSystemPath_Personal);
		if (cPlatform::FolderExists(cString::AddSlashAtEndW(sScreenShotDir) + _W("Desktop"))) {
			sScreenShotDir = cString::AddSlashAtEndW(sScreenShotDir) + _W("Desktop");
		}
		sBaseName = cString::AddSlashAtEndW(sScreenShotDir) + _W("Amnesia_");
#endif
		int lCount = 0;

		do{
			sFileName = sBaseName+cString::To16Char("Screenshot")+_W("_");
			sFileName += cString::ToStringW(lCount,3);
			/*
			if(lCount >= 100)		sFileName += _W("")+cString::ToStringW(lCount);
			else if(lCount >= 10)	sFileName += _W("0")+cString::ToStringW(lCount);
			else					sFileName += _W("00")+cString::ToStringW(lCount);
			*/

			sFileName += _W(".")+cString::To16Char(gpBase->mpConfigHandler->msScreenShotExt);
			++lCount;

		}
		while(cPlatform::FileExists(sFileName));
		
		cEngine *pEngine = gpBase->mpEngine;
		
		cBitmap *pBmp = pEngine->GetGraphics()->GetLowLevel()->CopyFrameBufferToBitmap();
		pEngine->GetResources()->GetBitmapLoaderHandler()->SaveBitmap(pBmp,sFileName,0);
		hplDelete(pBmp);
	}

	/////////////////
	// Debug output
	if(mpInput->BecameTriggerd(eLuxAction_PrintInfo))
	{
		gpBase->mpMapHandler->GetViewport()->GetRenderSettings()->mbLog = true;
	}

	/////////////////
	// Send to GUI
	if(CurrentStateSendsInputToGui())
	{
		cGui *pGui = gpBase->mpEngine->GetGui();

		//Key presses
		while(mpInput->GetKeyboard()->KeyIsPressed())
		{
			pGui->SendKeyPress(mpInput->GetKeyboard()->GetKey());
		}

		//Mouse movement
		pGui->SendMousePos(mpInput->GetMouse()->GetAbsPosition(), mpInput->GetMouse()->GetRelPosition());

		
		//Mouse click
		if(mpInput->BecameTriggerd(eLuxAction_LeftClick))		pGui->SendMouseClickDown(eGuiMouseButton_Left);
		if(mpInput->WasTriggerd(eLuxAction_LeftClick))			pGui->SendMouseClickUp(eGuiMouseButton_Left);
		if(mpInput->DoubleTriggerd(eLuxAction_LeftClick, 0.3f))	pGui->SendMouseDoubleClick(eGuiMouseButton_Left);

		if(mpInput->BecameTriggerd(eLuxAction_MiddleClick))		
			pGui->SendMouseClickDown(eGuiMouseButton_Middle);
		if(mpInput->WasTriggerd(eLuxAction_MiddleClick))			pGui->SendMouseClickUp(eGuiMouseButton_Middle);
		if(mpInput->DoubleTriggerd(eLuxAction_MiddleClick, 0.3f))	pGui->SendMouseDoubleClick(eGuiMouseButton_Middle);

		if(mpInput->BecameTriggerd(eLuxAction_RightClick))		pGui->SendMouseClickDown(eGuiMouseButton_Right);
		if(mpInput->WasTriggerd(eLuxAction_RightClick))			pGui->SendMouseClickUp(eGuiMouseButton_Right);
		if(mpInput->DoubleTriggerd(eLuxAction_RightClick, 0.3f))	pGui->SendMouseDoubleClick(eGuiMouseButton_Right);

		if(mpInput->BecameTriggerd(eLuxAction_ScrollUp))		pGui->SendMouseClickDown(eGuiMouseButton_WheelUp);
		if(mpInput->WasTriggerd(eLuxAction_ScrollUp))		pGui->SendMouseClickUp(eGuiMouseButton_WheelUp);

		if(mpInput->BecameTriggerd(eLuxAction_ScrollDown))		pGui->SendMouseClickDown(eGuiMouseButton_WheelDown);
		if(mpInput->WasTriggerd(eLuxAction_ScrollDown))		pGui->SendMouseClickUp(eGuiMouseButton_WheelDown);

		if(mpInput->BecameTriggerd(eLuxAction_MouseButton6Click))		pGui->SendMouseClickDown(eGuiMouseButton_Button6);
		if(mpInput->WasTriggerd(eLuxAction_MouseButton6Click))		pGui->SendMouseClickUp(eGuiMouseButton_Button6);
		if(mpInput->DoubleTriggerd(eLuxAction_MouseButton6Click, 0.3f))	pGui->SendMouseDoubleClick(eGuiMouseButton_Button6);

		if(mpInput->BecameTriggerd(eLuxAction_MouseButton7Click))		pGui->SendMouseClickDown(eGuiMouseButton_Button7);
		if(mpInput->WasTriggerd(eLuxAction_MouseButton7Click))		pGui->SendMouseClickUp(eGuiMouseButton_Button7);
		if(mpInput->DoubleTriggerd(eLuxAction_MouseButton7Click, 0.3f))	pGui->SendMouseDoubleClick(eGuiMouseButton_Button7);
		
		if(mpInput->BecameTriggerd(eLuxAction_MouseButton8Click))		pGui->SendMouseClickDown(eGuiMouseButton_Button8);
		if(mpInput->WasTriggerd(eLuxAction_MouseButton8Click))		pGui->SendMouseClickUp(eGuiMouseButton_Button8);
		if(mpInput->DoubleTriggerd(eLuxAction_MouseButton8Click, 0.3f))	pGui->SendMouseDoubleClick(eGuiMouseButton_Button8);

		if(mpInput->BecameTriggerd(eLuxAction_MouseButton9Click))		pGui->SendMouseClickDown(eGuiMouseButton_Button9);
		if(mpInput->WasTriggerd(eLuxAction_MouseButton9Click))		pGui->SendMouseClickUp(eGuiMouseButton_Button9);
		if(mpInput->DoubleTriggerd(eLuxAction_MouseButton9Click, 0.3f))	pGui->SendMouseDoubleClick(eGuiMouseButton_Button9);

#ifdef USE_GAMEPAD
		mbGamepadUIInput = false;
		mbGamepadUIInput = UpdateGamepadUIInput();
#endif
	}
}

#ifdef USE_GAMEPAD
bool cLuxInputHandler::UpdateGamepadUIInput()
{
	bool bRet = false;
	cGui* pGui = gpBase->mpEngine->GetGui();

	//Key presses
	if(IsGamepadPresent())
	{
		while(mpPad->HasInputUpdates())
		{
			pGui->SendGamepadInput(mpPad->GetInputUpdate());
		}
	}

	int vActionIDs[] = 
	{
		eLuxAction_UIArrowUp,
		eLuxAction_UIArrowRight,
		eLuxAction_UIArrowDown,
		eLuxAction_UIArrowLeft,

		-1,

		eLuxAction_UIPrimary,
		eLuxAction_UISecondary,
		eLuxAction_UIPrevPage,
		eLuxAction_UINextPage,
		eLuxAction_Interact,
		eLuxAction_UIDelete,
		eLuxAction_UIClear,

		-1
	};

	int vInputIDs[] = 
	{
		eUIArrow_Up,
		eUIArrow_Right,
		eUIArrow_Down,
		eUIArrow_Left,

		-1,

		eUIButton_Primary,
		eUIButton_Secondary,
		eUIButton_PrevPage,
		eUIButton_NextPage,
		eUIButton_Interact,
		eUIButton_Delete,
		eUIButton_Clear,

		-1
	};

	int i=0;
	for(; vActionIDs[i]!=-1; ++i)
	{
		int lAction = vActionIDs[i];
		int lInput = vInputIDs[i];

		if(mpInput->BecameTriggerd(lAction))
		{
			pGui->SendUIArrowPress((eUIArrow)lInput);
			bRet = true;
		}
		if(mpInput->WasTriggerd(lAction))
		{
			pGui->SendUIArrowRelease((eUIArrow)lInput);
			bRet = true;
		}
	}

	++i;

	for(; vActionIDs[i]!=-1; ++i)
	{
		int lAction = vActionIDs[i];
		int lInput = vInputIDs[i];

		if(mpInput->BecameTriggerd(lAction))
		{
			pGui->SendUIButtonPress((eUIButton)lInput);
			bRet = true;
		}
		if(mpInput->WasTriggerd(lAction))
		{
			pGui->SendUIButtonRelease((eUIButton)lInput);
			bRet = true;
		}
		if(mpInput->DoubleTriggerd(lAction, 0.3f))
		{
			pGui->SendUIButtonDoublePress((eUIButton)lInput);
			bRet = true;
		}
	}

	return bRet;
}
#endif

//-----------------------------------------------------------------------

void cLuxInputHandler::UpdateGameInput()
{
	/////////////////
	// Debug
	if(mpInput->BecameTriggerd(eLuxAction_OpenDebug))
	{
		gpBase->mpDebugHandler->SetDebugWindowActive(true);
	}
	if(mpInput->BecameTriggerd(eLuxAction_ReloadMap) && gpBase->mpConfigHandler->mbLoadDebugMenu)
	{
		gpBase->mpDebugHandler->QuickReloadMap();
	}
	if(mpInput->BecameTriggerd(eLuxAction_FastForward) && gpBase->mpConfigHandler->mbLoadDebugMenu)
	{
		bool bActivate = !gpBase->mpDebugHandler->GetFastForward();

		gpBase->mpDebugHandler->SetFastForward(bActivate);
	}

	if(mpPlayer->IsDead()==false && gpBase->mpDebugHandler->GetAllowQuickSave() && gpBase->mbPTestActivated==false)
	{
		if(mpInput->BecameTriggerd(eLuxAction_QuickSave))
		{
			gpBase->mpSaveHandler->AutoSave();
		}
		if(mpInput->BecameTriggerd(eLuxAction_QuickLoad))
		{
			gpBase->mpSaveHandler->AutoLoad(false);
		}
	}

	////////////////////
	//Exit
	if(mpInput->BecameTriggerd(eLuxAction_Exit))
	{
		gpBase->mpEngine->GetUpdater()->SetContainer("MainMenu");
	}

	////////////////////
	//Toggle Crosshair
	if(mpInput->BecameTriggerd(eLuxAction_CrosshairToggle))
	{
		gpBase->mpPlayer->SetShowCrosshair(!gpBase->mpPlayer->GetShowCrosshair());
	}

	////////////////////
	// Player or other
	if(gpBase->mpMessageHandler->IsPauseMessageActive())
	{
		UpdateGameMessageInput();
	}
	else if(gpBase->mpEffectHandler->GetPlayerIsPaused())
	{
		UpdateGameEffectInput();
	}
	else
	{
		UpdateGamePlayerInput();
	}
	
}
//-----------------------------------------------------------------------

void cLuxInputHandler::UpdateGamePlayerInput()
{
	/////////////////
	// Check if player is dead
	if(mpPlayer->IsDead())
	{
		if(	mpInput->BecameTriggerd(eLuxAction_Attack) ||
			mpInput->BecameTriggerd(eLuxAction_Interact) ||
			mpInput->BecameTriggerd(eLuxAction_Ignite) ||
			mpInput->BecameTriggerd(eLuxAction_Jump) ||
			mpInput->BecameTriggerd(eLuxAction_Crouch) )
		{
			mpPlayer->GetHelperDeath()->OnPressButton();
		}
		return;
	}
	if(mpPlayer->IsActive()==false) return;

	////////////////////
	// High level
	if(mpPlayer->GetCurrentStateData()->AllowPlayerMenus())
	{
		if(mpInput->BecameTriggerd(eLuxAction_Inventory))
		{
			if(gpBase->mpInventory->GetDisabled()==false)
				gpBase->mpEngine->GetUpdater()->SetContainer("Inventory");
		}
		if(mpInput->BecameTriggerd(eLuxAction_Journal))
		{
			if(gpBase->mpInventory->GetDisabled()==false)
				gpBase->mpEngine->GetUpdater()->SetContainer("Journal");
		}
		if(mpInput->BecameTriggerd(eLuxAction_QuestLog))
		{
			if(gpBase->mpInventory->GetDisabled()==false)
			{
				gpBase->mpJournal->SetForceInstantExit(true);
				gpBase->mpEngine->GetUpdater()->SetContainer("Journal");
				gpBase->mpJournal->ChangeState(eLuxJournalState_QuestLog);
			}
		}
		if(mpInput->BecameTriggerd(eLuxAction_RecentText))
		{
			if(gpBase->mpInventory->GetDisabled()==false)
			{
				gpBase->mpJournal->OpenLastReadText();
			}
		}
	}

	/////////////////
	// Movement Direction
	if(mpInput->IsTriggerd(eLuxAction_Forward))
	{
		mpPlayer->Move(eCharDir_Forward, 1);
	}
	if(mpInput->IsTriggerd(eLuxAction_Backward))
	{
		mpPlayer->Move(eCharDir_Forward, -1);
	}
	if(mpInput->IsTriggerd(eLuxAction_Right))
	{
		mpPlayer->Move(eCharDir_Right, 1);
	}
	if(mpInput->IsTriggerd(eLuxAction_Left))
	{
		mpPlayer->Move(eCharDir_Right, -1);
	}

	/////////////////
	// Lean
	if(mpInput->IsTriggerd(eLuxAction_LeanRight))
	{
		mpPlayer->SetLean(1);
	}
	if(mpInput->IsTriggerd(eLuxAction_LeanLeft))
	{
		mpPlayer->SetLean(-1);
	}

	/////////////////
	// Actions

	//Attack
	if(mpInput->BecameTriggerd(eLuxAction_Attack))	mpPlayer->DoAction(eLuxPlayerAction_Attack, true);
	if(mpInput->WasTriggerd(eLuxAction_Attack))		mpPlayer->DoAction(eLuxPlayerAction_Attack, false);

	//Interact
	if(mpInput->BecameTriggerd(eLuxAction_Interact))
	{
		mpInput->BecameTriggerd(eLuxAction_LeftClick); 
		mpPlayer->DoAction(eLuxPlayerAction_Interact ,true);
	}
	if(mpInput->WasTriggerd(eLuxAction_Interact))
	{
		mpInput->WasTriggerd(eLuxAction_LeftClick); 
		mpPlayer->DoAction(eLuxPlayerAction_Interact, false);
	}

	//Ignite
	if(mpInput->BecameTriggerd(eLuxAction_Ignite))	mpPlayer->DoAction(eLuxPlayerAction_Ignite ,true);
	if(mpInput->WasTriggerd(eLuxAction_Ignite))		mpPlayer->DoAction(eLuxPlayerAction_Ignite, false);

	//Holster
	if(mpInput->BecameTriggerd(eLuxAction_Holster))	
	{
		if(gpBase->mpConfigHandler->mbLoadDebugMenu)
		{
			mpPlayer->GiveSanityDamage(10);
			mpPlayer->LowerSanity(3, true);//1.0f / 60.0f);
			mpPlayer->GiveDamage(10, 10, eLuxDamageType_BloodSplat, true, true);
		}	
	}
		//mpPlayer->DoAction(eLuxPlayerAction_Holster ,true);
	//if(mpInput->WasTriggerd(eLuxAction_Holster))	mpPlayer->DoAction(eLuxPlayerAction_Holster, false);
	
	//Lantern
	if(mpInput->BecameTriggerd(eLuxAction_Lantern))	mpPlayer->DoAction(eLuxPlayerAction_Lantern ,true);
	if(mpInput->WasTriggerd(eLuxAction_Lantern))	mpPlayer->DoAction(eLuxPlayerAction_Lantern, false);


	//Scroll
	if(mpInput->BecameTriggerd(eLuxAction_ScrollUp))	mpPlayer->Scroll(1.0f);
	if(mpInput->BecameTriggerd(eLuxAction_ScrollDown))	mpPlayer->Scroll(-1.0f);


	/////////////////
	// Movements

	//Run
	if(mpInput->BecameTriggerd(eLuxAction_Run))	mpPlayer->Run(true);
	if(mpInput->WasTriggerd(eLuxAction_Run))	mpPlayer->Run(false);

	//Jump
	if(mpInput->BecameTriggerd(eLuxAction_Jump))mpPlayer->Jump(true);
	if(mpInput->WasTriggerd(eLuxAction_Jump))	mpPlayer->Jump(false);

	//Crouch
	if(mpInput->BecameTriggerd(eLuxAction_Crouch))mpPlayer->Crouch(true);
	if(mpInput->WasTriggerd(eLuxAction_Crouch))	mpPlayer->Crouch(false);

	/////////////////
	// Head

	// Mouse
	cVector2l vMouseRelPos = mpInput->GetMouse()->GetRelPosition();
	cVector2f vMouseRelPosFloat = cVector2f(vMouseRelPos.x, vMouseRelPos.y)*mfMouseSensitivity;
	cVector2l vAbsRel = cMath::RoundToInt(vMouseRelPosFloat);
	cVector2f vRelPos = cVector2f((float)vAbsRel.x,(float)vAbsRel.y) / (1.7 * mpGraphics->GetLowLevel()->GetScreenSizeFloat().y);
	cVector2f vFinalPos;

	//Check if position should be smoothed.
	if(mbSmoothMouse)	vFinalPos = GetSmoothMousePos(vRelPos);
	else				vFinalPos = vRelPos;

	//Invert the Y-axis
	if(mbInvertMouse)
	{
		vFinalPos.y = -vFinalPos.y;
	}

	if(mpInput->IsTriggerd(eLuxAction_Lean))
	{
		mpPlayer->AddLean(vRelPos.x);
	}

#ifdef USE_GAMEPAD
	//////////////////////////////////////////
	// Gamepad movement and look
	if(IsGamepadPresent())
	{
#if USE_SDL2
		if(mpPad->ButtonIsDown(eGamepadButton_DpadUp) || mpPad->ButtonIsDown(eGamepadButton_DpadDown))
#else
		if(mpPad->HatIsInState(eGamepadHat_0, eGamepadHatState_Up) || mpPad->HatIsInState(eGamepadHat_0, eGamepadHatState_Down))
#endif
		{
			if(mpInput->IsTriggerd(eLuxAction_ZoomOut))	mpPlayer->Scroll( gpBase->mpEngine->GetFrameTime() * 8.0f);
			if(mpInput->IsTriggerd(eLuxAction_ZoomIn))	mpPlayer->Scroll(-gpBase->mpEngine->GetFrameTime() * 8.0f);
		}

		//////////////////////////////////////////
		// Walk
		//cVector2f vAnalogWalkAxis = cVector2f(mpPad->GetAxisValue(eGamepadAxis_0), mpPad->GetAxisValue(eGamepadAxis_1));
		
		//if(cMath::Abs(vAnalogWalkAxis.x) > 0.05f)
		//	mpPlayer->Move(eCharDir_Right, vAnalogWalkAxis.x);
		//if(cMath::Abs(vAnalogWalkAxis.y) > 0.05f)
		//	mpPlayer->Move(eCharDir_Forward, -vAnalogWalkAxis.y);
		

		//////////////////////////////////////////
		// Look / Lean
#if USE_SDL2
		cVector2f vAnalogLookAxis = cVector2f(mpPad->GetAxisValue(eGamepadAxis_RightX), mpPad->GetAxisValue(eGamepadAxis_RightY));
#else
		cVector2f vAnalogLookAxis = cVector2f(mpPad->GetAxisValue(eGamepadAxis_4), mpPad->GetAxisValue(eGamepadAxis_3));
#endif

		
		{
			if(mpInput->IsTriggerd(eLuxAction_Rotate))
			{
				mpPlayer->Scroll(-gpBase->mpEngine->GetFrameTime() * 6.0f * vAnalogLookAxis.y);

#if USE_SDL2
				vAnalogLookAxis = cVector2f(mpPad->GetAxisValue(eGamepadAxis_LeftX), mpPad->GetAxisValue(eGamepadAxis_LeftY));
#else
				vAnalogLookAxis = cVector2f(mpPad->GetAxisValue(eGamepadAxis_0), mpPad->GetAxisValue(eGamepadAxis_1));
#endif
				cVector2f vE = cMath::Vector2Abs(vAnalogLookAxis);	vE.x = sqrtf(vE.x); vE.y = sqrtf(vE.y);
				vAnalogLookAxis = vE * vAnalogLookAxis * mfGamepadLookSensitivity / 1.25f;

				if(vAnalogLookAxis.Length() > 0)
				{
					mpPlayer->GetCharacterBody()->StopMovement();
				}
			}

			///////////////
			// Make up for the dead zone
			vAnalogLookAxis -= cVector2f(cMath::Sign(vAnalogLookAxis.x), cMath::Sign(vAnalogLookAxis.y)) * mpPad->GetAxisDeadZoneRadiusValue();
			vAnalogLookAxis *= 1.0 / (1.0f - mpPad->GetAxisDeadZoneRadiusValue());

			cVector2f vExponent = cMath::Vector2Abs(vAnalogLookAxis);	vExponent.x = sqrtf(vExponent.x); vExponent.y = sqrtf(vExponent.y);
			cVector2f vGamepadPos = (vAnalogLookAxis * vExponent) * mfGamepadLookSensitivity*gpBase->mpEngine->GetStepSize();

			//Invert the Y-axis
			if(mbGamepadLookInvert)
			{
				vGamepadPos.y = -vGamepadPos.y;
			}

			vFinalPos += vGamepadPos;
		}		

		if(mpInput->IsTriggerd(eLuxAction_Lean))
		{
			if(cMath::Abs(vAnalogLookAxis.x) > 0.05f) mpPlayer->SetLean(vAnalogLookAxis.x);
			else									  mpPlayer->SetLean(0);
		}
	}
#endif
	if(mpInput->IsTriggerd(eLuxAction_Lean))
	{
		vFinalPos = 0;
	}

	mpPlayer->AddYaw(-vFinalPos.x);
	mpPlayer->AddPitch(-vFinalPos.y);
}

//-----------------------------------------------------------------------

void cLuxInputHandler::UpdateGameMessageInput()
{
	//Attack
	if(mpInput->BecameTriggerd(eLuxAction_Attack))	gpBase->mpMessageHandler->DoAction(eLuxPlayerAction_Attack, true);
	if(mpInput->WasTriggerd(eLuxAction_Attack))		gpBase->mpMessageHandler->DoAction(eLuxPlayerAction_Attack, false);

	//Interact
	if(mpInput->BecameTriggerd(eLuxAction_Interact))gpBase->mpMessageHandler->DoAction(eLuxPlayerAction_Interact ,true);
	if(mpInput->WasTriggerd(eLuxAction_Interact))	gpBase->mpMessageHandler->DoAction(eLuxPlayerAction_Interact, false);

#ifdef USE_GAMEPAD
	////////////
	// Use the UI input from the gamepad to do the same thing
	if(mpInput->BecameTriggerd(eLuxAction_UIPrimary))	gpBase->mpMessageHandler->DoAction(eLuxPlayerAction_Interact, true);
	if(mpInput->WasTriggerd(eLuxAction_UIPrimary))		gpBase->mpMessageHandler->DoAction(eLuxPlayerAction_Interact, false);

	if(mpInput->BecameTriggerd(eLuxAction_UISecondary))	gpBase->mpMessageHandler->DoAction(eLuxPlayerAction_Attack, true);
	if(mpInput->WasTriggerd(eLuxAction_UISecondary))	gpBase->mpMessageHandler->DoAction(eLuxPlayerAction_Attack, false);
#endif
	
}

//-----------------------------------------------------------------------

void cLuxInputHandler:: UpdateGameEffectInput()
{
	//Attack
	if(mpInput->BecameTriggerd(eLuxAction_Attack))	gpBase->mpEffectHandler->DoAction(eLuxPlayerAction_Attack, true);
	if(mpInput->WasTriggerd(eLuxAction_Attack))		gpBase->mpEffectHandler->DoAction(eLuxPlayerAction_Attack, false);

	//Interact
	if(mpInput->BecameTriggerd(eLuxAction_Interact))gpBase->mpEffectHandler->DoAction(eLuxPlayerAction_Interact ,true);
	if(mpInput->WasTriggerd(eLuxAction_Interact))	gpBase->mpEffectHandler->DoAction(eLuxPlayerAction_Interact, false);

#ifdef USE_GAMEPAD
	////////////
	// Use the UI input from the gamepad to do the same thing
	if(mpInput->BecameTriggerd(eLuxAction_UIPrimary))	gpBase->mpEffectHandler->DoAction(eLuxPlayerAction_Interact, true);
	if(mpInput->WasTriggerd(eLuxAction_UIPrimary))		gpBase->mpEffectHandler->DoAction(eLuxPlayerAction_Interact, false);

	if(mpInput->BecameTriggerd(eLuxAction_UISecondary))	gpBase->mpMessageHandler->DoAction(eLuxPlayerAction_Attack, true);
	if(mpInput->WasTriggerd(eLuxAction_UISecondary))	gpBase->mpEffectHandler->DoAction(eLuxPlayerAction_Attack, false);
#endif
}

//-----------------------------------------------------------------------

void cLuxInputHandler::UpdateInventoryInput()
{
	if(ShowMouseOnMouseInput())
	{
		gpBase->mpInventory->GetSet()->SetMouseMovementEnabled(true);
		gpBase->mpInventory->GetSet()->SetDrawMouse(true);
		gpBase->mpInventory->GetSet()->SetDrawFocus(false);
	}

	////////////////////
	//Exit
	if(	mpInput->BecameTriggerd(eLuxAction_Exit) ||
		mpInput->BecameTriggerd(eLuxAction_Inventory))
	{
		gpBase->mpInventory->ExitPressed();
	}

	////////////////////
	//Journal
	if(mpInput->BecameTriggerd(eLuxAction_Journal))
	{
		gpBase->mpInventory->OpenJournal();
	}

	////////////////////
	//If a message is active, we use special input
	if(gpBase->mpInventory->GetMessageActive())
	{
		gpBase->mpEngine->GetGui()->SendMousePos(mpInput->GetMouse()->GetAbsPosition(), mpInput->GetMouse()->GetRelPosition());

		if(	mpInput->BecameTriggerd(eLuxAction_LeftClick) || mpInput->BecameTriggerd(eLuxAction_MiddleClick) ||
			mpInput->BecameTriggerd(eLuxAction_RightClick) ||
			mpInput->BecameTriggerd(eLuxAction_UIPrimary) ||
			mpInput->BecameTriggerd(eLuxAction_UISecondary))
		{
			gpBase->mpInventory->ExitPressed(); //This just exits message!		
		}
	}

}

//-----------------------------------------------------------------------

void cLuxInputHandler::UpdatePreMenuInput()
{
	if(ShowMouseOnMouseInput())
	{
		gpBase->mpPreMenu->GetSet()->SetMouseMovementEnabled(true);
		gpBase->mpPreMenu->GetSet()->SetDrawMouse(true);
	}
	
	////////////////////
	//Key press
	if(mpInput->BecameTriggerd(eLuxAction_Exit) || mpInput->BecameTriggerd(eLuxAction_UIPrimary))
	{
		gpBase->mpPreMenu->ButtonPressed();
	}
}

//-----------------------------------------------------------------------

void cLuxInputHandler::UpdateMainMenuInput()
{
	if(ShowMouseOnMouseInput())
	{
		gpBase->mpMainMenu->GetSet()->SetMouseMovementEnabled(true);
		gpBase->mpMainMenu->GetSet()->SetDrawMouse(true);
	}

	////////////////////
	//Exit
	if(mpInput->BecameTriggerd(eLuxAction_Exit))
	{
		gpBase->mpMainMenu->ExitPressed();
	}
}

//-----------------------------------------------------------------------

void cLuxInputHandler::UpdateJournalInput()
{
	if(ShowMouseOnMouseInput())
	{
		gpBase->mpJournal->GetSet()->SetMouseMovementEnabled(true);
		gpBase->mpJournal->GetSet()->SetDrawMouse(true);
	}

	////////////////////
	//Exit
	if(	mpInput->BecameTriggerd(eLuxAction_Exit))
	{
		gpBase->mpJournal->ExitPressed(false);
	}
	else if(mpInput->BecameTriggerd(eLuxAction_Journal) || 
			mpInput->BecameTriggerd(eLuxAction_QuestLog) || 
			mpInput->BecameTriggerd(eLuxAction_Inventory) ||
			mpInput->BecameTriggerd(eLuxAction_RecentText))
	{
#ifdef USE_GAMEPAD
		if(mbGamepadUIInput==false) gpBase->mpJournal->ExitPressed(true);
#else
		gpBase->mpJournal->ExitPressed(true);
#endif
	}
}

//-----------------------------------------------------------------------

void cLuxInputHandler::UpdateDebugInput()
{
	if(mpInput->BecameTriggerd(eLuxAction_OpenDebug))
	{
		gpBase->mpDebugHandler->SetDebugWindowActive(false);
	}
}

//-----------------------------------------------------------------------

void cLuxInputHandler::UpdateCreditsInput()
{
	if(	mpInput->BecameTriggerd(eLuxAction_Exit) ||
		mpInput->BecameTriggerd(eLuxAction_Jump) ||
		mpInput->BecameTriggerd(eLuxAction_Attack) ||
		mpInput->BecameTriggerd(eLuxAction_Interact) ||
		mpInput->BecameTriggerd(eLuxAction_Inventory))
	{
		gpBase->mpCredits->ExitPressed();
	}
}

//-----------------------------------------------------------------------

void cLuxInputHandler::UpdateDemoEndInput()
{
	if(gpBase->mpDemoEnd)
	{
		if(mpInput->BecameTriggerd(eLuxAction_Exit))
		{
			gpBase->mpDemoEnd->Exit(false);
		}
	}
}

//-----------------------------------------------------------------------


void cLuxInputHandler::UpdateLoadScreenInput()
{
	if(	mpInput->BecameTriggerd(eLuxAction_Exit) ||
		mpInput->BecameTriggerd(eLuxAction_Jump) ||
		mpInput->BecameTriggerd(eLuxAction_Attack) ||
		mpInput->BecameTriggerd(eLuxAction_Interact) ||
		mpInput->BecameTriggerd(eLuxAction_Inventory))
	{
		gpBase->mpLoadScreenHandler->ExitPressed();
	}
}

//-----------------------------------------------------------------------

bool cLuxInputHandler::CurrentStateSendsInputToGui()
{
	switch(mState)
	{
	case eLuxInputState_Inventory:
		{
			//When message is active, do not send anything to GUI.
			if(gpBase->mpInventory->GetMessageActive()) return false;
		}
	case eLuxInputState_Debug: 
	case eLuxInputState_Journal:
	case eLuxInputState_MainMenu: 
	case eLuxInputState_PreMenu:
	case eLuxInputState_DemoEnd:
		return true;	
	}

	return false;
}

//-----------------------------------------------------------------------

void cLuxInputHandler::CreateActions()
{
	//////////////////////////////////
	// Loop through all defined actions and create them
	for(int i=0; gvLuxActions[i].msName != ""; ++i)
	{
		cLuxAction *pLuxAction = &gvLuxActions[i];

        mpInput->CreateAction(pLuxAction->msName, pLuxAction->mlId);
	}

	tString sSep = ".";
	//////////////////////////////////
	// Loop through all defined inputs and bind them to actions
	for(int i=0; gvLuxInputs[i].msInputType != ""; ++i)
	{
		cLuxInput *pLuxInput = &gvLuxInputs[i];

        cAction *pAction = mpInput->GetAction(pLuxInput->mlActionId);

		int lPara = 0;
		if(pLuxInput->mlActionId==eLuxAction_Forward)
			lPara=1;

		tStringVec vInputParts;
		cString::GetStringVec(pLuxInput->msInputType, vInputParts, &sSep);

		CreateSubAction(pAction, 
						vInputParts,
						pLuxInput->mlValue);
	}
    
}

//-----------------------------------------------------------------------

void cLuxInputHandler::CreateSubAction(cAction *apAction, const tStringVec& avType, int alValue)
{
	tString sType = cString::ToLowerCase(avType[0]);
	
    //Keyboard
	if(sType == "keyboard")
	{
		apAction->AddKey((eKey)alValue);
	}
	//Mouse
	else if(sType == "mousebutton")
	{
		apAction->AddMouseButton((eMouseButton)alValue);
	}
#ifdef USE_GAMEPAD
	//Gamepad
	else if(cString::GetFirstStringPos(sType, "gamepad")==0)
	{
		if(avType[0]=="GamepadButton")
			apAction->AddGamepadButton(0, (eGamepadButton)alValue);
		else
		{
			eGamepadHat hat = iGamepad::StringToHat(avType[1]);
			eGamepadAxis axis = iGamepad::StringToAxis(avType[1]);

			//////////////////////////////////////////////////
			// Gamepad elements
			if(hat!=eGamepadHat_LastEnum)
			{
				apAction->AddGamepadHat(0, hat, (eGamepadHatState)alValue);
			}
			else if(axis!=eGamepadAxis_LastEnum)
			{
				apAction->AddGamepadAxis(0, axis, (eGamepadAxisRange)alValue, 0.25f);
			}
		}
	}
#endif
}

//-----------------------------------------------------------------------

tStringVec cLuxInputHandler::GetInputValueStrings(const tString& asX)
{
	tStringVec vInputStrings;
	cString::GetStringVec(asX, vInputStrings);

	return vInputStrings;
}

//-----------------------------------------------------------------------

bool cLuxInputHandler::CreateSubActionFromInputString(cAction* apAction, const tString& asInputString)
{
	tString sSep = ".";
	tStringVec vInputParts;
	cString::GetStringVec(asInputString, vInputParts, &sSep);

	if(vInputParts.empty()==false)
	{
		int lInputValue = -1;
		tString sInputType = cString::ToLowerCase(vInputParts[0]);
				
		///////////////////////////////////////////////////
		// Now check if the input type - value combo is valid
		if(sInputType=="keyboard")
		{
			lInputValue = mpInput->GetKeyboard()->StringToKey(vInputParts[1]);
			if(lInputValue==eKey_LastEnum) lInputValue=-1;
		}
		else if(sInputType=="mousebutton")
		{
			lInputValue = mpInput->GetMouse()->StringToButton(vInputParts[1]);
			if(lInputValue==eMouseButton_LastEnum) lInputValue=-1;
		}
#ifdef USE_GAMEPAD
		else if(cString::GetFirstStringPos(sInputType,"gamepad")!=-1)
		{

			if(vInputParts[0]=="GamepadButton")
			{
				lInputValue = iGamepad::StringToButton(vInputParts[1]);
				if(lInputValue==eGamepadButton_LastEnum) lInputValue=-1;
			}
			else
			{
				eGamepadHat hat = iGamepad::StringToHat(vInputParts[1]);
				eGamepadAxis axis = iGamepad::StringToAxis(vInputParts[1]);
								
				if(hat!=eGamepadHat_LastEnum)
				{
					lInputValue = iGamepad::StringToHatState(vInputParts[2]);
					if(lInputValue==eGamepadHat_LastEnum) lInputValue=-1;
				}
				else if(axis!=eGamepadAxis_LastEnum)
				{
					lInputValue = iGamepad::StringToAxisRange(vInputParts[2]);
					if(lInputValue==eGamepadAxisRange_LastEnum) lInputValue=-1;
				}
			}
		}
#else
		else
			vInputParts.clear();
#endif
		
		////////////////////////////////////////
		// If input is valid, create sub action
		if(vInputParts.empty()==false && lInputValue!=-1)
		{
			CreateSubAction(apAction, vInputParts, lInputValue);

			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------

bool cLuxInputHandler::ShowMouseOnMouseInput()
{
	////////////////
	// Checks if there are have been any mouse input since last frame
	bool bMouseActive = false;

	for(int i = 0; i < eMouseButton_LastEnum; ++i)
	{
		bMouseActive = bMouseActive || mpInput->GetMouse()->ButtonIsDown(eMouseButton(i));
	}

	/////////////////
	// Get the relative mouse position, cant use GetRelPosition() when mouse cursor is hidden
	cVector2l vAbsPos = mpInput->GetMouse()->GetAbsPosition();

	bMouseActive = bMouseActive || (mvLastAbsMousePos - vAbsPos).SqrLength() > 0;

	mvLastAbsMousePos = vAbsPos;

	////////////////
	// Show the cursor
	if(bMouseActive)
	{
		if(mfMouseActiveAt != -1)
		{
		//	gpBase->SetDrawOnLiveCursor(true);
		}
		else
		{
			bMouseActive = false;
		}
		mfMouseActiveAt = gpBase->mpEngine->GetGameTime();
	}
#ifdef USE_GAMEPAD
	else if(IsGamepadPresent())
	{
		bool bDirPressed = mpInput->IsTriggerd(eLuxAction_UIArrowUp) || 
						   mpInput->IsTriggerd(eLuxAction_UIArrowDown) ||
						   mpInput->IsTriggerd(eLuxAction_UIArrowLeft) ||
						   mpInput->IsTriggerd(eLuxAction_UIArrowRight);

		if(bDirPressed || (mfMouseActiveAt + 5 < gpBase->mpEngine->GetGameTime() && mfMouseActiveAt > 0))
		{
			gpBase->mpInventory->GetSet()->SetDrawMouse(false);
			gpBase->mpInventory->GetSet()->SetMouseMovementEnabled(false);
			gpBase->mpInventory->GetSet()->SetDrawFocus(true);
			gpBase->mpMainMenu->GetSet()->SetMouseMovementEnabled(false);
			gpBase->mpMainMenu->GetSet()->SetDrawMouse(false);
			gpBase->mpJournal->GetSet()->SetMouseMovementEnabled(false);
			gpBase->mpJournal->GetSet()->SetDrawMouse(false);
			gpBase->mpPreMenu->GetSet()->SetMouseMovementEnabled(false);
			gpBase->mpPreMenu->GetSet()->SetDrawMouse(false);

			mvLastAbsMousePos = mpGraphics->GetLowLevel()->GetScreenSizeInt() / 2;
			mfMouseActiveAt = -1;
		}
	}
#endif

	return bMouseActive;
}

//-----------------------------------------------------------------------

#ifdef USE_GAMEPAD
void cLuxInputHandler::SetUpGamepad()
{
	mpPad = mpInput->GetGamepad(0);
	if(IsGamepadPresent())
	{
		mpPad->SetAxisDeadZoneRadiusValue(0.15f);
	}
}
#endif

//-----------------------------------------------------------------------
