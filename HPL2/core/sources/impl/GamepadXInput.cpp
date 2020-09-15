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

#include "impl/GamepadXInput.h"

#if USE_XINPUT

#include "math/Math.h"
#include <limits>

#ifdef Win32
#include <conio.h>
#endif

namespace hpl {
	
	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	float cGamepadXInput::mfInvAxisMax = 1.0f/(float(32768));
	float cGamepadXInput::mfDeadZoneRadius = 3200.0f*cGamepadXInput::mfInvAxisMax;
	bool cGamepadXInput::mbDeviceConnected[4] = { false, false, false, false };

	//-----------------------------------------------------------------------
	
	cGamepadXInput::cGamepadXInput(int alIndex) : iGamepad("XInput Portable Gamepad", alIndex)
	{
		msGamepadName = "XInput Gamepad";

		memset(&mState, 0, sizeof(mState));

		mvButtonArray.resize(10);
		mvButtonArray.assign(mvButtonArray.size(), false);

		mvAxisArray.resize(7);
		mvAxisArray.assign(mvAxisArray.size(), 0.0f);

		mvHatArray.resize(1);
		mvHatArray.assign(mvHatArray.size(), eGamepadHatState_Centered);

		mvBallAbsPosArray.resize(0);
		mvBallAbsPosArray.assign(mvBallAbsPosArray.size(), cVector2l(0,0));
		mvBallRelPosArray.resize(0);
		mvBallRelPosArray.assign(mvBallRelPosArray.size(), cVector2l(0,0));

		mfLeftTrigger = 0;
		mfRightTrigger = 0;
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	int cGamepadXInput::GetNumButtons()
	{
		return (int)mvButtonArray.size();
	}

	int cGamepadXInput::GetNumAxes()
	{
		return (int)mvAxisArray.size();
	}

	int cGamepadXInput::GetNumHats()
	{
		return (int)mvHatArray.size();
	}

	int cGamepadXInput::GetNumBalls()
	{
		return (int)mvBallAbsPosArray.size();
	}

	//-----------------------------------------------------------------------


	void cGamepadXInput::Update()
	{
		cGamepadInputData inputUpdate;

		mlstInputUpdates.clear();
		mlstButtonsPressed.clear();
		mlstButtonsReleased.clear();
		mlstHatStateChanges.clear();

		///////////////////
		// Get a new input state
		memset(&mState, 0, sizeof(mState));

		DWORD lResult = XInputGetState(mlIndex, &mState);

		///////////
		// Only check if it is connected
		if(lResult == ERROR_SUCCESS)
		{
			WORD lButtons = mState.Gamepad.wButtons;

			///////////////////
			// D-pad
			int lHatState = 0;
			if(lButtons & XINPUT_GAMEPAD_DPAD_UP)		lHatState |= eGamepadHatState_Up;
			if(lButtons & XINPUT_GAMEPAD_DPAD_DOWN)		lHatState |= eGamepadHatState_Down;
			if(lButtons & XINPUT_GAMEPAD_DPAD_RIGHT)	lHatState |= eGamepadHatState_Right;
			if(lButtons & XINPUT_GAMEPAD_DPAD_LEFT)		lHatState |= eGamepadHatState_Left;

			if(mvHatArray[0] != lHatState)
			{
				mvHatArray[0] = static_cast<eGamepadHatState>(lHatState);
				inputUpdate = cGamepadInputData(mlIndex, eGamepadInputType_Hat, 0, (float)lHatState);
				mlstHatStateChanges.push_back(inputUpdate);
				mlstInputUpdates.push_back(inputUpdate);
			}

			///////////////////
			// Buttons
			size_t vButton[10] = 
			{ 
				XINPUT_GAMEPAD_A, XINPUT_GAMEPAD_B, XINPUT_GAMEPAD_X, XINPUT_GAMEPAD_Y, 
				XINPUT_GAMEPAD_LEFT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER, XINPUT_GAMEPAD_BACK, 
				XINPUT_GAMEPAD_START, XINPUT_GAMEPAD_LEFT_THUMB, XINPUT_GAMEPAD_RIGHT_THUMB 
			};

			for(int i = 0; i < 10; ++i)
			{
				eGamepadButton button = XInputToButton(vButton[i]);
				inputUpdate = cGamepadInputData(mlIndex, eGamepadInputType_Button, button, 0.0f);

				bool bCurrent = mvButtonArray[button];
				bool bPressed = (lButtons & vButton[i]) == vButton[i];

				if(bCurrent && !bPressed)
				{
					inputUpdate.mfInputValue = 0.0f;
					mlstButtonsReleased.push_back(inputUpdate);
				}
				else if(!bCurrent && bPressed)
				{
					inputUpdate.mfInputValue = 1.0f;
					mlstButtonsPressed.push_back(inputUpdate);
				}

				if(bCurrent != bPressed)
				{
					mlstInputUpdates.push_back(inputUpdate);
				}

				mvButtonArray[button] = bPressed;
			}

			//////////////////
			// Axis
			UpdateAxis(0, XInputToAxisValue(mState.Gamepad.sThumbLX));
			UpdateAxis(1,-XInputToAxisValue(mState.Gamepad.sThumbLY));
			UpdateAxis(3,-XInputToAxisValue(mState.Gamepad.sThumbRY));
			UpdateAxis(4, XInputToAxisValue(mState.Gamepad.sThumbRX));
			
			UpdateTrigger(XInputToTriggerValue(mState.Gamepad.bLeftTrigger), XInputToTriggerValue(mState.Gamepad.bRightTrigger));
		}
		else
		{
			mvButtonArray.assign(mvButtonArray.size(), false);
			mvAxisArray.assign(mvAxisArray.size(), 0.0f);
			mvHatArray.assign(mvHatArray.size(), eGamepadHatState_Centered);
		}
	}

	//-----------------------------------------------------------------------

	bool cGamepadXInput::HasInputUpdates()
	{
		return mlstInputUpdates.empty()==false;
	}

	cGamepadInputData cGamepadXInput::GetInputUpdate()
	{
		cGamepadInputData input = mlstInputUpdates.front();
		mlstInputUpdates.pop_front();

		switch(input.mInputType)
		{
		case eGamepadInputType_Button:
			{
				if(input.mfInputValue==0.0f)
					mlstButtonsReleased.remove(input);
				else
					mlstButtonsPressed.remove(input);
			}
			break;
		case eGamepadInputType_Hat:
			mlstHatStateChanges.remove(input);
			break;
		case eGamepadInputType_Axis:
			mlstAxisChanges.remove(input);
			break;
		}

		return input;
	}

	//-----------------------------------------------------------------------

    bool cGamepadXInput::ButtonIsDown(eGamepadButton aButton)
	{
		return mvButtonArray[aButton];
	}

	//-----------------------------------------------------------------------


	cGamepadInputData cGamepadXInput::GetButton()
	{
		cGamepadInputData button = mlstButtonsPressed.front();
		mlstButtonsPressed.pop_front();
		mlstInputUpdates.remove(button);

		return button;
	}

	//-----------------------------------------------------------------------

	bool cGamepadXInput::ButtonIsPressed()
	{
		return mlstButtonsPressed.empty()==false;
	}

	//-----------------------------------------------------------------------

	cGamepadInputData cGamepadXInput::GetReleasedButton()
	{
		cGamepadInputData button = mlstButtonsReleased.front();
		mlstButtonsReleased.pop_front();
		mlstInputUpdates.remove(button);

		return button;
	}

	//-----------------------------------------------------------------------

	bool cGamepadXInput::ButtonIsReleased()
	{
		return mlstButtonsReleased.empty()==false;
	}

	//-----------------------------------------------------------------------

	float cGamepadXInput::GetAxisValue(eGamepadAxis aAxis)
	{
		return mvAxisArray[aAxis];
	}

	float cGamepadXInput::GetAxisDeadZoneRadiusValue()
	{
		return mfDeadZoneRadius;
	}

	void cGamepadXInput::SetAxisDeadZoneRadiusValue(float afValue)
	{
		mfDeadZoneRadius = afValue;
	}

	cGamepadInputData cGamepadXInput::GetUpdatedAxis()
	{
		cGamepadInputData axis = mlstAxisChanges.front();
		mlstAxisChanges.pop_front();
		mlstInputUpdates.remove(axis);

		return axis;
	}

	bool cGamepadXInput::AxesUpdated()
	{
		return mlstAxisChanges.empty()==false;
	}

	//-----------------------------------------------------------------------

	eGamepadHatState cGamepadXInput::GetHatCurrentState(eGamepadHat aHat)
	{
		return mvHatArray[aHat];
	}

	bool cGamepadXInput::HatIsInState(eGamepadHat aHat, eGamepadHatState aState)
	{
		return (GetHatCurrentState(aHat)&aState)!=0;
	}

	bool cGamepadXInput::HatsChanged()
	{
		return mlstHatStateChanges.empty()==false;
	}

	cGamepadInputData cGamepadXInput::GetHatState()
	{
		cGamepadInputData change = mlstHatStateChanges.front();
		mlstHatStateChanges.pop_front();
		return change;
	}
	
	//-----------------------------------------------------------------------

	cVector2l cGamepadXInput::GetBallAbsPos(eGamepadBall aBall)
	{
		return mvBallAbsPosArray[aBall];
	}

	cVector2l cGamepadXInput::GetBallRelPos(eGamepadBall aBall)
	{
		return mvBallRelPosArray[aBall];
	}

	bool cGamepadXInput::IsConnected(int alIndex)
	{
		XINPUT_STATE state;

		memset(&state, 0, sizeof(state));

		return XInputGetState(alIndex, &state) == ERROR_SUCCESS;
	}
	
	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	/////////////////////////////////////////////////////////////////////////
	
	//----------------------------------------------------------------------

	void cGamepadXInput::UpdateAxis(int alAxis, float afVal)
	{
		if(cMath::Abs(afVal) < mfDeadZoneRadius)
			afVal = 0.0f;
				
		if(afVal!=mvAxisArray[alAxis])
		{
			cGamepadInputData inputUpdate = cGamepadInputData(mlIndex, eGamepadInputType_Axis, static_cast<eGamepadAxis>(alAxis), afVal);

			mlstAxisChanges.push_back(inputUpdate);
			mlstInputUpdates.push_back(inputUpdate);
		}

		mvAxisArray[alAxis] = afVal;
	}

	//----------------------------------------------------------------------

	void cGamepadXInput::UpdateTrigger(float afLVal, float afRVal)
	{
		// Todo: Remove hack that uses axis 2 for both left and right trigger
		
		if(afLVal < mfDeadZoneRadius)
			afLVal = 0.0f;
		if(afRVal < mfDeadZoneRadius)
			afRVal = 0.0f;

		if(afRVal != mfRightTrigger)
		{
			mfRightTrigger = afRVal;

			cGamepadInputData inputUpdate = cGamepadInputData(mlIndex, eGamepadInputType_Axis, 2,-mfRightTrigger);

			mlstAxisChanges.push_back(inputUpdate);
			mlstInputUpdates.push_back(inputUpdate);
			
			mvAxisArray[2] =-mfRightTrigger;
		}

		if(afLVal != mfLeftTrigger)
		{
			mfLeftTrigger = afLVal;

			cGamepadInputData inputUpdate = cGamepadInputData(mlIndex, eGamepadInputType_Axis, 2, mfLeftTrigger);

			mlstAxisChanges.push_back(inputUpdate);
			mlstInputUpdates.push_back(inputUpdate);

			mvAxisArray[2] = mfLeftTrigger;
		}
	}

	//-----------------------------------------------------------------------
	
	eGamepadButton cGamepadXInput::XInputToButton(size_t alButton)
	{
		/////////////
		// Convert button to correct enum
		switch(alButton)
		{
		case XINPUT_GAMEPAD_A:				return eGamepadButton_0;
		case XINPUT_GAMEPAD_B:				return eGamepadButton_1;
		case XINPUT_GAMEPAD_X:				return eGamepadButton_2;
		case XINPUT_GAMEPAD_Y:				return eGamepadButton_3;
		case XINPUT_GAMEPAD_LEFT_SHOULDER:	return eGamepadButton_4;
		case XINPUT_GAMEPAD_RIGHT_SHOULDER: return eGamepadButton_5;
		case XINPUT_GAMEPAD_BACK:			return eGamepadButton_6;
		case XINPUT_GAMEPAD_START:			return eGamepadButton_7;
		case XINPUT_GAMEPAD_LEFT_THUMB:		return eGamepadButton_8;
		case XINPUT_GAMEPAD_RIGHT_THUMB:	return eGamepadButton_9;
		}

		return eGamepadButton_LastEnum;
	}

	float cGamepadXInput::XInputToAxisValue(SHORT alAxisValue)
	{
		return alAxisValue * mfInvAxisMax;
	}

	float cGamepadXInput::XInputToTriggerValue(BYTE alTriggerValue)
	{
		return alTriggerValue / 255.0f;
	}

	//-----------------------------------------------------------------------

}


#endif