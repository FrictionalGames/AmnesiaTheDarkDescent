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

#include "impl/KeyboardSDL.h"

#if USE_SDL2
#include "SDL2/SDL.h"
#else
#include "SDL/SDL.h"
#endif

#include "impl/LowLevelInputSDL.h"
#include "system/String.h"

#ifdef Win32
#include <conio.h>
#endif

namespace hpl {
	
	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cKeyboardSDL::cKeyboardSDL(cLowLevelInputSDL *apLowLevelInputSDL) : iKeyboard("SDL Portable Keyboard")
	{
		mpLowLevelInputSDL = apLowLevelInputSDL;

		mvKeyArray.resize(eKey_LastEnum);
#if !USE_SDL2 && defined __APPLE__
		// world keys 0 to 95
		mvWorldKeyMap.resize(96);
		// Initialize to None
		for (int k=0; k<=95; ++k) {
			mvWorldKeyMap[k] = eKey_None;
		}
#endif
		ClearKeyList();

#if !SDL_VERSION_ATLEAST(2, 0, 0)
		SDL_EnableUNICODE(1);
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
#endif
	}
	
	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	void cKeyboardSDL::Update()
	{
		mlstKeysPressed.clear();
		mlstKeysReleased.clear();
		std::list<SDL_Event>::iterator it = mpLowLevelInputSDL->mlstEvents.begin();
        for(; it != mpLowLevelInputSDL->mlstEvents.end(); ++it)
		{
			SDL_Event *pEvent = &(*it);

			if(pEvent->type == SDL_KEYDOWN || pEvent->type == SDL_KEYUP)
#if SDL_VERSION_ATLEAST(2, 0, 0)
			{
                eKey key = SDLToKey(pEvent->key.keysym.sym);

                mvKeyArray[key] = pEvent->key.state == SDL_PRESSED?true:false;
                int sdl_mod = pEvent->key.keysym.mod;

                if(pEvent->key.state == SDL_PRESSED)
                {
                    AddKeyToList(sdl_mod, key, 0, mlstKeysPressed);
                }
                else
                {
                    AddKeyToList(sdl_mod, key, 0, mlstKeysReleased);
                }
            }
            else if(pEvent->type == SDL_TEXTINPUT)
            {
                tWString tstr = cString::UTF8ToWChar(pEvent->text.text);
                for (size_t i=0,l=tstr.size(); i<l; ++i) {
                    AddKeyToList(SDL_GetModState(), eKey_None, (int)tstr[i], mlstKeysPressed);
                }
            }
#else
            {
                eKey key = SDLToKey(pEvent->key.keysym.sym);
#   ifdef __APPLE__
                if (key >= eKey_World_0 && key <= eKey_World_95) {
                    if (pEvent->type == SDL_KEYDOWN) {
                        eKey test = SDLToKey(pEvent->key.keysym.unicode);
                        mvWorldKeyMap[key - eKey_World_0] = test;
                        if (test != eKey_None) key = test;
                    } else {
                        if (mvWorldKeyMap[key - eKey_World_0] != eKey_None) {
                            key = mvWorldKeyMap[key - eKey_World_0];
                        }
                    }
                }
#   endif
                mvKeyArray[key] = pEvent->type == SDL_KEYDOWN?true:false;

                int lUnicode = pEvent->key.keysym.unicode;
                int sdl_mod = pEvent->key.keysym.mod;

                if(pEvent->type == SDL_KEYDOWN)
                {
                    AddKeyToList(sdl_mod, key, lUnicode, mlstKeysPressed);
                }
                else
                    AddKeyToList(sdl_mod, key, lUnicode, mlstKeysReleased);
            }
#endif
        }
	}

	//-----------------------------------------------------------------------

    bool cKeyboardSDL::KeyIsDown(eKey aKey)
	{
		return mvKeyArray[aKey];
	}

	//-----------------------------------------------------------------------


	cKeyPress cKeyboardSDL::GetKey()
	{
		cKeyPress key = mlstKeysPressed.front();
		mlstKeysPressed.pop_front();
		return key;
	}

	//-----------------------------------------------------------------------

	bool cKeyboardSDL::KeyIsPressed()
	{
		return mlstKeysPressed.empty()==false;
	}

	//-----------------------------------------------------------------------

	/*
	int cKeyboardSDL::GetModifier()
	{
		return eKeyModifier_None;
	}
	*/

	//-----------------------------------------------------------------------

	cKeyPress cKeyboardSDL::GetReleasedKey()
	{
		cKeyPress key = mlstKeysReleased.front();
		mlstKeysReleased.pop_front();
		return key;
	}

	//-----------------------------------------------------------------------

	bool cKeyboardSDL::KeyIsReleased()
	{
		return mlstKeysReleased.empty()==false;
	}
	
	//-----------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	/////////////////////////////////////////////////////////////////////////
	
	//-----------------------------------------------------------------------
	
	eKey cKeyboardSDL::SDLToKey(int alKey)
	{
		switch(alKey)
		{
			case	SDLK_BACKSPACE: return eKey_BackSpace;
			case	SDLK_TAB:return  eKey_Tab;
			case 	SDLK_CLEAR: return eKey_Clear;
			case 	SDLK_RETURN: return eKey_Return;
			case 	SDLK_PAUSE: return eKey_Pause;
			case 	SDLK_ESCAPE: return eKey_Escape;
			case 	SDLK_SPACE: return eKey_Space;
			case 	SDLK_EXCLAIM: return eKey_Exclaim;
			case 	SDLK_QUOTEDBL: return eKey_QuoteDouble;
			case 	SDLK_HASH: return eKey_Hash;
			case 	SDLK_DOLLAR: return eKey_Dollar;
			case 	SDLK_AMPERSAND: return eKey_Ampersand;
			case 	SDLK_QUOTE: return eKey_Quote;
			case 	SDLK_LEFTPAREN: return eKey_LeftParen;
			case 	SDLK_RIGHTPAREN: return eKey_RightParen;
			case 	SDLK_ASTERISK: return eKey_Asterisk;
			case 	SDLK_PLUS: return eKey_Plus;
			case 	SDLK_COMMA: return eKey_Comma;
			case 	SDLK_MINUS: return eKey_Minus;
			case 	SDLK_PERIOD: return eKey_Period;
			case 	SDLK_SLASH: return eKey_Slash;
			case 	SDLK_0: return eKey_0;
			case 	SDLK_1: return eKey_1;
			case 	SDLK_2: return eKey_2;
			case 	SDLK_3: return eKey_3;
			case 	SDLK_4: return eKey_4;
			case 	SDLK_5: return eKey_5;
			case 	SDLK_6: return eKey_6;
			case 	SDLK_7: return eKey_7;
			case 	SDLK_8: return eKey_8;
			case 	SDLK_9: return eKey_9;
			case 	SDLK_COLON: return eKey_Colon;
			case 	SDLK_SEMICOLON: return eKey_SemiColon;
			case 	SDLK_LESS: return eKey_Less;
			case 	SDLK_EQUALS: return eKey_Equals;
			case 	SDLK_GREATER: return eKey_Greater;
			case 	SDLK_QUESTION: return eKey_Question;
			case 	SDLK_AT: return eKey_At;
			case 	SDLK_LEFTBRACKET: return eKey_LeftBracket;
			case 	SDLK_BACKSLASH: return eKey_BackSlash;
			case 	SDLK_RIGHTBRACKET: return eKey_RightBracket;
			case 	SDLK_CARET: return eKey_Caret;
			case 	SDLK_UNDERSCORE: return eKey_Underscore;
			case 	SDLK_BACKQUOTE: return eKey_BackSlash;
			case 	SDLK_a: return eKey_A;
			case 	SDLK_b: return eKey_B;
			case 	SDLK_c: return eKey_C;
			case 	SDLK_d: return eKey_D;
			case 	SDLK_e: return eKey_E;
			case 	SDLK_f: return eKey_F;
			case 	SDLK_g: return eKey_G;
			case 	SDLK_h: return eKey_H;
			case 	SDLK_i: return eKey_I;
			case 	SDLK_j: return eKey_J;
			case 	SDLK_k: return eKey_K;
			case 	SDLK_l: return eKey_L;
			case 	SDLK_m: return eKey_M;
			case 	SDLK_n: return eKey_N;
			case 	SDLK_o: return eKey_O;
			case 	SDLK_p: return eKey_P;
			case 	SDLK_q: return eKey_Q;
			case 	SDLK_r: return eKey_R;
			case 	SDLK_s: return eKey_S;
			case 	SDLK_t: return eKey_T;
			case 	SDLK_u: return eKey_U;
			case 	SDLK_v: return eKey_V;
			case 	SDLK_w: return eKey_W;
			case 	SDLK_x: return eKey_X;
			case 	SDLK_y: return eKey_Y;
			case 	SDLK_z: return eKey_Z;
			case 	SDLK_DELETE: return eKey_Delete;
#if SDL_VERSION_ATLEAST(2, 0, 0)
			case 	SDLK_KP_0: return eKey_KP_0;
			case 	SDLK_KP_1: return eKey_KP_1;
			case 	SDLK_KP_2: return eKey_KP_2;
			case 	SDLK_KP_3: return eKey_KP_3;
			case 	SDLK_KP_4: return eKey_KP_4;
			case 	SDLK_KP_5: return eKey_KP_5;
			case 	SDLK_KP_6: return eKey_KP_6;
			case 	SDLK_KP_7: return eKey_KP_7;
			case 	SDLK_KP_8: return eKey_KP_8;
			case 	SDLK_KP_9: return eKey_KP_9;
#else
			case 	SDLK_KP0: return eKey_KP_0;
			case 	SDLK_KP1: return eKey_KP_1;
			case 	SDLK_KP2: return eKey_KP_2;
			case 	SDLK_KP3: return eKey_KP_3;
			case 	SDLK_KP4: return eKey_KP_4;
			case 	SDLK_KP5: return eKey_KP_5;
			case 	SDLK_KP6: return eKey_KP_6;
			case 	SDLK_KP7: return eKey_KP_7;
			case 	SDLK_KP8: return eKey_KP_8;
			case 	SDLK_KP9: return eKey_KP_9;
#endif
			case 	SDLK_KP_PERIOD: return eKey_KP_Period;
			case 	SDLK_KP_DIVIDE: return eKey_KP_Divide;
			case 	SDLK_KP_MULTIPLY: return eKey_KP_Multiply;
			case 	SDLK_KP_MINUS: return eKey_KP_Minus;
			case 	SDLK_KP_PLUS: return eKey_KP_Plus;
			case 	SDLK_KP_ENTER: return eKey_KP_Enter;
			case 	SDLK_KP_EQUALS: return eKey_KP_Equals;
			case 	SDLK_UP: return eKey_Up;
			case 	SDLK_DOWN: return eKey_Down;
			case 	SDLK_RIGHT: return eKey_Right;
			case 	SDLK_LEFT: return eKey_Left;
			case 	SDLK_INSERT: return eKey_Insert;
			case 	SDLK_HOME: return eKey_Home;
			case 	SDLK_END: return eKey_End;
			case 	SDLK_PAGEUP: return eKey_PageUp;
			case 	SDLK_PAGEDOWN: return eKey_PageDown;
			case 	SDLK_F1: return eKey_F1;
			case 	SDLK_F2: return eKey_F2;
			case 	SDLK_F3: return eKey_F3;
			case 	SDLK_F4: return eKey_F4;
			case 	SDLK_F5: return eKey_F5;
			case 	SDLK_F6: return eKey_F6;
			case 	SDLK_F7: return eKey_F7;
			case 	SDLK_F8: return eKey_F8;
			case 	SDLK_F9: return eKey_F9;
			case 	SDLK_F10: return eKey_F10;
			case 	SDLK_F11: return eKey_F11;
			case 	SDLK_F12: return eKey_F12;
			case 	SDLK_F13: return eKey_F13;
			case 	SDLK_F14: return eKey_F14;
			case 	SDLK_F15: return eKey_F15;
#if SDL_VERSION_ATLEAST(2, 0, 0)
            case    SDLK_NUMLOCKCLEAR: return eKey_NumLock;
			case 	SDLK_SCROLLLOCK: return eKey_ScrollLock;
			case 	SDLK_LGUI: return eKey_LeftSuper;
			case 	SDLK_RGUI: return eKey_RightSuper;
            case    SDLK_PRINTSCREEN: return eKey_Print;
#else
			case 	SDLK_NUMLOCK: return eKey_NumLock;
			case 	SDLK_SCROLLOCK: return eKey_ScrollLock;
			case 	SDLK_LSUPER: return eKey_LeftSuper;
			case 	SDLK_RSUPER: return eKey_RightSuper;
			case 	SDLK_RMETA: return eKey_RightMeta;
			case 	SDLK_LMETA: return eKey_LeftMeta;
			case 	SDLK_PRINT: return eKey_Print;
			case 	SDLK_BREAK: return eKey_Break;
#endif
			case 	SDLK_CAPSLOCK: return eKey_CapsLock;
			case 	SDLK_RSHIFT: return eKey_RightShift;
			case 	SDLK_LSHIFT: return eKey_LeftShift;
			case 	SDLK_RCTRL: return eKey_RightCtrl;
			case 	SDLK_LCTRL: return eKey_LeftCtrl;
			case 	SDLK_RALT: return eKey_RightAlt;
			case 	SDLK_LALT: return eKey_LeftAlt;
			case 	SDLK_MODE: return eKey_Mode;
			case 	SDLK_HELP: return eKey_Help;
			case 	SDLK_SYSREQ: return eKey_SysReq;
			case 	SDLK_MENU: return eKey_Menu;
			case 	SDLK_POWER: return eKey_Power;
#if !SDL_VERSION_ATLEAST(2, 0, 0)
			case 	SDLK_EURO: return eKey_Euro;
			case 	SDLK_WORLD_0: return eKey_World_0;
			case 	SDLK_WORLD_1: return eKey_World_1;
			case 	SDLK_WORLD_2: return eKey_World_2;
			case 	SDLK_WORLD_3: return eKey_World_3;
			case 	SDLK_WORLD_4: return eKey_World_4;
			case 	SDLK_WORLD_5: return eKey_World_5;
			case 	SDLK_WORLD_6: return eKey_World_6;
			case 	SDLK_WORLD_7: return eKey_World_7;
			case 	SDLK_WORLD_8: return eKey_World_8;
			case 	SDLK_WORLD_9: return eKey_World_9;
			case 	SDLK_WORLD_10: return eKey_World_10;
			case 	SDLK_WORLD_11: return eKey_World_11;
			case 	SDLK_WORLD_12: return eKey_World_12;
			case 	SDLK_WORLD_13: return eKey_World_13;
			case 	SDLK_WORLD_14: return eKey_World_14;
			case 	SDLK_WORLD_15: return eKey_World_15;
			case 	SDLK_WORLD_16: return eKey_World_16;
			case 	SDLK_WORLD_17: return eKey_World_17;
			case 	SDLK_WORLD_18: return eKey_World_18;
			case 	SDLK_WORLD_19: return eKey_World_19;
			case 	SDLK_WORLD_20: return eKey_World_20;
			case 	SDLK_WORLD_21: return eKey_World_21;
			case 	SDLK_WORLD_22: return eKey_World_22;
			case 	SDLK_WORLD_23: return eKey_World_23;
			case 	SDLK_WORLD_24: return eKey_World_24;
			case 	SDLK_WORLD_25: return eKey_World_25;
			case 	SDLK_WORLD_26: return eKey_World_26;
			case 	SDLK_WORLD_27: return eKey_World_27;
			case 	SDLK_WORLD_28: return eKey_World_28;
			case 	SDLK_WORLD_29: return eKey_World_29;
			case 	SDLK_WORLD_30: return eKey_World_30;
			case 	SDLK_WORLD_31: return eKey_World_31;
			case 	SDLK_WORLD_32: return eKey_World_32;
			case 	SDLK_WORLD_33: return eKey_World_33;
			case 	SDLK_WORLD_34: return eKey_World_34;
			case 	SDLK_WORLD_35: return eKey_World_35;
			case 	SDLK_WORLD_36: return eKey_World_36;
			case 	SDLK_WORLD_37: return eKey_World_37;
			case 	SDLK_WORLD_38: return eKey_World_38;
			case 	SDLK_WORLD_39: return eKey_World_39;
			case 	SDLK_WORLD_40: return eKey_World_40;
			case 	SDLK_WORLD_41: return eKey_World_41;
			case 	SDLK_WORLD_42: return eKey_World_42;
			case 	SDLK_WORLD_43: return eKey_World_43;
			case 	SDLK_WORLD_44: return eKey_World_44;
			case 	SDLK_WORLD_45: return eKey_World_45;
			case 	SDLK_WORLD_46: return eKey_World_46;
			case 	SDLK_WORLD_47: return eKey_World_47;
			case 	SDLK_WORLD_48: return eKey_World_48;
			case 	SDLK_WORLD_49: return eKey_World_49;
			case 	SDLK_WORLD_50: return eKey_World_50;
			case 	SDLK_WORLD_51: return eKey_World_51;
			case 	SDLK_WORLD_52: return eKey_World_52;
			case 	SDLK_WORLD_53: return eKey_World_53;
			case 	SDLK_WORLD_54: return eKey_World_54;
			case 	SDLK_WORLD_55: return eKey_World_55;
			case 	SDLK_WORLD_56: return eKey_World_56;
			case 	SDLK_WORLD_57: return eKey_World_57;
			case 	SDLK_WORLD_58: return eKey_World_58;
			case 	SDLK_WORLD_59: return eKey_World_59;
			case 	SDLK_WORLD_60: return eKey_World_60;
			case 	SDLK_WORLD_61: return eKey_World_61;
			case 	SDLK_WORLD_62: return eKey_World_62;
			case 	SDLK_WORLD_63: return eKey_World_63;
			case 	SDLK_WORLD_64: return eKey_World_64;
			case 	SDLK_WORLD_65: return eKey_World_65;
			case 	SDLK_WORLD_66: return eKey_World_66;
			case 	SDLK_WORLD_67: return eKey_World_67;
			case 	SDLK_WORLD_68: return eKey_World_68;
			case 	SDLK_WORLD_69: return eKey_World_69;
			case 	SDLK_WORLD_70: return eKey_World_70;
			case 	SDLK_WORLD_71: return eKey_World_71;
			case 	SDLK_WORLD_72: return eKey_World_72;
			case 	SDLK_WORLD_73: return eKey_World_73;
			case 	SDLK_WORLD_74: return eKey_World_74;
			case 	SDLK_WORLD_75: return eKey_World_75;
			case 	SDLK_WORLD_76: return eKey_World_76;
			case 	SDLK_WORLD_77: return eKey_World_77;
			case 	SDLK_WORLD_78: return eKey_World_78;
			case 	SDLK_WORLD_79: return eKey_World_79;
			case 	SDLK_WORLD_80: return eKey_World_80;
			case 	SDLK_WORLD_81: return eKey_World_81;
			case 	SDLK_WORLD_82: return eKey_World_82;
			case 	SDLK_WORLD_83: return eKey_World_83;
			case 	SDLK_WORLD_84: return eKey_World_84;
			case 	SDLK_WORLD_85: return eKey_World_85;
			case 	SDLK_WORLD_86: return eKey_World_86;
			case 	SDLK_WORLD_87: return eKey_World_87;
			case 	SDLK_WORLD_88: return eKey_World_88;
			case 	SDLK_WORLD_89: return eKey_World_89;
			case 	SDLK_WORLD_90: return eKey_World_90;
			case 	SDLK_WORLD_91: return eKey_World_91;
			case 	SDLK_WORLD_92: return eKey_World_92;
			case 	SDLK_WORLD_93: return eKey_World_93;
			case 	SDLK_WORLD_94: return eKey_World_94;
			case 	SDLK_WORLD_95: return eKey_World_95;
#endif
		}

		return eKey_None;
	}
	
	//-----------------------------------------------------------------------

	void cKeyboardSDL::ClearKeyList()
	{
		mvKeyArray.assign(mvKeyArray.size(),false);
	}

	//-----------------------------------------------------------------------

	void cKeyboardSDL::AddKeyToList(int alSDLMod, eKey aKey, int alUnicode, std::list<cKeyPress>& alstKeys)
	{
		int mod =0;

		if(alSDLMod & KMOD_CTRL)		mod |= eKeyModifier_Ctrl;
		if(alSDLMod & KMOD_SHIFT)		mod |= eKeyModifier_Shift;
		if(alSDLMod & KMOD_ALT)			mod |= eKeyModifier_Alt;
		
		alstKeys.push_back(cKeyPress(aKey,alUnicode,mod));
		
		//if(mlstKeysPressed.size()>MAX_KEY_PRESSES) mlstKeysPressed.pop_front();
	}

	//-----------------------------------------------------------------------

}
