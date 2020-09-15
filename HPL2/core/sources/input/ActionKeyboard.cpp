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

#include "input/ActionKeyboard.h"
#include "input/Input.h"
#include "input/Keyboard.h"

namespace hpl
{
	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	cActionKeyboard::cActionKeyboard(cInput *apInput, eKey aKey)
	{
		mKey = aKey;	
		mpInput = apInput;
	}
	
	//-----------------------------------------------------------------------
	
	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cActionKeyboard::IsTriggerd()
	{
		return mpInput->GetKeyboard()->KeyIsDown(mKey);
	}
	//-----------------------------------------------------------------------

	float cActionKeyboard::GetValue()
	{
		if(IsTriggerd())return 1.0;
		else return 0.0;
	}
	
	//-----------------------------------------------------------------------
	
	tString cActionKeyboard::GetInputName()
	{
		return mpInput->GetKeyboard()->KeyToString(mKey);
		/*switch(mKey)
		{
			case eKey_BackSpace: return "BackSpace";
			case eKey_Tab: return "Tab";
			case eKey_Clear: return "Clear";
			case eKey_Return: return "Return";
			case eKey_Pause: return "Pause";
			case eKey_Escape: return "Escape";
			case eKey_Space: return "Space";
			case eKey_Exclaim: return "Exclaim";
			case eKey_QuoteDouble: return "DblQoute";
			case eKey_Hash: return "Hash";
			case eKey_Dollar: return "Dollar";
			case eKey_Ampersand: return "Ampersand";
			case eKey_Quote: return "Quote";
			case eKey_LeftParen: return "LeftParent";
			case eKey_RightParen: return "RightParent";
			case eKey_Asterisk: return "Asterisk";
			case eKey_Plus: return "Plus";
			case eKey_Comma: return "Comma";
			case eKey_Minus: return "Minus";
			case eKey_Period: return "Period";
			case eKey_Slash: return "Slash";
			case eKey_0: return "0";
			case eKey_1: return "1";
			case eKey_2: return "2";
			case eKey_3: return "3";
			case eKey_4: return "4";
			case eKey_5: return "5";
			case eKey_6: return "6";
			case eKey_7: return "7";
			case eKey_8: return "8";
			case eKey_9: return "9";
			case eKey_Colon: return "Colon";
			case eKey_SemiColon: return "SemiColon";
			case eKey_Less: return "Less";
			case eKey_Equals: return "Equals";
			case eKey_Greater: return "Greater";
			case eKey_Question: return "Question";
			case eKey_At: return "At";
			case eKey_LeftBracket: return "LeftBracket";
			case eKey_BackSlash: return "BackSlash";
			case eKey_RightBracket: return "RightBracket";
			case eKey_Caret: return "Caret";
			case eKey_Underscore: return "Underscore";
			case eKey_BackQuote: return "BackQuote";
			case eKey_A: return "A";
			case eKey_B: return "B";
			case eKey_C: return "C";
			case eKey_D: return "D";
			case eKey_E: return "E";
			case eKey_F: return "F";
			case eKey_G: return "G";
			case eKey_H: return "H";
			case eKey_I: return "I";
			case eKey_J: return "J";
			case eKey_K: return "K";
			case eKey_L: return "L";
			case eKey_M: return "M";
			case eKey_N: return "N";
			case eKey_O: return "O";
			case eKey_P: return "P";
			case eKey_Q: return "Q";
			case eKey_R: return "R";
			case eKey_S: return "S";
			case eKey_T: return "T";
			case eKey_U: return "U";
			case eKey_V: return "V";
			case eKey_W: return "W";
			case eKey_X: return "X";
			case eKey_Y: return "Y";
			case eKey_Z: return "Z";
			case eKey_Delete: return "Delete";
			case eKey_KP_0: return "KP_0";
			case eKey_KP_1: return "KP_1";
			case eKey_KP_2: return "KP_2";
			case eKey_KP_3: return "KP_3";
			case eKey_KP_4: return "KP_4";
			case eKey_KP_5: return "KP_5";
			case eKey_KP_6: return "KP_6";
			case eKey_KP_7: return "KP_7";
			case eKey_KP_8: return "KP_8";
			case eKey_KP_9: return "KP_9";
			case eKey_KP_Period: return "KP_Period";
			case eKey_KP_Divide: return "KP_Divide";
			case eKey_KP_Multiply: return "KP_Multiply";
			case eKey_KP_Minus: return "KP_Minus";
			case eKey_KP_Plus: return "KP_Plus";
			case eKey_KP_Enter: return "KP_Enter";
			case eKey_KP_Equals: return "KP_Equals";
			case eKey_Up: return "Up";
			case eKey_Down: return "Down";
			case eKey_Right: return "Right";
			case eKey_Left: return "Left";
			case eKey_Insert: return "Insert";
			case eKey_Home: return "Home";
			case eKey_End: return "End";
			case eKey_PageUp: return "PageUp";
			case eKey_PageDown: return "PageDown";
			case eKey_F1: return "F1";
			case eKey_F2: return "F2";
			case eKey_F3: return "F3";
			case eKey_F4: return "F4";
			case eKey_F5: return "F5";
			case eKey_F6: return "F6";
			case eKey_F7: return "F7";
			case eKey_F8: return "F8";
			case eKey_F9: return "F9";
			case eKey_F10: return "F10";
			case eKey_F11: return "F11";
			case eKey_F12: return "F12";
			case eKey_F13: return "F13";
			case eKey_F14: return "F14";
			case eKey_F15: return "F15";
			case eKey_NumLock: return "NumLock";
			case eKey_CapsLock: return "CapsLock";
			case eKey_ScrollLock: return "ScrollLock";
			case eKey_RightShift: return "RightShift";
			case eKey_LeftShift: return "LeftShift";
			case eKey_RightCtrl: return "RightControl";
			case eKey_LeftCtrl: return "LeftControl";
			case eKey_RightAlt: return "RightAlt";
			case eKey_LeftAlt: return "LeftAlt";
			case eKey_RightMeta: return "RightMeta";
			case eKey_LeftMeta: return "LeftMeta";
			case eKey_LeftSuper: return "LeftSuper";
			case eKey_RightSuper: return "RightSuper";
			case eKey_Mode: return "Mode";
			case eKey_Help: return "Help";
			case eKey_Print: return "Print";
			case eKey_SysReq: return "SysReq";
			case eKey_Break: return "Break";
			case eKey_Menu: return "Menu";
			case eKey_Power: return "Power";
			case eKey_Euro: return "Euro";
			case eKey_None: return "None";
		}
		
		return "Unknown";*/
	}

	//-----------------------------------------------------------------------
}
