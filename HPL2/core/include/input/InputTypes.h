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

#ifndef HPL_INPUT_TYPES_H
#define HPL_INPUT_TYPES_H

namespace hpl {

	//-------------------------------------------------

	enum eInputDeviceType {
		eInputDeviceType_Keyboard,
		eInputDeviceType_Mouse,
		eInputDeviceType_Gamepad,
		eInputDeviceType_LastEnum
	};

	//-------------------------------------------------

	enum eMouseButton {
		eMouseButton_Left,
		eMouseButton_Middle,
		eMouseButton_Right,
		eMouseButton_WheelUp,
		eMouseButton_WheelDown,
		eMouseButton_Button6,
		eMouseButton_Button7,
		eMouseButton_Button8,
		eMouseButton_Button9,
		eMouseButton_LastEnum
	};

	//-------------------------------------------------

	enum eKey {
		eKey_BackSpace,
		eKey_Tab,
		eKey_Clear,
		eKey_Return,
		eKey_Pause,
		eKey_Escape,
		eKey_Space,
		eKey_Exclaim,
		eKey_QuoteDouble,
		eKey_Hash,
		eKey_Dollar,
		eKey_Ampersand,
		eKey_Quote,
		eKey_LeftParen,
		eKey_RightParen,
		eKey_Asterisk,
		eKey_Plus,
		eKey_Comma,
		eKey_Minus,
		eKey_Period,
		eKey_Slash,
		eKey_0,
		eKey_1,
		eKey_2,
		eKey_3,
		eKey_4,
		eKey_5,
		eKey_6,
		eKey_7,
		eKey_8,
		eKey_9,
		eKey_Colon,
		eKey_SemiColon,
		eKey_Less,
		eKey_Equals,
		eKey_Greater,
		eKey_Question,
		eKey_At,
		eKey_LeftBracket,
		eKey_BackSlash,
		eKey_RightBracket,
		eKey_Caret,
		eKey_Underscore,
		eKey_BackQuote,
		eKey_A,
		eKey_B,
		eKey_C,
		eKey_D,
		eKey_E,
		eKey_F,
		eKey_G,
		eKey_H,
		eKey_I,
		eKey_J,
		eKey_K,
		eKey_L,
		eKey_M,
		eKey_N,
		eKey_O,
		eKey_P,
		eKey_Q,
		eKey_R,
		eKey_S,
		eKey_T,
		eKey_U,
		eKey_V,
		eKey_W,
		eKey_X,
		eKey_Y,
		eKey_Z,
		eKey_Delete,
		eKey_KP_0,
		eKey_KP_1,
		eKey_KP_2,
		eKey_KP_3,
		eKey_KP_4,
		eKey_KP_5,
		eKey_KP_6,
		eKey_KP_7,
		eKey_KP_8,
		eKey_KP_9,
		eKey_KP_Period,
		eKey_KP_Divide,
		eKey_KP_Multiply,
		eKey_KP_Minus,
		eKey_KP_Plus,
		eKey_KP_Enter,
		eKey_KP_Equals,
		eKey_Up,
		eKey_Down,
		eKey_Right,
		eKey_Left,
		eKey_Insert,
		eKey_Home,
		eKey_End,
		eKey_PageUp,
		eKey_PageDown,
		eKey_F1,
		eKey_F2,
		eKey_F3,
		eKey_F4,
		eKey_F5,
		eKey_F6,
		eKey_F7,
		eKey_F8,
		eKey_F9,
		eKey_F10,
		eKey_F11,
		eKey_F12,
		eKey_F13,
		eKey_F14,
		eKey_F15,
		eKey_NumLock,
		eKey_CapsLock,
		eKey_ScrollLock,
		eKey_RightShift,
		eKey_LeftShift,
		eKey_RightCtrl,
		eKey_LeftCtrl,
		eKey_RightAlt,
		eKey_LeftAlt,
		eKey_RightMeta,
		eKey_LeftMeta,
		eKey_LeftSuper,
		eKey_RightSuper,
		eKey_Mode,
		eKey_Help,
		eKey_Print,
		eKey_SysReq,
		eKey_Break,
		eKey_Menu,
		eKey_Power,
		eKey_Euro,
		eKey_None,
		eKey_World_0,
		eKey_World_1,
		eKey_World_2,
		eKey_World_3,
		eKey_World_4,
		eKey_World_5,
		eKey_World_6,
		eKey_World_7,
		eKey_World_8,
		eKey_World_9,
		eKey_World_10,
		eKey_World_11,
		eKey_World_12,
		eKey_World_13,
		eKey_World_14,
		eKey_World_15,
		eKey_World_16,
		eKey_World_17,
		eKey_World_18,
		eKey_World_19,
		eKey_World_20,
		eKey_World_21,
		eKey_World_22,
		eKey_World_23,
		eKey_World_24,
		eKey_World_25,
		eKey_World_26,
		eKey_World_27,
		eKey_World_28,
		eKey_World_29,
		eKey_World_30,
		eKey_World_31,
		eKey_World_32,
		eKey_World_33,
		eKey_World_34,
		eKey_World_35,
		eKey_World_36,
		eKey_World_37,
		eKey_World_38,
		eKey_World_39,
		eKey_World_40,
		eKey_World_41,
		eKey_World_42,
		eKey_World_43,
		eKey_World_44,
		eKey_World_45,
		eKey_World_46,
		eKey_World_47,
		eKey_World_48,
		eKey_World_49,
		eKey_World_50,
		eKey_World_51,
		eKey_World_52,
		eKey_World_53,
		eKey_World_54,
		eKey_World_55,
		eKey_World_56,
		eKey_World_57,
		eKey_World_58,
		eKey_World_59,
		eKey_World_60,
		eKey_World_61,
		eKey_World_62,
		eKey_World_63,
		eKey_World_64,
		eKey_World_65,
		eKey_World_66,
		eKey_World_67,
		eKey_World_68,
		eKey_World_69,
		eKey_World_70,
		eKey_World_71,
		eKey_World_72,
		eKey_World_73,
		eKey_World_74,
		eKey_World_75,
		eKey_World_76,
		eKey_World_77,
		eKey_World_78,
		eKey_World_79,
		eKey_World_80,
		eKey_World_81,
		eKey_World_82,
		eKey_World_83,
		eKey_World_84,
		eKey_World_85,
		eKey_World_86,
		eKey_World_87,
		eKey_World_88,
		eKey_World_89,
		eKey_World_90,
		eKey_World_91,
		eKey_World_92,
		eKey_World_93,
		eKey_World_94,
		eKey_World_95,
		eKey_LastEnum
	};

	//-------------------------------------------------

	enum eKeyModifier {
		eKeyModifier_None	= 0x00000,
		eKeyModifier_Ctrl	= 0x00001,
		eKeyModifier_Shift	= 0x00002,
		eKeyModifier_Alt	= 0x00004,
		eKeyModifier_LastEnum = 4
	};

    //-------------------------------------------------

	struct cKeyPress
	{
		cKeyPress(){}
		cKeyPress(eKey aKey, int alUnicode,int alModifier) 
			: mKey(aKey), mlUnicode(alUnicode), mlModifier(alModifier) {}

		eKey mKey;
		int mlUnicode;
		int mlModifier;
	};

	//-------------------------------------------------

	enum eGamepadInputType
	{
		eGamepadInputType_Button,
		eGamepadInputType_Hat,
		eGamepadInputType_Axis,
		eGamepadInputType_Ball,

		eGamepadInputType_LastEnum
	};

	enum eGamepadButton
	{
		eGamepadButton_0,
		eGamepadButton_1,
		eGamepadButton_2,
		eGamepadButton_3,
		eGamepadButton_4,
		eGamepadButton_5,
		eGamepadButton_6,
		eGamepadButton_7,
		eGamepadButton_8,
		eGamepadButton_9,
		eGamepadButton_10,
		eGamepadButton_11,
		eGamepadButton_12,
		eGamepadButton_13,
		eGamepadButton_14,
		eGamepadButton_15,
		eGamepadButton_16,
		eGamepadButton_17,
		eGamepadButton_18,
		eGamepadButton_19,
		eGamepadButton_20,
		eGamepadButton_21,
		eGamepadButton_22,
		eGamepadButton_23,
		eGamepadButton_24,
		eGamepadButton_25,
		eGamepadButton_26,
		eGamepadButton_27,
		eGamepadButton_28,
		eGamepadButton_29,
		
		eGamepadButton_LastEnum,
        // Pretty symbols for SDL2
        eGamepadButton_A = eGamepadButton_0,
        eGamepadButton_B = eGamepadButton_1,
        eGamepadButton_X = eGamepadButton_2,
        eGamepadButton_Y = eGamepadButton_3,
        eGamepadButton_Back = eGamepadButton_4,
        eGamepadButton_Guide = eGamepadButton_5,
        eGamepadButton_Start = eGamepadButton_6,
        eGamepadButton_LeftStick = eGamepadButton_7,
        eGamepadButton_RightStick = eGamepadButton_8,
        eGamepadButton_LeftShoulder = eGamepadButton_9,
        eGamepadButton_RightShoulder = eGamepadButton_10,
        eGamepadButton_DpadUp = eGamepadButton_11,
        eGamepadButton_DpadDown = eGamepadButton_12,
        eGamepadButton_DpadLeft = eGamepadButton_13,
        eGamepadButton_DpadRight = eGamepadButton_14
	};

	enum eGamepadAxis
	{
		eGamepadAxis_0,
		eGamepadAxis_1,
		eGamepadAxis_2,
		eGamepadAxis_3,
		eGamepadAxis_4,
		eGamepadAxis_5,
		eGamepadAxis_6,
		eGamepadAxis_7,
		eGamepadAxis_8,
		eGamepadAxis_9,
				
		eGamepadAxis_LastEnum,
        // Pretty symbosl for SDL2
        eGamepadAxis_LeftX = eGamepadAxis_0,
        eGamepadAxis_LeftY = eGamepadAxis_1,
        eGamepadAxis_RightX = eGamepadAxis_2,
        eGamepadAxis_RightY = eGamepadAxis_3,
        eGamepadAxis_LeftTrigger = eGamepadAxis_4,
        eGamepadAxis_RightTrigger = eGamepadAxis_5
	};

	enum eGamepadAxisRange
	{
		eGamepadAxisRange_Dead,
		eGamepadAxisRange_Positive,
		eGamepadAxisRange_Negative,
		
		eGamepadAxisRange_LastEnum
	};

	enum eGamepadHat
	{
		eGamepadHat_0,
		eGamepadHat_1,
		eGamepadHat_2,
		eGamepadHat_3,
		eGamepadHat_4,
		eGamepadHat_5,
		eGamepadHat_6,
		eGamepadHat_7,
		eGamepadHat_8,
		eGamepadHat_9,

		eGamepadHat_LastEnum
	};

	enum eGamepadHatState
	{
		eGamepadHatState_Centered	= 0,
		eGamepadHatState_Up			= 0x1,
		eGamepadHatState_Right		= 0x2,
		eGamepadHatState_Down		= 0x4,
		eGamepadHatState_Left		= 0x8,
		eGamepadHatState_RightUp	= eGamepadHatState_Up | eGamepadHatState_Right,
		eGamepadHatState_RightDown	= eGamepadHatState_Down | eGamepadHatState_Right,
		eGamepadHatState_LeftUp		= eGamepadHatState_Up | eGamepadHatState_Left,
		eGamepadHatState_LeftDown	= eGamepadHatState_Down | eGamepadHatState_Left,

		eGamepadHatState_LastEnum	= 0x10
	};

	enum eGamepadBall
	{
		eGamepadBall_0,
		
		eGamepadBall_LastEnum
	};

	//-------------------------------------------------

	struct cGamepadInputData
	{
		cGamepadInputData() : mlPadIndex(0), mInputType(eGamepadInputType_LastEnum), mlInputId(0), mfInputValue(0) {}
		cGamepadInputData(int alPadIndex, eGamepadInputType aType, int alInputId, float afInputValue) 
			: mlPadIndex(alPadIndex), mInputType(aType), mlInputId(alInputId), mfInputValue(afInputValue) {}

		bool operator==(const cGamepadInputData& aData)
		{
			return mlPadIndex==aData.mlPadIndex && mInputType==aData.mInputType && 
				mlInputId==aData.mlInputId && mfInputValue==aData.mfInputValue;
		}

		int mlPadIndex;
		eGamepadInputType mInputType;
		int mlInputId;
		float mfInputValue;
	};

	//-------------------------------------------------

};

#endif // HPL_INPUT_TYPES_H
