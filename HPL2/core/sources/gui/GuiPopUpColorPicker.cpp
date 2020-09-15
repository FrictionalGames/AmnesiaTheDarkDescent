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

#include "gui/GuiPopUpColorPicker.h"

#include "gui/GuiTypes.h"

#include "system/LowLevelSystem.h"
#include "system/Platform.h"
#include "system/String.h"

#include "math/Math.h"

#include "graphics/FontData.h"

#include "gui/Gui.h"
#include "gui/GuiSkin.h"
#include "gui/GuiSet.h"

#include "gui/WidgetTextBox.h"
#include "gui/WidgetWindow.h"
#include "gui/WidgetButton.h"
#include "gui/WidgetFrame.h"
#include "gui/WidgetSlider.h"
#include "gui/WidgetLabel.h"

namespace hpl {

	/////////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	/////////////////////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------------------

	cGuiPopUpColorPicker::cGuiPopUpColorPicker(cGuiSet* apSet, 
											   cGuiSkin* apSkin,
											   const cVector3f& avPos, 
											   cColor* apDestColor, 
											   void *apCallbackObject, tGuiCallbackFunc apCallback) : iGuiPopUp(apSet, true, cVector2f(350,250))
	{
		//mvPos = avPos;
		mpCallbackObject = apCallbackObject;
		mpCallback = apCallback;

		mpDestColor = apDestColor;

		if(mpDestColor)
			mColor = *mpDestColor;
		else
			mColor = cColor(1);

		Init();
	}

    //-------------------------------------------------------------------------------

	cGuiPopUpColorPicker::~cGuiPopUpColorPicker()
	{
		for(int i=0;i<4;++i)
		{
			if(i<3 && mvButtons[i]) mpSet->DestroyWidget(mvButtons[i]);
			if(mvSliderColorComponent[i]) mpSet->DestroyWidget(mvSliderColorComponent[i]);
			if(mvInputColorComponent[i]) mpSet->DestroyWidget(mvInputColorComponent[i]);
		}
				
		if(mpFrameCurrentColor) mpSet->DestroyWidget(mpFrameCurrentColor);
		if(mpFramePreviousColor) mpSet->DestroyWidget(mpFramePreviousColor);
	}

	//-------------------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	/////////////////////////////////////////////////////////////////////////////////


	//-------------------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////////////
	// PROTECTED METHODS
	/////////////////////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------------------------

	bool cGuiPopUpColorPicker::Button_Pressed(iWidget* apWidget, const cGuiMessageData &aData)
	{
		/////////////////////////////
		// Color picking tool ???
		if(apWidget == mvButtons[0])
		{

		}
		/////////////////////////////
		// Accept button
		if(apWidget == mvButtons[1])
		{
			ClosePopUp();
		}
		/////////////////////////////
		// Cancel button
		if(apWidget == mvButtons[2])
		{
			SelfDestruct();
		}
		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpColorPicker, Button_Pressed);

	//-------------------------------------------------------------------------------

	bool cGuiPopUpColorPicker::Slider_OnMove(iWidget* apWidget, const cGuiMessageData& aData)
	{
		int i=0;
		for(;i<4;++i)	if(mvSliderColorComponent[i]==apWidget)break;

		mColor.v[i] = mvSliderColorComponent[i]->GetValue()*0.001f;

		UpdateInputs();

		mpFrameCurrentColor->SetBackGroundColor(cColor(mColor.r,mColor.g,mColor.b,1));

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpColorPicker, Slider_OnMove);

	//-------------------------------------------------------------------------------

	bool cGuiPopUpColorPicker::Input_OnTextBoxEnter(iWidget* apWidget, const cGuiMessageData& aData)
	{
		int i=0;
		for(;i<4;++i)	if(mvInputColorComponent[i]==apWidget)break;

		mColor.v[i] = mvInputColorComponent[i]->GetNumericValue();

		UpdateSliders();

		mpFrameCurrentColor->SetBackGroundColor(cColor(mColor.r,mColor.g,mColor.b,1));

		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpColorPicker, Input_OnTextBoxEnter);


	//-------------------------------------------------------------------------------

	bool cGuiPopUpColorPicker::WindowClose(iWidget* apWidget, const cGuiMessageData& aData)
	{
		SelfDestruct();
		return true;
	}
	kGuiCallbackDeclaredFuncEnd(cGuiPopUpColorPicker, WindowClose);


	//-------------------------------------------------------------------------------

	/** Builds the window and sets up stuff
	 *
	 */
	void cGuiPopUpColorPicker::Init()
	{
		/////////////////////////////////
		// Init widgets

		// Main window
		mpWindow->SetText(_W("Color Picker"));
		mpWindow->SetStatic(false);
				
		// Buttons
		for(int i=0; i<3; ++i)
		{
			mvButtons[i] = mpSet->CreateWidgetButton(	0,
														30,
														_W(""), 
														mpWindow);

			mvButtons[i]->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(Button_Pressed));
		}

		mvButtons[0]->SetEnabled(false);
		mvButtons[0]->SetVisible(false);
		mvButtons[0]->SetPosition(cVector3f(60,200,0.1f));
		mvButtons[0]->SetSize(23);
		
		mvButtons[1]->SetText(_W("Ok"));
		mvButtons[1]->SetPosition(cVector3f(60,220, 0.5f));
		mvButtons[1]->SetSize(cVector2f(70,20));
		
		mvButtons[2]->SetText(_W("Cancel"));
		mvButtons[2]->SetPosition(cVector3f(150,220, 0.5f));
		mvButtons[2]->SetSize(cVector2f(70,20));

		tWString vLabelStrings[4] = { _W("R"),_W("G"),_W("B"), _W("A") };

		cVector3f vPos = cVector3f(95,50,0.2f);
		for(int i=0;i<4;++i)
		{
			mvLabelColorComponent[i] = mpSet->CreateWidgetLabel(vPos+cVector3f(5,5,0) + cVector3f(0,30.0f*i,0),0,vLabelStrings[i],mpWindow);

			mvSliderColorComponent[i] = mpSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal,vPos + cVector3f(30,30.0f*i,0),cVector2f(100,25),1000, mpWindow);
			mvSliderColorComponent[i]->SetValue((int)(mColor.v[i]*1000));
			mvSliderColorComponent[i]->AddCallback(eGuiMessage_SliderMove, this, kGuiCallback(Slider_OnMove));

			mvInputColorComponent[i] = mpSet->CreateWidgetTextBox(vPos + cVector3f(150,30.0f*i,0),cVector2f(80,25),_W(""), mpWindow,eWidgetTextBoxInputType_Numeric, 0.1f);
			mvInputColorComponent[i]->SetLowerBound(true,0);
			mvInputColorComponent[i]->SetUpperBound(true,1);
			mvInputColorComponent[i]->SetDecimals(3);
			mvInputColorComponent[i]->SetNumericValue(mColor.v[i]);
			mvInputColorComponent[i]->AddCallback(eGuiMessage_TextBoxEnter,this,kGuiCallback(Input_OnTextBoxEnter));
		}
		
		mOldColor = mColor;

		mpFrameCurrentColor = mpSet->CreateWidgetFrame(cVector3f(10,40,0.5f),50,true,mpWindow);
		mpFrameCurrentColor->SetBackGroundColor(cColor(mColor.r,mColor.g,mColor.b,1));
		mpFrameCurrentColor->SetDrawBackground(true);
		mpFrameCurrentColor->SetBackgroundZ(0);
		

		mpFramePreviousColor = mpSet->CreateWidgetFrame(cVector3f(35,65,0.1f),50,true,mpWindow);
		mpFramePreviousColor->SetBackGroundColor(cColor(mOldColor.r,mOldColor.g,mOldColor.b,1));
		mpFramePreviousColor->SetDrawBackground(true);
		mpFramePreviousColor->SetBackgroundZ(0);
	}

	//-------------------------------------------------------------------------------

	

	//-------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------

	void cGuiPopUpColorPicker::ClosePopUp()
	{
		if((mColor == mOldColor)==false)
		{
			if(mpDestColor)
				*mpDestColor = mColor;

			RunCallback(mpCallbackObject, mpCallback, NULL, cGuiMessageData(), true);
		}

		SelfDestruct();
	}
	//-------------------------------------------------------------------------------

	void cGuiPopUpColorPicker::UpdateSliders()
	{
		for(int i=0;i<4;++i)
		{
			mvSliderColorComponent[i]->SetValue((int)(mColor.v[i]*1000),false);
		}
	}

	//-------------------------------------------------------------------------------

	void cGuiPopUpColorPicker::UpdateInputs()
	{
		for(int i=0;i<4;++i)
			mvInputColorComponent[i]->SetNumericValue(mColor.v[i]);
	}

	//-------------------------------------------------------------------------------

};




