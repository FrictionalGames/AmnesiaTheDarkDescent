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

#ifndef HPL_GAMEPAD_XINPUT_H
#define HPL_GAMEPAD_XINPUT_H

#define USE_XINPUT 0

#if USE_XINPUT

#include <windows.h>
#include <XInput.h>

#include <vector>
#include <list>
#include "system/SystemTypes.h"
#include "input/Gamepad.h"
//#include "input/InputTypes.h"

namespace hpl {

	class cGamepadXInput : public iGamepad
	{
	public:
		cGamepadXInput(int alIndex);

		tString GetGamepadName() { return msGamepadName; }

		int GetNumButtons();
		int GetNumAxes();
		int GetNumHats();
		int GetNumBalls();

		void Update();

		//Gamepad specific
		bool HasInputUpdates();
		cGamepadInputData GetInputUpdate();
		
		bool ButtonIsDown(eGamepadButton aButton);
		cGamepadInputData GetButton();
		bool ButtonIsPressed();
		bool ButtonIsReleased();
		cGamepadInputData GetReleasedButton();

		float GetAxisValue(eGamepadAxis aAxis);
		float GetAxisDeadZoneRadiusValue();
		void SetAxisDeadZoneRadiusValue(float afValue);
		bool AxesUpdated();
		cGamepadInputData GetUpdatedAxis();

		eGamepadHatState GetHatCurrentState(eGamepadHat aHat);
		bool HatIsInState(eGamepadHat aHat, eGamepadHatState aState);
		bool HatsChanged();
		cGamepadInputData GetHatState();

		cVector2l GetBallAbsPos(eGamepadBall aBall);
		cVector2l GetBallRelPos(eGamepadBall aBall);

		static bool IsConnected(int alIndex);

		static bool GetWasConnected(int alIndex) { return mbDeviceConnected[alIndex]; }
		static void SetWasConnected(int alIndex, bool abVal) { mbDeviceConnected[alIndex] = abVal; }
	private:
		void UpdateAxis(int alAxis, float afVal);
		void UpdateTrigger(float afLVal, float afRVal);

		eGamepadButton		XInputToButton(size_t alButton);
		float				XInputToAxisValue(SHORT alAxisValue);
		float				XInputToTriggerValue(BYTE alTriggerValue);
        //void ClearKeyList();
		//eKey AsciiToKey(int alChar);

		//void AddKeyToList(int alSDLMod, eKey aKey, int alUnicode, std::list<cKeyPress>& alstKeys);

		tString msGamepadName;

		std::vector<float>				mvAxisArray;
		std::vector<eGamepadAxisRange>	mvAxisRange;
		std::vector<bool>				mvButtonArray;
		std::vector<eGamepadHatState>	mvHatArray;
		std::vector<cVector2l>			mvBallAbsPosArray;
		std::vector<cVector2l>			mvBallRelPosArray;

		std::list<cGamepadInputData> mlstInputUpdates;

		std::list<cGamepadInputData> mlstButtonsPressed;
		std::list<cGamepadInputData> mlstButtonsReleased;

		std::list<cGamepadInputData> mlstAxisChanges;

		std::list<cGamepadInputData> mlstHatStateChanges;

		static float mfInvAxisMax;
		static float mfDeadZoneRadius;

		XINPUT_STATE mState;

		float mfLeftTrigger;
		float mfRightTrigger;

		static bool mbDeviceConnected[4];
	};

};

#endif // USE_XINPUT
#endif // HPL_GAMEPAD_XINPUT_H
