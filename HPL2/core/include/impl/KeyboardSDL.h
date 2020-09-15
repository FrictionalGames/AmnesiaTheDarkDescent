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

#ifndef HPL_KEYBOARD_SDL_H
#define HPL_KEYBOARD_SDL_H

#include <vector>
#include <list>
#include "system/SystemTypes.h"
#include "input/Keyboard.h"

namespace hpl {

#define MAX_KEY_PRESSES (20)

	class cLowLevelInputSDL;
	
	class cKeyboardSDL : public iKeyboard
	{
	public:
		cKeyboardSDL(cLowLevelInputSDL *apLowLevelInputSDL);

		void Update();

		//Keyboard specific 
		bool KeyIsDown(eKey aKey);
		cKeyPress GetKey();
		bool KeyIsPressed();
		//int GetModifier();
		bool KeyIsReleased();
		cKeyPress GetReleasedKey();

	private:
		eKey SDLToKey(int alKey);
        void ClearKeyList();
		eKey AsciiToKey(int alChar);

		void AddKeyToList(int alSDLMod, eKey aKey, int alUnicode, std::list<cKeyPress>& alstKeys);

		std::vector<bool> mvKeyArray;
#if !USE_SDL2 && defined __APPLE__
		std::vector<eKey> mvWorldKeyMap;
#endif
		std::list<cKeyPress> mlstKeysPressed;
		std::list<cKeyPress> mlstKeysReleased;

		cLowLevelInputSDL *mpLowLevelInputSDL;
	};

};

#endif // HPL_KEYBOARD_SDL_H
