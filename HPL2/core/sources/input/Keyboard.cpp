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

#include "input/Keyboard.h"
#include "input/InputTypes.h"

namespace hpl
{
	
	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iKeyboard::iKeyboard(tString asName) : iInputDevice(asName, eInputDeviceType_Keyboard)
	{
	
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	int iKeyboard::GetModifier()
	{
		int lMod = 0;
		lMod |= (KeyIsDown(eKey_RightCtrl) || KeyIsDown(eKey_LeftCtrl))? eKeyModifier_Ctrl : 0;
		lMod |= (KeyIsDown(eKey_RightShift) || KeyIsDown(eKey_LeftShift))? eKeyModifier_Shift : 0;
		lMod |= (KeyIsDown(eKey_RightAlt) || KeyIsDown(eKey_LeftAlt))? eKeyModifier_Alt : 0;

		return lMod;
	}

	//-----------------------------------------------------------------------

	tString iKeyboard::KeyToString(eKey aKey)
	{
		switch(aKey)
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
			case eKey_World_0: return "World_0";
			case eKey_World_1: return "World_1";
			case eKey_World_2: return "World_2";
			case eKey_World_3: return "World_3";
			case eKey_World_4: return "World_4";
			case eKey_World_5: return "World_5";
			case eKey_World_6: return "World_6";
			case eKey_World_7: return "World_7";
			case eKey_World_8: return "World_8";
			case eKey_World_9: return "World_9";
			case eKey_World_10: return "World_10";
			case eKey_World_11: return "World_11";
			case eKey_World_12: return "World_12";
			case eKey_World_13: return "World_13";
			case eKey_World_14: return "World_14";
			case eKey_World_15: return "World_15";
			case eKey_World_16: return "World_16";
			case eKey_World_17: return "World_17";
			case eKey_World_18: return "World_18";
			case eKey_World_19: return "World_19";
			case eKey_World_20: return "World_20";
			case eKey_World_21: return "World_21";
			case eKey_World_22: return "World_22";
			case eKey_World_23: return "World_23";
			case eKey_World_24: return "World_24";
			case eKey_World_25: return "World_25";
			case eKey_World_26: return "World_26";
			case eKey_World_27: return "World_27";
			case eKey_World_28: return "World_28";
			case eKey_World_29: return "World_29";
			case eKey_World_30: return "World_30";
			case eKey_World_31: return "World_31";
			case eKey_World_32: return "World_32";
			case eKey_World_33: return "World_33";
			case eKey_World_34: return "World_34";
			case eKey_World_35: return "World_35";
			case eKey_World_36: return "World_36";
			case eKey_World_37: return "World_37";
			case eKey_World_38: return "World_38";
			case eKey_World_39: return "World_39";
			case eKey_World_40: return "World_40";
			case eKey_World_41: return "World_41";
			case eKey_World_42: return "World_42";
			case eKey_World_43: return "World_43";
			case eKey_World_44: return "World_44";
			case eKey_World_45: return "World_45";
			case eKey_World_46: return "World_46";
			case eKey_World_47: return "World_47";
			case eKey_World_48: return "World_48";
			case eKey_World_49: return "World_49";
			case eKey_World_50: return "World_50";
			case eKey_World_51: return "World_51";
			case eKey_World_52: return "World_52";
			case eKey_World_53: return "World_53";
			case eKey_World_54: return "World_54";
			case eKey_World_55: return "World_55";
			case eKey_World_56: return "World_56";
			case eKey_World_57: return "World_57";
			case eKey_World_58: return "World_58";
			case eKey_World_59: return "World_59";
			case eKey_World_60: return "World_60";
			case eKey_World_61: return "World_61";
			case eKey_World_62: return "World_62";
			case eKey_World_63: return "World_63";
			case eKey_World_64: return "World_64";
			case eKey_World_65: return "World_65";
			case eKey_World_66: return "World_66";
			case eKey_World_67: return "World_67";
			case eKey_World_68: return "World_68";
			case eKey_World_69: return "World_69";
			case eKey_World_70: return "World_70";
			case eKey_World_71: return "World_71";
			case eKey_World_72: return "World_72";
			case eKey_World_73: return "World_73";
			case eKey_World_74: return "World_74";
			case eKey_World_75: return "World_75";
			case eKey_World_76: return "World_76";
			case eKey_World_77: return "World_77";
			case eKey_World_78: return "World_78";
			case eKey_World_79: return "World_79";
			case eKey_World_80: return "World_80";
			case eKey_World_81: return "World_81";
			case eKey_World_82: return "World_82";
			case eKey_World_83: return "World_83";
			case eKey_World_84: return "World_84";
			case eKey_World_85: return "World_85";
			case eKey_World_86: return "World_86";
			case eKey_World_87: return "World_87";
			case eKey_World_88: return "World_88";
			case eKey_World_89: return "World_89";
			case eKey_World_90: return "World_90";
			case eKey_World_91: return "World_91";
			case eKey_World_92: return "World_92";
			case eKey_World_93: return "World_93";
			case eKey_World_94: return "World_94";
			case eKey_World_95: return "World_95";
		}
		
		return "Unknown";
	}

	//-----------------------------------------------------------------------

	tString iKeyboard::ModifierKeyToString(eKeyModifier aModKey)
	{
		switch(aModKey)
		{
		case eKeyModifier_None: return "";
		case eKeyModifier_Ctrl: return "CTRL";
		case eKeyModifier_Shift: return "SHIFT";
		case eKeyModifier_Alt: return "ALT";
		}

		return "Unknown";
	}

	//-----------------------------------------------------------------------

	eKey iKeyboard::StringToKey(const tString& asKey)
	{
		if(asKey=="BackSpace") return eKey_BackSpace;
		if(asKey=="Tab") return eKey_Tab;
		if(asKey=="Clear") return eKey_Clear;
		if(asKey=="Return") return eKey_Return;
		if(asKey=="Pause") return eKey_Pause;
		if(asKey=="Escape") return eKey_Escape;
		if(asKey=="Space") return eKey_Space;
		if(asKey=="Exclaim") return eKey_Exclaim;
		if(asKey=="DblQoute") return eKey_QuoteDouble;
		if(asKey=="Hash") return eKey_Hash;
		if(asKey=="Dollar") return eKey_Dollar;
		if(asKey=="Ampersand") return eKey_Ampersand;
		if(asKey=="Quote") return eKey_Quote;
		if(asKey=="LeftParent") return eKey_LeftParen;
		if(asKey=="RightParent") return eKey_RightParen;
		if(asKey=="Asterisk") return eKey_Asterisk;
		if(asKey=="Plus") return eKey_Plus;
		if(asKey=="Comma") return eKey_Comma;
		if(asKey=="Minus") return eKey_Minus;
		if(asKey=="Period") return eKey_Period;
		if(asKey=="Slash") return eKey_Slash;
		if(asKey=="0") return eKey_0;
		if(asKey=="1") return eKey_1;
		if(asKey=="2") return eKey_2;
		if(asKey=="3") return eKey_3;
		if(asKey=="4") return eKey_4;
		if(asKey=="5") return eKey_5;
		if(asKey=="6") return eKey_6;
		if(asKey=="7") return eKey_7;
		if(asKey=="8") return eKey_8;
		if(asKey=="9") return eKey_9;
		if(asKey=="Colon") return eKey_Colon;
		if(asKey=="SemiColon") return eKey_SemiColon;
		if(asKey=="Less") return eKey_Less;
		if(asKey=="Equals") return eKey_Equals;
		if(asKey=="Greater") return eKey_Greater;
		if(asKey=="Question") return eKey_Question;
		if(asKey=="At") return eKey_At;
		if(asKey=="LeftBracket") return eKey_LeftBracket;
		if(asKey=="BackSlash") return eKey_BackSlash;
		if(asKey=="RightBracket") return eKey_RightBracket;
		if(asKey=="Caret") return eKey_Caret;
		if(asKey=="Underscore") return eKey_Underscore;
		if(asKey=="BackQuote") return eKey_BackQuote;
		if(asKey=="A") return eKey_A;
		if(asKey=="B") return eKey_B;
		if(asKey=="C") return eKey_C;
		if(asKey=="D") return eKey_D;
		if(asKey=="E") return eKey_E;
		if(asKey=="F") return eKey_F;
		if(asKey=="G") return eKey_G;
		if(asKey=="H") return eKey_H;
		if(asKey=="I") return eKey_I;
		if(asKey=="J") return eKey_J;
		if(asKey=="K") return eKey_K;
		if(asKey=="L") return eKey_L;
		if(asKey=="M") return eKey_M;
		if(asKey=="N") return eKey_N;
		if(asKey=="O") return eKey_O;
		if(asKey=="P") return eKey_P;
		if(asKey=="Q") return eKey_Q;
		if(asKey=="R") return eKey_R;
		if(asKey=="S") return eKey_S;
		if(asKey=="T") return eKey_T;
		if(asKey=="U") return eKey_U;
		if(asKey=="V") return eKey_V;
		if(asKey=="W") return eKey_W;
		if(asKey=="X") return eKey_X;
		if(asKey=="Y") return eKey_Y;
		if(asKey=="Z") return eKey_Z;
		if(asKey=="Delete") return eKey_Delete;
		if(asKey=="KP_0") return eKey_KP_0;
		if(asKey=="KP_1") return eKey_KP_1;
		if(asKey=="KP_2") return eKey_KP_2;
		if(asKey=="KP_3") return eKey_KP_3;
		if(asKey=="KP_4") return eKey_KP_4;
		if(asKey=="KP_5") return eKey_KP_5;
		if(asKey=="KP_6") return eKey_KP_6;
		if(asKey=="KP_7") return eKey_KP_7;
		if(asKey=="KP_8") return eKey_KP_8;
		if(asKey=="KP_9") return eKey_KP_9;
		if(asKey=="KP_Period") return eKey_KP_Period;
		if(asKey=="KP_Divide") return eKey_KP_Divide;
		if(asKey=="KP_Multiply") return eKey_KP_Multiply;
		if(asKey=="KP_Minus") return eKey_KP_Minus;
		if(asKey=="KP_Plus") return eKey_KP_Plus;
		if(asKey=="KP_Enter") return eKey_KP_Enter;
		if(asKey=="KP_Equals") return eKey_KP_Equals;
		if(asKey=="Up") return eKey_Up;
		if(asKey=="Down") return eKey_Down;
		if(asKey=="Right") return eKey_Right;
		if(asKey=="Left") return eKey_Left;
		if(asKey=="Insert") return eKey_Insert;
		if(asKey=="Home") return eKey_Home;
		if(asKey=="End") return eKey_End;
		if(asKey=="PageUp") return eKey_PageUp;
		if(asKey=="PageDown") return eKey_PageDown;
		if(asKey=="F1") return eKey_F1;
		if(asKey=="F2") return eKey_F2;
		if(asKey=="F3") return eKey_F3;
		if(asKey=="F4") return eKey_F4;
		if(asKey=="F5") return eKey_F5;
		if(asKey=="F6") return eKey_F6;
		if(asKey=="F7") return eKey_F7;
		if(asKey=="F8") return eKey_F8;
		if(asKey=="F9") return eKey_F9;
		if(asKey=="F10") return eKey_F10;
		if(asKey=="F11") return eKey_F11;
		if(asKey=="F12") return eKey_F12;
		if(asKey=="F13") return eKey_F13;
		if(asKey=="F14") return eKey_F14;
		if(asKey=="F15") return eKey_F15;
		if(asKey=="NumLock") return eKey_NumLock;
		if(asKey=="CapsLock") return eKey_CapsLock;
		if(asKey=="ScrollLock") return eKey_ScrollLock;
		if(asKey=="RightShift") return eKey_RightShift;
		if(asKey=="LeftShift") return eKey_LeftShift;
		if(asKey=="RightControl") return eKey_RightCtrl;
		if(asKey=="LeftControl") return eKey_LeftCtrl;
		if(asKey=="RightAlt") return eKey_RightAlt;
		if(asKey=="LeftAlt") return eKey_LeftAlt;
		if(asKey=="RightMeta") return eKey_RightMeta;
		if(asKey=="LeftMeta") return eKey_LeftMeta;
		if(asKey=="LeftSuper") return eKey_LeftSuper;
		if(asKey=="RightSuper") return eKey_RightSuper;
		if(asKey=="Mode") return eKey_Mode;
		if(asKey=="Help") return eKey_Help;
		if(asKey=="Print") return eKey_Print;
		if(asKey=="SysReq") return eKey_SysReq;
		if(asKey=="Break") return eKey_Break;
		if(asKey=="Menu") return eKey_Menu;
		if(asKey=="Power") return eKey_Power;
		if(asKey=="Euro") return eKey_Euro;
		if(asKey=="None") return eKey_None;
		if(asKey=="World_0") return eKey_World_0;
		if(asKey=="World_1") return eKey_World_1;
		if(asKey=="World_2") return eKey_World_2;
		if(asKey=="World_3") return eKey_World_3;
		if(asKey=="World_4") return eKey_World_4;
		if(asKey=="World_5") return eKey_World_5;
		if(asKey=="World_6") return eKey_World_6;
		if(asKey=="World_7") return eKey_World_7;
		if(asKey=="World_8") return eKey_World_8;
		if(asKey=="World_9") return eKey_World_9;
		if(asKey=="World_10") return eKey_World_10;
		if(asKey=="World_11") return eKey_World_11;
		if(asKey=="World_12") return eKey_World_12;
		if(asKey=="World_13") return eKey_World_13;
		if(asKey=="World_14") return eKey_World_14;
		if(asKey=="World_15") return eKey_World_15;
		if(asKey=="World_16") return eKey_World_16;
		if(asKey=="World_17") return eKey_World_17;
		if(asKey=="World_18") return eKey_World_18;
		if(asKey=="World_19") return eKey_World_19;
		if(asKey=="World_20") return eKey_World_20;
		if(asKey=="World_21") return eKey_World_21;
		if(asKey=="World_22") return eKey_World_22;
		if(asKey=="World_23") return eKey_World_23;
		if(asKey=="World_24") return eKey_World_24;
		if(asKey=="World_25") return eKey_World_25;
		if(asKey=="World_26") return eKey_World_26;
		if(asKey=="World_27") return eKey_World_27;
		if(asKey=="World_28") return eKey_World_28;
		if(asKey=="World_29") return eKey_World_29;
		if(asKey=="World_30") return eKey_World_30;
		if(asKey=="World_31") return eKey_World_31;
		if(asKey=="World_32") return eKey_World_32;
		if(asKey=="World_33") return eKey_World_33;
		if(asKey=="World_34") return eKey_World_34;
		if(asKey=="World_35") return eKey_World_35;
		if(asKey=="World_36") return eKey_World_36;
		if(asKey=="World_37") return eKey_World_37;
		if(asKey=="World_38") return eKey_World_38;
		if(asKey=="World_39") return eKey_World_39;
		if(asKey=="World_40") return eKey_World_40;
		if(asKey=="World_41") return eKey_World_41;
		if(asKey=="World_42") return eKey_World_42;
		if(asKey=="World_43") return eKey_World_43;
		if(asKey=="World_44") return eKey_World_44;
		if(asKey=="World_45") return eKey_World_45;
		if(asKey=="World_46") return eKey_World_46;
		if(asKey=="World_47") return eKey_World_47;
		if(asKey=="World_48") return eKey_World_48;
		if(asKey=="World_49") return eKey_World_49;
		if(asKey=="World_50") return eKey_World_50;
		if(asKey=="World_51") return eKey_World_51;
		if(asKey=="World_52") return eKey_World_52;
		if(asKey=="World_53") return eKey_World_53;
		if(asKey=="World_54") return eKey_World_54;
		if(asKey=="World_55") return eKey_World_55;
		if(asKey=="World_56") return eKey_World_56;
		if(asKey=="World_57") return eKey_World_57;
		if(asKey=="World_58") return eKey_World_58;
		if(asKey=="World_59") return eKey_World_59;
		if(asKey=="World_60") return eKey_World_60;
		if(asKey=="World_61") return eKey_World_61;
		if(asKey=="World_62") return eKey_World_62;
		if(asKey=="World_63") return eKey_World_63;
		if(asKey=="World_64") return eKey_World_64;
		if(asKey=="World_65") return eKey_World_65;
		if(asKey=="World_66") return eKey_World_66;
		if(asKey=="World_67") return eKey_World_67;
		if(asKey=="World_68") return eKey_World_68;
		if(asKey=="World_69") return eKey_World_69;
		if(asKey=="World_70") return eKey_World_70;
		if(asKey=="World_71") return eKey_World_71;
		if(asKey=="World_72") return eKey_World_72;
		if(asKey=="World_73") return eKey_World_73;
		if(asKey=="World_74") return eKey_World_74;
		if(asKey=="World_75") return eKey_World_75;
		if(asKey=="World_76") return eKey_World_76;
		if(asKey=="World_77") return eKey_World_77;
		if(asKey=="World_78") return eKey_World_78;
		if(asKey=="World_79") return eKey_World_79;
		if(asKey=="World_80") return eKey_World_80;
		if(asKey=="World_81") return eKey_World_81;
		if(asKey=="World_82") return eKey_World_82;
		if(asKey=="World_83") return eKey_World_83;
		if(asKey=="World_84") return eKey_World_84;
		if(asKey=="World_85") return eKey_World_85;
		if(asKey=="World_86") return eKey_World_86;
		if(asKey=="World_87") return eKey_World_87;
		if(asKey=="World_88") return eKey_World_88;
		if(asKey=="World_89") return eKey_World_89;
		if(asKey=="World_90") return eKey_World_90;
		if(asKey=="World_91") return eKey_World_91;
		if(asKey=="World_92") return eKey_World_92;
		if(asKey=="World_93") return eKey_World_93;
		if(asKey=="World_94") return eKey_World_94;
		if(asKey=="World_95") return eKey_World_95;
		return eKey_LastEnum;
	}

	//-----------------------------------------------------------------------
}
