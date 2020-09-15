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

#ifndef HPL_WIDGET_TEXT_BOX_H
#define HPL_WIDGET_TEXT_BOX_H

#include "gui/Widget.h"

namespace hpl {

	class cGuiSkinFont;
	
	class cWidgetButton;
	class cGuiPopUpUIKeyboard;

	class cWidgetTextBox : public iWidget
	{
		friend class cGuiPopUpUIKeyboard;
	public:
		cWidgetTextBox(cGuiSet *apSet, cGuiSkin *apSkin, eWidgetTextBoxInputType aType);
		virtual ~cWidgetTextBox();

		void SetDefaultFontSize(const cVector2f& avSize);

		void SetCallbackOnLostFocus(bool abX) { mbCallbackOnLostFocus = abX; }
		bool GetCallbackOnLostFocus() { return mbCallbackOnLostFocus; }

		void SetForceCallBackOnEnter(bool abX) { mbForceCallBackOnEnter=abX; }
		bool GetForceCallBackOnEnter() { return mbForceCallBackOnEnter; }
		
		void SetMaxTextLength(int alLength);
		int GetMaxTextLength(){return mlMaxCharacters;}

		void SetSelectedText(int alStart=0, int alCount=-1);

		cVector2f GetBackgroundSize();

		void SetMaxTextSizeNeg(float afX);

		void SetCanEdit(bool abX);
		bool GetCanEdit(){return mbCanEdit;}

		void SetShowButtons(bool abX) { mbShowButtons = abX; }

		void SetNumericAdd(float afX) { mfNumericAdd = afX; }
		float GetNumericAdd() { return mfNumericAdd; }

		void SetNumericValue(float afX);
		float GetNumericValue();

		void SetLowerBound(bool abX, float afValue=0);
		float GetLowerBound() { return mfLowerBound; }
		void SetUpperBound(bool abX, float afValue=0);
		float GetUpperBound() { return mfUpperBound; }

		void SetDecimals(int alX) { mlDecimals = alX; }
		int GetDecimals();

		static void SetDefaultDecimals(int alX) { mlDefaultDecimals = alX; }

		void SetIllegalChars(const tWString &asIllegalChars);
		const tWString& GetIllegalChars();

		void SetLegalCharCodeLimitEnabled(bool abX);
		
		/**
		* A character that is lower (NOT equal) to this is considered illegal
		*/
		void SetLegalCharCodeMinLimit(wchar_t alX);
		/**
		* A character that is higher (NOT equal) to this is considered illegal
		*/
		void SetLegalCharCodeMaxLimit(wchar_t alX);

		bool GetLegalCharCodeLimitEnabled(){ return mbLegalCharCodeLimitEnabled;}
		wchar_t GetLegalCharCodeMinLimit(){ return mlLegalCharCodeMinLimit;}
		wchar_t GetLegalCharCodeMaxLimit(){ return mlLegalCharCodeMaxLimit;}
		
	protected:
		/////////////////////////
		// Own functions
		bool Widget_OnValueUp(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(Widget_OnValueUp);
		bool Widget_OnValueDown(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(Widget_OnValueDown);
		bool ButtonPressed(iWidget* apWidget, const cGuiMessageData& aData);
		kGuiCallbackDeclarationEnd(ButtonPressed);

		int WorldToCharPos(const cVector2f &avWorldPos);
		float CharToLocalPos(int alChar);
		void SetMarkerPos(int alPos);
		int GetLastCharInSize(int alStartPos, float afMaxSize, float afLengthAdd);
		int GetFirstCharInSize(int alStartPos, float afMaxSize, float afLengthAdd);
		bool WidgetConsiderSomeCharsIllegal();
		bool IsIllegalChar(wchar_t alChar);
		void SetTextUpdated();

		/////////////////////////
		// Implemented functions
		void OnInit();
		void OnLoadGraphics();
		void OnChangeSize();
		void OnChangeText();
		
		void OnDraw(float afTimeStep, cGuiClipRegion *apClipRegion);

		bool OnMouseMove(const cGuiMessageData& aData);
		bool OnMouseDown(const cGuiMessageData& aData);
		bool OnMouseDoubleClick(const cGuiMessageData& aData);
		bool OnMouseUp(const cGuiMessageData& aData);
		bool OnMouseEnter(const cGuiMessageData& aData);
		bool OnMouseLeave(const cGuiMessageData& aData);

		bool OnGotFocus(const cGuiMessageData& aData);
		bool OnGotTabFocus(const cGuiMessageData& aData);
		bool OnLostFocus(const cGuiMessageData& aData);

		bool OnKeyPress(const cGuiMessageData& aData);

		bool OnUIButtonPress(const cGuiMessageData& aData);

		bool OnEnter(const cGuiMessageData& aData);


		////////////////////////////
		// Interface for UI keyboard
		void OnUIKeyboardOpen();
		void OnUIKeyboardClose();

		/////////////////////////
		// Data
		iWidget* mpPrevAttention;
		bool mbPressed;
		int mlMarkerCharPos;
		int mlSelectedTextEnd;
		int mlFirstVisibleChar;
		int mlVisibleCharSize;
		float mfTextMaxSize;

		int mlMaxCharacters;

		float mfMaxTextSizeNeg;

		bool mbCanEdit;

		eWidgetTextBoxInputType mInputType;
		bool mbShowButtons;
		float mfNumericAdd;
		float mfNumericValue;
		bool mbNumericValueUpdated;

		bool mbHasLowerBound;
		float mfLowerBound;

		bool mbHasUpperBound;
		float mfUpperBound;

		int mlDecimals;
		static int mlDefaultDecimals;

		tWString msIllegalChars;
		bool mbLegalCharCodeLimitEnabled;
		wchar_t mlLegalCharCodeMaxLimit;
		wchar_t mlLegalCharCodeMinLimit;

		bool mbChangedSinceLastEnter;
		bool mbForceCallBackOnEnter;
		bool mbCallbackOnLostFocus;

		cWidgetButton *mvButtons[2];
		
		cGuiGfxElement *mpGfxMarker;
		cGuiGfxElement *mpGfxSelectedTextBack;

		cGuiGfxElement *mpGfxBackground;
		
		cGuiGfxElement *mvGfxBorders[4];
		cGuiGfxElement *mvGfxCorners[4];

		bool mbGotFocusRecently;

		bool mbUIKeyboardOpen;
	};

};
#endif // HPL_WIDGET_TEXT_BOX_H
