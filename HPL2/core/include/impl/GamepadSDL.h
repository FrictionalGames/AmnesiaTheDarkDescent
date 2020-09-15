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

#ifndef HPL_GAMEPAD_SDL_H
#define HPL_GAMEPAD_SDL_H

#if !USE_SDL2

#include <vector>
#include <list>
#include "system/SystemTypes.h"
#include "input/Gamepad.h"
//#include "input/InputTypes.h"

#include "SDL/SDL.h"

namespace hpl {

	class cLowLevelInputSDL;
	
	class cGamepadSDL : public iGamepad
	{
	public:
		cGamepadSDL(cLowLevelInputSDL *apLowLevelInputSDL, int alIndex);

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

	private:
		eGamepadButton		SDLToButton(Uint8 alButton);
		eGamepadAxis		SDLToAxis(Uint8 alAxis);
		float				SDLToAxisValue(Sint16 alAxisValue);
		eGamepadHat			SDLToHat(Uint8 alHat);
		eGamepadHatState	SDLToHatState(Uint8 alHat);
		eGamepadBall		SDLToBall(Uint8 alBall);
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

		SDL_Joystick		*mpHandle;
		cLowLevelInputSDL	*mpLowLevelInputSDL;

		static float mfInvAxisMax;
		static float mfDeadZoneRadius;
	};

};

#endif // USE_SDL2

#endif // HPL_GAMEPAD_SDL_H
