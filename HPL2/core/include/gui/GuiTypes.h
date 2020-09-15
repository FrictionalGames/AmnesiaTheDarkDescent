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

#ifndef HPL_GUI_TYPES_H
#define HPL_GUI_TYPES_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "input/InputTypes.h"
#include "system/SystemTypes.h"


namespace hpl {
	//--------------------------------

	enum eGuiWindowZDest
	{
		eGuiWindowZDest_Regular,
		eGuiWindowZDest_RegularOnTop,
		eGuiWindowZDest_MessageBox,
		eGuiWindowZDest_FilePicker,
		eGuiWindowZDest_ContextMenu,
		eGuiWindowZDest_LastEnum,
	};
	
	//--------------------------------
	
	enum eGuiMouseButton
	{
		eGuiMouseButton_Left =		0x00000001,
		eGuiMouseButton_Middle =	0x00000002,
		eGuiMouseButton_Right	=	0x00000004,
		eGuiMouseButton_WheelUp =	0x00000008,
		eGuiMouseButton_WheelDown = 0x00000010,
		eGuiMouseButton_Button6	  = 0x00000020,
		eGuiMouseButton_Button7	  = 0x00000040,
		eGuiMouseButton_Button8	  = 0x00000080,
		eGuiMouseButton_Button9	  = 0x00000100,

		eGuiMouseButton_LastEnum  = 0x00000200
	};

	//--------------------------------

	enum eUIArrow
	{
		eUIArrow_Up			= eFlagBit_0,
		eUIArrow_Right		= eFlagBit_1,
		eUIArrow_Down		= eFlagBit_2,
		eUIArrow_Left		= eFlagBit_3,
		eUIArrow_LeftUp		= eUIArrow_Left | eUIArrow_Up,
		eUIArrow_LeftDown	= eUIArrow_Left | eUIArrow_Down,
		eUIArrow_RightUp	= eUIArrow_Right | eUIArrow_Up,
		eUIArrow_RightDown	= eUIArrow_Right | eUIArrow_Down,

		eUIArrow_LastEnum	= eFlagBit_None
	};

	enum eUIButton
	{
		eUIButton_Primary		= eFlagBit_0,
		eUIButton_Secondary		= eFlagBit_1,

		eUIButton_PrevPage		= eFlagBit_2,
		eUIButton_NextPage		= eFlagBit_3,

		eUIButton_Interact		= eFlagBit_4,
		eUIButton_Delete		= eFlagBit_5,
		eUIButton_Clear			= eFlagBit_6,

		eUIButton_LastEnum		= eFlagBit_None
	};

	//--------------------------------

	enum eGuiMaterial
	{
		eGuiMaterial_Diffuse,
		eGuiMaterial_Alpha,
		eGuiMaterial_FontNormal,
		eGuiMaterial_Additive,
		eGuiMaterial_Modulative,
		eGuiMaterial_PremulAlpha,
		eGuiMaterial_LastEnum,
	};
	
	//--------------------------------
	
	enum eWidgetType
	{
		eWidgetType_Root,
		eWidgetType_Window,
		eWidgetType_Button,
		eWidgetType_Frame,
		eWidgetType_Label,
		eWidgetType_Slider,
		eWidgetType_TextBox,
		eWidgetType_CheckBox,
		eWidgetType_Image,
		eWidgetType_ListBox,
		eWidgetType_ComboBox,
		eWidgetType_ContextMenu,
		eWidgetType_MainMenu,
		eWidgetType_MenuItem,
		eWidgetType_Tab,
		eWidgetType_TabLabel,
		eWidgetType_TabFrame,
		eWidgetType_Group,

		eWidgetType_User,
		eWidgetType_LastEnum,
	};

	//--------------------------------

	enum eListBoxActionType
	{
		eListBoxActionType_SingleSelect,
		eListBoxActionType_SingleDeselect,
		eListBoxActionType_Multiple,

		eListBoxActionType_LastEnum
	};

	//--------------------------------

	enum eFilePickerType
	{
		eFilePickerType_Save,
		eFilePickerType_Load,

		eFilePickerType_LastEnum
	};

	//--------------------------------
	
	enum eGuiSkinFont
	{
		eGuiSkinFont_Default,
		eGuiSkinFont_Disabled,
		eGuiSkinFont_WindowLabel,

		eGuiSkinFont_LastEnum
	};

	//--------------------------------


	enum eGuiSkinAttribute
	{
		eGuiSkinAttribute_WindowLabelTextOffset,
		eGuiSkinAttribute_WindowButtonSize,
		eGuiSkinAttribute_WindowBordersOutside,

		eGuiSkinAttribute_ButtonPressedContentOffset,

		eGuiSkinAttribute_SliderButtonSize,

		eGuiSkinAttribute_ListBoxSliderWidth,

		eGuiSkinAttribute_ComboBoxButtonWidth,
		eGuiSkinAttribute_ComboBoxSliderWidth,

		eGuiSkinAttribute_ContextMenuItemArrowSize,
		eGuiSkinAttribute_ContextMenuItemCheckSize,
		eGuiSkinAttribute_ContextMenuItemHPadding,
		eGuiSkinAttribute_ContextMenuItemVPadding,
		eGuiSkinAttribute_ContextMenuItemSeparation,
		eGuiSkinAttribute_ContextMenuItemTextLeftPadding,
		eGuiSkinAttribute_ContextMenuItemTextRightPadding,
		eGuiSkinAttribute_ContextMenuSeparatorPadding,
		
		eGuiSkinAttribute_MainMenuItemTextPadding,
		eGuiSkinAttribute_MainMenuItemVPadding,
		eGuiSkinAttribute_MainMenuItemSeparation,

		eGuiSkinAttribute_MenuSeparatorPadding,
		eGuiSkinAttribute_MenuSeparatorHeight,

		eGuiSkinAttribute_DefaultFontHighlightColor,

		eGuiSkinAttribute_TabFrameButtonSize,
		eGuiSkinAttribute_TabFrameIconPadding,
		eGuiSkinAttribute_TabFrameTextLeftPadding,
		eGuiSkinAttribute_TabFrameTextRightPadding,
		eGuiSkinAttribute_TabFrameLabelInactiveOffset,
		eGuiSkinAttribute_TabFrameLabelActiveOffset,
		eGuiSkinAttribute_FirstTabOffset,
		eGuiSkinAttribute_TabSeparation,

		eGuiSkinAttribute_GroupHeaderOffset,
		eGuiSkinAttribute_GroupHeaderOverUpperBorder,

		eGuiSkinAttribute_LastEnum
	};
	//--------------------------------
		
	enum eGuiSkinGfx
	{
		///////////////////////////////////
		// Pointer
		eGuiSkinGfx_PointerNormal,
		eGuiSkinGfx_PointerText,

		///////////////////////////////////
		// Window
		eGuiSkinGfx_WindowBorderRight,
		eGuiSkinGfx_WindowBorderLeft,
		eGuiSkinGfx_WindowBorderUp,
		eGuiSkinGfx_WindowBorderDown,

		eGuiSkinGfx_WindowCornerLU,
		eGuiSkinGfx_WindowCornerRU,
		eGuiSkinGfx_WindowCornerRD,
		eGuiSkinGfx_WindowCornerLD,

		eGuiSkinGfx_WindowLabel,
		eGuiSkinGfx_WindowBackground,

		eGuiSkinGfx_WindowButtonCross,

		///////////////////////////////////
		// Frame
		eGuiSkinGfx_FrameBorderRight,
		eGuiSkinGfx_FrameBorderLeft,
		eGuiSkinGfx_FrameBorderUp,
		eGuiSkinGfx_FrameBorderDown,

		eGuiSkinGfx_FrameCornerLU,
		eGuiSkinGfx_FrameCornerRU,
		eGuiSkinGfx_FrameCornerRD,
		eGuiSkinGfx_FrameCornerLD,

		eGuiSkinGfx_FrameBackground,

		///////////////////////////////////
		// Check Box
		eGuiSkinGfx_CheckBoxEnabledUnchecked,
		eGuiSkinGfx_CheckBoxEnabledChecked,
		eGuiSkinGfx_CheckBoxDisabledUnchecked,
		eGuiSkinGfx_CheckBoxDisabledChecked,


		///////////////////////////////////
		// Text Box
		eGuiSkinGfx_TextBoxBackground,
		eGuiSkinGfx_TextBoxSelectedTextBack,
		eGuiSkinGfx_TextBoxMarker,

		///////////////////////////////////
		// List Box
		eGuiSkinGfx_ListBoxBackground,

		///////////////////////////////////
		// Combo Box
		eGuiSkinGfx_ComboBoxButtonIcon,

		eGuiSkinGfx_ComboBoxBorderRight,
		eGuiSkinGfx_ComboBoxBorderLeft,
		eGuiSkinGfx_ComboBoxBorderUp,
		eGuiSkinGfx_ComboBoxBorderDown,

		eGuiSkinGfx_ComboBoxCornerLU,
		eGuiSkinGfx_ComboBoxCornerRU,
		eGuiSkinGfx_ComboBoxCornerRD,
		eGuiSkinGfx_ComboBoxCornerLD,

		eGuiSkinGfx_ComboBoxBackground,


		///////////////////////////////////
		// Slider
		eGuiSkinGfx_SliderHandleBorderRight,
		eGuiSkinGfx_SliderHandleBorderLeft,
		eGuiSkinGfx_SliderHandleBorderUp,
		eGuiSkinGfx_SliderHandleBorderDown,

		eGuiSkinGfx_SliderHandleCornerLU,
		eGuiSkinGfx_SliderHandleCornerRU,
		eGuiSkinGfx_SliderHandleCornerRD,
		eGuiSkinGfx_SliderHandleCornerLD,

		eGuiSkinGfx_SliderHandleBackground,

		eGuiSkinGfx_SliderVertArrowUp,
		eGuiSkinGfx_SliderVertArrowDown,
		eGuiSkinGfx_SliderVertBackground,

		eGuiSkinGfx_SliderHoriArrowUp,
		eGuiSkinGfx_SliderHoriArrowDown,
		eGuiSkinGfx_SliderHoriBackground,

		///////////////////////////////////
		// Button
		eGuiSkinGfx_ButtonUpBorderRight,
		eGuiSkinGfx_ButtonUpBorderLeft,
		eGuiSkinGfx_ButtonUpBorderUp,
		eGuiSkinGfx_ButtonUpBorderDown,

		eGuiSkinGfx_ButtonUpCornerLU,
		eGuiSkinGfx_ButtonUpCornerRU,
		eGuiSkinGfx_ButtonUpCornerRD,
		eGuiSkinGfx_ButtonUpCornerLD,

		eGuiSkinGfx_ButtonUpBackground,

		eGuiSkinGfx_ButtonDownBorderRight,
		eGuiSkinGfx_ButtonDownBorderLeft,
		eGuiSkinGfx_ButtonDownBorderUp,
		eGuiSkinGfx_ButtonDownBorderDown,

		eGuiSkinGfx_ButtonDownCornerLU,
		eGuiSkinGfx_ButtonDownCornerRU,
		eGuiSkinGfx_ButtonDownCornerRD,
		eGuiSkinGfx_ButtonDownCornerLD,

		eGuiSkinGfx_ButtonDownBackground,

		///////////////////////////////////
		// Menu
		eGuiSkinGfx_MenuEnabledChecked,
		eGuiSkinGfx_MenuDisabledChecked,
		eGuiSkinGfx_MenuSeparatorLine,
		eGuiSkinGfx_MenuItemArrow,

		///////////////////////////////////
		// Tab Frame
		eGuiSkinGfx_TabLabelBorderRight,
		eGuiSkinGfx_TabLabelBorderLeft,
		eGuiSkinGfx_TabLabelBorderUp,
		eGuiSkinGfx_TabLabelBorderDown,

		eGuiSkinGfx_TabLabelCornerLU,
		eGuiSkinGfx_TabLabelCornerRU,
		eGuiSkinGfx_TabLabelCornerRD,
		eGuiSkinGfx_TabLabelCornerLD,
		
		eGuiSkinGfx_TabBorderRight,
		eGuiSkinGfx_TabBorderLeft,
		eGuiSkinGfx_TabBorderUp,
		eGuiSkinGfx_TabBorderDown,

		eGuiSkinGfx_TabCornerLU,
		eGuiSkinGfx_TabCornerRU,
		eGuiSkinGfx_TabCornerRD,
		eGuiSkinGfx_TabCornerLD,

		eGuiSkinGfx_TabLabelBackground,
		eGuiSkinGfx_TabLabelBackgroundActive,
		eGuiSkinGfx_TabBackground,
		eGuiSkinGfx_TabFrameArrowLeft,
		eGuiSkinGfx_TabFrameArrowRight,

		//////////////////////////////////////
		// Group
		eGuiSkinGfx_GroupBorderLeft,
		eGuiSkinGfx_GroupBorderRight,
		eGuiSkinGfx_GroupBorderUp,
		eGuiSkinGfx_GroupBorderDown,

		eGuiSkinGfx_GroupCornerLU,
		eGuiSkinGfx_GroupCornerRU,
		eGuiSkinGfx_GroupCornerRD,
		eGuiSkinGfx_GroupCornerLD,

		eGuiSkinGfx_GroupBackground,

		///////////////////////////////////////
		// File Picker
		eGuiSkinGfx_FilePickerUpButton,

		eGuiSkinGfx_FilePickerIconGraphics,
		eGuiSkinGfx_FilePickerIconModels,
		eGuiSkinGfx_FilePickerIconSounds,
		eGuiSkinGfx_FilePickerIconText,
		eGuiSkinGfx_FilePickerIconOther,
		eGuiSkinGfx_FilePickerIconFolder,


		eGuiSkinGfx_LastEnum,
	};

	//---------------------------------------------

	enum eWidgetSliderOrientation
	{
		eWidgetSliderOrientation_Horizontal,
		eWidgetSliderOrientation_Vertical,
		eWidgetSliderOrientation_LastEnum
	};

	//--------------------------------

	enum eWidgetTextBoxInputType
	{
		eWidgetTextBoxInputType_Normal,
		eWidgetTextBoxInputType_Numeric,
		eWidgetTextBoxInputType_LastEnum
	};
	
	//--------------------------------

	enum eGuiMessage
	{
		eGuiMessage_MouseMove,			//pos= mouse pos, lVal=buttons down
		eGuiMessage_MouseDown,			//pos= mouse pos, lVal=button
		eGuiMessage_MouseUp,			//pos= mouse pos, lVal=button
		eGuiMessage_MouseDoubleClick,	//pos= mouse pos, lVal=button
		eGuiMessage_MouseEnter,			//pos= mouse pos, lVal=buttons down
		eGuiMessage_MouseLeave,			//pos= mouse pos, lVal=buttons down

		eGuiMessage_GotFocus,			//pos= mouse pos, lVal=buttons down
		eGuiMessage_GotTabFocus,
		eGuiMessage_LostFocus,			//pos= mouse pos, lVal=buttons down

		eGuiMessage_OnDraw,				//fVal = TimeStep, data = clipregion pointer

		eGuiMessage_OnUpdate,			//fVal = TimeStep

		eGuiMessage_ButtonPressed,		//pos= mouse pos, lVal=buttons down

		eGuiMessage_TextChange,			//nothing

		eGuiMessage_CheckChange,		//lVal = checked or not.

		eGuiMessage_KeyPress,			//pos= mouse pos, lVal=char code.
		eGuiMessage_KeyRelease,			//pos= mouse pos, lVal=char code.

		eGuiMessage_GamepadInput,
		
		eGuiMessage_UIArrowPress,		//mlVal = arrow dir
		eGuiMessage_UIArrowRelease,
		eGuiMessage_UIButtonPress,		//mlVal = button
		eGuiMessage_UIButtonRelease,
		eGuiMessage_UIButtonDoublePress,

		eGuiMessage_GetUINavFocus,
		eGuiMessage_LoseUINavFocus,
		
		eGuiMessage_SliderMove,			//val=slider value
		eGuiMessage_SliderRelease,		//val=slider value

		eGuiMessage_SelectionChange,	//val=selected val

		eGuiMessage_SelectionDoubleClick,	// pos = mouse pos, val=mouse button

		eGuiMessage_TextBoxEnter,		//nothing
		eGuiMessage_TextBoxValueUp,
		eGuiMessage_TextBoxValueDown,

		eGuiMessage_WindowClose,		//nothing

		eGuiMessage_LastEnum,
	};


	//--------------------------------

	
	struct cGuiMessageData
	{
		cGuiMessageData(){}
		cGuiMessageData(const cVector2f& avPos, const cVector2f& avRel){
			mvPos = avPos;
            mvRel = avRel;
		}
		cGuiMessageData(const cVector2f& avPos,const cVector2f& avRel, int alVal){
			mvPos = avPos;
			mvRel = avRel;
			mlVal = alVal;
		}
		cGuiMessageData(int alVal){
			mlVal = alVal;
		}
		cGuiMessageData(float afVal){
			mfVal = afVal;
		}
		cGuiMessageData(const cKeyPress& aKeyPress){
			mKeyPress = aKeyPress;
		}
		cGuiMessageData(const cGamepadInputData& aGamepadInput)
			: mGamepadInputData(aGamepadInput){}

		cVector2f			mvPos;
		cVector2f			mvRel;
		int					mlVal;
		cKeyPress			mKeyPress;
		cGamepadInputData	mGamepadInputData;
		float				mfVal;
		void*				mpData;
		eGuiMessage			mMessage;
	};

	//--------------------------------


	typedef tFlag tWidgetWindowButtonFlag;
	
	#define eWidgetWindowButtonFlag_None			(0x00000000)
	#define eWidgetWindowButtonFlag_ButtonClose		(0x00000001)

	
	//--------------------------------
	
	
	class iWidget;

	typedef bool (*tGuiCallbackFunc)(void *,iWidget*, const cGuiMessageData&);

	#define kGuiCallbackDeclarationEnd(FuncName) \
		static bool FuncName##_static_gui(void *apObject,iWidget* apWidget,const cGuiMessageData& aData);

	#define kGuiCallbackDeclaredFuncEnd(ThisClass,FuncName) \
		bool ThisClass::FuncName##_static_gui(void *apObject,iWidget* apWidget,const cGuiMessageData& aData) \
		{\
			return ((ThisClass*)apObject)->FuncName(apWidget, aData); \
		}
		
	#define kGuiCallbackFuncEnd(ThisClass,FuncName) \
		static bool FuncName##_static_gui(void *apObject,iWidget* apWidget,const cGuiMessageData& aData) \
		{ \
			return ((ThisClass*)apObject)->FuncName(apWidget, aData); \
		} 

	#define kGuiCallback(FuncName) & FuncName##_static_gui
	
	//--------------------------------
	
	typedef std::list<iWidget*> tWidgetList;
	typedef tWidgetList::iterator tWidgetListIt;

	//--------------------------------
	
	typedef std::vector<iWidget*>	tWidgetVec;
	typedef tWidgetVec::iterator	tWidgetVecIt;

	//--------------------------------

	class cGuiGfxElement;
	class cGuiSet;
	
	typedef std::list<cGuiGfxElement*> tGuiGfxElementList;
	typedef tGuiGfxElementList::iterator tGuiGfxElementListIt;

	typedef std::list<cGuiSet*> tGuiSetList;
	typedef tGuiSetList::iterator tGuiSetListIt;

	typedef std::multimap<tString, cGuiSet*> tGuiSetMap;
	typedef tGuiSetMap::iterator tGuiSetMapIt;

	//--------------------------------

	typedef cSTLIterator<cGuiSet*, tGuiSetList, tGuiSetListIt> cGuiSetListIterator;


	//--------------------------------


};

#endif // HPL_GAME_TYPES_H
